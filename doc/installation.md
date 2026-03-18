## Installation

LibSourcey uses CMake to generate project files for your compiler. The codebase is cross-platform and compiles on any system with a C++20 compiler.

Platform-specific guides:

* [Linux](installation-linux.md)
* [macOS](installation-osx.md)
* [Windows](installation-windows.md)

## Dependencies

* **OpenSSL 3.x** (required) - SSL networking, encryption and cryptography
* **FFmpeg 6+/7+** (optional) - Media encoding, decoding, device capture via the `av` module
* **OpenCV 3.0+** (optional) - Video capture and computer vision

## CMake build options

| Option | Default | Description |
| ------ | ------- | ----------- |
| `BUILD_SHARED_LIBS` | `OFF` | Build shared libraries (.dll/.so) instead of static (.lib/.a) |
| `BUILD_MODULES` | `ON` | Build all LibSourcey modules |
| `BUILD_MODULE_xxx` | | Enable or disable a specific module |
| `BUILD_APPLICATIONS` | `ON` | Build applications |
| `BUILD_TESTS` | `OFF` | Build module tests |
| `BUILD_SAMPLES` | `OFF` | Build module samples |
| `BUILD_ALPHA` | `OFF` | Build alpha development modules |
| `WITH_FFMPEG` | `OFF` | Enable FFmpeg support |
| `WITH_OPENCV` | `OFF` | Enable OpenCV support |
| `ENABLE_LOGGING` | `ON` | Enable internal debug logging |

For an exhaustive list of options check the `CMakeLists.txt` in the project root.

## Example configurations

```bash
# Minimum build (base only)
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_MODULES=OFF \
  -DBUILD_MODULE_base=ON

# Networking build
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_MODULES=OFF -DBUILD_TESTS=ON \
  -DBUILD_MODULE_base=ON -DBUILD_MODULE_crypto=ON -DBUILD_MODULE_net=ON

# HTTP build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_MODULES=OFF \
  -DBUILD_TESTS=ON -DBUILD_SAMPLES=ON \
  -DBUILD_MODULE_base=ON -DBUILD_MODULE_crypto=ON \
  -DBUILD_MODULE_json=ON -DBUILD_MODULE_net=ON \
  -DBUILD_MODULE_http=ON

# Media build (with FFmpeg)
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_MODULES=OFF -DBUILD_TESTS=ON \
  -DWITH_FFMPEG=ON -DBUILD_MODULE_base=ON -DBUILD_MODULE_av=ON

# STUN/TURN build
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_MODULES=OFF \
  -DBUILD_MODULE_base=ON -DBUILD_MODULE_crypto=ON \
  -DBUILD_MODULE_net=ON -DBUILD_MODULE_stun=ON -DBUILD_MODULE_turn=ON
```

## Installing

```bash
cmake --install build --prefix /usr/local
```

## Docker

```bash
docker build -t libsourcey .
```

Multi-stage Ubuntu 24.04 image via the included `Dockerfile`.

## External module plugin

LibSourcey supports external modules that live outside the main source tree. Create a directory with a `CMakeLists.txt` that uses `scy_add_module()`:

```cmake
scy_add_module(mymodule
  DEPENDS base net
  PACKAGES OpenSSL::SSL
  PRETTY_NAME MyModule
)
```

Then pass the module path when configuring:

```bash
cmake -B build -DBUILD_MODULE_mymodule=ON -DLibSourcey_EXTRA_MODULES=/path/to/mymodule
```

## Building packages

LibSourcey can be packaged into `deb`, `rpm`, `tar.gz`, `zip` and other formats using CPack:

```bash
cd build
cmake .. -DCPACK_GENERATOR=DEB
cpack
```
