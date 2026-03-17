## Installation

LibSourcey uses `CMake` to generate the project files for your compiler, and the codebase is cross platform so it should compile on any system with a C++20 compiler. LibSourcey has been successfully compiled and used on the following platforms:

* Linux (`gcc` >= 10, `clang` >= 10)
* macOS (`Xcode`, `clang`)
* Windows (`msvc`, `msys2`)

Please refer to the platform specific installation guides:

* [Linux](installation-linux.md)
* [macOS](installation-osx.md)
* [Windows](installation-windows.md)

## Dependencies

* **OpenSSL** (required, external)
  Required by the `net` and `crypto` modules for SSL networking, encryption and cryptography
* **FFmpeg** (optional, external)
  Required to compile media related encoding, decoding, device and capture classes in the `av` module
* **OpenCV** (>= 3.0, optional, external)
  Video capture and computer vision algorithms

## CMake Build Options

The main build options you will want to configure are as follows:

* `BUILD_SHARED_LIBS`: Build shared libraries (.dll/.so) instead of static ones (.lib/.a). _default: OFF_
* `BUILD_MODULES`: Build LibSourcey modules. _default: ON_
* `BUILD_MODULE_xxx`: Enable or disable a specific module replacing `xxx` with the module name.
* `BUILD_APPLICATIONS`: Build LibSourcey applications. _default: ON_
* `BUILD_TESTS`: Build module test applications. _default: OFF_
* `BUILD_SAMPLES`: Build module sample applications. _default: OFF_
* `WITH_OPENSSL`: Enable OpenSSL support. _default: ON_
* `WITH_FFMPEG`: Enable FFmpeg support. _default: OFF_
* `WITH_OPENCV`: Enable OpenCV support. _default: OFF_
* `ENABLE_LOGGING`: Enable internal debug logging. _default: OFF_

If you are using third-party libraries in custom locations then make sure you update the CMake include paths: `CMAKE_SYSTEM_PREFIX_PATH` and `CMAKE_LIBRARY_PATH`.

For an exhaustive list of options check the `CMakeLists.txt` in the main directory.

## Building Packages

LibSourcey can be packaged into `deb`, `rpm`, `tar.gz`, `zip` and many other formats using CPack:

```bash
cd libsourcey/build
cmake .. -DCPACK_GENERATOR=DEB # other build commands here
sudo cpack ..
```

The output package will be in the `build` folder.

Check the [CPack documentation](https://cmake.org/cmake/resources/software.html) for a full list of supported package generators.

## CMake Build Shortcuts

Below are a number of build shortcuts that can be used to build LibSourcey with different features enabled:

```bash
# Default debug build
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF \
         -DBUILD_MODULES=ON -DBUILD_APPLICATIONS=ON \
         -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON \
         -DWITH_FFMPEG=ON

# Minimum build (base only)
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_MODULES=OFF \
         -DBUILD_MODULE_base=ON

# Media build
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF \
         -DBUILD_APPLICATIONS=OFF -DBUILD_MODULES=OFF \
         -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON \
         -DWITH_FFMPEG=ON -DWITH_OPENCV=OFF \
         -DBUILD_MODULE_av=ON -DBUILD_MODULE_base=ON

# Symple build (no media)
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF \
         -DBUILD_APPLICATIONS=ON -DBUILD_MODULES=OFF \
         -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON \
         -DWITH_FFMPEG=OFF -DWITH_OPENCV=OFF \
         -DBUILD_MODULE_archo=ON -DBUILD_MODULE_base=ON \
         -DBUILD_MODULE_crypto=ON -DBUILD_MODULE_http=ON \
         -DBUILD_MODULE_json=ON -DBUILD_MODULE_net=ON \
         -DBUILD_MODULE_socketio=ON -DBUILD_MODULE_symple=ON \
         -DBUILD_MODULE_util=ON

# Symple build (with media)
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF \
         -DBUILD_APPLICATIONS=ON -DBUILD_MODULES=OFF \
         -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON \
         -DWITH_FFMPEG=ON -DWITH_OPENCV=OFF \
         -DBUILD_MODULE_archo=ON -DBUILD_MODULE_base=ON \
         -DBUILD_MODULE_crypto=ON -DBUILD_MODULE_http=ON \
         -DBUILD_MODULE_json=ON -DBUILD_MODULE_av=ON \
         -DBUILD_MODULE_net=ON -DBUILD_MODULE_socketio=ON \
         -DBUILD_MODULE_symple=ON -DBUILD_MODULE_util=ON

# HTTP build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=OFF \
         -DBUILD_APPLICATIONS=ON -DBUILD_MODULES=OFF \
         -DBUILD_SAMPLES=ON -DBUILD_TESTS=ON \
         -DBUILD_MODULE_archo=ON -DBUILD_MODULE_base=ON \
         -DBUILD_MODULE_crypto=ON -DBUILD_MODULE_http=ON \
         -DBUILD_MODULE_json=ON -DBUILD_MODULE_net=ON \
         -DBUILD_MODULE_util=ON

# STUN/TURN build
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF \
         -DBUILD_APPLICATIONS=ON -DBUILD_MODULES=OFF \
         -DBUILD_MODULE_base=ON -DBUILD_MODULE_crypto=ON \
         -DBUILD_MODULE_net=ON -DBUILD_MODULE_stun=ON \
         -DBUILD_MODULE_turn=ON
```
