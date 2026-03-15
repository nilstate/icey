# Installing on Windows

## Install Dependencies

### Install Git

[Download git-scm](https://git-scm.com/)

### Install CMake

[Download CMake](https://cmake.org/download/)

### Install OpenSSL

Install OpenSSL using [vcpkg](https://vcpkg.io/) or download prebuilt binaries from [slproweb.com](https://slproweb.com/products/Win32OpenSSL.html).

Set the `OPENSSL_ROOT_DIR` CMake variable to point to your OpenSSL installation directory and the build system will do the rest.

### Install FFmpeg (optional)

Download FFmpeg development libraries from [gyan.dev](https://www.gyan.dev/ffmpeg/builds/) (both shared and dev packages).

Add the FFmpeg `bin` directory to your system `PATH`, then set the `FFMPEG_ROOT_DIR` CMake variable:

```bash
cmake .. -DOPENSSL_ROOT_DIR=C:\vendor\OpenSSL-Win64 -DWITH_FFMPEG=ON -DFFMPEG_ROOT_DIR=C:\vendor\ffmpeg-dev
```

### Download LibSourcey

```bash
git clone https://github.com/sourcey/libsourcey.git
```

## Generate Project Files

Open the CMake GUI and set the project directory to point to the LibSourcey root directory. Execute "Configure" to do the initial configuration, then adjust any options, then press "Configure" again and then press "Generate".

[See here](installation.md#cmake-build-options) for a complete list of build options.

## Compile with Visual Studio

1. Generate solutions using CMake, as described above. Make sure you choose the proper generator (32-bit or 64-bit)
2. Launch Visual Studio, locate and open `libsourcey.sln` in your generated build folder. Select "Debug" configuration, build the solution (Ctrl-Shift-B), and/or select "Release" and build it.
3. Add `{CMAKE_BINARY_DIR}\bin\Release` and `{CMAKE_BINARY_DIR}\bin\Debug` to your system PATH.
