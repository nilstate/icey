{#visionmodule}

# vision

Video intelligence primitives for sampled frame processing and event output.

### Namespaces

| Name | Description |
|------|-------------|
| [`vision`](#vision) |  |

{#vision}

# vision

### Classes

| Name | Description |
|------|-------------|
| [`DetectionQueue`](#detectionqueue) | Async queue for decoded video frames. |
| [`FrameSampler`](#framesampler) | [PacketProcessor](base.md#packetprocessor) that forwards a sampled subset of decoded video frames. |
| [`MotionDetector`](#motiondetector) |  |
| [`Detection`](#detection) |  |
| [`FrameRef`](#frameref) |  |
| [`FrameSamplerConfig`](#framesamplerconfig) |  |
| [`FrameSamplerStats`](#framesamplerstats) |  |
| [`MotionDetectorConfig`](#motiondetectorconfig) |  |
| [`MotionDetectorStats`](#motiondetectorstats) |  |
| [`Region`](#region-1) |  |
| [`VisionEvent`](#visionevent) |  |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `Vision_APIFrameRef` | [`makeFrameRef`](#makeframeref)  |  |
| `Vision_APIjson::Value` | [`toJson`](#tojson-3)  |  |
| `Vision_APIjson::Value` | [`toJson`](#tojson-4)  |  |
| `Vision_APIjson::Value` | [`toJson`](#tojson-5)  |  |
| `Vision_APIjson::Value` | [`toJson`](#tojson-6)  |  |

---

{#makeframeref}

#### makeFrameRef

```cpp
Vision_APIFrameRef makeFrameRef(const av::PlanarVideoPacket & packet, uint64_t sequence)
```

---

{#tojson-3}

#### toJson

```cpp
Vision_APIjson::Value toJson(const Region & region)
```

---

{#tojson-4}

#### toJson

```cpp
Vision_APIjson::Value toJson(const FrameRef & frame)
```

---

{#tojson-5}

#### toJson

```cpp
Vision_APIjson::Value toJson(const Detection & detection)
```

---

{#tojson-6}

#### toJson

```cpp
Vision_APIjson::Value toJson(const VisionEvent & event)
```

{#detectionqueue}

## DetectionQueue

```cpp
#include <icy/vision/detectionqueue.h>
```

> **Inherits:** [`PlanarVideoPacket >`](base.md#asyncpacketqueue)

Async queue for decoded video frames.

This is the intentional clone boundary for detection work. Upstream stages can keep borrowed, zero-copy semantics until a frame enters this queue.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`DetectionQueue`](#detectionqueue-1) `inline` `explicit` |  |

---

{#detectionqueue-1}

#### DetectionQueue

`inline` `explicit`

```cpp
inline explicit DetectionQueue(int maxFrames)
```

{#framesampler}

## FrameSampler

```cpp
#include <icy/vision/framesampler.h>
```

> **Inherits:** [`PacketProcessor`](base.md#packetprocessor)

[PacketProcessor](base.md#packetprocessor) that forwards a sampled subset of decoded video frames.

The sampler is intentionally zero-copy on its synchronous path: sampled frames are forwarded by reference and dropped frames are discarded before any queue hop. Pair it with [DetectionQueue](#detectionqueue) when a worker-thread boundary is required.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-8)  |  |

---

{#emitter-8}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FrameSampler`](#framesampler-1) `inline` `explicit` |  |
| `void` | [`setConfig`](#setconfig-1) `inline` |  |
| `FrameSamplerConfig` | [`config`](#config-1) `const` `inline` |  |
| `FrameSamplerStats` | [`stats`](#stats-1) `const` `inline` |  |
| `void` | [`reset`](#reset-16) `inline` |  |
| `void` | [`process`](#process-8) `virtual` `inline` | This method performs processing on the given packet and emits the result. |
| `bool` | [`accepts`](#accepts-3) `virtual` `inline` | This method ensures compatibility with the given packet type. Return false to reject the packet. |

---

{#framesampler-1}

#### FrameSampler

`inline` `explicit`

```cpp
inline explicit FrameSampler(FrameSamplerConfig config)
```

---

{#setconfig-1}

#### setConfig

`inline`

```cpp
inline void setConfig(FrameSamplerConfig config)
```

---

{#config-1}

#### config

`const` `inline`

```cpp
inline FrameSamplerConfig config() const
```

---

{#stats-1}

#### stats

`const` `inline`

```cpp
inline FrameSamplerStats stats() const
```

---

{#reset-16}

#### reset

`inline`

```cpp
inline void reset()
```

---

{#process-8}

#### process

`virtual` `inline`

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Processors that defer work asynchronously must either clone the packet or retain an owned equivalent. See [retention()](base.md#classicy_1_1PacketStreamAdapter_1a37a1aa11e35a20a7c527efa78e99e928).

---

{#accepts-3}

#### accepts

`virtual` `inline`

```cpp
virtual inline bool accepts(IPacket *)
```

This method ensures compatibility with the given packet type. Return false to reject the packet.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `FrameSamplerConfig` | [`_config`](#_config-1)  |  |
| `std::atomic< uint64_t >` | [`_seen`](#_seen-1)  |  |
| `std::atomic< uint64_t >` | [`_forwarded`](#_forwarded)  |  |
| `std::atomic< uint64_t >` | [`_dropped`](#_dropped)  |  |
| `std::atomic< int64_t >` | [`_lastForwardedTimeUsec`](#_lastforwardedtimeusec)  |  |

---

{#_config-1}

#### _config

```cpp
FrameSamplerConfig _config
```

---

{#_seen-1}

#### _seen

```cpp
std::atomic< uint64_t > _seen {0}
```

---

{#_forwarded}

#### _forwarded

```cpp
std::atomic< uint64_t > _forwarded {0}
```

---

{#_dropped}

#### _dropped

```cpp
std::atomic< uint64_t > _dropped {0}
```

---

{#_lastforwardedtimeusec}

#### _lastForwardedTimeUsec

```cpp
std::atomic< int64_t > _lastForwardedTimeUsec {kUnsetTimeUsec}
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`shouldForward`](#shouldforward) `const` `inline` |  |

---

{#shouldforward}

#### shouldForward

`const` `inline`

```cpp
inline bool shouldForward(const av::PlanarVideoPacket & frame, uint64_t seen) const
```

### Private Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `FrameSamplerConfig` | [`sanitize`](#sanitize-1) `static` `inline` |  |

---

{#sanitize-1}

#### sanitize

`static` `inline`

```cpp
static inline FrameSamplerConfig sanitize(FrameSamplerConfig config)
```

{#motiondetector}

## MotionDetector

```cpp
#include <icy/vision/motiondetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(const VisionEvent &)>` | [`Event`](#event-6)  |  |

---

{#event-6}

#### Event

```cpp
Signal< void(const VisionEvent &)> Event
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MotionDetector`](#motiondetector-1) `explicit` |  |
| `void` | [`setConfig`](#setconfig-2)  |  |
| `MotionDetectorConfig` | [`config`](#config-2) `const` |  |
| `MotionDetectorStats` | [`stats`](#stats-2) `const` |  |
| `void` | [`reset`](#reset-17)  |  |
| `void` | [`process`](#process-9)  |  |

---

{#motiondetector-1}

#### MotionDetector

`explicit`

```cpp
explicit MotionDetector(MotionDetectorConfig config)
```

---

{#setconfig-2}

#### setConfig

```cpp
void setConfig(MotionDetectorConfig config)
```

---

{#config-2}

#### config

`const`

```cpp
MotionDetectorConfig config() const
```

---

{#stats-2}

#### stats

`const`

```cpp
MotionDetectorStats stats() const
```

---

{#reset-17}

#### reset

```cpp
void reset()
```

---

{#process-9}

#### process

```cpp
void process(const av::PlanarVideoPacket & packet)
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `MotionDetectorConfig` | [`_config`](#_config-2)  |  |
| `uint64_t` | [`_sequence`](#_sequence-1)  |  |
| `uint64_t` | [`_seen`](#_seen-2)  |  |
| `uint64_t` | [`_emitted`](#_emitted-1)  |  |
| `uint32_t` | [`_warmedFrames`](#_warmedframes)  |  |
| `int` | [`_width`](#_width)  |  |
| `int` | [`_height`](#_height)  |  |
| `int64_t` | [`_lastEventTimeUsec`](#_lasteventtimeusec)  |  |
| `float` | [`_lastScore`](#_lastscore)  |  |
| `std::vector< uint8_t >` | [`_previousGrid`](#_previousgrid)  |  |
| `std::vector< uint8_t >` | [`_currentGrid`](#_currentgrid)  |  |

---

{#_config-2}

#### _config

```cpp
MotionDetectorConfig _config
```

---

{#_sequence-1}

#### _sequence

```cpp
uint64_t _sequence = 0
```

---

{#_seen-2}

#### _seen

```cpp
uint64_t _seen = 0
```

---

{#_emitted-1}

#### _emitted

```cpp
uint64_t _emitted = 0
```

---

{#_warmedframes}

#### _warmedFrames

```cpp
uint32_t _warmedFrames = 0
```

---

{#_width}

#### _width

```cpp
int _width = 0
```

---

{#_height}

#### _height

```cpp
int _height = 0
```

---

{#_lasteventtimeusec}

#### _lastEventTimeUsec

```cpp
int64_t _lastEventTimeUsec = 0
```

---

{#_lastscore}

#### _lastScore

```cpp
float _lastScore = 0.0f
```

---

{#_previousgrid}

#### _previousGrid

```cpp
std::vector< uint8_t > _previousGrid
```

---

{#_currentgrid}

#### _currentGrid

```cpp
std::vector< uint8_t > _currentGrid
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`sampleLumaGrid`](#samplelumagrid) `const` |  |
| `float` | [`diffScore`](#diffscore) `const` |  |
| `void` | [`emitEvent`](#emitevent-1)  |  |

---

{#samplelumagrid}

#### sampleLumaGrid

`const`

```cpp
void sampleLumaGrid(const av::PlanarVideoPacket & packet, std::vector< uint8_t > & out) const
```

---

{#diffscore}

#### diffScore

`const`

```cpp
float diffScore(const std::vector< uint8_t > & current) const
```

---

{#emitevent-1}

#### emitEvent

```cpp
void emitEvent(const av::PlanarVideoPacket & packet, float score)
```

### Private Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `MotionDetectorConfig` | [`sanitize`](#sanitize-2) `static` |  |

---

{#sanitize-2}

#### sanitize

`static`

```cpp
static MotionDetectorConfig sanitize(MotionDetectorConfig config)
```

{#detection}

## Detection

```cpp
#include <icy/vision/event.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`label`](#label-1)  |  |
| `float` | [`confidence`](#confidence)  |  |
| `int64_t` | [`trackId`](#trackid)  |  |
| `Region` | [`region`](#region)  |  |
| `json::Value` | [`data`](#data-7)  |  |

---

{#label-1}

#### label

```cpp
std::string label
```

---

{#confidence}

#### confidence

```cpp
float confidence = 0.0f
```

---

{#trackid}

#### trackId

```cpp
int64_t trackId = -1
```

---

{#region}

#### region

```cpp
Region region
```

---

{#data-7}

#### data

```cpp
json::Value data = json::Value::object()
```

{#frameref}

## FrameRef

```cpp
#include <icy/vision/event.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`sequence`](#sequence-1)  |  |
| `int64_t` | [`timeUsec`](#timeusec-1)  |  |
| `int` | [`width`](#width-3)  |  |
| `int` | [`height`](#height-3)  |  |
| `std::string` | [`pixelFmt`](#pixelfmt-2)  |  |

---

{#sequence-1}

#### sequence

```cpp
uint64_t sequence = 0
```

---

{#timeusec-1}

#### timeUsec

```cpp
int64_t timeUsec = 0
```

---

{#width-3}

#### width

```cpp
int width = 0
```

---

{#height-3}

#### height

```cpp
int height = 0
```

---

{#pixelfmt-2}

#### pixelFmt

```cpp
std::string pixelFmt
```

{#framesamplerconfig}

## FrameSamplerConfig

```cpp
#include <icy/vision/framesampler.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`everyNthFrame`](#everynthframe)  |  |
| `int64_t` | [`minIntervalUsec`](#minintervalusec)  |  |

---

{#everynthframe}

#### everyNthFrame

```cpp
uint32_t everyNthFrame = 1
```

---

{#minintervalusec}

#### minIntervalUsec

```cpp
int64_t minIntervalUsec = 0
```

{#framesamplerstats}

## FrameSamplerStats

```cpp
#include <icy/vision/framesampler.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`seen`](#seen-1)  |  |
| `uint64_t` | [`forwarded`](#forwarded)  |  |
| `uint64_t` | [`dropped`](#dropped)  |  |

---

{#seen-1}

#### seen

```cpp
uint64_t seen = 0
```

---

{#forwarded}

#### forwarded

```cpp
uint64_t forwarded = 0
```

---

{#dropped}

#### dropped

```cpp
uint64_t dropped = 0
```

{#motiondetectorconfig}

## MotionDetectorConfig

```cpp
#include <icy/vision/motiondetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`source`](#source-2)  |  |
| `std::string` | [`detectorName`](#detectorname-1)  |  |
| `uint32_t` | [`gridWidth`](#gridwidth)  |  |
| `uint32_t` | [`gridHeight`](#gridheight)  |  |
| `uint32_t` | [`warmupFrames`](#warmupframes)  |  |
| `float` | [`threshold`](#threshold)  |  |
| `int64_t` | [`minEventIntervalUsec`](#mineventintervalusec)  |  |

---

{#source-2}

#### source

```cpp
std::string source
```

---

{#detectorname-1}

#### detectorName

```cpp
std::string detectorName = "motion"
```

---

{#gridwidth}

#### gridWidth

```cpp
uint32_t gridWidth = 32
```

---

{#gridheight}

#### gridHeight

```cpp
uint32_t gridHeight = 18
```

---

{#warmupframes}

#### warmupFrames

```cpp
uint32_t warmupFrames = 2
```

---

{#threshold}

#### threshold

```cpp
float threshold = 0.08f
```

---

{#mineventintervalusec}

#### minEventIntervalUsec

```cpp
int64_t minEventIntervalUsec = 500000
```

{#motiondetectorstats}

## MotionDetectorStats

```cpp
#include <icy/vision/motiondetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`seen`](#seen-2)  |  |
| `uint64_t` | [`emitted`](#emitted-1)  |  |
| `float` | [`lastScore`](#lastscore)  |  |

---

{#seen-2}

#### seen

```cpp
uint64_t seen = 0
```

---

{#emitted-1}

#### emitted

```cpp
uint64_t emitted = 0
```

---

{#lastscore}

#### lastScore

```cpp
float lastScore = 0.0f
```

{#region-1}

## Region

```cpp
#include <icy/vision/event.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `float` | [`x`](#x)  |  |
| `float` | [`y`](#y)  |  |
| `float` | [`width`](#width-4)  |  |
| `float` | [`height`](#height-4)  |  |

---

{#x}

#### x

```cpp
float x = 0.0f
```

---

{#y}

#### y

```cpp
float y = 0.0f
```

---

{#width-4}

#### width

```cpp
float width = 0.0f
```

---

{#height-4}

#### height

```cpp
float height = 0.0f
```

{#visionevent}

## VisionEvent

```cpp
#include <icy/vision/event.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`type`](#type-24)  |  |
| `std::string` | [`source`](#source-3)  |  |
| `std::string` | [`detector`](#detector-1)  |  |
| `int64_t` | [`emittedAtUsec`](#emittedatusec-1)  |  |
| `FrameRef` | [`frame`](#frame-2)  |  |
| `std::vector< Detection >` | [`detections`](#detections)  |  |
| `json::Value` | [`data`](#data-8)  |  |

---

{#type-24}

#### type

```cpp
std::string type = "detect"
```

---

{#source-3}

#### source

```cpp
std::string source
```

---

{#detector-1}

#### detector

```cpp
std::string detector
```

---

{#emittedatusec-1}

#### emittedAtUsec

```cpp
int64_t emittedAtUsec = 0
```

---

{#frame-2}

#### frame

```cpp
FrameRef frame
```

---

{#detections}

#### detections

```cpp
std::vector< Detection > detections
```

---

{#data-8}

#### data

```cpp
json::Value data = json::Value::object()
```

