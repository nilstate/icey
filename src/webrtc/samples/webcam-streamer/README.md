# Webcam Streamer

Streams video from a camera (or test file) to a browser peer via WebRTC. Encodes to H.264 and sends through the PacketStream pipeline.

Pipeline: `MediaCapture → VideoPacketEncoder → WebRtcTrackSender → [browser]`

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake and the `webrtc` module prerequisites are available.

## Requirements

- Camera (or set `USE_CAMERA` to `0` for test file)
- FFmpeg with libx264 encoder
- Running Symple server
- A browser client that can place a Symple-backed WebRTC call

## Usage

```
./webcam-streamer -host 127.0.0.1 -port 4500 -user streamer -name "Webcam"
```

Connect from a browser using `symple-player`'s CallManager. The streamer auto-accepts incoming calls and begins sending H.264 video.

Set `USE_CAMERA` to `1` in the source to stream from a camera instead of the test file.

## Operational Notes

- The sample joins the `public` room after coming online.
- It builds the media pipeline once and starts capture only when `PeerSession` reaches `Active`.
- If you are debugging signalling, check that the browser can see the streamer's presence before you touch the media path.
- If you are debugging media, switch to the test file first so camera negotiation is not part of the problem.

## See Also

- [Webcam To Browser recipe](../../../../docs/recipes/webrtc-webcam-to-browser.md)
- [WebRTC guide](../../../../docs/modules/webrtc.md)
- [PacketStream concept](../../../../docs/concepts/packetstream.md)
