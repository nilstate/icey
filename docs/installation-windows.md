# Installing on Windows

## Requirements

- [Git](https://git-scm.com/)
- [CMake 3.21+](https://cmake.org/download/)
- [Visual Studio 2022](https://visualstudio.microsoft.com/) (C++ workload)

## Dependencies

### OpenSSL

Install via [vcpkg](https://vcpkg.io/):

```powershell
vcpkg install openssl:x64-windows
```

Or download prebuilt binaries from [slproweb.com](https://slproweb.com/products/Win32OpenSSL.html) and set `OPENSSL_ROOT_DIR`.

### FFmpeg (optional)

Download development libraries from [gyan.dev](https://www.gyan.dev/ffmpeg/builds/). Add the `bin` directory to your system `PATH`, then set `FFMPEG_ROOT`:

```powershell
cmake -B build -DFFMPEG_ROOT=C:\vendor\ffmpeg
```

## Build

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

See [installation.md](installation.md) for the full list of CMake options.
