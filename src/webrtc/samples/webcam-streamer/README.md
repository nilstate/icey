# Webcam Streamer

Streams video from a camera (or test file) to a browser peer via WebRTC. Encodes to H.264 and sends through the PacketStream pipeline.

Pipeline: `MediaCapture → VideoEncoder → WebRtcTrackSender → [browser]`

## Build

Built automatically when `-DBUILD_SAMPLES=ON -DWITH_LIBDATACHANNEL=ON -DWITH_FFMPEG=ON` is passed to CMake.

## Requirements

- Camera (or set `USE_CAMERA` to `0` for test file)
- FFmpeg with libx264 encoder
- Running Symple server

## Usage

```
./webcam-streamer -host 127.0.0.1 -port 4500 -user streamer -name "Webcam"
```

Connect from a browser using `symple-client-player`'s CallManager. The streamer auto-accepts incoming calls and begins sending H.264 video.

Set `USE_CAMERA` to `1` in the source to stream from a camera instead of the test file.
