# LibSourcey

[![CI](https://github.com/sourcey/libsourcey/actions/workflows/ci.yml/badge.svg)](https://github.com/sourcey/libsourcey/actions/workflows/ci.yml)
[![License: LGPL-2.1+](https://img.shields.io/badge/license-LGPL--2.1%2B-blue.svg)](LICENSE.md)

> C++ Networking Evolved

Most C++ projects that need networking, media encoding, and real-time communication end up gluing together FFmpeg, OpenCV, libuv, and OpenSSL by hand - fighting memory ownership across threading models and build systems that take longer to debug than the code. LibSourcey is the connective tissue: a modular C++20 toolkit that unifies all of it into a single composable pipeline. All dependencies managed via CMake FetchContent.

* **Documentation**: [doc/](doc/SUMMARY.md)
* **Changelog**: [CHANGELOG.md](CHANGELOG.md)
* **Repository**: [https://github.com/sourcey/libsourcey](https://github.com/sourcey/libsourcey)
* **Licence**: [LGPL-2.1+](LICENSE.md)

## Features

* **Packet pipeline** - Composable `PacketStream` architecture: sources emit, processors transform, sinks consume. Backpressure, frame dropping, and lifecycle management built in.
* **Async IO** - Non-blocking TCP, SSL/TLS, and UDP sockets on `libuv` with signal-driven dispatch (`Connection += [](auto) { ... }`).
* **HTTP stack** - Servers, clients, WebSockets, streaming responses, file transfers, auth. Parsing via `llhttp`.
* **Media encoding** - `FFmpeg` capture, encoding, recording, streaming. RAII wrappers, no manual `av_frame_free()`. Compatible with FFmpeg 5, 6, and 7.
* **STUN/TURN** - Full RFC 5766 TURN server/client with channel binding, RFC 5389 STUN for NAT traversal.
* **Real-time messaging** - Socket.IO v4 client and Symple signalling protocol.
* **Cross platform** - Linux, macOS, Windows. GCC 12+, Clang 15+, MSVC 2022+.
* **Modular** - Include only what you need. 14 modules with automatic dependency resolution.

## Performance

LibSourcey's HTTP module is built directly on libuv and llhttp - the same async IO and HTTP parsing that powers Node.js. The difference is there's no runtime, no garbage collector, and no language bridge sitting between your application and the event loop. The result:

| Server | Req/sec | Latency |
| ------ | ------: | ------: |
| Raw libuv+llhttp | 19,329 | 4.59ms |
| **LibSourcey** | **13,514** | **8.14ms** |
| Node.js v20 | 8,778 | 11.44ms |

Raw libuv+llhttp is a zero-abstraction baseline: hand-rolled C with no connection management, no header building, nothing. LibSourcey adds a full HTTP stack on top - connection lifecycle, response construction, signal dispatch, WebSocket upgrade, streaming - and keeps 70% of that ceiling. Node.js, with the same foundations underneath, reaches 45%.

See [src/http/samples/httpbenchmark/](src/http/samples/httpbenchmark/) for methodology and multi-core results.

## Quick start

### Requirements

| Platform | Compiler |
|----------|----------|
| Linux | GCC 12+ or Clang 15+ |
| macOS | AppleClang 15+ (Xcode 15+) |
| Windows | MSVC 2022 (Visual Studio 17+) |

Dependencies: CMake 3.21+, pkg-config (Linux/macOS). The following are fetched automatically via FetchContent if not found on the system:

| Dependency | Version |
|------------|---------|
| libuv | 1.50 |
| llhttp | 9.2.1 |
| OpenSSL | 3.x |
| nlohmann/json | 3.11.3 |
| zlib | 1.3.1 |

### CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(libsourcey
  GIT_REPOSITORY https://github.com/sourcey/libsourcey.git
  GIT_TAG master
)
FetchContent_MakeAvailable(libsourcey)
target_link_libraries(myapp PRIVATE scy_base scy_net scy_crypto)
```

### Build from source

```bash
git clone https://github.com/sourcey/libsourcey.git
cd libsourcey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

### find_package

After installing (`cmake --install build`):

```cmake
find_package(LibSourcey REQUIRED)
target_link_libraries(myapp PRIVATE scy_base scy_net scy_http)
```

## Examples

### Media pipeline

Camera to encoder to network socket in six lines:

```cpp
PacketStream stream;
stream.attachSource(videoCapture);
stream.attach(new av::MultiplexPacketEncoder(opts), 5);
stream.attach(socket, 10);
stream.start();
```

### HTTP echo server

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

## Contributors

Thanks to all contributors:

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

PRs welcome. See the [contributing guide](doc/contributing.md) for code style, tests, and workflow.
