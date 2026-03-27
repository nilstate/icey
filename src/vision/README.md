# Vision

Low-latency video intelligence primitives for Icey.

The first landing is intentionally narrow:

- `FrameSampler` keeps the branch zero-copy on the synchronous hot path.
- `DetectionQueue` is the explicit async clone boundary.
- `MotionDetector` provides a minimal built-in video detector without adding
  external dependencies.
- `VisionEvent` serializes directly to `json::Value` so it can flow into
  existing Symple/WebSocket surfaces without another transport format.

This module is video-only. Audio intelligence belongs in a future sibling
module rather than a combined `ai` surface.
