# Modules

LibSourcey is organised into 14 modules. Each module builds as a separate library and declares its dependencies via CMake.

## Dependency Diagram

```
                        symple
                       /  |   \
                socketio  |    \
               / |  \  \  |    \
              /  |   \  json    \
             /   |    \  |       \
            /    |     http     sched
           /     |    / |  \
          /      |   /  |   \
         /       net    |    \
         \      / \     |    /
          \    /   \    |   /
           crypto   \   |  /
              \      \  | /
               \      base
                \      |
                 \    libuv (vendored)
                  \
            openssl (system)

            archo     pluga     av
              |         |       |
             base      base    base (+FFmpeg)

          stun ---- turn
           |       / |
           net    /  |
           |     /   |
          crypto    net
           |
          base

          pacm
         / | \ \
       net json http archo
        |   |    |     |
       ...  |   ...   base
           base
```

## Module Overview

| Module | Description | Dependencies |
|--------|-------------|--------------|
| **base** | Core platform abstractions, event loop (libuv), signals, streams, logging, filesystem, process, timers, rate limiter, stream manager, timed manager | libuv (vendored) |
| **crypto** | Cryptographic operations - hashing, HMAC, RSA, X509 certificates | base, OpenSSL |
| **net** | TCP/SSL/UDP networking, socket adapters, DNS resolution | base, crypto |
| **http** | HTTP client/server, WebSocket support, form handling, cookies, URL parsing | base, net, crypto |
| **json** | JSON serialization using nlohmann/json | base |
| **av** | Audio/video encoding, decoding, device capture, media formats | base, FFmpeg (optional) |
| **socketio** | Socket.IO protocol implementation | base, crypto, net, http, json |
| **symple** | Realtime messaging protocol for presence and messaging | base, crypto, net, http, socketio, json |
| **stun** | STUN protocol (RFC 5389) for NAT traversal | base, net, crypto |
| **turn** | TURN relay server (RFC 5766) for media relaying | base, net, stun, crypto |
| **pacm** | Package manager for distributing and installing plugins | base, net, json, http, archo, crypto |
| **archo** | Archive and ZIP handling | base |
| **pluga** | Plugin system for shared library loading | base |
| **sched** | Task scheduler for deferred and periodic jobs | base, json |
