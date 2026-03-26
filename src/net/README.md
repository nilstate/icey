# Net Module

Async TCP, SSL/TLS, and UDP networking on libuv, with socket adapters, DNS resolution, and shared address utilities.

- Namespace: `icy::net`
- CMake target: `icey::net`
- Primary headers: `include/icy/net/socket.h`, `tcpsocket.h`, `sslsocket.h`, `udpsocket.h`
- Directory layout: `include/` for the public API, `src/` for transport implementations, `samples/` for runnable socket demos, `tests/` for transport coverage
- Higher-level modules such as `http`, `stun`, `turn`, and `symple` all build on this layer

Read next:
- [Net module guide](../../doc/modules/net.md)
- [Runtime contracts](../../doc/concepts/runtime-contracts.md)
