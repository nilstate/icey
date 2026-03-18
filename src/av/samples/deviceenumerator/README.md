# Device Enumerator

Lists all audio/video capture devices and supported FFmpeg input/output formats available on the system.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- FFmpeg libraries (libavdevice, libavformat)

## Usage

```
./deviceenumerator
```

Prints detected cameras, microphones, and all compiled-in FFmpeg demuxer/muxer formats to stdout.
