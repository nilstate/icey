# Media Recorder

Receives H.264 video from a browser peer via WebRTC, decodes it with FFmpeg, and writes an MP4 recording.

Pipeline: `[browser] → WebRtcTrackReceiver → VideoDecoder → MultiplexPacketEncoder → file`

The recorder lazy-initializes the MP4 muxer on the first decoded frame so the output dimensions and pixel format come from the actual stream, not hard-coded guesses. If the browser joins mid-GOP, the sample waits for the first decodable H.264 keyframe before opening the file.

## Build

Built automatically when `-DBUILD_SAMPLES=ON -DWITH_LIBDATACHANNEL=ON -DWITH_FFMPEG=ON` is passed to CMake.

## Requirements

- FFmpeg
- Running Symple server
- A browser peer sending video (e.g. using symple-player's WebRTCPlayer)

## Usage

```
./media-recorder -output recording.mp4 -host 127.0.0.1 -port 4500 -user recorder
```

Connect from a browser and send video. The recorder writes `recording.mp4` once the first decodable H.264 keyframe arrives.
