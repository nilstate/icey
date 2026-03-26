# Getting Started

This page is the shortest path from zero to a working icey program.

If you need every build knob, install prefix, or platform package detail, use the [installation guide](installation.md). If you are still figuring out which subsystem you need, start with the [module map](modules.md).

## Quick Start

The fastest way to use icey is CMake FetchContent. No manual cloning, no system installation; CMake downloads and builds only the modules you need.

```cmake
cmake_minimum_required(VERSION 3.21)
project(myapp)

include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/sourcey/icey.git
  GIT_TAG 2.3.0
)
FetchContent_MakeAvailable(icey)

add_executable(myapp src/main.cpp)
target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
```

Each module is a separate imported CMake target under the `icey::` namespace. Link only what you use; dependencies resolve automatically.

## Available Targets

| Target | Module | Optional Dependencies |
| ------ | ------ | --------------------- |
| `icey::base` | [Base](modules/base.md) | — |
| `icey::crypto` | [Crypto](modules/crypto.md) | OpenSSL 3.x |
| `icey::net` | [Net](modules/net.md) | — |
| `icey::http` | [HTTP](modules/http.md) | — |
| `icey::json` | [JSON](modules/json.md) | — |
| `icey::av` | [AV](modules/av.md) | FFmpeg 5+/6+/7+ |
| `icey::webrtc` | [WebRTC](modules/webrtc.md) | libdatachannel |
| `icey::symple` | [Symple](modules/symple.md) | — |
| `icey::stun` | [STUN](modules/stun.md) | — |
| `icey::turn` | [TURN](modules/turn.md) | — |
| `icey::archo` | [Archo](modules/archo.md) | — |
| `icey::pacm` | [Pacm](modules/pacm.md) | — |
| `icey::pluga` | [Pluga](modules/pluga.md) | — |
| `icey::sched` | [Sched](modules/sched.md) | — |

## Building from Source

If you want a local build without installing system-wide:

```bash
git clone https://github.com/sourcey/icey.git
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

See the [installation guide](installation.md) for the full build/install path and platform-specific instructions:

- [Linux](installation-linux.md)
- [macOS](installation-osx.md)
- [Windows](installation-windows.md)

## Enabling Optional Modules

icey auto-detects optional system dependencies and builds the matching modules when they are available.

- **`icey::av`** (FFmpeg): install `libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev` (Ubuntu/Debian) or `ffmpeg` (Homebrew). Point CMake at a custom build with `-DFFmpeg_ROOT=/path/to/ffmpeg`. The `av` module builds when FFmpeg is found.
- **`icey::webrtc`** (libdatachannel): requires the `av` prerequisites above plus libdatachannel. libdatachannel is fetched automatically via FetchContent when the `webrtc` module is enabled and its prerequisites are present.
- **`icey::crypto`, `icey::net`, `icey::stun`, `icey::turn`** (OpenSSL): install `libssl-dev` (Ubuntu/Debian) or `openssl` (Homebrew). These modules build when OpenSSL is found.

For the full build matrix and all user-settable options, use [installation.md](installation.md).

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

- Read the [module map](modules.md) if you want the dependency picture first.
- Go straight to a guide if you know the subsystem:
  - [Base](modules/base.md)
  - [HTTP](modules/http.md)
  - [WebRTC](modules/webrtc.md)
  - [TURN](modules/turn.md)
- Explore the samples and apps in each module's `src/*/samples` or `src/*/apps` directories.
- Read the [contributing guide](contributing.md) if you want to work on icey itself.
