# icey

The usual C++ stack for realtime and media work is a mess. HTTP on one side, FFmpeg on another, WebRTC hidden behind Google's monolith, TURN treated as somebody else's problem, signalling left as an exercise for the reader.

icey pulls those layers into one runtime. WebRTC, signalling, TURN relay, and media encoding in one C++ stack. Run it as a product. Embed it as a library. Same modules either way.

A small `PacketStream` graph is the core shape:

```cpp
PacketStream stream;
stream.attachSource(capture.get());
stream.attach(&session->media().videoSender(), 5);
stream.start();
```

If you're evaluating alternatives, the split is simple:

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

## See It Work

:::steps

1. Run the server

   ```bash
   docker run --rm --network host 0state/icey
   ```

2. Open `http://localhost:4500`

3. Click **Watch** on the `icey` peer

:::

Live video in your browser. One binary, no external services.

That binary — `icey-server` — is built entirely from icey's library modules. The same modules you can use in your own C++ application. The server proves the library works. The library means you are not locked into the server's opinions.

:::card-group{cols="3"}

::card{title="Run" icon="play" href="run/"}
Install and run icey-server. Docker, release binaries, package managers. Stream, record, or relay in under a minute.
::

::card{title="Build" icon="code" href="build/getting-started"}
Use icey modules in your own C++ project. FetchContent, CMake targets, recipes for HTTP, WebRTC, TURN, and more.
::

::card{title="Operate" icon="settings" href="operate/config"}
Deploy to production. Config reference, TLS, TURN, health endpoints, monitoring, and troubleshooting.
::

:::

## Start With The Job

| Goal | Start here |
| --- | --- |
| Run the media server now | [See It Work](run/) |
| Install icey-server | [Install](run/install) |
| Understand the three modes (stream, record, relay) | [Modes](run/modes) |
| Understand how the server maps to the library | [Architecture](concepts/architecture) |
| Build icey into my own C++ project | [Getting Started](build/getting-started) |
| Understand the runtime rules | [Runtime Contracts](concepts/runtime-contracts) |
| Understand the module layout | [Module Map](modules) |
| Build a fast HTTP service | [HTTP Server](recipes/http-server) |
| Stream a webcam to a browser | [Webcam To Browser](recipes/webrtc-webcam-to-browser) |
| Record browser media on the server | [Browser To Recorder](recipes/webrtc-browser-to-recorder) |
| Deploy to production | [Deploy](operate/deploy) |
| Configure TLS | [TLS](operate/tls) |
| Debug a TURN problem | [TURN Deployment](operate/turn) |

## Docs vs API Reference

The pages here explain architecture, runtime contracts, workflows, and operations. The **API Reference** tab is the generated C++ reference from Doxygen. Use the prose docs to understand the shape. Use the API tab for exact type signatures.

## Repository

- [README](https://github.com/nilstate/icey/blob/main/README.md)
- [Contributing](contributing.md)
- [Repository Conventions](conventions.md)
- [Releasing](releasing.md)
- [Changelog](https://github.com/nilstate/icey/blob/main/CHANGELOG.md)
