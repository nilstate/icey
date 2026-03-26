# icey

[![CI](https://github.com/nilstate/icey/actions/workflows/ci.yml/badge.svg)](https://github.com/nilstate/icey/actions/workflows/ci.yml)
[![License: LGPL-2.1+](https://img.shields.io/badge/license-LGPL--2.1%2B-blue.svg)](LICENSE.md)

> The C++ Media Stack

WebRTC, FFmpeg, and async networking in one toolkit. No Google monolith. No dependency hell. No fighting three build systems to get a frame on screen.

```cpp
// The core of a WebRTC media server
PacketStream stream;
stream.attachSource(capture.get());
stream.attach(&session->media().videoSender(), 5);
stream.start();
```

icey is the connective tissue: a modular C++20 toolkit that pulls FFmpeg, libuv, OpenSSL, llhttp, libdatachannel, Symple, STUN, and TURN into one runtime model. Capture, encode, transport, signalling, and relay. Core third-party code is pulled in by CMake; system TLS and media dependencies are auto-detected. Builds in minutes.

**[Documentation](docs/index.md)** | **[Changelog](CHANGELOG.md)** | **[Contributing](docs/contributing.md)** | **[LGPL-2.1+](LICENSE.md)**

## Fastest Path

If you want the shortest path from zero to browser video, use the published [Media Server Demo](src/webrtc/apps/media-server/docker/README.md) image.

One command. One URL. One click.

```bash
docker run --rm --network host 0state/icey-media-server-demo:latest
```

Then open `http://localhost:4500` and click `Watch` on the `Media Server` peer.

This express path targets Linux host networking. If you want the source-backed path for local edits, use `docker compose up --build` from [`src/webrtc/apps/media-server/docker/`](src/webrtc/apps/media-server/docker/).

## Why icey

| | libWebRTC (Google) | libdatachannel | GStreamer | **icey** |
|---|---|---|---|---|
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

## Architecture

Everything flows through `PacketStream`. Plug in a source, chain processors, attach a sink. Borrowed packets stay zero-copy until the first queue or retained adapter; that boundary is explicit in the graph. The pipeline handles backpressure, frame dropping, and teardown so you don't. Nothing runs that you didn't ask for.

```
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

## What You Can Build

### Stream a webcam to any browser

150 lines of C++. Camera capture, H.264 encoding, WebRTC transport, Symple signalling. Open a browser, see video. No plugins, no Google, no pain.

```cpp
// Accept call, wire up the pipeline, stream
session.IncomingCall += [&](const std::string& peerId) {
    session.accept();
};

session.StateChanged += [&](wrtc::PeerSession::State state) {
    if (state == wrtc::PeerSession::State::Active) {
        stream.attachSource(capture.get());
        stream.attach(&session->media().videoSender(), 5);
        stream.start();
    }
};
```

See [src/webrtc/samples/webcam-streamer/](src/webrtc/samples/webcam-streamer/) or read [WebRTC in 150 Lines of C++](https://0state.com/writing/webrtc-in-150-lines-of-cpp).

### Record a browser's camera server-side

Browser sends WebRTC, your C++ server decodes with FFmpeg, writes to any format. Video depositions, telehealth recording, proctoring - server-side recording without cloud vendor lock-in.

See [src/webrtc/samples/media-recorder/](src/webrtc/samples/media-recorder/).

### Stream any video file to a browser

Feed an MP4 in, get a real-time WebRTC stream out. Includes a simple data channel for control messages alongside the media stream.

See [src/webrtc/samples/file-streamer/](src/webrtc/samples/file-streamer/).

### Run your own TURN relay

Production-grade RFC 5766 TURN server with channel binding and TCP support. Stop paying for hosted TURN. ~30% of real-world WebRTC connections need relay through symmetric NATs; this handles them.

See [src/turn/samples/turnserver/](src/turn/samples/turnserver/).

### HTTP that outperforms Go

72,000 req/s with keep-alive on a single-core micro VM. Built on the same libuv + llhttp that powers Node.js, minus the runtime, GC, and language bridge.

| Server | Req/sec | Latency |
| ------ | ------: | ------: |
| Raw libuv+llhttp | 96,088 | 1.04ms |
| **icey** | **72,209** | **1.43ms** |
| Go 1.25 net/http | 53,878 | 2.31ms |
| Node.js v20 | 45,514 | 3.56ms |

icey delivers **75% of raw libuv throughput** while providing a complete HTTP stack (connection management, header construction, WebSocket upgrade, streaming responses). It outperforms Go's `net/http` by 34% and Node.js by 59%. All three share the same foundation (libuv for async IO, llhttp for HTTP parsing); the difference is pure runtime overhead.

See [src/http/perf/](src/http/perf/) for the cross-stack methodology, and [src/http/bench/](src/http/bench/) for the reportable HTTP microbenchmarks.

## Docs Start Here

If you just want the right page:

- Build and link your first program: [docs/getting-started.md](docs/getting-started.md)
- Full build and install options: [docs/installation.md](docs/installation.md)
- Module map and dependency picture: [docs/modules.md](docs/modules.md)
- HTTP and WebSocket: [docs/modules/http.md](docs/modules/http.md)
- TURN relay: [docs/modules/turn.md](docs/modules/turn.md)
- WebRTC media flows: [docs/modules/webrtc.md](docs/modules/webrtc.md)
- Full docs landing page: [docs/index.md](docs/index.md)

## Quick Start

### Requirements

| Platform | Compiler |
|----------|----------|
| Linux | GCC 12+ or Clang 15+ |
| macOS | AppleClang 15+ (Xcode 15+) |
| Windows | MSVC 2022 (Visual Studio 17+) |

CMake 3.21+ and pkg-config (Linux/macOS) required. icey fetches its core bundled third-party code automatically:

| Dependency | Version |
|------------|---------|
| libuv | 1.50 |
| llhttp | 9.2.1 |
| nlohmann/json | 3.11.3 |
| zlib | 1.3.1 |

System dependencies such as OpenSSL 3.x, FFmpeg 5+/6+/7+, and OpenCV 3.0+ are auto-detected when present. `libdatachannel` is fetched automatically when building the `webrtc` module.

### Build from source

```bash
git clone https://github.com/nilstate/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

### CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/nilstate/icey.git
  GIT_TAG 2.4.0
)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
```

### find_package

After installing (`cmake --install build`):

```cmake
find_package(icey REQUIRED)
target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
```

### Package Managers

Public registry submission is not live yet, but the repo now carries local package-manager support:

```bash
# Conan
conan create packaging/conan --build=missing -s compiler.cppstd=20
# or: make package-conan

# vcpkg overlay port
ICEY_VCPKG_SOURCE_PATH=$PWD vcpkg install icey --overlay-ports=$PWD/packaging/vcpkg
# or: make package-vcpkg

# Arch package / AUR seed
cd packaging/arch && makepkg --force --cleanbuild --syncdeps
# or: make package-arch

# Homebrew tap-local formulae
brew install --formula ./packaging/homebrew/Formula/libdatachannel.rb
brew install --formula ./packaging/homebrew/Formula/icey.rb
# or: make package-homebrew

# Debian source package / Launchpad PPA seed
make package-debian-source
```

The Conan recipe lives at `packaging/conan/conanfile.py`, the `vcpkg` overlay port lives at `packaging/vcpkg/icey/`, the Arch packaging files live at `packaging/arch/`, the Homebrew tap formulae live at `packaging/homebrew/Formula/`, and the Debian / PPA seed lives at `packaging/debian/debian/`.

For the release/tag/archive-pin flow behind those package managers, use [`docs/releasing.md`](docs/releasing.md).

## Code Examples

### Media pipeline

Camera to encoder to network:

```cpp
PacketStream stream;
stream.attachSource(videoCapture);
stream.attach(new av::MultiplexPacketEncoder(opts), 5);
stream.attach(socket, 10);
stream.start();
```

### HTTP server

```cpp
http::Server srv{ "127.0.0.1", 1337 };
srv.Connection += [](http::ServerConnection::Ptr conn) {
    conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
        conn.send(bufferCast<const char*>(buffer), buffer.size());
        conn.close();
    };
};
srv.start();
```

### WebRTC peer session

```cpp
wrtc::PeerSession::Config config;
config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
config.media.videoCodec = av::VideoCodec("H264", "libx264", 1280, 720, 30);

wrtc::SympleSignaller signaller(client);
wrtc::PeerSession session(signaller, config);

session.IncomingCall += [&](const std::string& peerId) {
    session.accept();
};

session.StateChanged += [&](wrtc::PeerSession::State state) {
    if (state == wrtc::PeerSession::State::Active)
        startStreaming(session);
};
```

## Modules

14 modules. Include only what you need; dependencies resolve automatically.

| Module | What it does |
|--------|-------------|
| **base** | Event loop (libuv), signals, streams, logging, filesystem, timers |
| **crypto** | Hashing, HMAC, RSA, X509 (OpenSSL 3.x) |
| **net** | TCP, SSL/TLS, UDP sockets, DNS |
| **http** | HTTP server/client, WebSocket, cookies, streaming, keep-alive |
| **json** | JSON serialisation (nlohmann/json) |
| **av** | FFmpeg capture, encode, decode, record, stream (FFmpeg 5/6/7) |
| **symple** | Real-time messaging, presence, rooms, WebRTC call signalling |
| **stun** | RFC 5389 STUN for NAT traversal |
| **turn** | RFC 5766 TURN relay server |
| **webrtc** | WebRTC via libdatachannel: media bridge, peer sessions, codec negotiation |
| **archo** | ZIP/archive handling |
| **pluga** | Plugin system (shared library loading) |
| **pacm** | Package manager for plugin distribution |
| **sched** | Task scheduler for deferred/periodic jobs |

## Contributors

* Kam Low ([@auscaster](https://github.com/auscaster)) - Creator and primary developer
* Sergey Parfenyuk ([@sparfenyuk](https://github.com/sparfenyuk)) - macOS compile fixes, type corrections, buffer handling
* Yury Shubin ([@yuryshubin](https://github.com/yuryshubin)) - iOS build toolchain and platform fixes
* Norm Ovenseri ([@normano](https://github.com/normano)) - Apple/FFmpeg builds, AVFoundation support, verbose logging
* Igor Lutsyk ([@lutsykigor](https://github.com/lutsykigor)) - WebRTC/WebSocket fixes, OpenCV+WebRTC sample app, Firefox compatibility
* Kryton ([@Malesio](https://github.com/Malesio)) - Segfault fixes and Valgrind cleanup
* Vinci Xu ([@VinciShark](https://github.com/VinciShark)) - Windows documentation, testing and updates
* Michael Fig ([@michael-fig](https://github.com/michael-fig)) - Compiler flags for building without FFmpeg
* Stanislav Kapulkin ([@kapulkin](https://github.com/kapulkin)) - WebRTC modernisation and macOS compile definitions
* Thomas Reichhart ([@blackforest-tom](https://github.com/blackforest-tom)) - FFmpeg constant updates and ARM build fixes
* Artem Suprunov ([@artemiuzzz](https://github.com/artemiuzzz)) - WebRTC null pointer fix and library path resolution
* Hyunuk Kim ([@surinkim](https://github.com/surinkim)) - Windows `std::codecvt` unicode conversion fix
* Cameron Smith ([@cksmith](https://github.com/cksmith)) - Git line ending normalisation
* Damian Zelim ([@ZelimDamian](https://github.com/ZelimDamian)) - OS X compiler flag fixes
* Alexey ([@deilos](https://github.com/deilos)) - Cross-platform FFmpeg build script fixes

## Contributing

PRs welcome. See the [contributing guide](docs/contributing.md) for code style, tests, and workflow.
