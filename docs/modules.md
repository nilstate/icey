# Modules

icey is organised into 16 modules. Each module builds as a separate library and declares its dependencies via CMake. Enable only what you need; dependencies resolve automatically.

This page is the map. If you want the quickest path to a task, use the table below and then jump into the module guide that actually owns the work.

## Common Starting Points

| Goal | Start here |
| ---- | ---------- |
| Runtime contracts, loop affinity, ownership rules | [concepts/runtime-contracts.md](concepts/runtime-contracts.md) |
| Packet pipelines, queue boundaries, media graphs | [concepts/packetstream.md](concepts/packetstream.md) |
| HTTP server or WebSocket service | [recipes/http-server.md](recipes/http-server.md) |
| Browser media send path | [recipes/webrtc-webcam-to-browser.md](recipes/webrtc-webcam-to-browser.md) |
| Browser media receive and recording | [recipes/webrtc-browser-to-recorder.md](recipes/webrtc-browser-to-recorder.md) |
| Decoded media intelligence branches | [modules/vision.md](modules/vision.md) and [modules/speech.md](modules/speech.md) |
| Full self-hosted media stack | [run/index.md](run/index.md) |
| Signalling, presence, and rooms | [modules/symple.md](modules/symple.md) |
| TURN relay and NAT traversal | [recipes/turn-server.md](recipes/turn-server.md) |
| Low-level STUN message work | [modules/stun.md](modules/stun.md) |
| Capture, encode, decode, muxing | [modules/av.md](modules/av.md) |

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

        speech   vision
           \      /
            \    /
             av json
              \ /
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
| **[speech](modules/speech.md)** | Audio intelligence primitives for decoded media streams | base, av, json |
| **[vision](modules/vision.md)** | Video intelligence primitives for sampled decoded frames | base, av, json |
| **[webrtc](modules/webrtc.md)** | WebRTC media transport via libdatachannel | base, net, crypto, av, json, libdatachannel, OpenSSL 3.x, FFmpeg 5+ |
| **[symple](modules/symple.md)** | Real-time messaging, presence, rooms, WebRTC signalling | base, crypto, net, http, json |
| **[stun](modules/stun.md)** | RFC 5389 STUN for NAT traversal | base, net, crypto |
| **[turn](modules/turn.md)** | RFC 5766 TURN relay server and client | base, net, stun, crypto |
| **[archo](modules/archo.md)** | ZIP archive extraction | base |
| **[pacm](modules/pacm.md)** | Package manager for plugin distribution | base, net, json, http, archo, crypto |
| **[pluga](modules/pluga.md)** | Plugin system for shared library loading | base |
| **[sched](modules/sched.md)** | Task scheduler for deferred and periodic jobs | base, json |

## Read This With The Sidebar

This page answers:

- what each module is for
- how the dependency graph is shaped
- where to start if you know the job but not the subsystem

The module guides answer the rest: architecture, contracts, usage, examples, and links into samples or apps. The concepts and workflow pages exist so you do not have to reverse-engineer that path from the module list every time.
