# Building LibSourcey

## Prerequisites

- CMake 3.21+
- C++20 compiler (GCC 12+, Clang 15+, MSVC 2022+, AppleClang 15+)
- OpenSSL 3.x development headers
- pkg-config (Linux/macOS)
- FFmpeg 6.x or 7.x (optional, for the `av` module)

### Platform-specific

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install build-essential cmake pkg-config libssl-dev
# Optional: for av module
sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev
```

**macOS:**
```bash
brew install cmake openssl@3
# Optional: for av module
brew install ffmpeg
```

**Windows:**
```powershell
choco install cmake openssl
```

## Default build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
```

## Build with tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

## Build options

| Option | Default | Description |
| ------ | ------- | ----------- |
| `BUILD_SHARED_LIBS` | `OFF` | Build shared libraries |
| `BUILD_MODULES` | `ON` | Build all LibSourcey modules |
| `BUILD_TESTS` | `OFF` | Build module tests |
| `BUILD_SAMPLES` | `OFF` | Build module samples |
| `BUILD_APPLICATIONS` | `ON` | Build applications |
| `BUILD_ALPHA` | `OFF` | Build alpha development modules |
| `ENABLE_LOGGING` | `ON` | Enable internal debug logging |

## Example configurations

### Minimum build (base only)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_MODULES=OFF -DBUILD_MODULE_base=ON
```

### Networking build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_MODULES=OFF -DBUILD_TESTS=ON \
  -DBUILD_MODULE_base=ON \
  -DBUILD_MODULE_crypto=ON \
  -DBUILD_MODULE_net=ON
```

### HTTP build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_MODULES=OFF -DBUILD_TESTS=ON -DBUILD_SAMPLES=ON \
  -DBUILD_MODULE_base=ON -DBUILD_MODULE_crypto=ON \
  -DBUILD_MODULE_json=ON -DBUILD_MODULE_net=ON \
  -DBUILD_MODULE_http=ON
```

### Media build (with FFmpeg)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_MODULES=OFF -DBUILD_TESTS=ON \
  -DWITH_FFMPEG=ON \
  -DBUILD_MODULE_base=ON -DBUILD_MODULE_av=ON
```

## Installing

```bash
cmake --install build --prefix /usr/local
```

## Docker

```bash
docker build -t libsourcey .
```

## External module plugin pattern

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

## Generating API docs

```bash
# Requires: doxygen, npx (Node.js)
doxygen Doxyfile
npx moxygen --groups --output=./doc/api-%s.md ./build/doxygen/xml
```
