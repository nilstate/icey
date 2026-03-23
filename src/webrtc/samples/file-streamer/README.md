# File Streamer

Streams a media file (MP4, MKV, etc.) to a browser peer via WebRTC. Loops continuously. Includes a data channel for control messages (seek, pause).

Pipeline: `MediaCapture (file) → WebRtcTrackSender → [browser]`

> **Note:** The encode stage (VideoEncoder) is not yet wired in. Currently, decoded frames from MediaCapture feed directly to the RTP sender. A VideoEncoder processor needs to be inserted to produce the H.264 NAL units the packetizer expects.

## Build

Built automatically when `-DBUILD_SAMPLES=ON -DWITH_LIBDATACHANNEL=ON -DWITH_FFMPEG=ON` is passed to CMake.

## Requirements

- Media file (defaults to test.mp4)
- FFmpeg
- Running Symple server

## Usage

```
./file-streamer -file /path/to/video.mp4 -host 127.0.0.1 -port 4500 -user streamer
```

Connect from a browser. The streamer auto-accepts and loops the file.
