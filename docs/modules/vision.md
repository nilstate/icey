# Vision

> Video intelligence primitives for sampled decoded frames.

**[Source →](../../src/vision/)** · **CMake target:** `icey::vision`

## Overview

The `vision` module is the decoded-frame intelligence branch for Icey. It is designed to sit beside browser delivery, recording, or relay flows rather than in front of them. Feed it decoded `PlanarVideoPacket` frames from any transport path and it emits structured detection events without changing the media plane.

The first landing is intentionally narrow:

- `FrameSampler` keeps the synchronous hot path zero-copy.
- `DetectionQueue` is the explicit async clone boundary.
- `MotionDetector` provides a minimal built-in detector without external dependencies.
- `VisionEvent` and `Detection` serialize directly to JSON for existing Symple, WebSocket, or HTTP event planes.

This module is video-only. Audio intelligence belongs in the sibling `speech` module.

## Build And Dependencies

`vision` depends on `base`, `av`, and `json`.

- `av` must be available, so decoded planar video packets exist.
- OpenCV is optional future backend territory, not a hard dependency of the module.
- No external inference runtime is required for the first cut.

The module builds only when `icey::av` is available.

## Architecture

The design goal is explicit clone boundaries.

```text
decoded PlanarVideoPacket
        │
        ├── FrameSampler  ── zero-copy sampled branch
        │
        └── DetectionQueue ── clone boundary ──► detector
                                                │
                                                └── VisionEvent -> json::Value
```

That lets the main encode/send path stay untouched while the analysis branch samples and queues only the work you asked for.

## Core Types

- `vision::FrameSampler`: `PacketProcessor` that forwards every `N`th frame and/or enforces a minimum interval.
- `vision::DetectionQueue`: `AsyncPacketQueue<av::PlanarVideoPacket>` specialized for detection work.
- `vision::MotionDetector`: grid-based luma motion detector for low-overhead event generation.
- `vision::VisionEvent`: structured event payload with frame metadata and detection results.

## Usage

### Sample, queue, detect

```cpp
#include "icy/vision/framesampler.h"
#include "icy/vision/detectionqueue.h"
#include "icy/vision/motiondetector.h"

vision::FrameSampler sampler({
    .everyNthFrame = 6,
    .minIntervalUsec = 200000,
});
vision::DetectionQueue queue(8);
vision::MotionDetector detector({
    .source = "camera/front",
    .gridWidth = 32,
    .gridHeight = 18,
    .threshold = 0.08f,
});

sampler.emitter += [&](IPacket& packet) { queue.process(packet); };
queue.emitter += [&](IPacket& packet) {
    detector.process(static_cast<av::PlanarVideoPacket&>(packet));
};

detector.Event += [&](const vision::VisionEvent& event) {
    auto json = vision::toJson(event);
    publish(json);
};
```

### Where it fits

Common placements:

- RTSP-to-browser pipelines with side-channel motion or detection events
- browser-originated receive paths that need snapshots or triggers
- file or camera ingest pipelines that need live intelligence without blocking delivery

If you need the pipeline ownership rules first, read [PacketStream](../concepts/packetstream.md).
