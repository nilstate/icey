# Symple

> Real-time messaging, peer presence, and WebRTC call signalling over native WebSocket.

**[API Reference →](../api/symple.md)** · **[Source →](../../src/symple/)**

## Overview

Symple is a lightweight protocol and C++ implementation for peer discovery, room-based message routing, and real-time communication. It runs over plain WebSocket (RFC 6455); there is no Socket.IO dependency, no Engine.IO framing, and no polling fallback. Every connection is a native WebSocket from the first byte.

We use Symple in two roles: as the signalling layer for WebRTC calls, and as a general-purpose messaging bus for any application that needs peer presence and real-time delivery. The C++ implementation is protocol-compatible with the JavaScript server (`symple-server`) and browser client (`symple-client`), so a native client or server can interoperate with browser peers transparently.

**Headers:** `icy/symple/`
**Namespace:** `icy::smpl`
**CMake target:** `icy_symple`
**License:** LGPL-2.1+

---

## Architecture

A Symple deployment has one server and any number of clients. Clients connect over WebSocket, authenticate with a user identifier and an optional token, and receive a server-assigned session ID. From that point on each peer is reachable by address, participates in room-based presence, and can send and receive any of the four message types.

```text
┌───────────────────────────────────────────────────────┐
│  Application                                          │
│                                                       │
│   smpl::Client                   smpl::Server         │
│   ├── Roster                     ├── ServerPeer[]     │
│   ├── PacketSignal               ├── rooms map        │
│   └── Stateful<ClientState>      └── http::Server     │
│         │                               │             │
│         └─── http::ClientConnection ────┘             │
│              (WebSocket over HTTP upgrade)             │
└───────────────────────────────────────────────────────┘
```

`smpl::Client` depends on `icy/http/client.h` and `icy/http/websocket.h` for its transport. The HTTP module handles the TCP connection, optional TLS, and the WebSocket upgrade handshake. Once the handshake completes, Symple sends the `auth` message as the first application frame.

`smpl::Server` wraps an `http::Server`, intercepts WebSocket upgrade requests, and manages all protocol state. Non-WebSocket HTTP requests can be delegated to a custom `http::ServerConnectionFactory` passed at startup, so the same port can serve static files or a REST API alongside the Symple endpoint.

The event loop is a single-threaded libuv loop. There are no locks in the message-handling hot path on either side. `Server` uses a mutex only around its peer and room maps, which are modified from connection callbacks.

### Protocol flow

```text
Client                              Server
  |                                    |
  |--- WebSocket connect ------------->|
  |<-- {"type":"ready"} --------------|  WS handshake complete
  |--- {"type":"auth", ...} --------->|  first application frame
  |<-- {"type":"welcome", ...} -------|  assigned peer + rooms
  |  or                                |
  |<-- {"type":"error", ...} ---------|  auth failed; connection closed
  |                                    |
  |<--> presence broadcasts ---------->|  peer online/offline
  |<--> messages, commands, events --->|  routed by server
  |                                    |
  |--- {"type":"close"} ------------->|  graceful disconnect
  |--- WebSocket close --------------->|
```

### Key concepts

| Term | Description |
| --- | --- |
| **Address** | `user\|id` string that uniquely identifies a peer session |
| **Peer** | JSON object carrying identity and arbitrary application data |
| **Roster** | Client-side registry of currently online peers, indexed by session ID |
| **Room** | Named group; the permission boundary for messaging and presence |
| **Message types** | `Message`, `Command`, `Event`, `Presence`; all share a common base |

---

## Usage

### Connecting (Client)

Configure a `smpl::Client::Options` struct and call `connect()`. All signal connections should be made before `connect()` is called.

```cpp
#include "icy/symple/client.h"

smpl::Client::Options opts;
opts.host = "127.0.0.1";
opts.port = 4500;
opts.user = "alice";            // required; used for addressing and room assignment
opts.name = "Alice";            // display name
opts.token = "auth-token";      // omit for anonymous mode
opts.secure = false;            // set true for wss://
opts.reconnection = true;       // auto-reconnect on disconnect
opts.reconnectDelay = 3000;     // milliseconds between attempts
opts.reconnectAttempts = 0;     // 0 = unlimited

smpl::Client client(opts);

// Wire up signals before connecting
client.StateChange += [](void*, smpl::ClientState& state, const smpl::ClientState&) {
    switch (state.id()) {
        case smpl::ClientState::Online:
            std::cout << "Online\n";
            break;
        case smpl::ClientState::Error:
            std::cout << "Disconnected\n";
            break;
    }
};

client.Announce += [](const int& status) {
    if (status != 200)
        throw std::runtime_error("Auth failed: " + std::to_string(status));
};

client.connect();

// Drive the event loop
uv::runLoop();
```

The client state machine progresses through five states:

| State | Description |
| --- | --- |
| `Closed` | Not connected |
| `Connecting` | TCP and WebSocket handshake in progress |
| `Authenticating` | `auth` sent, waiting for `welcome` |
| `Online` | Authenticated; can send and receive messages |
| `Error` | Connection failed; reconnect timer may be pending |

`Announce` fires with HTTP status 200 on success or 401/403 on auth failure. It fires before `StateChange` transitions to `Online`, so it is the right place to handle auth errors.

For SSL connections, initialise the SSL context before connecting:

```cpp
#include "icy/net/sslmanager.h"

icy::net::SSLManager::initNoVerifyClient(); // or initClient() with a CA bundle
opts.secure = true;
```

### Addressing

`smpl::Address` encodes a peer endpoint as `user|id`. The `user` field comes from the auth message; the `id` field is the session ID assigned by the server after authentication.

```cpp
#include "icy/symple/address.h"

// Parse from wire format
smpl::Address addr("alice|a1b2c3d4");
std::cout << addr.user; // "alice"
std::cout << addr.id;   // "a1b2c3d4"

// Construct from parts
smpl::Address addr2("alice", "a1b2c3d4");

// Serialise back to wire format
std::string s = addr.toString(); // "alice|a1b2c3d4"

// Validity: at least one of user or id must be non-empty
assert(addr.valid());
```

When the server routes a message, it interprets the `to` field as follows:

| `to` value | Routing |
| --- | --- |
| `"user\|id"` | Exact session; one delivery |
| `"user"` | Any active session for that user |
| omitted | Broadcast to all rooms the sender has joined (excluding sender) |
| `["room1", "room2"]` | Broadcast to specific rooms |

A direct message (`to: "user|id"` or `to: "user"`) is silently dropped if the sender and recipient share no room.

### Peers and Presence

#### Peer

`smpl::Peer` extends `json::Value` with typed accessors for the standard fields. It can carry arbitrary application data by setting additional JSON keys directly.

```cpp
#include "icy/symple/peer.h"

smpl::Peer peer;
peer.setID("a1b2c3d4");
peer.setUser("alice");
peer.setName("Alice");
peer.setType("Peer");

// Address from peer fields
smpl::Address addr = peer.address(); // "alice|a1b2c3d4"

// Arbitrary application data
peer["status"] = "available";
peer["capabilities"] = json::array({"video", "audio"});
peer["version"] = "1.0.0";

// Validity check: requires id, user, and type
assert(peer.valid());
```

The `CreatePresence` signal on `smpl::Client` is the right place to populate custom peer data before a presence broadcast. Anything set there goes into the `data` field of the outgoing presence message and is visible to all peers in shared rooms:

```cpp
client.CreatePresence += [](smpl::Peer& peer) {
    peer["agent"] = "MyApp";
    peer["version"] = "1.0.0";
    peer["status"] = "available";
};
```

#### Roster

`smpl::Roster` is a `LiveCollection<std::string, Peer>` indexed by session ID. The client maintains it automatically: peers enter on `online: true` presence and leave on `online: false` (including server-generated disconnect broadcasts).

```cpp
smpl::Roster& roster = client.roster();

// Look up by session ID
smpl::Peer* peer = roster.get("a1b2c3d4");

// Look up by host address
smpl::Peer* peer = roster.getByHost("192.168.1.100");

// Snapshot copy for safe iteration
auto peers = roster.peers();
for (auto& [id, peer] : peers) {
    std::cout << peer->address() << " - " << peer->name() << '\n';
}

// Debug dump
roster.print(std::cout);
```

`PeerConnected` and `PeerDisconnected` signals on `smpl::Client` fire on the event loop thread immediately after the roster is updated:

```cpp
client.PeerConnected += [](smpl::Peer& peer) {
    std::cout << peer.address() << " came online\n";
};

client.PeerDisconnected += [](smpl::Peer& peer) {
    std::cout << peer.address() << " went offline\n";
};
```

### Sending Messages

All four message types are sent through `client.send()`. The `from` field is set automatically from the client's session. `client.respond()` swaps `to` and `from` and sends, which is the correct way to reply to an incoming message.

```cpp
// General message
smpl::Message msg;
msg.setTo("bob|x9y8z7w6");        // or just "bob" to reach any session
msg["subtype"] = "chat";
msg.setData("text", "Hello from Alice");
client.send(msg);

// Reply: swaps to/from and sends
client.respond(msg);

// Broadcast presence to all joined rooms
client.sendPresence();

// Directed presence probe: recipient should reply with their presence
smpl::Address target("bob", "x9y8z7w6");
client.sendPresence(target, /*probe=*/true);
```

For delivery confirmation, use a transaction. The `StateChange` signal on the transaction fires with `TransactionState::Success` or `TransactionState::Failed`:

```cpp
auto transaction = client.createTransaction(msg);
transaction->StateChange += slot(this, &MyHandler::onAckState);
transaction->send();
```

### Commands

`smpl::Command` adds a `node` path and an `action` verb to the base `Message`. The node is a colon-delimited path; individual segments are accessible by 1-based index. Wildcard matching is supported via `matches()`.

```cpp
#include "icy/symple/command.h"

smpl::Command cmd;
cmd.setTo("bob|x9y8z7w6");
cmd.setNode("media:video:quality");
cmd.setAction("set");
cmd.setData("level", "high");
client.send(cmd);

// Receiving: route by node path
void onCommand(smpl::Command& cmd) {
    if (cmd.matches("media:video:*")) {
        std::string component = cmd.param(2); // "video"
        std::string leaf = cmd.param(3);      // "quality"
        handleVideoCommand(cmd, leaf);
    }
    else if (cmd.matches("media:audio:*")) {
        handleAudioCommand(cmd);
    }
    else if (cmd.isRequest()) {
        // Unrecognised command: reply with 404
        cmd.setStatus(404);
        client.respond(cmd);
    }
}
```

Valid action strings: `"execute"` (default), `"get"`, `"set"`, `"result"`, `"error"`.

### Events

`smpl::Event` carries a named occurrence with a Unix timestamp set automatically on construction. Use events for fire-and-forget notifications where the request/response pattern of `Command` is not needed.

```cpp
#include "icy/symple/event.h"

smpl::Event ev;
ev.setTo("bob|x9y8z7w6");
ev.setName("typing");
// ev.setTime(t) to override; defaults to now
client.send(ev);

// Receiving
void onEvent(smpl::Event& ev) {
    std::cout << ev.name() << " at " << ev.time() << '\n';
    if (ev.name() == "typing") {
        showTypingIndicator(ev.from().user);
    }
}
```

### Server Setup

Start a server with `smpl::Server::start()`. The server runs on the same libuv event loop as the client; call `uv::runLoop()` to drive it.

```cpp
#include "icy/symple/server.h"

smpl::Server server;

server.start({
    .host = "0.0.0.0",
    .port = 4500,
    .authentication = true,      // require token in auth message
    .dynamicRooms = false,        // lock rooms to server-assigned only
    .maxConnections = 1000,       // 0 = unlimited
    .maxMessageSize = 65536,      // bytes; 64 KB default
    .rateLimit = 100.0,           // messages per window
    .rateSeconds = 10.0,          // window duration
});

uv::runLoop();
```

To serve HTTP (static files, REST API) on the same port, pass a custom factory:

```cpp
server.start(opts, std::make_unique<MyHttpFactory>());

// Or access the underlying http::Server directly after start
server.httpServer().addRoute("/health", myHealthHandler);
```

#### Server-side routing and broadcast

```cpp
// Broadcast to all peers in a room (excluding one peer by session ID)
server.broadcast("team-a", msg, excludeId);

// Send to a specific session
server.sendTo("a1b2c3d4", msg);

// Send to any session for a given user
server.sendToUser("alice", msg);

// Query peers
smpl::ServerPeer* peer = server.getPeer("a1b2c3d4");
std::vector<smpl::ServerPeer*> team = server.getPeersInRoom("team-a");
size_t n = server.peerCount();
```

#### Peer lifecycle signals

```cpp
server.PeerConnected += [](smpl::ServerPeer& peer) {
    std::cout << peer.id() << " joined\n";
    for (auto& room : peer.rooms())
        std::cout << "  room: " << room << '\n';
};

server.PeerDisconnected += [](smpl::ServerPeer& peer) {
    std::cout << peer.id() << " left\n";
};
```

### Rooms

Rooms are named groups of peers and the permission boundary for all messaging and presence. A peer can only exchange messages with peers that share at least one room.

Every peer is automatically joined to a room named after their `user` field at authentication. Additional rooms come from three sources, in order of trust:

1. The `Authenticate` hook (recommended for production).
2. The `rooms` array in the client's `auth` message (only honoured in anonymous mode or when no hook is connected).
3. Explicit `join`/`leave` messages from the client (only when `dynamicRooms = true`).

Clients can join and leave rooms at runtime (when `dynamicRooms` is enabled):

```cpp
client.joinRoom("team-a");
client.leaveRoom("team-a");
```

Direct messages between peers that share no room are silently dropped by the server's router.

### Authentication

Two modes:

**Anonymous** (default, `authentication = false`): any client that provides a non-empty `user` field is accepted. No token required.

**Token-based** (`authentication = true`): the client must include a `token` field in the `auth` message. The server validates it via the `Authenticate` signal. Failed auth results in an `error` message (`status: 401`) and the connection is closed.

```cpp
server.Authenticate += [&db](smpl::ServerPeer& peer,
                              const json::Value& auth,
                              bool& allowed,
                              std::vector<std::string>& rooms) {
    auto user = db.findByToken(auth.value("token", ""));
    if (!user) {
        allowed = false;   // connection closed with 401
        return;
    }
    allowed = true;
    rooms = user->teams;   // e.g. {"team-a", "design"}
    // peer.peer() is writable here for server-side field overrides
};
```

If no handler is connected to `Authenticate`, all connections with a valid `user` field are accepted and joined to their personal room only.

### Forms

`smpl::Form`, `smpl::FormElement`, and `smpl::FormField` provide structured data exchange attached to `Command` messages. A form has pages, pages have sections, and sections have fields. This is useful for configuration UIs, settings panels, or any structured request/response exchange.

Form actions govern the exchange direction:

| Action | Direction | Meaning |
| --- | --- | --- |
| `"form"` | server -> client | Requesting the client to fill in the form |
| `"submit"` | client -> server | Submitting completed field values |
| `"result"` | server -> client | Returning data to the client |
| `"cancel"` | client -> server | User cancelled |

```cpp
#include "icy/symple/form.h"

smpl::Command cmd;
cmd.setTo("bob|x9y8z7w6");
cmd.setNode("settings:profile");
cmd.setAction("get");

// Attach a form to the command
smpl::Form form(cmd); // binds to cmd["form"]
form.setAction("form");

auto page = form.addPage("main", "Profile Settings");
auto section = page.addSection("basic", "Basic Info");

auto nameField = section.addField("text", "name", "Display Name");
nameField.setValue("Alice");
nameField.setHint("Your name as shown to other peers");

auto roleField = section.addField("list", "role", "Role");
roleField.addOption("admin", "Administrator");
roleField.addOption("member", "Member");
roleField.setValue("member");

auto activeField = section.addField("boolean", "active", "Active");
activeField.setValue(true);

client.send(cmd);
```

Reading a received form:

```cpp
void onCommand(smpl::Command& cmd) {
    smpl::Form form(cmd);
    if (!form.valid())
        return;

    smpl::FormField nameField;
    if (form.getField("name", nameField))
        std::cout << "name: " << nameField.value() << '\n';

    smpl::FormField activeField;
    if (form.getField("active", activeField))
        std::cout << "active: " << activeField.boolValue() << '\n';
}
```

The `live` flag on a field or section enables partial submissions for auto-complete scenarios. With `form.setPartial(true)`, only the changed fields need to be transmitted rather than the entire form payload.

Supported field types: `"text"`, `"text-multi"`, `"list"`, `"list-multi"`, `"boolean"`, `"media"`, `"custom"`.

### WebRTC Signalling

Symple is the signalling transport for the `webrtc` module. `wrtc::SympleSignaller` (defined in `icy/webrtc/symplesignaller.h`) implements `wrtc::SignallingInterface` by encoding SDP, ICE candidates, and call control messages as Symple `Message` packets with a `subtype` field following the Symple call protocol.

The call protocol uses these subtypes:

| Subtype | Direction | Meaning |
| --- | --- | --- |
| `call:init` | caller -> callee | Initiate a call |
| `call:accept` | callee -> caller | Accept; callee is ready to receive the offer |
| `call:reject` | callee -> caller | Decline the call |
| `call:offer` | caller -> callee | SDP offer |
| `call:answer` | callee -> caller | SDP answer |
| `call:candidate` | either | ICE candidate |
| `call:hangup` | either | End the call |

`SympleSignaller` subscribes to the client's `PacketSignal` in its constructor and unsubscribes in its destructor. The `smpl::Client` instance must outlive the `SympleSignaller`.

```cpp
#include "icy/symple/client.h"
#include "icy/webrtc/symplesignaller.h"
#include "icy/webrtc/peersession.h"

smpl::Client::Options opts;
opts.host = "localhost";
opts.port = 4500;
opts.user = "streamer";

smpl::Client symple(opts);
symple.connect();

// SympleSignaller subscribes to symple's PacketSignal internally
wrtc::SympleSignaller signaller(symple);

wrtc::PeerSession::Config rtcConfig;
rtcConfig.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
wrtc::PeerSession session(signaller, rtcConfig);

// Incoming call flow
signaller.ControlReceived += [&](const std::string& peerId,
                                  const std::string& type,
                                  const std::string& reason) {
    if (type == "init") {
        // Callee accepts and waits for the offer
        signaller.sendControl(peerId, "accept");
    }
    else if (type == "hangup") {
        session.close();
    }
};

// Outgoing call flow
// 1. Send init to remote peer
signaller.sendControl(remotePeerId, "init");

// 2. On receiving "accept" from ControlReceived, create the offer
session.createOffer(remotePeerId);

// Session active: wire up the media pipeline
session.StateChanged += [&](wrtc::PeerSession::State state) {
    if (state == wrtc::PeerSession::State::Active)
        startMediaPipeline(session);
};

uv::runLoop();
```

SDP and ICE candidates are exchanged automatically by `SympleSignaller`; your application code only handles control events (`init`, `accept`, `reject`, `hangup`). See the [webrtc module guide](webrtc.md) for `PeerSession`, `SignallingInterface`, and the full media pipeline.

---

## Samples

### [sympleconsole](../../src/symple/samples/sympleconsole/)

An interactive console client that demonstrates connection, presence, room management, and message sending. The key pattern it illustrates is bridging blocking console I/O (`std::getchar`) to the libuv event loop using `ipc::SyncQueue`. All Symple operations must happen on the event loop thread; the IPC queue lets the console thread push work to it safely.

```cpp
#include "icy/application.h"
#include "icy/ipc.h"
#include "icy/symple/client.h"

// Wire up signals before connecting
client += packetSlot(this, &SympleApplication::onRecvMessage);
client += packetSlot(this, &SympleApplication::onRecvPresence);
client += packetSlot(this, &SympleApplication::onRecvEvent);
client.Announce += slot(this, &SympleApplication::onClientAnnounce);
client.StateChange += slot(this, &SympleApplication::onClientStateChange);
client.CreatePresence += slot(this, &SympleApplication::onCreatePresence);
client.connect();

// Console thread: push work to the event loop thread via IPC queue
app->ipc.push(new icy::ipc::Action(
    [app](const icy::ipc::Action& a) { app->onSyncMessage(a); },
    message));

// Event loop thread: send with transaction for delivery confirmation
void onSyncMessage(const icy::ipc::Action& action)
{
    auto message = static_cast<icy::smpl::Message*>(action.arg);
    auto transaction = client.createTransaction(*message);
    transaction->StateChange += slot(this, &SympleApplication::onAckState);
    transaction->send();
    delete message;
}

// Populate custom presence data
void onCreatePresence(icy::smpl::Peer& peer)
{
    peer["agent"] = "SympleConsole";
    peer["version"] = "0.1.0";
}

// On Online state, join the public room
void onClientStateChange(void*, icy::smpl::ClientState& state, const icy::smpl::ClientState&)
{
    if (state.id() == icy::smpl::ClientState::Online)
        client.joinRoom("public");
}
```

Run with:

```bash
sympleconsole -host localhost -port 4500 -user alice -name Alice -token <token>
```

### [symplestreamer](../../src/symple/samples/symplestreamer/)

Demonstrates using Symple purely for signalling while actual media travels over a separate WebSocket connection. The streamer connects to Symple for peer discovery, listens for `call:init` messages, starts an MJPEG WebSocket server, and returns the stream URL to the caller in `call:accept`. The media itself never touches the Symple connection.

```cpp
#include "icy/symple/client.h"
#include "icy/av/mediacapture.h"
#include "icy/av/multiplexpacketencoder.h"
#include "icy/http/server.h"
#include "icy/packetstream.h"

// Handle incoming call signalling
void onRecvMessage(smpl::Message& msg)
{
    std::string subtype;
    if (msg.find("subtype") != msg.end())
        subtype = msg["subtype"].get<std::string>();

    if (subtype == "call:init") {
        smpl::Message accept;
        accept["subtype"] = "call:accept";
        accept.setTo(msg.from());
        // Tell the player where to connect for the media stream
        accept.setData("url", "ws://localhost:" + std::to_string(StreamPort));
        accept.setData("engine", "MJPEGWebSocket");
        client.send(accept);
    }
    else if (subtype == "call:hangup") {
        streaming = false;
    }
}

// Join the public room once online so the player can discover us
void onStateChange(void*, smpl::ClientState& state, const smpl::ClientState&)
{
    if (state.id() == smpl::ClientState::Online)
        client.joinRoom("public");
}

// Announce ourselves as a streamer in presence broadcasts
void onCreatePresence(smpl::Peer& peer)
{
    peer["agent"] = "Icey";
    peer["type"] = "streamer";
}
```

The `StreamResponder` class (one per connected player) attaches to the shared `av::MediaCapture` source and drives its own `PacketStream` encoding pipeline:

```cpp
StreamResponder(http::ServerConnection& conn)
    : http::ServerResponder(conn)
    , _stream("symplestream")
{
    _stream.attachSource(gCapture.get(), false, true);

    av::EncoderOptions options;
    options.oformat = av::Format("MJPEG", "mjpeg",
        av::VideoCodec("mjpeg", 400, 300, 25));
    gCapture->getEncoderFormat(options.iformat);

    auto encoder = new av::MultiplexPacketEncoder(options);
    encoder->init();
    _stream.attach(encoder, 5, true);
    _stream.emitter += packetSlot(this, &StreamResponder::onPacket);
    _stream.start();
}

void onPacket(RawPacket& packet)
{
    connection().send(packet.data(), packet.size(), http::ws::Binary);
}
```

Run with:

```bash
symplestreamer -host localhost -port 4500 -user streamer -name Streamer -token <token>
```

---

## Configuration

### Client options

| Option | Default | Description |
| --- | --- | --- |
| `host` | `"127.0.0.1"` | Server hostname or IP |
| `port` | `4500` | Server port |
| `secure` | `false` | Use `wss://` instead of `ws://` |
| `user` | `""` | User identifier (required) |
| `name` | `""` | Display name |
| `type` | `""` | Peer type string |
| `token` | `""` | Auth token (required when server has authentication enabled) |
| `reconnection` | `true` | Auto-reconnect on disconnect |
| `reconnectDelay` | `3000` | Milliseconds between reconnect attempts |
| `reconnectAttempts` | `0` | Max reconnect attempts; 0 = unlimited |

### Server options

| Option | Default | Description |
| --- | --- | --- |
| `host` | `"0.0.0.0"` | Bind address |
| `port` | `4500` | Listen port |
| `authentication` | `false` | Require token in auth message |
| `dynamicRooms` | `true` | Allow clients to join/leave rooms at runtime |
| `maxConnections` | `0` | Max simultaneous WebSocket connections; 0 = unlimited |
| `maxMessageSize` | `65536` | Max message payload in bytes |
| `rateLimit` | `100.0` | Max messages per rate window (per peer) |
| `rateSeconds` | `10.0` | Rate window duration in seconds |

Rate limiting uses a token bucket per peer. Messages from a peer that exceed their allowance are dropped before routing. The defaults (100 messages / 10 seconds) work for most applications; reduce them for public-facing servers.

### Virtual peers

The server can register virtual peers: these appear in presence broadcasts and are routable like any WebSocket-connected peer, but messages are delivered via a callback. Useful for bots or server-side agents.

```cpp
smpl::Peer botPeer;
botPeer.setID("bot-001");
botPeer.setUser("bot");
botPeer.setName("Assistant Bot");
botPeer["online"] = true;

server.addVirtualPeer(botPeer, {"team-a"}, [](const json::Value& msg) {
    std::cout << "Bot received: " << msg.dump() << '\n';
});

server.removeVirtualPeer("bot-001");
```

### Building

Enable the module (on by default) and build the test suite:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --target sympletests
./build/symple/tests/sympletests
```

The test suite covers 18 scenarios: address parsing, peer construction, message format, connect/authenticate, two-peer presence, message routing, auth failure, disconnect presence, max connections, max message size, rate limiting, and graceful shutdown.

Link the module in your own CMake project:

```cmake
target_link_libraries(myapp PRIVATE icy_symple)
```

## See Also

- [WebRTC](webrtc.md) — `PeerSession`, `SympleSignaller`, and media pipeline
- [HTTP](http.md) — WebSocket transport, `http::Server`, and `http::ClientConnection`
- [Protocol spec](../../src/symple/PROTOCOL.md) — full wire protocol specification
