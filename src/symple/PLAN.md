# Symple v4 Implementation Plan

> Drop Socket.IO. Plain WebSocket everywhere. Hard cutover.

## What Changes

| Component | Current (v3) | Target (v4) |
|-----------|-------------|-------------|
| Wire transport | Socket.IO (Engine.IO + WS) | Native WebSocket |
| Auth handshake | Socket.IO middleware | First JSON message |
| Heartbeat | Engine.IO ping/pong bytes | WebSocket ping/pong frames |
| C++ client | depends on socketio module | depends on http module (WebSocket) |
| JS client | depends on socket.io-client | depends on nothing (native WebSocket) |
| JS server | depends on socket.io | depends on ws (or native in Node 22+) |
| C++ server | does not exist | new: embedded in symple module |
| socketio module | active | deleted |

## Phase 1: C++ Symple Server

New code in `src/symple/`. Uses libsourcey's HTTP/WebSocket server.

### Files

```
src/symple/include/scy/symple/
  server.h          - Symple server (WebSocket, rooms, routing, auth)
  serverconnection.h - per-client connection state

src/symple/src/
  server.cpp
  serverconnection.cpp
```

### Server Architecture

```cpp
class Server {
    // HTTP/WebSocket server
    http::Server _http;

    // Connected peers indexed by session ID
    std::unordered_map<std::string, ServerConnection*> _peers;

    // Rooms: room name -> set of session IDs
    std::unordered_map<std::string, std::unordered_set<std::string>> _rooms;

    // Config
    struct Options {
        std::string host = "0.0.0.0";
        uint16_t port = 4500;
        bool authentication = false;
        bool dynamicRooms = true;
        int sessionTTL = -1;
        // TLS via http::Server options
    };

    void start(const Options& opts);
    void shutdown();

    // Override for custom auth
    Signal<void(ServerConnection&, const json::Value& auth, bool& allowed)> Authenticate;

    // Hooks
    Signal<void(ServerConnection&)> PeerConnected;
    Signal<void(ServerConnection&)> PeerDisconnected;
};
```

```cpp
class ServerConnection {
    http::WebSocket& _ws;
    Peer _peer;
    std::unordered_set<std::string> _rooms;
    bool _authenticated = false;

    void send(const json::Value& msg);
    void join(const std::string& room);
    void leave(const std::string& room);
};
```

### Message Routing

Same logic as the Node server:

1. Parse `to` field from message
2. If `to` is omitted: broadcast to all rooms the sender joined (excluding sender)
3. If `to` is `"user|id"`: find peer by session ID, send directly
4. If `to` is `"user"`: find any peer with that user, send to their room
5. If `to` is an array: send to each listed room

### Auth Flow

1. Client connects via WebSocket
2. Server starts 10-second auth timeout
3. Client sends `{"type": "auth", "user": "alice", ...}`
4. Server fires `Authenticate` signal (custom validation)
5. If allowed: create Peer, join user room, send `welcome`, broadcast presence
6. If denied: send `error`, close connection

## Phase 2: C++ Symple Client Rewrite

Replace socketio dependency with plain WebSocket.

### Changes to existing files

```
src/symple/include/scy/symple/client.h   - rewrite
src/symple/src/client.cpp                - rewrite
```

### Key Changes

- `smpl::Client` uses `http::ClientConnection` (WebSocket) instead of `sockio::Client`
- Auth sent as first JSON message instead of Socket.IO handshake middleware
- Reconnection logic moves from socketio to symple client directly
- State machine: Closed -> Connecting -> Authenticating -> Online -> Error
- `send()` calls `ws.send(json.dump())` instead of `socket.emit("message", json)`
- Join/leave send JSON messages instead of Socket.IO events

### API Compatibility

Public API stays the same:
- `connect()`, `disconnect()`, `send()`, `joinRoom()`, `leaveRoom()`
- `sendPresence()`, `ourID()`, `roster()`
- `Announce`, `StateChange`, `CreatePresence`, `PeerConnected`, `PeerDisconnected` signals

Internal transport changes, external API unchanged. Existing C++ code that uses `smpl::Client` recompiles without changes.

### Remove

- `smpl::TCPClient` / `smpl::SSLClient` distinction (WebSocket handles TLS via URL scheme)
- socketio dependency from CMakeLists.txt

## Phase 3: JS symple-client Rewrite

Replace socket.io-client with native WebSocket.

### Changes

```
symple/symple-client/src/client.js    - rewrite transport layer
symple/symple-client/package.json     - remove socket.io-client dep
```

### Key Changes

- `io.connect(url)` -> `new WebSocket(url)`
- `socket.emit('message', m)` -> `ws.send(JSON.stringify(m))`
- `socket.on('message', cb)` -> `ws.onmessage = (e) => cb(JSON.parse(e.data))`
- `socket.on('connect', cb)` -> `ws.onopen` + send auth message
- `socket.on('disconnect', cb)` -> `ws.onclose`
- Auth: send `{type: "auth", ...}` as first message, wait for `{type: "welcome"}`
- Reconnection: implement in client (was handled by socket.io-client)
- Room join/leave: send JSON messages instead of Socket.IO events

### API Compatibility

Public API stays the same:
- `connect()`, `disconnect()`, `send()`, `join()`, `leave()`
- `sendPresence()`, `ourID`, `roster`
- Event emitter: `on('connect')`, `on('message')`, `on('presence')`, etc.

symple-client-player (CallManager, WebRTCPlayer, Player) is unaffected; it talks to symple-client, not to the transport.

## Phase 4: JS symple-server Rewrite

Replace socket.io with ws package (or Node 22+ native WebSocket).

### Changes

```
symple/symple-server/lib/symple.js    - rewrite
symple/symple-server/lib/peer.js      - unchanged
symple/symple-server/package.json     - replace socket.io with ws
symple/symple-server/config.js        - remove CORS config (not needed without Socket.IO)
```

### Key Changes

- `new Server(httpServer)` -> `new WebSocket.Server({server: httpServer})`
- Auth: parse first message instead of middleware
- Rooms: managed manually (Map of Sets) instead of Socket.IO rooms
- Broadcasting: iterate room members and send
- Redis adapter: publish/subscribe directly instead of through socket.io adapter

### Redis Scaling

Same architecture, different transport:
- `PUBLISH symple:room:public <message>` for room broadcasts
- `SUBSCRIBE symple:room:*` for receiving cross-process messages
- Session storage in Redis unchanged

## Phase 5: Kill socketio Module

- Delete `src/socketio/` entirely
- Remove from `src/.gitignore` whitelist
- Remove socketio from any module DEPENDS lists
- Update symple CMakeLists.txt: DEPENDS changes from `base crypto net http socketio json` to `base crypto net http json`

## Phase 6: Media Server App

With the C++ Symple server in place:

```
src/webrtc/apps/media-server/
  main.cpp
  config.h
  config.json
```

Single binary that embeds:
- Symple server (signalling + presence + rooms)
- TURN relay (from turn module)
- HTTP server (serves web UI)
- Media engine (stream/record via webrtc module)

Browser loads the page, connects via WebSocket to the same server, sees peers, calls the media server peer, video flows. One binary, one port for HTTP/WS, one port for TURN.

## Phase 7: Update WebRTC Samples

- SympleSignaller in samples updated for new client API (should be minimal since `smpl::Client` API is unchanged)
- Samples point at C++ Symple server instead of Node.js (or either; both speak v4)

## Dependency Changes

### Before (v3)
```
symple -> socketio -> http -> net -> base
                   -> json
```

### After (v4)
```
symple -> http -> net -> base
       -> json
```

socketio module deleted. One fewer module, one fewer dependency layer.

## Testing Strategy

1. **C++ server unit tests**: auth flow, room join/leave, message routing, presence broadcast, connection timeout
2. **C++ client integration tests**: connect to C++ server, auth, send/receive messages, presence
3. **JS client against C++ server**: browser connects, auth, presence, messaging (manual browser test)
4. **JS server against JS client**: same test against Node server
5. **Cross-server**: JS client talks to C++ server, C++ client talks to JS server (interop)

## Migration Path

Hard cutover. No compatibility layer.

1. Ship C++ server + rewritten C++ client in libsourcey
2. Ship rewritten JS client + JS server in symple repo
3. All new code speaks v4 exclusively
4. v3 code is archived, not maintained
