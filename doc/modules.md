# Modules

Icey is organised into 14 modules. Each module builds as a separate library and declares its dependencies via CMake. Enable only what you need; dependencies resolve automatically.

## Dependency Diagram

```text
                        symple
                       /  |   \
                      /   |    \
                     /   json    \
                    /     |       \
                   /     http     sched
                  /     / |  \
                 /     /  |   \
                /    net   |    \
                \   / \    |    /
                 \ /   \   |   /
                crypto  \  |  /
                  \      \ | /
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

          webrtc
         / | \ \ \
       base net crypto av json
        |                |
       libuv            base
      (+libdatachannel, OpenSSL 3.x, FFmpeg)

          pacm
         / | \ \
       net json http archo
        |   |    |     |
       ...  |   ...   base
           base
```

## Module Overview

| Module | Description | Dependencies |
| ------ | ----------- | ------------ |
| **[base](modules/base.md)** | Event loop (libuv), signals, PacketStream pipeline, logging, filesystem, timers | libuv (vendored) |
| **[crypto](modules/crypto.md)** | Hashing, HMAC, symmetric ciphers, X.509 certificates, RSA | base, OpenSSL |
| **[net](modules/net.md)** | TCP/SSL/UDP sockets, chainable adapters, async DNS | base, crypto |
| **[http](modules/http.md)** | HTTP client/server, WebSocket, forms, cookies, streaming | base, net, crypto |
| **[json](modules/json.md)** | JSON serialization (nlohmann/json), configuration | base |
| **[av](modules/av.md)** | FFmpeg capture, encode, decode, device management | base, FFmpeg (optional) |
| **[webrtc](modules/webrtc.md)** | WebRTC media transport via libdatachannel | base, net, crypto, av, json, libdatachannel, OpenSSL 3.x, FFmpeg 5+ |
| **[symple](modules/symple.md)** | Real-time messaging, presence, rooms, WebRTC signalling | base, crypto, net, http, json |
| **[stun](modules/stun.md)** | RFC 5389 STUN for NAT traversal | base, net, crypto |
| **[turn](modules/turn.md)** | RFC 5766 TURN relay server and client | base, net, stun, crypto |
| **[archo](modules/archo.md)** | ZIP archive extraction | base |
| **[pacm](modules/pacm.md)** | Package manager for plugin distribution | base, net, json, http, archo, crypto |
| **[pluga](modules/pluga.md)** | Plugin system for shared library loading | base |
| **[sched](modules/sched.md)** | Task scheduler for deferred and periodic jobs | base, json |
