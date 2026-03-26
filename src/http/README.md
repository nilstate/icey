# HTTP Module

Async HTTP/1.1 client and server support, WebSocket upgrade, streaming responders, forms, cookies, and authentication.

- Namespace: `icy::http`
- CMake target: `icey::http`
- Primary headers: `include/icy/http/server.h`, `client.h`, `websocket.h`, `url.h`
- Directory layout: `include/` for the public API, `src/` for parser and connection internals, `samples/` for runnable demos, `tests/` for request/response and upgrade coverage
- Performance harnesses live under `perf/` and `bench/`

Read next:
- [HTTP module guide](../../docs/modules/http.md)
- [HTTP server recipe](../../docs/recipes/http-server.md)
- [WebSocket client and server recipe](../../docs/recipes/websocket-client-server.md)
- [HTTP lifecycle concept guide](../../docs/concepts/http-lifecycle.md)
