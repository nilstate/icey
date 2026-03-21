# Getting Started

## Quick Start

The fastest way to use Icey is CMake FetchContent. No manual cloning, no system installation; CMake downloads and builds only the modules you need.

```cmake
cmake_minimum_required(VERSION 3.21)
project(myapp)

include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/sourcey/icey.git
  GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(icey)

add_executable(myapp src/main.cpp)
target_link_libraries(myapp PRIVATE icy_base icy_net icy_http)
```

Each module is a separate CMake target prefixed with `icy_`. Link only what you use; dependencies resolve automatically.

## Available Targets

| Target | Module | Optional Dependencies |
| ------ | ------ | --------------------- |
| `icy_base` | [Base](modules/base.md) | — |
| `icy_crypto` | [Crypto](modules/crypto.md) | OpenSSL 3.x |
| `icy_net` | [Net](modules/net.md) | — |
| `icy_http` | [HTTP](modules/http.md) | — |
| `icy_json` | [JSON](modules/json.md) | — |
| `icy_av` | [AV](modules/av.md) | FFmpeg 5+/6+/7+ |
| `icy_webrtc` | [WebRTC](modules/webrtc.md) | libdatachannel |
| `icy_symple` | [Symple](modules/symple.md) | — |
| `icy_stun` | [STUN](modules/stun.md) | — |
| `icy_turn` | [TURN](modules/turn.md) | — |
| `icy_archo` | [Archo](modules/archo.md) | — |
| `icy_pacm` | [Pacm](modules/pacm.md) | — |
| `icy_pluga` | [Pluga](modules/pluga.md) | — |
| `icy_sched` | [Sched](modules/sched.md) | — |

## Building from Source

If you prefer to build and install Icey on your system:

```bash
git clone https://github.com/sourcey/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

Then use `find_package` in your project:

```cmake
find_package(Icey REQUIRED)
target_link_libraries(myapp PRIVATE icy_base icy_net icy_http)
```

See the [installation guide](installation.md) for platform-specific instructions ([Linux](installation-linux.md), [macOS](installation-osx.md), [Windows](installation-windows.md)).

## Enabling Optional Modules

Some modules require external libraries. Icey auto-detects dependencies via `find_package`; there are no `WITH_*` flags to set. Install the libraries so CMake can find them and the corresponding modules build automatically.

- **`icy_av`** (FFmpeg): install `libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev` (Ubuntu/Debian) or `ffmpeg` (Homebrew). Point CMake at a custom build with `-DFFmpeg_ROOT=/path/to/ffmpeg`.
- **`icy_webrtc`** (libdatachannel): no installation needed; CMake fetches it automatically via FetchContent when `icy_webrtc` is requested.
- **`icy_crypto`, `icy_net`, `icy_stun`, `icy_turn`** (OpenSSL): install `libssl-dev` (Ubuntu/Debian) or `openssl` (Homebrew). These modules are disabled if OpenSSL is not found.

The user-settable CMake options are: `BUILD_TESTS`, `BUILD_SHARED_LIBS`, `ENABLE_NATIVE_ARCH`, `ENABLE_LTO`, `ASAN`.

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

- Browse the [module guides](modules.md) for architecture, usage, and code examples
- Explore the samples in each module's `samples/` directory
- Read the [contributing guide](contributing.md) if you want to send a PR
