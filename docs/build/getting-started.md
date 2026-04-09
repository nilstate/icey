# Getting Started

This page is the shortest path from zero to a working icey program.

If you need every build knob, install prefix, or platform package detail, use the [installation guide](installation.md). If you are still figuring out which subsystem you need, start with the [module map](../modules.md).

## Quick Start

:::steps

1. Add icey to your CMakeLists.txt

   The fastest way to use icey is CMake FetchContent. No manual cloning, no system installation; CMake downloads and builds only the modules you need.

   ```cmake
   cmake_minimum_required(VERSION 3.21)
   project(myapp)

   include(FetchContent)
   FetchContent_Declare(icey
     GIT_REPOSITORY https://github.com/nilstate/icey.git
     GIT_TAG 2.4.4
   )
   FetchContent_MakeAvailable(icey)

   add_executable(myapp src/main.cpp)
   target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
   ```

2. Link only what you need

   Each module is a separate imported CMake target under the `icey::` namespace. Dependencies resolve automatically.

3. Build

   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

:::

## Available Targets

| Target | Module | Optional Dependencies |
| ------ | ------ | --------------------- |
| `icey::base` | [Base](../modules/base.md) | — |
| `icey::crypto` | [Crypto](../modules/crypto.md) | OpenSSL 3.x |
| `icey::net` | [Net](../modules/net.md) | — |
| `icey::http` | [HTTP](../modules/http.md) | — |
| `icey::json` | [JSON](../modules/json.md) | — |
| `icey::av` | [AV](../modules/av.md) | FFmpeg 5+/6+/7+ |
| `icey::webrtc` | [WebRTC](../modules/webrtc.md) | libdatachannel |
| `icey::symple` | [Symple](../modules/symple.md) | — |
| `icey::stun` | [STUN](../modules/stun.md) | — |
| `icey::turn` | [TURN](../modules/turn.md) | — |
| `icey::archo` | [Archo](../modules/archo.md) | — |
| `icey::graft` | [Graft](../modules/graft.md) | — |
| `icey::pacm` | [Pacm](../modules/pacm.md) | — |
| `icey::sched` | [Sched](../modules/sched.md) | — |

## Building from Source

If you want a local build without installing system-wide:

```bash
git clone https://github.com/nilstate/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

Then use `find_package` in your project:

```cmake
find_package(icey REQUIRED)
target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
```

See the [installation guide](installation.md) for the full build/install path, platform-specific dependencies, and CMake options.

## Enabling Optional Modules

icey auto-detects optional system dependencies and builds the matching modules when they are available.

- **`icey::av`** (FFmpeg): install `libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev` (Ubuntu/Debian) or `ffmpeg` (Homebrew). Point CMake at a custom build with `-DFFmpeg_ROOT=/path/to/ffmpeg`. The `av` module builds when FFmpeg is found.
- **`icey::webrtc`** (libdatachannel): requires the `av` prerequisites above plus libdatachannel. libdatachannel is fetched automatically via FetchContent when the `webrtc` module is enabled and its prerequisites are present.
- **`icey::crypto`, `icey::net`, `icey::stun`, `icey::turn`** (OpenSSL): install `libssl-dev` (Ubuntu/Debian) or `openssl` (Homebrew). These modules build when OpenSSL is found.

For the full build matrix and all user-settable options, use [installation.md](installation.md).

## What You Can Build

### Stream a webcam to any browser

150 lines of C++. Camera capture, H.264 encoding, WebRTC transport, Symple signalling. Open a browser, see video.

```cpp
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

See [`webcam-streamer`](https://github.com/nilstate/icey/tree/main/src/webrtc/samples/webcam-streamer/) or read [WebRTC in 150 Lines of C++](https://0state.com/writing/webrtc-in-150-lines-of-cpp).

### Record a browser's camera server-side

Browser sends WebRTC, your C++ server decodes with FFmpeg, writes to any format. Server-side recording without cloud vendor lock-in.

See [`media-recorder`](https://github.com/nilstate/icey/tree/main/src/webrtc/samples/media-recorder/).

### Media pipeline

Camera to encoder to network in four lines:

```cpp
PacketStream stream;
stream.attachSource(videoCapture);
stream.attach(new av::MultiplexPacketEncoder(opts), 5);
stream.attach(socket, 10);
stream.start();
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

## Your First Program

A minimal HTTP echo server:

```cpp
#include "icy/http/server.h"
#include "icy/loop.h"

int main() {
    icy::http::Server srv{"0.0.0.0", 8080};
    srv.Connection += [](icy::http::ServerConnection::Ptr conn) {
        conn->Payload += [](icy::http::ServerConnection& conn,
                            const icy::MutableBuffer& buf) {
            conn.send(icy::bufferCast<const char*>(buf), buf.size());
            conn.close();
        };
    };
    srv.start();
    icy::uv::runLoop();
    return 0;
}
```

## Next Steps

- Read the [module map](../modules.md) if you want the dependency picture first.
- Go straight to a guide if you know the subsystem:
  - [Base](../modules/base.md)
  - [HTTP](../modules/http.md)
  - [WebRTC](../modules/webrtc.md)
  - [TURN](../modules/turn.md)
- Explore the samples and apps in each module's `src/*/samples` or `src/*/apps` directories.
- Read the [contributing guide](../contributing.md) if you want to work on icey itself.
