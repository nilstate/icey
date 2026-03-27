# Speech

> Audio intelligence primitives for decoded media streams.

**[Source →](../../src/speech/)** · **CMake target:** `icey::speech`

## Overview

The `speech` module is the audio-side intelligence branch for Icey. It stays transport agnostic on purpose: feed it decoded `PlanarAudioPacket` instances from any capture, file, WebRTC, or HTTP pipeline, and it emits structured events back out as plain `json::Value`.

The first landing is intentionally narrow:

- `SpeechQueue` is the explicit async clone boundary for decoded audio packets.
- `VoiceActivityDetector` performs lightweight level-based speech detection without introducing a model runtime.
- `SpeechEvent` and `AudioRef` serialize directly to JSON for existing Symple, WebSocket, or HTTP event planes.

This module is audio-only. Video intelligence belongs in the sibling `vision` module rather than a combined `ai` surface.

## Build And Dependencies

`speech` depends on `base`, `av`, and `json`.

- `av` must be available, so FFmpeg-backed packet types are present.
- No OpenCV dependency is required.
- No Python runtime or external inference stack is required.

The module builds only when `icey::av` is available.

## Architecture

The module is built around one explicit rule: keep the hot path borrowed and zero-copy until you actually cross an async boundary.

```text
decoded PlanarAudioPacket
        │
        ├── synchronous branch (borrowed packet)
        │
        └── SpeechQueue  ── clone boundary ──► worker / detector
                                              │
                                              └── SpeechEvent -> json::Value
```

That makes it safe to branch off a live send or record pipeline without forcing the transport path to pay for queue retention unless you asked for it.

## Core Types

- `speech::SpeechQueue`: `AsyncPacketQueue<av::PlanarAudioPacket>` specialized for speech work.
- `speech::VoiceActivityDetector`: lightweight energy-based VAD with hysteresis and update throttling.
- `speech::SpeechEvent`: structured event payload with detector metadata, audio timing, and activity levels.

## Usage

### Queue and VAD

```cpp
#include "icy/speech/speechqueue.h"
#include "icy/speech/voiceactivitydetector.h"

speech::SpeechQueue queue(32);
speech::VoiceActivityDetector vad({
    .source = "mic/front",
    .sampleRateHint = 48000,
    .startThreshold = 0.045f,
    .stopThreshold = 0.020f,
    .minSilenceUsec = 250000,
    .updateIntervalUsec = 250000,
});

queue.emitter += [&](IPacket& packet) {
    vad.process(static_cast<av::PlanarAudioPacket&>(packet));
};

vad.Event += [&](const speech::SpeechEvent& event) {
    auto json = speech::toJson(event);
    publish(json);
};
```

### Where it fits

Common placements:

- decoded microphone ingest before ASR or recording
- decoded browser audio receive path
- decoded file or RTSP audio branch
- side-channel activity events for a browser UI or automation system

If you need packet timing and ownership rules first, read [PacketStream](../concepts/packetstream.md).
