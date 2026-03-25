{#avmodule}

# av

Audio/video encoding, decoding, device capture, and media formats.

### Namespaces

| Name | Description |
|------|-------------|
| [`av`](#av) | Audio/video codecs, capture devices, packet types, and media helpers. |
| [`v4l2`](#v4l2) | Linux V4L2 device enumeration helpers. |
| [`coreaudio`](#coreaudio) | Apple CoreAudio device enumeration helpers. |
| [`legacy`](#legacy) | Legacy timing helpers kept for compatibility with older media pipelines. |
| [`avfoundation`](#avfoundation) | Apple AVFoundation device enumeration helpers. |
| [`mediafoundation`](#mediafoundation) | Windows Media Foundation device enumeration helpers. |
| [`wasapi`](#wasapi) | Windows WASAPI device enumeration helpers. |

### Macros

| Name | Description |
|------|-------------|
| [`MAX_AUDIO_PACKET_SIZE`](#max_audio_packet_size)  | 1 second of 48khz 32bit audio |
| [`MAX_ENCODE_DURATION`](#max_encode_duration)  | 3 hours |

---

{#max_audio_packet_size}

#### MAX_AUDIO_PACKET_SIZE

```cpp
MAX_AUDIO_PACKET_SIZE()
```

1 second of 48khz 32bit audio

---

{#max_encode_duration}

#### MAX_ENCODE_DURATION

```cpp
MAX_ENCODE_DURATION()
```

3 hours

{#av}

# av

Audio/video codecs, capture devices, packet types, and media helpers.

### Classes

| Name | Description |
|------|-------------|
| [`AppleDeviceWatcher`](#appledevicewatcher) | Monitors device add/remove events via AVFoundation notifications and CoreAudio property listeners. |
| [`AudioCapture`](#audiocapture) | Cross-platform audio capture device backed by FFmpeg input devices. |
| [`AudioPacketEncoder`](#audiopacketencoder) | [PacketProcessor](base.md#packetprocessor) that encodes raw audio samples ([PlanarAudioPacket](#planaraudiopacket) or [AudioPacket](#audiopacket)) into compressed packets via [AudioEncoder](#audioencoder). |
| [`DeviceManager`](#devicemanager) | Enumerates and manages system audio and video devices. |
| [`DeviceWatcher`](#devicewatcher) | Base class for platform-specific device change monitors. |
| [`FormatRegistry`](#formatregistry) | [Singleton](base.md#singleton) registry of available media container formats for encoding and decoding. |
| [`FPSCounter`](#fpscounter) | FPS counter based on the simple moving average (SMA) algorithm. |
| [`FPSLimiter`](#fpslimiter) | [PacketStream](base.md#packetstream) processor that caps packet throughput to a maximum FPS. |
| [`ICapture`](#icapture) | Abstract interface for audio and video capture devices. |
| [`IEncoder`](#iencoder) | This is the abstract class for all encoders. |
| [`LinuxDeviceWatcher`](#linuxdevicewatcher) | Monitors device add/remove events via libudev. |
| [`MediaCapture`](#mediacapture) | Unified capture and decode source for files and live media devices. |
| [`MultiplexEncoder`](#multiplexencoder) | Multiplexing encoder that writes synchronized audio and video streams. |
| [`MultiplexPacketEncoder`](#multiplexpacketencoder) | Encodes and multiplexes a realtime video stream form audio / video capture sources. FFmpeg is used for encoding. |
| [`RealtimePacketQueue`](#realtimepacketqueue) | [Queue](base.md#queue) that emits media packets in presentation-timestamp order relative to a realtime clock. |
| [`VideoCapture`](#videocapture) | Cross-platform video device capturer backed by FFmpeg avdevice. |
| [`VideoPacketEncoder`](#videopacketencoder) | [PacketProcessor](base.md#packetprocessor) that encodes raw video frames ([PlanarVideoPacket](#planarvideopacket) or [VideoPacket](#videopacket)) into compressed packets via [VideoEncoder](#videoencoder). |
| [`WindowsDeviceWatcher`](#windowsdevicewatcher) | Monitors device add/remove events via IMMNotificationClient (audio) and RegisterDeviceNotification (video). |
| [`AudioBuffer`](#audiobuffer) | FIFO buffer for queuing audio samples between encoding stages. |
| [`AudioCodec`](#audiocodec) | Audio codec parameters including channels, sample rate, and sample format. |
| [`AudioContext`](#audiocontext) | Base context for audio encoding and decoding via FFmpeg. |
| [`AudioDecoder`](#audiodecoder) | Decodes compressed audio packets into raw sample frames. |
| [`AudioEncoder`](#audioencoder) | Encodes raw audio samples into a compressed format. |
| [`AudioPacket`](#audiopacket) | Audio packet for interleaved formats. |
| [`AudioResampler`](#audioresampler) | Converts audio samples between different formats, sample rates, and channel layouts. |
| [`Codec`](#codec-1) | [Codec](#codec-1) for encoding/decoding media. |
| [`Deleter`](#deleter-1) | RAII helpers for owning FFmpeg allocation types. |
| [`Deleterp`](#deleterp) | [Deleter](#deleter-1) adaptor for functions like av_freep that take a pointer to a pointer. |
| [`Device`](#device) | Represents a system audio, video or render device. |
| [`EncoderOptions`](#encoderoptions) | [Configuration](base.md#configuration) options for audio and video encoders. |
| [`EncoderState`](#encoderstate) | [State](base.md#state) machine states for the encoder pipeline. |
| [`Format`](#format-1) | Defines a media container format which is available through the [FormatRegistry](#formatregistry) for encoding or decoding. |
| [`MediaPacket`](#mediapacket) | Timestamped media packet carrying raw audio or video data. |
| [`PlanarAudioPacket`](#planaraudiopacket) | Audio packet for planar formats. |
| [`PlanarVideoPacket`](#planarvideopacket) | Video packet for planar formats. |
| [`VideoCodec`](#videocodec) | Video codec parameters including resolution, frame rate, and pixel format. |
| [`VideoContext`](#videocontext) | Base video context from which all video encoders and decoders derive. |
| [`VideoConverter`](#videoconverter) | Converts video frames between pixel formats and resolutions. |
| [`VideoDecoder`](#videodecoder) | Decodes compressed video packets into raw frames. |
| [`VideoEncoder`](#videoencoder) | Encodes raw video frames into a compressed format. |
| [`VideoPacket`](#videopacket) | Video packet for interleaved formats. |

### Enumerations

| Name | Description |
|------|-------------|
| [`MediaCapabilities`](#mediacapabilities)  | Bitmask of media capabilities detected on this system. |

---

{#mediacapabilities}

#### MediaCapabilities

```cpp
enum MediaCapabilities
```

Bitmask of media capabilities detected on this system.

| Value | Description |
|-------|-------------|
| `AUDIO_RECV` | Audio capture or decode is available. |
| `AUDIO_SEND` | Audio playback or encode is available. |
| `VIDEO_RECV` | Video capture or decode is available. |
| `VIDEO_SEND` | Video render or encode is available. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::list< Codec >` | [`CodecList`](#codeclist)  | List of codec value objects. |
| `std::list< Codec * >` | [`CodecPList`](#codecplist)  | List of codec pointers. |
| `std::unique_ptr< AVFrame, Deleterp< AVFrame, void, av_frame_free > >` | [`AVFrameHolder`](#avframeholder)  | Owning AVFrame pointer released with av_frame_free(). |
| `std::unique_ptr< AVFormatContext, Deleter< AVFormatContext, void, avformat_free_context > >` | [`AVFormatContextHolder`](#avformatcontextholder)  | Owning AVFormatContext pointer released with avformat_free_context(). |
| `std::unique_ptr< AVCodecContext, Deleterp< AVCodecContext, void, avcodec_free_context > >` | [`AVCodecContextHolder`](#avcodeccontextholder)  | Owning AVCodecContext pointer released with avcodec_free_context(). |
| `std::unique_ptr< AVDictionary *, Deleter< AVDictionary *, void, av_dict_free > >` | [`AVDictionaryCleanup`](#avdictionarycleanup)  | Cleanup wrapper for AVDictionary* values freed with av_dict_free(). |
| `std::unique_ptr< AVPacket, Deleterp< AVPacket, void, av_packet_free > >` | [`AVPacketHolder`](#avpacketholder)  | Owning AVPacket pointer released with av_packet_free(). |
| `std::vector< Format >` | [`FormatList`](#formatlist)  | List of container format value objects. |
| `std::vector< Format * >` | [`FormatPList`](#formatplist)  | List of container format pointers. |
| `IEncoder` | [`IPacketEncoder`](#ipacketencoder)  | Legacy alias for [IEncoder](#iencoder) kept for 0.8.x compatibility. |

---

{#codeclist}

#### CodecList

```cpp
std::list< Codec > CodecList()
```

List of codec value objects.

---

{#codecplist}

#### CodecPList

```cpp
std::list< Codec * > CodecPList()
```

List of codec pointers.

---

{#avframeholder}

#### AVFrameHolder

```cpp
std::unique_ptr< AVFrame, Deleterp< AVFrame, void, av_frame_free > > AVFrameHolder()
```

Owning AVFrame pointer released with av_frame_free().

---

{#avformatcontextholder}

#### AVFormatContextHolder

```cpp
std::unique_ptr< AVFormatContext, Deleter< AVFormatContext, void, avformat_free_context > > AVFormatContextHolder()
```

Owning AVFormatContext pointer released with avformat_free_context().

---

{#avcodeccontextholder}

#### AVCodecContextHolder

```cpp
std::unique_ptr< AVCodecContext, Deleterp< AVCodecContext, void, avcodec_free_context > > AVCodecContextHolder()
```

Owning AVCodecContext pointer released with avcodec_free_context().

---

{#avdictionarycleanup}

#### AVDictionaryCleanup

```cpp
std::unique_ptr< AVDictionary *, Deleter< AVDictionary *, void, av_dict_free > > AVDictionaryCleanup()
```

Cleanup wrapper for AVDictionary* values freed with av_dict_free().

---

{#avpacketholder}

#### AVPacketHolder

```cpp
std::unique_ptr< AVPacket, Deleterp< AVPacket, void, av_packet_free > > AVPacketHolder()
```

Owning AVPacket pointer released with av_packet_free().

---

{#formatlist}

#### FormatList

```cpp
std::vector< Format > FormatList()
```

List of container format value objects.

---

{#formatplist}

#### FormatPList

```cpp
std::vector< Format * > FormatPList()
```

List of container format pointers.

---

{#ipacketencoder}

#### IPacketEncoder

```cpp
IEncoder IPacketEncoder()
```

Legacy alias for [IEncoder](#iencoder) kept for 0.8.x compatibility.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`initAudioCodecFromContext`](#initaudiocodecfromcontext)  | Populate an [AudioCodec](#audiocodec) from an open AVCodecContext. |
| `AVSampleFormat` | [`selectSampleFormat`](#selectsampleformat)  | Select the best supported sample format for a codec given the requested parameters. Returns the requested format if supported, otherwise the first format with the same planarity. |
| `bool` | [`isSampleFormatSupported`](#issampleformatsupported)  | Check whether a specific sample format is in the codec's supported list. |
| `bool` | [`formatIsPlanar`](#formatisplanar)  | Return true if the named sample format is planar (e.g. "fltp", "s16p"). |
| `bool` | [`formatIsPlanar`](#formatisplanar-1)  | Return true if the given AVSampleFormat is planar. |
| `int64_t` | [`fpsToInterval`](#fpstointerval) `inline` | Convert a frame rate to a nanosecond frame interval. |
| `int` | [`intervalToFps`](#intervaltofps) `inline` | Convert a nanosecond frame interval to a frame rate. |
| `float` | [`intervalToFpsFloat`](#intervaltofpsfloat) `inline` | Convert a nanosecond frame interval to a floating-point frame rate. |
| `void` | [`initializeFFmpeg`](#initializeffmpeg)  | Initialize the FFmpeg library. |
| `void` | [`uninitializeFFmpeg`](#uninitializeffmpeg)  | Uninitializes the FFmpeg library. |
| `std::string` | [`averror`](#averror)  | Get an error string for the given error code. |
| `void` | [`printInputFormats`](#printinputformats)  | Print all available FFmpeg demuxer (input) format names to the given stream. |
| `void` | [`printOutputFormats`](#printoutputformats)  | Print all available FFmpeg muxer (output) format names to the given stream. |
| `void` | [`printEncoders`](#printencoders)  | Print all available FFmpeg encoder names to the given stream. |
| `AVFrame *` | [`createVideoFrame`](#createvideoframe)  | Allocate a new AVFrame with the given pixel format and dimensions. Uses av_frame_get_buffer for reference-counted allocation with 16-byte alignment. |
| `AVFrame *` | [`cloneVideoFrame`](#clonevideoframe)  | Perform a deep copy of an AVFrame including its buffer data and properties. |
| `void` | [`initVideoCodecFromContext`](#initvideocodecfromcontext)  | Populate a [VideoCodec](#videocodec) from an open AVStream and AVCodecContext. |
| `AVPixelFormat` | [`selectPixelFormat`](#selectpixelformat)  | Select the best supported pixel format for a codec given the requested parameters. Returns the requested format if supported, otherwise the first format with the same plane count. |

---

{#initaudiocodecfromcontext}

#### initAudioCodecFromContext

```cpp
void initAudioCodecFromContext(const AVCodecContext * ctx, AudioCodec & params)
```

Populate an [AudioCodec](#audiocodec) from an open AVCodecContext. 
#### Parameters
* `ctx` The FFmpeg codec context to read from. 

* `params` The [AudioCodec](#audiocodec) struct to fill with codec, channel, sample rate, and format info.

---

{#selectsampleformat}

#### selectSampleFormat

```cpp
AVSampleFormat selectSampleFormat(const AVCodec * codec, av::AudioCodec & params)
```

Select the best supported sample format for a codec given the requested parameters. Returns the requested format if supported, otherwise the first format with the same planarity. 
#### Parameters
* `codec` The FFmpeg codec whose supported formats are queried. 

* `params` The audio codec parameters specifying the desired sample format. 

#### Returns
The chosen AVSampleFormat, or AV_SAMPLE_FMT_NONE if none is compatible.

---

{#issampleformatsupported}

#### isSampleFormatSupported

```cpp
bool isSampleFormatSupported(const AVCodec * codec, enum AVSampleFormat sampleFormat)
```

Check whether a specific sample format is in the codec's supported list. 
#### Parameters
* `codec` The FFmpeg codec to query. 

* `sampleFormat` The AVSampleFormat to look up. 

#### Returns
True if the format is supported.

---

{#formatisplanar}

#### formatIsPlanar

```cpp
bool formatIsPlanar(const std::string & pixfmt)
```

Return true if the named sample format is planar (e.g. "fltp", "s16p"). 
#### Parameters
* `pixfmt` The sample format name string.

---

{#formatisplanar-1}

#### formatIsPlanar

```cpp
bool formatIsPlanar(AVSampleFormat format)
```

Return true if the given AVSampleFormat is planar. 
#### Parameters
* `format` The AVSampleFormat to check.

---

{#fpstointerval}

#### fpsToInterval

`inline`

```cpp
inline int64_t fpsToInterval(int fps)
```

Convert a frame rate to a nanosecond frame interval. 
#### Parameters
* `fps` The frame rate in frames per second. 

#### Returns
The interval in nanoseconds between frames, or the minimum interval if fps is zero.

---

{#intervaltofps}

#### intervalToFps

`inline`

```cpp
inline int intervalToFps(int64_t interval)
```

Convert a nanosecond frame interval to a frame rate. 
#### Parameters
* `interval` The nanosecond interval between frames. 

#### Returns
The frame rate in frames per second, or zero if interval is zero.

---

{#intervaltofpsfloat}

#### intervalToFpsFloat

`inline`

```cpp
inline float intervalToFpsFloat(int64_t interval)
```

Convert a nanosecond frame interval to a floating-point frame rate. 
#### Parameters
* `interval` The nanosecond interval between frames. 

#### Returns
The frame rate in frames per second as a float, or 0.0f if interval is zero.

---

{#initializeffmpeg}

#### initializeFFmpeg

```cpp
void initializeFFmpeg()
```

Initialize the FFmpeg library.

---

{#uninitializeffmpeg}

#### uninitializeFFmpeg

```cpp
void uninitializeFFmpeg()
```

Uninitializes the FFmpeg library.

---

{#averror}

#### averror

```cpp
std::string averror(const int error)
```

Get an error string for the given error code.

---

{#printinputformats}

#### printInputFormats

```cpp
void printInputFormats(std::ostream & ost, const char * delim)
```

Print all available FFmpeg demuxer (input) format names to the given stream. 
#### Parameters
* `ost` The output stream to write to. 

* `delim` Delimiter inserted between each name.

---

{#printoutputformats}

#### printOutputFormats

```cpp
void printOutputFormats(std::ostream & ost, const char * delim)
```

Print all available FFmpeg muxer (output) format names to the given stream. 
#### Parameters
* `ost` The output stream to write to. 

* `delim` Delimiter inserted between each name.

---

{#printencoders}

#### printEncoders

```cpp
void printEncoders(std::ostream & ost, const char * delim)
```

Print all available FFmpeg encoder names to the given stream. 
#### Parameters
* `ost` The output stream to write to. 

* `delim` Delimiter inserted between each name.

---

{#createvideoframe}

#### createVideoFrame

```cpp
AVFrame * createVideoFrame(AVPixelFormat pixelFmt, int width, int height)
```

Allocate a new AVFrame with the given pixel format and dimensions. Uses av_frame_get_buffer for reference-counted allocation with 16-byte alignment. 
#### Parameters
* `pixelFmt` The pixel format for the frame. 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

#### Returns
A newly allocated AVFrame, or nullptr on failure.

---

{#clonevideoframe}

#### cloneVideoFrame

```cpp
AVFrame * cloneVideoFrame(AVFrame * source)
```

Perform a deep copy of an AVFrame including its buffer data and properties. 
#### Parameters
* `source` The source frame to copy. 

#### Returns
A newly allocated AVFrame with copied data. Caller owns the result.

---

{#initvideocodecfromcontext}

#### initVideoCodecFromContext

```cpp
void initVideoCodecFromContext(const AVStream * stream, const AVCodecContext * ctx, VideoCodec & params)
```

Populate a [VideoCodec](#videocodec) from an open AVStream and AVCodecContext. 
#### Parameters
* `stream` The AVStream for frame rate information (may be null). 

* `ctx` The FFmpeg codec context to read from. 

* `params` The [VideoCodec](#videocodec) struct to fill with codec, dimension, and format info.

---

{#selectpixelformat}

#### selectPixelFormat

```cpp
AVPixelFormat selectPixelFormat(const AVCodec * codec, VideoCodec & params)
```

Select the best supported pixel format for a codec given the requested parameters. Returns the requested format if supported, otherwise the first format with the same plane count. 
#### Parameters
* `codec` The FFmpeg codec whose supported formats are queried. 

* `params` The video codec parameters specifying the desired pixel format. 

#### Returns
The chosen AVPixelFormat.

{#appledevicewatcher}

## AppleDeviceWatcher

```cpp
#include <icy/av/apple/coreaudio.h>
```

> **Inherits:** [`DeviceWatcher`](#devicewatcher)

Monitors device add/remove events via AVFoundation notifications and CoreAudio property listeners.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AppleDeviceWatcher`](#appledevicewatcher-1) `explicit` | #### Parameters |
| `bool` | [`start`](#start) `virtual` | Begin monitoring for device connect/disconnect events. |
| `void` | [`stop`](#stop) `virtual` | Stop monitoring and release all notification observers. |

---

{#appledevicewatcher-1}

#### AppleDeviceWatcher

`explicit`

```cpp
explicit AppleDeviceWatcher(DeviceManager * manager)
```

#### Parameters
* `manager` The [DeviceManager](#devicemanager) whose DevicesChanged signal will be emitted on changes.

---

{#start}

#### start

`virtual`

```cpp
virtual bool start()
```

Begin monitoring for device connect/disconnect events. 
#### Returns
True if monitoring was successfully started.

---

{#stop}

#### stop

`virtual`

```cpp
virtual void stop()
```

Stop monitoring and release all notification observers.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< Impl >` | [`_impl`](#_impl)  |  |
| `DeviceManager *` | [`_manager`](#_manager)  |  |

---

{#_impl}

#### _impl

```cpp
std::unique_ptr< Impl > _impl
```

---

{#_manager}

#### _manager

```cpp
DeviceManager * _manager
```

{#audiocapture}

## AudioCapture

```cpp
#include <icy/av/audiocapture.h>
```

> **Inherits:** [`MediaCapture`](#mediacapture)

Cross-platform audio capture device backed by FFmpeg input devices.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioCapture`](#audiocapture-1)  | Construct without opening a device. Call [openAudio()](#openaudio) before [start()](#start-4). |
|  | [`AudioCapture`](#audiocapture-2)  | Construct and immediately open the given audio device using an [AudioCodec](#audiocodec) params struct. |
|  | [`AudioCapture`](#audiocapture-3)  | Construct and immediately open the given audio device with individual parameters. |
| `void` | [`openAudio`](#openaudio) `virtual` | Open the given audio device using an [AudioCodec](#audiocodec) params struct. |
| `void` | [`openAudio`](#openaudio-1) `virtual` | Open the given audio device with individual parameters. Configures the FFmpeg input format and passes device options via AVDictionary. If the device cannot satisfy the requested parameters, resampling will be applied. |

---

{#audiocapture-1}

#### AudioCapture

```cpp
AudioCapture()
```

Construct without opening a device. Call [openAudio()](#openaudio) before [start()](#start-4).

---

{#audiocapture-2}

#### AudioCapture

```cpp
AudioCapture(const std::string & device, const av::AudioCodec & params)
```

Construct and immediately open the given audio device using an [AudioCodec](#audiocodec) params struct. 
#### Parameters
* `device` The platform audio device identifier. 

* `params` The desired capture parameters (channels, sample rate, format).

---

{#audiocapture-3}

#### AudioCapture

```cpp
AudioCapture(const std::string & device, int channels, int sampleRate, const std::string & sampleFmt)
```

Construct and immediately open the given audio device with individual parameters. 
#### Parameters
* `device` The platform audio device identifier. 

* `channels` The desired number of channels, or -1 for device default. 

* `sampleRate` The desired sample rate in Hz, or -1 for device default. 

* `sampleFmt` The desired sample format string, or "" for device default.

---

{#openaudio}

#### openAudio

`virtual`

```cpp
virtual void openAudio(const std::string & device, const av::AudioCodec & params)
```

Open the given audio device using an [AudioCodec](#audiocodec) params struct. 
#### Parameters
* `device` The platform audio device identifier. 

* `params` The desired capture parameters.

---

{#openaudio-1}

#### openAudio

`virtual`

```cpp
virtual void openAudio(const std::string & device, int channels, int sampleRate, const std::string & sampleFmt)
```

Open the given audio device with individual parameters. Configures the FFmpeg input format and passes device options via AVDictionary. If the device cannot satisfy the requested parameters, resampling will be applied. 
#### Parameters
* `device` The platform audio device identifier. 

* `channels` The desired number of channels, or -1 for device default. 

* `sampleRate` The desired sample rate in Hz, or -1 for device default. 

* `sampleFmt` The desired sample format string, or "" for device default.

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr)  |  |

---

{#ptr}

#### Ptr

```cpp
std::shared_ptr< AudioCapture > Ptr()
```

{#audiopacketencoder}

## AudioPacketEncoder

```cpp
#include <icy/av/audiopacketencoder.h>
```

> **Inherits:** [`AudioEncoder`](#audioencoder), [`PacketProcessor`](base.md#packetprocessor)

[PacketProcessor](base.md#packetprocessor) that encodes raw audio samples ([PlanarAudioPacket](#planaraudiopacket) or [AudioPacket](#audiopacket)) into compressed packets via [AudioEncoder](#audioencoder).

Drop this into a [PacketStream](base.md#packetstream) between a source that emits raw audio (e.g. [MediaCapture](#mediacapture)) and a sink that expects encoded data (e.g. WebRtcTrackSender or [MultiplexPacketEncoder](#multiplexpacketencoder)).

Example:

auto encoder = std::make_shared<av::AudioPacketEncoder>(); encoder->oparams = [av::AudioCodec](#audiocodec)("opus", "libopus", 2, 48000);

[PacketStream](base.md#packetstream) stream; stream.attachSource(capture); stream.attach(encoder, 1, true); stream.attach(&webrtcSender, 5, false); stream.start();

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioPacketEncoder`](#audiopacketencoder-1)  |  |
|  | [`AudioPacketEncoder`](#audiopacketencoder-2)  | Deleted constructor. |
| `void` | [`process`](#process-1) `virtual` | [Process](base.md#process) an [AudioPacket](#audiopacket) or [PlanarAudioPacket](#planaraudiopacket) from the stream. Encodes the samples and emits the resulting compressed packet. |
| `bool` | [`accepts`](#accepts) `virtual` | Accept [AudioPacket](#audiopacket) and [PlanarAudioPacket](#planaraudiopacket) types. |

---

{#audiopacketencoder-1}

#### AudioPacketEncoder

```cpp
AudioPacketEncoder(AVFormatContext * format)
```

---

{#audiopacketencoder-2}

#### AudioPacketEncoder

```cpp
AudioPacketEncoder(const AudioPacketEncoder &) = delete
```

Deleted constructor.

---

{#process-1}

#### process

`virtual`

```cpp
virtual void process(IPacket & packet)
```

[Process](base.md#process) an [AudioPacket](#audiopacket) or [PlanarAudioPacket](#planaraudiopacket) from the stream. Encodes the samples and emits the resulting compressed packet.

---

{#accepts}

#### accepts

`virtual`

```cpp
virtual bool accepts(IPacket * packet)
```

Accept [AudioPacket](#audiopacket) and [PlanarAudioPacket](#planaraudiopacket) types.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`_initialized`](#_initialized)  |  |
| `std::mutex` | [`_mutex`](#_mutex)  |  |

---

{#_initialized}

#### _initialized

```cpp
bool _initialized = false
```

---

{#_mutex}

#### _mutex

```cpp
std::mutex _mutex
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onStreamStateChange`](#onstreamstatechange) `virtual` | Called by the [PacketStream](base.md#packetstream) to notify when the internal [Stream](base.md#stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |

---

{#onstreamstatechange}

#### onStreamStateChange

`virtual`

```cpp
virtual void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](base.md#packetstream) to notify when the internal [Stream](base.md#stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

{#devicemanager}

## DeviceManager

```cpp
#include <icy/av/devicemanager.h>
```

Enumerates and manages system audio and video devices.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `NullSignal` | [`DevicesChanged`](#deviceschanged)  | Signals when a system device is connected or removed. Emitted by the platform-specific `[DeviceWatcher](#devicewatcher)`. |

---

{#deviceschanged}

#### DevicesChanged

```cpp
NullSignal DevicesChanged
```

Signals when a system device is connected or removed. Emitted by the platform-specific `[DeviceWatcher](#devicewatcher)`.

NOTE: This signal may be emitted from a background thread (e.g. inotify thread on Linux, COM callback on Windows, CoreAudio listener on macOS). Handlers must be thread-safe and should NOT call back into [DeviceManager](#devicemanager) to enumerate. Instead, post to the main thread or set a flag.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`DeviceManager`](#devicemanager-1)  |  |
| `bool` | [`getCameras`](#getcameras) `const` | Populate `devices` with all connected video input (camera) devices. |
| `bool` | [`getMicrophones`](#getmicrophones) `const` | Populate `devices` with all connected audio input (microphone) devices. |
| `bool` | [`getSpeakers`](#getspeakers) `const` | Populate `devices` with all connected audio output (speaker) devices. |
| `bool` | [`getDefaultCamera`](#getdefaultcamera) `const` | Return the default (or first available) camera. |
| `bool` | [`getDefaultMicrophone`](#getdefaultmicrophone) `const` | Return the default (or first available) microphone. |
| `bool` | [`getDefaultSpeaker`](#getdefaultspeaker) `const` | Return the default (or first available) speaker. |
| `bool` | [`findCamera`](#findcamera) `const` | Find a camera by display name or device id. |
| `bool` | [`findMicrophone`](#findmicrophone) `const` | Find a microphone by display name or device id. |
| `bool` | [`findSpeaker`](#findspeaker) `const` | Find a speaker by display name or device id. |
| `bool` | [`getDeviceList`](#getdevicelist) `const` | Populate `devices` from the platform-specific backend for the given type. |
| `int` | [`getCapabilities`](#getcapabilities) `const` | #### Returns |
| `void` | [`setWatcher`](#setwatcher)  | Replace the active device watcher. Takes ownership. |
| `DeviceWatcher *` | [`watcher`](#watcher) `const` | #### Returns |
| `void` | [`print`](#print-3) `const` | Print all devices to the output stream. |
| `std::vector< HardwareCodec >` | [`getHardwareCodecs`](#gethardwarecodecs) `const` | Detect available hardware-accelerated codecs via FFmpeg. |
| `std::optional< std::pair< Device, Device::VideoCapability > >` | [`negotiateVideoCapture`](#negotiatevideocapture) `const` | Resolve the best video capture parameters for a device. |
| `std::optional< std::pair< Device, Device::AudioCapability > >` | [`negotiateAudioCapture`](#negotiateaudiocapture) `const` | Resolve the best audio capture parameters for a device. Returns std::nullopt if no microphone is available. |
| `const AVInputFormat *` | [`findVideoInputFormat`](#findvideoinputformat) `const` | Find base FFmpeg formats. |
| `const AVInputFormat *` | [`findAudioInputFormat`](#findaudioinputformat) `const` |  |

---

{#devicemanager-1}

#### DeviceManager

```cpp
DeviceManager()
```

---

{#getcameras}

#### getCameras

`const`

```cpp
bool getCameras(std::vector< Device > & devices) const
```

Populate `devices` with all connected video input (camera) devices. 
#### Parameters
* `devices` Output vector to fill. 

#### Returns
True if enumeration succeeded (even if the list is empty).

---

{#getmicrophones}

#### getMicrophones

`const`

```cpp
bool getMicrophones(std::vector< Device > & devices) const
```

Populate `devices` with all connected audio input (microphone) devices. 
#### Parameters
* `devices` Output vector to fill. 

#### Returns
True if enumeration succeeded.

---

{#getspeakers}

#### getSpeakers

`const`

```cpp
bool getSpeakers(std::vector< Device > & devices) const
```

Populate `devices` with all connected audio output (speaker) devices. 
#### Parameters
* `devices` Output vector to fill. 

#### Returns
True if enumeration succeeded.

---

{#getdefaultcamera}

#### getDefaultCamera

`const`

```cpp
bool getDefaultCamera(Device & device) const
```

Return the default (or first available) camera. 
#### Parameters
* `device` Output [Device](#device) to fill. 

#### Returns
True if a camera was found.

---

{#getdefaultmicrophone}

#### getDefaultMicrophone

`const`

```cpp
bool getDefaultMicrophone(Device & device) const
```

Return the default (or first available) microphone. 
#### Parameters
* `device` Output [Device](#device) to fill. 

#### Returns
True if a microphone was found.

---

{#getdefaultspeaker}

#### getDefaultSpeaker

`const`

```cpp
bool getDefaultSpeaker(Device & device) const
```

Return the default (or first available) speaker. 
#### Parameters
* `device` Output [Device](#device) to fill. 

#### Returns
True if a speaker was found.

---

{#findcamera}

#### findCamera

`const`

```cpp
bool findCamera(std::string_view name, Device & device) const
```

Find a camera by display name or device id. 
#### Parameters
* `name` The name or id to search for. 

* `device` Output [Device](#device) to fill if found. 

#### Returns
True if a matching device was found.

---

{#findmicrophone}

#### findMicrophone

`const`

```cpp
bool findMicrophone(std::string_view name, Device & device) const
```

Find a microphone by display name or device id. 
#### Parameters
* `name` The name or id to search for. 

* `device` Output [Device](#device) to fill if found. 

#### Returns
True if a matching device was found.

---

{#findspeaker}

#### findSpeaker

`const`

```cpp
bool findSpeaker(std::string_view name, Device & device) const
```

Find a speaker by display name or device id. 
#### Parameters
* `name` The name or id to search for. 

* `device` Output [Device](#device) to fill if found. 

#### Returns
True if a matching device was found.

---

{#getdevicelist}

#### getDeviceList

`const`

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices) const
```

Populate `devices` from the platform-specific backend for the given type. 
#### Parameters
* `type` The device type to enumerate. 

* `devices` Output vector to fill. 

#### Returns
True if enumeration succeeded.

---

{#getcapabilities}

#### getCapabilities

`const`

```cpp
int getCapabilities() const
```

#### Returns
A bitmask of MediaCapabilities flags indicating which capture/render types are available.

---

{#setwatcher}

#### setWatcher

```cpp
void setWatcher(DeviceWatcher * watcher)
```

Replace the active device watcher. Takes ownership. 
#### Parameters
* `watcher` The new [DeviceWatcher](#devicewatcher), or nullptr to disable watching.

---

{#watcher}

#### watcher

`const`

```cpp
DeviceWatcher * watcher() const
```

#### Returns
The currently active [DeviceWatcher](#devicewatcher), or nullptr if none has been set.

---

{#print-3}

#### print

`const`

```cpp
void print(std::ostream & ost) const
```

Print all devices to the output stream.

---

{#gethardwarecodecs}

#### getHardwareCodecs

`const`

```cpp
std::vector< HardwareCodec > getHardwareCodecs() const
```

Detect available hardware-accelerated codecs via FFmpeg.

---

{#negotiatevideocapture}

#### negotiateVideoCapture

`const`

```cpp
std::optional< std::pair< Device, Device::VideoCapability > > negotiateVideoCapture(std::string_view deviceName, int width, int height, double fps) const
```

Resolve the best video capture parameters for a device.

Looks up the device by name/id (or default camera if empty), queries its capabilities, and returns the closest match. Returns std::nullopt if no camera is available.

Usage: if (auto result = devman.negotiateVideoCapture("", 1280, 720, 30)) { auto& [dev, cap] = *result; capture.openVideo(dev.id, cap.width, cap.height, cap.maxFps, cap.pixelFormat); }

---

{#negotiateaudiocapture}

#### negotiateAudioCapture

`const`

```cpp
std::optional< std::pair< Device, Device::AudioCapability > > negotiateAudioCapture(std::string_view deviceName, int sampleRate, int channels) const
```

Resolve the best audio capture parameters for a device. Returns std::nullopt if no microphone is available.

---

{#findvideoinputformat}

#### findVideoInputFormat

`const`

```cpp
const AVInputFormat * findVideoInputFormat() const
```

Find base FFmpeg formats.

---

{#findaudioinputformat}

#### findAudioInputFormat

`const`

```cpp
const AVInputFormat * findAudioInputFormat() const
```

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `DeviceManager &` | [`instance`](#instance) `static` | Returns the default `[DeviceManager](#devicemanager)` singleton. |
| `void` | [`shutdown`](#shutdown) `static` | Shuts down the `MediaFactory` and deletes the singleton instance. |

---

{#instance}

#### instance

`static`

```cpp
static DeviceManager & instance()
```

Returns the default `[DeviceManager](#devicemanager)` singleton.

---

{#shutdown}

#### shutdown

`static`

```cpp
static void shutdown()
```

Shuts down the `MediaFactory` and deletes the singleton instance.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< DeviceWatcher >` | [`_watcher`](#_watcher)  |  |

---

{#_watcher}

#### _watcher

```cpp
std::unique_ptr< DeviceWatcher > _watcher
```

{#hardwarecodec}

## HardwareCodec

```cpp
#include <icy/av/devicemanager.h>
```

Hardware codec descriptor.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`name`](#name-2)  |  |
| `std::string` | [`type`](#type-2)  |  |

---

{#name-2}

#### name

```cpp
std::string name
```

---

{#type-2}

#### type

```cpp
std::string type
```

{#devicewatcher}

## DeviceWatcher

```cpp
#include <icy/av/devicemanager.h>
```

> **Subclassed by:** [`AppleDeviceWatcher`](#appledevicewatcher), [`LinuxDeviceWatcher`](#linuxdevicewatcher), [`WindowsDeviceWatcher`](#windowsdevicewatcher)

Base class for platform-specific device change monitors.

Call [start()](#start-1) to begin monitoring and [stop()](#stop-1) to end. The watcher is NOT auto-started; callers must explicitly start it.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`DeviceWatcher`](#devicewatcher-1) `inline` `explicit` |  |
| `bool` | [`start`](#start-1) `virtual` `inline` |  |
| `void` | [`stop`](#stop-1) `virtual` `inline` |  |

---

{#devicewatcher-1}

#### DeviceWatcher

`inline` `explicit`

```cpp
inline explicit DeviceWatcher(DeviceManager *)
```

---

{#start-1}

#### start

`virtual` `inline`

```cpp
virtual inline bool start()
```

---

{#stop-1}

#### stop

`virtual` `inline`

```cpp
virtual inline void stop()
```

{#formatregistry}

## FormatRegistry

```cpp
#include <icy/av/formatregistry.h>
```

[Singleton](base.md#singleton) registry of available media container formats for encoding and decoding.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FormatRegistry`](#formatregistry-1)  |  |
| `Format &` | [`get`](#get) `virtual` | Return the format with the given display name. Throws std::runtime_error if no format with that name is registered. |
| `Format &` | [`getByID`](#getbyid) `virtual` | Return the format with the given short ID (e.g. "mp4"). Throws std::runtime_error if no format with that ID is registered. |
| `Format &` | [`getOrDefault`](#getordefault) `virtual` | Return the format with the given name, or the default format if not found. |
| `Format &` | [`getDefault`](#getdefault) `virtual` | If a default has been specified it will be returned, other the format with the highest priority will take precedence. |
| `void` | [`registerFormat`](#registerformat) `virtual` | Registers the given media format overriding existing media formats of the same name. |
| `bool` | [`unregisterFormat`](#unregisterformat) `virtual` | Unregisters the media format matching the given name. |
| `void` | [`setDefault`](#setdefault) `virtual` | Sets the default fallback media format. |
| `bool` | [`exists`](#exists) `virtual` | #### Returns |
| `void` | [`clear`](#clear) `virtual` | Remove all registered formats and clear the default. |
| `FormatList` | [`formats`](#formats) `virtual` `const` | #### Returns |

---

{#formatregistry-1}

#### FormatRegistry

```cpp
FormatRegistry()
```

---

{#get}

#### get

`virtual`

```cpp
virtual Format & get(std::string_view name)
```

Return the format with the given display name. Throws std::runtime_error if no format with that name is registered. 
#### Parameters
* `name` The display name to look up.

---

{#getbyid}

#### getByID

`virtual`

```cpp
virtual Format & getByID(std::string_view id)
```

Return the format with the given short ID (e.g. "mp4"). Throws std::runtime_error if no format with that ID is registered. 
#### Parameters
* `id` The short format ID to look up.

---

{#getordefault}

#### getOrDefault

`virtual`

```cpp
virtual Format & getOrDefault(std::string_view name)
```

Return the format with the given name, or the default format if not found. 
#### Parameters
* `name` The display name to look up.

---

{#getdefault}

#### getDefault

`virtual`

```cpp
virtual Format & getDefault()
```

If a default has been specified it will be returned, other the format with the highest priority will take precedence.

---

{#registerformat}

#### registerFormat

`virtual`

```cpp
virtual void registerFormat(const Format & format)
```

Registers the given media format overriding existing media formats of the same name.

---

{#unregisterformat}

#### unregisterFormat

`virtual`

```cpp
virtual bool unregisterFormat(std::string_view name)
```

Unregisters the media format matching the given name.

---

{#setdefault}

#### setDefault

`virtual`

```cpp
virtual void setDefault(std::string_view name)
```

Sets the default fallback media format.

---

{#exists}

#### exists

`virtual`

```cpp
virtual bool exists(std::string_view name)
```

#### Returns
True if a format with the given display name is registered. 

#### Parameters
* `name` The display name to check.

---

{#clear}

#### clear

`virtual`

```cpp
virtual void clear()
```

Remove all registered formats and clear the default.

---

{#formats}

#### formats

`virtual` `const`

```cpp
virtual FormatList formats() const
```

#### Returns
A snapshot copy of all registered formats.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `FormatRegistry &` | [`instance`](#instance-1) `static` | Return the singleton [FormatRegistry](#formatregistry) instance. |

---

{#instance-1}

#### instance

`static`

```cpp
static FormatRegistry & instance()
```

Return the singleton [FormatRegistry](#formatregistry) instance.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `FormatList` | [`_formats`](#_formats)  |  |
| `std::string` | [`_default`](#_default)  |  |
| `std::mutex` | [`_mutex`](#_mutex-1)  |  |

---

{#_formats}

#### _formats

```cpp
FormatList _formats
```

---

{#_default}

#### _default

```cpp
std::string _default
```

---

{#_mutex-1}

#### _mutex

```cpp
std::mutex _mutex
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FormatRegistry`](#formatregistry-2)  | Deleted constructor. |
|  | [`FormatRegistry`](#formatregistry-3)  | Deleted constructor. |
| `Format &` | [`findByName`](#findbyname)  |  |
| `Format &` | [`defaultLocked`](#defaultlocked)  |  |

---

{#formatregistry-2}

#### FormatRegistry

```cpp
FormatRegistry(const FormatRegistry &) = delete
```

Deleted constructor.

---

{#formatregistry-3}

#### FormatRegistry

```cpp
FormatRegistry(FormatRegistry &&) = delete
```

Deleted constructor.

---

{#findbyname}

#### findByName

```cpp
Format & findByName(std::string_view name)
```

---

{#defaultlocked}

#### defaultLocked

```cpp
Format & defaultLocked()
```

{#fpscounter}

## FPSCounter

```cpp
#include <icy/av/fpscounter.h>
```

FPS counter based on the simple moving average (SMA) algorithm.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `double` | [`fps`](#fps)  |  |
| `int64_t` | [`frames`](#frames)  |  |

---

{#fps}

#### fps

```cpp
double fps
```

---

{#frames}

#### frames

```cpp
int64_t frames
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FPSCounter`](#fpscounter-1) `inline` | Initialise the counter with all zeroes. |
| `void` | [`reset`](#reset-1) `inline` | Reset all counters and the SMA window to zero. |
| `void` | [`tick`](#tick) `inline` | Record a new frame and update the FPS estimate. Must be called once per frame. Updates `fps` and increments `frames`. |

---

{#fpscounter-1}

#### FPSCounter

`inline`

```cpp
inline FPSCounter()
```

Initialise the counter with all zeroes.

---

{#reset-1}

#### reset

`inline`

```cpp
inline void reset()
```

Reset all counters and the SMA window to zero.

---

{#tick}

#### tick

`inline`

```cpp
inline void tick()
```

Record a new frame and update the FPS estimate. Must be called once per frame. Updates `fps` and increments `frames`.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`tickIndex`](#tickindex)  |  |
| `clock_t` | [`prevTick`](#prevtick)  |  |
| `clock_t` | [`tickSum`](#ticksum)  |  |
| `clock_t` | [`tickList`](#ticklist)  |  |

---

{#tickindex}

#### tickIndex

```cpp
int tickIndex
```

---

{#prevtick}

#### prevTick

```cpp
clock_t prevTick
```

---

{#ticksum}

#### tickSum

```cpp
clock_t tickSum
```

---

{#ticklist}

#### tickList

```cpp
clock_t tickList
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `double` | [`updateAvg`](#updateavg) `inline` |  |

---

{#updateavg}

#### updateAvg

`inline`

```cpp
inline double updateAvg(clock_t newTick)
```

{#fpslimiter}

## FPSLimiter

```cpp
#include <icy/av/fpscounter.h>
```

> **Inherits:** [`PacketProcessor`](base.md#packetprocessor)

[PacketStream](base.md#packetstream) processor that caps packet throughput to a maximum FPS.

If the observed frame rate exceeds the configured FPS budget, packets are dropped until the rate falls back under the limit.

Note that previous processors must not fragment packets, otherwise this class will not be accurate, and the packet drop rate will be too high.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-1)  |  |

---

{#emitter-1}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FPSLimiter`](#fpslimiter-1) `inline` | Construct the limiter. |
| `void` | [`process`](#process-2) `virtual` `inline` | [Process](base.md#process) a packet: forward it if within the rate limit, drop it otherwise. |
| `void` | [`onStreamStateChange`](#onstreamstatechange-1) `virtual` `inline` | Reset the FPS counter when the stream state changes. |

---

{#fpslimiter-1}

#### FPSLimiter

`inline`

```cpp
inline FPSLimiter(int max, bool videoOnly)
```

Construct the limiter. 
#### Parameters
* `max` The maximum allowed frame rate in frames per second. 

* `videoOnly` If true, non-VideoPacket packets are always forwarded regardless of rate.

---

{#process-2}

#### process

`virtual` `inline`

```cpp
virtual inline void process(IPacket & packet)
```

[Process](base.md#process) a packet: forward it if within the rate limit, drop it otherwise. 
#### Parameters
* `packet` The incoming packet to evaluate.

---

{#onstreamstatechange-1}

#### onStreamStateChange

`virtual` `inline`

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Reset the FPS counter when the stream state changes.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`_max`](#_max)  |  |
| `bool` | [`_videoOnly`](#_videoonly)  |  |
| `legacy::FPSCounter` | [`_counter`](#_counter)  |  |

---

{#_max}

#### _max

```cpp
int _max
```

---

{#_videoonly}

#### _videoOnly

```cpp
bool _videoOnly
```

---

{#_counter}

#### _counter

```cpp
legacy::FPSCounter _counter
```

{#icapture}

## ICapture

```cpp
#include <icy/av/icapture.h>
```

> **Inherits:** [`PacketStreamAdapter`](base.md#packetstreamadapter), [`Startable`](base.md#startable)
> **Subclassed by:** [`MediaCapture`](#mediacapture)

Abstract interface for audio and video capture devices.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-2)  |  |

---

{#emitter-2}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ICapture`](#icapture-1) `inline` |  |
| `void` | [`start`](#start-2)  | Start capturing and emitting packets. |
| `void` | [`stop`](#stop-2)  | Stop capturing and release device resources. |
| `void` | [`openFile`](#openfile) `virtual` `inline` | Open a media file as the capture source. |
| `void` | [`close`](#close-5) `virtual` `inline` | Release the capture source and any associated resources. |
| `void` | [`getEncoderFormat`](#getencoderformat)  | Populate `iformat` with the encoder-ready format derived from this capture source. |
| `void` | [`getEncoderAudioCodec`](#getencoderaudiocodec) `virtual` `inline` | Populate `params` with the encoder-ready audio codec parameters. |
| `void` | [`getEncoderVideoCodec`](#getencodervideocodec) `virtual` `inline` | Populate `params` with the encoder-ready video codec parameters. |
| `void` | [`openAudio`](#openaudio-2) `virtual` `inline` | Open an audio capture device with the given parameters. |
| `void` | [`openVideo`](#openvideo) `virtual` `inline` | Open a video capture device with the given parameters. |
| `void` | [`onStreamStateChange`](#onstreamstatechange-2) `virtual` `inline` | React to [PacketStream](base.md#packetstream) state transitions by starting or stopping capture. |

---

{#icapture-1}

#### ICapture

`inline`

```cpp
inline ICapture()
```

---

{#start-2}

#### start

```cpp
void start()
```

Start capturing and emitting packets.

---

{#stop-2}

#### stop

```cpp
void stop()
```

Stop capturing and release device resources.

---

{#openfile}

#### openFile

`virtual` `inline`

```cpp
virtual inline void openFile(const std::string & file)
```

Open a media file as the capture source. 
#### Parameters
* `file` Path to the media file.

---

{#close-5}

#### close

`virtual` `inline`

```cpp
virtual inline void close()
```

Release the capture source and any associated resources.

---

{#getencoderformat}

#### getEncoderFormat

```cpp
void getEncoderFormat(Format & iformat)
```

Populate `iformat` with the encoder-ready format derived from this capture source. 
#### Parameters
* `iformat` Output [Format](#format-1) struct to fill.

---

{#getencoderaudiocodec}

#### getEncoderAudioCodec

`virtual` `inline`

```cpp
virtual inline void getEncoderAudioCodec(AudioCodec & params)
```

Populate `params` with the encoder-ready audio codec parameters. 
#### Parameters
* `params` Output [AudioCodec](#audiocodec) struct to fill.

---

{#getencodervideocodec}

#### getEncoderVideoCodec

`virtual` `inline`

```cpp
virtual inline void getEncoderVideoCodec(VideoCodec & params)
```

Populate `params` with the encoder-ready video codec parameters. 
#### Parameters
* `params` Output [VideoCodec](#videocodec) struct to fill.

---

{#openaudio-2}

#### openAudio

`virtual` `inline`

```cpp
virtual inline void openAudio(const std::string & device, const AudioCodec & params)
```

Open an audio capture device with the given parameters. 
#### Parameters
* `device` The platform audio device identifier. 

* `params` The desired capture parameters.

---

{#openvideo}

#### openVideo

`virtual` `inline`

```cpp
virtual inline void openVideo(const std::string & device, const VideoCodec & params)
```

Open a video capture device with the given parameters. 
#### Parameters
* `device` The platform video device identifier. 

* `params` The desired capture parameters.

---

{#onstreamstatechange-2}

#### onStreamStateChange

`virtual` `inline`

```cpp
virtual inline void onStreamStateChange(const PacketStreamState & state)
```

React to [PacketStream](base.md#packetstream) state transitions by starting or stopping capture.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`openStream`](#openstream) `virtual` `inline` |  |

---

{#openstream}

#### openStream

`virtual` `inline`

```cpp
virtual inline void openStream(const std::string & filename, const AVInputFormat * inputFormat, AVDictionary ** formatParams)
```

{#iencoder}

## IEncoder

```cpp
#include <icy/av/iencoder.h>
```

> **Inherits:** [`Stateful< EncoderState >`](base.md#stateful)
> **Subclassed by:** [`MultiplexEncoder`](#multiplexencoder)

This is the abstract class for all encoders.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`init`](#init)  | Initialise the encoder, open codec contexts, and transition to Ready or Encoding state. |
| `void` | [`uninit`](#uninit)  | Flush remaining packets, close codec contexts, and release all encoder resources. |
| `void` | [`cleanup`](#cleanup) `virtual` `inline` | Perform any additional cleanup after [uninit()](#uninit). Default is a no-op. |
| `EncoderOptions &` | [`options`](#options-1)  | #### Returns |
| `void` | [`createVideo`](#createvideo) `virtual` `inline` | Initialise the video codec context and stream. |
| `void` | [`freeVideo`](#freevideo) `virtual` `inline` | Free the video codec context and stream. |
| `bool` | [`encodeVideo`](#encodevideo) `virtual` `inline` | Encode a single AVFrame of video. |
| `void` | [`createAudio`](#createaudio) `virtual` `inline` | Initialise the audio codec context and stream. |
| `void` | [`freeAudio`](#freeaudio) `virtual` `inline` | Free the audio codec context and stream. |
| `void` | [`flush`](#flush-3) `virtual` `inline` | Flush any internally buffered packets to the output. |
| `bool` | [`isNone`](#isnone) `const` `inline` | #### Returns |
| `bool` | [`isReady`](#isready) `const` `inline` | #### Returns |
| `bool` | [`isEncoding`](#isencoding) `virtual` `const` `inline` | #### Returns |
| `bool` | [`isActive`](#isactive) `virtual` `const` `inline` | #### Returns |
| `bool` | [`isStopped`](#isstopped) `virtual` `const` `inline` | #### Returns |
| `bool` | [`isError`](#iserror) `virtual` `const` `inline` | #### Returns |

---

{#init}

#### init

```cpp
void init()
```

Initialise the encoder, open codec contexts, and transition to Ready or Encoding state.

---

{#uninit}

#### uninit

```cpp
void uninit()
```

Flush remaining packets, close codec contexts, and release all encoder resources.

---

{#cleanup}

#### cleanup

`virtual` `inline`

```cpp
virtual inline void cleanup()
```

Perform any additional cleanup after [uninit()](#uninit). Default is a no-op.

---

{#options-1}

#### options

```cpp
EncoderOptions & options()
```

#### Returns
A reference to the encoder's configuration options.

---

{#createvideo}

#### createVideo

`virtual` `inline`

```cpp
virtual inline void createVideo()
```

Initialise the video codec context and stream.

---

{#freevideo}

#### freeVideo

`virtual` `inline`

```cpp
virtual inline void freeVideo()
```

Free the video codec context and stream.

---

{#encodevideo}

#### encodeVideo

`virtual` `inline`

```cpp
virtual inline bool encodeVideo(AVFrame * frame)
```

Encode a single AVFrame of video. 
#### Parameters
* `frame` The source video frame; all fields (format, width, height, pts) must be set. 

#### Returns
True if a packet was successfully encoded and written.

---

{#createaudio}

#### createAudio

`virtual` `inline`

```cpp
virtual inline void createAudio()
```

Initialise the audio codec context and stream.

---

{#freeaudio}

#### freeAudio

`virtual` `inline`

```cpp
virtual inline void freeAudio()
```

Free the audio codec context and stream.

---

{#flush-3}

#### flush

`virtual` `inline`

```cpp
virtual inline void flush()
```

Flush any internally buffered packets to the output.

---

{#isnone}

#### isNone

`const` `inline`

```cpp
inline bool isNone() const
```

#### Returns
True if the encoder is in the None state.

---

{#isready}

#### isReady

`const` `inline`

```cpp
inline bool isReady() const
```

#### Returns
True if the encoder is ready but not yet encoding.

---

{#isencoding}

#### isEncoding

`virtual` `const` `inline`

```cpp
virtual inline bool isEncoding() const
```

#### Returns
True if the encoder is actively encoding.

---

{#isactive}

#### isActive

`virtual` `const` `inline`

```cpp
virtual inline bool isActive() const
```

#### Returns
True if the encoder is in the Ready or Encoding state.

---

{#isstopped}

#### isStopped

`virtual` `const` `inline`

```cpp
virtual inline bool isStopped() const
```

#### Returns
True if the encoder has stopped.

---

{#iserror}

#### isError

`virtual` `const` `inline`

```cpp
virtual inline bool isError() const
```

#### Returns
True if the encoder is in an error state.

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-6)  |  |

---

{#type-6}

#### Type

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` | huh? |
| `Video` | video only |
| `Audio` | audio only |
| `Multiplex` | both video & audio |

{#linuxdevicewatcher}

## LinuxDeviceWatcher

```cpp
#include <icy/av/linux/v4l2.h>
```

> **Inherits:** [`DeviceWatcher`](#devicewatcher)

Monitors device add/remove events via libudev.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`LinuxDeviceWatcher`](#linuxdevicewatcher-1) `explicit` | #### Parameters |
| `bool` | [`start`](#start-3) `virtual` | Begin monitoring via libudev inotify events. |
| `void` | [`stop`](#stop-3) `virtual` | Stop monitoring and close the udev monitor. |

---

{#linuxdevicewatcher-1}

#### LinuxDeviceWatcher

`explicit`

```cpp
explicit LinuxDeviceWatcher(DeviceManager * manager)
```

#### Parameters
* `manager` The [DeviceManager](#devicemanager) whose DevicesChanged signal will be emitted on changes.

---

{#start-3}

#### start

`virtual`

```cpp
virtual bool start()
```

Begin monitoring via libudev inotify events. 
#### Returns
True if the udev monitor was successfully started.

---

{#stop-3}

#### stop

`virtual`

```cpp
virtual void stop()
```

Stop monitoring and close the udev monitor.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< Impl >` | [`_impl`](#_impl-1)  |  |
| `DeviceManager *` | [`_manager`](#_manager-1)  |  |

---

{#_impl-1}

#### _impl

```cpp
std::unique_ptr< Impl > _impl
```

---

{#_manager-1}

#### _manager

```cpp
DeviceManager * _manager
```

{#mediacapture}

## MediaCapture

```cpp
#include <icy/av/mediacapture.h>
```

> **Inherits:** [`ICapture`](#icapture), [`Runnable`](base.md#runnable)
> **Subclassed by:** [`AudioCapture`](#audiocapture), [`VideoCapture`](#videocapture)

Unified capture and decode source for files and live media devices.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `NullSignal` | [`Closing`](#closing)  | Signals that the capture thread is closing. This signal is emitted from the capture thread context. |

---

{#closing}

#### Closing

```cpp
NullSignal Closing
```

Signals that the capture thread is closing. This signal is emitted from the capture thread context.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MediaCapture`](#mediacapture-1)  |  |
|  | [`MediaCapture`](#mediacapture-2)  | Deleted constructor. |
|  | [`MediaCapture`](#mediacapture-3)  | Deleted constructor. |
| `void` | [`openFile`](#openfile-1) `virtual` | Open a media file for decoding. Automatically detects video and audio streams. |
| `void` | [`close`](#close-6) `virtual` | Stop the capture thread and close the media stream and all decoders. |
| `void` | [`start`](#start-4) `virtual` | Start the background capture and decode thread. Throws std::runtime_error if no media streams have been opened. |
| `void` | [`stop`](#stop-4) `virtual` | [Signal](base.md#signal) the capture thread to stop and join it before returning. |
| `void` | [`run`](#run) `virtual` | Entry point for the background capture thread. Reads and decodes packets from the format context until EOF or [stop()](#stop-4) is called. |
| `void` | [`getEncoderFormat`](#getencoderformat-1) `virtual` | Fill `format` with the combined encoder-ready video and audio codec parameters. |
| `void` | [`getEncoderAudioCodec`](#getencoderaudiocodec-1) `virtual` | Fill `params` with the decoder's output audio codec parameters. Throws std::runtime_error if audio parameters have not been initialised. |
| `void` | [`getEncoderVideoCodec`](#getencodervideocodec-1) `virtual` | Fill `params` with the decoder's output video codec parameters. Throws std::runtime_error if video parameters have not been initialised. |
| `void` | [`setLoopInput`](#setloopinput)  | Continuously loop the input file when set. |
| `void` | [`setLimitFramerate`](#setlimitframerate)  | Limit playback to video FPS. |
| `void` | [`setRealtimePTS`](#setrealtimepts)  | Set to use realtime PTS calculation. This is preferred when using live captures as FFmpeg-provided values are not always reliable. |
| `AVFormatContext *` | [`formatCtx`](#formatctx) `const` | #### Returns |
| `VideoDecoder *` | [`video`](#video-1) `const` | #### Returns |
| `AudioDecoder *` | [`audio`](#audio-1) `const` | #### Returns |
| `bool` | [`stopping`](#stopping) `const` | #### Returns |
| `std::string` | [`error`](#error-2) `const` | #### Returns |

---

{#mediacapture-1}

#### MediaCapture

```cpp
MediaCapture()
```

---

{#mediacapture-2}

#### MediaCapture

```cpp
MediaCapture(const MediaCapture &) = delete
```

Deleted constructor.

---

{#mediacapture-3}

#### MediaCapture

```cpp
MediaCapture(MediaCapture &&) = delete
```

Deleted constructor.

---

{#openfile-1}

#### openFile

`virtual`

```cpp
virtual void openFile(const std::string & file)
```

Open a media file for decoding. Automatically detects video and audio streams. 
#### Parameters
* `file` Path to the media file.

---

{#close-6}

#### close

`virtual`

```cpp
virtual void close()
```

Stop the capture thread and close the media stream and all decoders.

---

{#start-4}

#### start

`virtual`

```cpp
virtual void start()
```

Start the background capture and decode thread. Throws std::runtime_error if no media streams have been opened.

---

{#stop-4}

#### stop

`virtual`

```cpp
virtual void stop()
```

[Signal](base.md#signal) the capture thread to stop and join it before returning.

---

{#run}

#### run

`virtual`

```cpp
virtual void run()
```

Entry point for the background capture thread. Reads and decodes packets from the format context until EOF or [stop()](#stop-4) is called.

---

{#getencoderformat-1}

#### getEncoderFormat

`virtual`

```cpp
virtual void getEncoderFormat(Format & format)
```

Fill `format` with the combined encoder-ready video and audio codec parameters. 
#### Parameters
* `format` Output [Format](#format-1) struct to populate.

---

{#getencoderaudiocodec-1}

#### getEncoderAudioCodec

`virtual`

```cpp
virtual void getEncoderAudioCodec(AudioCodec & params)
```

Fill `params` with the decoder's output audio codec parameters. Throws std::runtime_error if audio parameters have not been initialised. 
#### Parameters
* `params` Output [AudioCodec](#audiocodec) struct to populate.

---

{#getencodervideocodec-1}

#### getEncoderVideoCodec

`virtual`

```cpp
virtual void getEncoderVideoCodec(VideoCodec & params)
```

Fill `params` with the decoder's output video codec parameters. Throws std::runtime_error if video parameters have not been initialised. 
#### Parameters
* `params` Output [VideoCodec](#videocodec) struct to populate.

---

{#setloopinput}

#### setLoopInput

```cpp
void setLoopInput(bool flag)
```

Continuously loop the input file when set.

---

{#setlimitframerate}

#### setLimitFramerate

```cpp
void setLimitFramerate(bool flag)
```

Limit playback to video FPS.

---

{#setrealtimepts}

#### setRealtimePTS

```cpp
void setRealtimePTS(bool flag)
```

Set to use realtime PTS calculation. This is preferred when using live captures as FFmpeg-provided values are not always reliable.

---

{#formatctx}

#### formatCtx

`const`

```cpp
AVFormatContext * formatCtx() const
```

#### Returns
The underlying AVFormatContext (thread-safe, mutex-protected).

---

{#video-1}

#### video

`const`

```cpp
VideoDecoder * video() const
```

#### Returns
The active [VideoDecoder](#videodecoder), or nullptr if no video stream was opened.

---

{#audio-1}

#### audio

`const`

```cpp
AudioDecoder * audio() const
```

#### Returns
The active [AudioDecoder](#audiodecoder), or nullptr if no audio stream was opened.

---

{#stopping}

#### stopping

`const`

```cpp
bool stopping() const
```

#### Returns
True if the capture thread has been asked to stop.

---

{#error-2}

#### error

`const`

```cpp
std::string error() const
```

#### Returns
The last error message, or an empty string if no error has occurred.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-2)  |  |
| `Thread` | [`_thread`](#_thread)  |  |
| `AVFormatContext *` | [`_formatCtx`](#_formatctx)  |  |
| `std::unique_ptr< VideoDecoder >` | [`_video`](#_video)  |  |
| `std::unique_ptr< AudioDecoder >` | [`_audio`](#_audio)  |  |
| `std::string` | [`_error`](#_error)  |  |
| `std::atomic< bool >` | [`_stopping`](#_stopping)  |  |
| `std::atomic< bool >` | [`_looping`](#_looping)  |  |
| `std::atomic< bool >` | [`_realtime`](#_realtime)  |  |
| `std::atomic< bool >` | [`_ratelimit`](#_ratelimit)  |  |

---

{#_mutex-2}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_thread}

#### _thread

```cpp
Thread _thread
```

---

{#_formatctx}

#### _formatCtx

```cpp
AVFormatContext * _formatCtx
```

---

{#_video}

#### _video

```cpp
std::unique_ptr< VideoDecoder > _video
```

---

{#_audio}

#### _audio

```cpp
std::unique_ptr< AudioDecoder > _audio
```

---

{#_error}

#### _error

```cpp
std::string _error
```

---

{#_stopping}

#### _stopping

```cpp
std::atomic< bool > _stopping
```

---

{#_looping}

#### _looping

```cpp
std::atomic< bool > _looping
```

---

{#_realtime}

#### _realtime

```cpp
std::atomic< bool > _realtime
```

---

{#_ratelimit}

#### _ratelimit

```cpp
std::atomic< bool > _ratelimit
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`openStream`](#openstream-1) `virtual` | Open the underlying media stream. |
| `void` | [`emit`](#emit) `virtual` | Emit an existing packet directly onto the outgoing signal. |

---

{#openstream-1}

#### openStream

`virtual`

```cpp
virtual void openStream(const std::string & filename, const AVInputFormat * inputFormat, AVDictionary ** formatParams)
```

Open the underlying media stream.

#### Parameters
* `filename` The file path or device name to open. 

* `inputFormat` The forced input format, or nullptr for auto-detect. 

* `formatParams` Optional format parameters; may be updated by FFmpeg on return.

---

{#emit}

#### emit

`virtual`

```cpp
virtual void emit(IPacket & packet)
```

Emit an existing packet directly onto the outgoing signal. 
#### Parameters
* `packet` The packet to forward; must remain valid for the duration of the call.

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-1)  |  |

---

{#ptr-1}

#### Ptr

```cpp
std::shared_ptr< MediaCapture > Ptr()
```

{#multiplexencoder}

## MultiplexEncoder

```cpp
#include <icy/av/multiplexencoder.h>
```

> **Inherits:** [`IEncoder`](#iencoder)
> **Subclassed by:** [`MultiplexPacketEncoder`](#multiplexpacketencoder)

Multiplexing encoder that writes synchronized audio and video streams.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-3)  |  |

---

{#emitter-3}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MultiplexEncoder`](#multiplexencoder-1)  | Construct the encoder with the given options. |
|  | [`MultiplexEncoder`](#multiplexencoder-2)  | Deleted constructor. |
|  | [`MultiplexEncoder`](#multiplexencoder-3)  | Deleted constructor. |
| `void` | [`init`](#init-1) `virtual` | Open the output container, create codec streams, and write the format header. |
| `void` | [`uninit`](#uninit-1) `virtual` | Flush encoded packets, write the format trailer, and close the output container. |
| `void` | [`cleanup`](#cleanup-1) `virtual` | Release all resources allocated by [init()](#init-1) without writing a trailer. |
| `void` | [`createVideo`](#createvideo-1) `virtual` | Create the video encoder and add the stream to the output container. |
| `void` | [`freeVideo`](#freevideo-1) `virtual` | Flush and free the video encoder and its stream. |
| `bool` | [`encodeVideo`](#encodevideo-1) `virtual` | Encode a single video frame. All frame values must be set, such as size, pixel format, and PTS. |
| `bool` | [`encodeVideo`](#encodevideo-2) `virtual` | Encode a single interleaved video frame. If the frame time is specified it should be the microseconds offset since the start of the input stream. If no time is specified a real-time value will be assigned to the frame. |
| `bool` | [`encodeVideo`](#encodevideo-3) `virtual` | Encode a single planar video frame. |
| `void` | [`createAudio`](#createaudio-1) `virtual` | Create the audio encoder and add the stream to the output container. |
| `void` | [`freeAudio`](#freeaudio-1) `virtual` | Flush and free the audio encoder and its stream. |
| `bool` | [`encodeAudio`](#encodeaudio) `virtual` | Encode a single interleaved audio frame. |
| `bool` | [`encodeAudio`](#encodeaudio-1) `virtual` | Encode a single planar audio frame. |
| `void` | [`flush`](#flush-4) `virtual` | Flush any buffered or queued packets to the output container. |
| `EncoderOptions &` | [`options`](#options-2) `virtual` | #### Returns |
| `VideoEncoder *` | [`video`](#video-2)  | #### Returns |
| `AudioEncoder *` | [`audio`](#audio-2)  | #### Returns |

---

{#multiplexencoder-1}

#### MultiplexEncoder

```cpp
MultiplexEncoder(const EncoderOptions & options)
```

Construct the encoder with the given options. 
#### Parameters
* `options` The encoder configuration (input/output formats and file paths).

---

{#multiplexencoder-2}

#### MultiplexEncoder

```cpp
MultiplexEncoder(const MultiplexEncoder &) = delete
```

Deleted constructor.

---

{#multiplexencoder-3}

#### MultiplexEncoder

```cpp
MultiplexEncoder(MultiplexEncoder &&) = delete
```

Deleted constructor.

---

{#init-1}

#### init

`virtual`

```cpp
virtual void init()
```

Open the output container, create codec streams, and write the format header.

---

{#uninit-1}

#### uninit

`virtual`

```cpp
virtual void uninit()
```

Flush encoded packets, write the format trailer, and close the output container.

---

{#cleanup-1}

#### cleanup

`virtual`

```cpp
virtual void cleanup()
```

Release all resources allocated by [init()](#init-1) without writing a trailer.

---

{#createvideo-1}

#### createVideo

`virtual`

```cpp
virtual void createVideo()
```

Create the video encoder and add the stream to the output container.

---

{#freevideo-1}

#### freeVideo

`virtual`

```cpp
virtual void freeVideo()
```

Flush and free the video encoder and its stream.

---

{#encodevideo-1}

#### encodeVideo

`virtual`

```cpp
virtual bool encodeVideo(AVFrame * frame)
```

Encode a single video frame. All frame values must be set, such as size, pixel format, and PTS.

---

{#encodevideo-2}

#### encodeVideo

`virtual`

```cpp
virtual bool encodeVideo(uint8_t * buffer, int bufferSize, int width, int height, int64_t time)
```

Encode a single interleaved video frame. If the frame time is specified it should be the microseconds offset since the start of the input stream. If no time is specified a real-time value will be assigned to the frame.

#### Parameters
* `buffer` The raw video frame buffer. 

* `bufferSize` The buffer size in bytes. 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

---

{#encodevideo-3}

#### encodeVideo

`virtual`

```cpp
virtual bool encodeVideo(uint8_t * data, int linesize, int width, int height, int64_t time)
```

Encode a single planar video frame.

#### Parameters
* `data` Array of per-plane data pointers (up to 4 planes). 

* `linesize` Array of per-plane byte strides. 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

---

{#createaudio-1}

#### createAudio

`virtual`

```cpp
virtual void createAudio()
```

Create the audio encoder and add the stream to the output container.

---

{#freeaudio-1}

#### freeAudio

`virtual`

```cpp
virtual void freeAudio()
```

Flush and free the audio encoder and its stream.

---

{#encodeaudio}

#### encodeAudio

`virtual`

```cpp
virtual bool encodeAudio(uint8_t * buffer, int numSamples, int64_t time)
```

Encode a single interleaved audio frame.

#### Parameters
* `buffer` The interleaved audio sample buffer. 

* `numSamples` The number of samples per channel. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

---

{#encodeaudio-1}

#### encodeAudio

`virtual`

```cpp
virtual bool encodeAudio(uint8_t * data, int numSamples, int64_t time)
```

Encode a single planar audio frame.

#### Parameters
* `data` Array of per-plane sample buffers (one per channel). 

* `numSamples` The number of samples per channel. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

---

{#flush-4}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush any buffered or queued packets to the output container.

---

{#options-2}

#### options

`virtual`

```cpp
virtual EncoderOptions & options()
```

#### Returns
A reference to the encoder's configuration options.

---

{#video-2}

#### video

```cpp
VideoEncoder * video()
```

#### Returns
The active [VideoEncoder](#videoencoder), or nullptr if video has not been created.

---

{#audio-2}

#### audio

```cpp
AudioEncoder * audio()
```

#### Returns
The active [AudioEncoder](#audioencoder), or nullptr if audio has not been created.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EncoderOptions` | [`_options`](#_options)  |  |
| `AVFormatContext *` | [`_formatCtx`](#_formatctx-1)  |  |
| `std::unique_ptr< VideoEncoder >` | [`_video`](#_video-1)  |  |
| `std::unique_ptr< AudioEncoder >` | [`_audio`](#_audio-1)  |  |
| `AVIOContext *` | [`_ioCtx`](#_ioctx)  |  |
| `std::unique_ptr< uint8_t[]>` | [`_ioBuffer`](#_iobuffer)  |  |
| `int64_t` | [`_pts`](#_pts)  |  |
| `std::mutex` | [`_mutex`](#_mutex-3)  |  |

---

{#_options}

#### _options

```cpp
EncoderOptions _options
```

---

{#_formatctx-1}

#### _formatCtx

```cpp
AVFormatContext * _formatCtx
```

---

{#_video-1}

#### _video

```cpp
std::unique_ptr< VideoEncoder > _video
```

---

{#_audio-1}

#### _audio

```cpp
std::unique_ptr< AudioEncoder > _audio
```

---

{#_ioctx}

#### _ioCtx

```cpp
AVIOContext * _ioCtx
```

---

{#_iobuffer}

#### _ioBuffer

```cpp
std::unique_ptr< uint8_t[]> _ioBuffer
```

---

{#_pts}

#### _pts

```cpp
int64_t _pts
```

---

{#_mutex-3}

#### _mutex

```cpp
std::mutex _mutex
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`writeOutputPacket`](#writeoutputpacket)  |  |
| `bool` | [`updateStreamPts`](#updatestreampts)  | Convert input microseconds to the stream time base. |
| `void` | [`onVideoEncoded`](#onvideoencoded)  |  |
| `void` | [`onAudioEncoded`](#onaudioencoded)  |  |

---

{#writeoutputpacket}

#### writeOutputPacket

```cpp
bool writeOutputPacket(AVPacket & packet)
```

---

{#updatestreampts}

#### updateStreamPts

```cpp
bool updateStreamPts(AVStream * stream, int64_t * pts)
```

Convert input microseconds to the stream time base.

#### Parameters
* `stream` The target stream for time base conversion. 

* `pts` Pointer to the timestamp; converted in place.

---

{#onvideoencoded}

#### onVideoEncoded

```cpp
void onVideoEncoded(av::VideoPacket & packet)
```

---

{#onaudioencoded}

#### onAudioEncoded

```cpp
void onAudioEncoded(av::AudioPacket & packet)
```

{#multiplexpacketencoder}

## MultiplexPacketEncoder

```cpp
#include <icy/av/multiplexpacketencoder.h>
```

> **Inherits:** [`MultiplexEncoder`](#multiplexencoder), [`PacketProcessor`](base.md#packetprocessor)

Encodes and multiplexes a realtime video stream form audio / video capture sources. FFmpeg is used for encoding.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MultiplexPacketEncoder`](#multiplexpacketencoder-1)  | Construct the encoder with the given options. |
|  | [`MultiplexPacketEncoder`](#multiplexpacketencoder-2)  | Deleted constructor. |
|  | [`MultiplexPacketEncoder`](#multiplexpacketencoder-3)  | Deleted constructor. |
| `void` | [`encode`](#encode-6) `virtual` | Encode a [VideoPacket](#videopacket), dispatching to the planar or interleaved encode path as appropriate. |
| `void` | [`encode`](#encode-7) `virtual` | Encode an [AudioPacket](#audiopacket), dispatching to the planar or interleaved encode path as appropriate. |
| `bool` | [`accepts`](#accepts-1) `virtual` | #### Returns |
| `void` | [`process`](#process-3) `virtual` | Dispatch the incoming packet to [encode(VideoPacket&)](#encode-6) or [encode(AudioPacket&)](#encode-7). Throws std::invalid_argument if the packet type is unrecognised. |

---

{#multiplexpacketencoder-1}

#### MultiplexPacketEncoder

```cpp
MultiplexPacketEncoder(const EncoderOptions & options)
```

Construct the encoder with the given options. 
#### Parameters
* `options` The encoder configuration (input/output formats and file paths).

---

{#multiplexpacketencoder-2}

#### MultiplexPacketEncoder

```cpp
MultiplexPacketEncoder(const MultiplexPacketEncoder &) = delete
```

Deleted constructor.

---

{#multiplexpacketencoder-3}

#### MultiplexPacketEncoder

```cpp
MultiplexPacketEncoder(MultiplexPacketEncoder &&) = delete
```

Deleted constructor.

---

{#encode-6}

#### encode

`virtual`

```cpp
virtual void encode(VideoPacket & packet)
```

Encode a [VideoPacket](#videopacket), dispatching to the planar or interleaved encode path as appropriate. 
#### Parameters
* `packet` The video packet to encode.

---

{#encode-7}

#### encode

`virtual`

```cpp
virtual void encode(AudioPacket & packet)
```

Encode an [AudioPacket](#audiopacket), dispatching to the planar or interleaved encode path as appropriate. 
#### Parameters
* `packet` The audio packet to encode.

---

{#accepts-1}

#### accepts

`virtual`

```cpp
virtual bool accepts(IPacket * packet)
```

#### Returns
True if the packet is an [av::MediaPacket](#mediapacket) (audio or video).

---

{#process-3}

#### process

`virtual`

```cpp
virtual void process(IPacket & packet)
```

Dispatch the incoming packet to [encode(VideoPacket&)](#encode-6) or [encode(AudioPacket&)](#encode-7). Throws std::invalid_argument if the packet type is unrecognised. 
#### Parameters
* `packet` The incoming media packet.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-4)  |  |

---

{#_mutex-4}

#### _mutex

```cpp
std::mutex _mutex
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onStreamStateChange`](#onstreamstatechange-3) `virtual` | Called by the [PacketStream](base.md#packetstream) to notify when the internal [Stream](base.md#stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |

---

{#onstreamstatechange-3}

#### onStreamStateChange

`virtual`

```cpp
virtual void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](base.md#packetstream) to notify when the internal [Stream](base.md#stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

{#realtimepacketqueue}

## RealtimePacketQueue

```cpp
#include <icy/av/realtimepacketqueue.h>
```

> **Inherits:** [`AsyncPacketQueue< PacketT >`](base.md#asyncpacketqueue)

[Queue](base.md#queue) that emits media packets in presentation-timestamp order relative to a realtime clock.

Packets are sorted by their `time` field on insertion. On each pop attempt the queue checks whether the wall-clock time since stream activation has reached the next packet's timestamp; if not, the packet is held back. This provides a soft realtime playback scheduler.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`RealtimePacketQueue`](#realtimepacketqueue-1) `inline` | Construct the queue with a maximum capacity. |
| `void` | [`push`](#push) `virtual` `inline` | Insert a packet into the queue and re-sort by presentation timestamp. |
| `int64_t` | [`realTime`](#realtime) `inline` | Return the elapsed time since stream activation in microseconds. |

---

{#realtimepacketqueue-1}

#### RealtimePacketQueue

`inline`

```cpp
inline RealtimePacketQueue(int maxSize)
```

Construct the queue with a maximum capacity. 
#### Parameters
* `maxSize` The maximum number of packets the queue will hold.

---

{#push}

#### push

`virtual` `inline`

```cpp
virtual inline void push(PacketT * item)
```

Insert a packet into the queue and re-sort by presentation timestamp. 
#### Parameters
* `item` The packet to enqueue; ownership is transferred.

---

{#realtime}

#### realTime

`inline`

```cpp
inline int64_t realTime()
```

Return the elapsed time since stream activation in microseconds.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int64_t` | [`_startTime`](#_starttime)  |  |

---

{#_starttime}

#### _startTime

```cpp
int64_t _startTime
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `PacketT *` | [`popNext`](#popnext) `virtual` `inline` | Return the next packet whose timestamp is <= [realTime()](#realtime), or nullptr if none is ready. |
| `void` | [`onStreamStateChange`](#onstreamstatechange-4) `virtual` `inline` | Record the stream start time when the stream becomes active. |

---

{#popnext}

#### popNext

`virtual` `inline`

```cpp
virtual inline PacketT * popNext()
```

Return the next packet whose timestamp is <= [realTime()](#realtime), or nullptr if none is ready.

---

{#onstreamstatechange-4}

#### onStreamStateChange

`virtual` `inline`

```cpp
virtual inline void onStreamStateChange(const PacketStreamState & state)
```

Record the stream start time when the stream becomes active.

### Public Types

| Name | Description |
|------|-------------|
| [`BaseQueue`](#basequeue)  |  |

---

{#basequeue}

#### BaseQueue

```cpp
AsyncPacketQueue< PacketT > BaseQueue()
```

{#mediapackettimecompare}

## MediaPacketTimeCompare

```cpp
#include <icy/av/realtimepacketqueue.h>
```

Comparator for ordering media packets by presentation timestamp.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`operator()`](#operator-3) `inline` |  |

---

{#operator-3}

#### operator()

`inline`

```cpp
inline bool operator()(const MediaPacket * a, const MediaPacket * b)
```

{#videocapture}

## VideoCapture

```cpp
#include <icy/av/videocapture.h>
```

> **Inherits:** [`MediaCapture`](#mediacapture)

Cross-platform video device capturer backed by FFmpeg avdevice.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoCapture`](#videocapture-1)  | Construct without opening a device. Call [openVideo()](#openvideo-1) before [start()](#start-4). |
|  | [`VideoCapture`](#videocapture-2)  | Construct and immediately open the given video device using a [VideoCodec](#videocodec) params struct. |
|  | [`VideoCapture`](#videocapture-3)  | Construct and immediately open the given video device with individual parameters. |
| `void` | [`openVideo`](#openvideo-1) `virtual` | Open the given video device using a [VideoCodec](#videocodec) params struct. |
| `void` | [`openVideo`](#openvideo-2) `virtual` | Open the given video device with individual parameters. Configures the FFmpeg input format and passes device options via AVDictionary. If the device cannot satisfy the requested parameters, pixel format conversion and scaling will be applied by the decoder. |

---

{#videocapture-1}

#### VideoCapture

```cpp
VideoCapture()
```

Construct without opening a device. Call [openVideo()](#openvideo-1) before [start()](#start-4).

---

{#videocapture-2}

#### VideoCapture

```cpp
VideoCapture(const std::string & device, const av::VideoCodec & params)
```

Construct and immediately open the given video device using a [VideoCodec](#videocodec) params struct. 
#### Parameters
* `device` The platform video device identifier (e.g. "/dev/video0" or "0" on Windows). 

* `params` The desired capture parameters (width, height, fps, pixel format).

---

{#videocapture-3}

#### VideoCapture

```cpp
VideoCapture(const std::string & device, int width, int height, double framerate, const std::string & pixelFmt)
```

Construct and immediately open the given video device with individual parameters. 
#### Parameters
* `device` The platform video device identifier. 

* `width` The desired frame width in pixels, or 0 for device default. 

* `height` The desired frame height in pixels, or 0 for device default. 

* `framerate` The desired frame rate, or 0 for device default. 

* `pixelFmt` The desired pixel format string, or "" for "yuv420p".

---

{#openvideo-1}

#### openVideo

`virtual`

```cpp
virtual void openVideo(const std::string & device, const av::VideoCodec & params)
```

Open the given video device using a [VideoCodec](#videocodec) params struct. 
#### Parameters
* `device` The platform video device identifier. 

* `params` The desired capture parameters.

---

{#openvideo-2}

#### openVideo

`virtual`

```cpp
virtual void openVideo(const std::string & device, int width, int height, double framerate, const std::string & pixelFmt)
```

Open the given video device with individual parameters. Configures the FFmpeg input format and passes device options via AVDictionary. If the device cannot satisfy the requested parameters, pixel format conversion and scaling will be applied by the decoder. 
#### Parameters
* `device` The platform video device identifier. 

* `width` The desired frame width in pixels, or 0 for device default. 

* `height` The desired frame height in pixels, or 0 for device default. 

* `framerate` The desired frame rate, or 0 for device default. 

* `pixelFmt` The desired pixel format string, or "" for "yuv420p".

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-2)  |  |

---

{#ptr-2}

#### Ptr

```cpp
std::shared_ptr< VideoCapture > Ptr()
```

{#videopacketencoder}

## VideoPacketEncoder

```cpp
#include <icy/av/videopacketencoder.h>
```

> **Inherits:** [`VideoEncoder`](#videoencoder), [`PacketProcessor`](base.md#packetprocessor)

[PacketProcessor](base.md#packetprocessor) that encodes raw video frames ([PlanarVideoPacket](#planarvideopacket) or [VideoPacket](#videopacket)) into compressed packets via [VideoEncoder](#videoencoder).

Drop this into a [PacketStream](base.md#packetstream) between a source that emits raw frames (e.g. [MediaCapture](#mediacapture)) and a sink that expects encoded data (e.g. WebRtcTrackSender or [MultiplexPacketEncoder](#multiplexpacketencoder)).

Example:

auto encoder = std::make_shared<av::VideoPacketEncoder>(); encoder->iparams = capture->videoCodec(); // decoded format encoder->oparams = [av::VideoCodec](#videocodec)("H264", "libx264", 640, 480, 30);

[PacketStream](base.md#packetstream) stream; stream.attachSource(capture); stream.attach(encoder, 1, true); stream.attach(&webrtcSender, 5, false); stream.start();

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoPacketEncoder`](#videopacketencoder-1)  |  |
|  | [`VideoPacketEncoder`](#videopacketencoder-2)  | Deleted constructor. |
| `void` | [`process`](#process-4) `virtual` | [Process](base.md#process) a [VideoPacket](#videopacket) or [PlanarVideoPacket](#planarvideopacket) from the stream. Encodes the frame and emits the resulting compressed packet. |
| `bool` | [`accepts`](#accepts-2) `virtual` | Accept [VideoPacket](#videopacket) and [PlanarVideoPacket](#planarvideopacket) types. |

---

{#videopacketencoder-1}

#### VideoPacketEncoder

```cpp
VideoPacketEncoder(AVFormatContext * format)
```

---

{#videopacketencoder-2}

#### VideoPacketEncoder

```cpp
VideoPacketEncoder(const VideoPacketEncoder &) = delete
```

Deleted constructor.

---

{#process-4}

#### process

`virtual`

```cpp
virtual void process(IPacket & packet)
```

[Process](base.md#process) a [VideoPacket](#videopacket) or [PlanarVideoPacket](#planarvideopacket) from the stream. Encodes the frame and emits the resulting compressed packet.

---

{#accepts-2}

#### accepts

`virtual`

```cpp
virtual bool accepts(IPacket * packet)
```

Accept [VideoPacket](#videopacket) and [PlanarVideoPacket](#planarvideopacket) types.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`_initialized`](#_initialized-1)  |  |
| `std::mutex` | [`_mutex`](#_mutex-5)  |  |

---

{#_initialized-1}

#### _initialized

```cpp
bool _initialized = false
```

---

{#_mutex-5}

#### _mutex

```cpp
std::mutex _mutex
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onStreamStateChange`](#onstreamstatechange-5) `virtual` | Called by the [PacketStream](base.md#packetstream) to notify when the internal [Stream](base.md#stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |

---

{#onstreamstatechange-5}

#### onStreamStateChange

`virtual`

```cpp
virtual void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](base.md#packetstream) to notify when the internal [Stream](base.md#stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

{#windowsdevicewatcher}

## WindowsDeviceWatcher

```cpp
#include <icy/av/win32/mediafoundation.h>
```

> **Inherits:** [`DeviceWatcher`](#devicewatcher)

Monitors device add/remove events via IMMNotificationClient (audio) and RegisterDeviceNotification (video).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WindowsDeviceWatcher`](#windowsdevicewatcher-1) `explicit` | #### Parameters |
| `bool` | [`start`](#start-5) `virtual` | Begin monitoring via IMMNotificationClient and RegisterDeviceNotification. |
| `void` | [`stop`](#stop-5) `virtual` | Stop monitoring and unregister all device notifications. |

---

{#windowsdevicewatcher-1}

#### WindowsDeviceWatcher

`explicit`

```cpp
explicit WindowsDeviceWatcher(DeviceManager * manager)
```

#### Parameters
* `manager` The [DeviceManager](#devicemanager) whose DevicesChanged signal will be emitted on changes.

---

{#start-5}

#### start

`virtual`

```cpp
virtual bool start()
```

Begin monitoring via IMMNotificationClient and RegisterDeviceNotification. 
#### Returns
True if monitoring was successfully started.

---

{#stop-5}

#### stop

`virtual`

```cpp
virtual void stop()
```

Stop monitoring and unregister all device notifications.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< Impl >` | [`_impl`](#_impl-2)  |  |
| `DeviceManager *` | [`_manager`](#_manager-2)  |  |

---

{#_impl-2}

#### _impl

```cpp
std::unique_ptr< Impl > _impl
```

---

{#_manager-2}

#### _manager

```cpp
DeviceManager * _manager
```

{#audiobuffer}

## AudioBuffer

```cpp
#include <icy/av/audiobuffer.h>
```

FIFO buffer for queuing audio samples between encoding stages.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `AVAudioFifo *` | [`fifo`](#fifo)  | Underlying FFmpeg audio FIFO handle. |

---

{#fifo}

#### fifo

```cpp
AVAudioFifo * fifo
```

Underlying FFmpeg audio FIFO handle.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioBuffer`](#audiobuffer-1)  |  |
|  | [`AudioBuffer`](#audiobuffer-2)  | Deleted constructor. |
|  | [`AudioBuffer`](#audiobuffer-3)  | Deleted constructor. |
| `void` | [`alloc`](#alloc)  | Allocate the audio FIFO buffer. |
| `void` | [`reset`](#reset)  | Discard all samples currently held in the FIFO without freeing the buffer. |
| `void` | [`close`](#close)  | Free the underlying AVAudioFifo buffer. |
| `void` | [`write`](#write)  | Write samples into the FIFO buffer. |
| `bool` | [`read`](#read)  | Read samples from the FIFO buffer. |
| `int` | [`available`](#available) `const` | Return the number of samples per channel currently available in the FIFO. |

---

{#audiobuffer-1}

#### AudioBuffer

```cpp
AudioBuffer()
```

---

{#audiobuffer-2}

#### AudioBuffer

```cpp
AudioBuffer(const AudioBuffer &) = delete
```

Deleted constructor.

---

{#audiobuffer-3}

#### AudioBuffer

```cpp
AudioBuffer(AudioBuffer &&) = delete
```

Deleted constructor.

---

{#alloc}

#### alloc

```cpp
void alloc(const std::string & sampleFmt, int channels, int numSamples)
```

Allocate the audio FIFO buffer.

#### Parameters
* `sampleFmt` The sample format name (e.g. "s16", "fltp"). 

* `channels` The number of audio channels. 

* `numSamples` The initial buffer capacity in samples per channel.

---

{#reset}

#### reset

```cpp
void reset()
```

Discard all samples currently held in the FIFO without freeing the buffer.

---

{#close}

#### close

```cpp
void close()
```

Free the underlying AVAudioFifo buffer.

---

{#write}

#### write

```cpp
void write(void ** samples, int numSamples)
```

Write samples into the FIFO buffer.

#### Parameters
* `samples` Array of per-channel sample buffers. 

* `numSamples` The number of samples per channel to write.

---

{#read}

#### read

```cpp
bool read(void ** samples, int numSamples)
```

Read samples from the FIFO buffer.

#### Parameters
* `samples` Array of per-channel sample buffers to fill. 

* `numSamples` The number of samples per channel to read. 

#### Returns
True if enough samples were available.

---

{#available}

#### available

`const`

```cpp
int available() const
```

Return the number of samples per channel currently available in the FIFO. 
#### Returns
The number of available samples, or zero if the buffer is not allocated.

{#audiocodec}

## AudioCodec

```cpp
#include <icy/av/codec.h>
```

> **Inherits:** [`Codec`](#codec-1)

Audio codec parameters including channels, sample rate, and sample format.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`channels`](#channels)  |  |
| `std::string` | [`sampleFmt`](#samplefmt)  | One of: u8, s16, s32, flt, dbl, u8p, s16p, s32p, fltp, dblp. |

---

{#channels}

#### channels

```cpp
int channels
```

---

{#samplefmt}

#### sampleFmt

```cpp
std::string sampleFmt
```

One of: u8, s16, s32, flt, dbl, u8p, s16p, s32p, fltp, dblp.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioCodec`](#audiocodec-1)  | Construct a disabled audio codec with zeroed parameters. |
|  | [`AudioCodec`](#audiocodec-2)  | Construct an anonymous audio codec from raw parameters. |
|  | [`AudioCodec`](#audiocodec-3)  | Construct a named audio codec. |
|  | [`AudioCodec`](#audiocodec-4)  | Construct a named audio codec with an explicit FFmpeg encoder name. |
| `std::string` | [`toString`](#tostring) `virtual` `const` | #### Returns |
| `void` | [`print`](#print) `virtual` | Print a multi-line human-readable description to the given stream. |

---

{#audiocodec-1}

#### AudioCodec

```cpp
AudioCodec()
```

Construct a disabled audio codec with zeroed parameters.

---

{#audiocodec-2}

#### AudioCodec

```cpp
AudioCodec(int channels, int sampleRate, const std::string & sampleFmt, int bitRate)
```

Construct an anonymous audio codec from raw parameters. 
#### Parameters
* `channels` The number of audio channels. 

* `sampleRate` The sample rate in Hz. 

* `sampleFmt` The sample format string (e.g. "s16", "fltp"). 

* `bitRate` The target bit rate in bits per second.

---

{#audiocodec-3}

#### AudioCodec

```cpp
AudioCodec(const std::string & name, int channels, int sampleRate, int bitRate, const std::string & sampleFmt)
```

Construct a named audio codec. 
#### Parameters
* `name` The codec display name. 

* `channels` The number of audio channels. 

* `sampleRate` The sample rate in Hz. 

* `bitRate` The target bit rate in bits per second. 

* `sampleFmt` The sample format string.

---

{#audiocodec-4}

#### AudioCodec

```cpp
AudioCodec(const std::string & name, const std::string & encoder, int channels, int sampleRate, int bitRate, const std::string & sampleFmt)
```

Construct a named audio codec with an explicit FFmpeg encoder name. 
#### Parameters
* `name` The codec display name. 

* `encoder` The FFmpeg encoder name (e.g. "libopus"). 

* `channels` The number of audio channels. 

* `sampleRate` The sample rate in Hz. 

* `bitRate` The target bit rate in bits per second. 

* `sampleFmt` The sample format string.

---

{#tostring}

#### toString

`virtual` `const`

```cpp
virtual std::string toString() const
```

#### Returns
A string in the form "AudioCodec[name:encoder:sampleRate:bitRate:channels:sampleFmt:enabled]".

---

{#print}

#### print

`virtual`

```cpp
virtual void print(std::ostream & ost)
```

Print a multi-line human-readable description to the given stream. 
#### Parameters
* `ost` The output stream to write to.

{#audiocontext}

## AudioContext

```cpp
#include <icy/av/audiocontext.h>
```

> **Subclassed by:** [`AudioDecoder`](#audiodecoder), [`AudioEncoder`](#audioencoder)

Base context for audio encoding and decoding via FFmpeg.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter)  |  |
| `AudioCodec` | [`iparams`](#iparams)  | input parameters |
| `AudioCodec` | [`oparams`](#oparams)  | output parameters |
| `AVStream *` | [`stream`](#stream-1)  | encoder or decoder stream |
| `AVCodecContext *` | [`ctx`](#ctx)  | encoder or decoder context |
| `const AVCodec *` | [`codec`](#codec)  | encoder or decoder codec |
| `AVFrame *` | [`frame`](#frame)  | last encoded or decoded frame |
| `std::unique_ptr< AudioResampler >` | [`resampler`](#resampler)  | audio resampler |
| `int` | [`outputFrameSize`](#outputframesize)  | encoder or decoder output frame size |
| `int64_t` | [`time`](#time)  | stream time in codec time base |
| `int64_t` | [`pts`](#pts)  | last packet pts value |
| `double` | [`seconds`](#seconds)  | audio time in seconds |
| `std::string` | [`error`](#error-1)  | error message |

---

{#emitter}

#### emitter

```cpp
PacketSignal emitter
```

---

{#iparams}

#### iparams

```cpp
AudioCodec iparams
```

input parameters

---

{#oparams}

#### oparams

```cpp
AudioCodec oparams
```

output parameters

---

{#stream-1}

#### stream

```cpp
AVStream * stream
```

encoder or decoder stream

---

{#ctx}

#### ctx

```cpp
AVCodecContext * ctx
```

encoder or decoder context

---

{#codec}

#### codec

```cpp
const AVCodec * codec
```

encoder or decoder codec

---

{#frame}

#### frame

```cpp
AVFrame * frame
```

last encoded or decoded frame

---

{#resampler}

#### resampler

```cpp
std::unique_ptr< AudioResampler > resampler
```

audio resampler

---

{#outputframesize}

#### outputFrameSize

```cpp
int outputFrameSize
```

encoder or decoder output frame size

---

{#time}

#### time

```cpp
int64_t time
```

stream time in codec time base

---

{#pts}

#### pts

```cpp
int64_t pts
```

last packet pts value

---

{#seconds}

#### seconds

```cpp
double seconds
```

audio time in seconds

---

{#error-1}

#### error

```cpp
std::string error
```

error message

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioContext`](#audiocontext-1)  |  |
|  | [`AudioContext`](#audiocontext-2)  | Deleted constructor. |
|  | [`AudioContext`](#audiocontext-3)  | Deleted constructor. |
| `void` | [`create`](#create)  | Initialise the AVCodecContext with codec-specific defaults. Implemented by [AudioEncoder](#audioencoder) and [AudioDecoder](#audiodecoder). |
| `void` | [`open`](#open) `virtual` | Open the codec and create the resampler if input/output parameters differ. Throws std::runtime_error if the codec context has not been created. |
| `void` | [`close`](#close-1) `virtual` | Close the codec context, free the frame, and reset timestamps. |
| `bool` | [`decode`](#decode) `virtual` | Decode a compressed audio packet and emit the resulting samples. |
| `bool` | [`encode`](#encode) `virtual` | Encode a buffer of interleaved audio samples. |
| `bool` | [`encode`](#encode-1) `virtual` | Encode a buffer of planar audio samples. |
| `bool` | [`encode`](#encode-2) `virtual` | Encode a single AVFrame. |
| `void` | [`flush`](#flush) `virtual` | Flush any frames buffered inside the codec and emit remaining output. |
| `bool` | [`recreateResampler`](#recreateresampler) `virtual` | Recreate the [AudioResampler](#audioresampler) using the current iparams and oparams. Called automatically by [open()](#open) when format conversion is required. |

---

{#audiocontext-1}

#### AudioContext

```cpp
AudioContext()
```

---

{#audiocontext-2}

#### AudioContext

```cpp
AudioContext(const AudioContext &) = delete
```

Deleted constructor.

---

{#audiocontext-3}

#### AudioContext

```cpp
AudioContext(AudioContext &&) = delete
```

Deleted constructor.

---

{#create}

#### create

```cpp
void create()
```

Initialise the AVCodecContext with codec-specific defaults. Implemented by [AudioEncoder](#audioencoder) and [AudioDecoder](#audiodecoder).

---

{#open}

#### open

`virtual`

```cpp
virtual void open()
```

Open the codec and create the resampler if input/output parameters differ. Throws std::runtime_error if the codec context has not been created.

---

{#close-1}

#### close

`virtual`

```cpp
virtual void close()
```

Close the codec context, free the frame, and reset timestamps.

---

{#decode}

#### decode

`virtual`

```cpp
virtual bool decode(AVPacket & ipacket)
```

Decode a compressed audio packet and emit the resulting samples. 
#### Parameters
* `ipacket` The compressed audio packet to decode. 

#### Returns
True if one or more decoded frames were emitted, false otherwise.

---

{#encode}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * samples, int numSamples, int64_t pts)
```

Encode a buffer of interleaved audio samples. 
#### Parameters
* `samples` Pointer to the interleaved sample buffer. 

* `numSamples` Number of samples per channel. 

* `pts` Presentation timestamp in stream time base units. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#encode-1}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * samples, int numSamples, int64_t pts)
```

Encode a buffer of planar audio samples. 
#### Parameters
* `samples` Array of per-channel sample buffers (up to 4 planes). 

* `numSamples` Number of samples per channel. 

* `pts` Presentation timestamp in stream time base units. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#encode-2}

#### encode

`virtual`

```cpp
virtual bool encode(AVFrame * iframe)
```

Encode a single AVFrame. 
#### Parameters
* `iframe` The source audio frame; must have all fields set correctly. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#flush}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush any frames buffered inside the codec and emit remaining output.

---

{#recreateresampler}

#### recreateResampler

`virtual`

```cpp
virtual bool recreateResampler()
```

Recreate the [AudioResampler](#audioresampler) using the current iparams and oparams. Called automatically by [open()](#open) when format conversion is required. 
#### Returns
True if the resampler was successfully created.

{#audiodecoder}

## AudioDecoder

```cpp
#include <icy/av/audiodecoder.h>
```

> **Inherits:** [`AudioContext`](#audiocontext)

Decodes compressed audio packets into raw sample frames.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioDecoder`](#audiodecoder-1)  | Construct a decoder for the given stream. The codec parameters are read from the stream's codecpar. |
| `void` | [`create`](#create-1) `virtual` | Initialise the AVCodecContext from the stream's codec parameters. |
| `void` | [`close`](#close-2) `virtual` | Close and free the AVCodecContext and associated resources. |
| `bool` | [`decode`](#decode-1) `virtual` | Decode the given compressed audio packet and emit the decoded samples. |
| `void` | [`flush`](#flush-1) `virtual` | Flush any frames buffered inside the decoder. Call this after the last packet to retrieve all remaining decoded output. |

---

{#audiodecoder-1}

#### AudioDecoder

```cpp
AudioDecoder(AVStream * stream)
```

Construct a decoder for the given stream. The codec parameters are read from the stream's codecpar. 
#### Parameters
* `stream` The AVStream to decode; must remain valid for the lifetime of this decoder.

---

{#create-1}

#### create

`virtual`

```cpp
virtual void create()
```

Initialise the AVCodecContext from the stream's codec parameters.

---

{#close-2}

#### close

`virtual`

```cpp
virtual void close()
```

Close and free the AVCodecContext and associated resources.

---

{#decode-1}

#### decode

`virtual`

```cpp
virtual bool decode(AVPacket & ipacket)
```

Decode the given compressed audio packet and emit the decoded samples. 
#### Parameters
* `ipacket` The compressed audio packet to decode. 

#### Returns
True if one or more output frames were decoded and emitted, false otherwise.

---

{#flush-1}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush any frames buffered inside the decoder. Call this after the last packet to retrieve all remaining decoded output.

{#audioencoder}

## AudioEncoder

```cpp
#include <icy/av/audioencoder.h>
```

> **Inherits:** [`AudioContext`](#audiocontext)
> **Subclassed by:** [`AudioPacketEncoder`](#audiopacketencoder)

Encodes raw audio samples into a compressed format.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `av::AudioBuffer` | [`fifo`](#fifo-1)  |  |
| `AVFormatContext *` | [`format`](#format)  |  |

---

{#fifo-1}

#### fifo

```cpp
av::AudioBuffer fifo
```

---

{#format}

#### format

```cpp
AVFormatContext * format
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioEncoder`](#audioencoder-1)  | Construct an encoder, optionally tied to an existing muxer context. |
| `void` | [`create`](#create-2) `virtual` | Initialise the AVCodecContext using oparams. Adds an audio stream to `format` if one was provided at construction. |
| `void` | [`close`](#close-3) `virtual` | Close and free the AVCodecContext, FIFO buffer, and associated resources. |
| `bool` | [`encode`](#encode-3) `virtual` | Encode interleaved audio samples. |
| `bool` | [`encode`](#encode-4) `virtual` | Encode planar audio samples. |
| `bool` | [`encode`](#encode-5) `virtual` | Encode a single AVFrame (typically from a decoder or resampler). |
| `void` | [`flush`](#flush-2) `virtual` | Flush remaining packets to be encoded. This method should be called once before stream closure. |

---

{#audioencoder-1}

#### AudioEncoder

```cpp
AudioEncoder(AVFormatContext * format)
```

Construct an encoder, optionally tied to an existing muxer context. 
#### Parameters
* `format` The AVFormatContext that will receive encoded packets, or nullptr for standalone use.

---

{#create-2}

#### create

`virtual`

```cpp
virtual void create()
```

Initialise the AVCodecContext using oparams. Adds an audio stream to `format` if one was provided at construction.

---

{#close-3}

#### close

`virtual`

```cpp
virtual void close()
```

Close and free the AVCodecContext, FIFO buffer, and associated resources.

---

{#encode-3}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * samples, const int numSamples, const int64_t pts)
```

Encode interleaved audio samples.

#### Parameters
* `samples` The input samples to encode. 

* `numSamples` The number of input samples per channel. 

* `pts` The input samples presentation timestamp.

---

{#encode-4}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * samples, const int numSamples, const int64_t pts)
```

Encode planar audio samples.

#### Parameters
* `samples` The input samples to encode. 

* `numSamples` The number of input samples per channel. 

* `pts` The input samples presentation timestamp.

---

{#encode-5}

#### encode

`virtual`

```cpp
virtual bool encode(AVFrame * iframe)
```

Encode a single AVFrame (typically from a decoder or resampler). 
#### Parameters
* `iframe` The source audio frame with all fields set. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#flush-2}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush remaining packets to be encoded. This method should be called once before stream closure.

{#audiopacket}

## AudioPacket

```cpp
#include <icy/av/packet.h>
```

> **Inherits:** [`MediaPacket`](#mediapacket)
> **Subclassed by:** [`PlanarAudioPacket`](#planaraudiopacket)

Audio packet for interleaved formats.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `size_t` | [`numSamples`](#numsamples)  | Number of audio samples per channel. |
| `AVPacket *` | [`avpacket`](#avpacket)  | Non-owning pointer to the encoded AVPacket from FFmpeg. Set by [AudioEncoder](#audioencoder), read by [MultiplexEncoder](#multiplexencoder). |

---

{#numsamples}

#### numSamples

```cpp
size_t numSamples
```

Number of audio samples per channel.

---

{#avpacket}

#### avpacket

```cpp
AVPacket * avpacket = nullptr
```

Non-owning pointer to the encoded AVPacket from FFmpeg. Set by [AudioEncoder](#audioencoder), read by [MultiplexEncoder](#multiplexencoder).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioPacket`](#audiopacket-1) `inline` | Construct an audio packet with an interleaved sample buffer. |
| `std::unique_ptr< IPacket >` | [`clone`](#clone) `virtual` `const` `inline` | #### Returns |
| `uint8_t *` | [`samples`](#samples) `virtual` `const` `inline` | #### Returns |
| `const char *` | [`className`](#classname) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |

---

{#audiopacket-1}

#### AudioPacket

`inline`

```cpp
inline AudioPacket(uint8_t * data, size_t size, size_t numSamples, int64_t time)
```

Construct an audio packet with an interleaved sample buffer. 
#### Parameters
* `data` Pointer to the interleaved sample buffer (may be null). 

* `size` Size of the buffer in bytes. 

* `numSamples` Number of samples per channel. 

* `time` Presentation timestamp in microseconds.

---

{#clone}

#### clone

`virtual` `const` `inline`

```cpp
virtual inline std::unique_ptr< IPacket > clone() const
```

#### Returns
A heap-allocated copy of this packet.

---

{#samples}

#### samples

`virtual` `const` `inline`

```cpp
virtual inline uint8_t * samples() const
```

#### Returns
A pointer to the raw interleaved sample buffer.

---

{#classname}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

{#audioresampler}

## AudioResampler

```cpp
#include <icy/av/audioresampler.h>
```

Converts audio samples between different formats, sample rates, and channel layouts.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `SwrContext *` | [`ctx`](#ctx-1)  | the conversion context |
| `AudioCodec` | [`iparams`](#iparams-1)  | input audio parameters |
| `AudioCodec` | [`oparams`](#oparams-1)  | output audio parameters |
| `uint8_t **` | [`outSamples`](#outsamples)  | the output samples buffer |
| `int` | [`outNumSamples`](#outnumsamples)  | the number of samples currently in the output buffer |
| `int` | [`outBufferSize`](#outbuffersize)  | the number of bytes currently in the buffer |
| `int` | [`maxNumSamples`](#maxnumsamples)  | the maximum number of samples that can be stored in |
| `enum AVSampleFormat` | [`inSampleFmt`](#insamplefmt)  | input sample format |
| `enum AVSampleFormat` | [`outSampleFmt`](#outsamplefmt)  | output sample format |

---

{#ctx-1}

#### ctx

```cpp
SwrContext * ctx
```

the conversion context

---

{#iparams-1}

#### iparams

```cpp
AudioCodec iparams
```

input audio parameters

---

{#oparams-1}

#### oparams

```cpp
AudioCodec oparams
```

output audio parameters

---

{#outsamples}

#### outSamples

```cpp
uint8_t ** outSamples
```

the output samples buffer

---

{#outnumsamples}

#### outNumSamples

```cpp
int outNumSamples
```

the number of samples currently in the output buffer

---

{#outbuffersize}

#### outBufferSize

```cpp
int outBufferSize
```

the number of bytes currently in the buffer

---

{#maxnumsamples}

#### maxNumSamples

```cpp
int maxNumSamples
```

the maximum number of samples that can be stored in

---

{#insamplefmt}

#### inSampleFmt

```cpp
enum AVSampleFormat inSampleFmt
```

input sample format

---

{#outsamplefmt}

#### outSampleFmt

```cpp
enum AVSampleFormat outSampleFmt
```

output sample format

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AudioResampler`](#audioresampler-1)  |  |
|  | [`AudioResampler`](#audioresampler-2)  | Deleted constructor. |
|  | [`AudioResampler`](#audioresampler-3)  | Deleted constructor. |
| `void` | [`open`](#open-1)  | Initialise the libswresample context using iparams and oparams. Throws std::runtime_error if the context is already open or if required parameters (channels, sample rate, format) are missing. |
| `void` | [`close`](#close-4)  | Free the libswresample context and release the output sample buffer. |
| `int` | [`resample`](#resample)  | Convert the input samples to the output format. NOTE: Input buffers must be contiguous, therefore only interleaved input formats are accepted at this point. |

---

{#audioresampler-1}

#### AudioResampler

```cpp
AudioResampler(const AudioCodec & iparams, const AudioCodec & oparams)
```

---

{#audioresampler-2}

#### AudioResampler

```cpp
AudioResampler(const AudioResampler &) = delete
```

Deleted constructor.

---

{#audioresampler-3}

#### AudioResampler

```cpp
AudioResampler(AudioResampler &&) = delete
```

Deleted constructor.

---

{#open-1}

#### open

```cpp
void open()
```

Initialise the libswresample context using iparams and oparams. Throws std::runtime_error if the context is already open or if required parameters (channels, sample rate, format) are missing.

---

{#close-4}

#### close

```cpp
void close()
```

Free the libswresample context and release the output sample buffer.

---

{#resample}

#### resample

```cpp
int resample(uint8_t ** inSamples, int inNumSamples)
```

Convert the input samples to the output format. NOTE: Input buffers must be contiguous, therefore only interleaved input formats are accepted at this point.

Converted samples are accessible via the `outSamples` class member.

#### Parameters
* `inSamples` Pointer to the input sample buffer array. 

* `inNumSamples` The number of input samples per channel. 

#### Returns
The number of converted samples, or zero if samples were internally buffered.

{#codec-1}

## Codec

```cpp
#include <icy/av/codec.h>
```

> **Subclassed by:** [`AudioCodec`](#audiocodec), [`VideoCodec`](#videocodec)

[Codec](#codec-1) for encoding/decoding media.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`name`](#name)  | The display name for this codec. |
| `std::string` | [`encoder`](#encoder)  | The encoder name for FFmpeg. |
| `int` | [`sampleRate`](#samplerate)  | The sampling rate or RTP clock rate. |
| `int` | [`bitRate`](#bitrate)  | The bit rate to encode at. |
| `int` | [`quality`](#quality)  | Optional quality value, variable range depending on codec. |
| `int` | [`compliance`](#compliance)  | FFmpeg strict_std_compliance level (default: FF_COMPLIANCE_EXPERIMENTAL). |
| `bool` | [`enabled`](#enabled)  | Weather or not the codec is available for use. |
| `std::map< std::string, std::string >` | [`options`](#options)  | Arbitrary encoder options passed to FFmpeg via av_opt_set(). Keys are FFmpeg option names (e.g. "preset", "tune", "profile"). Applied after codec-specific defaults, so they override everything. |

---

{#name}

#### name

```cpp
std::string name
```

The display name for this codec.

---

{#encoder}

#### encoder

```cpp
std::string encoder
```

The encoder name for FFmpeg.

---

{#samplerate}

#### sampleRate

```cpp
int sampleRate
```

The sampling rate or RTP clock rate.

---

{#bitrate}

#### bitRate

```cpp
int bitRate
```

The bit rate to encode at.

---

{#quality}

#### quality

```cpp
int quality
```

Optional quality value, variable range depending on codec.

---

{#compliance}

#### compliance

```cpp
int compliance
```

FFmpeg strict_std_compliance level (default: FF_COMPLIANCE_EXPERIMENTAL).

---

{#enabled}

#### enabled

```cpp
bool enabled
```

Weather or not the codec is available for use.

---

{#options}

#### options

```cpp
std::map< std::string, std::string > options
```

Arbitrary encoder options passed to FFmpeg via av_opt_set(). Keys are FFmpeg option names (e.g. "preset", "tune", "profile"). Applied after codec-specific defaults, so they override everything.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Codec`](#codec-2)  | Construct a disabled codec with zeroed parameters. |
|  | [`Codec`](#codec-3)  | Construct a codec with a display name, sample rate, bit rate, and enabled flag. |
|  | [`Codec`](#codec-4)  | Construct a codec with an explicit FFmpeg encoder name. |
|  | [`~Codec`](#codec-5) `virtual` | Codec(const Codec& r);. |
| `std::string` | [`toString`](#tostring-1) `virtual` `const` | Return a compact string representation of this codec. |
| `bool` | [`specified`](#specified) `const` | Returns true when this codec explicitly names either an RTP/media codec or an FFmpeg encoder and is enabled for use. |
| `void` | [`print`](#print-1) `virtual` | Print a multi-line human-readable description to the given stream. |

---

{#codec-2}

#### Codec

```cpp
Codec()
```

Construct a disabled codec with zeroed parameters.

---

{#codec-3}

#### Codec

```cpp
Codec(const std::string & name, int sampleRate, int bitRate, bool enabled)
```

Construct a codec with a display name, sample rate, bit rate, and enabled flag. 
#### Parameters
* `name` The codec display name. 

* `sampleRate` The sampling rate or RTP clock rate in Hz. 

* `bitRate` The target bit rate in bits per second. 

* `enabled` Whether the codec is available for use.

---

{#codec-4}

#### Codec

```cpp
Codec(const std::string & name, const std::string & encoder, int sampleRate, int bitRate, bool enabled)
```

Construct a codec with an explicit FFmpeg encoder name. 
#### Parameters
* `name` The codec display name. 

* `encoder` The FFmpeg encoder name (e.g. "libx264"). 

* `sampleRate` The sampling rate or RTP clock rate in Hz. 

* `bitRate` The target bit rate in bits per second. 

* `enabled` Whether the codec is available for use.

---

{#codec-5}

#### ~Codec

`virtual`

```cpp
virtual ~Codec() noexcept
```

Codec(const Codec& r);.

---

{#tostring-1}

#### toString

`virtual` `const`

```cpp
virtual std::string toString() const
```

Return a compact string representation of this codec. 
#### Returns
A string in the form "Codec[name:encoder:sampleRate:enabled]".

---

{#specified}

#### specified

`const`

```cpp
bool specified() const
```

Returns true when this codec explicitly names either an RTP/media codec or an FFmpeg encoder and is enabled for use.

---

{#print-1}

#### print

`virtual`

```cpp
virtual void print(std::ostream & ost)
```

Print a multi-line human-readable description to the given stream. 
#### Parameters
* `ost` The output stream to write to.

{#deleter-1}

## Deleter

```cpp
#include <icy/av/ffmpeg.h>
```

RAII helpers for owning FFmpeg allocation types.

[Deleter](#deleter-1) adaptor for functions like av_free that take a pointer.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator) `const` `inline` |  |

---

{#operator}

#### operator()

`const` `inline`

```cpp
inline void operator()(T * p) const
```

{#deleterp}

## Deleterp

```cpp
#include <icy/av/ffmpeg.h>
```

[Deleter](#deleter-1) adaptor for functions like av_freep that take a pointer to a pointer.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-1) `const` `inline` |  |

---

{#operator-1}

#### operator()

`const` `inline`

```cpp
inline void operator()(T * p) const
```

{#device}

## Device

```cpp
#include <icy/av/devicemanager.h>
```

Represents a system audio, video or render device.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Type` | [`type`](#type)  |  |
| `std::string` | [`id`](#id)  |  |
| `std::string` | [`name`](#name-1)  |  |
| `bool` | [`isDefault`](#isdefault)  |  |
| `bool` | [`isConnected`](#isconnected)  |  |
| `bool` | [`isInUse`](#isinuse)  |  |
| `std::vector< VideoCapability >` | [`videoCapabilities`](#videocapabilities)  |  |
| `std::vector< AudioCapability >` | [`audioCapabilities`](#audiocapabilities)  |  |

---

{#type}

#### type

```cpp
Type type {}
```

---

{#id}

#### id

```cpp
std::string id
```

---

{#name-1}

#### name

```cpp
std::string name
```

---

{#isdefault}

#### isDefault

```cpp
bool isDefault {false}
```

---

{#isconnected}

#### isConnected

```cpp
bool isConnected {true}
```

---

{#isinuse}

#### isInUse

```cpp
bool isInUse {false}
```

---

{#videocapabilities}

#### videoCapabilities

```cpp
std::vector< VideoCapability > videoCapabilities
```

---

{#audiocapabilities}

#### audioCapabilities

```cpp
std::vector< AudioCapability > audioCapabilities
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Device`](#device-1)  | Construct a device with Unknown type and empty fields. |
|  | [`Device`](#device-2)  | Construct a device with explicit fields. |
| `void` | [`print`](#print-2) `const` | Print device details (type, id, name, capabilities) to the given stream. |
| `bool` | [`operator==`](#operator-2) `const` `inline` | Equality based on type, id, and name. |
| `VideoCapability` | [`bestVideoCapability`](#bestvideocapability) `const` `inline` | Find the video capability closest to the requested parameters. |
| `AudioCapability` | [`bestAudioCapability`](#bestaudiocapability) `const` `inline` | Find the audio capability closest to the requested parameters. |

---

{#device-1}

#### Device

```cpp
Device()
```

Construct a device with Unknown type and empty fields.

---

{#device-2}

#### Device

```cpp
Device(Type type, const std::string & id, const std::string & name, bool isDefault)
```

Construct a device with explicit fields. 
#### Parameters
* `type` The device type (VideoInput, AudioInput, etc.). 

* `id` The platform-specific device identifier. 

* `name` The human-readable device name. 

* `isDefault` True if this is the system default device of its type.

---

{#print-2}

#### print

`const`

```cpp
void print(std::ostream & os) const
```

Print device details (type, id, name, capabilities) to the given stream. 
#### Parameters
* `os` The output stream to write to.

---

{#operator-2}

#### operator==

`const` `inline`

```cpp
inline bool operator==(const Device & that) const
```

Equality based on type, id, and name.

---

{#bestvideocapability}

#### bestVideoCapability

`const` `inline`

```cpp
inline VideoCapability bestVideoCapability(int width, int height, double fps) const
```

Find the video capability closest to the requested parameters.

Scoring: resolution match weighted 70%, fps match weighted 30%. Both dimensions are normalised to [0,1] so the weights are meaningful. Prefers capabilities that are >= the requested resolution (upscaling is worse than slight downscaling).

Returns a default capability with the requested values if none are available (allows callers to skip the empty check).

---

{#bestaudiocapability}

#### bestAudioCapability

`const` `inline`

```cpp
inline AudioCapability bestAudioCapability(int sampleRate, int channels) const
```

Find the audio capability closest to the requested parameters.

Scoring: sample rate match weighted 70%, channel count weighted 30%. Both dimensions are normalised to [0,1].

Returns a default capability with the requested values if none are available.

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-1)  |  |

---

{#type-1}

#### Type

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `Unknown` |  |
| `VideoInput` |  |
| `VideoOutput` |  |
| `AudioInput` |  |
| `AudioOutput` |  |

{#audiocapability}

## AudioCapability

```cpp
#include <icy/av/devicemanager.h>
```

An audio format supported by the device.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`sampleRate`](#samplerate-1)  |  |
| `int` | [`channels`](#channels-1)  |  |
| `std::string` | [`sampleFormat`](#sampleformat)  |  |

---

{#samplerate-1}

#### sampleRate

```cpp
int sampleRate {0}
```

---

{#channels-1}

#### channels

```cpp
int channels {0}
```

---

{#sampleformat}

#### sampleFormat

```cpp
std::string sampleFormat
```

{#videocapability}

## VideoCapability

```cpp
#include <icy/av/devicemanager.h>
```

A video format supported by the device.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`width`](#width)  |  |
| `int` | [`height`](#height)  |  |
| `double` | [`minFps`](#minfps)  |  |
| `double` | [`maxFps`](#maxfps)  |  |
| `std::string` | [`pixelFormat`](#pixelformat)  |  |

---

{#width}

#### width

```cpp
int width {0}
```

---

{#height}

#### height

```cpp
int height {0}
```

---

{#minfps}

#### minFps

```cpp
double minFps {0}
```

---

{#maxfps}

#### maxFps

```cpp
double maxFps {0}
```

---

{#pixelformat}

#### pixelFormat

```cpp
std::string pixelFormat
```

{#encoderoptions}

## EncoderOptions

```cpp
#include <icy/av/iencoder.h>
```

[Configuration](base.md#configuration) options for audio and video encoders.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Format` | [`iformat`](#iformat)  | input media format. |
| `Format` | [`oformat`](#oformat)  | output media format. |
| `std::string` | [`ifile`](#ifile)  | input file path. |
| `std::string` | [`ofile`](#ofile)  | output file path. |
| `long` | [`duration`](#duration)  | duration of time to record in nanoseconds. |

---

{#iformat}

#### iformat

```cpp
Format iformat
```

input media format.

---

{#oformat}

#### oformat

```cpp
Format oformat
```

output media format.

---

{#ifile}

#### ifile

```cpp
std::string ifile
```

input file path.

---

{#ofile}

#### ofile

```cpp
std::string ofile
```

output file path.

---

{#duration}

#### duration

```cpp
long duration
```

duration of time to record in nanoseconds.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`EncoderOptions`](#encoderoptions-1) `inline` |  |

---

{#encoderoptions-1}

#### EncoderOptions

`inline`

```cpp
inline EncoderOptions(const Format & iformat, const Format & oformat, const std::string & ifile, const std::string & ofile, long duration)
```

{#encoderstate}

## EncoderState

```cpp
#include <icy/av/iencoder.h>
```

> **Inherits:** [`State`](base.md#state)

[State](base.md#state) machine states for the encoder pipeline.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`str`](#str) `const` `inline` |  |

---

{#str}

#### str

`const` `inline`

```cpp
inline std::string str(unsigned int id) const
```

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-3)  |  |

---

{#type-3}

#### Type

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` |  |
| `Ready` |  |
| `Encoding` |  |
| `Stopped` |  |
| `Error` |  |

{#format-1}

## Format

```cpp
#include <icy/av/format.h>
```

Defines a media container format which is available through the [FormatRegistry](#formatregistry) for encoding or decoding.

A format bundles the preferred default audio and video codec settings for a named container such as MP4, MKV, or WAV.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`name`](#name-3)  | Base [Format](#format-1) Variables. |
| `std::string` | [`id`](#id-1)  | The short name of this format. |
| `VideoCodec` | [`video`](#video)  | The video codec. |
| `AudioCodec` | [`audio`](#audio)  | The audio codec. |
| `int` | [`priority`](#priority)  | The priority this format will be displayed on the list. |

---

{#name-3}

#### name

```cpp
std::string name
```

Base [Format](#format-1) Variables.

The display name of this format.

---

{#id-1}

#### id

```cpp
std::string id
```

The short name of this format.

---

{#video}

#### video

```cpp
VideoCodec video
```

The video codec.

---

{#audio}

#### audio

```cpp
AudioCodec audio
```

The audio codec.

---

{#priority}

#### priority

```cpp
int priority
```

The priority this format will be displayed on the list.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Format`](#format-2)  | Ctors/Dtors. |
|  | [`Format`](#format-3)  | Construct a multiplex (audio + video) format. |
|  | [`Format`](#format-4)  | Construct a video-only format. |
|  | [`Format`](#format-5)  | Construct an audio-only format. |
|  | [`Format`](#format-6)  |  |
| `Type` | [`type`](#type-4) `const` | Return the media type (None, Video, Audio, or Multiplex) derived from which codecs are enabled. |
| `std::string` | [`toString`](#tostring-2) `virtual` `const` | #### Returns |
| `void` | [`print`](#print-4) `virtual` | Print a multi-line human-readable description to the given stream. |

---

{#format-2}

#### Format

```cpp
Format()
```

Ctors/Dtors.

Construct an empty format with no codecs enabled.

---

{#format-3}

#### Format

```cpp
Format(const std::string & name, const std::string & id, const VideoCodec & video, const AudioCodec & audio, int priority)
```

Construct a multiplex (audio + video) format. 
#### Parameters
* `name` The display name of the format. 

* `id` The short FFmpeg muxer ID (e.g. "mp4"). 

* `video` The video codec parameters. 

* `audio` The audio codec parameters. 

* `priority` Sort priority; higher values appear first.

---

{#format-4}

#### Format

```cpp
Format(const std::string & name, const std::string & id, const VideoCodec & video, int priority)
```

Construct a video-only format. 
#### Parameters
* `name` The display name of the format. 

* `id` The short FFmpeg muxer ID. 

* `video` The video codec parameters. 

* `priority` Sort priority; higher values appear first.

---

{#format-5}

#### Format

```cpp
Format(const std::string & name, const std::string & id, const AudioCodec & audio, int priority)
```

Construct an audio-only format. 
#### Parameters
* `name` The display name of the format. 

* `id` The short FFmpeg muxer ID. 

* `audio` The audio codec parameters. 

* `priority` Sort priority; higher values appear first.

---

{#format-6}

#### Format

```cpp
Format(const Format & r)
```

---

{#type-4}

#### type

`const`

```cpp
Type type() const
```

Return the media type (None, Video, Audio, or Multiplex) derived from which codecs are enabled.

---

{#tostring-2}

#### toString

`virtual` `const`

```cpp
virtual std::string toString() const
```

#### Returns
A compact string representation of the format and its enabled codecs.

---

{#print-4}

#### print

`virtual`

```cpp
virtual void print(std::ostream & ost)
```

Print a multi-line human-readable description to the given stream. 
#### Parameters
* `ost` The output stream to write to.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`preferable`](#preferable) `static` `inline` | Comparator returning true if `first` has higher priority than `second`. |

---

{#preferable}

#### preferable

`static` `inline`

```cpp
static inline bool preferable(const Format & first, const Format & second)
```

Comparator returning true if `first` has higher priority than `second`. 
#### Parameters
* `first` The format to compare. 

* `second` The format to compare against. 

#### Returns
True if first.priority > second.priority.

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-5)  |  |

---

{#type-5}

#### Type

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` |  |
| `Video` | video only |
| `Audio` | audio only |
| `Multiplex` | both video & audio |

{#mediapacket}

## MediaPacket

```cpp
#include <icy/av/packet.h>
```

> **Inherits:** [`RawPacket`](base.md#rawpacket)
> **Subclassed by:** [`AudioPacket`](#audiopacket), [`VideoPacket`](#videopacket)

Timestamped media packet carrying raw audio or video data.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int64_t` | [`time`](#time-1)  | Presentation timestamp in microseconds. |

---

{#time-1}

#### time

```cpp
int64_t time
```

Presentation timestamp in microseconds.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MediaPacket`](#mediapacket-1) `inline` | Construct with a non-owning or owning mutable buffer. |
|  | [`MediaPacket`](#mediapacket-2) `inline` | Construct with const data (copied, owning). |
|  | [`MediaPacket`](#mediapacket-3) `inline` | Copy constructor. |
| `std::unique_ptr< IPacket >` | [`clone`](#clone-1) `virtual` `const` `inline` | #### Returns |
| `const char *` | [`className`](#classname-1) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |

---

{#mediapacket-1}

#### MediaPacket

`inline`

```cpp
inline MediaPacket(uint8_t * data, size_t size, int64_t time)
```

Construct with a non-owning or owning mutable buffer. 
#### Parameters
* `data` Pointer to the raw data buffer (may be null). 

* `size` Size of the buffer in bytes. 

* `time` Presentation timestamp in microseconds.

---

{#mediapacket-2}

#### MediaPacket

`inline`

```cpp
inline MediaPacket(const uint8_t * data, size_t size, int64_t time)
```

Construct with const data (copied, owning). 
#### Parameters
* `data` Pointer to the const raw data buffer (data is copied). 

* `size` Size of the buffer in bytes. 

* `time` Presentation timestamp in microseconds.

---

{#mediapacket-3}

#### MediaPacket

`inline`

```cpp
inline MediaPacket(const MediaPacket & r)
```

Copy constructor.

---

{#clone-1}

#### clone

`virtual` `const` `inline`

```cpp
virtual inline std::unique_ptr< IPacket > clone() const
```

#### Returns
A heap-allocated copy of this packet.

---

{#classname-1}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

{#planaraudiopacket}

## PlanarAudioPacket

```cpp
#include <icy/av/packet.h>
```

> **Inherits:** [`AudioPacket`](#audiopacket)

Audio packet for planar formats.

#### Parameters
* `data` Array of per-plane sample buffers (one per channel). 

* `channels` The number of audio channels. 

* `numSamples` The number of samples per channel. 

* `sampleFmt` The sample format name (e.g. "fltp"). 

* `time` The timestamp in microseconds.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t *` | [`buffer`](#buffer)  |  |
| `int` | [`linesize`](#linesize)  |  |
| `int` | [`channels`](#channels-2)  |  |
| `std::string` | [`sampleFmt`](#samplefmt-1)  |  |
| `bool` | [`owns_buffer`](#owns_buffer)  |  |

---

{#buffer}

#### buffer

```cpp
uint8_t * buffer = {nullptr}
```

---

{#linesize}

#### linesize

```cpp
int linesize = 0
```

---

{#channels-2}

#### channels

```cpp
int channels = 0
```

---

{#samplefmt-1}

#### sampleFmt

```cpp
std::string sampleFmt
```

---

{#owns_buffer}

#### owns_buffer

```cpp
bool owns_buffer = false
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`PlanarAudioPacket`](#planaraudiopacket-1)  | Construct a planar audio packet, copying the plane pointers (not the sample data). |
|  | [`PlanarAudioPacket`](#planaraudiopacket-2)  | Copy constructor. Performs a deep copy of the owned buffer if owns_buffer is set. |
| `std::unique_ptr< IPacket >` | [`clone`](#clone-2) `virtual` `const` `inline` | #### Returns |
| `const char *` | [`className`](#classname-2) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |

---

{#planaraudiopacket-1}

#### PlanarAudioPacket

```cpp
PlanarAudioPacket(uint8_t * data, int channels, size_t numSamples, const std::string & sampleFmt, int64_t time)
```

Construct a planar audio packet, copying the plane pointers (not the sample data). 
#### Parameters
* `data` Array of up to 4 per-channel sample buffers. 

* `channels` Number of audio channels. 

* `numSamples` Number of samples per channel. 

* `sampleFmt` The sample format string (e.g. "fltp"). 

* `time` Presentation timestamp in microseconds.

---

{#planaraudiopacket-2}

#### PlanarAudioPacket

```cpp
PlanarAudioPacket(const PlanarAudioPacket & r)
```

Copy constructor. Performs a deep copy of the owned buffer if owns_buffer is set.

---

{#clone-2}

#### clone

`virtual` `const` `inline`

```cpp
virtual inline std::unique_ptr< IPacket > clone() const
```

#### Returns
A heap-allocated deep copy of this packet.

---

{#classname-2}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

{#planarvideopacket}

## PlanarVideoPacket

```cpp
#include <icy/av/packet.h>
```

> **Inherits:** [`VideoPacket`](#videopacket)

Video packet for planar formats.

#### Parameters
* `data` Array of per-plane data pointers (up to 4 planes). 

* `linesize` Array of per-plane byte strides. 

* `pixelFmt` The pixel format name (e.g. "yuv420p"). 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `time` The timestamp in microseconds.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t *` | [`buffer`](#buffer-1)  |  |
| `int` | [`linesize`](#linesize-1)  |  |
| `std::string` | [`pixelFmt`](#pixelfmt)  |  |
| `bool` | [`owns_buffer`](#owns_buffer-1)  |  |
| `AVFrame *` | [`avframe`](#avframe)  | Non-owning pointer to the decoded AVFrame from FFmpeg. Set by [VideoDecoder](#videodecoder). |

---

{#buffer-1}

#### buffer

```cpp
uint8_t * buffer = {nullptr}
```

---

{#linesize-1}

#### linesize

```cpp
int linesize = {0}
```

---

{#pixelfmt}

#### pixelFmt

```cpp
std::string pixelFmt
```

---

{#owns_buffer-1}

#### owns_buffer

```cpp
bool owns_buffer = false
```

---

{#avframe}

#### avframe

```cpp
AVFrame * avframe = nullptr
```

Non-owning pointer to the decoded AVFrame from FFmpeg. Set by [VideoDecoder](#videodecoder).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`PlanarVideoPacket`](#planarvideopacket-1)  | Construct a planar video packet, copying the plane pointers (not the pixel data). |
|  | [`PlanarVideoPacket`](#planarvideopacket-2)  | Copy constructor. Performs a deep copy of the owned buffer if owns_buffer is set. |
| `std::unique_ptr< IPacket >` | [`clone`](#clone-3) `virtual` `const` `inline` | #### Returns |
| `const char *` | [`className`](#classname-3) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |

---

{#planarvideopacket-1}

#### PlanarVideoPacket

```cpp
PlanarVideoPacket(uint8_t * data, const int linesize, const std::string & pixelFmt, int width, int height, int64_t time)
```

Construct a planar video packet, copying the plane pointers (not the pixel data). 
#### Parameters
* `data` Array of up to 4 per-plane data pointers. 

* `linesize` Array of per-plane byte strides. 

* `pixelFmt` The pixel format string (e.g. "yuv420p"). 

* `width` Frame width in pixels. 

* `height` Frame height in pixels. 

* `time` Presentation timestamp in microseconds.

---

{#planarvideopacket-2}

#### PlanarVideoPacket

```cpp
PlanarVideoPacket(const PlanarVideoPacket & r)
```

Copy constructor. Performs a deep copy of the owned buffer if owns_buffer is set.

---

{#clone-3}

#### clone

`virtual` `const` `inline`

```cpp
virtual inline std::unique_ptr< IPacket > clone() const
```

#### Returns
A heap-allocated deep copy of this packet.

---

{#classname-3}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

{#videocodec}

## VideoCodec

```cpp
#include <icy/av/codec.h>
```

> **Inherits:** [`Codec`](#codec-1)

Video codec parameters including resolution, frame rate, and pixel format.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`width`](#width-1)  |  |
| `int` | [`height`](#height-1)  |  |
| `double` | [`fps`](#fps-1)  |  |
| `std::string` | [`pixelFmt`](#pixelfmt-1)  |  |

---

{#width-1}

#### width

```cpp
int width
```

---

{#height-1}

#### height

```cpp
int height
```

---

{#fps-1}

#### fps

```cpp
double fps
```

---

{#pixelfmt-1}

#### pixelFmt

```cpp
std::string pixelFmt
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoCodec`](#videocodec-1)  | Construct a disabled video codec with zeroed parameters. |
|  | [`VideoCodec`](#videocodec-2)  | Construct an anonymous video codec from raw parameters. |
|  | [`VideoCodec`](#videocodec-3)  | Construct a named video codec. |
|  | [`VideoCodec`](#videocodec-4)  | Construct a named video codec with an explicit FFmpeg encoder name. |
|  | [`VideoCodec`](#videocodec-5)  |  |
| `std::string` | [`toString`](#tostring-3) `virtual` `const` | #### Returns |
| `void` | [`print`](#print-5) `virtual` | Print a multi-line human-readable description to the given stream. |

---

{#videocodec-1}

#### VideoCodec

```cpp
VideoCodec()
```

Construct a disabled video codec with zeroed parameters.

---

{#videocodec-2}

#### VideoCodec

```cpp
VideoCodec(int width, int height, double fps, const std::string & pixelFmt, int bitRate, int sampleRate)
```

Construct an anonymous video codec from raw parameters. 
#### Parameters
* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `fps` The target frame rate in frames per second. 

* `pixelFmt` The pixel format string (e.g. "yuv420p"). 

* `bitRate` The target bit rate in bits per second. 

* `sampleRate` The RTP clock rate in Hz.

---

{#videocodec-3}

#### VideoCodec

```cpp
VideoCodec(const std::string & name, int width, int height, double fps, int bitRate, int sampleRate, const std::string & pixelFmt)
```

Construct a named video codec. 
#### Parameters
* `name` The codec display name. 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `fps` The target frame rate in frames per second. 

* `bitRate` The target bit rate in bits per second. 

* `sampleRate` The RTP clock rate in Hz. 

* `pixelFmt` The pixel format string.

---

{#videocodec-4}

#### VideoCodec

```cpp
VideoCodec(const std::string & name, const std::string & encoder, int width, int height, double fps, int bitRate, int sampleRate, const std::string & pixelFmt)
```

Construct a named video codec with an explicit FFmpeg encoder name. 
#### Parameters
* `name` The codec display name. 

* `encoder` The FFmpeg encoder name (e.g. "libx264"). 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `fps` The target frame rate in frames per second. 

* `bitRate` The target bit rate in bits per second. 

* `sampleRate` The RTP clock rate in Hz. 

* `pixelFmt` The pixel format string.

---

{#videocodec-5}

#### VideoCodec

```cpp
VideoCodec(const VideoCodec & r)
```

---

{#tostring-3}

#### toString

`virtual` `const`

```cpp
virtual std::string toString() const
```

#### Returns
A string in the form "VideoCodec[name:encoder:width:height:fps:pixelFmt:enabled]".

---

{#print-5}

#### print

`virtual`

```cpp
virtual void print(std::ostream & ost)
```

Print a multi-line human-readable description to the given stream. 
#### Parameters
* `ost` The output stream to write to.

{#videocontext}

## VideoContext

```cpp
#include <icy/av/videocontext.h>
```

> **Subclassed by:** [`VideoDecoder`](#videodecoder), [`VideoEncoder`](#videoencoder)

Base video context from which all video encoders and decoders derive.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-4)  |  |
| `VideoCodec` | [`iparams`](#iparams-2)  | input parameters |
| `VideoCodec` | [`oparams`](#oparams-2)  | output parameters |
| `AVStream *` | [`stream`](#stream-2)  | encoder or decoder stream |
| `AVCodecContext *` | [`ctx`](#ctx-2)  | encoder or decoder context |
| `const AVCodec *` | [`codec`](#codec-6)  | encoder or decoder codec |
| `AVFrame *` | [`frame`](#frame-1)  | encoder or decoder frame |
| `std::unique_ptr< VideoConverter >` | [`conv`](#conv)  | video conversion context |
| `int64_t` | [`time`](#time-2)  | stream time in codec time base |
| `int64_t` | [`pts`](#pts-1)  | last packet pts value |
| `double` | [`seconds`](#seconds-1)  | video time in seconds |
| `std::string` | [`error`](#error-3)  | error message |

---

{#emitter-4}

#### emitter

```cpp
PacketSignal emitter
```

---

{#iparams-2}

#### iparams

```cpp
VideoCodec iparams
```

input parameters

---

{#oparams-2}

#### oparams

```cpp
VideoCodec oparams
```

output parameters

---

{#stream-2}

#### stream

```cpp
AVStream * stream
```

encoder or decoder stream

---

{#ctx-2}

#### ctx

```cpp
AVCodecContext * ctx
```

encoder or decoder context

---

{#codec-6}

#### codec

```cpp
const AVCodec * codec
```

encoder or decoder codec

---

{#frame-1}

#### frame

```cpp
AVFrame * frame
```

encoder or decoder frame

---

{#conv}

#### conv

```cpp
std::unique_ptr< VideoConverter > conv
```

video conversion context

---

{#time-2}

#### time

```cpp
int64_t time
```

stream time in codec time base

---

{#pts-1}

#### pts

```cpp
int64_t pts
```

last packet pts value

---

{#seconds-1}

#### seconds

```cpp
double seconds
```

video time in seconds

---

{#error-3}

#### error

```cpp
std::string error
```

error message

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoContext`](#videocontext-1)  |  |
|  | [`VideoContext`](#videocontext-2)  | Deleted constructor. |
|  | [`VideoContext`](#videocontext-3)  | Deleted constructor. |
| `void` | [`create`](#create-3) `virtual` | Initialise the AVCodecContext with codec-specific defaults. Overridden by [VideoEncoder](#videoencoder) and [VideoDecoder](#videodecoder). |
| `void` | [`open`](#open-2) `virtual` | Open the codec and create the pixel format conversion context if required. Throws std::runtime_error if the codec context has not been created. |
| `void` | [`close`](#close-7) `virtual` | Close the codec context, free the frame, and reset timestamps. |
| `bool` | [`decode`](#decode-2) `virtual` | Decode a compressed video packet and emit the resulting frame. |
| `bool` | [`encode`](#encode-8) `virtual` | Encode a buffer of interleaved video data. |
| `bool` | [`encode`](#encode-9) `virtual` | Encode a planar video frame. |
| `bool` | [`encode`](#encode-10) `virtual` | Encode a single AVFrame. |
| `void` | [`flush`](#flush-5) `virtual` | Flush any frames buffered inside the codec and emit remaining output. |
| `AVFrame *` | [`convert`](#convert) `virtual` | Convert the video frame and return the result. |
| `bool` | [`recreateConverter`](#recreateconverter) `virtual` | Recreate the [VideoConverter](#videoconverter) if the input or output parameters have changed. Called automatically by [open()](#open-2) and [convert()](#convert). |

---

{#videocontext-1}

#### VideoContext

```cpp
VideoContext()
```

---

{#videocontext-2}

#### VideoContext

```cpp
VideoContext(const VideoContext &) = delete
```

Deleted constructor.

---

{#videocontext-3}

#### VideoContext

```cpp
VideoContext(VideoContext &&) = delete
```

Deleted constructor.

---

{#create-3}

#### create

`virtual`

```cpp
virtual void create()
```

Initialise the AVCodecContext with codec-specific defaults. Overridden by [VideoEncoder](#videoencoder) and [VideoDecoder](#videodecoder).

---

{#open-2}

#### open

`virtual`

```cpp
virtual void open()
```

Open the codec and create the pixel format conversion context if required. Throws std::runtime_error if the codec context has not been created.

---

{#close-7}

#### close

`virtual`

```cpp
virtual void close()
```

Close the codec context, free the frame, and reset timestamps.

---

{#decode-2}

#### decode

`virtual`

```cpp
virtual bool decode(AVPacket & ipacket)
```

Decode a compressed video packet and emit the resulting frame. 
#### Parameters
* `ipacket` The compressed video packet to decode. 

#### Returns
True if an output frame was decoded and emitted, false otherwise.

---

{#encode-8}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * data, int size, int64_t pts)
```

Encode a buffer of interleaved video data. 
#### Parameters
* `data` Pointer to the interleaved frame buffer. 

* `size` Size of the buffer in bytes. 

* `pts` Presentation timestamp in stream time base units. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#encode-9}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * data, int linesize, int64_t pts)
```

Encode a planar video frame. 
#### Parameters
* `data` Array of per-plane data pointers (up to 4 planes). 

* `linesize` Array of per-plane byte strides. 

* `pts` Presentation timestamp in stream time base units. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#encode-10}

#### encode

`virtual`

```cpp
virtual bool encode(AVFrame * iframe)
```

Encode a single AVFrame. 
#### Parameters
* `iframe` The source video frame; must have all fields set correctly. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#flush-5}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush any frames buffered inside the codec and emit remaining output.

---

{#convert}

#### convert

`virtual`

```cpp
virtual AVFrame * convert(AVFrame * iframe)
```

Convert the video frame and return the result.

The input frame will only be converted if it doesn't match the output format. If the frame is not converted the input frame will be returned. If the input frame format does not match the `[VideoConverter](#videoconverter)` context then the `[VideoConverter](#videoconverter)` will be recreated with the input frame params.

---

{#recreateconverter}

#### recreateConverter

`virtual`

```cpp
virtual bool recreateConverter()
```

Recreate the [VideoConverter](#videoconverter) if the input or output parameters have changed. Called automatically by [open()](#open-2) and [convert()](#convert). 
#### Returns
True if the converter was (re)created, false if it is already up to date.

{#videoconverter}

## VideoConverter

```cpp
#include <icy/av/videoconverter.h>
```

Converts video frames between pixel formats and resolutions.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `SwsContext *` | [`ctx`](#ctx-3)  | libswscale conversion context. |
| `AVFrame *` | [`oframe`](#oframe)  | Reusable output frame allocated by [create()](#create-4). |
| `VideoCodec` | [`iparams`](#iparams-3)  | Expected input video parameters. |
| `VideoCodec` | [`oparams`](#oparams-3)  | Target output video parameters. |

---

{#ctx-3}

#### ctx

```cpp
SwsContext * ctx
```

libswscale conversion context.

---

{#oframe}

#### oframe

```cpp
AVFrame * oframe
```

Reusable output frame allocated by [create()](#create-4).

---

{#iparams-3}

#### iparams

```cpp
VideoCodec iparams
```

Expected input video parameters.

---

{#oparams-3}

#### oparams

```cpp
VideoCodec oparams
```

Target output video parameters.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoConverter`](#videoconverter-1)  |  |
|  | [`VideoConverter`](#videoconverter-2)  | Deleted constructor. |
|  | [`VideoConverter`](#videoconverter-3)  | Deleted constructor. |
| `void` | [`create`](#create-4) `virtual` | Initialise the libswscale context and allocate the output frame. Uses iparams and oparams to configure the conversion pipeline. Throws std::runtime_error if already initialised or if parameters are invalid. |
| `void` | [`close`](#close-8) `virtual` | Free the libswscale context and the output frame. |
| `AVFrame *` | [`convert`](#convert-1) `virtual` | Convert `iframe` to the output pixel format and resolution. The returned frame is owned by this converter and is overwritten on the next call. |

---

{#videoconverter-1}

#### VideoConverter

```cpp
VideoConverter()
```

---

{#videoconverter-2}

#### VideoConverter

```cpp
VideoConverter(const VideoConverter &) = delete
```

Deleted constructor.

---

{#videoconverter-3}

#### VideoConverter

```cpp
VideoConverter(VideoConverter &&) = delete
```

Deleted constructor.

---

{#create-4}

#### create

`virtual`

```cpp
virtual void create()
```

Initialise the libswscale context and allocate the output frame. Uses iparams and oparams to configure the conversion pipeline. Throws std::runtime_error if already initialised or if parameters are invalid.

---

{#close-8}

#### close

`virtual`

```cpp
virtual void close()
```

Free the libswscale context and the output frame.

---

{#convert-1}

#### convert

`virtual`

```cpp
virtual AVFrame * convert(AVFrame * iframe)
```

Convert `iframe` to the output pixel format and resolution. The returned frame is owned by this converter and is overwritten on the next call. 
#### Parameters
* `iframe` The source AVFrame; must match iparams dimensions and pixel format. 

#### Returns
The converted output AVFrame.

{#videodecoder}

## VideoDecoder

```cpp
#include <icy/av/videodecoder.h>
```

> **Inherits:** [`VideoContext`](#videocontext)

Decodes compressed video packets into raw frames.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoDecoder`](#videodecoder-1)  | Construct a decoder for the given stream. The codec parameters are read from the stream's codecpar. |
| `void` | [`create`](#create-5) `virtual` | Initialise the AVCodecContext from the stream's codec parameters. |
| `void` | [`open`](#open-3) `virtual` | Open the codec and initialise any required pixel format conversion context. |
| `void` | [`close`](#close-9) `virtual` | Close and free the AVCodecContext and associated resources. |
| `bool` | [`decode`](#decode-3) `virtual` | Decode the given compressed video packet and emit the decoded frame. Input packets must use the raw AVStream time base; time base conversion to microseconds is performed internally. |
| `void` | [`flush`](#flush-6) `virtual` | Flush any frames buffered inside the decoder. Call repeatedly after the last packet until false is returned. |

---

{#videodecoder-1}

#### VideoDecoder

```cpp
VideoDecoder(AVStream * stream)
```

Construct a decoder for the given stream. The codec parameters are read from the stream's codecpar. 
#### Parameters
* `stream` The AVStream to decode; must remain valid for the lifetime of this decoder.

---

{#create-5}

#### create

`virtual`

```cpp
virtual void create()
```

Initialise the AVCodecContext from the stream's codec parameters.

---

{#open-3}

#### open

`virtual`

```cpp
virtual void open()
```

Open the codec and initialise any required pixel format conversion context.

---

{#close-9}

#### close

`virtual`

```cpp
virtual void close()
```

Close and free the AVCodecContext and associated resources.

---

{#decode-3}

#### decode

`virtual`

```cpp
virtual bool decode(AVPacket & ipacket)
```

Decode the given compressed video packet and emit the decoded frame. Input packets must use the raw AVStream time base; time base conversion to microseconds is performed internally. 
#### Parameters
* `ipacket` The compressed video packet to decode. 

#### Returns
True if an output frame was decoded and emitted, false otherwise.

---

{#flush-6}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush any frames buffered inside the decoder. Call repeatedly after the last packet until false is returned.

{#videoencoder}

## VideoEncoder

```cpp
#include <icy/av/videoencoder.h>
```

> **Inherits:** [`VideoContext`](#videocontext)
> **Subclassed by:** [`VideoPacketEncoder`](#videopacketencoder)

Encodes raw video frames into a compressed format.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `AVFormatContext *` | [`format`](#format-7)  |  |

---

{#format-7}

#### format

```cpp
AVFormatContext * format
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoEncoder`](#videoencoder-1)  | Construct an encoder, optionally tied to an existing muxer context. |
| `void` | [`create`](#create-6) `virtual` | Initialise the AVCodecContext using oparams. Adds a video stream to `format` if one was provided at construction. |
| `void` | [`close`](#close-10) `virtual` | Close and free the AVCodecContext and associated resources. |
| `bool` | [`encode`](#encode-11) `virtual` | Encode a single video frame. This method is for interleaved video formats. |
| `bool` | [`encode`](#encode-12) `virtual` | Encode a single video frame. This method is for planar video formats. |
| `bool` | [`encode`](#encode-13) `virtual` | Encode a single AVFrame (typically from a decoder or converter). |
| `void` | [`flush`](#flush-7) `virtual` | Flush remaining packets to be encoded. This method should be called once before stream closure. |

---

{#videoencoder-1}

#### VideoEncoder

```cpp
VideoEncoder(AVFormatContext * format)
```

Construct an encoder, optionally tied to an existing muxer context. 
#### Parameters
* `format` The AVFormatContext that will receive encoded packets, or nullptr for standalone use.

---

{#create-6}

#### create

`virtual`

```cpp
virtual void create()
```

Initialise the AVCodecContext using oparams. Adds a video stream to `format` if one was provided at construction.

---

{#close-10}

#### close

`virtual`

```cpp
virtual void close()
```

Close and free the AVCodecContext and associated resources.

---

{#encode-11}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * data, int size, int64_t pts)
```

Encode a single video frame. This method is for interleaved video formats.

#### Parameters
* `data` The raw video frame buffer. 

* `size` The buffer size in bytes. 

* `pts` The presentation timestamp in stream time base units.

---

{#encode-12}

#### encode

`virtual`

```cpp
virtual bool encode(uint8_t * data, int linesize, int64_t pts)
```

Encode a single video frame. This method is for planar video formats.

#### Parameters
* `data` Array of per-plane data pointers (up to 4 planes). 

* `linesize` Array of per-plane byte strides. 

* `pts` The presentation timestamp in stream time base units.

---

{#encode-13}

#### encode

`virtual`

```cpp
virtual bool encode(AVFrame * iframe)
```

Encode a single AVFrame (typically from a decoder or converter). 
#### Parameters
* `iframe` The source video frame with all fields set. 

#### Returns
True if an encoded packet was produced and emitted.

---

{#flush-7}

#### flush

`virtual`

```cpp
virtual void flush()
```

Flush remaining packets to be encoded. This method should be called once before stream closure.

{#videopacket}

## VideoPacket

```cpp
#include <icy/av/packet.h>
```

> **Inherits:** [`MediaPacket`](#mediapacket)
> **Subclassed by:** [`PlanarVideoPacket`](#planarvideopacket)

Video packet for interleaved formats.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`width`](#width-2)  | Frame width in pixels. |
| `int` | [`height`](#height-2)  | Frame height in pixels. |
| `bool` | [`iframe`](#iframe)  | True if this is an intra (keyframe) frame. |
| `AVPacket *` | [`avpacket`](#avpacket-1)  | Non-owning pointer to the encoded AVPacket from FFmpeg. Set by [VideoEncoder](#videoencoder), read by [MultiplexEncoder](#multiplexencoder). |

---

{#width-2}

#### width

```cpp
int width
```

Frame width in pixels.

---

{#height-2}

#### height

```cpp
int height
```

Frame height in pixels.

---

{#iframe}

#### iframe

```cpp
bool iframe
```

True if this is an intra (keyframe) frame.

---

{#avpacket-1}

#### avpacket

```cpp
AVPacket * avpacket = nullptr
```

Non-owning pointer to the encoded AVPacket from FFmpeg. Set by [VideoEncoder](#videoencoder), read by [MultiplexEncoder](#multiplexencoder).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`VideoPacket`](#videopacket-1) `inline` | Construct a video packet with an interleaved buffer. |
|  | [`VideoPacket`](#videopacket-2) `inline` | Copy constructor. The avpacket pointer is shallow-copied (non-owning). |
| `std::unique_ptr< IPacket >` | [`clone`](#clone-4) `virtual` `const` `inline` | #### Returns |
| `const char *` | [`className`](#classname-4) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |

---

{#videopacket-1}

#### VideoPacket

`inline`

```cpp
inline VideoPacket(uint8_t * data, size_t size, int width, int height, int64_t time)
```

Construct a video packet with an interleaved buffer. 
#### Parameters
* `data` Pointer to the raw frame buffer (may be null). 

* `size` Size of the buffer in bytes. 

* `width` Frame width in pixels. 

* `height` Frame height in pixels. 

* `time` Presentation timestamp in microseconds.

---

{#videopacket-2}

#### VideoPacket

`inline`

```cpp
inline VideoPacket(const VideoPacket & r)
```

Copy constructor. The avpacket pointer is shallow-copied (non-owning).

---

{#clone-4}

#### clone

`virtual` `const` `inline`

```cpp
virtual inline std::unique_ptr< IPacket > clone() const
```

#### Returns
A heap-allocated copy of this packet.

---

{#classname-4}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

{#v4l2}

# v4l2

Linux V4L2 device enumeration helpers.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`getDeviceList`](#getdevicelist-1)  | Enumerate video input devices using V4L2. Populates device capabilities (resolutions, frame rates, pixel formats). |

---

{#getdevicelist-1}

#### getDeviceList

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices)
```

Enumerate video input devices using V4L2. Populates device capabilities (resolutions, frame rates, pixel formats). 
#### Parameters
* `type` Must be [Device::VideoInput](#structicy_1_1av_1_1Device_1aee01e78ad670f447f295fcdcc9b65c9fa88dbbc17e909f22c94224b6a98ebae87). 

* `devices` Output vector to fill with discovered devices. 

#### Returns
True if enumeration succeeded.

{#coreaudio}

# coreaudio

Apple CoreAudio device enumeration helpers.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`getDeviceList`](#getdevicelist-2)  | Enumerate audio input and output devices using CoreAudio. Populates audio capabilities (sample rates, channels). |

---

{#getdevicelist-2}

#### getDeviceList

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices)
```

Enumerate audio input and output devices using CoreAudio. Populates audio capabilities (sample rates, channels). 
#### Parameters
* `type` The device type to enumerate (AudioInput or AudioOutput). 

* `devices` Output vector to fill with discovered devices. 

#### Returns
True if enumeration succeeded.

{#legacy}

# legacy

Legacy timing helpers kept for compatibility with older media pipelines.

### Classes

| Name | Description |
|------|-------------|
| [`FPSCounter`](#fpscounter-2) | Legacy frames-per-second counter. |

{#fpscounter-2}

## FPSCounter

```cpp
#include <icy/av/fpscounter.h>
```

Legacy frames-per-second counter.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `clock_t` | [`start`](#start-6)  |  |
| `clock_t` | [`end`](#end)  |  |
| `int64_t` | [`frames`](#frames-1)  |  |
| `double` | [`total`](#total)  |  |
| `double` | [`fps`](#fps-2)  |  |

---

{#start-6}

#### start

```cpp
clock_t start
```

---

{#end}

#### end

```cpp
clock_t end
```

---

{#frames-1}

#### frames

```cpp
int64_t frames
```

---

{#total}

#### total

```cpp
double total
```

---

{#fps-2}

#### fps

```cpp
double fps
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FPSCounter`](#fpscounter-3) `inline` |  |
| `void` | [`tick`](#tick-1) `inline` | Begin a new timing cycle (calls [endFrame()](#endframe) first if already started). |
| `void` | [`reset`](#reset-2) `inline` | Reset all counters to zero. |
| `bool` | [`started`](#started) `inline` | #### Returns |
| `void` | [`startFrame`](#startframe) `inline` | Record the frame start time. |
| `double` | [`endFrame`](#endframe) `inline` | Record the frame end time and update the cumulative FPS average. |

---

{#fpscounter-3}

#### FPSCounter

`inline`

```cpp
inline FPSCounter()
```

---

{#tick-1}

#### tick

`inline`

```cpp
inline void tick()
```

Begin a new timing cycle (calls [endFrame()](#endframe) first if already started).

---

{#reset-2}

#### reset

`inline`

```cpp
inline void reset()
```

Reset all counters to zero.

---

{#started}

#### started

`inline`

```cpp
inline bool started()
```

#### Returns
True if [startFrame()](#startframe) has been called and the timer is running.

---

{#startframe}

#### startFrame

`inline`

```cpp
inline void startFrame()
```

Record the frame start time.

---

{#endframe}

#### endFrame

`inline`

```cpp
inline double endFrame()
```

Record the frame end time and update the cumulative FPS average. 
#### Returns
The updated FPS value.

{#avfoundation}

# avfoundation

Apple AVFoundation device enumeration helpers.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`getDeviceList`](#getdevicelist-3)  | Enumerate video input devices using AVFoundation. Populates device capabilities (resolutions, frame rates, pixel formats). |

---

{#getdevicelist-3}

#### getDeviceList

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices)
```

Enumerate video input devices using AVFoundation. Populates device capabilities (resolutions, frame rates, pixel formats). 
#### Parameters
* `type` Must be [Device::VideoInput](#structicy_1_1av_1_1Device_1aee01e78ad670f447f295fcdcc9b65c9fa88dbbc17e909f22c94224b6a98ebae87). 

* `devices` Output vector to fill with discovered devices. 

#### Returns
True if enumeration succeeded.

{#mediafoundation}

# mediafoundation

Windows Media Foundation device enumeration helpers.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`getDeviceList`](#getdevicelist-4)  | Enumerate video and audio input devices using Media Foundation. Populates device capabilities (resolutions, frame rates, pixel formats). |

---

{#getdevicelist-4}

#### getDeviceList

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices)
```

Enumerate video and audio input devices using Media Foundation. Populates device capabilities (resolutions, frame rates, pixel formats). 
#### Parameters
* `type` The device type to enumerate (VideoInput or AudioInput). 

* `devices` Output vector to fill with discovered devices. 

#### Returns
True if enumeration succeeded.

{#wasapi}

# wasapi

Windows WASAPI device enumeration helpers.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`getDeviceList`](#getdevicelist-5)  | Enumerate audio input and output devices using WASAPI. Populates audio capabilities (sample rates, channels, formats). |

---

{#getdevicelist-5}

#### getDeviceList

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices)
```

Enumerate audio input and output devices using WASAPI. Populates audio capabilities (sample rates, channels, formats). 
#### Parameters
* `type` The device type to enumerate (AudioInput or AudioOutput). 

* `devices` Output vector to fill with discovered devices. 

#### Returns
True if enumeration succeeded.

