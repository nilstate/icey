# Media Recorder

Receives H.264 video from a browser peer via WebRTC, decodes it with FFmpeg, and writes an MP4 recording.

Pipeline: `[browser] → WebRtcTrackReceiver → VideoDecoder → MultiplexPacketEncoder → file`

The recorder lazy-initializes the MP4 muxer on the first decoded frame so the output dimensions and pixel format come from the actual stream, not hard-coded guesses. If the browser joins mid-GOP, the sample waits for the first decodable H.264 keyframe before opening the file.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake and the `webrtc` module prerequisites are available.

## Requirements

- FFmpeg
- Running Symple server
- A browser peer sending video (e.g. using `symple-client-player`)

## Usage

```
./media-recorder -output recording.mp4 -host 127.0.0.1 -port 4500 -user recorder
```

Connect from a browser and send video. The recorder writes `recording.mp4` once the first decodable H.264 keyframe arrives.

## Operational Notes

- The sample is receive-only, but it still configures the codec it expects to receive. Codec setup is strict now.
- The MP4 mux is opened lazily on the first decodable frame so the file shape comes from the actual stream.
- If the caller joins mid-GOP, the recorder will wait for the first usable keyframe instead of producing a broken file.
- If you are not getting output, debug in this order: signalling, call activation, encoded packets arriving, decodable keyframe arriving.

## See Also

- [Browser To Recorder recipe](../../../../doc/recipes/webrtc-browser-to-recorder.md)
- [WebRTC guide](../../../../doc/modules/webrtc.md)
- [PacketStream concept](../../../../doc/concepts/packetstream.md)
