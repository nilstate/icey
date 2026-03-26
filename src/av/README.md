# AV Module

FFmpeg-based audio/video capture, encoding, decoding, device management, and PacketStream integration.

- Namespace: `icy::av`
- CMake target: `icey::av`
- Primary headers: `include/icy/av/av.h`, `videocapture.h`, `audiocapture.h`, `multiplexencoder.h`
- Directory layout: `include/` for the public API, `src/` for FFmpeg and platform backends, `samples/` for runnable media demos, `tests/` for codec and pipeline coverage
- Platform backends live under `include/icy/av/linux/`, `apple/`, and `win32/`

Read next:
- [AV module guide](../../docs/modules/av.md)
- [Media server stack recipe](../../docs/recipes/media-server-stack.md)
- [WebRTC webcam-to-browser recipe](../../docs/recipes/webrtc-webcam-to-browser.md)
