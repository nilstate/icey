# Symple (C++)

Symple v4 server and client for real-time messaging and presence over native WebSocket.

Part of [icey](https://github.com/nilstate/icey). Implements the same protocol as the [JS server](https://github.com/icey/symple-server) and [JS client](https://github.com/icey/symple-client).

## Features

- Native WebSocket transport (no Socket.IO, no polling)
- Peer presence with online/offline broadcasting
- Room-based message routing with team/group permission scoping
- Per-peer rate limiting and max message size enforcement
- Connection pooling and keep-alive for high throughput
- WebRTC call signalling via SympleSignaller
- Single-threaded libuv transport with explicit peer/room indexing and server-side routing policy

## Server

```cpp
#include "icy/symple/server.h"

smpl::Server server;

// Optional: custom authentication with room assignment
server.Authenticate += [&db](smpl::ServerPeer& peer, const json::Value& auth,
                             bool& allowed, std::vector<std::string>& rooms) {
    auto user = db.findByToken(auth.value("token", ""));
    if (!user) {
        allowed = false;
        return;
    }
    allowed = true;
    rooms = user->teams;  // {"team-a", "design"}
};

server.start({
    .host = "0.0.0.0",
    .port = 4500,
    .authentication = true,
    .dynamicRooms = false,       // lock rooms to server-assigned only
    .maxConnections = 1000,
    .maxMessageSize = 64 * 1024, // 64KB
    .rateLimit = 100.0,          // messages per window
    .rateSeconds = 10.0          // window size
});

// Run the event loop
uv::runLoop();
```

## Client

```cpp
#include "icy/symple/client.h"

smpl::Client::Options opts;
opts.host = "127.0.0.1";
opts.port = 4500;
opts.user = "alice";
opts.name = "Alice";
opts.token = "auth-token";

smpl::Client client(opts);

client.StateChange += [](void*, smpl::ClientState& state, const smpl::ClientState&) {
    if (state.id() == smpl::ClientState::Online)
        std::cout << "Online" << std::endl;
};

client += packetSlot(&handler, &Handler::onMessage);

client.start();
```

## WebRTC Signalling

```cpp
#include "icy/webrtc/support/symplesignaller.h"

smpl::Client symple(opts);
symple.start();

wrtc::SympleSignaller signaller(symple);
wrtc::PeerSession session(signaller, config);

// Incoming call
signaller.ControlReceived += [&](const std::string& peerId,
                                  const std::string& type,
                                  const std::string& reason) {
    if (type == "init")
        session.createAnswer(peerId);
};
```

## Teams and Permissions

Rooms are the permission boundary. A peer can only see presence from and send direct messages to peers that share at least one room.

Rooms are assigned in three ways:

1. **Authenticate hook** (recommended): the server-side auth callback queries a database and returns room memberships
2. **Auth message**: the client includes `"rooms": [...]` in the auth message (only trusted in anonymous mode)
3. **Dynamic rooms**: with `dynamicRooms = true`, clients can join/leave freely (no permission scoping)

Direct messages to a peer that doesn't share any room with the sender are silently dropped.

## Protocol

See [PROTOCOL.md](PROTOCOL.md) for the full wire protocol specification.

## Building

Symple is built as part of icey:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target sympletests
./build/symple/tests/sympletests
```

## Tests

18 tests covering protocol, integration, and production hardening:

- Address parsing, peer construction, message format
- Connect, authenticate, two-peer presence, message routing
- Auth failure, disconnect presence
- Max connections, max message size, rate limiting, graceful shutdown

## License

LGPL-2.1+
