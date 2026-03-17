# LibSourcey

[![CI](https://github.com/sourcey/libsourcey/actions/workflows/ci.yml/badge.svg)](https://github.com/sourcey/libsourcey/actions/workflows/ci.yml)
[![License: LGPL-2.1+](https://img.shields.io/badge/license-LGPL--2.1%2B-blue.svg)](LICENSE.md)

> C++ Networking Evolved

LibSourcey is a collection of cross-platform C++20 modules for building high-performance networked and media streaming applications. It combines the async IO of `libuv` with `OpenSSL`, `FFmpeg`, and modern C++ to provide a clean, modular toolkit for servers, clients, and real-time communication. All dependencies are managed via CMake FetchContent - just add LibSourcey to your project and build.

**Version 2.0** - Complete modernisation: C++20, OpenSSL 3.x, FFmpeg 6+, llhttp, smart pointers throughout, zero `assert()` in production code, comprehensive test coverage. See [CHANGELOG.md](CHANGELOG.md) for details.

* **Documentation**: [doc/](doc/SUMMARY.md)
* **Repository**: [https://github.com/sourcey/libsourcey](https://github.com/sourcey/libsourcey)
* **Licence**: [LGPL-2.1+](LICENSE.md)

## Features

* **Event-based IO** - Built on `libuv` for asynchronous, non-blocking IO across all platforms.
* **Cross platform** - Compiles on Linux, macOS and Windows with any C++20 compiler (GCC 12+, Clang 15+, MSVC 2022+, AppleClang 15+).
* **Modular libraries** - Include only what you need. Each module is self-contained with automatic dependency resolution via CMake.
* **Solid networking** - TCP, SSL/TLS, and UDP sockets built on `libuv` and `OpenSSL`.
* **HTTP stack** - Servers, clients, WebSockets, media streaming, file transfers, and authentication. HTTP parsing via `llhttp`.
* **Media encoding** - The `av` module wraps `FFmpeg` for capture, encoding, recording, streaming and analysis.
* **Real-time messaging** - Socket.IO client (protocol revision 4) and Symple messaging protocol.
* **STUN/TURN** - Full RFC 5766 TURN server/client with RFC 5389 STUN for NAT traversal.
* **Docker images** - Multi-stage Ubuntu 24.04 images available via the included `Dockerfile`.

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

### Process IO

```cpp
Process proc{ "ping", "example.com" };
proc.onstdout = [](std::string line) {
    // handle process output
};
proc.onexit = [](std::int64_t status) {
    // handle process exit
};
proc.spawn();
```

### Media streaming

```cpp
PacketStream stream;

av::EncoderOptions options;
options.oformat = av::Format{"MP4", "mp4",
    { "H.264", "libx264", 640, 480, 25, 48000, 128000, "yuv420p" },
    { "AAC", "aac", 2, 44100, 64000, "fltp" }};

av::DeviceManager devman;
av::Device device;

if (devman.getDefaultCamera(device)) {
    auto video = std::make_shared<av::VideoCapture>(device.id, 640, 480);
    video->getEncoderFormat(options.iformat);
    stream.attachSource(video, true);
}

av::MultiplexPacketEncoder::Ptr encoder(options);
stream.attach(encoder);
stream.attach(socket); // broadcast over network
stream.start();
```

## Regenerating API docs

The API reference in `doc/` is generated from source code using [Doxygen](https://www.doxygen.nl/) and [Moxygen](https://github.com/sourcey/moxygen).

Prerequisites: `doxygen` and `npx` (Node.js).

```bash
make docs
```

This runs Doxygen to parse the C++ source into XML, then Moxygen to convert it into markdown files (`doc/api-*.md`).

## Contributors

LibSourcey has been shaped by contributions from developers around the world. Thanks to everyone who has helped make it better:

* Kam Low ([@auscaster](https://github.com/auscaster)) - Creator and primary developer
* Sergey Parfenyuk ([@sparfenyuk](https://github.com/sparfenyuk)) - Extensive compile fixes across macOS, type corrections, and buffer handling
* Yury Shubin ([@yuryshubin](https://github.com/yuryshubin)) - iOS build toolchain and platform fixes
* Norm Ovenseri ([@normano](https://github.com/normano)) - Apple/FFmpeg builds, AVFoundation support, verbose logging
* Igor Lutsyk ([@lutsykigor](https://github.com/lutsykigor)) - WebRTC/WebSocket fixes, OpenCV+WebRTC sample app, Firefox compatibility
* Kryton ([@Malesio](https://github.com/Malesio)) - Fixed segfaults in samples and tidied up Valgrind warnings
* Vinci Xu ([@VinciShark](https://github.com/VinciShark)) - Windows documentation, testing and updates
* Michael Fig ([@michael-fig](https://github.com/michael-fig)) - Fixed compiler flags to build without FFmpeg
* Kapulkin Stanislav ([@kapulkin](https://github.com/kapulkin)) - WebRTC modernisation and macOS compile definitions
* Thomas Reichhart ([@blackforest-tom](https://github.com/blackforest-tom)) - FFmpeg constant updates and ARM build fixes
* Artem Suprunov ([@artemiuzzz](https://github.com/artemiuzzz)) - WebRTC null pointer fix and library path resolution
* Hyunuk Kim ([@surinkim](https://github.com/surinkim)) - Fixed `std::codecvt` unicode character conversion on Windows
* Cameron Smith ([@cksmith](https://github.com/cksmith)) - Git line ending normalisation
* Damian Zelim ([@ZelimDamian](https://github.com/ZelimDamian)) - Fixed compiler flags for OS X build
* Alexey ([@deilos](https://github.com/deilos)) - Fixed cross-platform FFmpeg build script

## Contributing

Pull Requests are always welcome, so if you fix or make anything better then feel free to float a PR back upstream :)

1. [Fork LibSourcey on Github](https://github.com/sourcey/libsourcey)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
