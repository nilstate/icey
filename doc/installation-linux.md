# Installing on Linux

## Install Dependencies

Install the core dependencies:

```bash
sudo apt-get update
sudo apt-get install -y build-essential pkg-config git cmake openssl libssl-dev
```

### Install FFmpeg (optional)

FFmpeg is an optional but recommended dependency that's required to make use of Icey's media encoding capabilities.

On most Linux distributions you can install FFmpeg development libraries from the package manager:

```bash
# Ubuntu/Debian
sudo apt-get install -y libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libswresample-dev libswscale-dev libpostproc-dev

# Fedora/RHEL
sudo dnf install ffmpeg-devel
```

Alternatively you can compile FFmpeg from source following the [official guide](https://trac.ffmpeg.org/wiki/CompilationGuide).

If FFmpeg is installed in a non-standard location you can expose it to the CMake build system like so:

```bash
cmake .. -DWITH_FFMPEG=ON -DFFMPEG_ROOT_DIR=<PATH_TO_FFMPEG_BUILD_DIRECTORY>
```

### Install OpenCV (optional)

OpenCV is an optional dependency used by Icey for video capture, video analysis and computer vision algorithms.

```bash
# Ubuntu/Debian
sudo apt-get install -y libopencv-dev
```

If you prefer to build from source:

```bash
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DWITH_FFMPEG=OFF ..
make -j $(nproc)
sudo make install
sudo ldconfig
```

Note: if you're compiling FFmpeg yourself (as above), build OpenCV with `WITH_FFMPEG=OFF` to avoid conflicting FFmpeg libraries on your system.

Enable OpenCV in the Icey build:

```bash
cmake .. -DWITH_OPENCV=ON
```

### Install Icey

```bash
git clone https://github.com/sourcey/icey.git
cd icey
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE # extra cmake commands here...
make
sudo make install
```

[See here](installation.md#cmake-build-options) for a complete list of build options.
