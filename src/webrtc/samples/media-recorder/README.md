# Media Recorder

Receives video from a browser peer via WebRTC and records to file. Demonstrates the receive path: `WebRtcTrackReceiver → decode → record`.

Pipeline: `[browser] → WebRtcTrackReceiver → VideoDecoder → MultiplexEncoder → file`

## Build

Built automatically when `-DBUILD_SAMPLES=ON -DWITH_LIBDATACHANNEL=ON -DWITH_FFMPEG=ON` is passed to CMake.

## Requirements

- FFmpeg
- Running Symple server
- A browser peer sending video (e.g. using symple-client-player's WebRTCPlayer)

## Usage

```
./media-recorder -output recording.mp4 -host 127.0.0.1 -port 4500 -user recorder
```

Connect from a browser and send video. The recorder logs received frame sizes and timestamps. In a production setup, insert a VideoDecoder and MultiplexEncoder into the pipeline to write a proper container file.
