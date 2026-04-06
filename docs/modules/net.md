# Net

> Async TCP, SSL/TLS, and UDP networking built on libuv and OpenSSL.

**[API Reference →](../api/net.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/net/)**

## Overview

The `net` module is icey's networking foundation. It provides non-blocking TCP streams, TLS-encrypted connections via OpenSSL 3.x, and UDP datagrams, all driven by the same libuv event loop that powers the `base` module. Everything above it in the stack — HTTP, WebSocket, Symple, STUN, TURN — is built on `net`.

All code lives in the `icy::net` namespace. The entry-point header is `icy/net/socket.h`, which pulls in the abstract `Socket` interface and the `SocketAdapter` chain; most consumers will also include the concrete socket header they need (`tcpsocket.h`, `sslsocket.h`, `udpsocket.h`). The CMake target is `icey::net`.

`icey::net` depends on `icey::base` (event loop, handles, streams, signals, packet infrastructure) and `icey::crypto` (OpenSSL certificate and key wrappers). OpenSSL 3.x is a hard requirement; the module is skipped entirely if `HAVE_OPENSSL` is not set.

Two buffer size constants bound the receive path:

```cpp
constexpr int MAX_TCP_PACKET_SIZE = 64 * 1024;  // per TCP recv call
constexpr int MAX_UDP_PACKET_SIZE = 1500;        // per UDP datagram
```

Sockets are always heap-allocated and owned by `std::shared_ptr`. Use the `makeSocket<T>()` factory rather than `new` or `make_shared` directly; the factory wires the socket to the default event loop and handles the subtleties of libuv handle initialisation:

```cpp
#include "icy/net/socket.h"

auto tcp = icy::net::makeSocket<icy::net::TCPSocket>();
auto ssl = icy::net::makeSocket<icy::net::SSLSocket>();
auto udp = icy::net::makeSocket<icy::net::UDPSocket>();
```

An optional `uv::Loop*` argument lets you associate a socket with a non-default loop; omit it for the default.

---

## Architecture

### Class hierarchy

```text
SocketAdapter                          (icy/net/socketadapter.h)
├── Socket                             (icy/net/socket.h)
│   ├── TCPSocket                      (icy/net/tcpsocket.h)
│   │   └── SSLSocket                  (icy/net/sslsocket.h)
│   └── UDPSocket                      (icy/net/udpsocket.h)
└── SocketEmitter                      (icy/net/socketemitter.h)
    └── PacketSocketEmitter            (icy/net/packetsocket.h)
        └── Transaction<PacketT>       (icy/net/transaction.h)
```

`TCPSocket` inherits from both `Stream<uv_tcp_t>` (the libuv stream wrapper from `base`) and `Socket`. `UDPSocket` inherits from `Handle<uv_udp_t>` and `Socket`. Both handle types are from the `base` module. `SSLSocket` extends `TCPSocket` and embeds an `SSLAdapter` (the OpenSSL BIO bridge) directly as a member.

### Adapter chain

The adapter chain is the core extensibility mechanism. `SocketAdapter` defines four virtual callbacks — `onSocketConnect`, `onSocketRecv`, `onSocketError`, `onSocketClose` — and a receiver list. When a socket event fires, it walks the receiver list and calls the corresponding virtual on each. Any adapter returning `true` stops propagation; returning `false` continues to the next receiver.

```text
uv_tcp_t recv callback
    └── TCPSocket::onRead()
            └── TCPSocket::onRecv()
                    └── SocketAdapter::onSocketRecv()  [dispatch to receivers]
                            ├── MyAdapter::onSocketRecv()   → false (continue)
                            └── SocketEmitter::onSocketRecv() → fires Recv signal
```

Outgoing data flows in the opposite direction. `SocketAdapter::send()` proxies to its `_sender`, which chains upward until it reaches the concrete `Socket`, which writes to the libuv handle.

### Transport type

At runtime, `socket->transport()` returns one of three values:

| Enumerator   | Value   | Socket type |
|--------------|---------|-------------|
| `net::TCP`   | `TCP`   | `TCPSocket` |
| `net::SSLTCP`| `SSLTCP`| `SSLSocket` |
| `net::UDP`   | `UDP`   | `UDPSocket` |

Use this when you hold a `Socket*` base pointer and need to branch on protocol.

### Socket mode

`net::SocketMode` distinguishes the two roles a socket can play:

```cpp
enum SocketMode { ServerSide, ClientSide };
```

`acceptConnection()` sets accepted sockets to `ServerSide` automatically. You rarely set this manually, but `SSLSocket` reads it to decide whether to run the handshake as client or server.

---

## Usage

### TCP Connections

#### Client side

```cpp
#include "icy/net/tcpsocket.h"
#include "icy/net/socketemitter.h"

using namespace icy;

auto socket = net::makeSocket<net::TCPSocket>();
net::SocketEmitter emitter(socket);

emitter.Connect += [](net::Socket& s) {
    s.send("hello\n", 6);
    return false; // false = continue propagation
};

emitter.Recv += [](net::Socket& s, const MutableBuffer& buf, const net::Address& peer) {
    std::cout.write(bufferCast<const char*>(buf), buf.size());
    return false;
};

emitter.Error += [](net::Socket& s, const icy::Error& err) {
    std::cerr << "error: " << err.message << '\n';
    return false;
};

emitter.Close += [](net::Socket& s) {
    return false;
};

// DNS resolution happens asynchronously; errors come back via the Error signal
socket->connect("example.com", 80);

// Or skip DNS with a pre-resolved address
net::Address peer("93.184.216.34", 80);
socket->connect(peer);
```

The `MutableBuffer` passed to `Recv` is only valid for the duration of the callback. Copy the data before returning if you need it later.

#### Server side

```cpp
#include "icy/net/tcpsocket.h"

using namespace icy;

auto server = net::makeSocket<net::TCPSocket>();
server->bind(net::Address("0.0.0.0", 1337));
server->listen(); // backlog defaults to 64

server->AcceptConnection += [](const net::TCPSocket::Ptr& client) {
    // client is fully connected; begin using it immediately
    client->send("welcome\n", 8);
};
```

`AcceptConnection` is a `LocalSignal<void(const net::TCPSocket::Ptr&)>` on `TCPSocket`. It fires once per accepted client with a `shared_ptr` to the new socket. You are responsible for keeping a reference to the socket; once you drop the `shared_ptr`, the socket closes.

#### Sending data

```cpp
ssize_t n = socket->send("ping", 4);
if (n < 0) {
    // error; check socket->error()
}
```

`send` returns the number of bytes queued (always the full `len` for TCP), or `-1` on error. TCP is a stream; there are no message boundaries. The two-argument `send(data, len, peerAddress)` overload accepts a peer address for interface conformance but ignores it on TCP.

#### Socket options

```cpp
socket->setNoDelay(true);           // disable Nagle's algorithm (lower latency)
socket->setKeepAlive(true, 60);     // enable TCP keepalive, 60s initial delay
socket->setReusePort();             // SO_REUSEPORT; Linux 3.9+, call after bind()
socket->setSimultaneousAccepts(true); // Windows only; no-op elsewhere
```

All four return `bool`; they return `false` rather than throwing if the option is not supported on the current platform. `setReusePort()` enables kernel-level load balancing across multiple sockets bound to the same address, provided all of them set the flag before binding and share the same effective user ID.

#### Graceful shutdown

```cpp
// Send FIN; socket closes after the peer acknowledges
bool ok = socket->shutdown();

// Force-close immediately
socket->close();
```

`shutdown()` is `[[nodiscard]]`. `close()` triggers the `onClose` callback asynchronously after libuv processes the handle close.

---

### SSL/TLS

`SSLSocket` extends `TCPSocket` with a full TLS layer. The TCP connection is established first; the SSL handshake starts automatically once it succeeds. After the handshake, `send` and `recv` are transparent to the application: plaintext in, plaintext out.

#### SSLManager

`SSLManager` is a process-wide singleton that holds the default server and client `SSLContext` objects. Every `SSLSocket` that does not receive an explicit context queries `SSLManager` for the appropriate default at construction time.

For testing or internal services where certificate validation is not required:

```cpp
#include "icy/net/sslmanager.h"

// Server: registers a no-verify context with the given key and certificate
net::SSLManager::initNoVerifyServer("server.key.pem", "server.cert.pem");

// Client: accepts any server certificate without validation
net::SSLManager::initNoVerifyClient();
```

Always shut down the manager before process exit to release OpenSSL resources cleanly:

```cpp
net::SSLManager::instance().shutdown();
```

`SSLManager` also exposes three signals for certificate lifecycle events:

```cpp
Signal<void(VerificationErrorDetails&)> ServerVerificationError;
Signal<void(VerificationErrorDetails&)> ClientVerificationError;
Signal<void(std::string&)>              PrivateKeyPassphraseRequired;
```

#### SSLContext

`SSLContext` wraps `SSL_CTX*` and controls the certificate, private key, CA location, verification mode, and cipher list. Create one explicitly when the defaults provided by `SSLManager` are not sufficient.

```cpp
#include "icy/net/sslcontext.h"

// Full server context: key, certificate, CA directory, strict client verification
auto ctx = std::make_shared<net::SSLContext>(
    net::SSLContext::SERVER_USE,
    "server.key.pem",    // private key file
    "server.cert.pem",   // certificate file (PEM)
    "/etc/ssl/certs",    // CA directory; empty string uses OpenSSL built-ins
    net::SSLContext::VERIFY_STRICT,
    9,                   // maximum verification chain depth
    true                 // load OpenSSL built-in CA certificates
);
net::SSLManager::instance().initializeServer(ctx);
```

The two-argument constructor omits key and certificate (supply them later via `useCertificate()`):

```cpp
// Client context: no private key, built-in CAs, relaxed verification
auto ctx = std::make_shared<net::SSLContext>(
    net::SSLContext::CLIENT_USE,
    "",                              // caLocation; empty = use built-ins
    net::SSLContext::VERIFY_RELAXED,
    9,
    true
);
```

**Verification modes:**

| Mode             | Server behaviour                  | Client behaviour        |
|------------------|-----------------------------------|-------------------------|
| `VERIFY_NONE`    | No client cert requested          | Server cert not checked |
| `VERIFY_RELAXED` | Client cert checked if presented  | Server cert checked     |
| `VERIFY_STRICT`  | Client cert required and checked  | Same as RELAXED         |
| `VERIFY_ONCE`    | Verify only on initial handshake  | Same as RELAXED         |

These map directly to the corresponding `SSL_VERIFY_*` flags.

**ALPN negotiation** (e.g., for HTTP/2):

```cpp
ctx->setALPNProtocols({"h2", "http/1.1"});
```

**Session caching** (server side):

```cpp
ctx->enableSessionCache(true, "myapp");  // sessionIdContext identifies the application
ctx->setSessionCacheSize(1024);          // max cached sessions; 0 = unlimited
ctx->setSessionTimeout(300);             // seconds until a cached session expires
ctx->flushSessionCache();                // evict all cached sessions immediately
ctx->disableStatelessSessionResumption(); // disable RFC 4507 tickets
```

**Certificate chain and verification overrides:**

```cpp
crypto::X509Certificate chain_cert("chain.pem");
ctx->addChainCertificate(chain_cert);

crypto::X509Certificate extra_ca("extra-ca.pem");
ctx->addVerificationCertificate(extra_ca);
```

**Utility functions** (available in `sslcontext.h`):

```cpp
// Parse a string to a VerificationMode enum value
auto mode = net::convertVerificationMode("strict"); // net::SSLContext::VERIFY_STRICT

// Get the last OpenSSL error as a string
std::string err = net::getLastError();

// Clear the OpenSSL error stack
net::clearErrorStack();
```

#### SSLSocket: client side

```cpp
#include "icy/net/sslsocket.h"

auto socket = net::makeSocket<net::SSLSocket>();

// Set hostname before connect() for SNI and certificate CN verification
socket->setHostname("api.example.com");
socket->connect("api.example.com", 443);

// Plaintext send/recv work identically to TCPSocket after the handshake
socket->send("GET / HTTP/1.1\r\n\r\n", 18);
```

You can pass an explicit context to the constructor instead of using the `SSLManager` default:

```cpp
auto socket = net::makeSocket<net::SSLSocket>(ctx);
```

#### SSLSocket: server side

```cpp
// Assumes SSLManager has a server context registered
auto server = net::makeSocket<net::SSLSocket>();
server->bind(net::Address("0.0.0.0", 1338));
server->listen();

server->AcceptConnection += [](const net::TCPSocket::Ptr& client) {
    // AcceptConnection fires with a TCPSocket::Ptr; cast for SSL-specific methods
    auto ssl = std::static_pointer_cast<net::SSLSocket>(client);

    X509* cert = ssl->peerCertificate(); // nullptr if no client cert was presented
    if (cert) {
        // inspect cert with OpenSSL X509 APIs
    }
    ssl->send("connected\n", 10);
};
```

The SSL handshake runs in the background before `AcceptConnection` fires on the accepted socket. On the server, the accepted socket is initialised with the server-side SSL context automatically.

#### Session resumption

Session resumption avoids a full handshake on reconnection, saving one round-trip and CPU-intensive key exchange.

```cpp
// Save the session after a connection closes
net::SSLSession::Ptr saved = socket->currentSession();

// Reuse it on the next connection
auto next = net::makeSocket<net::SSLSocket>(ctx, saved);
next->connect("api.example.com", 443);

// After the handshake, verify that resumption actually occurred
if (next->sessionWasReused()) {
    // fast path: no full key exchange
}
```

Session caching must be enabled on the server side (`ctx->enableSessionCache(...)`) for resumption to succeed.

#### Certificate verification errors

`SSLManager` fires `ClientVerificationError` (or `ServerVerificationError`) when the OpenSSL verification callback receives a failing certificate. Connect a handler before making any connections:

```cpp
net::SSLManager::instance().ClientVerificationError +=
    [](net::VerificationErrorDetails& details) {
        std::cerr << "cert error " << details.errorNumber()
                  << " at depth " << details.errorDepth()
                  << ": " << details.errorMessage() << '\n';

        // Inspect the certificate that caused the failure
        const crypto::X509Certificate& cert = details.certificate();

        // Override the failure and allow the connection to proceed
        details.setIgnoreError(true);
    };
```

`VerificationErrorDetails` carries: the offending certificate (`certificate()`), its depth in the chain (`errorDepth()`), the OpenSSL error code (`errorNumber()`), the human-readable message (`errorMessage()`), and a mutable `setIgnoreError(bool)` that controls whether the handshake continues.

#### Encrypted private key passphrase

If the server private key is passphrase-protected, handle `PrivateKeyPassphraseRequired` before loading the context:

```cpp
net::SSLManager::instance().PrivateKeyPassphraseRequired +=
    [](std::string& passphrase) {
        passphrase = "my-secret";
    };
```

#### SSLAdapter internals

`SSLAdapter` is an implementation detail of `SSLSocket`. It owns the `SSL*` object and two memory BIOs: a read BIO for incoming ciphertext from the network, and a write BIO for outgoing ciphertext headed to the network. You do not interact with it directly.

When encrypted bytes arrive from the network, `SSLSocket::onRead` feeds them into `SSLAdapter::addIncomingData`, which drives the handshake state machine or decrypts and delivers plaintext via `TCPSocket::onRecv`. When the application calls `SSLSocket::send`, the plaintext is queued via `addOutgoingData`, encrypted by `flushWriteBIO`, and written to the TCP stream.

---

### UDP

`UDPSocket` wraps `uv_udp_t`. UDP is connectionless; calling `connect()` on a `UDPSocket` records the peer address for use by the single-argument `send()` overload, but sends no data and performs no handshake. The receive loop starts automatically when `bind()` is called.

#### Binding and receiving

```cpp
#include "icy/net/udpsocket.h"
#include "icy/net/socketadapter.h"

using namespace icy;

struct EchoHandler : public net::SocketAdapter {
    net::UDPSocket::Ptr socket;

    bool onSocketRecv(net::Socket&, const MutableBuffer& buf,
                      const net::Address& peer) override
    {
        // Echo the datagram back; peer address is required for UDP
        socket->send(bufferCast<const char*>(buf), buf.size(), peer);
        return false;
    }
};

EchoHandler handler;
handler.socket = net::makeSocket<net::UDPSocket>();
handler.socket->addReceiver(&handler);
handler.socket->bind(net::Address("0.0.0.0", 9000));
// receive loop starts on bind(); no separate listen() call needed
```

Unlike TCP, each `onSocketRecv` call carries the sender's address in `peerAddress`. You must pass this address back to `send()` when responding; the socket has no implicit peer.

#### Sending datagrams

```cpp
// Explicit peer address
net::Address peer("192.168.1.10", 9000);
ssize_t n = socket->send("ping", 4, peer);

// Record a default peer with connect(), then use the single-argument overload
socket->connect(peer);
socket->send("ping", 4);
```

`send` returns the number of bytes accepted for sending, or `-1` on error.

#### Multicast

```cpp
socket->setMulticastLoop(true);  // receive your own multicast transmissions
socket->setMulticastTTL(8);      // hop limit; must be in [1, 255] or throws
```

#### Broadcast

```cpp
socket->setBroadcast(true);

net::Address bcast("255.255.255.255", 9000);
socket->send("beacon", 6, bcast);
```

All three option setters return `bool` and return `false` on failure rather than throwing. `setMulticastTTL` is the exception; it throws `std::invalid_argument` if the value is outside `[1, 255]`.

---

### Socket Adapters

`SocketAdapter` is the mechanism for composing behaviour on top of a socket without subclassing it. Any object that extends `SocketAdapter` and overrides one or more of the four virtual callbacks can be attached to a socket (or to another adapter) via `addReceiver`. Multiple receivers are dispatched in registration order; lower `priority` values sort earlier if you set them.

```cpp
class Net_API SocketAdapter
{
public:
    virtual bool onSocketConnect(Socket& socket);
    virtual bool onSocketRecv(Socket& socket, const MutableBuffer& buffer,
                              const Address& peerAddress);
    virtual bool onSocketError(Socket& socket, const Error& error);
    virtual bool onSocketClose(Socket& socket);

    virtual void addReceiver(SocketAdapter* adapter);
    virtual void removeReceiver(SocketAdapter* adapter);
    virtual bool hasReceiver(SocketAdapter* adapter);
    std::vector<SocketAdapter*> receivers();

    virtual void setSender(SocketAdapter* adapter);
    SocketAdapter* sender();

    virtual ssize_t send(const char* data, size_t len, int flags = 0);
    virtual ssize_t send(const char* data, size_t len, const Address& peer, int flags = 0);
    virtual ssize_t sendPacket(const IPacket& packet, int flags = 0);
    virtual void    sendPacket(IPacket& packet); // throws if socket closed

    int priority = 0;
};
```

Returning `true` from any callback stops propagation to subsequent receivers. Returning `false` continues to the next one in the chain.

`setSender` / `sender` allows an adapter to proxy outgoing data to another adapter rather than directly to the socket, enabling multi-stage processing pipelines:

```cpp
// Adapter chain: application → compressor → encryptor → socket
compressor.setSender(&socket);
encryptor.setSender(&compressor);
// application sends through encryptor
```

#### SocketEmitter

`SocketEmitter` is a `SocketAdapter` that exposes the four callbacks as signals instead of virtual methods. It wraps the underlying socket via a `Socket::Ptr impl` member and overloads `operator->` for transparent access.

```cpp
#include "icy/net/socketemitter.h"

using namespace icy;

net::SocketEmitter emitter(net::makeSocket<net::TCPSocket>());

emitter.Connect += [](net::Socket& s) {
    s.send("hello", 5);
    return false;
};

emitter.Recv += [](net::Socket& s, const MutableBuffer& buf,
                   const net::Address& peer) {
    // process received data
    return false;
};

emitter.Error += [](net::Socket& s, const icy::Error& err) {
    std::cerr << "error: " << err.message << '\n';
    return false;
};

emitter.Close += [](net::Socket& s) {
    return false;
};

// Access underlying socket methods via ->
emitter->connect("example.com", 80);
emitter->setNoDelay(true);

// Cast to a concrete type when needed
auto* tcp = emitter.as<net::TCPSocket>(); // nullptr if wrong type
```

`swap(socket)` replaces the underlying socket after construction. It throws `std::logic_error` if the emitter already has a socket attached.

#### PacketSocketEmitter

`PacketSocketEmitter` extends `SocketEmitter` with a `PacketFactory`. Raw received bytes are parsed into `IPacket` subclasses by the factory's registered strategies before being dispatched via `PacketSignal`. This is how the `http` module layers HTTP parsing on top of a raw `TCPSocket`.

```cpp
#include "icy/net/packetsocket.h"

net::PacketSocketEmitter pse(socket);
pse.factory.registerStrategy<MyPacketStrategy>();

// Typed packets arrive via operator+=
pse += slot(this, &MyClass::onPacket); // void onPacket(IPacket&)
```

The factory tries registered strategies in priority order. The first one that successfully parses the buffer wins; the resulting packet is emitted through `PacketSignal`.

#### SocketPacket and PacketInfo

`SocketPacket` is the default packet type emitted by sockets when no packet factory is registered. It extends `RawPacket` and carries a `PacketInfo` attachment that holds a `Socket::Ptr` and the originating `Address`.

```cpp
void onPacket(IPacket& pkt) {
    auto& sp = static_cast<net::SocketPacket&>(pkt);
    net::PacketInfo* info = sp.packetInfo();

    // info->socket   — the receiving socket
    // info->peerAddress — where the data came from
    std::cout << "from " << info->peerAddress
              << ", " << sp.size() << " bytes\n";
}
```

The buffer inside `SocketPacket` is only valid for the duration of the receive callback. Call `sp.clone()` if you need to retain the data.

#### Transaction

`Transaction<PacketT>` implements request/response over a socket. It combines `PacketSocketEmitter` (for packet parsing) with `PacketTransaction<PacketT>` (for timeout and retry logic from `base`). It sends the request, waits for a matching response, retries on timeout, and emits the response via `PacketSignal`. The STUN and TURN modules use this directly.

```cpp
#include "icy/net/transaction.h"

net::Transaction<MyPacket> txn(socket, peerAddress,
                               /*timeout_ms=*/5000,
                               /*retries=*/2);

// Connect a response handler
txn += slot(this, &MyClass::onResponse);

// Set the request packet
txn.request() = MyPacket(/* ... */);

// Send; returns false if the socket could not deliver the packet
if (!txn.send()) {
    // handle failure
}
```

`checkResponse` verifies a candidate packet by matching `impl->address()` against the socket address in `PacketInfo` and `_peerAddress` against `PacketInfo::peerAddress`. Subclass `Transaction<PacketT>` and override `checkResponse` for stricter or looser matching rules.

---

### DNS Resolution

Async DNS resolution wraps libuv's `uv_getaddrinfo`:

```cpp
#include "icy/net/dns.h"

net::dns::resolve("example.com", 80,
    [](int status, const net::Address& addr) {
        if (status != 0) {
            std::cerr << "DNS failed: " << uv_strerror(status) << '\n';
            return;
        }
        std::cout << "resolved: " << addr << '\n';
        // addr is now a valid net::Address; pass to socket->connect(addr)
    });
```

The callback fires on the event loop thread. Letting the returned request handle go out of scope cancels an in-flight query.

`TCPSocket::connect(host, port)` and `UDPSocket::connect(host, port)` call `net::dns::resolve` internally. The string `"localhost"` is mapped to `"127.0.0.1"` before resolution to avoid a round-trip to the OS resolver.

---

### Address Handling

`net::Address` represents an IPv4 or IPv6 socket address. It stores the native `sockaddr` internally via a shared pimpl and supports value semantics: copyable, comparable, streamable, and sortable.

```cpp
#include "icy/net/address.h"

// From separate host and port
net::Address a("192.168.1.1", 8080);

// From a combined string (host:port)
net::Address b("192.168.1.1:8080");

// IPv6; brackets required in combined form
net::Address c("[::1]:9000");

// From a service name instead of a numeric port
net::Address d("0.0.0.0", net::Address::resolveService("http")); // port 80

// From a native sockaddr (e.g. from a libuv callback)
net::Address e(reinterpret_cast<const sockaddr*>(&sa), sizeof(sa));

// The default constructor produces a wildcard (0.0.0.0:0)
net::Address wildcard;
assert(!wildcard.valid());

// Accessors
std::string host = a.host();        // "192.168.1.1"
uint16_t    port = a.port();        // 8080
bool        ok   = a.valid();       // true; false for wildcard
std::string s    = a.toString();    // "192.168.1.1:8080"
int         af   = a.af();          // AF_INET or AF_INET6
Address::Family f = a.family();     // Address::IPv4 or Address::IPv6
socklen_t   len  = a.length();      // sizeof(sockaddr_in) or sizeof(sockaddr_in6)
const sockaddr* raw = a.addr();     // pointer to internal native address

// Comparison
assert(a == net::Address("192.168.1.1", 8080));
assert(a != c);

// Ordering (by family, then port)
std::set<net::Address> addr_set;
addr_set.insert(a);

// Streaming
std::cout << a << '\n'; // 192.168.1.1:8080

// Static helpers
bool valid_ip = net::Address::validateIP("10.0.0.1");   // true
bool not_ip   = net::Address::validateIP("not-an-ip");  // false
uint16_t http = net::Address::resolveService("http");   // 80
```

`address()` and `peerAddress()` on a socket return a wildcard address when the socket is not yet bound or connected. Always check `addr.valid()` before using a socket address.

---

### Utility Helpers

`icy/net/util.h` provides low-level platform helpers:

```cpp
#include "icy/net/util.h"

// Enumerate all local network interface addresses
std::vector<net::Address> ifaces;
net::getNetworkInterfaces(ifaces);
for (const auto& addr : ifaces)
    std::cout << addr << '\n';

// Query OS socket buffer sizes (templated on uv handle type)
int send_sz = net::getServerSocketSendBufSize(handle); // returns current value
int recv_sz = net::getServerSocketRecvBufSize(handle);

// Set OS socket buffer sizes
net::setServerSocketSendBufSize(handle, 256 * 1024);
net::setServerSocketRecvBufSize(handle, 256 * 1024);
```

The buffer size functions are templated on the libuv handle type and accept any `uv::Handle<T>`. They call `uv_send_buffer_size` / `uv_recv_buffer_size` with `val=0` to query, or with the provided value to set. Return values are libuv error codes (0 = success, negative = error).

The `nativeSocketFd(handle)` macro extracts the OS file descriptor from a libuv handle. It is platform-specific (Windows uses `.socket`; Linux uses `io_watcher.fd`; macOS uses `uv___stream_fd`).

---

## Samples

### [echoserver](https://github.com/nilstate/icey/tree/main/src/net/samples/echoserver/)

Runs three echo servers simultaneously — TCP on port 1337, SSL on port 1338, UDP on port 1339 — in a single binary. This is the canonical reference for the adapter pattern and for handling all three transports in one codebase.

The TCP and SSL servers share the templated `EchoServer<SocketT>` class in `tcpechoserver.h`. `EchoServer` is itself a `SocketAdapter`; when a client connects via `AcceptConnection`, the server registers itself as a receiver on the new client socket. `onSocketRecv` echoes the buffer straight back. `onSocketClose` removes the closed socket from the `sockets` vector to release the `shared_ptr` and allow libuv to clean up the handle.

The template works for both `TCPSocket` and `SSLSocket` without change in the recv/close paths. The only SSL-specific code is in `start()`, which `dynamic_cast`s to `SSLSocket*` to install the server context from `SSLManager`.

The UDP server (`UDPEchoServer` in `udpechoserver.h`) is simpler: it registers itself as a receiver on construction, binds in `start()`, and echoes each datagram back to the peer address received in `onSocketRecv`.

**Build:**

```bash
cmake -B build -DBUILD_SAMPLES=ON
cmake --build build --target echoserver
```

**Run:**

```bash
./echoserver server.key.pem server.cert.pem
```

**Test:**

```bash
# TCP
echo "hello" | nc localhost 1337

# SSL (suppress cert warnings with -quiet)
echo "hello" | openssl s_client -connect localhost:1338 -quiet

# UDP
echo "hello" | nc -u localhost 1339
```

Send `SIGINT` (Ctrl+C) to trigger a clean shutdown via `waitForShutdown` (from `base`).

---

## Configuration

### CMake options

`icey::net` is enabled automatically when OpenSSL is detected. The relevant CMake flag that controls detection is `HAVE_OPENSSL`; this is set by the top-level `CMakeLists.txt` when `find_package(OpenSSL)` succeeds.

To build samples:

```cmake
cmake -B build -DBUILD_SAMPLES=ON
```

To link `icey::net` in your project:

```cmake
target_link_libraries(myapp PRIVATE icey::net)
```

`icey::net` transitively pulls in `icey::base`, `icey::crypto`, `OpenSSL::SSL`, and `OpenSSL::Crypto`. You do not need to list them separately.

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/nilstate/icey.git
  GIT_TAG 2.4.3
)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE icey::net)
```

### OpenSSL version

OpenSSL 3.x is required. OpenSSL 1.x is not supported. On Linux, install `libssl-dev`; on macOS, `brew install openssl@3` and set `OPENSSL_ROOT_DIR` if CMake does not find it automatically.

---

## See Also

- [Base](base.md) — event loop, handles, streams, signals, `PacketStream`, `PacketTransaction`
- [HTTP](http.md) — HTTP server and client, WebSocket upgrade; builds entirely on `net`
- [Crypto](crypto.md) — X.509 certificates, RSA keys; used by `SSLContext` and `SSLManager`
- [STUN](stun.md) — NAT traversal; uses `Transaction<StunMessage>` over UDP
- [TURN](turn.md) — relay server; builds on `net` TCP and UDP sockets

## Best Next Reads

- [Runtime Contracts](../concepts/runtime-contracts.md) for the borrowed buffer and `send()` vs `sendOwned()` rules
- [HTTP](http.md) for the first higher-level protocol layer built directly on top of these sockets
- [HTTP Server](../recipes/http-server.md) if you want a concrete service path instead of the raw transport layer
