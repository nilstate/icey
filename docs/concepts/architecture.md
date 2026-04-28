# Architecture

`icey-server` is not a monolith. It is a thin assembly layer over independent icey library modules. Every piece of the server — HTTP, signalling, media transport, relay, encoding — is a module you can use on its own.

This page explains how the server maps to the library. If you ran `icey-server` and want to understand what is underneath, start here.

## The Stack

```text
Browser ─── HTTP GET /     ──── Static files (web UI)
        ─── HTTP GET /api  ──── REST status endpoints
        ─── WSS /ws        ──── Symple signalling + presence
        ─── WebRTC         ──── Media (H.264 + Opus)
        ─── TURN           ──── NAT traversal relay (port 3478)
```

Everything on the right side is an icey library module:

```text
icey-server
├── http     → serves UI, REST endpoints, WebSocket upgrade
├── symple   → signalling, peer presence, rooms, call control
├── webrtc   → session negotiation, RTP track send/receive
├── turn     → RFC 5766 relay for NAT traversal
├── av       → FFmpeg capture, H.264/Opus encode/decode, MP4 mux
├── vision   → decoded frame sampling, motion detection events
├── speech   → decoded audio VAD, speech activity events
├── stun     → STUN message parsing (used by TURN)
├── net      → TCP, SSL/TLS, UDP sockets
├── crypto   → OpenSSL hashing, HMAC, certificates
├── json     → configuration, signalling payloads
└── base     → event loop (libuv), signals, PacketStream, logging
```

The server's `main()` wires these together. The modules do the work.

## Module Map

:::card-group{cols="3"}

::card{title="base" href="../modules/base"}
Event loop, signals, PacketStream pipeline, logging, timers. The runtime substrate everything else sits on.
::

::card{title="net" href="../modules/net"}
TCP, SSL/TLS, UDP sockets, async DNS. The transport layer.
::

::card{title="http" href="../modules/http"}
HTTP server/client, WebSocket, streaming, keep-alive. 72k req/s on a single core.
::

::card{title="av" href="../modules/av"}
FFmpeg capture, encode, decode, mux. Handles file, camera, and RTSP sources.
::

::card{title="webrtc" href="../modules/webrtc"}
Media sessions via libdatachannel. Track send/receive, codec negotiation, REMB-adaptive bitrate.
::

::card{title="symple" href="../modules/symple"}
WebSocket signalling protocol. Peer presence, rooms, call control. The coordination layer.
::

::card{title="turn" href="../modules/turn"}
RFC 5766 TURN relay. UDP and TCP. Handles the hard NAT cases.
::

::card{title="crypto" href="../modules/crypto"}
OpenSSL 3.x. Hashing, HMAC, symmetric ciphers, X.509 certificates, RSA.
::

::card{title="vision" href="../modules/vision"}
Sampled decoded frame analysis. Motion detection, event emission. Side pipeline, does not touch transport.
::

::card{title="speech" href="../modules/speech"}
Decoded audio analysis. Voice activity detection, speech events. Side pipeline, does not touch transport.
::

:::

:::tip
Every module above is independently usable. You do not need the server to use the library. `icey::http` is a standalone HTTP server. `icey::turn` is a standalone TURN relay. `icey::av` is a standalone FFmpeg wrapper. The server is one assembly — your application can be a different one.
:::

## Data Flow

Everything flows through `PacketStream`. Plug in a source, chain processors, attach a sink. Borrowed packets stay zero-copy until the first queue or retained adapter; that boundary is explicit in the graph. The pipeline handles backpressure, frame dropping, and teardown so you don't. Nothing runs that you didn't ask for.

```text
┌─────────────────────────────────────────────────────────────────┐
│                        PacketStream                             │
│                                                                 │
│  ┌──────────┐    ┌──────────────┐    ┌───────────────────────┐  │
│  │  Source  │───▶│  Processor   │───▶│        Sink           │  │
│  │          │    │              │    │                       │  │
│  │ Camera   │    │ FFmpeg H.264 │    │ WebRTC Track Sender   │  │
│  │ File     │    │ Opus encode  │    │ Network socket        │  │
│  │ Network  │    │ OpenCV       │    │ File recorder         │  │
│  │ Device   │    │ Custom       │    │ HTTP response         │  │
│  └──────────┘    └──────────────┘    └───────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘

WebRTC send path:
  MediaCapture → VideoEncoder → WebRtcTrackSender → [libdatachannel]
                                                        │
  Browser ◀── RTP/SRTP ◀── DTLS ◀── ICE (libjuice) ◀───┘
                                      │
                              icey TURN server
                              (relay for symmetric NATs)

WebRTC receive path:
  [libdatachannel] → WebRtcTrackReceiver → FFmpeg decode → file/display
        │
        └─── ICE → DTLS → SRTP decrypt → RTP depacketise → raw frames

Signalling (Symple v4):
  C++ server/client ◀──── WebSocket ────▶ Browser (symple-player)
  Auth, presence, rooms, call protocol (init/accept/offer/answer/candidate)
```

Camera to browser in 150 lines. Browser to file in 130. The pipeline handles the plumbing.

In `icey-server`, the pipeline takes a different shape per mode:

```text
Stream:  MediaCapture → VideoPacketEncoder → WebRtcTrackSender → browser
                       → AudioPacketEncoder → WebRtcTrackSender → browser

Record:  browser → WebRtcTrackReceiver → VideoDecoder → MultiplexPacketEncoder → MP4

Relay:   browser (source) → WebRtcTrackReceiver → WebRtcTrackSender → browser (viewers)
```

Decoded branches can feed `vision` and `speech` processors without changing the transport path.

The [PacketStream concept page](packetstream.md) explains ownership rules and retention boundaries. The [WebRTC session flow](webrtc-session-flow.md) explains session negotiation and control.

## Why icey

| | libWebRTC (Google) | libdatachannel | GStreamer | **icey** |
| --- | --- | --- | --- | --- |
| Build system | GN/Ninja | CMake | Meson | **CMake** |
| Build time | Hours | Minutes | 30+ min | **Minutes** |
| Binary size | 50MB+ | Small | Large | **Small** |
| SSL | BoringSSL (conflicts) | OpenSSL | OpenSSL | **OpenSSL** |
| Media codecs | Bundled | None | GObject plugins | **FFmpeg (any codec)** |
| Capture/encode | Included | No | Plugin pipeline | **PacketStream pipeline** |
| Signalling | No | No | No | **Symple (built-in)** |
| TURN server | No | No | No | **RFC 5766 (built-in)** |
| Language | C++ | C++17 | C/GObject | **C++20** |

libdatachannel gives you the WebRTC transport pipe. icey gives you the pipe, the water, and the faucet.

### HTTP performance

The same runtime model that handles media also handles HTTP. On a single-core micro VM:

| Server | Req/sec | Latency |
| --- | ---: | ---: |
| Raw libuv+llhttp | 96,088 | 1.04ms |
| **icey** | **72,209** | **1.43ms** |
| Go 1.25 net/http | 53,878 | 2.31ms |
| Node.js v20 | 45,514 | 3.56ms |

75% of raw libuv throughput with a complete HTTP stack. 34% faster than Go's `net/http`. All three share the same foundation (libuv for async IO, llhttp for parsing); the difference is pure runtime overhead.

## Why This Matters

Most media server projects are either:

- A library with good internals but no deployable product surface
- A deployable server where the internals are not designed for reuse

icey is both. The server proves the library works under real conditions. The library means you are not locked into the server's opinions about how to assemble the stack.

If the server does 90% of what you need, run it. If it does 60%, read the module guides and build your own assembly. The modules are the same either way.

## Next Steps

- [Runtime contracts](runtime-contracts.md) — the six rules that govern icey's runtime model
- [PacketStream](packetstream.md) — the pipeline architecture
- [HTTP Lifecycle](http-lifecycle.md) — connection state machine, keep-alive, streaming, WebSocket upgrade
- [Module map](../modules.md) — full dependency diagram and overview
- [HTTP Server recipe](../recipes/http-server.md) — use the library in your own C++ project
- [HTTP performance harness](https://github.com/nilstate/icey/tree/main/src/http/perf/) — cross-stack benchmark methodology and results
