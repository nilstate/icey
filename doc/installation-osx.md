# Installing on macOS

Uses [Homebrew](https://brew.sh). Works on both Intel and Apple Silicon.

## Dependencies

```bash
brew install cmake openssl
```

### FFmpeg (optional)

```bash
brew install ffmpeg
```

## Build and Install

```bash
git clone https://github.com/sourcey/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON \
  -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
cmake --build build --parallel $(sysctl -n hw.ncpu)
ctest --test-dir build --output-on-failure
sudo cmake --install build
```

## Xcode

Generate an Xcode project instead of Makefiles:

```bash
cmake -B build -G Xcode -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
open build/icey.xcodeproj
```

See [installation.md](installation.md) for the full list of CMake options.
