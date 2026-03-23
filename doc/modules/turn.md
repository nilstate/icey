# TURN

> RFC 5766 relay server and client with TCP extensions (RFC 6062) for NAT traversal through symmetric NATs.

**[API Reference →](../api/turn.md)** · **[Source →](../../src/turn/)**

## Overview

TURN (Traversal Using Relays around NAT, RFC 5766) lets a client obtain a public relay address on the server. Peers send data to that relay address; the server forwards it to the client. Roughly 30% of real-world WebRTC connections cannot reach each other directly and require this relay path.

The module provides both sides of the protocol:

- **Server**: listens on UDP and TCP, authenticates requests via a pluggable observer, manages `ServerAllocation` objects keyed by 5-tuple, and handles the full allocation lifecycle including Refresh and CreatePermission.
- **Client**: drives the complete allocation sequence from first Allocate through the 401 challenge, re-authentication, CreatePermission, data relay via Send/Data Indications, periodic Refresh, and clean shutdown.
- **TCP extensions (RFC 6062)**: `TCPAllocation` on the server and `TCPClient` on the client implement the Connect/ConnectionBind/ConnectionAttempt flow for connection-oriented relay.

Every TURN message is a STUN message. Read [stun.md](stun.md) first if you are unfamiliar with `stun::Message`, attribute types, or the HMAC-SHA1 integrity mechanism; this document assumes that knowledge.

Link against `Icey::turn` (which pulls in `Icey::stun` automatically):

```cmake
target_link_libraries(myapp PRIVATE Icey::turn)
```

## Architecture

```text
Server
├── ServerOptions          (bind address, realm, lifetimes, flags)
├── ServerObserver         (authenticateRequest, allocation lifecycle callbacks)
├── net::UDPSocket         (datagram control + data plane)
├── net::TCPSocket         (listener; accepts control connections)
├── ServerAllocationMap    (std::map<FiveTuple, unique_ptr<ServerAllocation>>)
│   ├── UDPAllocation      (one relay UDP socket per allocation)
│   └── TCPAllocation      (one control TCP socket + one peer acceptor per allocation)
│       └── TCPConnectionPairMap
│           └── TCPConnectionPair  (client socket <-> peer socket pipe)
└── Timer                  (periodic expiry sweep)

Client  (Stateful<ClientState> + IAllocation)
├── Client::Options        (server address, credentials, lifetime)
├── ClientObserver         (state changes, data received, lifecycle events)
├── UDPClient              (single UDP socket for control + data)
└── TCPClient              (control TCP socket + one relay TCP socket per peer)
    └── TCPClientObserver  (relay connection created/closed/error callbacks)

Supporting types
├── FiveTuple              (remote + local address + transport; map key)
├── IAllocation            (base: 5-tuple, username, permissions, lifetime, bandwidth)
├── Permission             (peer IP + 300 s expiry timer)
└── Request                (stun::Message + transport + local/remote address + hash)
```

The server is single-threaded and event-driven via libuv. All socket callbacks, timer ticks, and allocation operations run on the same event loop thread. The allocation map is protected by a mutex for the snapshot method `allocations()` only; all other access is single-threaded.

## Usage

### Server setup

Include the server headers and construct `Server` with a `ServerObserver` and `ServerOptions`:

```cpp
#include "icy/turn/server/server.h"
#include "icy/turn/server/serverallocation.h"
#include "icy/turn/server/udpallocation.h"
#include "icy/turn/server/tcpallocation.h"
#include "icy/turn/server/tcpconnectionpair.h"
```

```cpp
turn::ServerOptions opts;
opts.software    = "MyServer/1.0";
opts.realm       = "example.com";
opts.listenAddr  = net::Address("0.0.0.0", 3478);
opts.externalIP  = "203.0.113.1"; // public IP; leave empty if machine has a public IP directly
opts.allocationDefaultLifetime = 2  * 60 * 1000; // 2 minutes (ms)
opts.allocationMaxLifetime     = 15 * 60 * 1000; // 15 minutes (ms)
opts.allocationMaxPermissions  = 10;
opts.timerInterval             = 10 * 1000;       // 10-second sweep (ms)
opts.earlyMediaBufferSize      = 8192;            // TCP early-data buffer (bytes)
opts.enableUDP                 = true;
opts.enableTCP                 = true;
opts.enableLocalIPPermissions  = true;  // auto-grant RFC 1918 / loopback
```

`externalIP` is critical in cloud deployments where the process binds to a private interface (`10.x`, `172.x`, `192.168.x`) but must advertise a public IP in `XOR-RELAYED-ADDRESS`. Leaving it empty causes the server to report its bind address, which is only correct on machines with a direct public IP.

`enableLocalIPPermissions` auto-grants permissions for RFC 1918 and loopback peers. This is useful during development and in WebRTC deployments where ICE connectivity checks probe loopback or LAN addresses before any explicit `CreatePermission` has been sent.

Construct the server with yourself as the observer and call `start()`:

```cpp
class MyServer : public turn::ServerObserver {
public:
    turn::Server server;

    MyServer(const turn::ServerOptions& opts)
        : server(*this, opts) {}

    void start() { server.start(); }
    void stop()  { server.stop();  }

    // ServerObserver interface
    turn::AuthenticationState authenticateRequest(turn::Server*, turn::Request& req) override;
    void onServerAllocationCreated(turn::Server*, turn::IAllocation* alloc) override;
    void onServerAllocationRemoved(turn::Server*, turn::IAllocation* alloc) override;
};
```

`Server::start()` binds the UDP socket, starts the TCP listener, and arms the maintenance timer. `Server::stop()` destroys all allocations, closes all sockets, and disarms the timer.

### Authentication

The server calls `ServerObserver::authenticateRequest()` for every incoming STUN request before dispatching it. The observer returns one of four states:

| State | Wire effect |
| --- | --- |
| `Authorized` | Request proceeds to the method handler |
| `NotAuthorized` | Server sends a 401 error response |
| `QuotaReached` | Server sends a 486 error response |
| `Authenticating` | Server holds the request; call `server.handleRequest(req, state)` when ready |

The `Authenticating` state allows async auth (database lookups, token introspection) without blocking the event loop. Store the `Request` by value and call back from the completion handler.

A synchronous implementation using the RFC 5389 long-term credential mechanism:

```cpp
turn::AuthenticationState MyServer::authenticateRequest(turn::Server*, turn::Request& req)
{
    // Indications cannot carry credentials; pass them through unconditionally.
    // SendIndication and Binding requests bypass auth per RFC 5766 section 10.
    if (req.methodType() == stun::Message::SendIndication ||
        req.methodType() == stun::Message::Binding)
        return turn::AuthenticationState::Authorized;

    // Require all four credential attributes.
    auto* username  = req.get<stun::Username>();
    auto* realm     = req.get<stun::Realm>();
    auto* nonce     = req.get<stun::Nonce>();
    auto* integrity = req.get<stun::MessageIntegrity>();
    if (!username || !realm || !nonce || !integrity)
        return turn::AuthenticationState::NotAuthorized; // triggers 401

    // Derive the HMAC key: MD5(username:realm:password)
    // In production, look up the password (or pre-computed key) by username.
    crypto::Hash md5("md5");
    md5.update(username->asString() + ":" + realm->asString() + ":" + lookupPassword(username->asString()));
    req.hash = md5.digestStr();

    // Verify the HMAC. Setting req.hash is sufficient for the server to sign
    // responses even if you skip verification (e.g. in a trusted network).
    if (integrity->verifyHmac(req.hash))
        return turn::AuthenticationState::Authorized;

    return turn::AuthenticationState::NotAuthorized;
}
```

`req.hash` must be set before returning `Authorized`. `Server::respond()` reads it to sign the response with the same key. If you return `Authorized` without setting `req.hash`, the response goes out without a `MESSAGE-INTEGRITY` attribute.

The first Allocate from a new client will have no credentials; the missing attributes cause a 401. The client re-sends with credentials; `authenticateRequest()` is called again on the authenticated copy.

### Allocation lifecycle callbacks

```cpp
void MyServer::onServerAllocationCreated(turn::Server*, turn::IAllocation* alloc)
{
    // alloc->tuple()           -- FiveTuple (remote, local, transport)
    // alloc->username()        -- authenticated username
    // alloc->relayedAddress()  -- assigned relay address
    // alloc->lifetime()        -- negotiated lifetime in seconds
    log("allocation created for " + alloc->username());
}

void MyServer::onServerAllocationRemoved(turn::Server*, turn::IAllocation* alloc)
{
    log("allocation removed for " + alloc->username());
    // Do not delete alloc; the server owns it.
}
```

Use these callbacks to maintain per-user allocation counts, enforce quotas, update metrics, or clean up application state.

### Binding requests

The server handles `Binding` requests itself via `Server::handleBindingRequest()`, which responds with an `XOR-MAPPED-ADDRESS` containing the client's reflexive address. No allocation is created. This lets you run a combined STUN/TURN server on port 3478 without any additional code.

### Sending error responses

`Server::respondError()` constructs a fully-formed error response with `SOFTWARE`, `REALM`, `NONCE`, and `ERROR-CODE` attributes, signs it if `req.hash` is set, and sends it:

```cpp
server.respondError(req, stun::Message::AllocationMismatch, "Allocation Mismatch");
server.respondError(req, stun::Message::InsufficientCapacity, "Insufficient Capacity");
```

`Server::respond()` sends an already-constructed response message; use it when you need to add custom attributes before sending.

### Server Internals

#### FiveTuple

Every allocation is uniquely identified by its 5-tuple: client remote address, server local address, and transport protocol. `FiveTuple` is used as the `std::map` key and supports equality, ordering, and stream output:

```cpp
#include "icy/turn/fivetuple.h"

turn::FiveTuple tuple(remoteAddr, localAddr, net::UDP);
turn::ServerAllocation* alloc = server.getAllocation(tuple);

std::cout << tuple; // "FiveTuple[203.0.113.42:54321:0.0.0.0:3478:UDP]"
```

#### ServerAllocation

`ServerAllocation` is the base class for both transport types. It inherits from `IAllocation` and adds the server reference, a server-enforced maximum lifetime, and two shared request handlers:

- `handleRefreshRequest()`: updates the allocation lifetime from the `LIFETIME` attribute, or sets the delete flag if the client sends `LIFETIME=0`. Responds with the negotiated lifetime (minimum of requested and maximum).
- `handleCreatePermission()`: installs or refreshes permissions for every `XOR-PEER-ADDRESS` attribute in the request. Up to `allocationMaxPermissions` entries are maintained; excess entries are silently dropped.

`ServerAllocation::onTimer()` is called by `Server::onTimer()` on every timer tick. It removes expired permissions and returns `false` when the allocation has expired or been deleted, signalling the server to destroy it.

`timeRemaining()` returns the minimum of `IAllocation::timeRemaining()` (client-negotiated lifetime) and `maxTimeRemaining()` (server-enforced ceiling).

#### UDPAllocation

`UDPAllocation` binds a dedicated UDP socket for peer communication. The socket's bound address becomes the `XOR-RELAYED-ADDRESS` reported in the Allocate response. Two data paths:

**Peer to client** (`onPeerDataReceived`): datagrams arriving on the relay socket are checked against the permission list. Permitted datagrams are forwarded to the client as `DataIndication` messages on the server's main UDP socket, with `XOR-PEER-ADDRESS` set to the datagram source and `DATA` carrying the payload. Unpermitted datagrams are silently dropped.

**Client to peer** (`handleSendIndication`): `SendIndication` messages from the client carry `XOR-PEER-ADDRESS` and `DATA`. After a permission check, the payload is forwarded to the peer via the relay socket with `UDPAllocation::send()`.

Bandwidth usage is tracked on every relay operation via `IAllocation::updateUsage()`. Allocations with a bandwidth cap set return `-1` from `send()` when the quota is exhausted.

#### TCPAllocation

`TCPAllocation` owns three sockets:

- `_control`: the accepted TCP socket used for STUN signalling (Refresh, CreatePermission, Connect, ConnectionBind).
- `_acceptor`: a separate TCP listener bound to the relay address for incoming peer connections.
- `_pairs`: a `TCPConnectionPairMap` (keyed by `connectionID`) of active `TCPConnectionPair` objects.

When the control socket closes, `onControlClosed()` fires and marks the allocation for deletion on the next timer tick.

#### TCPConnectionPair

Each `TCPConnectionPair` is a bidirectional relay pipe between one client socket and one peer socket:

```text
TURN client --[TCP]--> server client socket <---[pair]---> peer socket --[TCP]--> remote peer
```

Key fields:

| Field | Type | Description |
| --- | --- | --- |
| `connectionID` | `uint32_t` | Random ID linking the Connect response to the ConnectionBind request |
| `client` | `net::SocketEmitter` | The TURN client's data TCP socket (set during ConnectionBind) |
| `peer` | `net::SocketEmitter` | The remote peer's TCP socket |
| `earlyPeerData` | `Buffer` | Peer data buffered before the client socket arrives (up to 8 KiB) |
| `isDataConnection` | `bool` | True once `makeDataConnection()` has activated the relay |
| `timeout` | `Timeout` | 30-second `ConnectionBind` deadline |
| `pendingDelete` | `bool` | Set by `requestDeletion()`; destroyed on the next timer tick |

`makeDataConnection()` flushes `earlyPeerData` to the client socket and arms bidirectional forwarding. Any peer data arriving after `makeDataConnection()` is forwarded directly without buffering.

#### IAllocation

`IAllocation` is the abstract base shared by both server and client allocations. It holds:

- The 5-tuple (`FiveTuple _tuple`)
- The authenticated username (`std::string _username`)
- The permission list (`PermissionList _permissions`)
- Lifetime tracking (`_lifetime`, `_createdAt`, `_updatedAt`)
- Bandwidth tracking (`_bandwidthLimit`, `_bandwidthUsed`)

Key methods:

| Method | Description |
| --- | --- |
| `updateUsage(numBytes)` | Refresh the activity timestamp and accumulate bytes |
| `setLifetime(seconds)` | Update lifetime and reset the activity timestamp |
| `setBandwidthLimit(bytes)` | Set a cap; 0 = unlimited |
| `expired()` | True if lifetime elapsed or bandwidth quota exhausted |
| `deleted()` | True if `expired()` or the delete flag is set |
| `timeRemaining()` | Seconds until expiry |
| `bandwidthRemaining()` | Bytes remaining before quota |
| `addPermission(ip)` | Add or refresh a permission entry |
| `removeExpiredPermissions()` | Prune entries whose 300 s timer has elapsed |
| `hasPermission(ip)` | Check for a valid permission; always true for local IPs if enabled |

#### Permission

`Permission` associates a peer IPv4 address with a 300-second expiry timer (exactly as specified in RFC 5766 section 8):

```cpp
#include "icy/turn/permission.h"

// PERMISSION_LIFETIME = 300,000 ms (5 minutes)
turn::Permission perm("203.0.113.42");
perm.refresh(); // reset the 300 s timer
bool match = (perm == "203.0.113.42"); // true
```

Permissions are per-IP, not per-address+port. A single `CreatePermission` request can install multiple permissions by including multiple `XOR-PEER-ADDRESS` attributes.

### Client Usage

#### UDP client

`UDPClient` uses a single UDP socket for both STUN control messages and Send/Data Indications. Include the client headers:

```cpp
#include "icy/turn/client/client.h"
#include "icy/turn/client/udpclient.h"
```

Implement `ClientObserver`:

```cpp
class MyObserver : public turn::ClientObserver {
public:
    void onClientStateChange(turn::Client& client,
                             turn::ClientState& state,
                             const turn::ClientState& oldState) override
    {
        if (state.id() == turn::ClientState::Success) {
            // Allocation live; relayedAddress() is valid
            log("relay address: " + client.relayedAddress().toString());
        } else if (state.id() == turn::ClientState::Failed) {
            log("allocation failed");
        }
    }

    void onRelayDataReceived(turn::Client& client,
                             const char* data, size_t size,
                             const net::Address& peerAddress) override
    {
        // Data received from peerAddress via the TURN relay
        processData(data, size, peerAddress);
    }

    void onAllocationFailed(turn::Client& client,
                            int errorCode, const std::string& reason) override
    {
        log("error " + std::to_string(errorCode) + ": " + reason);
    }

    void onAllocationDeleted(turn::Client& client,
                             const stun::Transaction& tx) override
    {
        log("allocation deleted");
    }
};
```

Configure and start:

```cpp
turn::Client::Options opts;
opts.serverAddr    = net::Address("turn.example.com", 3478);
opts.username      = "alice";
opts.password      = "s3cr3t";
opts.lifetime      = 5 * 60 * 1000; // 5 minutes (ms)
opts.timeout       = 10 * 1000;     // transaction timeout (ms)
opts.timerInterval = 30 * 1000;     // refresh/check interval (ms)

MyObserver obs;
turn::UDPClient client(obs, opts);

// Register peers before initiating so CreatePermission is sent immediately after Allocate
client.addPermission("203.0.113.42");
client.addPermission("198.51.100.7");

client.initiate(); // connect socket and send first Allocate
```

Once `onClientStateChange` delivers `ClientState::Success`, send data:

```cpp
net::Address peer("203.0.113.42", 5000);
client.sendData(payload, payloadSize, peer);
```

Incoming data arrives in `onRelayDataReceived`.

#### Client state machine

```text
None --> Allocating --> Authorizing --> Success
                   \                       ^
                    \--> Failed            | (periodic Refresh)
                                          |
                                     timer tick
```

| State | Meaning |
| --- | --- |
| `None` | Initial; not started or shut down |
| `Allocating` | First Allocate sent; awaiting server response (may be a 401 challenge) |
| `Authorizing` | Allocate succeeded; CreatePermission requests in flight |
| `Success` | Permissions confirmed; `sendData()` delivers immediately |
| `Failed` | Unrecoverable error; `onAllocationFailed()` has been called |

#### Authentication flow

The client handles the 401 challenge automatically. The sequence:

1. `sendAllocate()` sends an Allocate request with no credentials.
2. Server responds with 401 carrying `REALM` and `NONCE`.
3. `handleAllocateErrorResponse()` stores the realm and nonce, computes the HMAC key (`MD5(username:realm:password)`), and calls `sendAllocate()` again.
4. `authenticateRequest()` decorates the new request with `USERNAME`, `REALM`, `NONCE`, and `MESSAGE-INTEGRITY`.
5. Server verifies and responds with success.

If the server later sends a 438 (Stale Nonce), the client extracts the new nonce and retries once more. All other error codes transition the client to `Failed`.

`authenticateRequest()` is also called on every subsequent request (Refresh, CreatePermission) after the realm is established, so all requests are signed for the lifetime of the allocation.

#### Pending indications

If `sendData()` is called while `CreatePermission` is still in flight (the `Authorizing` state), the `Send Indication` is queued in `_pendingIndications` and flushed automatically once `handleCreatePermissionResponse()` fires. The caller does not need to wait or check state before calling `sendData()`.

#### Lifecycle management

The internal timer fires every 30 seconds. When less than one-third of the allocation lifetime remains, the client automatically sends a `Refresh` request. `handleRefreshResponse()` updates `_lifetime` from the server's response.

To tear down cleanly:

```cpp
client.shutdown(); // sends Refresh with LIFETIME=0, then closes the socket
```

`onAllocationDeleted()` fires when the server confirms deletion. After `shutdown()`, do not call any other client methods.

#### Adding permissions after allocation

Permissions can be added after `initiate()`. If the client is already in `Success` state, call `sendCreatePermission()` manually after adding the new IP:

```cpp
client.addPermission("203.0.113.99");
client.sendCreatePermission(); // sends a new CreatePermission for all current IPs
```

The server will respond within one round trip; new `sendData()` calls for the new peer can be queued immediately (they will be held in `_pendingIndications` if still in `Authorizing`).

#### Channel binding

`Client::sendChannelBind()` always throws `std::logic_error`. Channel binding is intentionally not implemented. In Icey's deployment model, media flows through ICE/DTLS managed by libdatachannel/libjuice, not through the TURN client's `sendData()` path. The TURN client is used for ICE candidate gathering; once ICE selects the relay candidate, the media framing is handled elsewhere. The 32-byte per-packet saving (4-byte ChannelData header vs. ~36-byte STUN Send Indication) does not justify the complexity of channel number allocation and 10-minute refresh timers.

If you are building a non-WebRTC relay application where the TURN client's data path carries sustained media, subclass `Client` and implement `sendChannelBind()` and ChannelData framing there.

### TCP Extensions (RFC 6062)

RFC 6062 extends TURN to relay TCP connections. Use it when UDP is blocked, or when the application requires connection-oriented semantics from the relay. Three new methods are added: `Connect`, `ConnectionBind`, and `ConnectionAttempt`.

#### TCPClient

```cpp
#include "icy/turn/client/tcpclient.h"
```

`TCPClient` extends `Client` with a second TCP socket per peer. The control connection carries all STUN messages; each peer gets a dedicated data socket that becomes a transparent pipe after `ConnectionBind` succeeds.

Implement `TCPClientObserver`:

```cpp
class MyTCPObserver : public turn::TCPClientObserver {
public:
    void onRelayConnectionCreated(turn::TCPClient& client,
                                  const net::TCPSocket::Ptr& socket,
                                  const net::Address& peerAddress) override
    {
        // socket is now a transparent pipe to peerAddress; write directly to it
        socket->send("hello", 5);
    }

    void onRelayConnectionClosed(turn::TCPClient& client,
                                 const net::TCPSocket::Ptr& socket,
                                 const net::Address& peerAddress) override
    {
        log("relay connection to " + peerAddress.toString() + " closed");
    }

    bool onPeerConnectionAttempt(turn::TCPClient& client,
                                 const net::Address& peerAddress) override
    {
        // Return true to accept an inbound peer connection (ConnectionAttempt flow)
        return true;
    }

    // Inherit onClientStateChange and onRelayDataReceived from ClientObserver
    void onClientStateChange(turn::Client&, turn::ClientState&, const turn::ClientState&) override {}
    void onRelayDataReceived(turn::Client&, const char*, size_t, const net::Address&) override {}
};
```

Construct and initiate:

```cpp
MyTCPObserver obs;
turn::TCPClient client(obs, opts);
client.addPermission("203.0.113.42");
client.initiate();
```

Once in `Success` state, open a relay connection to a peer:

```cpp
client.sendConnectRequest(net::Address("203.0.113.42", 8080));
```

#### Client-initiated connection (Connect flow)

1. `sendConnectRequest()` sends a `Connect` request on the control connection with `XOR-PEER-ADDRESS` set to the peer.
2. The server opens a TCP connection to the peer. On success it responds with `Connect` success carrying `CONNECTION-ID`.
3. `handleConnectResponse()` extracts the `CONNECTION-ID` and calls `createAndBindConnection()`.
4. `createAndBindConnection()` opens a new TCP socket to the TURN server and sends a `ConnectionBind` request carrying the `CONNECTION-ID`.
5. The server associates the socket with the pending peer connection and responds with `ConnectionBind` success.
6. `handleConnectionBindResponse()` marks the relay socket as active and calls `onRelayConnectionCreated()`.

The relay socket is now a transparent pipe. Write to it directly; data flows to the peer without any STUN framing.

#### Server-initiated connection (ConnectionAttempt flow)

When a remote peer connects to the TURN relay address:

1. The server sends a `ConnectionAttempt` indication on the control connection, carrying `CONNECTION-ID` and `XOR-PEER-ADDRESS`.
2. `handleConnectionAttemptIndication()` calls `TCPClientObserver::onPeerConnectionAttempt()`.
3. If the observer returns `true`, `createAndBindConnection()` is called with the connection ID and peer address.
4. The `ConnectionBind` handshake proceeds as in the client-initiated flow.

#### ConnectionBind timeout

If the client does not send `ConnectionBind` within 30 seconds of the server responding to `Connect`, the server's `TCPConnectionPair` expires (`kConnectionBindTimeout = 30,000 ms`) and the peer connection is closed. The client side receives a socket close on the relay connection.

#### sendData on TCPClient

`TCPClient::sendData()` writes directly to the relay socket for the given peer address, bypassing STUN framing entirely:

```cpp
client.sendData(data, size, net::Address("203.0.113.42", 8080));
```

The peer must have an active relay connection (established via `Connect` / `ConnectionBind`). If no connection exists for the peer, the data is silently dropped.

## Samples

### [turnserver](../../src/turn/samples/turnserver/)

A minimal but complete RFC 5766 relay server with long-term credential authentication. It demonstrates the full `ServerObserver` implementation, `ServerOptions` configuration, and the `Application::waitForShutdown()` event loop pattern.

**Full walkthrough:**

Configure and instantiate:

```cpp
#include "icy/application.h"
#include "icy/crypto/hash.h"
#include "icy/turn/server/server.h"

const std::string SERVER_USERNAME("username");
const std::string SERVER_PASSWORD("password");
const std::string SERVER_REALM("example.com");

turn::ServerOptions opts;
opts.software                  = "Icey STUN/TURN Server [rfc5766]";
opts.realm                     = SERVER_REALM;
opts.listenAddr                = net::Address("0.0.0.0", 3478);
opts.externalIP                = "203.0.113.1"; // replace with your public IP
opts.allocationDefaultLifetime = 2  * 60 * 1000;
opts.allocationMaxLifetime     = 10 * 60 * 1000;
opts.timerInterval             = 5  * 1000;

RelayServer srv(opts);
srv.start();
```

`RelayServer` subclasses `ServerObserver` and passes itself as the observer to `Server`:

```cpp
class RelayServer : public turn::ServerObserver {
public:
    turn::Server server;
    RelayServer(const turn::ServerOptions& so) : server(*this, so) {}
    void start() { server.start(); }
};
```

Authentication (simplified from the sample):

```cpp
turn::AuthenticationState authenticateRequest(turn::Server*, turn::Request& req) override
{
    // Indications and Binding requests cannot carry credentials
    if (req.methodType() == stun::Message::SendIndication ||
        req.methodType() == stun::Message::Binding)
        return turn::AuthenticationState::Authorized;

    auto* username  = req.get<stun::Username>();
    auto* realm     = req.get<stun::Realm>();
    auto* nonce     = req.get<stun::Nonce>();
    auto* integrity = req.get<stun::MessageIntegrity>();
    if (!username || !realm || !nonce || !integrity)
        return turn::AuthenticationState::NotAuthorized;

    // Compute MD5(username:realm:password) and store for response signing
    crypto::Hash md5("md5");
    md5.update(SERVER_USERNAME + ":" + SERVER_REALM + ":" + SERVER_PASSWORD);
    req.hash = md5.digestStr();

    if (integrity->verifyHmac(req.hash))
        return turn::AuthenticationState::Authorized;
    return turn::AuthenticationState::NotAuthorized;
}
```

Shutdown via signal:

```cpp
app.waitForShutdown([](void* opaque) {
    reinterpret_cast<RelayServer*>(opaque)->server.stop();
}, &srv);
```

Test with coturn's `turnutils_uclient`:

```bash
turnutils_uclient -u username -w password 127.0.0.1
```

This exercises the full allocation flow: 401 challenge, re-authentication, `CreatePermission`, `SendIndication` / `DataIndication` relay, `Refresh`, and teardown.

## Configuration

### ServerOptions defaults

| Field | Default | Description |
| --- | --- | --- |
| `software` | `"Icey STUN/TURN Server [rfc5766]"` | SOFTWARE attribute in responses |
| `realm` | `"0state.com"` | REALM for long-term credential auth |
| `listenAddr` | `0.0.0.0:3478` | Bind address for UDP and TCP |
| `externalIP` | `""` | Public IP for XOR-RELAYED-ADDRESS; empty = use bind address |
| `allocationDefaultLifetime` | `120,000` ms | Default allocation lifetime (2 min) |
| `allocationMaxLifetime` | `900,000` ms | Maximum allowed lifetime (15 min) |
| `allocationMaxPermissions` | `10` | Maximum permissions per allocation |
| `timerInterval` | `10,000` ms | Expiry sweep interval (10 s) |
| `earlyMediaBufferSize` | `8192` bytes | TCP early-data buffer per connection pair |
| `enableTCP` | `true` | Accept TCP connections |
| `enableUDP` | `true` | Accept UDP datagrams |
| `enableLocalIPPermissions` | `true` | Auto-grant permissions for RFC 1918 / loopback |

### Client::Options defaults

| Field | Default | Description |
| --- | --- | --- |
| `software` | `"Icey STUN/TURN Client [rfc5766]"` | SOFTWARE attribute in requests |
| `username` | Random 4-char string | TURN username |
| `password` | Random 22-char string | TURN password |
| `lifetime` | `300,000` ms | Requested allocation lifetime (5 min) |
| `timeout` | `10,000` ms | Per-transaction timeout |
| `timerInterval` | `30,000` ms | Refresh/check interval (30 s) |
| `serverAddr` | `127.0.0.1:3478` | TURN server address |

### Protocol constants

```cpp
// types.h
constexpr uint8_t kProtocolUDP = 17; // IANA UDP
constexpr uint8_t kProtocolTCP = 6;  // IANA TCP

constexpr int CLIENT_SOCK_BUF_SIZE = 65536;           // 64 KiB per client socket
constexpr int SERVER_SOCK_BUF_SIZE = CLIENT_SOCK_BUF_SIZE * 32; // 2 MiB server socket

// permission.h
constexpr int PERMISSION_LIFETIME = 300 * 1000; // 300 s (RFC 5766 section 8)

// tcpconnectionpair.h
constexpr int kConnectionBindTimeout = 30 * 1000; // 30 s (RFC 6062 section 5.2)
```

### WebRTC Integration

When ICE selects a relay candidate for a WebRTC peer session, the path is:

```text
Browser <-- RTP/SRTP <-- DTLS <-- ICE (libjuice) <-- Icey TURN server
```

The TURN server does not inspect the relayed payload. `enableLocalIPPermissions` is particularly useful during ICE connectivity checks, which probe loopback and LAN addresses before any `CreatePermission` has been sent. Without it, checks against those addresses fail with "permission not found" errors.

Configure the WebRTC module to use the Icey TURN server:

```cpp
#include "icy/webrtc/peersession.h"

wrtc::PeerSession::Config config;
config.rtcConfig.iceServers.emplace_back(
    "turn:turn.example.com:3478",
    "username",
    "password"
);
wrtc::PeerSession session(signaller, config);
```

See [webrtc.md](webrtc.md) for the full peer session and signalling setup.

## See Also

- [STUN](stun.md) — STUN message codec, attribute types, and HMAC-SHA1 integrity
- [Crypto](crypto.md) — `crypto::Hash` for HMAC key derivation
- [WebRTC](webrtc.md) — WebRTC module that uses this server for ICE relay
