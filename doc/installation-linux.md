# Installing on Linux

## Dependencies

```bash
sudo apt-get update
sudo apt-get install -y build-essential pkg-config git cmake libssl-dev
```

### FFmpeg (optional)

Install FFmpeg development libraries to enable the `av` module:

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

### OpenCV (optional)

```bash
sudo apt-get install -y libopencv-dev
```

## Build and Install

```bash
git clone https://github.com/sourcey/icey.git
cd icey
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
sudo cmake --install build
```

See [installation.md](installation.md) for the full list of CMake options.
