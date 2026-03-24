# File Streamer

Streams a media file (MP4, MKV, etc.) to a browser peer via WebRTC. Loops continuously. Includes a data channel for control messages.

Pipeline: `MediaCapture (file) → VideoPacketEncoder → WebRtcTrackSender → [browser]`

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake and the `webrtc` module prerequisites are available.

## Requirements

- Media file (defaults to test.mp4)
- FFmpeg
- Running Symple server

## Usage

```
./file-streamer -file /path/to/video.mp4 -host 127.0.0.1 -port 4500 -user streamer
```

Connect from a browser. The streamer auto-accepts incoming calls and loops the file.
