# HTTP Server

Use this page when the job is simple:

- accept HTTP requests
- send responses fast
- stream when needed
- upgrade to WebSocket when needed

If you need the whole module surface, read the [HTTP guide](../modules/http.md). If you want the actual connection model first, read [HTTP Lifecycle](../concepts/http-lifecycle.md). This page is the shortest path from zero to a real server.

## The Shape

icey's HTTP server is one event loop, one parser path per live socket, pooled `ServerConnection` objects, and no thread-per-request model.

The normal flow is:

```text
accept -> parse headers -> optional body chunks -> request complete
      -> write response -> keep alive or close
```

For most servers, that means:

- create `http::Server`
- connect `srv.Connection`
- populate `Response`
- call `sendHeader()`
- call `send()`
- close or keep alive

## The Smallest Useful Server

```cpp
#include "icy/http/server.h"

using namespace icy;

http::Server srv("0.0.0.0", 1337);
srv.start();

srv.Connection += [](http::ServerConnection::Ptr conn) {
    std::string body = "hello\n";

    auto& res = conn->response();
    res.setStatus(http::StatusCode::OK);
    res.setContentType("text/plain");
    res.setContentLength(body.size());

    conn->sendHeader();
    conn->send(body.c_str(), body.size());
    conn->close();
};

uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);
```

That is already the real server path. It is not a toy wrapper around some other subsystem.

## Request Bodies

If the request has a body, handle it on the connection:

```cpp
srv.Connection += [](http::ServerConnection::Ptr conn) {
    conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buf) {
        conn.send(bufferCast<const char*>(buf), buf.size());
        conn.close();
    };
};
```

:::warning
`MutableBuffer` is borrowed for the callback only. Do not stash the pointer. If you need the bytes later, copy them or move them across a retained boundary immediately.
:::

## HTTPS

HTTPS is just the same server on an `SSLSocket`:

```cpp
#include "icy/net/sslmanager.h"
#include "icy/net/sslsocket.h"

net::SSLManager::initNoVerifyServer("key.pem", "cert.pem");

auto sslSocket = std::make_shared<net::SSLSocket>(
    net::SSLManager::instance().defaultServerContext());

http::Server srv("0.0.0.0", 443, sslSocket);
srv.start();
```

For real deployments, use a real server context and certificate chain. The sample above is for local bring-up.

## Structured Handlers

Once the lambda starts growing teeth, switch to a `ServerResponder`.

That gives you four clean hooks:

- `onHeaders()`
- `onPayload()`
- `onRequest()`
- `onClose()`

Use it when you need real request state, uploads, routing, or per-connection behavior that no longer fits in one callback.

## Streaming Responses

icey treats long-lived responses as a first-class state, not as a normal response that never got closed.

Use streaming when the server is intentionally going to keep writing:

- chunked event streams
- multipart replace streams
- media pipelines

The connection can enter streaming mode explicitly, and the server will stop treating it like an idle keep-alive socket.

That matters for correctness. A slow one-shot response and a real stream are not the same thing.

## WebSocket Upgrade

Upgrade is adapter replacement, not a second parallel server stack.

The normal server-side pattern is:

```cpp
srv.Connection += [](http::ServerConnection::Ptr conn) {
    if (conn->request().get("Upgrade") == "websocket") {
        conn->replaceAdapter(
            std::make_unique<http::ws::ConnectionAdapter>(
                conn.get(), http::ws::ServerSide));
        return;
    }

    conn->response().setStatus(http::StatusCode::OK);
    conn->sendHeader();
    conn->close();
};
```

After upgrade:

- the connection stays upgraded until close
- outgoing `send()` calls go through the WebSocket adapter
- incoming payload is frame payload, not HTTP body

If you care about the details there, read [HTTP Lifecycle](../concepts/http-lifecycle.md).

## Performance Notes

icey's HTTP server is fast for a reason:

- llhttp does the parse work
- the connection model stays explicit
- `send()` stays the zero-copy fast path
- keep-alive reuse resets state instead of rebuilding everything

If you want to compare it properly, the benchmark harness lives under:

- [`src/http/perf/`](https://github.com/nilstate/icey/tree/main/src/http/perf/)

The reportable microbenchmarks live under:

- [`src/http/bench/`](https://github.com/nilstate/icey/tree/main/src/http/bench/)

That is benchmark infrastructure, not a sample.

## Good Next Stops

- [HTTP guide](../modules/http.md) for the full module surface
- [HTTP Lifecycle](../concepts/http-lifecycle.md) for keep-alive, streaming, and upgrade rules
- [`httpechoserver`](https://github.com/nilstate/icey/tree/main/src/http/samples/httpechoserver/) for runnable sample code
- [`httpperf`](https://github.com/nilstate/icey/tree/main/src/http/perf/) for cross-stack throughput testing
- [`httpbench`](https://github.com/nilstate/icey/tree/main/src/http/bench/) for HTTP microbenchmarks
