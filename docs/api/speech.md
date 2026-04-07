{#speechmodule}

# speech

Audio intelligence primitives for decoded media streams.

### Namespaces

| Name | Description |
|------|-------------|
| [`speech`](#speech) |  |

{#speech}

# speech

### Classes

| Name | Description |
|------|-------------|
| [`SpeechQueue`](#speechqueue) | Async queue for decoded audio packets. |
| [`VoiceActivityDetector`](#voiceactivitydetector) |  |
| [`AudioRef`](#audioref) |  |
| [`SpeechEvent`](#speechevent) |  |
| [`VoiceActivityDetectorConfig`](#voiceactivitydetectorconfig) |  |
| [`VoiceActivityDetectorStats`](#voiceactivitydetectorstats) |  |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `Speech_APIAudioRef` | [`makeAudioRef`](#makeaudioref)  |  |
| `Speech_APIjson::Value` | [`toJson`](#tojson-1)  |  |
| `Speech_APIjson::Value` | [`toJson`](#tojson-2)  |  |

---

{#makeaudioref}

#### makeAudioRef

```cpp
Speech_APIAudioRef makeAudioRef(const av::PlanarAudioPacket & packet, uint64_t sequence, int sampleRateHint)
```

---

{#tojson-1}

#### toJson

```cpp
Speech_APIjson::Value toJson(const AudioRef & audio)
```

---

{#tojson-2}

#### toJson

```cpp
Speech_APIjson::Value toJson(const SpeechEvent & event)
```

{#speechqueue}

## SpeechQueue

```cpp
#include <icy/speech/speechqueue.h>
```

> **Inherits:** [`PlanarAudioPacket >`](base.md#asyncpacketqueue)

Async queue for decoded audio packets.

This is the explicit clone boundary for speech processing. Upstream stages can stay borrowed/zero-copy until a packet enters this queue.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`SpeechQueue`](#speechqueue-1) `inline` `explicit` |  |

---

{#speechqueue-1}

#### SpeechQueue

`inline` `explicit`

```cpp
inline explicit SpeechQueue(int maxPackets)
```

{#voiceactivitydetector}

## VoiceActivityDetector

```cpp
#include <icy/speech/voiceactivitydetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(const SpeechEvent &)>` | [`Event`](#event-1)  |  |

---

{#event-1}

#### Event

```cpp
Signal< void(const SpeechEvent &)> Event
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VoiceActivityDetector`](#voiceactivitydetector-1) `explicit` |  |
| `void` | [`setConfig`](#setconfig)  |  |
| `VoiceActivityDetectorConfig` | [`config`](#config) `const` |  |
| `VoiceActivityDetectorStats` | [`stats`](#stats) `const` |  |
| `void` | [`reset`](#reset-14)  |  |
| `void` | [`process`](#process-7)  |  |

---

{#voiceactivitydetector-1}

#### VoiceActivityDetector

`explicit`

```cpp
explicit VoiceActivityDetector(VoiceActivityDetectorConfig config)
```

---

{#setconfig}

#### setConfig

```cpp
void setConfig(VoiceActivityDetectorConfig config)
```

---

{#config}

#### config

`const`

```cpp
VoiceActivityDetectorConfig config() const
```

---

{#stats}

#### stats

`const`

```cpp
VoiceActivityDetectorStats stats() const
```

---

{#reset-14}

#### reset

```cpp
void reset()
```

---

{#process-7}

#### process

```cpp
void process(const av::PlanarAudioPacket & packet)
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `VoiceActivityDetectorConfig` | [`_config`](#_config)  |  |
| `uint64_t` | [`_sequence`](#_sequence)  |  |
| `uint64_t` | [`_seen`](#_seen)  |  |
| `uint64_t` | [`_emitted`](#_emitted)  |  |
| `int64_t` | [`_lastUpdateUsec`](#_lastupdateusec)  |  |
| `int64_t` | [`_lastBelowThresholdUsec`](#_lastbelowthresholdusec)  |  |
| `bool` | [`_active`](#_active-1)  |  |
| `float` | [`_lastLevel`](#_lastlevel)  |  |
| `float` | [`_lastPeak`](#_lastpeak)  |  |

---

{#_config}

#### _config

```cpp
VoiceActivityDetectorConfig _config
```

---

{#_sequence}

#### _sequence

```cpp
uint64_t _sequence = 0
```

---

{#_seen}

#### _seen

```cpp
uint64_t _seen = 0
```

---

{#_emitted}

#### _emitted

```cpp
uint64_t _emitted = 0
```

---

{#_lastupdateusec}

#### _lastUpdateUsec

```cpp
int64_t _lastUpdateUsec = 0
```

---

{#_lastbelowthresholdusec}

#### _lastBelowThresholdUsec

```cpp
int64_t _lastBelowThresholdUsec = 0
```

---

{#_active-1}

#### _active

```cpp
bool _active = false
```

---

{#_lastlevel}

#### _lastLevel

```cpp
float _lastLevel = 0.0f
```

---

{#_lastpeak}

#### _lastPeak

```cpp
float _lastPeak = 0.0f
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `Levels` | [`measure`](#measure) `const` |  |
| `void` | [`emitEvent`](#emitevent)  |  |

---

{#measure}

#### measure

`const`

```cpp
Levels measure(const av::PlanarAudioPacket & packet) const
```

---

{#emitevent}

#### emitEvent

```cpp
void emitEvent(const char * type, const av::PlanarAudioPacket & packet, const Levels & levels, bool active)
```

### Private Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `VoiceActivityDetectorConfig` | [`sanitize`](#sanitize) `static` |  |

---

{#sanitize}

#### sanitize

`static`

```cpp
static VoiceActivityDetectorConfig sanitize(VoiceActivityDetectorConfig config)
```

{#levels}

## Levels

```cpp
#include <icy/speech/voiceactivitydetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `float` | [`level`](#level-2)  |  |
| `float` | [`peak`](#peak-1)  |  |
| `bool` | [`valid`](#valid-9)  |  |

---

{#level-2}

#### level

```cpp
float level = 0.0f
```

---

{#peak-1}

#### peak

```cpp
float peak = 0.0f
```

---

{#valid-9}

#### valid

```cpp
bool valid = false
```

{#audioref}

## AudioRef

```cpp
#include <icy/speech/event.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`sequence`](#sequence)  |  |
| `int64_t` | [`timeUsec`](#timeusec)  |  |
| `int64_t` | [`durationUsec`](#durationusec)  |  |
| `int` | [`sampleRate`](#samplerate-2)  |  |
| `int` | [`channels`](#channels-3)  |  |
| `size_t` | [`numSamples`](#numsamples-1)  |  |
| `std::string` | [`sampleFmt`](#samplefmt-2)  |  |

---

{#sequence}

#### sequence

```cpp
uint64_t sequence = 0
```

---

{#timeusec}

#### timeUsec

```cpp
int64_t timeUsec = 0
```

---

{#durationusec}

#### durationUsec

```cpp
int64_t durationUsec = 0
```

---

{#samplerate-2}

#### sampleRate

```cpp
int sampleRate = 0
```

---

{#channels-3}

#### channels

```cpp
int channels = 0
```

---

{#numsamples-1}

#### numSamples

```cpp
size_t numSamples = 0
```

---

{#samplefmt-2}

#### sampleFmt

```cpp
std::string sampleFmt
```

{#speechevent}

## SpeechEvent

```cpp
#include <icy/speech/event.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`type`](#type-18)  |  |
| `std::string` | [`source`](#source)  |  |
| `std::string` | [`detector`](#detector)  |  |
| `int64_t` | [`emittedAtUsec`](#emittedatusec)  |  |
| `AudioRef` | [`audio`](#audio-3)  |  |
| `bool` | [`active`](#active-1)  |  |
| `float` | [`level`](#level-1)  |  |
| `float` | [`peak`](#peak)  |  |
| `json::Value` | [`data`](#data-3)  |  |

---

{#type-18}

#### type

```cpp
std::string type = "speech:update"
```

---

{#source}

#### source

```cpp
std::string source
```

---

{#detector}

#### detector

```cpp
std::string detector
```

---

{#emittedatusec}

#### emittedAtUsec

```cpp
int64_t emittedAtUsec = 0
```

---

{#audio-3}

#### audio

```cpp
AudioRef audio
```

---

{#active-1}

#### active

```cpp
bool active = false
```

---

{#level-1}

#### level

```cpp
float level = 0.0f
```

---

{#peak}

#### peak

```cpp
float peak = 0.0f
```

---

{#data-3}

#### data

```cpp
json::Value data = json::Value::object()
```

{#voiceactivitydetectorconfig}

## VoiceActivityDetectorConfig

```cpp
#include <icy/speech/voiceactivitydetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`source`](#source-1)  |  |
| `std::string` | [`detectorName`](#detectorname)  |  |
| `int` | [`sampleRateHint`](#sampleratehint)  |  |
| `float` | [`startThreshold`](#startthreshold)  |  |
| `float` | [`stopThreshold`](#stopthreshold)  |  |
| `int64_t` | [`minSilenceUsec`](#minsilenceusec)  |  |
| `int64_t` | [`updateIntervalUsec`](#updateintervalusec)  |  |

---

{#source-1}

#### source

```cpp
std::string source
```

---

{#detectorname}

#### detectorName

```cpp
std::string detectorName = "energy_vad"
```

---

{#sampleratehint}

#### sampleRateHint

```cpp
int sampleRateHint = 0
```

---

{#startthreshold}

#### startThreshold

```cpp
float startThreshold = 0.045f
```

---

{#stopthreshold}

#### stopThreshold

```cpp
float stopThreshold = 0.020f
```

---

{#minsilenceusec}

#### minSilenceUsec

```cpp
int64_t minSilenceUsec = 250000
```

---

{#updateintervalusec}

#### updateIntervalUsec

```cpp
int64_t updateIntervalUsec = 250000
```

{#voiceactivitydetectorstats}

## VoiceActivityDetectorStats

```cpp
#include <icy/speech/voiceactivitydetector.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`seen`](#seen)  |  |
| `uint64_t` | [`emitted`](#emitted)  |  |
| `bool` | [`active`](#active-2)  |  |
| `float` | [`lastLevel`](#lastlevel)  |  |
| `float` | [`lastPeak`](#lastpeak)  |  |

---

{#seen}

#### seen

```cpp
uint64_t seen = 0
```

---

{#emitted}

#### emitted

```cpp
uint64_t emitted = 0
```

---

{#active-2}

#### active

```cpp
bool active = false
```

---

{#lastlevel}

#### lastLevel

```cpp
float lastLevel = 0.0f
```

---

{#lastpeak}

#### lastPeak

```cpp
float lastPeak = 0.0f
```

