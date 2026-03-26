# Installation

This is the full build and install page.

If you just want the fastest path to a first build, use [getting-started.md](getting-started.md). This page is for the complete picture: requirements, optional dependencies, build flags, install, and downstream consumption.

Icey uses CMake to build from source. The codebase is cross-platform and requires a C++20 compiler.

Platform-specific guides:

* [Linux](installation-linux.md)
* [macOS](installation-osx.md)
* [Windows](installation-windows.md)

## Requirements

| Requirement | Version |
| ----------- | ------- |
| CMake | 3.21+ |
| C++ compiler | GCC 12+, Clang 15+, AppleClang 15+, MSVC 2022 |
| OpenSSL | 3.x |
| pkg-config | (Linux/macOS only) |

## Optional Dependencies

These are auto-detected by CMake. If they are present, the matching modules build automatically.

| Dependency | Enables | Install |
| ---------- | ------- | ------- |
| FFmpeg 5+/6+/7+ | `av` module (capture, encode, decode) | `apt install libavcodec-dev libavformat-dev libswscale-dev libavdevice-dev` |
| OpenCV 3.0+ | OpenCV integration in `av` | `apt install libopencv-dev` |
| libdatachannel | `webrtc` module | fetched via CMake FetchContent |

If you need platform-specific package commands, use the platform pages:

- [Linux](installation-linux.md)
- [macOS](installation-osx.md)
- [Windows](installation-windows.md)

## Build

```bash
git clone https://github.com/sourcey/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

## Install

```bash
cmake --install build --prefix /usr/local
```

That gives you exported `Icey::...` targets for `find_package`.

## CMake Options

| Option | Default | Description |
| ------ | ------- | ----------- |
| `BUILD_TESTS` | `OFF` | Build module tests |
| `BUILD_TESTS_<module>` | `OFF` | Build tests for a specific module |
| `BUILD_SAMPLES` | `OFF` | Build module samples |
| `BUILD_SAMPLES_<module>` | `OFF` | Build samples for a specific module |
| `BUILD_MODULE_<name>` | `ON` | Set to `OFF` to disable a specific module |
| `BUILD_SHARED_LIBS` | `OFF` | Build shared libraries instead of static |
| `ENABLE_NATIVE_ARCH` | `ON` | Tune for build machine CPU (`-march=native`) |
| `ENABLE_LTO` | `ON` | Enable link-time optimization |
| `ASAN` | `OFF` | Enable AddressSanitizer |

## Example Configurations

```bash
# Full build with tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Disable a module you don't need
cmake -B build -DBUILD_MODULE_pacm=OFF -DBUILD_MODULE_pluga=OFF

# Sanitizer build
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DASAN=ON -DBUILD_TESTS=ON

# Point to custom FFmpeg
cmake -B build -DFFMPEG_ROOT=/opt/ffmpeg

# Point to custom OpenSSL (macOS)
cmake -B build -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

## Use as a Dependency

### CMake FetchContent (recommended)

```cmake
include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/sourcey/icey.git
  GIT_TAG v2.3.0
)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE Icey::base Icey::net Icey::http)
```

### find_package (after installing)

```cmake
find_package(Icey REQUIRED)
target_link_libraries(myapp PRIVATE Icey::base Icey::net Icey::http)
```

### Package Managers

The upstream public registries are not published yet, but the repo contains working local packaging entry points:

```bash
# Conan
conan create packaging/conan --build=missing -s compiler.cppstd=20
# or: make package-conan

# vcpkg overlay port
vcpkg install icey --overlay-ports=$PWD/packaging/vcpkg
# or: make package-vcpkg
```

Use these from the repository root. The `vcpkg` port is intended as an overlay/custom-registry seed until the upstream registry submissions happen.

If you only need a first success, the shorter version of both flows is already in [getting-started.md](getting-started.md).

## Docker

```bash
docker build -t icey .
```

Multi-stage Ubuntu 24.04 image via the included `Dockerfile`.
