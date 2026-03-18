# File Streamer

Streams a media file (MP4, MKV, etc.) to a browser peer via WebRTC. Loops continuously. Includes a data channel for control messages (seek, pause).

Pipeline: `MediaCapture (file) → VideoEncoder → WebRtcTrackSender → [browser]`

## Build

Built automatically when `-DBUILD_SAMPLES=ON -DWITH_LIBDATACHANNEL=ON -DWITH_FFMPEG=ON` is passed to CMake.

## Requirements

- Media file (defaults to test.mp4)
- FFmpeg with libx264 encoder
- Running Symple server

## Usage

```
./file-streamer -file /path/to/video.mp4 -host 127.0.0.1 -port 4500 -user streamer
```

Connect from a browser. The streamer auto-accepts and loops the file.
