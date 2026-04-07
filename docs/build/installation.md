# Installation

This is the full build and install page.

If you just want the fastest path to a first build, use [getting-started](getting-started.md). This page is for the complete picture: requirements, optional dependencies, build flags, install, and downstream consumption.

icey uses CMake to build from source. The codebase is cross-platform and requires a C++20 compiler.

## Requirements

| Requirement | Version |
| --- | --- |
| CMake | 3.21+ |
| C++ compiler | GCC 12+, Clang 15+, AppleClang 15+, MSVC 2022 |
| OpenSSL | 3.x |
| pkg-config | (Linux/macOS only) |

## Platform Dependencies

:::tabs

::tab{title="Linux (Ubuntu/Debian)"}

```bash
sudo apt-get update
sudo apt-get install -y build-essential pkg-config git cmake libssl-dev
```

**FFmpeg** (optional — enables `av` module):

```bash
# Ubuntu/Debian
sudo apt-get install -y libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev libswscale-dev

# Fedora/RHEL
sudo dnf install ffmpeg-devel
```

If FFmpeg is installed in a non-standard location:

```bash
cmake -B build -DFFMPEG_ROOT=/path/to/ffmpeg
```

**OpenCV** (optional):

```bash
sudo apt-get install -y libopencv-dev
```

::

::tab{title="macOS"}

Uses [Homebrew](https://brew.sh). Works on both Intel and Apple Silicon.

```bash
brew install cmake openssl
```

**FFmpeg** (optional):

```bash
brew install ffmpeg
```

:::note
On macOS you must point CMake at Homebrew's OpenSSL:

```bash
cmake -B build -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

:::

::

::tab{title="Windows"}

Requirements:

- [Git](https://git-scm.com/)
- [CMake 3.21+](https://cmake.org/download/)
- [Visual Studio 2022](https://visualstudio.microsoft.com/) (C++ workload)

**OpenSSL**: install via [vcpkg](https://vcpkg.io/):

```powershell
vcpkg install openssl:x64-windows
```

Or download prebuilt binaries from [slproweb.com](https://slproweb.com/products/Win32OpenSSL.html) and set `OPENSSL_ROOT_DIR`.

**FFmpeg** (optional):

Download development libraries from [gyan.dev](https://www.gyan.dev/ffmpeg/builds/). Add the `bin` directory to your system `PATH`, then set `FFMPEG_ROOT`:

```powershell
cmake -B build -DFFMPEG_ROOT=C:\vendor\ffmpeg
```

::

:::

## Optional Dependencies

These are auto-detected by CMake. If they are present, the matching modules build automatically.

| Dependency | Enables | Notes |
| --- | --- | --- |
| FFmpeg 5+/6+/7+ | `av` module (capture, encode, decode) | See platform tabs above |
| OpenCV 3.0+ | OpenCV integration in `av` | `apt install libopencv-dev` |
| libdatachannel | `webrtc` module | Fetched automatically via FetchContent |

## Build

:::tabs

::tab{title="Linux / macOS"}

```bash
git clone https://github.com/nilstate/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

On macOS, replace `$(nproc)` with `$(sysctl -n hw.ncpu)` and add `-DOPENSSL_ROOT_DIR=$(brew --prefix openssl)`.

::

::tab{title="Windows"}

### Command line

```powershell
git clone https://github.com/nilstate/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DOPENSSL_ROOT_DIR=C:\vendor\OpenSSL-Win64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
cmake --install build --prefix C:\icey
```

### Visual Studio

1. Open CMake GUI, point source to the icey root, configure, generate
2. Open `build/icey.sln` in Visual Studio
3. Build Debug and/or Release configurations

::

::tab{title="Xcode"}

Generate an Xcode project instead of Makefiles:

```bash
cmake -B build -G Xcode -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
open build/icey.xcodeproj
```

::

:::

## Install

```bash
cmake --install build --prefix /usr/local
```

That gives you exported `icey::...` targets for `find_package`.

## CMake Options

| Option | Default | Description |
| --- | --- | --- |
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

:::code-group

```bash title="Full debug build with tests"
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
```

```bash title="Disable modules you don't need"
cmake -B build -DBUILD_MODULE_pacm=OFF -DBUILD_MODULE_graft=OFF
```

```bash title="Sanitizer build"
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DASAN=ON -DBUILD_TESTS=ON
```

```bash title="Custom FFmpeg"
cmake -B build -DFFMPEG_ROOT=/opt/ffmpeg
```

```bash title="Custom OpenSSL (macOS)"
cmake -B build -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

:::

## Use as a Dependency

:::code-group

```cmake title="FetchContent (recommended)"
include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/nilstate/icey.git
  GIT_TAG 2.4.3
)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
```

```cmake title="find_package (after installing)"
find_package(icey REQUIRED)
target_link_libraries(myapp PRIVATE icey::base icey::net icey::http)
```

:::

## Package Managers

The upstream public registries are not published yet, but the repo contains working local packaging entry points:

```bash
# Conan
conan create packaging/conan --build=missing -s compiler.cppstd=20

# vcpkg overlay port
ICEY_VCPKG_SOURCE_PATH=$PWD vcpkg install icey --overlay-ports=$PWD/packaging/vcpkg

# Arch package / AUR seed
cd packaging/arch && makepkg --force --cleanbuild --syncdeps

# Homebrew tap-local formulae
brew install --formula ./packaging/homebrew/Formula/libdatachannel.rb
brew install --formula ./packaging/homebrew/Formula/icey.rb

# Debian source package / Launchpad PPA seed
make package-debian-source
```

If you are cutting a real release and need the version-sync, tag, archive-pin, and package publication flow, use [releasing.md](../releasing.md).

## Docker

```bash
docker build -t icey .
```

Multi-stage Ubuntu 24.04 image via the included `Dockerfile`.
