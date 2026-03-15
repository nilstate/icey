# Installing on macOS

This guide uses [Homebrew](https://brew.sh). Works on both Intel and Apple Silicon Macs.

## Install Dependencies

```bash
brew install cmake openssl
```

CMake should find OpenSSL automatically. If not, set the path:

```bash
cmake .. -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

For optional dependencies like FFmpeg, see the [Linux instructions](installation-linux.md#install-ffmpeg-optional) - the same `cmake` flags apply.

## Install LibSourcey

```bash
git clone https://github.com/sourcey/libsourcey.git
cd libsourcey
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE # extra cmake commands here...
make
sudo make install
```

## Compile with Xcode

* Generate an Xcode project: `cmake .. -G Xcode`
* Launch Xcode, open `libsourcey.xcodeproj`
* Select "Debug", build (Cmd-B), then select "Release" and build again
