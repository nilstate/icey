# icey Documentation

icey exists because the usual C++ stack for realtime and media work is a mess. HTTP on one side, FFmpeg on another, WebRTC hidden behind Google's toolchain, TURN treated as somebody else's problem, and the docs scattered across a README, generated API, and sample code.

icey pulls those layers into one library with one runtime model. libuv for the loop, PacketStream for the data plane, FFmpeg for media, libdatachannel for WebRTC transport, Symple for signalling, TURN when NAT gets ugly.

If you are here to do a specific job, start there first.

## Start With The Job

| Goal | Best first page | Next concrete stop |
| --- | --- | --- |
| Build icey and link a first program | [Getting Started](getting-started.md) | [Installation](installation.md) |
| Understand the runtime rules before writing code | [Runtime Contracts](concepts/runtime-contracts.md) | [PacketStream](concepts/packetstream.md) and [HTTP Lifecycle](concepts/http-lifecycle.md) |
| Understand the module layout | [Module Map](modules.md) | one of the module guides below |
| Build a fast HTTP service | [HTTP Server](recipes/http-server.md) | [HTTP performance harness](../src/http/perf/README.md) |
| Build a WebSocket client or upgraded server | [WebSocket Client And Server](recipes/websocket-client-server.md) | [HTTP Lifecycle](concepts/http-lifecycle.md) |
| Run your own TURN relay | [TURN Server](recipes/turn-server.md) | [turnserver sample](../src/turn/samples/turnserver/README.md) |
| Stream a webcam to a browser | [Webcam To Browser](recipes/webrtc-webcam-to-browser.md) | [webcam-streamer sample](../src/webrtc/samples/webcam-streamer/README.md) |
| Record browser media on the server | [Browser To Recorder](recipes/webrtc-browser-to-recorder.md) | [media-recorder sample](../src/webrtc/samples/media-recorder/README.md) |
| Run the self-hosted media stack | [Media Server Demo](../src/webrtc/apps/media-server/docker/README.md) | [Media Server Stack](recipes/media-server-stack.md) |

## What These Pages Are For

These docs and the generated API reference are not trying to do the same thing.

- The pages here explain architecture, runtime rules, workflows, and operations.
- The `API Reference` tab is the canonical generated reference surface in the site.
- The `src/*/README.md` files stay as the exact local docs for samples, apps, benchmarks, and important source directories.

Use the prose docs when you need to understand the shape of the system. Use the API tab when you need the exact surface of a type.

## The Shape Of The System

icey is organized around a few load-bearing modules:

- [`base`](modules/base.md): event loop, signals, PacketStream, handles, timers, logging, buffers, and the rest of the runtime substrate
- [`net`](modules/net.md) and [`http`](modules/http.md): sockets, TLS, HTTP client/server, WebSocket, streaming, and transport adapters
- [`av`](modules/av.md) and [`webrtc`](modules/webrtc.md): capture, encode/decode, RTP track send/receive, session control, and browser media flows
- [`symple`](modules/symple.md), [`stun`](modules/stun.md), and [`turn`](modules/turn.md): signalling, presence, NAT traversal, and relay

A lot of icey reduces to this:

```text
source -> PacketStream -> processors -> sink

camera/file/network -> encode/decode/transform -> socket / recorder / WebRTC track
```

That shape keeps showing up across the library:

- HTTP payloads and upgraded WebSocket connections run on the same base/net runtime
- TURN and STUN build on the same socket, buffer, and crypto foundations
- WebRTC media send/receive uses PacketStream to connect capture, encode/decode, and track transport

## Where To Go Next

- Start with [Getting Started](getting-started.md) if you need a buildable first program quickly.
- Use [Module Map](modules.md) if you want to understand where a feature lives before diving into code.
- Go straight to a module guide if you already know the subsystem you need.
- Use the generated API reference when you need type/member detail rather than design guidance.

## Good First Stops

- [Base](modules/base.md) if you need to understand runtime contracts, loops, signals, or PacketStream.
- [Runtime Contracts](concepts/runtime-contracts.md) if you want the rules without reading three module guides to find them.
- [PacketStream](concepts/packetstream.md) if you are working on capture, encode/decode, or transport pipelines.
- [HTTP Lifecycle](concepts/http-lifecycle.md) if you care about keep-alive reuse, streaming, or WebSocket upgrade flow.
- [HTTP](modules/http.md) if you care about throughput, keep-alive, WebSocket upgrade, or streaming.
- [WebSocket Client And Server](recipes/websocket-client-server.md) if the handshake and frame path is the thing you are actually building.
- [WebRTC](modules/webrtc.md) if you are building browser media flows.
- [TURN](modules/turn.md) if you need self-hosted relay for difficult NATs.
- [Symple](modules/symple.md) if you need signalling, presence, rooms, or call control transport.

## Repository Context

If you are reading the docs inside the repository, these are also useful:

- [Repository README](../README.md)
- [Contributing](contributing.md)
- [Releasing](releasing.md)
- [Change log](../CHANGELOG.md)
