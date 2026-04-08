# Vision

Low-latency video intelligence primitives for Icey.

The first landing is intentionally narrow:

- `types.h` defines the stable frame, detection, track, and region model.
- `VisionEvent` serializes directly to `json::Value` so the server layer can
  ship events over existing WebSocket signalling surfaces without another
  transport format.
- `eventemitter.h` provides the narrow event signal surface detectors publish
  through.
- `FrameSampler` keeps the branch zero-copy on the synchronous hot path.
- `VisionFramePacket` is the detector-side frame contract carrying stable
  metadata.
- `FrameNormalizer` is the pre-inference seam. With default zero values it is a
  no-op metadata/materialization step; if configured it can also resize or
  normalize pixel format before inference.
- `DetectionQueue` is the explicit bounded async clone boundary.
- `MotionDetector` is the first built-in detector and now consumes
  `VisionFramePacket` instead of raw decode packets directly.

This module is video-only. Audio intelligence belongs in a future sibling
module rather than a combined `ai` surface.
