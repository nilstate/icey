# HTTP

> HTTP/1.1 server and client, WebSocket, streaming, forms, cookies, and authentication; all async, no threads required.

**[API Reference →](../api/http.md)** · **[Source →](../../src/http/)**

## Overview

The `Icey::http` module delivers a full HTTP/1.1 stack built on two proven foundations: [libuv](https://libuv.org) for async I/O and [llhttp](https://github.com/nodejs/llhttp) for parsing (the same parser used in Node.js). Everything is non-blocking and event-driven. A single-threaded server handles thousands of concurrent connections without spawning one thread per request. For multicore machines, `SO_REUSEPORT` lets you run one server instance per CPU core with kernel-level load balancing, no user-space distributor needed.

The module covers:

- HTTP/1.1 server with connection pooling, keep-alive, and `SO_REUSEPORT` multicore
- HTTP client with streaming, download progress, and HTTPS via `SSLSocket`
- WebSocket (RFC 6455); both client and server, including fragmentation and masking
- RFC 3986 URL parsing with percent-encoding helpers
- Multipart form uploads and `application/x-www-form-urlencoded` submissions
- HTTP Basic and Digest authentication
- Cookies (Netscape v0 and RFC 2109 v1)
- Chunked transfer encoding and `multipart/x-mixed-replace` streaming adapters for media pipelines

Dependencies resolved automatically via CMake FetchContent:

| Dependency | Version | Role |
| ---------- | ------- | ---- |
| libuv | 1.50 | Event loop, TCP I/O |
| llhttp | 9.2.1 | HTTP parsing |
| OpenSSL | 3.x | TLS (for HTTPS/WSS) |

## Architecture

### Class hierarchy

```text
net::SocketAdapter
└── http::Connection                  (base; owns socket, parser, request, response)
    ├── http::ServerConnection         (accepted server-side connection)
    └── http::ClientConnection         (outgoing client-side connection)

net::SocketAdapter
└── http::ConnectionAdapter           (default HTTP read/write adapter; wraps Parser)
    └── http::ws::ConnectionAdapter   (WebSocket upgrade adapter)

http::ws::WebSocketAdapter            (standalone WebSocket; owns its own request/response)
└── http::ws::WebSocket               (standalone WebSocket class)
```

`Connection` sits at the centre. It owns a `net::TCPSocket` (or `SSLSocket`), a `ConnectionAdapter` that drives the `Parser`, and the `Request`/`Response` message pair. The `ConnectionAdapter` slots between the socket and the connection; it can be replaced at runtime to upgrade a plain HTTP connection to WebSocket (`replaceAdapter()`).

### Server connection lifecycle

```text
                        ┌──────────────────────────────────────────┐
                        │               http::Server                │
                        │                                           │
  TCP accept ──────────>│  ConnectionPool (LIFO, cap 128)          │
                        │       │ acquire()    release()            │
                        │       v                                   │
                        │  ServerConnection::reset(socket)          │
                        │       │                                   │
                        │       v                                   │
                        │  ConnectionAdapter (llhttp parser)        │
                        │       │                                   │
                        │  onHeaders()  onPayload()  onComplete()   │
                        │       │                                   │
                        │  Server::Connection signal fires          │
                        │       │                                   │
                        │  ServerResponder (optional)               │
                        └──────────────────────────────────────────┘
                                │
                         send / close
                                │
                        keep-alive timer (30s default)
```

### WebSocket upgrade flow

```text
Client                      Server (http::ServerConnection)
  │                                │
  │── GET /ws HTTP/1.1 ───────────>│
  │   Upgrade: websocket           │
  │   Sec-WebSocket-Key: ...       │
  │                                │── onHeaders() fires
  │                                │   replaceAdapter(ws::ConnectionAdapter, ServerSide)
  │<── HTTP/1.1 101 ───────────────│   adapter validates key, sends 101
  │    Sec-WebSocket-Accept: ...   │
  │                                │── onHandshakeComplete() fires
  │── [WS frames] ────────────────>│── WebSocketFramer::readFrame()
  │<── [WS frames] ────────────────│── WebSocketFramer::writeFrame()
```

After upgrade, `ServerConnection::upgraded()` returns `true` and all `send()` calls go through the `ws::ConnectionAdapter`, which frames data automatically.

## Usage

### HTTP Server

The minimal server: bind a port, connect a lambda, run the loop.

```cpp
#include "icy/http/server.h"

using namespace icy;

http::Server srv("0.0.0.0", 1337);
srv.start();

srv.Connection += [](http::ServerConnection::Ptr conn) {
    auto& req = conn->request();
    auto& res = conn->response();

    std::string body = "hello\n";
    res.setStatus(http::StatusCode::OK);
    res.setContentType("text/plain");
    res.setContentLength(body.size());

    conn->sendHeader();
    conn->send(body.c_str(), body.size());
    conn->close(); // omit to keep connection alive for next request
};

uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);
```

`srv.Connection` fires once per fully parsed request. For POST/PUT bodies, listen for `Payload` on the individual connection instead:

```cpp
srv.Connection += [](http::ServerConnection::Ptr conn) {
    conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buf) {
        // buf is a chunk of the request body; may fire multiple times
        conn.send(bufferCast<const char*>(buf), buf.size()); // echo it back
        conn.close();
    };
};
```

**HTTPS** requires an `SSLSocket` and a loaded SSL context:

```cpp
#include "icy/net/sslmanager.h"
#include "icy/net/sslsocket.h"

net::SSLManager::initNoVerifyServer("key.pem", "cert.pem");

http::Server srv(
    "0.0.0.0", 443,
    std::make_shared<net::SSLSocket>(
        net::SSLManager::instance().defaultServerContext()));
srv.start();
```

**Constructor overloads:**

```cpp
// Host + port; creates a TCPSocket internally
http::Server srv("0.0.0.0", 8080);

// net::Address
http::Server srv(net::Address("0.0.0.0", 8080));

// Pre-created socket (e.g. SSLSocket); loop is derived from the socket
http::Server srv("0.0.0.0", 443, sslSocket);

// With a custom factory and explicit loop
http::Server srv(address, loop, std::make_unique<MyFactory>());
```

**Server configuration:**

| Method | Default | Description |
| ------ | ------- | ----------- |
| `setReusePort(bool)` | false | Enable `SO_REUSEPORT`; must be called before `start()` |
| `setMaxPooledConnections(size_t)` | 128 | LIFO pool capacity; 0 disables pooling |
| `setKeepAliveTimeout(int)` | 30 | Seconds before idle keep-alive connections close; 0 disables |

**Server signals:**

```cpp
LocalSignal<void(ServerConnection::Ptr)> Connection; // new request ready
LocalSignal<void()>                      Shutdown;    // server shutting down
```

**ServerConnection signals:**

```cpp
LocalSignal<void(ServerConnection&, const MutableBuffer&)> Payload; // body chunk
LocalSignal<void(ServerConnection&)>                       Close;   // connection closed
```

**Useful `ServerConnection` members:**

```cpp
Request&  request();           // incoming request
Response& response();          // outgoing response to populate
bool      upgraded() const;    // true after WebSocket upgrade
void      touch();             // reset idle timestamp (called automatically)
double    idleSeconds() const; // seconds since last request
ssize_t   sendHeader();        // write the response header to the socket
ssize_t   send(const char*, size_t, int flags = 0);
void      close();
```

#### ServerResponder

For structured routing, subclass `ServerResponder` instead of connecting lambdas. The server calls your factory once per accepted connection.

```cpp
#include "icy/http/server.h"

class ApiResponder : public http::ServerResponder {
public:
    ApiResponder(http::ServerConnection& conn)
        : http::ServerResponder(conn) {}

    // Called when headers are parsed; body may not be available yet.
    void onHeaders(http::Request& req) override {
        // inspect method, URI, headers early
    }

    // Called for each body chunk.
    void onPayload(const MutableBuffer& body) override {
        _body.append(bufferCast<const char*>(body), body.size());
    }

    // Called when the complete message is received; write your response here.
    void onRequest(http::Request& req, http::Response& res) override {
        std::string reply = R"({"status":"ok"})";
        res.setStatus(http::StatusCode::OK);
        res.setContentType("application/json");
        res.setContentLength(reply.size());
        connection().sendHeader();
        connection().send(reply.c_str(), reply.size());
        connection().close();
    }

    // Called on connection close; clean up any per-request state.
    void onClose() override {}

private:
    std::string _body;
};

class ApiFactory : public http::ServerConnectionFactory {
public:
    std::unique_ptr<http::ServerResponder>
    createResponder(http::ServerConnection& conn) override {
        return std::make_unique<ApiResponder>(conn);
    }
};

http::Server srv("0.0.0.0", 8080, uv::defaultLoop(),
                 std::make_unique<ApiFactory>());
srv.start();
```

Responder lifecycle order: `onHeaders` -> `onPayload` (zero or more times) -> `onRequest` -> `onClose`.

#### Multicore servers

Spawn one server per CPU core, each with its own libuv loop and `setReusePort()`. The kernel distributes accepted connections across instances without a user-space load balancer.

```cpp
#include "icy/http/server.h"
#include <thread>

static const net::Address address("0.0.0.0", 1337);

void runInstance() {
    uv::ScopedLoop loop;
    http::Server srv(address, loop);
    srv.setReusePort();
    srv.start();

    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->response().set("Content-Length", "0");
        conn->sendHeader();
    };

    waitForShutdown([&](void*) { srv.shutdown(); }, nullptr, loop);
}

// In main:
int ncpus = std::thread::hardware_concurrency();
std::vector<std::unique_ptr<Thread>> threads;
for (int i = 0; i < ncpus; ++i)
    threads.push_back(std::make_unique<Thread>(runInstance));
for (auto& t : threads) t->join();
```

### HTTP Client

`ClientConnection` manages a single outgoing request. Call `send()` to initiate the TCP connection; data is buffered internally until the socket is ready.

```cpp
#include "icy/http/client.h"

using namespace icy;

auto conn = http::createConnectionT<http::ClientConnection>(
    http::URL("http://api.example.com/v1/items"));

conn->Headers += [](http::Response& res) {
    std::cout << static_cast<int>(res.getStatus())
              << " " << res.getReason() << '\n';
    for (const auto& [name, value] : res)
        std::cout << "  " << name << ": " << value << '\n';
};

conn->Payload += [](const MutableBuffer& buf) {
    std::cout.write(bufferCast<const char*>(buf), buf.size());
};

conn->Complete += [](const http::Response& res) {
    std::cout << "done\n";
};

conn->Close += [](http::Connection&) {
    uv_stop(uv::defaultLoop());
};

conn->send(); // connect + send GET
uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);
```

**`ClientConnection` signals:**

```cpp
NullSignal                         Connect;          // socket connected, data can flow
Signal<void(Response&)>            Headers;          // response headers received
Signal<void(const MutableBuffer&)> Payload;          // response body chunk
Signal<void(const Response&)>      Complete;         // full response received
Signal<void(Connection&)>          Close;            // connection closed
ProgressSignal                     IncomingProgress; // download progress (0-100%)
```

#### createConnectionT

`createConnectionT<T>` selects the right socket and adapter from the URL scheme:

| Scheme | Socket | Adapter |
| ------ | ------ | ------- |
| `http` | `TCPSocket` | HTTP |
| `https` | `SSLSocket` | HTTP |
| `ws` | `TCPSocket` | `ws::ConnectionAdapter` |
| `wss` | `SSLSocket` | `ws::ConnectionAdapter` |

```cpp
// Plain HTTP
auto conn = http::createConnectionT<http::ClientConnection>(
    http::URL("http://example.com/path"));

// HTTPS (SSLSocket selected automatically)
auto conn = http::createConnectionT<http::ClientConnection>(
    http::URL("https://api.example.com/path"));
```

#### Custom request

Replace the default GET by constructing a `Request` and calling the `send(Request&)` overload:

```cpp
http::Request req(http::Method::Post, "/upload");
req.setHost("api.example.com");
req.setContentType("application/json");
std::string body = R"({"key":"value"})";
req.setContentLength(body.size());

conn->send(req);
conn->send(body.c_str(), body.size());
```

#### Streaming response to a file

Call `setReadStream()` before `send()` to pipe the response body directly into any `std::ostream`. The connection takes ownership.

```cpp
auto* fs = new std::ofstream("/tmp/download.bin", std::ios::binary);
conn->setReadStream(fs);
conn->send();
```

Retrieve the stream later with the typed accessor:

```cpp
auto& file = conn->readStream<std::ofstream>();
```

#### Client instance

`http::Client` tracks multiple connections and shuts them down cleanly on `destroy()`.

```cpp
auto& client = http::Client::instance();
auto conn = client.createConnection(http::URL("http://example.com"));
conn->send();

// on shutdown:
http::Client::destroy();
```

`Client::createConnectionT<T>()` mirrors the free function and registers the connection with the instance automatically.

### WebSocket

WebSocket support is in `icy/http/websocket.h` under `icy::http::ws`. The same `createConnectionT` helper used for HTTP handles `ws://` and `wss://`, installing a `ws::ConnectionAdapter` automatically.

#### Client

```cpp
#include "icy/http/client.h"
#include "icy/http/websocket.h"

using namespace icy;

auto conn = http::createConnectionT<http::ClientConnection>(
    http::URL("ws://echo.websocket.events"));

conn->Connect += [&]() {
    // Connect fires after the WebSocket handshake, not just TCP connect.
    conn->send("Hello!", 6, http::ws::Text);
};

conn->Payload += [](const MutableBuffer& buf) {
    std::string msg(bufferCast<const char*>(buf), buf.size());
    std::cout << "received: " << msg << '\n';
};

conn->Close += [](http::Connection&) {
    uv_stop(uv::defaultLoop());
};

conn->send(); // TCP connect + WebSocket handshake
uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);
```

For `wss://`, substitute the scheme; `createConnectionT` selects `SSLSocket` transparently.

The `flags` argument to `send()` controls the WebSocket frame type:

```cpp
conn->send(data, len, http::ws::Text);   // FIN | Text opcode  (0x81)
conn->send(data, len, http::ws::Binary); // FIN | Binary opcode (0x82)
```

#### Server-side upgrade

Detect the `Upgrade` header and replace the connection adapter:

```cpp
srv.Connection += [](http::ServerConnection::Ptr conn) {
    if (conn->request().get("Upgrade") == "websocket") {
        // Validates Sec-WebSocket-Key, sends 101 Switching Protocols.
        conn->replaceAdapter(
            std::make_unique<http::ws::ConnectionAdapter>(
                conn.get(), http::ws::ServerSide));
        // After onHandshakeComplete(), incoming frames arrive via Payload
        // and outgoing frames are sent with conn->send(data, len, ws::Text).
        return;
    }

    conn->response().setStatus(http::StatusCode::OK);
    conn->sendHeader();
    conn->close();
};
```

#### Sending a close frame

```cpp
auto* adapter = static_cast<http::ws::ConnectionAdapter*>(conn->adapter());
adapter->shutdown(
    static_cast<uint16_t>(http::ws::CloseStatusCode::NormalClose),
    "goodbye");
```

#### Frame types and error codes

```cpp
// Opcodes
enum class ws::Opcode {
    Continuation = 0x00,
    Text         = 0x01,
    Binary       = 0x02,
    Close        = 0x08,
    Ping         = 0x09,
    Pong         = 0x0a,
};

// Close status codes (used with shutdown())
enum class ws::CloseStatusCode {
    NormalClose          = 1000,
    EndpointGoingAway    = 1001,
    ProtocolError        = 1002,
    PayloadNotAcceptable = 1003,
    MalformedPayload     = 1007,
    PolicyViolation      = 1008,
    PayloadTooBig        = 1009,
    ExtensionRequired    = 1010,
    UnexpectedCondition  = 1011,
};

// Error codes thrown as std::runtime_error
enum class ws::ErrorCode {
    NoHandshake              = 1,
    HandshakeNoVersion       = 2,
    HandshakeUnsupportedVersion = 3,
    HandshakeNoKey           = 4,
    HandshakeAccept          = 5,
    Unauthorized             = 6,
    PayloadTooBig            = 10,
    IncompleteFrame          = 11,
    InvalidRsvBits           = 12,
    InvalidOpcode            = 13,
    UnmaskedClientFrame      = 14,
};
```

#### Fragmentation and partial frames

`WebSocketFramer` reassembles fragmented messages (RFC 6455 Section 5.4) and buffers partial frames that span TCP segments. The `Payload` signal always delivers complete, reassembled application payloads; the application never sees raw frames or partial data.

Client-side framers mask outgoing payloads as required by RFC 6455. Server-side framers do not mask. This is handled automatically based on the `Mode` passed to `ConnectionAdapter`.

### URL Parsing

`http::URL` implements RFC 3986 parsing. Constructors and assignment operators throw `std::runtime_error` on an invalid URL. Pass `whiny = false` to `parse()` to return `false` instead.

```cpp
#include "icy/http/url.h"

http::URL url("https://user:pass@api.example.com:8443/v1/search?q=hello#top");

url.scheme();    // "https"
url.userInfo();  // "user:pass"
url.host();      // "api.example.com"
url.port();      // 8443  (scheme default 443 if no explicit port)
url.hasPort();   // true  (explicit port present)
url.path();      // "/v1/search"
url.query();     // "q=hello"
url.fragment();  // "top"
url.authority(); // "user:pass@api.example.com:8443"
url.pathEtc();   // "/v1/search?q=hello#top"
url.str();       // original string
url.valid();     // true
```

Build from components:

```cpp
// scheme + authority
http::URL url("https", "api.example.com:8443");

// scheme + authority + pathEtc
http::URL url("https", "api.example.com", "/v1/items?limit=10");

// scheme + authority + path + query + fragment
http::URL url("https", "api.example.com:8443", "/v1/search", "q=hello", "top");
```

Percent-encoding helpers equivalent to JavaScript's `encodeURIComponent` / `decodeURIComponent`:

```cpp
std::string enc = http::URL::encode("hello world & more");
// "hello%20world%20%26%20more"

std::string dec = http::URL::decode("hello%20world");
// "hello world"
```

Silent parse (no exception):

```cpp
http::URL url;
if (!url.parse("not a url", false)) {
    // handle invalid input
}
```

### Forms and File Upload

`http::FormWriter` handles both `application/x-www-form-urlencoded` and `multipart/form-data`. The write loop runs in a background thread so large file uploads do not block the event loop. The caller retains the thread and the connection on the main loop.

`FormWriter` is created via a factory method. It must be wrapped in a `ConnectionStream` to integrate with the `PacketStream` pipeline.

#### URL-encoded form

```cpp
#include "icy/http/form.h"
#include "icy/http/client.h"

using namespace icy;

auto conn = http::createConnectionT<http::ClientConnection>(
    http::URL("http://api.example.com/submit"));
http::ConnectionStream stream(conn);

auto* form = http::FormWriter::create(stream, http::FormWriter::ENCODING_URL);
form->set("username", "alice");
form->set("message", "hello world");

form->prepareSubmit(); // sets Content-Type and Content-Length on the request
form->start();         // starts the background writer thread
conn->send();
```

#### Multipart form with file upload

```cpp
auto* form = http::FormWriter::create(stream,
                                      http::FormWriter::ENCODING_MULTIPART_FORM);

// Text field
form->set("description", "profile photo");

// File attachment; FormWriter takes ownership of FilePart
form->addPart("avatar",
    new http::FilePart("/home/alice/photo.jpg", "image/jpeg"));

// In-memory binary data as a part
form->addPart("thumbnail",
    new http::StringPart(thumbnailBytes, "image/jpeg"));

form->prepareSubmit();
form->start();
conn->send();
```

`FilePart` streams the file in chunks. `StringPart` sends an in-memory buffer in a single pass. Upload progress is available via `ConnectionStream::OutgoingProgress`.

Custom parts: subclass `FormPart` and implement `writeChunk()`, `write()`, and `length()`.

**Encoding constants:**

```cpp
http::FormWriter::ENCODING_URL;               // "application/x-www-form-urlencoded"
http::FormWriter::ENCODING_MULTIPART_FORM;    // "multipart/form-data"
http::FormWriter::ENCODING_MULTIPART_RELATED; // "multipart/related" (RFC 2387)
```

### Authentication

#### Basic authentication

Client side: add `Authorization: Basic ...` to an outgoing request.

```cpp
#include "icy/http/authenticator.h"

http::BasicAuthenticator auth("alice", "s3cr3t");
auth.authenticate(conn->request());
conn->send();
```

Server side: extract credentials from an incoming request.

```cpp
http::BasicAuthenticator auth(conn->request());
std::string user = auth.username();
std::string pass = auth.password();
```

Proxy authentication uses `proxyAuthenticate()`, which sets `Proxy-Authorization` instead.

#### Basic and Digest (automatic)

`http::Authenticator` inspects the `WWW-Authenticate` header from a 401 response and constructs the correct `Authorization` header for the retry, handling Basic and Digest transparently.

```cpp
http::Authenticator auth("alice", "s3cr3t");

// Load credentials from a URL's user-info component
auth.fromURI(http::URL("http://alice:s3cr3t@api.example.com/"));

conn->Complete += [&](const http::Response& res) {
    if (res.getStatus() == http::StatusCode::Unauthorized) {
        // Inspect WWW-Authenticate, populate Authorization on the request
        auth.authenticate(conn->request(), res);
        // Re-establish the connection and retry
    }
};
```

For subsequent requests on the same connection (Digest with nonce counting):

```cpp
auth.updateAuthInfo(conn->request());
```

Proxy variants:

```cpp
auth.proxyAuthenticate(conn->request(), res);  // uses Proxy-Authenticate / Proxy-Authorization
auth.updateProxyAuthInfo(conn->request());
```

#### Credential inspection helpers

```cpp
// Header value inspection
bool http::isBasicCredentials(std::string_view header);
bool http::isDigestCredentials(std::string_view header);

// Request inspection
bool http::hasBasicCredentials(const http::Request& req);
bool http::hasDigestCredentials(const http::Request& req);
bool http::hasProxyBasicCredentials(const http::Request& req);
bool http::hasProxyDigestCredentials(const http::Request& req);

// Extract from "user:password" string
std::string user, pass;
http::extractCredentials("alice:s3cr3t", user, pass);

// Extract from URL user-info
http::extractCredentials(http::URL("http://alice:s3cr3t@host/"), user, pass);
```

### Cookies

`http::Cookie` supports Netscape v0 (default; maximum interoperability) and RFC 2109 v1 formats.

```cpp
#include "icy/http/cookie.h"

// Server: set a cookie in a response
http::Cookie session("sessionid", "abc123");
session.setPath("/");
session.setMaxAge(3600);   // expires after 1 hour; -1 = never, 0 = delete
session.setHttpOnly(true); // not accessible to JavaScript
session.setSecure(true);   // HTTPS only
session.setDomain(".example.com");
conn->response().addCookie(session);

// Server: read cookies from an incoming request
NVCollection jar;
conn->request().getCookies(jar);
std::string sid = jar.get("sessionid", "");

// Client: send cookies with a request
NVCollection jar;
jar.set("sessionid", "abc123");
jar.set("csrf", "xyz");
conn->request().setCookies(jar);

// Read cookies from a response
std::vector<http::Cookie> cookies;
conn->response().getCookies(cookies);
for (auto& c : cookies)
    std::cout << c.getName() << " = " << c.getValue() << '\n';
```

Escape values that contain whitespace or non-alphanumeric characters before storing them:

```cpp
std::string safe = http::Cookie::escape("hello world"); // "hello%20world"
std::string raw  = http::Cookie::unescape("hello%20world"); // "hello world"
```

Cookie version 1 (RFC 2109) adds a `Comment` attribute. Set `setVersion(1)` to opt in.

### Streaming (Chunked and Multipart)

Two adapters in `icy/http/packetizers.h` integrate HTTP with Icey's `PacketStream` pipeline. Both implement `IPacketizer` and can be chained as processors between a media source and the network socket.

#### ChunkedAdapter

Wraps outgoing packets in HTTP chunked transfer encoding. On the first packet it emits the `200 OK` response header with `Transfer-Encoding: chunked` and `Cache-Control: no-store`. Subsequent packets become `<hex-length>\r\n<data>\r\n` chunks.

```cpp
#include "icy/http/packetizers.h"

srv.Connection += [](http::ServerConnection::Ptr conn) {
    conn->response().setContentType("video/mp4");

    auto adapter = std::make_shared<http::ChunkedAdapter>(conn);
    // nocopy mode (default true): emits chunk header, optional separator,
    // and payload as three separate packets to avoid memcpy.

    PacketStream stream;
    stream.attachSource(videoSource.get());
    stream.attach(adapter.get(), 5);
    stream.attach(conn.get(), 10);
    stream.start();
};
```

Use `nocopy = false` if you need a single contiguous buffer per chunk:

```cpp
auto adapter = std::make_shared<http::ChunkedAdapter>(conn, "", false);
```

With a `frameSeparator`, the separator string is inserted before each payload (useful for separating JPEG frames inside a chunked stream):

```cpp
auto adapter = std::make_shared<http::ChunkedAdapter>(conn, "\r\n--boundary\r\n");
```

Without a `Connection` object (standalone response generation):

```cpp
auto adapter = std::make_shared<http::ChunkedAdapter>("video/mp4");
```

#### MultipartAdapter

Streams data as `multipart/x-mixed-replace`, the format browsers use for MJPEG camera streams. Each packet becomes a MIME part delimited by `--end`, with `Content-Type` headers. Browsers update `<img>` elements for each part without JavaScript.

```cpp
srv.Connection += [](http::ServerConnection::Ptr conn) {
    conn->response().setContentType("image/jpeg");

    auto adapter = std::make_shared<http::MultipartAdapter>(conn);

    PacketStream stream;
    stream.attachSource(cameraCapture.get());
    stream.attach(adapter.get(), 5);
    stream.start();
};
```

For base64-encoded parts (e.g. embedding binary data in a text protocol):

```cpp
auto adapter = std::make_shared<http::MultipartAdapter>(conn, /*base64=*/true);
```

Without a `Connection` object:

```cpp
auto adapter = std::make_shared<http::MultipartAdapter>("image/jpeg");
```

### Performance

Icey's HTTP server is built on the same libuv async I/O and llhttp parser that power Node.js, without the JavaScript runtime, garbage collector, or language bridge. All three servers in the benchmark below share the same underlying async I/O and parsing foundation; the difference is pure runtime overhead.

Benchmarked on a single-core micro VM using [wrk](https://github.com/wg/wrk) (`wrk -t4 -c400 -d30s http://127.0.0.1:1337/`) against the `httpbench` benchmark:

| Server | Req/sec | Latency | Notes |
| ------ | ------: | ------: | ----- |
| Raw libuv + llhttp | 96,088 | 1.04 ms | Theoretical ceiling; fixed buffers, no abstraction |
| **Icey (keep-alive)** | **72,209** | **1.43 ms** | Full stack with pooling, keep-alive, date cache |
| Go 1.25 net/http | 53,878 | 2.31 ms | |
| Node.js v20 | 45,514 | 3.56 ms | |

Icey delivers 75% of raw libuv throughput while providing the full stack. It outperforms Go `net/http` by 34% and Node.js by 59%.

**What drives performance:**

**Connection pooling.** `ConnectionPool` is a LIFO pool of `ServerConnection` objects. On close, the connection is `reset()` and returned to the pool; the next accepted socket reuses the object without a heap allocation. Default capacity is 128.

**Date header cache.** `DateCache` formats the `Date:` header string once per second and reuses it across all responses in that second, eliminating `strftime` on the hot path.

**No-copy chunked send.** `ChunkedAdapter` in `nocopy` mode emits the chunk length header, the optional separator, and the payload as three separate packets. The payload buffer is never copied; it is passed through as-is.

**Keep-alive.** HTTP/1.1 keep-alive is on by default. Keeping connections alive across requests eliminates TCP handshake overhead, which is the single largest source of latency at high request rates.

**`SO_REUSEPORT` multicore.** Spawn one server instance per CPU core on the same address and port. The kernel distributes incoming connections across instances with no user-space distributor, no mutex, no shared state.

**`UV_THREADPOOL_SIZE`.** Match libuv's thread pool to CPU core count before entering the event loop for maximum async file I/O throughput:

```cpp
#ifdef ICY_UNIX
setenv("UV_THREADPOOL_SIZE",
       std::to_string(std::thread::hardware_concurrency()).c_str(), 1);
#endif
```

**Running the benchmark:**

```bash
# Build in release mode with logging disabled
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)

# Single-core with keep-alive
./build/src/http/bench/httpbench keepalive

# Multicore with SO_REUSEPORT
./build/src/http/bench/httpbench multi

# Raw libuv+llhttp ceiling
./build/src/http/bench/httpbench raw-keepalive
```

Then in another terminal:

```bash
wrk -t4 -c400 -d30s http://127.0.0.1:1337/
```

Available modes: `single`, `keepalive`, `multi`, `echo`, `raw`, `raw-keepalive`.

## Samples

### [httpechoserver](../../src/http/samples/httpechoserver/)

An HTTP/HTTPS echo server that sends the request body back as the response body. Demonstrates plain HTTP, HTTPS with `SSLSocket`, and multicore `SO_REUSEPORT` variants in a single sample.

The plain HTTP variant:

```cpp
#include "icy/http/server.h"

using namespace icy;

http::Server srv(net::Address("0.0.0.0", 1337));
srv.start();

srv.Connection += [](http::ServerConnection::Ptr conn) {
    conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buf) {
        conn.send(bufferCast<const char*>(buf), buf.size());
        conn.close();
    };
};

waitForShutdown();
```

The HTTPS variant passes an `SSLSocket`:

```cpp
net::SSLManager::initNoVerifyServer(argv[1], argv[2]); // key.pem cert.pem

http::Server srv(
    address,
    std::make_shared<net::SSLSocket>(
        net::SSLManager::instance().defaultServerContext()));
srv.start();
// ... same Connection lambda
```

The multicore variant gives each thread its own loop and calls `setReusePort()`:

```cpp
void raiseMulticoreEchoServer() {
    uv::ScopedLoop loop;
    http::Server srv(address, net::makeSocket<net::TCPSocket>(loop));
    srv.setReusePort();
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buf) {
            conn.send(bufferCast<const char*>(buf), buf.size());
            conn.close();
        };
    };

    waitForShutdown([&](void*) { srv.shutdown(); }, nullptr, loop);
}
```

Usage: `httpechoserver <key.pem> <cert.pem>`

### [httpclient](../../src/http/samples/httpclient/)

A minimal HTTP GET client that prints response status, headers, and body. Demonstrates `createConnectionT`, signal-based response handling, and event loop management.

```cpp
#include "icy/http/client.h"

using namespace icy;

std::string url = "http://example.com";
auto conn = http::createConnectionT<http::ClientConnection>(http::URL(url));

conn->Headers += [](http::Response& res) {
    std::cout << "Status: " << static_cast<int>(res.getStatus())
              << " " << res.getReason() << '\n';
    for (const auto& [name, value] : res)
        std::cout << "  " << name << ": " << value << '\n';
};

conn->Payload += [](const MutableBuffer& buf) {
    std::cout.write(bufferCast<const char*>(buf), buf.size());
};

conn->Complete += [](const http::Response&) {
    std::cout << "Request complete.\n";
};

conn->Close += [](http::Connection&) {
    uv_stop(uv::defaultLoop());
};

conn->send();
uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);
```

Usage: `httpclient [url]` (default: `http://example.com`)

### [wsclient](../../src/http/samples/wsclient/)

A WebSocket client that connects to an echo server, sends a message, and prints the response. Demonstrates `ws://` scheme handling, the `Connect` signal timing (fires after the WebSocket handshake, not just TCP connect), and graceful shutdown via `waitForShutdown`.

```cpp
#include "icy/http/client.h"
#include "icy/http/websocket.h"

using namespace icy;

std::string url     = "ws://echo.websocket.events";
std::string message = "Hello from Icey!";

auto conn = http::createConnectionT<http::ClientConnection>(http::URL(url));

conn->Connect += [&]() {
    std::cout << "Connected. Sending: " << message << '\n';
    conn->send(message.c_str(), message.size(), http::ws::Text);
};

conn->Payload += [](const MutableBuffer& buf) {
    std::string data(bufferCast<const char*>(buf), buf.size());
    std::cout << "Received: " << data << '\n';
};

conn->Close += [](http::Connection&) {
    uv_stop(uv::defaultLoop());
};

conn->send(); // TCP connect + WebSocket handshake

waitForShutdown([&](void*) { conn->close(); });
```

To test locally, run `httpechoserver` first and connect to `ws://localhost:1337`.

Usage: `wsclient [url] [message]`

### [httpbench](../../src/http/bench/)

The benchmark used to produce the performance table above. Includes six modes covering single-core, keep-alive, multicore `SO_REUSEPORT`, echo, and a raw libuv+llhttp baseline with zero-copy fixed buffers and no heap allocation per connection (the theoretical ceiling).

Usage: `httpbench [single|keepalive|multi|echo|raw|raw-keepalive]`

## Configuration

### CMake integration

Link against `Icey::base`, `Icey::net`, and `Icey::http`:

```cmake
target_link_libraries(myapp PRIVATE Icey::base Icey::net Icey::http)
```

With FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/sourcey/icey.git
  GIT_TAG v2.3.0
)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE Icey::base Icey::net Icey::http)
```

After installing (`cmake --install build`):

```cmake
find_package(Icey REQUIRED)
target_link_libraries(myapp PRIVATE Icey::base Icey::net Icey::http)
```

### Compiler requirements

| Platform | Compiler |
| -------- | -------- |
| Linux | GCC 12+ or Clang 15+ |
| macOS | AppleClang 15+ (Xcode 15+) |
| Windows | MSVC 2022 (Visual Studio 17+) |

C++20 required. CMake 3.21+.

### Build flags

| Flag | Default | Effect |
| ---- | ------- | ------ |
| `CMAKE_BUILD_TYPE=Release` | Debug | Enables -O2/-O3; required for benchmark numbers |
| `ICY_ENABLE_LOGGING` (compile def) | defined | Undefine to strip all `LTrace`/`LDebug` calls; measurable throughput gain |
| `ICY_SHARED_LIBRARY` | off | Build as shared library; exports via `HTTP_API` |

### Runtime tuning

Match libuv thread pool size to CPU cores before the event loop starts (improves async file I/O throughput for form uploads and file streaming):

```cpp
#ifdef ICY_UNIX
setenv("UV_THREADPOOL_SIZE",
       std::to_string(std::thread::hardware_concurrency()).c_str(), 1);
#endif
uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);
```

## See Also

- [Net](net.md) — `TCPSocket`, `SSLSocket`, `SSLManager`, `net::Address`
- [Base](base.md) — `PacketStream`, `Signal`, `LocalSignal`, `uv::Loop`, `Timer`, `Thread`
- [Symple](symple.md) — WebSocket-based real-time messaging and WebRTC call signalling
- [AV](av.md) — FFmpeg capture, encode, and `PacketStream` sources for streaming over HTTP
