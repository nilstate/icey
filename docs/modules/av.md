# AV

> FFmpeg-backed capture, encode, decode, and mux for audio and video, wired into the PacketStream pipeline.

**[API Reference →](../api/av.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/av/)**

## Overview

The `av` module is icey's media layer. It wraps FFmpeg's libavcodec, libavformat, libswscale, and libswresample into RAII types that integrate directly with `PacketStream`. Every class either produces packets, transforms them, or consumes them. None of them require manual memory management beyond construction and wiring.

The module covers the full path from hardware device to network or file:

- **Device enumeration**: query cameras, microphones, and speakers with capability resolution
- **Device capture**: live video (V4L2, AVFoundation, DirectShow/MediaFoundation) and audio (ALSA, CoreAudio, WASAPI)
- **File and screen capture**: open any format FFmpeg understands, including screen capture devices
- **Encoding**: H.264, H.265, VP8/VP9, MJPEG, AAC, Opus, MP3, and any other codec FFmpeg supports
- **Multiplexing**: write MP4, MKV, WebM, raw streams, or custom I/O via `AVIOContext`
- **Decoding**: decompress any stream FFmpeg can read; decoders are managed automatically by `MediaCapture`
- **Format conversion**: pixel format and resolution conversion via libswscale; sample format and rate conversion via libswresample
- **Pipeline utilities**: FPS counting, FPS limiting, realtime packet scheduling

**Build requirement:** FFmpeg 5, 6, or 7. The `av` module builds automatically when FFmpeg is found by CMake. Camera and microphone capture requires the optional `avdevice` library; it is enabled automatically when `avdevice` is present (`HAVE_FFMPEG_AVDEVICE` is set). All headers guard themselves with `#ifdef HAVE_FFMPEG` so the rest of icey builds cleanly without FFmpeg.

**Link target:** `icey::av`

---

## Architecture

The module is organised around three concerns: parameter types, I/O components, and pipeline integration.

### Parameter types

`Codec`, `VideoCodec`, `AudioCodec`, and `Format` are plain structs that carry the configuration for every encoding or capture operation. They are cheap to copy and have no FFmpeg dependencies in their headers.

### I/O components

```text
ICapture  (PacketSource, Startable)
  MediaCapture  (Runnable)
    VideoCapture
    AudioCapture

IEncoder  (Stateful<EncoderState>)
  MultiplexEncoder
    MultiplexPacketEncoder  (PacketProcessor)

VideoContext
  VideoEncoder
  VideoDecoder

AudioContext
  AudioEncoder
  AudioDecoder
```

`ICapture` is the source end. It owns a `PacketSignal emitter` that `PacketStream` subscribes to. When the stream becomes active, `ICapture::onStreamStateChange()` automatically calls `start()`; when it stops, it calls `stop()`. You don't call those methods manually when using `PacketStream`.

`MultiplexEncoder` is the middle. `MultiplexPacketEncoder` adds `PacketProcessor`, so it slots directly into a `PacketStream`. Its `process()` method dispatches incoming `VideoPacket` and `AudioPacket` to the correct encode path.

### Packet type hierarchy

```text
IPacket
  RawPacket
    MediaPacket              time: int64_t (microseconds)
      VideoPacket            width, height, iframe, avpacket*
        PlanarVideoPacket    buffer[4], linesize[4], pixelFmt, avframe*
      AudioPacket            numSamples, avpacket*
        PlanarAudioPacket    buffer[4], linesize, channels, sampleFmt
```

`VideoDecoder` emits `PlanarVideoPacket`. `AudioDecoder` emits `PlanarAudioPacket`. Both interleaved and planar variants are accepted by `MultiplexPacketEncoder::process()`. The `avpacket` and `avframe` pointers in each packet type are non-owning; they alias FFmpeg's internal allocation and are only valid for the duration of the callback that receives the packet. Do not store them.

### Capture-to-network pipeline

```text
                PacketStream
                ┌─────────────────────────────────────────────────────┐
                │                                                     │
  VideoCapture ─┤                                                     │
                │──► MultiplexPacketEncoder ──► emitter ──► network  │
  AudioCapture ─┤                                                     │
                │                                                     │
                └─────────────────────────────────────────────────────┘

  VideoCapture        reads V4L2 / AVFoundation / DirectShow device
    └─ VideoDecoder   decodes device frames (YUYV, MJPEG, NV12, ...) → PlanarVideoPacket

  AudioCapture        reads ALSA / CoreAudio / WASAPI device
    └─ AudioDecoder   decodes device samples → PlanarAudioPacket

  MultiplexPacketEncoder
    ├─ VideoEncoder   libx264 / libx265 / VP9 / ... → compressed VideoPacket
    └─ AudioEncoder   AAC / Opus / MP3 / ... → compressed AudioPacket + FIFO buffer
```

### Platform backends

| Platform | Video capture | Audio capture | Device change |
| -------- | ------------- | ------------- | ------------- |
| Linux | V4L2 (`icy/av/linux/v4l2.h`) | ALSA via FFmpeg avdevice | libudev inotify (`LinuxDeviceWatcher`) |
| macOS | AVFoundation (`icy/av/apple/avfoundation.h`) | CoreAudio via AVFoundation | CoreAudio/AVFoundation callbacks |
| Windows | Media Foundation (`icy/av/win32/mediafoundation.h`) | WASAPI (`icy/av/win32/mediafoundation.h`) | `IMMNotificationClient` + `RegisterDeviceNotification` (`WindowsDeviceWatcher`) |

On Linux, V4L2 enumeration queries each `/dev/video*` node for `VIDIOC_ENUM_FMT` and `VIDIOC_ENUM_FRAMESIZES`. On macOS, AVFoundation calls `AVCaptureDevice` to populate capabilities. On Windows, Media Foundation uses `IMFActivate` for video and WASAPI's `IMMDeviceEnumerator` for audio.

FFmpeg `avdevice` translates the selected parameters into an `AVDictionary` passed to `avformat_open_input`. If the device cannot exactly satisfy a requested pixel format or resolution, the internal `VideoDecoder` applies software conversion via libswscale before emitting.

---

## Usage

### Codecs and Formats

#### Codec, VideoCodec, AudioCodec

`Codec` is the base parameter struct. `VideoCodec` and `AudioCodec` extend it with media-specific fields.

```cpp
#include "icy/av/codec.h"

// VideoCodec: named, with explicit FFmpeg encoder
av::VideoCodec video("H264", "libx264",
    /*width*/   1280,
    /*height*/  720,
    /*fps*/     30.0,
    /*bitRate*/ 400000,
    /*sampleRate (RTP clock)*/ 0,   // 0 = use default (384000)
    /*pixelFmt*/ "yuv420p");

// VideoCodec: short form (name = encoder name)
av::VideoCodec video("libx264", 1280, 720, 30.0);

// VideoCodec: inherit everything from the input (for transcoding)
av::VideoCodec video("libx264", 0, 0, 0);

// AudioCodec: named
av::AudioCodec audio("aac", /*channels*/ 2, /*sampleRate*/ 44100,
                     /*bitRate*/ 64000, /*sampleFmt*/ "fltp");

// AudioCodec: anonymous (for capture params, not encoding)
av::AudioCodec audio(/*channels*/ 2, /*sampleRate*/ 44100, "s16");
```

Passing `0` for numeric fields (width, height, fps, bitRate, channels, sampleRate) tells the encoder to inherit those values from the input format. This is the standard approach for transcoding where you want to match the source exactly.

Default constants for quick construction:

```cpp
DEFAULT_AUDIO_SAMPLE_RATE  44100
DEFAULT_AUDIO_BIT_RATE     64000
DEFAULT_AUDIO_CHANNELS     2
DEFAULT_AUDIO_SAMPLE_FMT   "s16"

DEFAULT_VIDEO_SAMPLE_RATE  384000  // RTP clock rate
DEFAULT_VIDEO_BIT_RATE     400000
DEFAULT_VIDEO_PIXEL_FMT    "yuv420p"
```

Frame rate / interval conversion:

```cpp
#include "icy/av/codec.h"

int64_t ns = av::fpsToInterval(30);      // nanoseconds per frame at 30fps
int fps     = av::intervalToFps(ns);     // back to fps
float fpsf  = av::intervalToFpsFloat(ns);
```

#### Format

`Format` pairs a container (muxer) short name with default codec parameters. `type()` returns `None`, `Video`, `Audio`, or `Multiplex` based on which codecs are enabled.

```cpp
#include "icy/av/format.h"

// Multiplex (video + audio)
av::Format fmt("MP4 Realtime", "mp4",
    av::VideoCodec("libx264", 1280, 720, 30.0, 400000),
    av::AudioCodec("aac", 2, 44100, 64000, "fltp"));

// Video only
av::Format fmt("MJPEG", "mjpeg",
    av::VideoCodec("mjpeg", 400, 300, 25));

// Audio only
av::Format fmt("MP3", "mp3",
    av::AudioCodec("libmp3lame", 2, 44100, 128000));

// Check type
if (fmt.type() == av::Format::Multiplex) { /* both streams */ }
```

The `id` field is the FFmpeg muxer short name passed to `avformat_alloc_output_context2`. Use `ffmpeg -formats` to list all available muxers.

#### FormatRegistry

`FormatRegistry` is a thread-safe singleton for named `Format` presets. Look up by display name or muxer ID:

```cpp
#include "icy/av/formatregistry.h"

auto& reg = av::FormatRegistry::instance();

// Register
reg.registerFormat(av::Format("Broadcast H264", "mp4",
    av::VideoCodec("libx264", 1920, 1080, 60.0, 6000000),
    av::AudioCodec("aac", 2, 48000, 192000, "fltp")));

// Retrieve
av::Format& fmt  = reg.get("Broadcast H264");    // by display name; throws if missing
av::Format& fmt2 = reg.getByID("mp4");            // by FFmpeg muxer ID
av::Format& def  = reg.getDefault();              // highest priority, or named default
av::Format& safe = reg.getOrDefault("Missing");   // falls back to default if not found

// Set default fallback
reg.setDefault("Broadcast H264");

// Snapshot
av::FormatList all = reg.formats();

// Remove
reg.unregisterFormat("Broadcast H264");
reg.clear();
```

---

### Device Enumeration

```cpp
#include "icy/av/devicemanager.h"

av::DeviceManager devman;

// Enumerate by type
std::vector<av::Device> cameras, mics, speakers;
devman.getCameras(cameras);
devman.getMicrophones(mics);
devman.getSpeakers(speakers);

// Find the system default
av::Device camera, mic;
devman.getDefaultCamera(camera);
devman.getDefaultMicrophone(mic);

// Find by name or device id
av::Device found;
if (devman.findCamera("Logitech C920", found)) {
    // use found.id to open
}

// Print everything
devman.print(std::cout);
```

Each `Device` carries:

```cpp
struct Device {
    enum Type { Unknown, VideoInput, VideoOutput, AudioInput, AudioOutput };

    Type        type;
    std::string id;          // platform device id (see table below)
    std::string name;        // human-readable display name
    bool        isDefault;
    bool        isConnected;
    bool        isInUse;

    std::vector<VideoCapability> videoCapabilities;
    std::vector<AudioCapability> audioCapabilities;
};

struct Device::VideoCapability {
    int width, height;
    double minFps, maxFps;
    std::string pixelFormat;  // "yuv420p", "nv12", "mjpeg", ...
};

struct Device::AudioCapability {
    int sampleRate, channels;
    std::string sampleFormat; // "s16", "f32", "fltp", ...
};
```

Platform device identifiers:

| Platform | Video id | Audio id |
| -------- | -------- | -------- |
| Linux | `/dev/video0`, `/dev/video1`, ... | `hw:0,0`, `default`, ... |
| macOS | Index string `"0"`, `"1"`, ... or UUID | Index string |
| Windows | DirectShow moniker string | DirectShow moniker string |

#### Capability negotiation

Rather than guessing parameters, we use `negotiateVideoCapture` and `negotiateAudioCapture`. These query the device's reported capabilities and return the closest match using a weighted distance function.

Video scoring: resolution match weighted 70%, fps match weighted 30%. Undersized resolutions are penalised 1.5x to prefer slight downscaling over upscaling.

Audio scoring: sample rate match weighted 70%, channel count weighted 30%.

```cpp
#include "icy/av/devicemanager.h"

// Returns std::optional<std::pair<Device, Device::VideoCapability>>
auto result = devman.negotiateVideoCapture(
    "",      // empty = system default camera; or pass a name/id
    1280,    // desired width
    720,     // desired height
    30.0     // desired fps
);

if (result) {
    auto& [device, cap] = *result;
    std::cout << device.name << " -> "
              << cap.width << "x" << cap.height
              << " @ " << cap.maxFps << " fps"
              << " (" << cap.pixelFormat << ")\n";
    // Open with the negotiated params:
    capture.openVideo(device.id, cap.width, cap.height, cap.maxFps, cap.pixelFormat);
}

// Audio
auto aresult = devman.negotiateAudioCapture("", 44100, 2);
if (aresult) {
    auto& [device, cap] = *aresult;
    capture.openAudio(device.id, cap.channels, cap.sampleRate);
}
```

If the device reports no capabilities (older V4L2 drivers, some virtual devices), `negotiateVideoCapture` returns a synthetic capability with the requested values and lets FFmpeg negotiate at open time.

#### MediaCapabilities bitmask

```cpp
int caps = devman.getCapabilities();
if (caps & av::VIDEO_SEND) { /* camera present */ }
if (caps & av::AUDIO_SEND) { /* microphone present */ }
if (caps & av::VIDEO_RECV) { /* video output present */ }
if (caps & av::AUDIO_RECV) { /* speaker present */ }
```

#### Hardware codec detection

```cpp
auto hwCodecs = devman.getHardwareCodecs();
for (auto& hc : hwCodecs) {
    // hc.name: "h264_nvenc", "hevc_nvenc", "h264_cuvid", "h264_videotoolbox", ...
    // hc.type: "encoder" or "decoder"
    std::cout << hc.name << " (" << hc.type << ")\n";
}
```

This iterates all codecs registered with FFmpeg and checks for hardware accelerated entries. Detected hardware encoders can be passed directly as the `encoder` field of `VideoCodec`:

```cpp
av::VideoCodec video("H264", "h264_nvenc", 1920, 1080, 60.0, 6000000);
```

---

### Video Capture

`VideoCapture` opens a camera through FFmpeg's avdevice layer and emits `PlanarVideoPacket` from a background thread.

```cpp
#include "icy/av/videocapture.h"
#include "icy/av/devicemanager.h"

// Construct empty, open later
av::VideoCapture capture;
capture.openVideo("/dev/video0", 1280, 720, 30.0);

// Construct and open immediately
av::VideoCapture capture("/dev/video0", 1280, 720, 30.0, "yuv420p");

// Open with a VideoCodec struct
av::VideoCodec params("libx264", 1280, 720, 30.0);
capture.openVideo("/dev/video0", params);
```

Pass `0` for width, height, or framerate to accept the device default. Pass `""` for pixelFmt to use `"yuv420p"`. If the device cannot satisfy the requested format natively, the internal `VideoDecoder` applies pixel format conversion via libswscale before emitting.

After opening, call `getEncoderFormat()` to retrieve the actual stream parameters before configuring an encoder:

```cpp
av::EncoderOptions options;
capture.getEncoderFormat(options.iformat);
// options.iformat.video is now populated with actual width/height/fps/pixelFmt
```

**Complete negotiated open:**

```cpp
#include "icy/av/devicemanager.h"
#include "icy/av/videocapture.h"
#include "icy/packetstream.h"

av::DeviceManager devman;
auto result = devman.negotiateVideoCapture("", 1280, 720, 30.0);
if (!result) {
    std::cerr << "no camera found\n";
    return 1;
}

auto& [camera, cap] = *result;
av::VideoCapture capture;
capture.openVideo(camera.id, cap.width, cap.height, cap.maxFps, cap.pixelFormat);

PacketStream stream;
stream.attachSource(&capture, false, true);
stream.emitter += [](IPacket& pkt) {
    auto& vpkt = static_cast<av::PlanarVideoPacket&>(pkt);
    // vpkt.width, vpkt.height, vpkt.pixelFmt, vpkt.time (microseconds)
    // vpkt.buffer[0..3], vpkt.linesize[0..3]
};
stream.start();
// ... capture runs until stream.stop()
stream.stop();
```

---

### Audio Capture

`AudioCapture` opens a microphone through FFmpeg's avdevice layer and emits `PlanarAudioPacket` from a background thread.

```cpp
#include "icy/av/audiocapture.h"

// Individual parameters; -1 = accept device default
av::AudioCapture capture("default", /*channels*/ 2, /*sampleRate*/ 44100, /*sampleFmt*/ "s16");

// With AudioCodec struct
av::AudioCodec params(2, 44100, "s16");
av::AudioCapture capture("hw:0,0", params);

// Construct empty, open later
av::AudioCapture capture;
capture.openAudio("default", 2, 44100);
```

Platform device strings:

- Linux: `"default"`, `"hw:0,0"`, `"plughw:1,0"` (ALSA device strings)
- macOS: `"0"` (device index from AVFoundation enumeration)
- Windows: `"0"` or DirectShow moniker

Pass `-1` for channels or sampleRate to accept the device default. The internal `AudioDecoder` applies resampling via libswresample if the device format differs from the requested format.

---

### File and Screen Capture

`MediaCapture` opens any source FFmpeg understands: media files, network streams (`rtsp://`, `http://`), or FFmpeg virtual devices like `x11grab` or `gdigrab`.

```cpp
#include "icy/av/mediacapture.h"

av::MediaCapture capture;

// File
capture.openFile("/path/to/video.mp4");

// Loop the file when it reaches EOF (useful for demo servers)
capture.setLoopInput(true);

// Pace output to the file's frame rate instead of decoding as fast as possible
capture.setLimitFramerate(true);

// Use wall-clock time for PTS instead of container timestamps.
// Preferred for live sources where FFmpeg timestamps are unreliable.
capture.setRealtimePTS(true);
```

`openFile()` auto-detects all streams. Access the underlying decoders:

```cpp
av::VideoDecoder* video = capture.video();  // null if no video stream
av::AudioDecoder* audio = capture.audio();  // null if no audio stream
```

**Thread lifecycle:**

`start()` launches the background decode thread. `stop()` signals it and joins before returning. `close()` stops the thread and tears down the `AVFormatContext` and decoders. The `Closing` signal fires from inside the thread just before it exits; do not join the thread from a `Closing` handler.

```cpp
capture.Closing += []() {
    std::cout << "capture thread exiting\n";
    // do NOT call capture.stop() here
};
```

`MediaCapture` is also the base of `VideoCapture` and `AudioCapture`. Screen capture works by passing the appropriate FFmpeg virtual device format:

```cpp
// Linux X11 screen capture
av::MediaCapture capture;
// Use openStream directly with x11grab format:
// avformat_open_input(&ctx, ":0.0", av_find_input_format("x11grab"), &opts)
// VideoCapture on Linux can be used similarly via V4L2 virtual devices.
```

---

### Video Encoding

`VideoEncoder` is a low-level RAII wrapper around an `AVCodecContext` for a single video stream. We normally don't use it directly; `MultiplexEncoder` manages it. It is documented here for completeness.

```cpp
#include "icy/av/videoencoder.h"

// Tied to an existing AVFormatContext (adds a stream to it)
av::VideoEncoder enc(formatCtx);
enc.oparams = av::VideoCodec("libx264", 1280, 720, 30.0, 400000);
enc.create();   // allocates AVCodecContext, finds codec, adds stream
enc.open();     // opens codec with oparams

// Encode interleaved frame
enc.encode(buffer, bufferSize, pts);

// Encode planar frame
enc.encode(planes, linesizes, pts);

// Encode AVFrame directly
enc.encode(avframe);

// Flush encoder at end of stream
enc.flush();
enc.close();
```

`VideoContext::convert()` is called internally before each encode call. If the input pixel format or dimensions differ from `oparams`, it transparently reconstructs the `VideoConverter` (libswscale) and converts before encoding.

`VideoConverter` can also be used standalone:

```cpp
#include "icy/av/videoconverter.h"

av::VideoConverter conv;
conv.iparams = av::VideoCodec(0, 1920, 1080, 0, 0, 0, "nv12");
conv.oparams = av::VideoCodec(0, 1280, 720, 0, 0, 0, "yuv420p");
conv.create();

AVFrame* converted = conv.convert(inputFrame);
// converted is owned by conv and overwritten on the next call
```

---

### Audio Encoding

`AudioEncoder` wraps `AVCodecContext` for a single audio stream. The key difference from `VideoEncoder` is the `AudioBuffer fifo` member, an internal `AVAudioFifo` that accumulates samples until a full encoder frame is available. This is mandatory for fixed-frame-size codecs like AAC (1024 samples/frame) and Opus (120, 240, 480, 960, or 1920 samples/frame).

```cpp
#include "icy/av/audioencoder.h"

av::AudioEncoder enc(formatCtx);
enc.oparams = av::AudioCodec("aac", 2, 44100, 64000, "fltp");
enc.create();
enc.open();

// Encode interleaved samples
enc.encode(buffer, numSamples, pts);

// Encode planar samples
uint8_t* planes[4] = { ... };
enc.encode(planes, numSamples, pts);

// Flush at end
enc.flush();
enc.close();
```

`AudioBuffer` can be used standalone when you need to queue audio samples independently:

```cpp
#include "icy/av/audiobuffer.h"

av::AudioBuffer fifo;
fifo.alloc("fltp", 2, 1024);

// Write samples in
void* planes[2] = { leftData, rightData };
fifo.write(reinterpret_cast<void**>(planes), numSamples);

// Read when enough are available
if (fifo.available() >= 1024) {
    uint8_t* outBuf[2];
    fifo.read(reinterpret_cast<void**>(outBuf), 1024);
}

fifo.close();
```

`AudioResampler` wraps libswresample for format, rate, and channel layout conversion:

```cpp
#include "icy/av/audioresampler.h"

av::AudioResampler resampler(
    av::AudioCodec(2, 48000, "s16"),   // input
    av::AudioCodec(2, 44100, "fltp")   // output
);
resampler.open();

int outSamples = resampler.resample(&inputPtr, inNumSamples);
// resampler.outSamples: converted sample buffer
// resampler.outNumSamples: number of converted samples

resampler.close();
```

---

### Multiplexing

`MultiplexEncoder` owns both a `VideoEncoder` and an `AudioEncoder` plus an `AVFormatContext` for muxed output. It handles stream creation, interleaved packet writing, and trailer finalisation.

```cpp
#include "icy/av/multiplexencoder.h"

av::EncoderOptions options;
options.ofile   = "output.mp4";
options.oformat = av::Format("MP4", "mp4",
    av::VideoCodec("libx264", 1280, 720, 30.0, 400000),
    av::AudioCodec("aac", 2, 44100, 64000, "fltp"));
// options.iformat populated from capture.getEncoderFormat()
// Leave iformat empty or set fields to 0 to match oformat

av::MultiplexEncoder encoder(options);
encoder.init();  // opens AVFormatContext, creates streams, writes header

// Encode frames directly
encoder.encodeVideo(buffer, size, width, height);           // interleaved
encoder.encodeVideo(planes, linesizes, width, height);      // planar
encoder.encodeAudio(buffer, numSamples);                    // interleaved
encoder.encodeAudio(planes, numSamples);                    // planar

// Flush and close (writes trailer)
encoder.uninit();

// Or abort without writing a trailer
encoder.cleanup();
```

`EncoderOptions` in detail:

```cpp
struct EncoderOptions {
    Format      iformat;    // input codec parameters (populated from capture)
    Format      oformat;    // output format and codec parameters
    std::string ifile;      // input file path (for file-to-file transcoding)
    std::string ofile;      // output file path; leave empty for custom I/O
    long        duration;   // max encode duration in nanoseconds; 0 = unlimited
};
```

**Custom I/O (in-memory or network output):**

Set `options.ofile` to `""` and supply your own `AVIOContext` by intercepting `MultiplexEncoder::_ioCtx` before calling `init()`. Alternatively, connect to `encoder.emitter` to receive each encoded `VideoPacket` or `AudioPacket`:

```cpp
encoder.emitter += [](IPacket& pkt) {
    // Receive encoded packets without writing to a file
    auto& rpkt = static_cast<RawPacket&>(pkt);
    socket.send(rpkt.data(), rpkt.size());
};
encoder.init();
```

**Encoder state machine:**

`IEncoder` derives from `Stateful<EncoderState>`:

```text
None → Ready → Encoding → Stopped
                        → Error
```

`init()` transitions to `Ready` then `Encoding`. `uninit()` transitions to `Stopped`. Query state:

```cpp
encoder.isEncoding();  // true while actively encoding
encoder.isActive();    // true for Ready or Encoding
encoder.isError();     // true on failure
```

---

### Decoding

`VideoDecoder` and `AudioDecoder` are constructed from an `AVStream*`. `MediaCapture` creates and manages them automatically. Direct use is only necessary when building a custom demux loop.

```cpp
#include "icy/av/videodecoder.h"
#include "icy/av/audiodecoder.h"

// VideoDecoder
av::VideoDecoder vdec(avStream);  // reads codecpar from the stream
vdec.create();
vdec.open();

// vdec.emitter fires PlanarVideoPacket for each decoded frame
vdec.emitter += [](IPacket& pkt) {
    auto& vpkt = static_cast<av::PlanarVideoPacket&>(pkt);
    // vpkt.width, vpkt.height, vpkt.time (microseconds), vpkt.pixelFmt
    // vpkt.buffer[0..3], vpkt.linesize[0..3]
    // vpkt.avframe: non-owning pointer, valid only in this callback
};

bool got = vdec.decode(compressedPacket);
// Flush after the last packet
vdec.flush();
vdec.close();

// AudioDecoder
av::AudioDecoder adec(avStream);
adec.create();
adec.open();

adec.emitter += [](IPacket& pkt) {
    auto& apkt = static_cast<av::PlanarAudioPacket&>(pkt);
    // apkt.channels, apkt.numSamples, apkt.sampleFmt, apkt.time
    // apkt.buffer[0..3]: per-channel sample buffers
};

adec.decode(compressedPacket);
adec.flush();  // call until flush() stops emitting
adec.close();
```

`VideoDecoder::decode()` converts the stream's time base to microseconds internally. The emitted packet's `time` field is always in microseconds regardless of the source stream's time base.

The decoders apply format conversion automatically:

- `VideoDecoder` calls `VideoContext::convert()` via libswscale if the decoded pixel format differs from `oparams.pixelFmt`
- `AudioDecoder` calls `AudioContext::recreateResampler()` via libswresample if the decoded sample format differs from `oparams.sampleFmt`

Access decoders from a running `MediaCapture`:

```cpp
av::MediaCapture capture;
capture.openFile("input.mp4");

av::VideoDecoder* video = capture.video();  // null if no video stream
av::AudioDecoder* audio = capture.audio();  // null if no audio stream

if (video) {
    // video->iparams: decoded format from the file
    // video->oparams: format being emitted (may differ if conversion applied)
}
```

---

### Device Watching

`DeviceWatcher` is the base class for platform-specific hot-plug monitors. `DeviceManager` creates the correct platform implementation automatically and holds it internally.

```cpp
#include "icy/av/devicemanager.h"

av::DeviceManager devman;

// Connect signal before starting; early events are not buffered
devman.DevicesChanged += [&devman]() {
    // WARNING: may fire from a background thread (inotify on Linux,
    // COM callback on Windows, CoreAudio listener on macOS).
    // Do NOT enumerate devices here directly if your app requires
    // thread safety. Post a flag or use an event loop instead.
    std::cout << "device change detected\n";
};

devman.watcher()->start();

// ... run event loop ...
waitForShutdown(nullptr, nullptr);

devman.watcher()->stop();
```

Platform implementations:

- **Linux** (`LinuxDeviceWatcher`): monitors `/dev` via libudev. Detects add/remove events for V4L2 video devices and ALSA audio devices.
- **macOS**: hooks into AVFoundation and CoreAudio device change notifications.
- **Windows** (`WindowsDeviceWatcher`): uses `IMMNotificationClient` for audio (WASAPI) and `RegisterDeviceNotification` for video (Media Foundation).

To replace the watcher with a custom implementation, subclass `DeviceWatcher` and call `devman.setWatcher()`:

```cpp
class MyWatcher : public av::DeviceWatcher {
public:
    explicit MyWatcher(av::DeviceManager* m) : DeviceWatcher(m) {}
    bool start() override { /* ... */ return true; }
    void stop() override { /* ... */ }
};

devman.setWatcher(new MyWatcher(&devman));
devman.watcher()->start();
```

---

### Pipeline Integration

The `av` module is designed around `PacketStream`. Capture classes implement `ICapture` (and through it `PacketSource`), so they attach as sources. Encoders implement `PacketProcessor`, so they attach as intermediate stages. Any callable that accepts `IPacket&` can be a sink.

A stream processes packets left-to-right through attached processors:

```text
source → processor(priority 5) → processor(priority 10) → emitter → sink
```

Lower priority numbers run first.

```cpp
#include "icy/packetstream.h"
#include "icy/av/videocapture.h"
#include "icy/av/audiocapture.h"
#include "icy/av/multiplexpacketencoder.h"

av::VideoCapture videoCapture;
av::AudioCapture audioCapture;
// ... open both ...

av::EncoderOptions options;
options.ofile = "output.mp4";
options.oformat = av::Format("MP4 Realtime", "mp4",
    av::VideoCodec("libx264", 1280, 720, 30.0, 400000),
    av::AudioCodec("aac", 2, 44100, 64000, "fltp"));
videoCapture.getEncoderFormat(options.iformat);
audioCapture.getEncoderFormat(options.iformat);

av::MultiplexPacketEncoder encoder(options);
encoder.init();

PacketStream stream;
stream.attachSource(&videoCapture, false, true);  // (source, freeOnStop, syncState)
stream.attachSource(&audioCapture, false, true);
stream.attach(&encoder, 5, false);                // (processor, priority, owned)

// Optional: tap the encoded output (packets after the encoder)
stream.emitter += [](IPacket& pkt) {
    // receives encoded VideoPacket and AudioPacket
};

stream.start();
// Both captures start automatically via onStreamStateChange

waitForShutdown([](void* s) {
    reinterpret_cast<PacketStream*>(s)->stop();
}, &stream);
// Both captures stop automatically; encoder.uninit() called via onStreamStateChange
```

**Attaching sources:** `attachSource(ptr, freeOnStop, syncState)`:

- `freeOnStop`: if true, `PacketStream` calls `delete` on the source when stopping
- `syncState`: if true, the source's `onStreamStateChange` is called to auto-start/stop

**Attaching processors:** `attach(ptr, priority, owned)`:

- `priority`: lower numbers run first (video capture at 5, a rate limiter at 3 before it, HTTP framing at 10 after)
- `owned`: if true, `PacketStream` calls `delete` on the processor when stopping

**FPS limiting:**

```cpp
#include "icy/av/fpscounter.h"

// Drop video packets exceeding 25fps; pass audio through unconditionally
auto* limiter = new av::FPSLimiter(25, /*videoOnly=*/true);
stream.attach(limiter, 3, true);
```

**Realtime pacing for file playback:**

```cpp
#include "icy/av/realtimepacketqueue.h"

// Hold packets until their PTS is due relative to a wall-clock start time
auto* queue = new av::RealtimePacketQueue<av::VideoPacket>(1024);
stream.attach(queue, 2, true);
```

**WebRTC integration:**

The `webrtc` module bridges the same `PacketStream` mechanism to WebRTC tracks:

```cpp
PacketStream stream;
stream.attachSource(capture.get());
stream.attach(&session->media().videoSender(), 5);
stream.start();
```

See the `webrtc` module documentation and the `webcam-streamer` sample for the full call flow including signalling.

**FPS measurement:**

```cpp
#include "icy/av/fpscounter.h"

av::FPSCounter counter;
stream.emitter += [&counter](IPacket&) {
    counter.tick();
    if (counter.frames % 30 == 0)
        std::cout << counter.fps << " fps\n";
};
```

---

## Samples

### [deviceenumerator](https://github.com/nilstate/icey/tree/main/src/av/samples/deviceenumerator/)

Lists all cameras, microphones, and speakers with their full capability lists (resolutions, frame rates, pixel formats, sample rates). Also prints all detected hardware codecs and all FFmpeg input/output format names. No hardware is required to understand what this reports; run it first on any new machine to confirm device visibility.

### [devicecapture](https://github.com/nilstate/icey/tree/main/src/av/samples/devicecapture/)

Opens the default camera (or a named camera passed as argv[1]) using `negotiateVideoCapture` to pick the best parameters the device actually supports for 720p at 30fps. Captures for 5 seconds, prints a frame count, and exits cleanly. This is the minimum end-to-end example for live video capture.

### [devicerecorder](https://github.com/nilstate/icey/tree/main/src/av/samples/devicerecorder/)

Records H.264 video and AAC audio from the default camera and microphone to `deviceoutput.mp4` until Ctrl-C. Uses `negotiateVideoCapture` and `negotiateAudioCapture` to pick the best parameters for both devices. Both capture sources feed a single `MultiplexPacketEncoder`. This is the canonical example for the full AV capture-to-file pipeline.

### [devicewatcher](https://github.com/nilstate/icey/tree/main/src/av/samples/devicewatcher/)

Demonstrates hot-plug detection. Prints the initial device state, then watches for device add/remove events until Ctrl-C. Plug and unplug a camera or microphone to see `DevicesChanged` fire. Shows the thread-safety concern: the signal callback re-enumerates devices immediately, which is safe here because it runs on the event thread, but in a real application you would post a flag and enumerate on the main thread.

### [filetranscode](https://github.com/nilstate/icey/tree/main/src/av/samples/filetranscode/)

Reads a media file, re-encodes it as H.264 + AAC in MP4, and writes the result. No capture hardware required. Passing `0` for codec dimensions, channels, and rates instructs the encoder to inherit all parameters from the input file. This is the simplest example that exercises `MediaCapture`, `getEncoderFormat()`, `MultiplexPacketEncoder`, and `PacketStream` together without any device dependency.

### [mjpegserver](https://github.com/nilstate/icey/tree/main/src/av/samples/mjpegserver/)

HTTP MJPEG streaming server. A shared `MediaCapture` instance (file or camera; toggle `USE_AVDEVICE_CAPTURE`) is shared across all connected clients. Each HTTP connection gets its own encoding pipeline: the shared capture is attached as a non-owned source to a per-connection `PacketStream`. A `MultiplexPacketEncoder` converts frames to MJPEG, and `http::MultipartAdapter` wraps each JPEG frame with MIME multipart boundaries so browsers render it as a live stream. Open `http://localhost:328` in any browser. This pattern is the basis for any multi-client streaming server.

### [wsstreamer](https://github.com/nilstate/icey/tree/main/src/av/samples/wsstreamer/)

WebSocket media streamer. Identical architecture to `mjpegserver` but delivers encoded frames as WebSocket binary frames instead of HTTP multipart. Each frame is a complete JPEG. A small JavaScript snippet (`ws.onmessage`) creates a Blob URL and sets it as an image source, giving frame-by-frame video in any browser without plugins. Listens on `ws://localhost:329`.

---

## Configuration

### CMake options

The `av` module is auto-detected: it builds when FFmpeg is found by CMake. There are no `WITH_FFMPEG` flags to set.

| Option | Default | Description |
| ------ | ------- | ----------- |
| `HAVE_FFMPEG_AVDEVICE` | auto-detected | Enables camera and microphone capture via FFmpeg `avdevice`. Set explicitly if auto-detection fails. |
| `BUILD_TESTS` | `OFF` | Build the `av` module tests. |

Install FFmpeg so CMake can find it:

```bash
# Ubuntu/Debian
sudo apt install libavcodec-dev libavformat-dev libavutil-dev \
                 libswscale-dev libswresample-dev libavdevice-dev

# Then configure normally; the av module builds automatically
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Link a downstream project:

```cmake
# After cmake --install build
find_package(icey REQUIRED)
target_link_libraries(myapp PRIVATE icey::av)

# Or via FetchContent
include(FetchContent)
FetchContent_Declare(icey
    GIT_REPOSITORY https://github.com/nilstate/icey.git
    GIT_TAG 2.4.8)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE icey::av)
```

### FFmpeg version compatibility

The module supports FFmpeg 5, 6, and 7. All three are tested in CI. Breaking API changes between major versions (notably `AVChannelLayout` in FFmpeg 5.1, `avcodec_receive_packet` deprecations in 6, and various `avformat` changes in 7) are handled internally. You do not need to conditionalize your code for version differences.

FFmpeg must be installed system-wide (Ubuntu: `libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev libavdevice-dev`) or pointed to via `FFmpeg_ROOT`:

```bash
cmake -B build -DFFmpeg_ROOT=/opt/ffmpeg
```

### Packet size limits

The following constants are defined in `icy/av/av.h` and can be overridden at compile time if you work with unusually large frames:

```cpp
MAX_VIDEO_PACKET_SIZE  (3 * 1024 * 1024)   // 3 MB
MAX_AUDIO_PACKET_SIZE  (128 * 1500)         // ~1 second of 48kHz 32-bit audio
MAX_ENCODE_DURATION    (3 * 60 * 60 * 1000) // 3 hours in milliseconds
```

---

## See Also

- [Base](base.md) — `PacketStream`, `PacketSource`, `PacketProcessor`, signals, threading
- [HTTP](http.md) — `http::MultipartAdapter` for MJPEG streaming; WebSocket for binary frame delivery
- [WebRTC](webrtc.md) — `WebRtcTrackSender` and `WebRtcTrackReceiver` for WebRTC media transport
- [Symple](symple.md) — signalling protocol for WebRTC call setup
