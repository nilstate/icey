# Speech

Low-latency audio intelligence primitives for Icey.

The first landing is intentionally narrow:

- `SpeechQueue` is the explicit async clone boundary for decoded audio.
- `VoiceActivityDetector` performs lightweight level-based speech detection
  without pulling in external ML runtimes.
- `SpeechEvent` serializes directly to `json::Value` so the app layer can
  publish events over existing transport surfaces.

This module is audio-only. Video intelligence belongs in the sibling
`vision` module rather than a combined `ai` surface.
