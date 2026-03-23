# Media Recorder

Receives video from a browser peer via WebRTC and demonstrates the receive path. Currently logs received frame sizes and timestamps to prove the path works.

Pipeline (current): `[browser] → WebRtcTrackReceiver → log output`

> **Note:** The decode and mux stages (VideoDecoder, MultiplexEncoder) are not yet wired in. The sample receives raw encoded packets and logs them. Writing to a proper container file requires inserting a decoder and MultiplexEncoder into the pipeline.

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

Connect from a browser and send video. The recorder logs received frame sizes and timestamps.
