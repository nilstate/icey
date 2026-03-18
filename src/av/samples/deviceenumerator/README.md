# Device Enumerator

Lists all video/audio capture and output devices with their capabilities (resolutions, frame rates, sample rates), available hardware codecs, and FFmpeg input/output formats.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- FFmpeg libraries

## Usage

```
./deviceenumerator
```

Prints detected cameras (with video capabilities), microphones (with audio capabilities), speakers, hardware-accelerated codecs, and all compiled-in FFmpeg demuxer/muxer formats.
