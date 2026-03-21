# av 

### Classes

| Name | Description |
|------|-------------|
| [`IDeviceManager`](#classicy_1_1av_1_1IDeviceManager) | A platform independent interface to enumerate audio and video devices on the system. |
| [`DeviceWatcher`](#classicy_1_1av_1_1DeviceWatcher) |  |
| [`DeviceManagerFactory`](#classicy_1_1av_1_1DeviceManagerFactory) |  |
| [`DeviceManager`](#classicy_1_1av_1_1DeviceManager) |  |
| [`FakeDeviceManager`](#classicy_1_1av_1_1FakeDeviceManager) |  |
| [`LinuxDeviceManager`](#classicy_1_1av_1_1LinuxDeviceManager) |  |
| [`MacDeviceManager`](#classicy_1_1av_1_1MacDeviceManager) |  |
| [`FLVMetadataInjector`](#classicy_1_1av_1_1FLVMetadataInjector) | This class implements a packetizer which appends correct stream headers and modifies the timestamp of FLV packets so Adobe's Flash Player will play our videos mid-stream. |
| [`VideoAnalyzer`](#classicy_1_1av_1_1VideoAnalyzer) | This class provides basic AV spectrum analysis on a video using the Fourier Transform algorithm. Data is outputted in CSV format. |
| [`MacDeviceWatcher`](#classicy_1_1av_1_1MacDeviceWatcher) |  |
| [`AudioCapture`](#classicy_1_1av_1_1AudioCapture) | This class implements a cross platform audio capturer. |
| [`FormatRegistry`](#classicy_1_1av_1_1FormatRegistry) |  |
| [`FPSCounter`](#classicy_1_1av_1_1FPSCounter) | FPS counter based on the simple moving average (SMA) algorithm. |
| [`FPSLimiter`](#classicy_1_1av_1_1FPSLimiter) | This class limits the throughput rate of IPackets in a [PacketStream](./doc/api-base.md#classicy_1_1PacketStream). If the throughput rate exceeds the max specified FPS then packets will be dropped. |
| [`ICapture`](#classicy_1_1av_1_1ICapture) |  |
| [`IEncoder`](#classicy_1_1av_1_1IEncoder) | This is the abstract class for all encoders. |
| [`MediaCapture`](#classicy_1_1av_1_1MediaCapture) | This class implements a cross platform audio, video, screen and video file capturer. |
| [`MultiplexEncoder`](#classicy_1_1av_1_1MultiplexEncoder) | This class implements a multiplex audio and video encoder. |
| [`MultiplexPacketEncoder`](#classicy_1_1av_1_1MultiplexPacketEncoder) | Encodes and multiplexes a realtime video stream form audio / video capture sources. FFmpeg is used for encoding. |
| [`RealtimePacketQueue`](#classicy_1_1av_1_1RealtimePacketQueue) | This class emits media packets based on their realtime pts value. |
| [`VideoCapture`](#classicy_1_1av_1_1VideoCapture) | This class implements a cross platform audio capturer. |
| [`Device`](#structicy_1_1av_1_1Device) | Represents a system audio, video or render device. |
| [`Options`](#structicy_1_1av_1_1VideoAnalyzer_1_1Options) |  |
| [`Stream`](#structicy_1_1av_1_1VideoAnalyzer_1_1Stream) |  |
| [`Packet`](#structicy_1_1av_1_1VideoAnalyzer_1_1Packet) |  |
| [`AudioBuffer`](#structicy_1_1av_1_1AudioBuffer) |  |
| [`AudioContext`](#structicy_1_1av_1_1AudioContext) |  |
| [`AudioDecoder`](#structicy_1_1av_1_1AudioDecoder) |  |
| [`AudioEncoder`](#structicy_1_1av_1_1AudioEncoder) |  |
| [`AudioResampler`](#structicy_1_1av_1_1AudioResampler) |  |
| [`Codec`](#structicy_1_1av_1_1Codec) | [Codec](#structicy_1_1av_1_1Codec) for encoding/decoding media. |
| [`AudioCodec`](#structicy_1_1av_1_1AudioCodec) |  |
| [`VideoCodec`](#structicy_1_1av_1_1VideoCodec) |  |
| [`Deleter`](#structicy_1_1av_1_1Deleter) | Utilites for RAII: |
| [`Deleterp`](#structicy_1_1av_1_1Deleterp) | [Deleter](#structicy_1_1av_1_1Deleter) adaptor for functions like av_freep that take a pointer to a pointer. |
| [`Format`](#structicy_1_1av_1_1Format) | Defines a media container format which is available through the [Format](#structicy_1_1av_1_1Format) Registry for encoding/decoding. A format defined preferred default values for each codec. |
| [`FPSCounter`](#structicy_1_1av_1_1legacy_1_1FPSCounter) |  |
| [`EncoderState`](#structicy_1_1av_1_1EncoderState) |  |
| [`EncoderOptions`](#structicy_1_1av_1_1EncoderOptions) |  |
| [`MediaPacket`](#structicy_1_1av_1_1MediaPacket) |  |
| [`VideoPacket`](#structicy_1_1av_1_1VideoPacket) | Video packet for interleaved formats. |
| [`PlanarVideoPacket`](#structicy_1_1av_1_1PlanarVideoPacket) | Video packet for planar formats. |
| [`AudioPacket`](#structicy_1_1av_1_1AudioPacket) | Audio packet for interleaved formats. |
| [`PlanarAudioPacket`](#structicy_1_1av_1_1PlanarAudioPacket) | Audio packet for planar formats. |
| [`MediaPacketTimeCompare`](#structicy_1_1av_1_1RealtimePacketQueue_1_1MediaPacketTimeCompare) |  |
| [`VideoContext`](#structicy_1_1av_1_1VideoContext) | Base video context from which all video encoders and decoders derive. |
| [`VideoConverter`](#structicy_1_1av_1_1VideoConverter) |  |
| [`VideoDecoder`](#structicy_1_1av_1_1VideoDecoder) |  |
| [`VideoEncoder`](#structicy_1_1av_1_1VideoEncoder) |  |

### Members

| Name | Description |
|------|-------------|
| [`MAX_VIDEO_PACKET_SIZE`](#group__av_1gaca35e31ea0ef7c5ce3514f4ca2c0f5b0) |  |
| [`MAX_AUDIO_PACKET_SIZE`](#group__av_1ga5022dc52e9bda86ed266bb7d4c74271b) | 1 second of 48khz 32bit audio |
| [`MAX_ENCODE_DURATION`](#group__av_1ga724e4af8d0f4d376da78d93f9b58754b) | 3 hours |
| [`INT64_C`](#group__av_1ga22369e5d04efcda4162fdb0f5759318e) |  |
| [`UINT64_C`](#group__av_1ga26a7bac63d90ef61175acb9f6fc4f2ca) |  |
| [`AV_API`](#group__av_1ga532e425a62b8fff224eba130411a911d) |  |
| [`DEFAULT_AUDIO_SAMPLE_RATE`](#group__av_1ga359d5a62ed81f83bdf547b4c358c9bcf) |  |
| [`DEFAULT_AUDIO_BIT_RATE`](#group__av_1ga43b6ace7b5940e34043d8edb0b8484c6) |  |
| [`DEFAULT_AUDIO_CHANNELS`](#group__av_1ga18987a0124b757cfc35978b672cd10d4) |  |
| [`DEFAULT_AUDIO_SAMPLE_FMT`](#group__av_1ga2745b7fc66dc97d94254a712e1cdc05b) |  |
| [`DEFAULT_VIDEO_SAMPLE_RATE`](#group__av_1ga4c664cf23e7f36fb3976e11becdd5f53) |  |
| [`DEFAULT_VIDEO_BIT_RATE`](#group__av_1ga7e716fda85b10b703cc0c3b7ded98df7) |  |
| [`DEFAULT_VIDEO_PIXEL_FMT`](#group__av_1ga29987df5370ad83340657e8cdb3ad686) |  |

---

#### MAX_VIDEO_PACKET_SIZE 

```cpp
MAX_VIDEO_PACKET_SIZE()
```

---

#### MAX_AUDIO_PACKET_SIZE 

```cpp
MAX_AUDIO_PACKET_SIZE()
```

1 second of 48khz 32bit audio

---

#### MAX_ENCODE_DURATION 

```cpp
MAX_ENCODE_DURATION()
```

3 hours

---

#### INT64_C 

```cpp
INT64_C()
```

---

#### UINT64_C 

```cpp
UINT64_C()
```

---

#### AV_API 

```cpp
AV_API()
```

---

#### DEFAULT_AUDIO_SAMPLE_RATE 

```cpp
DEFAULT_AUDIO_SAMPLE_RATE()
```

---

#### DEFAULT_AUDIO_BIT_RATE 

```cpp
DEFAULT_AUDIO_BIT_RATE()
```

---

#### DEFAULT_AUDIO_CHANNELS 

```cpp
DEFAULT_AUDIO_CHANNELS()
```

---

#### DEFAULT_AUDIO_SAMPLE_FMT 

```cpp
DEFAULT_AUDIO_SAMPLE_FMT()
```

---

#### DEFAULT_VIDEO_SAMPLE_RATE 

```cpp
DEFAULT_VIDEO_SAMPLE_RATE()
```

---

#### DEFAULT_VIDEO_BIT_RATE 

```cpp
DEFAULT_VIDEO_BIT_RATE()
```

---

#### DEFAULT_VIDEO_PIXEL_FMT 

```cpp
DEFAULT_VIDEO_PIXEL_FMT()
```

## IDeviceManager 

> **Subclasses:** `icy::av::DeviceManager`, `icy::av::FakeDeviceManager`
> **Defined in:** `devicemanager.h`

A platform independent interface to enumerate audio and video devices on the system.

### Members

| Name | Description |
|------|-------------|
| [`~IDeviceManager`](#group__av_1ga99bf003f84ab4e04a07d17185dbf491c) |  |
| [`initialize`](#group__av_1ga61c821fe1a7d181dbc439c90fa4524da) | Initialization. |
| [`uninitialize`](#group__av_1ga6438ea5095404fc8ded7619a7863288c) |  |
| [`getAudioInputDevices`](#group__av_1ga324ce38356335066bb2cb4fd6a8521ee) | [Device](#structicy_1_1av_1_1Device) enumeration. |
| [`getAudioOutputDevices`](#group__av_1ga389204a9d5fc74dfbfd1179f2cab8fe9) |  |
| [`getAudioInputDevice`](#group__av_1ga8eb682c21d928e20252dd682512799eb) |  |
| [`getAudioInputDevice`](#group__av_1gadf949c6e35c2d03faeabb78ce72264df) |  |
| [`getAudioOutputDevice`](#group__av_1ga294eb11a87c7bf21e263a5080cf86da6) |  |
| [`getAudioOutputDevice`](#group__av_1ga44f5d5891ec58d46f964814bdfdb9afc) |  |
| [`getVideoCaptureDevices`](#group__av_1gaf8af159e35b82018fe3c62ffd93e99b4) |  |
| [`getVideoCaptureDevice`](#group__av_1gaf9ca7d9b804d2fd3b80a66c99a5ce994) |  |
| [`getVideoCaptureDevice`](#group__av_1ga519256a027ac96e90434aaed2b84621f) |  |
| [`getDefaultAudioInputDevice`](#group__av_1ga3143d54a8029ce31cd3c35aeb458d8ba) |  |
| [`getDefaultAudioOutputDevice`](#group__av_1gac7a4388e97359a1456a1d16c582affc5) |  |
| [`getDefaultVideoCaptureDevice`](#group__av_1ga0f87a257398f70cb9ad933de0148d110) |  |
| [`getCapabilities`](#group__av_1ga96164fb6b34888ccee6930b3c79d5cda) | Capabilities. |
| [`print`](#group__av_1ga0c7df28789555440e61e55b0cf791ec1) |  |
| [`DevicesChanged`](#group__av_1ga69dbbe5f5c10c8aba8aac57c9a94e511) | Signals on DevicesChanged. Arg 1 is true when device is video, false for audio Arg 2 is true when device connects, flase on disconnection |
| [`kDefaultDeviceName`](#group__av_1ga02f897f60b7d17eea8b587087f802a53) |  |

---

#### ~IDeviceManager 

```cpp
virtual inline ~IDeviceManager()
```

---

#### initialize 

```cpp
bool initialize()
```

Initialization.

---

#### uninitialize 

```cpp
void uninitialize()
```

---

#### getAudioInputDevices 

```cpp
bool getAudioInputDevices(std::vector< Device > & devices)
```

[Device](#structicy_1_1av_1_1Device) enumeration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getAudioOutputDevices 

```cpp
bool getAudioOutputDevices(std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getAudioInputDevice 

```cpp
bool getAudioInputDevice(Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getAudioInputDevice 

```cpp
bool getAudioInputDevice(Device & out, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### getAudioOutputDevice 

```cpp
bool getAudioOutputDevice(Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getAudioOutputDevice 

```cpp
bool getAudioOutputDevice(Device & out, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### getVideoCaptureDevices 

```cpp
bool getVideoCaptureDevices(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

---

#### getVideoCaptureDevice 

```cpp
bool getVideoCaptureDevice(Device & out, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### getVideoCaptureDevice 

```cpp
bool getVideoCaptureDevice(Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getDefaultAudioInputDevice 

```cpp
bool getDefaultAudioInputDevice(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getDefaultAudioOutputDevice 

```cpp
bool getDefaultAudioOutputDevice(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getDefaultVideoCaptureDevice 

```cpp
bool getDefaultVideoCaptureDevice(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getCapabilities 

```cpp
int getCapabilities()
```

Capabilities.

---

#### print 

```cpp
void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### DevicesChanged 

```cpp
Signal< void(bool &, bool &)> DevicesChanged
```

Signals on DevicesChanged. Arg 1 is true when device is video, false for audio Arg 2 is true when device connects, flase on disconnection

---

#### kDefaultDeviceName 

```cpp
const char kDefaultDeviceName = ""
```

## DeviceWatcher 

> **Subclasses:** `icy::av::MacDeviceWatcher`
> **Defined in:** `devicemanager.h`

### Members

| Name | Description |
|------|-------------|
| [`DeviceWatcher`](#group__av_1ga66ffcd817f7ac8bc0a965e018271508c) |  |
| [`~DeviceWatcher`](#group__av_1ga191ccb64f6d1985e762bfdd54bb55bba) |  |
| [`start`](#group__av_1gab833459b9cf8c1cde8147159f265f7d8) |  |
| [`stop`](#group__av_1ga028347525717f62dfccf5bb82031fe79) |  |
| [`DeviceWatcher`](#group__av_1gaa9ae4c2ab1984066ac066e4a6cf5f5e5) |  |
| [`~DeviceWatcher`](#classicy_1_1av_1_1DeviceWatcher_1ga191ccb64f6d1985e762bfdd54bb55bba) |  |
| [`start`](#classicy_1_1av_1_1DeviceWatcher_1gab833459b9cf8c1cde8147159f265f7d8) |  |
| [`stop`](#classicy_1_1av_1_1DeviceWatcher_1ga028347525717f62dfccf5bb82031fe79) |  |

---

#### DeviceWatcher 

```cpp
inline explicit DeviceWatcher(IDeviceManager *)
```

---

#### ~DeviceWatcher 

```cpp
virtual inline ~DeviceWatcher()
```

---

#### start 

```cpp
virtual inline bool start()
```

---

#### stop 

```cpp
virtual inline void stop()
```

---

#### DeviceWatcher 

```cpp
inline explicit DeviceWatcher(DeviceManager *)
```

---

#### ~DeviceWatcher 

```cpp
virtual inline ~DeviceWatcher()
```

---

#### start 

```cpp
virtual inline bool start()
```

---

#### stop 

```cpp
virtual inline void stop()
```

## DeviceManagerFactory 

> **Defined in:** `devicemanager.h`

### Members

| Name | Description |
|------|-------------|
| [`create`](#group__av_1ga262295a8315bd7556241efc6b44219c2) |  |
| [`DeviceManagerFactory`](#group__av_1ga63037176ba23a045bb59eea820b6cf21) |  |

---

#### create 

```cpp
static IDeviceManager * create()
```

---

#### DeviceManagerFactory 

```cpp
DeviceManagerFactory()
```

## DeviceManager 

> **Extends:** `icy::av::IDeviceManager`
> **Subclasses:** `icy::av::LinuxDeviceManager`, `icy::av::MacDeviceManager`
> **Defined in:** `devicemanager.h`

### Members

| Name | Description |
|------|-------------|
| [`DeviceManager`](#group__av_1ga778c03d9b552d40b34bb8f8f9a1255d8) |  |
| [`~DeviceManager`](#group__av_1ga3635ac39fa55df574dcb0d98634c8140) |  |
| [`initialize`](#group__av_1gabe487635b243c75fae6d1ce1700f1def) | Initialization. |
| [`uninitialize`](#group__av_1gae2fcfc68cc20dedeaa8ce913307faafc) |  |
| [`getCapabilities`](#group__av_1ga42f596484bd7234fd92becaac4005e53) | Capabilities. |
| [`getAudioInputDevices`](#group__av_1gafeeaa53b13b8537cfbe903a5a26b890b) | [Device](#structicy_1_1av_1_1Device) enumeration. |
| [`getAudioOutputDevices`](#group__av_1ga2b4cd27667bb3e529de2ff4f3e3fc289) |  |
| [`getAudioInputDevice`](#group__av_1gae37c4a33994f7ef8416490531d3d0907) |  |
| [`getAudioInputDevice`](#group__av_1ga49d30065d7ecc80f9b768a47868ff505) |  |
| [`getAudioOutputDevice`](#group__av_1ga4cb9cac44ba585b23b8b07b3ab33dbb1) |  |
| [`getAudioOutputDevice`](#group__av_1ga115cb239e0244a5d40baf2a6239aec2e) |  |
| [`getVideoCaptureDevices`](#group__av_1ga70c7a7ff64734a32709d26e345398c52) |  |
| [`getVideoCaptureDevice`](#group__av_1ga2820413e3299477fb40fd84aebbf7ec8) |  |
| [`getVideoCaptureDevice`](#group__av_1gae4e7d0d99842dc18d5fef8536603ca19) | Returns the video capture device at the given system index. |
| [`getDefaultAudioInputDevice`](#group__av_1gaa87c5fbc8299cf4a82e192d69977b16e) |  |
| [`getDefaultAudioOutputDevice`](#group__av_1gab0bbdde6cab23808166819aafc305aa8) |  |
| [`getDefaultVideoCaptureDevice`](#group__av_1gac994fdb7f9415bde77421efc8cd84e7f) |  |
| [`initialized`](#group__av_1gaabba96ca78dc27e34ccd588ab813f8dc) |  |
| [`print`](#group__av_1ga485b4609f4a69f5d603b96deb411b306) |  |
| [`getCameras`](#group__av_1gaf2fcff1d7c936454117e453e4c9d6845) | [Device](#structicy_1_1av_1_1Device) enumeration. |
| [`getMicrophones`](#group__av_1gafe7f65212eb9b21f60570c3333d8e4a6) |  |
| [`getSpeakers`](#group__av_1ga47505851b38bff2744781a3e6c2f30b7) |  |
| [`getDefaultMicrophone`](#group__av_1gac40bea8a47b983fb9a759a4192c1eda7) | Default devices. |
| [`getDefaultSpeaker`](#group__av_1ga32a4b7178aea50fe2660ffe4fc4e8914) |  |
| [`getDefaultCamera`](#group__av_1gaa853bd3fb98b67a35c933127f1727dcc) |  |
| [`findCamera`](#group__av_1ga73314c79803a2665ec52ff3645930067) | Find device by name or id. |
| [`findMicrophone`](#group__av_1gaae2554630021f04763b96d0f73886724) |  |
| [`findSpeaker`](#group__av_1gaa2fd5c9c502a07c5a603ac7cf8725089) |  |
| [`getDeviceList`](#group__av_1gac2b135e577c2cca7dc422e226b7dc6f1) | Base device list. |
| [`findVideoInputFormat`](#group__av_1gac91dac73d9705d6d2c4a5e3ba3ef49a9) | Find base FFmpeg formats. |
| [`findAudioInputFormat`](#group__av_1ga3dd5c65e7f727f7befbdd4b51b5c3072) |  |
| [`filterDevices`](#group__av_1ga968051eb33e118a8a8a9d7f6a048e040) | The exclusionList MUST be a nullptr terminated list. |
| [`matchID`](#group__av_1ga1b3366864eeafa7f4bed8ff0fa1df7d5) | Returns a device matching the given ID. |
| [`matchNameAndID`](#group__av_1ga22a80ede9d0f8017cc48cc197450328b) | Returns a device matching the given name and ID. If the device name is not available at the given ID then first device of that name will be returned. If the ID should not be matched the given ID should be -1. |
| [`instance`](#group__av_1ga2a3a77dce3f0d2b147c97c34407f542f) | Returns the default `[DeviceManager](#classicy_1_1av_1_1DeviceManager)` singleton. |
| [`shutdown`](#group__av_1ga00aa8e9f2192b9e26bcbb8f1f5cd3342) | Shuts down the `MediaFactory` and deletes the singleton instance. |
| [`getDefaultAudioDevice`](#group__av_1gae9e2effb9514f16af8a0654cf59b2bf1) |  |
| [`getAudioDevices`](#group__av_1ga04e3b90e4bac6ad1bb03d00ba8337b6e) |  |
| [`getAudioDevice`](#group__av_1ga95c8c01f83b7e0b5450892be78e031eb) |  |
| [`getAudioDevice`](#group__av_1gabbe9f702e0082b2fc23f159bedf4fcfc) |  |
| [`setInitialized`](#group__av_1gab6cf6a072abd179483f6f7d484f796e4) |  |
| [`setWatcher`](#group__av_1ga90edc6121010b8344ebd6a9b8606fdf1) |  |
| [`watcher`](#group__av_1ga29bc8a0cfc10633446ae7b9dae73bb0e) |  |
| [`shouldDeviceBeIgnored`](#group__av_1ga08a06c5c337bc3ea08af5419860a2476) | The exclusionList MUST be a nullptr terminated list. |
| [`_watcher`](#group__av_1gae595c283673ca36a15dd318a953ecfc1) |  |
| [`_initialized`](#group__av_1ga24858dc86635de264ee7d020c1313eaf) |  |
| [`DevicesChanged`](#group__av_1ga066016a050182489096ab5920ecb3bca) | Signals when a system device is connecetd or removed. This signal is emitted by the platform specific `[DeviceWatcher](#classicy_1_1av_1_1DeviceWatcher)`. |
| [`_watcher`](#group__av_1gabe3bab2143c2e5739ff74505980f942d) |  |
| [`DeviceManager`](#classicy_1_1av_1_1DeviceManager_1ga778c03d9b552d40b34bb8f8f9a1255d8) |  |
| [`~DeviceManager`](#classicy_1_1av_1_1DeviceManager_1ga3635ac39fa55df574dcb0d98634c8140) |  |
| [`getCapabilities`](#classicy_1_1av_1_1DeviceManager_1ga42f596484bd7234fd92becaac4005e53) | Capabilities. |
| [`setWatcher`](#classicy_1_1av_1_1DeviceManager_1ga90edc6121010b8344ebd6a9b8606fdf1) |  |
| [`watcher`](#classicy_1_1av_1_1DeviceManager_1ga29bc8a0cfc10633446ae7b9dae73bb0e) |  |
| [`print`](#classicy_1_1av_1_1DeviceManager_1ga485b4609f4a69f5d603b96deb411b306) | Print all devices to the output stream. |

---

#### DeviceManager 

```cpp
DeviceManager()
```

---

#### ~DeviceManager 

```cpp
virtual ~DeviceManager()
```

---

#### initialize 

```cpp
virtual bool initialize()
```

Initialization.

---

#### uninitialize 

```cpp
virtual void uninitialize()
```

---

#### getCapabilities 

```cpp
virtual int getCapabilities()
```

Capabilities.

---

#### getAudioInputDevices 

```cpp
virtual bool getAudioInputDevices(std::vector< Device > & devices)
```

[Device](#structicy_1_1av_1_1Device) enumeration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getAudioOutputDevices 

```cpp
virtual bool getAudioOutputDevices(std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getAudioInputDevice 

```cpp
virtual bool getAudioInputDevice(Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getAudioInputDevice 

```cpp
virtual bool getAudioInputDevice(Device & out, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### getAudioOutputDevice 

```cpp
virtual bool getAudioOutputDevice(Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getAudioOutputDevice 

```cpp
virtual bool getAudioOutputDevice(Device & out, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### getVideoCaptureDevices 

```cpp
virtual bool getVideoCaptureDevices(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

---

#### getVideoCaptureDevice 

```cpp
virtual bool getVideoCaptureDevice(Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getVideoCaptureDevice 

```cpp
virtual bool getVideoCaptureDevice(Device & out, int id)
```

Returns the video capture device at the given system index.

| Parameter | Type | Description |
|-----------|------|-------------|
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### getDefaultAudioInputDevice 

```cpp
virtual bool getDefaultAudioInputDevice(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getDefaultAudioOutputDevice 

```cpp
virtual bool getDefaultAudioOutputDevice(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getDefaultVideoCaptureDevice 

```cpp
virtual bool getDefaultVideoCaptureDevice(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### initialized 

```cpp
inline bool initialized() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### getCameras 

```cpp
bool getCameras(std::vector< Device > & devices)
```

[Device](#structicy_1_1av_1_1Device) enumeration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getMicrophones 

```cpp
bool getMicrophones(std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getSpeakers 

```cpp
bool getSpeakers(std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |

---

#### getDefaultMicrophone 

```cpp
bool getDefaultMicrophone(Device & device)
```

Default devices.

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getDefaultSpeaker 

```cpp
bool getDefaultSpeaker(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### getDefaultCamera 

```cpp
bool getDefaultCamera(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### findCamera 

```cpp
bool findCamera(const std::string & name, Device & device)
```

Find device by name or id.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `device` | `Device &` |  |

---

#### findMicrophone 

```cpp
bool findMicrophone(const std::string & name, Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `device` | `Device &` |  |

---

#### findSpeaker 

```cpp
bool findSpeaker(const std::string & name, Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `device` | `Device &` |  |

---

#### getDeviceList 

```cpp
bool getDeviceList(Device::Type type, std::vector< av::Device > & devices)
```

Base device list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `Device::Type` |  |
| `devices` | `std::vector< av::Device > &` |  |

---

#### findVideoInputFormat 

```cpp
const AVInputFormat * findVideoInputFormat()
```

Find base FFmpeg formats.

---

#### findAudioInputFormat 

```cpp
const AVInputFormat * findAudioInputFormat()
```

---

#### filterDevices 

```cpp
static bool filterDevices(std::vector< Device > & devices, const char *const exclusionList)
```

The exclusionList MUST be a nullptr terminated list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |
| `exclusionList` | `const char *const` |  |

---

#### matchID 

```cpp
static bool matchID(std::vector< Device > & devices, Device & out, int id)
```

Returns a device matching the given ID.

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### matchNameAndID 

```cpp
static bool matchNameAndID(std::vector< Device > & devices, Device & out, const std::string & name, int id)
```

Returns a device matching the given name and ID. If the device name is not available at the given ID then first device of that name will be returned. If the ID should not be matched the given ID should be -1.

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `std::vector< Device > &` |  |
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### instance 

```cpp
static DeviceManager & instance()
```

Returns the default `[DeviceManager](#classicy_1_1av_1_1DeviceManager)` singleton.

---

#### shutdown 

```cpp
static void shutdown()
```

Shuts down the `MediaFactory` and deletes the singleton instance.

---

#### getDefaultAudioDevice 

```cpp
virtual bool getDefaultAudioDevice(bool input, Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `bool` |  |
| `device` | `Device &` |  |

---

#### getAudioDevices 

```cpp
virtual bool getAudioDevices(bool input, std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `bool` |  |
| `devs` | `std::vector< Device > &` |  |

---

#### getAudioDevice 

```cpp
virtual bool getAudioDevice(bool input, Device & out, const std::string & name, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `bool` |  |
| `out` | `Device &` |  |
| `name` | `const std::string &` |  |
| `id` | `int` |  |

---

#### getAudioDevice 

```cpp
virtual bool getAudioDevice(bool input, Device & out, int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `bool` |  |
| `out` | `Device &` |  |
| `id` | `int` |  |

---

#### setInitialized 

```cpp
void setInitialized(bool initialized)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `initialized` | `bool` |  |

---

#### setWatcher 

```cpp
void setWatcher(DeviceWatcher * watcher)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `watcher` | `DeviceWatcher *` |  |

---

#### watcher 

```cpp
DeviceWatcher * watcher()
```

---

#### shouldDeviceBeIgnored 

```cpp
static bool shouldDeviceBeIgnored(const std::string & deviceName, const char *const exclusionList)
```

The exclusionList MUST be a nullptr terminated list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `deviceName` | `const std::string &` |  |
| `exclusionList` | `const char *const` |  |

---

#### _watcher 

```cpp
DeviceWatcher * _watcher
```

---

#### _initialized 

```cpp
bool _initialized
```

---

#### DevicesChanged 

```cpp
NullSignal DevicesChanged
```

Signals when a system device is connecetd or removed. This signal is emitted by the platform specific `[DeviceWatcher](#classicy_1_1av_1_1DeviceWatcher)`.

---

#### _watcher 

```cpp
std::unique_ptr< DeviceWatcher > _watcher
```

---

#### DeviceManager 

```cpp
DeviceManager()
```

---

#### ~DeviceManager 

```cpp
~DeviceManager()
```

---

#### getCapabilities 

```cpp
virtual int getCapabilities()
```

Capabilities.

---

#### setWatcher 

```cpp
void setWatcher([DeviceWatcher](#classicy_1_1av_1_1DeviceWatcher) * watcher)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `watcher` | `[DeviceWatcher](#classicy_1_1av_1_1DeviceWatcher) *` |  |

---

#### watcher 

```cpp
[DeviceWatcher](#classicy_1_1av_1_1DeviceWatcher) * watcher()
```

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

Print all devices to the output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

## FakeDeviceManager 

> **Extends:** `icy::av::IDeviceManager`
> **Defined in:** `devicemanager_fake.h`

### Members

| Name | Description |
|------|-------------|
| [`FakeDeviceManager`](#group__av_1ga006594e6f296225b135e550be80dcb13) |  |
| [`initialize`](#group__av_1ga3f83116598bfa3b3d53a4c4c25cf1cee) | Initialization. |
| [`uninitialize`](#group__av_1ga1ba541135e1a1c0039ade4343403a25b) |  |
| [`getCapabilities`](#group__av_1gaa72540c5d7eb2f07ad1317944cd28565) | Capabilities. |
| [`getMicrophones`](#group__av_1gaed7a9dee4d3dffe342c52d5d1edbebeb) |  |
| [`getSpeakers`](#group__av_1ga89c33c93e68bc6bb55945952b8261f4e) |  |
| [`getMicrophone`](#group__av_1gabc947f13611657bf38e64b901449aa1d) |  |
| [`getSpeaker`](#group__av_1ga4d7895da9bbbcc0c57480d59c4c45dc4) |  |
| [`getCameras`](#group__av_1gad2513f312a81350e6b936fc33ad21405) |  |
| [`getDefaultCamera`](#group__av_1gaf3aaf6ebb5675f70bc0cc3acd70875ba) |  |
| [`setMicrophones`](#group__av_1gac6f0eef0d4653d05d1ed39ff61c3e775) |  |
| [`setSpeakers`](#group__av_1ga063298c41c2f469d96614f1c19d9cd4e) |  |
| [`setCameras`](#group__av_1ga7d8d1af543bbb57550820afe9d6d7b2f) |  |
| [`getCamera`](#group__av_1ga285e406d330f5c6c0f6ab8cdde3fd634) |  |
| [`getAudioDevice`](#group__av_1gaafe3fffef143106836168cc7ac4d0716) |  |
| [`findDeviceByName`](#group__av_1ga3ce54e061c6703f51c472734d300bec2) |  |
| [`input_devices_`](#group__av_1ga8447f95deb91c759c090f19365852cdc) |  |
| [`output_devices_`](#group__av_1ga1a2d888a668bbc0072cf71342df540da) |  |
| [`vidcap_devices_`](#group__av_1gabfb8bc40437c73c4ae6a1169916d69e5) |  |

---

#### FakeDeviceManager 

```cpp
inline FakeDeviceManager()
```

---

#### initialize 

```cpp
virtual inline bool initialize()
```

Initialization.

---

#### uninitialize 

```cpp
virtual inline void uninitialize()
```

---

#### getCapabilities 

```cpp
virtual inline int getCapabilities()
```

Capabilities.

---

#### getMicrophones 

```cpp
virtual inline bool getMicrophones(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

---

#### getSpeakers 

```cpp
virtual inline bool getSpeakers(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

---

#### getMicrophone 

```cpp
virtual inline bool getMicrophone(const std::string & name, Device & out)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `out` | `Device &` |  |

---

#### getSpeaker 

```cpp
virtual inline bool getSpeaker(const std::string & name, Device & out)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `out` | `Device &` |  |

---

#### getCameras 

```cpp
virtual inline bool getCameras(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

---

#### getDefaultCamera 

```cpp
virtual inline bool getDefaultCamera(Device & device)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `Device &` |  |

---

#### setMicrophones 

```cpp
inline void setMicrophones(const std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `const std::vector< Device > &` |  |

---

#### setSpeakers 

```cpp
inline void setSpeakers(const std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `const std::vector< Device > &` |  |

---

#### setCameras 

```cpp
inline void setCameras(const std::vector< Device > & devices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `const std::vector< Device > &` |  |

---

#### getCamera 

```cpp
virtual inline bool getCamera(const std::string & name, Device & out)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `out` | `Device &` |  |

---

#### getAudioDevice 

```cpp
inline bool getAudioDevice(bool input, const std::string & name, Device & out)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `bool` |  |
| `name` | `const std::string &` |  |
| `out` | `Device &` |  |

---

#### findDeviceByName 

```cpp
static inline bool findDeviceByName(const std::vector< Device > & devices, const std::string & name, Device & out)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devices` | `const std::vector< Device > &` |  |
| `name` | `const std::string &` |  |
| `out` | `Device &` |  |

---

#### input_devices_ 

```cpp
std::vector< Device > input_devices_
```

---

#### output_devices_ 

```cpp
std::vector< Device > output_devices_
```

---

#### vidcap_devices_ 

```cpp
std::vector< Device > vidcap_devices_
```

## LinuxDeviceManager 

> **Extends:** `icy::av::DeviceManager`
> **Defined in:** `devicemanager_linux.h`

### Members

| Name | Description |
|------|-------------|
| [`LinuxDeviceManager`](#group__av_1ga183809a0da4b49689b13e098f8d0ac03) |  |
| [`~LinuxDeviceManager`](#group__av_1gab60b8f07ec657f5cded2956320e10a48) |  |
| [`getCameras`](#group__av_1gac931c97e65968e0f46d18038e5789939) |  |

---

#### LinuxDeviceManager 

```cpp
LinuxDeviceManager()
```

---

#### ~LinuxDeviceManager 

```cpp
virtual ~LinuxDeviceManager()
```

---

#### getCameras 

```cpp
virtual bool getCameras(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

## MacDeviceManager 

> **Extends:** `icy::av::DeviceManager`
> **Defined in:** `devicemanager_mac.h`

### Members

| Name | Description |
|------|-------------|
| [`MacDeviceManager`](#group__av_1gac14003aeb093497f3a2930f26cae0acf) |  |
| [`~MacDeviceManager`](#group__av_1gaa0500c9b93a11442e3d505b0e7611848) |  |
| [`getCameras`](#group__av_1ga689d89e5f57be3c868009b9ffdd11fe2) |  |
| [`FilterDevice`](#group__av_1ga0827c843ec57dffe59c82763bb52af1e) | virtual bool [getAudioDevices(bool input, std::vector<Device>& devs)](#group__av_1ga04e3b90e4bac6ad1bb03d00ba8337b6e); |

---

#### MacDeviceManager 

```cpp
MacDeviceManager()
```

---

#### ~MacDeviceManager 

```cpp
virtual ~MacDeviceManager()
```

---

#### getCameras 

```cpp
virtual bool getCameras(std::vector< Device > & devs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `devs` | `std::vector< Device > &` |  |

---

#### FilterDevice 

```cpp
bool FilterDevice(const Device & d)
```

virtual bool [getAudioDevices(bool input, std::vector<Device>& devs)](#group__av_1ga04e3b90e4bac6ad1bb03d00ba8337b6e);

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `const Device &` |  |

## FLVMetadataInjector 

> **Extends:** `icy::PacketProcessor`
> **Defined in:** `flvmetadatainjector.h`

This class implements a packetizer which appends correct stream headers and modifies the timestamp of FLV packets so Adobe's Flash Player will play our videos mid-stream.

This adapter is useful for multicast situations where we don't have the option of restarting the encoder stream.

### Members

| Name | Description |
|------|-------------|
| [`AMFDataType`](#group__av_1gaa58292d406cc0079da1c7c6c8d362714) |  |
| [``](#group__av_1ga5f63483be4c5ad888f60998ab277162c) |  |
| [``](#group__av_1ga0c2328096dabc3e7794a0564891e002b) |  |
| [`IPacketizer`](#group__av_1ga9f887b63bbd57a16ebea78f42352cf80) |  |
| [`_format`](#group__av_1ga00b49e6d76e5c12d79a8b31f7f68aa16) |  |
| [`_initial`](#group__av_1ga5341424127e2b262009062d53f14dd89) |  |
| [`_modifyingStream`](#group__av_1ga620bcff399fc9e599fdb134d7f164a30) |  |
| [`_waitingForKeyframe`](#group__av_1ga595e597a9cb20e8a558501f7bbeff7ed) |  |
| [`_timestampOffset`](#group__av_1gae40bbe2c2eda197672f9fad8f13690cb) |  |
| [`onStreamStateChange`](#group__av_1gad92da5518ddd902a7136e83dde6ba503) | whenever the stream is restarted. |
| [`process`](#group__av_1gaf7675aec070348fb52657d0bbc81264d) | This method performs processing on the given packet and emits the result. |
| [`fastUpdateTimestamp`](#group__av_1gac6bdf4094f0164d4557b501346fa3f47) | Updates the timestamp in the given FLV tag buffer. No more need to copy data with this method. Caution: this method does not check buffer size. |
| [`fastIsFLVHeader`](#group__av_1gaff079b714922df3458c65ba3ef60a3f6) | Caution: this method does not check buffer size. |
| [`fastIsFLVKeyFrame`](#group__av_1ga34a40b333d9f32b4ef94f3e3f028665a) | Caution: this method does not check buffer size. |
| [`writeFLVHeader`](#group__av_1ga6aa18b140ad46d0e7a2d4ac44afe3e06) |  |
| [`doubleToInt`](#group__av_1ga371b1182cbe6cf19c17d7add8cec0274) |  |
| [`writeAMFSring`](#group__av_1ga28c5478abb189d041c739068082455c8) | AMF Helpers. |
| [`writeAMFDouble`](#group__av_1gad544d5db35df80ebb048dec5f5f3a286) |  |
| [`writeAMFBool`](#group__av_1ga2aa49c8d0d9ccf3602b3fe7bbbfb1b1d) |  |
| [`dumpFLVTags`](#group__av_1ga1070f13732cb0431b30509fba615b14e) |  |
| [`emitter`](#group__av_1ga8b361da8e252816c458d4987247a06c0) |  |
| [`_format`](#group__av_1gad846cf859e5cea50a984de6f9544b165) |  |
| [`_initial`](#group__av_1ga1d940a770234cb5ad44e7622110f4fb0) |  |
| [`_modifyingStream`](#group__av_1ga0d0de9ad89aeb3c708219b126397d703) |  |
| [`_waitingForKeyframe`](#group__av_1ga85b7dc7527c9cce0db4af6440700ffa8) |  |
| [`_timestampOffset`](#group__av_1ga612c3a994d9cd6472c5a195215b94b4d) |  |
| [`_fpsCounter`](#group__av_1ga73a76b5f863b967f49a85cdc8c72e433) |  |

---

#### AMFDataType 

```cpp
enum AMFDataType
```

| Value | Description |
|-------|-------------|
| `AMF_DATA_TYPE_NUMBER` |  |
| `AMF_DATA_TYPE_BOOL` |  |
| `AMF_DATA_TYPE_STRING` |  |
| `AMF_DATA_TYPE_OBJECT` |  |
| `AMF_DATA_TYPE_NULL` |  |
| `AMF_DATA_TYPE_UNDEFINED` |  |
| `AMF_DATA_TYPE_REFERENCE` |  |
| `AMF_DATA_TYPE_MIXEDARRAY` |  |
| `AMF_DATA_TYPE_OBJECT_END` |  |
| `AMF_DATA_TYPE_ARRAY` |  |
| `AMF_DATA_TYPE_DATE` |  |
| `AMF_DATA_TYPE_LONG_STRING` |  |
| `AMF_DATA_TYPE_UNSUPPORTED` |  |

---

####  

```cpp
enum 
```

| Value | Description |
|-------|-------------|
| `FLV_TAG_TYPE_AUDIO` |  |
| `FLV_TAG_TYPE_VIDEO` |  |
| `FLV_TAG_TYPE_SCRIPT` |  |

---

####  

```cpp
enum 
```

| Value | Description |
|-------|-------------|
| `FLV_FRAME_KEY` |  |
| `FLV_FRAME_INTER` |  |
| `FLV_FRAME_DISP_INTER` |  |

---

#### IPacketizer 

```cpp
IPacketizer(this-> emitter)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `emitter` | `this->` |  |

---

#### _format 

```cpp
_format(format)
```

---

#### _initial 

```cpp
_initial(true)
```

---

#### _modifyingStream 

```cpp
_modifyingStream(false)
```

---

#### _waitingForKeyframe 

```cpp
_waitingForKeyframe(false)
```

---

#### _timestampOffset 

```cpp
inline _timestampOffset(0) const
```

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState & state)
```

whenever the stream is restarted.

This method is called by the Packet [Stream](./doc/api-base.md#classicy_1_1Stream)

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `const PacketStreamState &` |  |

---

#### process 

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](./doc/api-base.md#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### fastUpdateTimestamp 

```cpp
virtual inline void fastUpdateTimestamp(char * buf, std::uint32_t timestamp)
```

Updates the timestamp in the given FLV tag buffer. No more need to copy data with this method. Caution: this method does not check buffer size.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `char *` |  |
| `timestamp` | `std::uint32_t` |  |

---

#### fastIsFLVHeader 

```cpp
virtual inline bool fastIsFLVHeader(char * buf)
```

Caution: this method does not check buffer size.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `char *` |  |

---

#### fastIsFLVKeyFrame 

```cpp
virtual inline bool fastIsFLVKeyFrame(char * buf)
```

Caution: this method does not check buffer size.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `char *` |  |

---

#### writeFLVHeader 

```cpp
virtual inline void writeFLVHeader(BitWriter & writer)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### doubleToInt 

```cpp
inline std::int64_t doubleToInt(double d)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `double` |  |

---

#### writeAMFSring 

```cpp
virtual inline void writeAMFSring(BitWriter & writer, const char * val)
```

AMF Helpers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |
| `val` | `const char *` |  |

---

#### writeAMFDouble 

```cpp
virtual inline void writeAMFDouble(BitWriter & writer, double val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |
| `val` | `double` |  |

---

#### writeAMFBool 

```cpp
virtual inline void writeAMFBool(BitWriter & writer, bool val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |
| `val` | `bool` |  |

---

#### dumpFLVTags 

```cpp
static inline bool dumpFLVTags(BitReader & reader)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### _format 

```cpp
Format _format
```

---

#### _initial 

```cpp
bool _initial
```

---

#### _modifyingStream 

```cpp
bool _modifyingStream
```

---

#### _waitingForKeyframe 

```cpp
bool _waitingForKeyframe
```

---

#### _timestampOffset 

```cpp
std::uint32_t _timestampOffset
```

---

#### _fpsCounter 

```cpp
legacy::FPSCounter _fpsCounter
```

## VideoAnalyzer 

> **Defined in:** `videoanalyzer.h`

This class provides basic AV spectrum analysis on a video using the Fourier Transform algorithm. Data is outputted in CSV format.

TODO:/// - Pluggable algorithms and processors

* Normalization (scaling) for output values

* [Process](./doc/api-base.md#classicy_1_1Process) multiple audio channels properly

* Inherit from [PacketProcessor](./doc/api-base.md#classicy_1_1PacketProcessor)

### Members

| Name | Description |
|------|-------------|
| [`VideoAnalyzer`](#group__av_1gac993a14f178ad54a007326285a81298e) |  |
| [`~VideoAnalyzer`](#group__av_1ga8baaaa919a048d47aae1a727f17c54e0) |  |
| [`initialize`](#group__av_1ga12c07a7905f57e13e4dd068d66bb1368) | Set everything up, and open the input file. |
| [`uninitialize`](#group__av_1ga8f31eec4f3b34bd581e9a06d723e5ae7) | Stop processing and free everything. |
| [`start`](#group__av_1gad69119157d765c60cfc1e15182403c1e) | Begin processing. |
| [`stop`](#group__av_1ga027a1e0984779cef7815b1ed349f5333) | Stop processing. |
| [`reader`](#group__av_1gafdcd38afd5348de442f3b5acd155df42) |  |
| [`options`](#group__av_1ga4e52c919db9335da1ba1b6fa69de9032) |  |
| [`error`](#group__av_1ga93f4c712ca703193022cf52d838778ab) |  |
| [`getGrayVideoFrame`](#group__av_1ga8e7e0a166943d5172047fa223900f961) |  |
| [`onReadComplete`](#group__av_1ga15b69731e04c27a54a8fe2d1f5c2f3c0) |  |
| [`onVideo`](#group__av_1gabccd45c480e1118524bd28dbb91f2d15) |  |
| [`onAudio`](#group__av_1gad9b3381f6e3189275d050327f42a57bf) |  |
| [`className`](#group__av_1ga14432811560e9bf62c137882300c7e03) |  |
| [`PacketOut`](#group__av_1gabcf4743d8bd41e4499ccd11191a4fbe2) | Signals on [VideoAnalyzer::Packet](#structicy_1_1av_1_1VideoAnalyzer_1_1Packet) output Raw FFT data is available via [VideoAnalyzer::Stream](#structicy_1_1av_1_1VideoAnalyzer_1_1Stream)->rdftData |
| [`Complete`](#group__av_1ga2f0831ef8267630b861cdc91158864a6) | Signals on analysis complete. |
| [`_mutex`](#group__av_1ga758687490e01b5e0f45eda901ab37ae8) |  |
| [`_options`](#group__av_1gae42dec896e9b00a69bcfebede88b3ff6) |  |
| [`_error`](#group__av_1ga0cdbb5f6f79191c6a6735d9745e20a78) |  |
| [`_reader`](#group__av_1gaa16cee59bc4837a365cd203c149a68f1) |  |
| [`_video`](#group__av_1ga3f21224e7f325b64bf3da5500ea28571) |  |
| [`_audio`](#group__av_1ga3da1b0cf78f242341edd610b022d83be) |  |
| [`_videoConv`](#group__av_1gacd5118e720bd223beb0ee6f7b7b1c3ee) |  |

---

#### VideoAnalyzer 

```cpp
VideoAnalyzer(const VideoAnalyzer::Options & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `const VideoAnalyzer::Options &` |  |

---

#### ~VideoAnalyzer 

```cpp
virtual ~VideoAnalyzer()
```

---

#### initialize 

```cpp
virtual void initialize()
```

Set everything up, and open the input file.

---

#### uninitialize 

```cpp
virtual void uninitialize()
```

Stop processing and free everything.

---

#### start 

```cpp
virtual void start()
```

Begin processing.

---

#### stop 

```cpp
virtual void stop()
```

Stop processing.

---

#### reader 

```cpp
virtual MediaCapture & reader()
```

---

#### options 

```cpp
virtual Options & options()
```

---

#### error 

```cpp
virtual std::string error() const
```

---

#### getGrayVideoFrame 

```cpp
AVFrame * getGrayVideoFrame()
```

---

#### onReadComplete 

```cpp
virtual void onReadComplete(void * sender)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |

---

#### onVideo 

```cpp
virtual void onVideo(void * sender, VideoPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `packet` | `VideoPacket &` |  |

---

#### onAudio 

```cpp
virtual void onAudio(void * sender, AudioPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `packet` | `AudioPacket &` |  |

---

#### className 

```cpp
inline const char * className() const
```

---

#### PacketOut 

```cpp
Signal< void(const VideoAnalyzer::Stream &, const VideoAnalyzer::Packet &)> PacketOut
```

Signals on [VideoAnalyzer::Packet](#structicy_1_1av_1_1VideoAnalyzer_1_1Packet) output Raw FFT data is available via [VideoAnalyzer::Stream](#structicy_1_1av_1_1VideoAnalyzer_1_1Stream)->rdftData

---

#### Complete 

```cpp
NullSignal Complete
```

Signals on analysis complete.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _options 

```cpp
Options _options
```

---

#### _error 

```cpp
std::string _error
```

---

#### _reader 

```cpp
MediaCapture _reader
```

---

#### _video 

```cpp
VideoAnalyzer::Stream * _video
```

---

#### _audio 

```cpp
VideoAnalyzer::Stream * _audio
```

---

#### _videoConv 

```cpp
VideoConverter * _videoConv
```

## Options 

> **Defined in:** `videoanalyzer.h`

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__av_1ga0610fd6f6ea8ccc982764e198c4191df) |  |
| [`ifile`](#group__av_1ga0588f37bc4a07997c5b52aad94488177) |  |
| [`rdftSize`](#group__av_1ga3930c2df8a401e1b6a6cb65d824a1f37) |  |

---

#### Options 

```cpp
inline Options()
```

---

#### ifile 

```cpp
std::string ifile
```

---

#### rdftSize 

```cpp
int rdftSize
```

## Packet 

> **Defined in:** `videoanalyzer.h`

### Members

| Name | Description |
|------|-------------|
| [`Packet`](#group__av_1gad1482a53678eb45b010543da58fd75d3) |  |
| [`time`](#group__av_1gaf5901e114c3cc04e690f91d4fb7b0b6b) |  |
| [`value`](#group__av_1ga35702a4a9df499a661bdf48c5f0a8921) |  |

---

#### Packet 

```cpp
Packet(double time, double value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | `double` |  |
| `value` | `double` |  |

---

#### time 

```cpp
double time
```

---

#### value 

```cpp
double value
```

## Stream 

> **Defined in:** `videoanalyzer.h`

### Members

| Name | Description |
|------|-------------|
| [`Stream`](#group__av_1ga4b11e34ec10c49f9dcdf30a25b2bd5d2) |  |
| [`~Stream`](#group__av_1ga523d33f3555c415b63ca428744f814c7) |  |
| [`initialize`](#group__av_1gad2da7bdf2d9164a40e83b4cc0acc7a82) |  |
| [`uninitialize`](#group__av_1gac92af80fd4eb8ff92e4bab9608c29dc3) |  |
| [`fft`](#group__av_1ga67a9547ab771c61d603086a5dd922b0f) |  |
| [`name`](#group__av_1ga0273e4f2e768d0daa30719dcf97a9a66) |  |
| [`rdft`](#group__av_1ga1c501d438007311a16dfb1d93572f540) |  |
| [`rdftData`](#group__av_1ga39711bf7e95a4b07f43c3934643fa930) |  |
| [`rdftSize`](#group__av_1gacca84a21ae498fcb9035ad6cb33b9f2b) |  |
| [`rdftBits`](#group__av_1ga2979c4b96e03f235536ea6ee7b8a230c) |  |
| [`frames`](#group__av_1ga96a21cba5b0e1fbb07eb3dc4236f414f) |  |
| [`filled`](#group__av_1gace58db739f0c0dcc61db800cafa2e45e) |  |

---

#### Stream 

```cpp
Stream(const std::string & name, int rdftSize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `rdftSize` | `int` |  |

---

#### ~Stream 

```cpp
~Stream()
```

---

#### initialize 

```cpp
void initialize()
```

---

#### uninitialize 

```cpp
void uninitialize()
```

---

#### fft 

```cpp
void fft()
```

---

#### name 

```cpp
std::string name
```

---

#### rdft 

```cpp
RDFTContext * rdft
```

---

#### rdftData 

```cpp
FFTSample * rdftData
```

---

#### rdftSize 

```cpp
int rdftSize
```

---

#### rdftBits 

```cpp
int rdftBits
```

---

#### frames 

```cpp
std::int64_t frames
```

---

#### filled 

```cpp
int filled
```

## MacDeviceWatcher 

> **Extends:** `icy::av::DeviceWatcher`

### Members

| Name | Description |
|------|-------------|
| [`MacDeviceWatcher`](#group__av_1gaac48968d26c753ef2eb58b8a5b940221) |  |
| [`~MacDeviceWatcher`](#group__av_1ga32910b3b0f011d047bb4521df07b2d67) |  |
| [`start`](#group__av_1ga86ba79c6b43c5f35db2f660da0e72147) |  |
| [`stop`](#group__av_1ga9133417d7365867fd021e28a52f08c90) |  |
| [`manager_`](#group__av_1gaeabaa25080e8af217554a29ddf32e862) |  |
| [`impl_`](#group__av_1ga3c7c48673111c854587f4c8d7fc19d8e) |  |

---

#### MacDeviceWatcher 

```cpp
explicit MacDeviceWatcher(IDeviceManager * dm)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dm` | `IDeviceManager *` |  |

---

#### ~MacDeviceWatcher 

```cpp
virtual ~MacDeviceWatcher()
```

---

#### start 

```cpp
virtual bool start()
```

---

#### stop 

```cpp
virtual void stop()
```

---

#### manager_ 

```cpp
IDeviceManager * manager_
```

---

#### impl_ 

```cpp
[DeviceWatcherImpl](./doc/api-undefined.md#interfaceDeviceWatcherImpl) * impl_
```

## AudioCapture 

> **Extends:** `icy::av::MediaCapture`
> **Defined in:** `audiocapture.h`

This class implements a cross platform audio capturer.

### Members

| Name | Description |
|------|-------------|
| [`AudioCapture`](#group__av_1gae078929c2f84b9c78014b9c071c12414) |  |
| [`AudioCapture`](#group__av_1gaa98c303de9b4a2e8daaf330c02a6eadd) |  |
| [`AudioCapture`](#group__av_1gaf0b333f407a129ff8f499df33744ef5b) |  |
| [`~AudioCapture`](#group__av_1ga5c267d2acd4fc853b46cc1db116b3d02) |  |
| [`openAudio`](#group__av_1ga7f191e45d9c4190b8ea6f42061e5aa73) |  |
| [`openAudio`](#group__av_1gaa845730311774b9f77706723acf6d6ab) |  |

---

#### AudioCapture 

```cpp
AudioCapture()
```

---

#### AudioCapture 

```cpp
AudioCapture(const std::string & device, const av::AudioCodec & params)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `params` | `const av::AudioCodec &` |  |

---

#### AudioCapture 

```cpp
AudioCapture(const std::string & device, int channels, int sampleRate, const std::string & sampleFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `channels` | `int` |  |
| `sampleRate` | `int` |  |
| `sampleFmt` | `const std::string &` |  |

---

#### ~AudioCapture 

```cpp
virtual ~AudioCapture()
```

---

#### openAudio 

```cpp
virtual void openAudio(const std::string & device, const av::AudioCodec & params)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `params` | `const av::AudioCodec &` |  |

---

#### openAudio 

```cpp
virtual void openAudio(const std::string & device, int channels, int sampleRate, const std::string & sampleFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `channels` | `int` |  |
| `sampleRate` | `int` |  |
| `sampleFmt` | `const std::string &` |  |

## FormatRegistry 

> **Defined in:** `formatregistry.h`

### Members

| Name | Description |
|------|-------------|
| [`instance`](#group__av_1gaf75e6c54e5e6a2a4c2fc5b51157351ba) |  |
| [`FormatRegistry`](#group__av_1gaaf5a5ede3c5909f579a34c56e8186f1c) |  |
| [`~FormatRegistry`](#group__av_1gac6f77454c59e8bce0a602d0973f82515) |  |
| [`get`](#group__av_1gabd3e9f2a811539d26a8edb4ba4e3ed27) |  |
| [`getByID`](#group__av_1ga5139a694c99b22a01dd6cebdcdcf80eb) |  |
| [`getOrDefault`](#group__av_1ga0060ffcf37bbd8efa2362c598872c81a) | Returns the default media format. |
| [`getDefault`](#group__av_1gac0c655caad54b6192ecfdc73c04b00e1) | If a default has been specified it will be returned, other the format with the highest priority will take precedence. |
| [`registerFormat`](#group__av_1ga2af81c756b1e9a18b4d9fec1d0aae501) | Registers the given media format overriding existing media formats of the same name. |
| [`unregisterFormat`](#group__av_1ga35d5b38f24071b665dbe0895f9c21840) | Unregisters the media format matching the given name. |
| [`setDefault`](#group__av_1gabf704e68f4ba8ca65eb6ed45a7b7ef7e) | Sets the default fallback media format. |
| [`exists`](#group__av_1ga4723ecceee10ce9b5c10500fcab53da1) |  |
| [`clear`](#group__av_1ga11cf8bc02b9f1236a52b8a8ba6f04342) |  |
| [`formats`](#group__av_1gac522b25711b01a224f02df815860a9d6) |  |
| [`FormatRegistry`](#group__av_1ga462123d2a4f764202f512ebf1e1ca0a3) |  |
| [`operator=`](#group__av_1ga5085e598588f063304f60e8456b9333d) |  |
| [`_formats`](#group__av_1gae0ccfdf862160fd24575efb58e9ed44a) |  |
| [`_default`](#group__av_1gae517ab74c8e87f17673ff47ed95949a9) |  |
| [`_mutex`](#group__av_1gaf7a13cd56f063bfc5744099b9a49d825) |  |

---

#### instance 

```cpp
static FormatRegistry & instance()
```

---

#### FormatRegistry 

```cpp
FormatRegistry()
```

---

#### ~FormatRegistry 

```cpp
virtual ~FormatRegistry()
```

---

#### get 

```cpp
virtual Format & get(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### getByID 

```cpp
virtual Format & getByID(const std::string & id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### getOrDefault 

```cpp
virtual Format & getOrDefault(const std::string & name)
```

Returns the default media format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### getDefault 

```cpp
virtual Format & getDefault()
```

If a default has been specified it will be returned, other the format with the highest priority will take precedence.

---

#### registerFormat 

```cpp
virtual void registerFormat(const Format & format)
```

Registers the given media format overriding existing media formats of the same name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `format` | `const Format &` |  |

---

#### unregisterFormat 

```cpp
virtual bool unregisterFormat(const std::string & name)
```

Unregisters the media format matching the given name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setDefault 

```cpp
virtual void setDefault(const std::string & name)
```

Sets the default fallback media format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### exists 

```cpp
virtual bool exists(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### clear 

```cpp
virtual void clear()
```

---

#### formats 

```cpp
virtual FormatList formats() const
```

---

#### FormatRegistry 

```cpp
FormatRegistry(FormatRegistry const &) = delete
```

---

#### operator= 

```cpp
FormatRegistry & operator=(FormatRegistry const &) = delete
```

---

#### _formats 

```cpp
FormatList _formats
```

---

#### _default 

```cpp
std::string _default
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## FPSCounter 

> **Defined in:** `fpscounter.h`

FPS counter based on the simple moving average (SMA) algorithm.

### Members

| Name | Description |
|------|-------------|
| [`updateAvg`](#group__av_1ga4271e5e2a3cc45859ed77f5aa230e89b) |  |
| [`FPSCounter`](#group__av_1ga3a7ac11cae2ba7667dd8ee25c2c09dd7) |  |
| [`reset`](#group__av_1ga5e7f4d6a27bcddab6c42387f7fa10b17) |  |
| [`tick`](#group__av_1ga462bac68ff4e426f966bb1bf92e426f8) |  |
| [`MAX`](#group__av_1ga1ccc52491f07a8187df9b8af7d028acb) |  |
| [`tickIndex`](#group__av_1gaff38614aa382382abd783764cfcf3b65) |  |
| [`prevTick`](#group__av_1gaf05e8b7251c3159f5bde8c39c6af0a75) |  |
| [`tickSum`](#group__av_1ga7dd1926c878fbcc471b3bea0e1f37492) |  |
| [`tickList`](#group__av_1ga5a7e8f4511f34e9e550ff2e3276278b0) |  |
| [`fps`](#group__av_1ga116ea3077cf60b7a576b59a6564e829d) |  |
| [`frames`](#group__av_1ga3aaf0a4591fdce8c91aba11e85a32a73) |  |

---

#### updateAvg 

```cpp
inline double updateAvg(clock_t newTick)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `newTick` | `clock_t` |  |

---

#### FPSCounter 

```cpp
inline FPSCounter()
```

---

#### reset 

```cpp
inline void reset()
```

---

#### tick 

```cpp
inline void tick()
```

---

#### MAX 

```cpp
const int MAX = 100
```

---

#### tickIndex 

```cpp
int tickIndex
```

---

#### prevTick 

```cpp
clock_t prevTick
```

---

#### tickSum 

```cpp
clock_t tickSum
```

---

#### tickList 

```cpp
clock_t tickList
```

---

#### fps 

```cpp
double fps
```

---

#### frames 

```cpp
int64_t frames
```

## FPSLimiter 

> **Extends:** `icy::PacketProcessor`
> **Defined in:** `fpscounter.h`

This class limits the throughput rate of IPackets in a [PacketStream](./doc/api-base.md#classicy_1_1PacketStream). If the throughput rate exceeds the max specified FPS then packets will be dropped.

Note that revious processors must not fragment packets otherwise this class will not be accurate, and the packet drop rate will be too high.

### Members

| Name | Description |
|------|-------------|
| [`FPSLimiter`](#group__av_1ga029c9650c19891d96c7d05b720ada193) |  |
| [`process`](#group__av_1ga44d11b0258baa981ef71b9b5647de97d) | This method performs processing on the given packet and emits the result. |
| [`onStreamStateChange`](#group__av_1ga159f48189ca69a2e40d20376335249fb) | Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`emitter`](#group__av_1ga59db66b46321468598d83db2a52b5495) |  |
| [`_max`](#group__av_1ga939b5b9fd83801ddc2fdea386c4e556d) |  |
| [`_videoOnly`](#group__av_1ga1d2f345f16e43261b3432d15157e58d6) |  |
| [`_counter`](#group__av_1ga5eb5923a12182c1c97382cf9da35f938) |  |

---

#### FPSLimiter 

```cpp
inline FPSLimiter(int max, bool videoOnly)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `max` | `int` |  |
| `videoOnly` | `bool` |  |

---

#### process 

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](./doc/api-base.md#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### _max 

```cpp
int _max
```

---

#### _videoOnly 

```cpp
bool _videoOnly
```

---

#### _counter 

```cpp
legacy::FPSCounter _counter
```

## ICapture 

> **Extends:** `icy::PacketStreamAdapter`, `icy::basic::Startable`
> **Subclasses:** `icy::av::MediaCapture`
> **Defined in:** `icapture.h`

### Members

| Name | Description |
|------|-------------|
| [`ICapture`](#group__av_1ga3f62aa3503362c5540dc612bce32a0b3) |  |
| [`~ICapture`](#group__av_1gac520e48bc641b38e050000b860bb6609) |  |
| [`start`](#group__av_1ga001da1b367e05e2a1975dd07e6daab35) |  |
| [`stop`](#group__av_1gad0feb65608ad2d11078abb46d5918284) |  |
| [`getEncoderFormat`](#group__av_1ga909c7fab8e95be80ed78d8e847658183) | Sets the input format for encoding with this capture device. |
| [`onStreamStateChange`](#group__av_1ga1410d332fa091a13dd52ea799f7040de) | Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`emitter`](#group__av_1ga3a2c153ddf81cd8be57efe0a784a2339) |  |

---

#### ICapture 

```cpp
inline ICapture()
```

---

#### ~ICapture 

```cpp
virtual ~ICapture() = default
```

---

#### start 

```cpp
void start()
```

---

#### stop 

```cpp
void stop()
```

---

#### getEncoderFormat 

```cpp
void getEncoderFormat(Format & iformat)
```

Sets the input format for encoding with this capture device.

| Parameter | Type | Description |
|-----------|------|-------------|
| `iformat` | `Format &` |  |

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### emitter 

```cpp
PacketSignal emitter
```

## IEncoder 

> **Extends:** `icy::Stateful< EncoderState >`
> **Subclasses:** `icy::av::MultiplexEncoder`
> **Defined in:** `iencoder.h`

This is the abstract class for all encoders.

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__av_1ga0fca7b5900200f462d9cf6a9e2ee316e) |  |
| [`init`](#group__av_1ga3017292356e589b3d61adca4383c51a0) |  |
| [`uninit`](#group__av_1ga06670aeae682d931c9378d0932fbe26b) |  |
| [`options`](#group__av_1gabf392bf61c0d255ef49b358624d3442b) |  |
| [`isNone`](#group__av_1ga773da7e63fe6afbdf30ad268ee61e512) |  |
| [`isReady`](#group__av_1gae6f19cdf4e3c776bdef058997307f208) |  |
| [`isEncoding`](#group__av_1ga2450b61a8a20b293a783bab685424285) |  |
| [`isActive`](#group__av_1ga7bc5645b62a07a4249de5fdcfe24c5e9) |  |
| [`isStopped`](#group__av_1gaa306a4894a57b67f41cc73b68060da93) |  |
| [`isError`](#group__av_1gabf096b994cc26365bdc9f4edcfb1045e) |  |

---

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

---

#### init 

```cpp
void init()
```

---

#### uninit 

```cpp
void uninit()
```

---

#### options 

```cpp
EncoderOptions & options()
```

---

#### isNone 

```cpp
virtual inline bool isNone() const
```

---

#### isReady 

```cpp
virtual inline bool isReady() const
```

---

#### isEncoding 

```cpp
virtual inline bool isEncoding() const
```

---

#### isActive 

```cpp
virtual inline bool isActive() const
```

---

#### isStopped 

```cpp
virtual inline bool isStopped() const
```

---

#### isError 

```cpp
virtual inline bool isError() const
```

## MediaCapture 

> **Extends:** `icy::av::ICapture`, `icy::basic::Runnable`
> **Subclasses:** `icy::av::AudioCapture`, `icy::av::VideoCapture`
> **Defined in:** `mediacapture.h`

This class implements a cross platform audio, video, screen and video file capturer.

### Members

| Name | Description |
|------|-------------|
| [`MediaCapture`](#group__av_1gafe3b6e3b9e358dd47a87b15362135f24) |  |
| [`~MediaCapture`](#group__av_1ga929846316b65d2377adb4a6affca7659) |  |
| [`openFile`](#group__av_1gae8bdf5281fce64d2132c2fd6c88ade0a) |  |
| [`close`](#group__av_1ga6a17c4e168d20df0e1357440e1ac9ec7) |  |
| [`start`](#group__av_1ga6a92a82d5b340cf9e4ca8369912b46f9) |  |
| [`stop`](#group__av_1ga8dae2308cae6bb09ae2537290347d6e2) |  |
| [`run`](#group__av_1ga7378e32ee0427263f16f630d20fbd3c2) | The run method will be called by the asynchronous context. |
| [`getEncoderFormat`](#group__av_1ga1fb0cc809168b8a715fd7c4e27be5f83) | Sets the input format for encoding with this capture device. |
| [`getEncoderAudioCodec`](#group__av_1ga466938fb1a24ea4ab3baeab7cc705954) |  |
| [`getEncoderVideoCodec`](#group__av_1ga5dc8470b740cfd42b9c60ac939f941fd) |  |
| [`setLoopInput`](#group__av_1ga397516f5cd21c7bcc07bb314e52252c9) | Continuously loop the input file when set. |
| [`setLimitFramerate`](#group__av_1gab7f834018d8db0c7f2db902b0be2e1e1) | Limit playback to video FPS. |
| [`setRealtimePTS`](#group__av_1gaf13da9d9e9ac1bc0ff99e8071022d271) | Set to use realtime PTS calculation. This is preferred when sing live captures as FFmpeg provided values are not always reliable. |
| [`formatCtx`](#group__av_1ga53559a0c45deab65991e8ae08786cb93) |  |
| [`video`](#group__av_1ga7c2ac780ac1c5f72a40478a2027d4f87) |  |
| [`audio`](#group__av_1ga2aa4d7bcc8edab38efada2fe3727c743) |  |
| [`stopping`](#group__av_1gae1cc728c49af510b91d021ae45c81262) |  |
| [`error`](#group__av_1gaa2b86ac2156b751b325221df2c4478a5) |  |
| [`openStream`](#group__av_1ga7eb4a7f7ce3ebf15a2d35bb9381b39d8) | Open the underlying media stream. |
| [`emit`](#group__av_1gad6bbe16edf99985bff5c1dd011e4814a) |  |
| [`Closing`](#group__av_1ga91af846c0fa6c00c8374bd451a03b411) | Signals that the capture thread is closing. Careful, this signal is emitted from inside the tread contect. |
| [`_mutex`](#group__av_1gad45e032e1e5b0ab7beca04754f066da9) |  |
| [`_thread`](#group__av_1gaaa3bffce55a2cc216cca0e0fba602ea6) |  |
| [`_formatCtx`](#group__av_1ga42967c34d29b6cfc7bf1d5fe035a809a) |  |
| [`_video`](#group__av_1ga7de4163cf1686424121e60c492403ace) |  |
| [`_audio`](#group__av_1gacfd70dc3c73cb0d7597d5d2b3e9003fa) |  |
| [`_error`](#group__av_1gac459b88f02baf63fe81c8fb2e8c57bac) |  |
| [`_stopping`](#group__av_1gae6b0b5339281a6e31bbc7acc96862e6e) |  |
| [`_looping`](#group__av_1ga9235aad492dca034b3c788e5b1a755fe) |  |
| [`_realtime`](#group__av_1ga256d90ae10187e4310241397d44bc6e5) |  |
| [`_ratelimit`](#group__av_1ga28871c47b4adf3dda87648347661874c) |  |

---

#### MediaCapture 

```cpp
MediaCapture()
```

---

#### ~MediaCapture 

```cpp
virtual ~MediaCapture()
```

---

#### openFile 

```cpp
virtual void openFile(const std::string & file)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | `const std::string &` |  |

---

#### close 

```cpp
virtual void close()
```

---

#### start 

```cpp
virtual void start()
```

---

#### stop 

```cpp
virtual void stop()
```

---

#### run 

```cpp
virtual void run()
```

The run method will be called by the asynchronous context.

---

#### getEncoderFormat 

```cpp
virtual void getEncoderFormat(Format & iformat)
```

Sets the input format for encoding with this capture device.

| Parameter | Type | Description |
|-----------|------|-------------|
| `iformat` | `Format &` |  |

---

#### getEncoderAudioCodec 

```cpp
virtual void getEncoderAudioCodec(AudioCodec & params)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `params` | `AudioCodec &` |  |

---

#### getEncoderVideoCodec 

```cpp
virtual void getEncoderVideoCodec(VideoCodec & params)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `params` | `VideoCodec &` |  |

---

#### setLoopInput 

```cpp
void setLoopInput(bool flag)
```

Continuously loop the input file when set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### setLimitFramerate 

```cpp
void setLimitFramerate(bool flag)
```

Limit playback to video FPS.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### setRealtimePTS 

```cpp
void setRealtimePTS(bool flag)
```

Set to use realtime PTS calculation. This is preferred when sing live captures as FFmpeg provided values are not always reliable.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### formatCtx 

```cpp
AVFormatContext * formatCtx() const
```

---

#### video 

```cpp
VideoDecoder * video() const
```

---

#### audio 

```cpp
AudioDecoder * audio() const
```

---

#### stopping 

```cpp
bool stopping() const
```

---

#### error 

```cpp
std::string error() const
```

---

#### openStream 

```cpp
virtual void openStream(const std::string & filename, const AVInputFormat * inputFormat, AVDictionary ** formatParams)
```

Open the underlying media stream.

#### Parameters
* `filename` The file path or device name to open. 

* `inputFormat` The forced input format, or nullptr for auto-detect. 

* `formatParams` Optional format parameters; may be updated by FFmpeg on return.

| Parameter | Type | Description |
|-----------|------|-------------|
| `filename` | `const std::string &` |  |
| `inputFormat` | `const AVInputFormat *` |  |
| `formatParams` | `AVDictionary **` |  |

---

#### emit 

```cpp
virtual void emit(IPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### Closing 

```cpp
NullSignal Closing
```

Signals that the capture thread is closing. Careful, this signal is emitted from inside the tread contect.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _thread 

```cpp
Thread _thread
```

---

#### _formatCtx 

```cpp
AVFormatContext * _formatCtx
```

---

#### _video 

```cpp
VideoDecoder * _video
```

---

#### _audio 

```cpp
AudioDecoder * _audio
```

---

#### _error 

```cpp
std::string _error
```

---

#### _stopping 

```cpp
bool _stopping
```

---

#### _looping 

```cpp
bool _looping
```

---

#### _realtime 

```cpp
bool _realtime
```

---

#### _ratelimit 

```cpp
bool _ratelimit
```

## MultiplexEncoder 

> **Extends:** `icy::av::IEncoder`
> **Subclasses:** `icy::av::MultiplexPacketEncoder`
> **Defined in:** `multiplexencoder.h`

This class implements a multiplex audio and video encoder.

### Members

| Name | Description |
|------|-------------|
| [`MultiplexEncoder`](#group__av_1gaf1d51958f4658bb7e9a6c4511511113d) |  |
| [`~MultiplexEncoder`](#group__av_1gaa064d2da3d35888df805b8ddc40d81aa) |  |
| [`init`](#group__av_1gaf48b2533aa945cd87bd97a0217c6993a) |  |
| [`uninit`](#group__av_1gac7855959dca2bf0f4b2bdc7a05bb45e5) |  |
| [`cleanup`](#group__av_1ga8185447f1ab599d8365c664a7e435130) |  |
| [`createVideo`](#group__av_1ga606f3cddfc03b600609068f380c38ec0) |  |
| [`freeVideo`](#group__av_1ga897ed265470ca56689ee14a3555887bc) |  |
| [`encodeVideo`](#group__av_1ga5457536d13c0108453bbddc39a2679f2) | Encode a single video frame. All frame values must be set, such as size, pizel format and PTS. |
| [`encodeVideo`](#group__av_1gabe192b9a42278fcc8d411a146020d87b) | Encode a single interleaved video frame. If the frame time is specified it should be the microseconds offset since the start of the input stream. If no time is specified a realtime time value will be assigned to the frame. |
| [`encodeVideo`](#group__av_1gaf497ecc4ab62a57ef70c7e7d02ef7420) | Encode a single planar video frame. |
| [`createAudio`](#group__av_1ga00991a66b30b1b007222ef1c4a696888) |  |
| [`freeAudio`](#group__av_1gac4853d19572e00b170f5cc55ac0e8daa) |  |
| [`encodeAudio`](#group__av_1gae5bb6f95b38149df123992196244cd2c) | Encode a single interleaved audio frame. |
| [`encodeAudio`](#group__av_1ga3cf8da92ac5941f642ec3b14e0135db7) | Encode a single planar audio frame. |
| [`flush`](#group__av_1gae8f1dfdc41fbf65984ed8b50d238eb29) | Flush and beffered or queued packets. |
| [`options`](#group__av_1ga7f3750b53714ca2098d293c8996a7630) |  |
| [`video`](#group__av_1ga85053ffee452ba42df42b3640b816136) |  |
| [`audio`](#group__av_1ga8ddbbc27769231a406f91de07a527943) |  |
| [`writeOutputPacket`](#group__av_1ga37465535671ab007f75646a7591fdc55) |  |
| [`updateStreamPts`](#group__av_1ga95dfd99fecd4cfe2e7b95e72cc298f42) | Convert input microseconds to the stream time base. |
| [`onVideoEncoded`](#group__av_1gab889e4258c74a7f7abf67e7494e7da3f) |  |
| [`onAudioEncoded`](#group__av_1gae4cea840999e5b11bff4f7d4f344590a) |  |
| [`emitter`](#group__av_1gaf593061a8871958488243581b9c837b3) |  |
| [`_options`](#group__av_1ga0f8d38890c1f48b4ebadc58106d41b3b) |  |
| [`_formatCtx`](#group__av_1ga3ad98660fb4df5e33fd76c69f5638c16) |  |
| [`_video`](#group__av_1gacb2c7d2ffc3ddc017acf99e95e8b9795) |  |
| [`_audio`](#group__av_1ga57ee884acf9e4e908263270fc7d74512) |  |
| [`_ioCtx`](#group__av_1ga9a948562b539af9cd0fa8a37a3c3a7d7) |  |
| [`_ioBuffer`](#group__av_1ga6198a189e2a84da5942e9e901bd060e5) |  |
| [`_pts`](#group__av_1ga71b53416c479a726dd69f5843f102979) |  |
| [`_mutex`](#group__av_1ga679e4f526d3f6c2cf713f1510bef1537) |  |

---

#### MultiplexEncoder 

```cpp
MultiplexEncoder(const EncoderOptions & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `const EncoderOptions &` |  |

---

#### ~MultiplexEncoder 

```cpp
virtual ~MultiplexEncoder()
```

---

#### init 

```cpp
virtual void init()
```

---

#### uninit 

```cpp
virtual void uninit()
```

---

#### cleanup 

```cpp
virtual void cleanup()
```

---

#### createVideo 

```cpp
virtual void createVideo()
```

---

#### freeVideo 

```cpp
virtual void freeVideo()
```

---

#### encodeVideo 

```cpp
virtual bool encodeVideo(AVFrame * frame)
```

Encode a single video frame. All frame values must be set, such as size, pizel format and PTS.

| Parameter | Type | Description |
|-----------|------|-------------|
| `frame` | `AVFrame *` |  |

---

#### encodeVideo 

```cpp
virtual bool encodeVideo(uint8_t * buffer, int bufferSize, int width, int height, int64_t time)
```

Encode a single interleaved video frame. If the frame time is specified it should be the microseconds offset since the start of the input stream. If no time is specified a realtime time value will be assigned to the frame.

#### Parameters
* `buffer` The raw video frame buffer. 

* `bufferSize` The buffer size in bytes. 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` |  |
| `bufferSize` | `int` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `time` | `int64_t` |  |

---

#### encodeVideo 

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

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `linesize` | `int` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `time` | `int64_t` |  |

---

#### createAudio 

```cpp
virtual void createAudio()
```

---

#### freeAudio 

```cpp
virtual void freeAudio()
```

---

#### encodeAudio 

```cpp
virtual bool encodeAudio(uint8_t * buffer, int numSamples, int64_t time)
```

Encode a single interleaved audio frame.

#### Parameters
* `buffer` The interleaved audio sample buffer. 

* `numSamples` The number of samples per channel. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` |  |
| `numSamples` | `int` |  |
| `time` | `int64_t` |  |

---

#### encodeAudio 

```cpp
virtual bool encodeAudio(uint8_t * data, int numSamples, int64_t time)
```

Encode a single planar audio frame.

#### Parameters
* `data` Array of per-plane sample buffers (one per channel). 

* `numSamples` The number of samples per channel. 

* `time` The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `numSamples` | `int` |  |
| `time` | `int64_t` |  |

---

#### flush 

```cpp
virtual void flush()
```

Flush and beffered or queued packets.

---

#### options 

```cpp
virtual EncoderOptions & options()
```

---

#### video 

```cpp
VideoEncoder * video()
```

---

#### audio 

```cpp
AudioEncoder * audio()
```

---

#### writeOutputPacket 

```cpp
bool writeOutputPacket(AVPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `AVPacket &` |  |

---

#### updateStreamPts 

```cpp
bool updateStreamPts(AVStream * stream, int64_t * pts)
```

Convert input microseconds to the stream time base.

#### Parameters
* `stream` The target stream for time base conversion. 

* `pts` Pointer to the timestamp; converted in place.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `AVStream *` |  |
| `pts` | `int64_t *` |  |

---

#### onVideoEncoded 

```cpp
void onVideoEncoded(av::VideoPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `av::VideoPacket &` |  |

---

#### onAudioEncoded 

```cpp
void onAudioEncoded(av::AudioPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `av::AudioPacket &` |  |

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### _options 

```cpp
EncoderOptions _options
```

---

#### _formatCtx 

```cpp
AVFormatContext * _formatCtx
```

---

#### _video 

```cpp
VideoEncoder * _video
```

---

#### _audio 

```cpp
AudioEncoder * _audio
```

---

#### _ioCtx 

```cpp
AVIOContext * _ioCtx
```

---

#### _ioBuffer 

```cpp
uint8_t * _ioBuffer
```

---

#### _pts 

```cpp
int64_t _pts
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## MultiplexPacketEncoder 

> **Extends:** `icy::av::MultiplexEncoder`, `icy::PacketProcessor`
> **Defined in:** `multiplexpacketencoder.h`

Encodes and multiplexes a realtime video stream form audio / video capture sources. FFmpeg is used for encoding.

### Members

| Name | Description |
|------|-------------|
| [`MultiplexPacketEncoder`](#group__av_1ga7594862654f7c36da468484365d21d35) |  |
| [`~MultiplexPacketEncoder`](#group__av_1ga7dbc839717bf8a252752ae80c663da1d) |  |
| [`encode`](#group__av_1gacb056692a6756d361fbc5a73df5378d2) |  |
| [`encode`](#group__av_1ga7db54ef667441a2c57a91193f7ed1b8a) |  |
| [`accepts`](#group__av_1ga27b350c01a4362cfde30ffd884a5e34c) | This method ensures compatibility with the given packet type. Return false to reject the packet. |
| [`process`](#group__av_1ga993f962cb2fa800af7c2902e474196f1) | This method performs processing on the given packet and emits the result. |
| [`onStreamStateChange`](#group__av_1ga8d0ee54bb9b2e3bfff877dcf9aef8b26) | Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`_mutex`](#group__av_1ga41b95d96e26fa281735483efd3955956) |  |

---

#### MultiplexPacketEncoder 

```cpp
MultiplexPacketEncoder(const EncoderOptions & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `const EncoderOptions &` |  |

---

#### ~MultiplexPacketEncoder 

```cpp
virtual ~MultiplexPacketEncoder()
```

---

#### encode 

```cpp
virtual void encode(VideoPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `VideoPacket &` |  |

---

#### encode 

```cpp
virtual void encode(AudioPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `AudioPacket &` |  |

---

#### accepts 

```cpp
virtual bool accepts(IPacket *)
```

This method ensures compatibility with the given packet type. Return false to reject the packet.

---

#### process 

```cpp
virtual void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](./doc/api-base.md#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### onStreamStateChange 

```cpp
virtual void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### _mutex 

```cpp
std::mutex _mutex
```

## RealtimePacketQueue 

> **Extends:** `icy::AsyncPacketQueue< PacketT >`
> **Defined in:** `realtimepacketqueue.h`

This class emits media packets based on their realtime pts value.

### Members

| Name | Description |
|------|-------------|
| [`RealtimePacketQueue`](#group__av_1ga2fb259d001893d803875ed5d0c34ef31) |  |
| [`~RealtimePacketQueue`](#group__av_1ga72e0189b06eaa7b0a1d142adfc39b4cd) |  |
| [`push`](#group__av_1ga139e6f5a0132f139ad53f478c811cea5) |  |
| [`realTime`](#group__av_1gae25eec661e079ffaf5377ceaf5f74619) |  |
| [`popNext`](#group__av_1gae94830422713dfb9b8c18e5e4ebdc758) | Pops the next waiting item. |
| [`onStreamStateChange`](#group__av_1gaa323b0bd9935c88d9c5abe9109245a15) | Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`_startTime`](#group__av_1gaf638e19a348745002a5c13c26dfea851) |  |

---

#### RealtimePacketQueue 

```cpp
inline RealtimePacketQueue(int maxSize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `maxSize` | `int` |  |

---

#### ~RealtimePacketQueue 

```cpp
virtual inline ~RealtimePacketQueue()
```

---

#### push 

```cpp
virtual inline void push(PacketT * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `PacketT *` |  |

---

#### realTime 

```cpp
inline int64_t realTime()
```

---

#### popNext 

```cpp
virtual inline PacketT * popNext()
```

Pops the next waiting item.

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](./doc/api-base.md#classicy_1_1PacketStream) to notify when the internal [Stream](./doc/api-base.md#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### _startTime 

```cpp
int64_t _startTime
```

## MediaPacketTimeCompare 

> **Defined in:** `realtimepacketqueue.h`

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__av_1gaae0732616533ecf4d9869a7751eae614) |  |

---

#### operator() 

```cpp
inline bool operator()(const MediaPacket * a, const MediaPacket * b)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `a` | `const MediaPacket *` |  |
| `b` | `const MediaPacket *` |  |

## VideoCapture 

> **Extends:** `icy::av::MediaCapture`
> **Defined in:** `videocapture.h`

This class implements a cross platform audio capturer.

### Members

| Name | Description |
|------|-------------|
| [`VideoCapture`](#group__av_1ga01e25e9c7ad589b2868861d40b6c2e0e) |  |
| [`VideoCapture`](#group__av_1gadffd637bc51d2f530b77250eb68a3007) |  |
| [`VideoCapture`](#group__av_1ga9a1818c1388b213e163eee6ec4d9da72) |  |
| [`~VideoCapture`](#group__av_1ga2dbd2571544143012cc8927236c3ebdf) |  |
| [`openVideo`](#group__av_1ga780a0453f9f44df2f2c56a7451dd9789) |  |
| [`openVideo`](#group__av_1ga6e06d59e636d2506dc2c72f36bd6959a) |  |

---

#### VideoCapture 

```cpp
VideoCapture()
```

---

#### VideoCapture 

```cpp
VideoCapture(const std::string & device, const av::VideoCodec & params)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `params` | `const av::VideoCodec &` |  |

---

#### VideoCapture 

```cpp
VideoCapture(const std::string & device, int width, int height, double framerate, const std::string & pixelFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `framerate` | `double` |  |
| `pixelFmt` | `const std::string &` |  |

---

#### ~VideoCapture 

```cpp
virtual ~VideoCapture()
```

---

#### openVideo 

```cpp
virtual void openVideo(const std::string & device, const av::VideoCodec & params)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `params` | `const av::VideoCodec &` |  |

---

#### openVideo 

```cpp
virtual void openVideo(const std::string & device, int width, int height, double framerate, const std::string & pixelFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `device` | `const std::string &` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `framerate` | `double` |  |
| `pixelFmt` | `const std::string &` |  |

## Device 

> **Defined in:** `devicemanager.h`

Represents a system audio, video or render device.

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__av_1gab88aea620eee238d583c96974d871c15) |  |
| [`Device`](#group__av_1ga53ea1e4acdb14bfacf39b439fbee83fb) |  |
| [`Device`](#group__av_1ga31dad4b6f0da95ac54d49666db8f8377) |  |
| [`print`](#group__av_1ga212e9032eac9a85662e69714b9a2d349) |  |
| [`operator==`](#group__av_1ga686faeba93e2f49350e5d317dab86a2c) |  |
| [`Device`](#group__av_1ga69f2c24d58a00f3d4f69bc468d8ad721) |  |
| [`type`](#group__av_1ga0844c85f6319f6213ea026ca35732e1f) |  |
| [`id`](#group__av_1gab7545dfaea8360e3bd2c071e5906e82e) |  |
| [`name`](#group__av_1ga2aa878bff3aa5e36fc6f904eb36d9ddd) |  |
| [`guid`](#group__av_1ga000efaabb0352c7d86870e649e2131c8) |  |
| [`isDefault`](#group__av_1gabf1ce9150645f511dd0b78dfbcc83522) |  |
| [`isAvailable`](#group__av_1ga1e1cd71321a4a5bf06474398eb14a39f) |  |
| [`type`](#group__av_1ga5895253619f5d138ac222469a70b81cb) |  |
| [`id`](#group__av_1gac2a5481204b6eb660c3116fae797de7a) |  |
| [`Device`](#structicy_1_1av_1_1Device_1ga53ea1e4acdb14bfacf39b439fbee83fb) |  |
| [`print`](#structicy_1_1av_1_1Device_1ga212e9032eac9a85662e69714b9a2d349) |  |
| [`operator==`](#structicy_1_1av_1_1Device_1ga686faeba93e2f49350e5d317dab86a2c) |  |

---

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

---

#### Device 

```cpp
Device()
```

---

#### Device 

```cpp
Device(const std::string & type, int id, const std::string & name, const std::string & guid, bool isDefault, bool isAvailable)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `id` | `int` |  |
| `name` | `const std::string &` |  |
| `guid` | `const std::string &` |  |
| `isDefault` | `bool` |  |
| `isAvailable` | `bool` |  |

---

#### print 

```cpp
void print(std::ostream & os)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### operator== 

```cpp
inline bool operator==(const Device & that) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const Device &` |  |

---

#### Device 

```cpp
Device(Type type, const std::string & id, const std::string & name, bool isDefault)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `Type` |  |
| `id` | `const std::string &` |  |
| `name` | `const std::string &` |  |
| `isDefault` | `bool` |  |

---

#### type 

```cpp
std::string type
```

---

#### id 

```cpp
int id
```

---

#### name 

```cpp
std::string name
```

---

#### guid 

```cpp
std::string guid
```

---

#### isDefault 

```cpp
bool isDefault
```

---

#### isAvailable 

```cpp
bool isAvailable
```

---

#### type 

```cpp
Type type
```

---

#### id 

```cpp
std::string id
```

---

#### Device 

```cpp
Device()
```

---

#### print 

```cpp
void print(std::ostream & os)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### operator== 

```cpp
inline bool operator==(const [Device](#structicy_1_1av_1_1Device) & that) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const [Device](#structicy_1_1av_1_1Device) &` |  |

## Options 

> **Defined in:** `videoanalyzer.h`

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__av_1ga0610fd6f6ea8ccc982764e198c4191df) |  |
| [`ifile`](#group__av_1ga0588f37bc4a07997c5b52aad94488177) |  |
| [`rdftSize`](#group__av_1ga3930c2df8a401e1b6a6cb65d824a1f37) |  |

---

#### Options 

```cpp
inline Options()
```

---

#### ifile 

```cpp
std::string ifile
```

---

#### rdftSize 

```cpp
int rdftSize
```

## Stream 

> **Defined in:** `videoanalyzer.h`

### Members

| Name | Description |
|------|-------------|
| [`Stream`](#group__av_1ga4b11e34ec10c49f9dcdf30a25b2bd5d2) |  |
| [`~Stream`](#group__av_1ga523d33f3555c415b63ca428744f814c7) |  |
| [`initialize`](#group__av_1gad2da7bdf2d9164a40e83b4cc0acc7a82) |  |
| [`uninitialize`](#group__av_1gac92af80fd4eb8ff92e4bab9608c29dc3) |  |
| [`fft`](#group__av_1ga67a9547ab771c61d603086a5dd922b0f) |  |
| [`name`](#group__av_1ga0273e4f2e768d0daa30719dcf97a9a66) |  |
| [`rdft`](#group__av_1ga1c501d438007311a16dfb1d93572f540) |  |
| [`rdftData`](#group__av_1ga39711bf7e95a4b07f43c3934643fa930) |  |
| [`rdftSize`](#group__av_1gacca84a21ae498fcb9035ad6cb33b9f2b) |  |
| [`rdftBits`](#group__av_1ga2979c4b96e03f235536ea6ee7b8a230c) |  |
| [`frames`](#group__av_1ga96a21cba5b0e1fbb07eb3dc4236f414f) |  |
| [`filled`](#group__av_1gace58db739f0c0dcc61db800cafa2e45e) |  |

---

#### Stream 

```cpp
Stream(const std::string & name, int rdftSize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `rdftSize` | `int` |  |

---

#### ~Stream 

```cpp
~Stream()
```

---

#### initialize 

```cpp
void initialize()
```

---

#### uninitialize 

```cpp
void uninitialize()
```

---

#### fft 

```cpp
void fft()
```

---

#### name 

```cpp
std::string name
```

---

#### rdft 

```cpp
RDFTContext * rdft
```

---

#### rdftData 

```cpp
FFTSample * rdftData
```

---

#### rdftSize 

```cpp
int rdftSize
```

---

#### rdftBits 

```cpp
int rdftBits
```

---

#### frames 

```cpp
std::int64_t frames
```

---

#### filled 

```cpp
int filled
```

## Packet 

> **Defined in:** `videoanalyzer.h`

### Members

| Name | Description |
|------|-------------|
| [`Packet`](#group__av_1gad1482a53678eb45b010543da58fd75d3) |  |
| [`time`](#group__av_1gaf5901e114c3cc04e690f91d4fb7b0b6b) |  |
| [`value`](#group__av_1ga35702a4a9df499a661bdf48c5f0a8921) |  |

---

#### Packet 

```cpp
Packet(double time, double value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | `double` |  |
| `value` | `double` |  |

---

#### time 

```cpp
double time
```

---

#### value 

```cpp
double value
```

## AudioBuffer 

> **Defined in:** `audiobuffer.h`

### Members

| Name | Description |
|------|-------------|
| [`AudioBuffer`](#group__av_1ga54e24e704368e437595732ec4d787b66) |  |
| [`~AudioBuffer`](#group__av_1ga183f5848e3216a98acbc8f98b1d75c5b) |  |
| [`alloc`](#group__av_1ga50782855bc3552a230530af683a5b1d1) | Allocate the audio FIFO buffer. |
| [`reset`](#group__av_1ga2c58be0d24a1523ba648932a75bb7ae4) |  |
| [`close`](#group__av_1ga3d5266c893d39885a4d2cbdf6ba5f3e2) |  |
| [`write`](#group__av_1gacf729dae184fbf799a2d87aabea490f5) | Write samples into the FIFO buffer. |
| [`read`](#group__av_1ga3aa2daaba75aae7d54585d6fad29b8f4) | Read samples from the FIFO buffer. |
| [`available`](#group__av_1ga84628a7fca7c64327f9d3fcf52467954) |  |
| [`fifo`](#group__av_1gae438b0740faf59aff51b83e8e140a026) |  |

---

#### AudioBuffer 

```cpp
AudioBuffer()
```

---

#### ~AudioBuffer 

```cpp
~AudioBuffer()
```

---

#### alloc 

```cpp
void alloc(const std::string & sampleFmt, int channels, int numSamples)
```

Allocate the audio FIFO buffer.

#### Parameters
* `sampleFmt` The sample format name (e.g. "s16", "fltp"). 

* `channels` The number of audio channels. 

* `numSamples` The initial buffer capacity in samples per channel.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sampleFmt` | `const std::string &` |  |
| `channels` | `int` |  |
| `numSamples` | `int` |  |

---

#### reset 

```cpp
void reset()
```

---

#### close 

```cpp
void close()
```

---

#### write 

```cpp
void write(void ** samples, int numSamples)
```

Write samples into the FIFO buffer.

#### Parameters
* `samples` Array of per-channel sample buffers. 

* `numSamples` The number of samples per channel to write.

| Parameter | Type | Description |
|-----------|------|-------------|
| `samples` | `void **` |  |
| `numSamples` | `int` |  |

---

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

| Parameter | Type | Description |
|-----------|------|-------------|
| `samples` | `void **` |  |
| `numSamples` | `int` |  |

---

#### available 

```cpp
int available() const
```

---

#### fifo 

```cpp
AVAudioFifo * fifo
```

## AudioContext 

> **Subclasses:** `icy::av::AudioDecoder`, `icy::av::AudioEncoder`
> **Defined in:** `audiocontext.h`

### Members

| Name | Description |
|------|-------------|
| [`AudioContext`](#group__av_1gae8e7d079f730e9b536bd874b835aa185) |  |
| [`~AudioContext`](#group__av_1ga1a8c4d6264230eeebb4c700c673f06c5) |  |
| [`create`](#group__av_1ga0fb23e6630fb7fbc72d2052f41f893c9) | Initialize the `AVCodecContext` with default values. |
| [`open`](#group__av_1gae6b206d9ba8c7fac839c061a2c014e92) | Open the AVCodecContext. |
| [`close`](#group__av_1gaef855c8fd3ca00b2ca309383106d86f4) | Close the AVCodecContext. |
| [`recreateResampler`](#group__av_1ga5c013628754b96eca69d6f018af884d7) | Create the audio resampling context. |
| [`emitter`](#group__av_1ga053c8542ccd8bf42d37203d910f849ca) |  |
| [`iparams`](#group__av_1ga066bc978752208f55e97f8df22af7cbf) | input parameters |
| [`oparams`](#group__av_1ga6768d9c0ddfdd36aecabeeb9ac16bba4) | output parameters |
| [`stream`](#group__av_1ga581e1f8ee20e84030fcc4386ccee52f9) | encoder or decoder stream |
| [`ctx`](#group__av_1gaebaea3fe2880fc7532291d9a3b6973a5) | encoder or decoder context |
| [`codec`](#group__av_1ga678094612d9636c92a5eed82ee8eeed3) | encoder or decoder codec |
| [`frame`](#group__av_1ga3fbc59cc52dd1afe38fa668ce744fed3) | last encoded or decoded frame |
| [`resampler`](#group__av_1ga86e526926949fc6c714e3a497a4fd146) | audio resampler |
| [`outputFrameSize`](#group__av_1ga159d8eedc606078055df3d368157f048) | encoder or decoder output frame size |
| [`time`](#group__av_1gaf1201cde2d8461b486a7b7684e4e923f) | stream time in codec time base |
| [`pts`](#group__av_1gacc84d8066a6154bdfb5883fa7c13e0f6) | last packet pts value |
| [`seconds`](#group__av_1gacd28170140313e31de9d73129b5f8d54) | audio time in seconds |
| [`error`](#group__av_1ga9c0b887f963fb41bb91620ca2dcf9a0f) | error message |

---

#### AudioContext 

```cpp
AudioContext()
```

---

#### ~AudioContext 

```cpp
virtual ~AudioContext()
```

---

#### create 

```cpp
void create()
```

Initialize the `AVCodecContext` with default values.

---

#### open 

```cpp
virtual void open()
```

Open the AVCodecContext.

---

#### close 

```cpp
virtual void close()
```

Close the AVCodecContext.

---

#### recreateResampler 

```cpp
virtual bool recreateResampler()
```

Create the audio resampling context.

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### iparams 

```cpp
AudioCodec iparams
```

input parameters

---

#### oparams 

```cpp
AudioCodec oparams
```

output parameters

---

#### stream 

```cpp
AVStream * stream
```

encoder or decoder stream

---

#### ctx 

```cpp
AVCodecContext * ctx
```

encoder or decoder context

---

#### codec 

```cpp
const AVCodec * codec
```

encoder or decoder codec

---

#### frame 

```cpp
AVFrame * frame
```

last encoded or decoded frame

---

#### resampler 

```cpp
std::unique_ptr< AudioResampler > resampler
```

audio resampler

---

#### outputFrameSize 

```cpp
int outputFrameSize
```

encoder or decoder output frame size

---

#### time 

```cpp
int64_t time
```

stream time in codec time base

---

#### pts 

```cpp
int64_t pts
```

last packet pts value

---

#### seconds 

```cpp
double seconds
```

audio time in seconds

---

#### error 

```cpp
std::string error
```

error message

## AudioDecoder 

> **Extends:** `icy::av::AudioContext`
> **Defined in:** `audiodecoder.h`

### Members

| Name | Description |
|------|-------------|
| [`AudioDecoder`](#group__av_1ga7ee29a239fffcde30f03ae74f409a941) |  |
| [`~AudioDecoder`](#group__av_1gacf2b2e8976e564ad308cf5fa291bd4ca) |  |
| [`create`](#group__av_1gac8d6012d59e2fdca98d88299ded32c0b) | Initialize the `AVCodecContext` with default values. |
| [`close`](#group__av_1gaf48be78d1c755a5b01240a604bb7c845) | Close the AVCodecContext. |
| [`decode`](#group__av_1ga1efd688cf09b41b183e18dd7c43e31ea) | Decodes a the given input packet. Returns true an output packet was created, false otherwise. |
| [`flush`](#group__av_1ga890c75160dd7216e8b531fec26fb9c1b) | Flushes buffered frames. This method should be called once after decoding. |

---

#### AudioDecoder 

```cpp
AudioDecoder(AVStream * stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `AVStream *` |  |

---

#### ~AudioDecoder 

```cpp
virtual ~AudioDecoder()
```

---

#### create 

```cpp
virtual void create()
```

Initialize the `AVCodecContext` with default values.

---

#### close 

```cpp
virtual void close()
```

Close the AVCodecContext.

---

#### decode 

```cpp
virtual bool decode(AVPacket & ipacket)
```

Decodes a the given input packet. Returns true an output packet was created, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ipacket` | `AVPacket &` |  |

---

#### flush 

```cpp
virtual void flush()
```

Flushes buffered frames. This method should be called once after decoding.

## AudioEncoder 

> **Extends:** `icy::av::AudioContext`
> **Defined in:** `audioencoder.h`

### Members

| Name | Description |
|------|-------------|
| [`AudioEncoder`](#group__av_1ga02819ded2a5745e255cc84b72abc8117) |  |
| [`~AudioEncoder`](#group__av_1ga11cee62e83aa74840d58b098cea1d8fd) |  |
| [`create`](#group__av_1ga2fc0a6bfdf50c6a244eb81e755c4ca98) | Initialize the `AVCodecContext` with default values. |
| [`close`](#group__av_1ga5bf457ec1389fb3f57f617fce3b70062) | Close the AVCodecContext. |
| [`encode`](#group__av_1ga2764e5766d3983744099fd244f119fdb) | Encode interleaved audio samples. |
| [`encode`](#group__av_1ga4d3190bfcc040c5ce01074b09ec3627b) | Encode planar audio samples. |
| [`encode`](#group__av_1ga1876deb14c7a94731f0482b707b1b652) | Encode a single AVFrame from the decoder. |
| [`flush`](#group__av_1gaa24448659b5df37e3f096660e4853164) | Flush remaining packets to be encoded. This method should be called once before stream closure. |
| [`fifo`](#group__av_1gaf230c718da64fb580621c0ff7f4ae69c) |  |
| [`format`](#group__av_1ga5401890103e86f78a2b37e428d63a1bd) |  |

---

#### AudioEncoder 

```cpp
AudioEncoder(AVFormatContext * format)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `format` | `AVFormatContext *` |  |

---

#### ~AudioEncoder 

```cpp
virtual ~AudioEncoder()
```

---

#### create 

```cpp
virtual void create()
```

Initialize the `AVCodecContext` with default values.

---

#### close 

```cpp
virtual void close()
```

Close the AVCodecContext.

---

#### encode 

```cpp
virtual bool encode(uint8_t * samples, const int numSamples, const int64_t pts)
```

Encode interleaved audio samples.

#### Parameters
* `samples` The input samples to encode. 

* `numSamples` The number of input samples per channel. 

* `pts` The input samples presentation timestamp. 

* `opacket` The output packet data will be encoded to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `samples` | `uint8_t *` |  |
| `numSamples` | `const int` |  |
| `pts` | `const int64_t` |  |

---

#### encode 

```cpp
virtual bool encode(uint8_t * samples, const int numSamples, const int64_t pts)
```

Encode planar audio samples.

#### Parameters
* `samples` The input samples to encode. 

* `numSamples` The number of input samples per channel. 

* `pts` The input samples presentation timestamp. 

* `opacket` The output packet data will be encoded to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `samples` | `uint8_t *` |  |
| `numSamples` | `const int` |  |
| `pts` | `const int64_t` |  |

---

#### encode 

```cpp
virtual bool encode(AVFrame * iframe)
```

Encode a single AVFrame from the decoder.

| Parameter | Type | Description |
|-----------|------|-------------|
| `iframe` | `AVFrame *` |  |

---

#### flush 

```cpp
virtual void flush()
```

Flush remaining packets to be encoded. This method should be called once before stream closure.

---

#### fifo 

```cpp
av::AudioBuffer fifo
```

---

#### format 

```cpp
AVFormatContext * format
```

## AudioResampler 

> **Defined in:** `audioresampler.h`

### Members

| Name | Description |
|------|-------------|
| [`AudioResampler`](#group__av_1gac1e2b031ed1c126010aa3c0ce96f4a59) |  |
| [`~AudioResampler`](#group__av_1ga838aa389dcc162d2d61aa595f61568c9) |  |
| [`open`](#group__av_1ga4ab47b82613b5d7d47a3880b350b1b5d) |  |
| [`close`](#group__av_1ga01bbe4d56a06dbfaaf11de4647b8bd81) |  |
| [`resample`](#group__av_1ga1756cc112e2502191d9c20a54273d9c5) | Convert the input samples to the output format. NOTE: Input buffers must be contiguous, therefore only interleaved input formats are accepted at this point. |
| [`ctx`](#group__av_1ga65a5c08ebb05b0790edc9c953298a4be) | the conversion context |
| [`iparams`](#group__av_1ga1222d77faf8e8d8f234bba69cf9fb9ad) | input audio parameters |
| [`oparams`](#group__av_1ga464b81f63fad692ecdfd72fb5c84c56c) | output audio parameters |
| [`outSamples`](#group__av_1ga67e76d39e36500f28b1d20753cbee2f0) | the output samples buffer |
| [`outNumSamples`](#group__av_1ga1fc4193f2a49fa6b33319f371015bffc) | the number of samples currently in the output buffer |
| [`outBufferSize`](#group__av_1gadcdf08a8cd007d67f271b924cf5dec98) | the number of bytes currently in the buffer |
| [`maxNumSamples`](#group__av_1ga47217cebf4e6262730fa223066a3a472) | the maximum number of samples that can be stored in |
| [`inSampleFmt`](#group__av_1gaac653caa68acf8f87d7e59cac9ca48b1) | input sample format |
| [`outSampleFmt`](#group__av_1ga01811a58ed47e08fce724b7a1e197ce6) | output sample format |

---

#### AudioResampler 

```cpp
AudioResampler(const AudioCodec & iparams, const AudioCodec & oparams)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `iparams` | `const AudioCodec &` |  |
| `oparams` | `const AudioCodec &` |  |

---

#### ~AudioResampler 

```cpp
~AudioResampler()
```

---

#### open 

```cpp
void open()
```

---

#### close 

```cpp
void close()
```

---

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

| Parameter | Type | Description |
|-----------|------|-------------|
| `inSamples` | `uint8_t **` |  |
| `inNumSamples` | `int` |  |

---

#### ctx 

```cpp
SwrContext * ctx
```

the conversion context

---

#### iparams 

```cpp
AudioCodec iparams
```

input audio parameters

---

#### oparams 

```cpp
AudioCodec oparams
```

output audio parameters

---

#### outSamples 

```cpp
uint8_t ** outSamples
```

the output samples buffer

---

#### outNumSamples 

```cpp
int outNumSamples
```

the number of samples currently in the output buffer

---

#### outBufferSize 

```cpp
int outBufferSize
```

the number of bytes currently in the buffer

---

#### maxNumSamples 

```cpp
int maxNumSamples
```

the maximum number of samples that can be stored in

---

#### inSampleFmt 

```cpp
enum AVSampleFormat inSampleFmt
```

input sample format

---

#### outSampleFmt 

```cpp
enum AVSampleFormat outSampleFmt
```

output sample format

## Codec 

> **Subclasses:** `icy::av::AudioCodec`, `icy::av::VideoCodec`
> **Defined in:** `codec.h`

[Codec](#structicy_1_1av_1_1Codec) for encoding/decoding media.

### Members

| Name | Description |
|------|-------------|
| [`Codec`](#group__av_1ga8b9bea5abd6c627618a66f5bb0cacdda) |  |
| [`Codec`](#group__av_1ga7e7561a995faa17643c97a585f92f042) |  |
| [`Codec`](#group__av_1ga168efa478447f30e8f6c4921016335f6) |  |
| [`~Codec`](#group__av_1gad5850bf5554a8187668d0480d340f01d) | Codec(const Codec& r);. |
| [`toString`](#group__av_1ga8b8296b25c3b401f87f21090fd8de249) |  |
| [`print`](#group__av_1gadc2b86e08e23d3201f0c3430ae795f5d) |  |
| [`name`](#group__av_1ga6844966d0cc504e1a19d138e4787bc26) | The display name for this codec. |
| [`encoder`](#group__av_1ga5552a927640813c68b25c07c89786f79) | The encoder name for FFmpeg. |
| [`sampleRate`](#group__av_1gad3b634766c2f3458c07803c6d9036b47) | The sampling rate or RTP clock rate. |
| [`bitRate`](#group__av_1ga1600230a934a25fb9d280681e16b0b98) | The bit rate to encode at. |
| [`quality`](#group__av_1gab28adf6739a1653c39cd6b004f9f3e8f) | Optional quality value, variable range depending on codec. |
| [`enabled`](#group__av_1gab6735b630af06c573029766f5944be93) | Weather or not the codec is available for use. |

---

#### Codec 

```cpp
Codec()
```

---

#### Codec 

```cpp
Codec(const std::string & name, int sampleRate, int bitRate, bool enabled)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `sampleRate` | `int` |  |
| `bitRate` | `int` |  |
| `enabled` | `bool` |  |

---

#### Codec 

```cpp
Codec(const std::string & name, const std::string & encoder, int sampleRate, int bitRate, bool enabled)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `encoder` | `const std::string &` |  |
| `sampleRate` | `int` |  |
| `bitRate` | `int` |  |
| `enabled` | `bool` |  |

---

#### ~Codec 

```cpp
virtual ~Codec()
```

Codec(const Codec& r);.

---

#### toString 

```cpp
virtual std::string toString() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### name 

```cpp
std::string name
```

The display name for this codec.

---

#### encoder 

```cpp
std::string encoder
```

The encoder name for FFmpeg.

---

#### sampleRate 

```cpp
int sampleRate
```

The sampling rate or RTP clock rate.

---

#### bitRate 

```cpp
int bitRate
```

The bit rate to encode at.

---

#### quality 

```cpp
int quality
```

Optional quality value, variable range depending on codec.

---

#### enabled 

```cpp
bool enabled
```

Weather or not the codec is available for use.

## AudioCodec 

> **Extends:** `icy::av::Codec`
> **Defined in:** `codec.h`

### Members

| Name | Description |
|------|-------------|
| [`AudioCodec`](#group__av_1gac7015ec1976fd68879c0af8e22c14de0) |  |
| [`AudioCodec`](#group__av_1ga57d52948e2e4de60591a72c6753823aa) |  |
| [`AudioCodec`](#group__av_1ga3c0da0a206873c832f0f7fdd83ac4ca4) |  |
| [`AudioCodec`](#group__av_1gafac2a4dbaf98eec7a3a09184ecee2af1) |  |
| [`~AudioCodec`](#group__av_1gacef753a31b9526ed27724a3fa9edf9b5) |  |
| [`toString`](#group__av_1ga1ac4331d3b1f81838009713165ed4f00) |  |
| [`print`](#group__av_1gabc909bb0d48bc67ad06763fbad49c13d) |  |
| [`channels`](#group__av_1ga6127b16a7d5e1329e88e124366015071) |  |
| [`sampleFmt`](#group__av_1ga4f3070e09c5d36971189f42402984670) | One of: u8, s16, s32, flt, dbl, u8p, s16p, s32p, fltp, dblp. |

---

#### AudioCodec 

```cpp
AudioCodec()
```

---

#### AudioCodec 

```cpp
AudioCodec(int channels, int sampleRate, const std::string & sampleFmt, int bitRate)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `channels` | `int` |  |
| `sampleRate` | `int` |  |
| `sampleFmt` | `const std::string &` |  |
| `bitRate` | `int` |  |

---

#### AudioCodec 

```cpp
AudioCodec(const std::string & name, int channels, int sampleRate, int bitRate, const std::string & sampleFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `channels` | `int` |  |
| `sampleRate` | `int` |  |
| `bitRate` | `int` |  |
| `sampleFmt` | `const std::string &` |  |

---

#### AudioCodec 

```cpp
AudioCodec(const std::string & name, const std::string & encoder, int channels, int sampleRate, int bitRate, const std::string & sampleFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `encoder` | `const std::string &` |  |
| `channels` | `int` |  |
| `sampleRate` | `int` |  |
| `bitRate` | `int` |  |
| `sampleFmt` | `const std::string &` |  |

---

#### ~AudioCodec 

```cpp
virtual ~AudioCodec()
```

---

#### toString 

```cpp
virtual std::string toString() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### channels 

```cpp
int channels
```

---

#### sampleFmt 

```cpp
std::string sampleFmt
```

One of: u8, s16, s32, flt, dbl, u8p, s16p, s32p, fltp, dblp.

## VideoCodec 

> **Extends:** `icy::av::Codec`
> **Defined in:** `codec.h`

### Members

| Name | Description |
|------|-------------|
| [`VideoCodec`](#group__av_1gaa44f4cc846f8b9aad8557078822c327a) |  |
| [`VideoCodec`](#group__av_1ga587fce7885e91f4f9ff83ca6f13577b3) |  |
| [`VideoCodec`](#group__av_1gaa6ed9271c03b9a9835702c211042d699) |  |
| [`VideoCodec`](#group__av_1gafd3818c6f3ee6e2521fa7748314d1a78) |  |
| [`VideoCodec`](#group__av_1gada45746b74cf8b5c55f69c4a1ae29aec) |  |
| [`~VideoCodec`](#group__av_1ga97e3ee1f4ba76c2ef660b8ca79443c8b) |  |
| [`toString`](#group__av_1gaa21213668b8e41b7532c82637e454762) |  |
| [`print`](#group__av_1ga2fb6dd668b213d39137cbe3b1d5fc34f) |  |
| [`width`](#group__av_1ga0d2a616eaac095f875d8f2a0bd98091f) |  |
| [`height`](#group__av_1ga6cbce4c67a21bce1e6a7ecee4d7b83b7) |  |
| [`fps`](#group__av_1gab5abeb6ba9ae28522af6da214811592b) |  |
| [`pixelFmt`](#group__av_1ga94555d9553e04922c805d3a4661462d2) |  |

---

#### VideoCodec 

```cpp
VideoCodec()
```

---

#### VideoCodec 

```cpp
VideoCodec(int width, int height, double fps, const std::string & pixelFmt, int bitRate, int sampleRate)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `width` | `int` |  |
| `height` | `int` |  |
| `fps` | `double` |  |
| `pixelFmt` | `const std::string &` |  |
| `bitRate` | `int` |  |
| `sampleRate` | `int` |  |

---

#### VideoCodec 

```cpp
VideoCodec(const std::string & name, int width, int height, double fps, int bitRate, int sampleRate, const std::string & pixelFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `fps` | `double` |  |
| `bitRate` | `int` |  |
| `sampleRate` | `int` |  |
| `pixelFmt` | `const std::string &` |  |

---

#### VideoCodec 

```cpp
VideoCodec(const std::string & name, const std::string & encoder, int width, int height, double fps, int bitRate, int sampleRatee, const std::string & pixelFmt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `encoder` | `const std::string &` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `fps` | `double` |  |
| `bitRate` | `int` |  |
| `sampleRatee` | `int` |  |
| `pixelFmt` | `const std::string &` |  |

---

#### VideoCodec 

```cpp
VideoCodec(const VideoCodec & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const VideoCodec &` |  |

---

#### ~VideoCodec 

```cpp
virtual ~VideoCodec()
```

---

#### toString 

```cpp
virtual std::string toString() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### width 

```cpp
int width
```

---

#### height 

```cpp
int height
```

---

#### fps 

```cpp
double fps
```

---

#### pixelFmt 

```cpp
std::string pixelFmt
```

## Deleter 

> **Defined in:** `ffmpeg.h`

Utilites for RAII:

[Deleter](#structicy_1_1av_1_1Deleter) adaptor for functions like av_free that take a pointer.

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__av_1ga43be0299f292908f86655a404dec15bd) |  |

---

#### operator() 

```cpp
inline void operator()(T * p) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `p` | `T *` |  |

## Deleterp 

> **Defined in:** `ffmpeg.h`

[Deleter](#structicy_1_1av_1_1Deleter) adaptor for functions like av_freep that take a pointer to a pointer.

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__av_1ga32ab3d485566a5de359e2615e817263b) |  |

---

#### operator() 

```cpp
inline void operator()(T * p) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `p` | `T *` |  |

## Format 

> **Defined in:** `format.h`

Defines a media container format which is available through the [Format](#structicy_1_1av_1_1Format) Registry for encoding/decoding. A format defined preferred default values for each codec.

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__av_1gaee45ab380f3ab82ddee6a904902e441a) |  |
| [`Format`](#group__av_1gadc07003ad1a442a1a1f6430e46b07545) | Ctors/Dtors. |
| [`Format`](#group__av_1ga6dfaf2481d30bc3a9172e3cd0fdfb021) |  |
| [`Format`](#group__av_1ga3b8c3f30d4bf073ae393bbce6d1b46f1) |  |
| [`Format`](#group__av_1ga5b4c8a4300f9436641f5ae2e4934cf1c) |  |
| [`Format`](#group__av_1gac534811daa5a3274f418f327b1d24f55) |  |
| [`type`](#group__av_1ga8cce3276f2c5399b7f7f86b2f3d431f8) | The format type. |
| [`toString`](#group__av_1ga55f0817773eba2440856dcac9cc8db04) |  |
| [`print`](#group__av_1gac2a467b9b251cc8274c3900360e9c58d) |  |
| [`preferable`](#group__av_1gac0061edac2d666861ddfe6f5171aeb87) |  |
| [`name`](#group__av_1gab23291d998e8eeeaf780711adb47f279) | Base [Format](#structicy_1_1av_1_1Format) Variables. |
| [`id`](#group__av_1gae301eb6d3562a0593b3cba4645944965) | The short name of this format. |
| [`video`](#group__av_1ga8c72beb62c31705f7954bb74941165f8) | The video codec. |
| [`audio`](#group__av_1ga2d7a7c6b006a17245c5e232d059f59be) | The audio codec. |
| [`priority`](#group__av_1gad33f46119d0512791b3116b07af86423) | The priority this format will be displayed on the list. |

---

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

---

#### Format 

```cpp
Format()
```

Ctors/Dtors.

---

#### Format 

```cpp
Format(const std::string & name, const std::string & id, const VideoCodec & video, const AudioCodec & audio, int priority)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `id` | `const std::string &` |  |
| `video` | `const VideoCodec &` |  |
| `audio` | `const AudioCodec &` |  |
| `priority` | `int` |  |

---

#### Format 

```cpp
Format(const std::string & name, const std::string & id, const VideoCodec & video, int priority)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `id` | `const std::string &` |  |
| `video` | `const VideoCodec &` |  |
| `priority` | `int` |  |

---

#### Format 

```cpp
Format(const std::string & name, const std::string & id, const AudioCodec & audio, int priority)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `id` | `const std::string &` |  |
| `audio` | `const AudioCodec &` |  |
| `priority` | `int` |  |

---

#### Format 

```cpp
Format(const Format & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Format &` |  |

---

#### type 

```cpp
Type type() const
```

The format type.

---

#### toString 

```cpp
virtual std::string toString() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### preferable 

```cpp
static inline bool preferable(const Format & first, const Format & second)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `first` | `const Format &` |  |
| `second` | `const Format &` |  |

---

#### name 

```cpp
std::string name
```

Base [Format](#structicy_1_1av_1_1Format) Variables.

The display name of this format.

---

#### id 

```cpp
std::string id
```

The short name of this format.

---

#### video 

```cpp
VideoCodec video
```

The video codec.

---

#### audio 

```cpp
AudioCodec audio
```

The audio codec.

---

#### priority 

```cpp
int priority
```

The priority this format will be displayed on the list.

## FPSCounter 

> **Defined in:** `fpscounter.h`

### Members

| Name | Description |
|------|-------------|
| [`FPSCounter`](#group__av_1ga08e60283f72b88f8ac4470e99b6c6a29) |  |
| [`tick`](#group__av_1ga8482fcfaf1b6130986021e717a31480f) |  |
| [`reset`](#group__av_1ga69069625ebaad7c2650f8b5694d9d236) |  |
| [`started`](#group__av_1ga957474be4397f33d4de755fd6a6dfa35) |  |
| [`startFrame`](#group__av_1ga3a5def4cade260d217e3d04f612be349) |  |
| [`endFrame`](#group__av_1ga5a038d317369a4f232ed373d5248bf42) |  |
| [`start`](#group__av_1gab8adff3b20da64480d14945b72c9f103) |  |
| [`end`](#group__av_1ga4c0df024844c9f7d93f9a211e8280d7b) |  |
| [`frames`](#group__av_1ga2ae1b95710ddb1c8b27353b1bc83aa8a) |  |
| [`total`](#group__av_1ga1c3c6dfc7993518d31340a13b3ce6f6f) |  |
| [`fps`](#group__av_1ga1371516475e42a8a82e150a0374b7ddd) |  |

---

#### FPSCounter 

```cpp
inline FPSCounter()
```

---

#### tick 

```cpp
inline void tick()
```

---

#### reset 

```cpp
inline void reset()
```

---

#### started 

```cpp
inline bool started()
```

---

#### startFrame 

```cpp
inline void startFrame()
```

---

#### endFrame 

```cpp
inline double endFrame()
```

---

#### start 

```cpp
clock_t start
```

---

#### end 

```cpp
clock_t end
```

---

#### frames 

```cpp
int64_t frames
```

---

#### total 

```cpp
double total
```

---

#### fps 

```cpp
double fps
```

## EncoderState 

> **Extends:** `icy::State`
> **Defined in:** `iencoder.h`

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__av_1gad24c76e0eb2d570f08ebb767d565ed38) |  |
| [`str`](#group__av_1ga1b8ea9145750488a74eb407a32fe90ba) |  |

---

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

---

#### str 

```cpp
inline std::string str(unsigned int id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `unsigned int` |  |

## EncoderOptions 

> **Defined in:** `iencoder.h`

### Members

| Name | Description |
|------|-------------|
| [`EncoderOptions`](#group__av_1ga387141c363e2ab3a4f500549fe359dd5) |  |
| [`~EncoderOptions`](#group__av_1gaa16b0ea452e4064444fac3c678a1ef0c) |  |
| [`iformat`](#group__av_1ga54c813e9943c855171e8279e8085548a) | input media format. |
| [`oformat`](#group__av_1gaaa1c4adc23b0755d9633cff732d931e9) | output media format. |
| [`ifile`](#group__av_1gaca1d7ab6e72cc562ef90860a01461b8a) | input file path. |
| [`ofile`](#group__av_1gae6f1fb56ae68ac24395c4f51dcf7c85b) | output file path. |
| [`duration`](#group__av_1ga65a112623aedce6dbb8f6a0f244dea05) | duration of time to record in nanoseconds. |

---

#### EncoderOptions 

```cpp
inline EncoderOptions(const Format & iformat, const Format & oformat, const std::string & ifile, const std::string & ofile, long duration)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `iformat` | `const Format &` |  |
| `oformat` | `const Format &` |  |
| `ifile` | `const std::string &` |  |
| `ofile` | `const std::string &` |  |
| `duration` | `long` |  |

---

#### ~EncoderOptions 

```cpp
virtual ~EncoderOptions() = default
```

---

#### iformat 

```cpp
Format iformat
```

input media format.

---

#### oformat 

```cpp
Format oformat
```

output media format.

---

#### ifile 

```cpp
std::string ifile
```

input file path.

---

#### ofile 

```cpp
std::string ofile
```

output file path.

---

#### duration 

```cpp
long duration
```

duration of time to record in nanoseconds.

## MediaPacket 

> **Extends:** `icy::RawPacket`
> **Subclasses:** `icy::av::AudioPacket`, `icy::av::VideoPacket`
> **Defined in:** `packet.h`

### Members

| Name | Description |
|------|-------------|
| [`MediaPacket`](#group__av_1gac9d853208831fdf8fcc3201cf9b0c28d) |  |
| [`MediaPacket`](#group__av_1ga0f8e4b7ec8dc735257864bd80302a318) | Construct with const data (copied, owning). |
| [`MediaPacket`](#group__av_1gaebb222de1256c5f9a53d3754634abf55) |  |
| [`~MediaPacket`](#group__av_1gaf8e865fc115d6d8b610ae363377e75ab) |  |
| [`clone`](#group__av_1ga074433f3edf7a433f683add7c24bf264) |  |
| [`className`](#group__av_1ga54de449000362cb481e49231dfffea58) |  |
| [`time`](#group__av_1gabb9a7542ea30867ed0749c549b452684) |  |

---

#### MediaPacket 

```cpp
inline MediaPacket(uint8_t * data, size_t size, int64_t time)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `size` | `size_t` |  |
| `time` | `int64_t` |  |

---

#### MediaPacket 

```cpp
inline MediaPacket(const uint8_t * data, size_t size, int64_t time)
```

Construct with const data (copied, owning).

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const uint8_t *` |  |
| `size` | `size_t` |  |
| `time` | `int64_t` |  |

---

#### MediaPacket 

```cpp
inline MediaPacket(const MediaPacket & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const MediaPacket &` |  |

---

#### ~MediaPacket 

```cpp
virtual ~MediaPacket() = default
```

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### time 

```cpp
int64_t time
```

## VideoPacket 

> **Extends:** `icy::av::MediaPacket`
> **Subclasses:** `icy::av::PlanarVideoPacket`
> **Defined in:** `packet.h`

Video packet for interleaved formats.

### Members

| Name | Description |
|------|-------------|
| [`VideoPacket`](#group__av_1gaff046f16e26635e322445cdf872ab8f5) |  |
| [`VideoPacket`](#group__av_1ga04b7a866edebd63332a1a852eba039e3) |  |
| [`~VideoPacket`](#group__av_1ga9756c5718969ab42329bfb31890da41a) |  |
| [`clone`](#group__av_1gad8b50045a88a7e594f11984f612cf9b5) |  |
| [`className`](#group__av_1ga9c572f1d81439fff33b4acf7d9c773b7) |  |
| [`width`](#group__av_1ga73d26deb4e00050f9db4a1fd99e58cce) |  |
| [`height`](#group__av_1ga25f5e1546e384451aef0d0e645e25a35) |  |
| [`iframe`](#group__av_1gaba12f8bdb6b1fe073f8a424dc59bc799) |  |

---

#### VideoPacket 

```cpp
inline VideoPacket(uint8_t * data, size_t size, int width, int height, int64_t time)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `size` | `size_t` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `time` | `int64_t` |  |

---

#### VideoPacket 

```cpp
inline VideoPacket(const VideoPacket & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const VideoPacket &` |  |

---

#### ~VideoPacket 

```cpp
virtual ~VideoPacket() = default
```

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### width 

```cpp
int width
```

---

#### height 

```cpp
int height
```

---

#### iframe 

```cpp
bool iframe
```

## PlanarVideoPacket 

> **Extends:** `icy::av::VideoPacket`
> **Defined in:** `packet.h`

Video packet for planar formats.

#### Parameters
* `data` Array of per-plane data pointers (up to 4 planes). 

* `linesize` Array of per-plane byte strides. 

* `pixelFmt` The pixel format name (e.g. "yuv420p"). 

* `width` The frame width in pixels. 

* `height` The frame height in pixels. 

* `time` The timestamp in microseconds.

### Members

| Name | Description |
|------|-------------|
| [`PlanarVideoPacket`](#group__av_1gaf8d9ac2e7fdf39ee4d73d0d0886b1ceb) |  |
| [`PlanarVideoPacket`](#group__av_1ga4abea12a09aa43ce4954e372baef7a8c) |  |
| [`~PlanarVideoPacket`](#group__av_1ga2aae877fb4dfd5691c0d666e18a28859) |  |
| [`clone`](#group__av_1ga03d18f5a75de381e3b7ce4b9e4a75d9a) |  |
| [`className`](#group__av_1gac9986c1091ed255c4614641e6e00f7e6) |  |
| [`buffer`](#group__av_1ga9a3c69a22b590d040cbbaab468c01bdb) |  |
| [`linesize`](#group__av_1ga7e7bfca3306f5509d135b114da83c5bc) |  |
| [`pixelFmt`](#group__av_1ga116fa37a9f36f2fa6f00c0ac93f68418) |  |

---

#### PlanarVideoPacket 

```cpp
PlanarVideoPacket(uint8_t * data, const int linesize, const std::string & pixelFmt, int width, int height, int64_t time)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `linesize` | `const int` |  |
| `pixelFmt` | `const std::string &` |  |
| `width` | `int` |  |
| `height` | `int` |  |
| `time` | `int64_t` |  |

---

#### PlanarVideoPacket 

```cpp
PlanarVideoPacket(const PlanarVideoPacket & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const PlanarVideoPacket &` |  |

---

#### ~PlanarVideoPacket 

```cpp
virtual ~PlanarVideoPacket()
```

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### buffer 

```cpp
uint8_t * buffer = { nullptr }
```

---

#### linesize 

```cpp
int linesize
```

---

#### pixelFmt 

```cpp
std::string pixelFmt
```

## AudioPacket 

> **Extends:** `icy::av::MediaPacket`
> **Subclasses:** `icy::av::PlanarAudioPacket`
> **Defined in:** `packet.h`

Audio packet for interleaved formats.

### Members

| Name | Description |
|------|-------------|
| [`AudioPacket`](#group__av_1ga195ac44889de6c61aaa6199f46e9dda5) |  |
| [`~AudioPacket`](#group__av_1ga89e0700ad79770d6dc344e5aabdfa890) |  |
| [`clone`](#group__av_1ga7e4bead8a7be118c1350e0ad11772df5) |  |
| [`samples`](#group__av_1gade6d4ffffca5ac7d08930e75cbbf5c10) |  |
| [`className`](#group__av_1ga11b528a4f6076f007177bdf29b35d207) |  |
| [`numSamples`](#group__av_1gaef38d5a53938b523ac986cb148afc245) |  |

---

#### AudioPacket 

```cpp
inline AudioPacket(uint8_t * data, size_t size, size_t numSamples, int64_t time)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `size` | `size_t` |  |
| `numSamples` | `size_t` |  |
| `time` | `int64_t` |  |

---

#### ~AudioPacket 

```cpp
virtual ~AudioPacket() = default
```

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### samples 

```cpp
virtual inline uint8_t * samples() const
```

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### numSamples 

```cpp
size_t numSamples
```

## PlanarAudioPacket 

> **Extends:** `icy::av::AudioPacket`
> **Defined in:** `packet.h`

Audio packet for planar formats.

#### Parameters
* `data` Array of per-plane sample buffers (one per channel). 

* `channels` The number of audio channels. 

* `numSamples` The number of samples per channel. 

* `sampleFmt` The sample format name (e.g. "fltp"). 

* `time` The timestamp in microseconds.

### Members

| Name | Description |
|------|-------------|
| [`PlanarAudioPacket`](#group__av_1ga3b900377bd05251506e88ca2257701b3) |  |
| [`PlanarAudioPacket`](#group__av_1ga51c7846e448e42ada6b920923a011e96) |  |
| [`~PlanarAudioPacket`](#group__av_1ga0613354409c88d2899e1e15a63512a21) |  |
| [`clone`](#group__av_1gaeea01ec76767635cbde78bf3e42a1c75) |  |
| [`className`](#group__av_1ga52925e8a703f1f2e7adc6822f3f6054e) |  |
| [`buffer`](#group__av_1ga4fe84e8d523089c8ac8227428675184d) |  |
| [`linesize`](#group__av_1gacb99bf2c2e1c52464ed2e8f5d713d7a7) |  |
| [`channels`](#group__av_1gad042ae63d86adce90c2089377ce37dee) |  |
| [`sampleFmt`](#group__av_1gae6212664c4b0dfd2a1a26a3b09365e86) |  |

---

#### PlanarAudioPacket 

```cpp
PlanarAudioPacket(uint8_t * data, int channels, size_t numSamples, const std::string & sampleFmt, int64_t time)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `channels` | `int` |  |
| `numSamples` | `size_t` |  |
| `sampleFmt` | `const std::string &` |  |
| `time` | `int64_t` |  |

---

#### PlanarAudioPacket 

```cpp
PlanarAudioPacket(const PlanarAudioPacket & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const PlanarAudioPacket &` |  |

---

#### ~PlanarAudioPacket 

```cpp
virtual ~PlanarAudioPacket()
```

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### buffer 

```cpp
uint8_t * buffer = { nullptr }
```

---

#### linesize 

```cpp
int linesize
```

---

#### channels 

```cpp
int channels
```

---

#### sampleFmt 

```cpp
std::string sampleFmt
```

## MediaPacketTimeCompare 

> **Defined in:** `realtimepacketqueue.h`

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__av_1gaae0732616533ecf4d9869a7751eae614) |  |

---

#### operator() 

```cpp
inline bool operator()(const MediaPacket * a, const MediaPacket * b)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `a` | `const MediaPacket *` |  |
| `b` | `const MediaPacket *` |  |

## VideoContext 

> **Subclasses:** `icy::av::VideoDecoder`, `icy::av::VideoEncoder`
> **Defined in:** `videocontext.h`

Base video context from which all video encoders and decoders derive.

### Members

| Name | Description |
|------|-------------|
| [`VideoContext`](#group__av_1gad6a1f87160fb60f7a2a003565931dab1) |  |
| [`~VideoContext`](#group__av_1ga56d1e423ba71d70cc3c4e71bc089bc44) |  |
| [`create`](#group__av_1gab3f5a2d0c17a8cd21cf78d0d0b85ccc5) | Create the `AVCodecContext` using default values. |
| [`open`](#group__av_1gac11a4a1ec6e53e4bfcabd5fdd4ab405b) | Open the `AVCodecContext` |
| [`close`](#group__av_1ga90c4aa947a1fd5cdb599027bf1fcc3d7) | Close the `AVCodecContext` |
| [`convert`](#group__av_1gaca7b0edc59811fcd33ace4314830ba59) | Convert the video frame and return the result. |
| [`recreateConverter`](#group__av_1ga25a0643fc77c96a00f4739a5b89b2d64) |  |
| [`emitter`](#group__av_1ga4b2399a155aef55e06b3b1161ef64c03) |  |
| [`iparams`](#group__av_1ga538caf74e90bbd3a5f89ddef10c21d8b) | input parameters |
| [`oparams`](#group__av_1ga6c2ffdffb62ea3590b2eee59821591b0) | output parameters |
| [`stream`](#group__av_1ga165fcfeea3b363b8d8584a1ee3fcd5d2) | encoder or decoder stream |
| [`ctx`](#group__av_1gae16196ccc02da8601dffcb1a76a22625) | encoder or decoder context |
| [`codec`](#group__av_1ga52c9688a8df25c77655e9f6815f18149) | encoder or decoder codec |
| [`frame`](#group__av_1gac9f0269a331589ae180b564de79ba1d6) | encoder or decoder frame |
| [`conv`](#group__av_1ga9d9a72086b8fc56058415fce010410ae) | video conversion context |
| [`time`](#group__av_1gafbef89eda47bfd2b4b4669f2d086286c) | stream time in codec time base |
| [`pts`](#group__av_1ga390bb6f03ef0d9607c1de75c1e7e8641) | last packet pts value |
| [`seconds`](#group__av_1ga9cca27f02f5a0a23e7ebd5acfc00aac7) | video time in seconds |
| [`error`](#group__av_1gaccb84925702ba5b2b196327db38a3dfe) | error message |

---

#### VideoContext 

```cpp
VideoContext()
```

---

#### ~VideoContext 

```cpp
virtual ~VideoContext()
```

---

#### create 

```cpp
virtual void create()
```

Create the `AVCodecContext` using default values.

---

#### open 

```cpp
virtual void open()
```

Open the `AVCodecContext`

---

#### close 

```cpp
virtual void close()
```

Close the `AVCodecContext`

---

#### convert 

```cpp
virtual AVFrame * convert(AVFrame * iframe)
```

Convert the video frame and return the result.

The input frame will only be converted if it doesn't match the output format. If the frame is not converted the input frame will be returned. If the input frame format does not match the `[VideoConverter](#structicy_1_1av_1_1VideoConverter)` context then the `[VideoConverter](#structicy_1_1av_1_1VideoConverter)` will be recreated with the input frame params.

| Parameter | Type | Description |
|-----------|------|-------------|
| `iframe` | `AVFrame *` |  |

---

#### recreateConverter 

```cpp
virtual bool recreateConverter()
```

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### iparams 

```cpp
VideoCodec iparams
```

input parameters

---

#### oparams 

```cpp
VideoCodec oparams
```

output parameters

---

#### stream 

```cpp
AVStream * stream
```

encoder or decoder stream

---

#### ctx 

```cpp
AVCodecContext * ctx
```

encoder or decoder context

---

#### codec 

```cpp
const AVCodec * codec
```

encoder or decoder codec

---

#### frame 

```cpp
AVFrame * frame
```

encoder or decoder frame

---

#### conv 

```cpp
std::unique_ptr< VideoConverter > conv
```

video conversion context

---

#### time 

```cpp
int64_t time
```

stream time in codec time base

---

#### pts 

```cpp
int64_t pts
```

last packet pts value

---

#### seconds 

```cpp
double seconds
```

video time in seconds

---

#### error 

```cpp
std::string error
```

error message

## VideoConverter 

> **Defined in:** `videoconverter.h`

### Members

| Name | Description |
|------|-------------|
| [`VideoConverter`](#group__av_1gaf23c8c6da3e8c134d2732611511b0971) |  |
| [`~VideoConverter`](#group__av_1ga219316cbdf4731474d84a7c6968816d2) |  |
| [`create`](#group__av_1gaf043126e335328df0dea9122aba1adab) |  |
| [`close`](#group__av_1ga03d0157af35129547a87d6c21431a9f4) |  |
| [`convert`](#group__av_1ga3a3f64ca7f046bfab8f805753b644606) |  |
| [`ctx`](#group__av_1ga862efcfb9c461be2de819548b8315c2b) |  |
| [`oframe`](#group__av_1gaa02822cf5189d7f220247c6aab786898) |  |
| [`iparams`](#group__av_1ga4869c5f4963a6f42cbc91e5000efa23c) |  |
| [`oparams`](#group__av_1gafee51c12b018305ff22d056aed1dfd70) |  |

---

#### VideoConverter 

```cpp
VideoConverter()
```

---

#### ~VideoConverter 

```cpp
virtual ~VideoConverter()
```

---

#### create 

```cpp
virtual void create()
```

---

#### close 

```cpp
virtual void close()
```

---

#### convert 

```cpp
virtual AVFrame * convert(AVFrame * iframe)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `iframe` | `AVFrame *` |  |

---

#### ctx 

```cpp
SwsContext * ctx
```

---

#### oframe 

```cpp
AVFrame * oframe
```

---

#### iparams 

```cpp
VideoCodec iparams
```

---

#### oparams 

```cpp
VideoCodec oparams
```

## VideoDecoder 

> **Extends:** `icy::av::VideoContext`
> **Defined in:** `videodecoder.h`

### Members

| Name | Description |
|------|-------------|
| [`VideoDecoder`](#group__av_1gaccadd97127e3ce1fd3f24ca533dee773) |  |
| [`~VideoDecoder`](#group__av_1ga72037b0014cde348ee674b1cdd7c1f28) |  |
| [`create`](#group__av_1ga302c5f9838c3770f1759455cbb684aeb) | Create the `AVCodecContext` using default values. |
| [`open`](#group__av_1ga015206763eca1d62ea6314a743cc1f23) | Open the `AVCodecContext` |
| [`close`](#group__av_1ga87067bb74819811c42d87a0cbd773c0e) | Close the `AVCodecContext` |
| [`decode`](#group__av_1ga8675b973c95f9231e3e3d8711776acac) | Decodes a the given input packet. Input packets should use the raw `AVStream` time base. Time base conversion will happen internally. Returns true an output packet was was decoded, false otherwise. |
| [`flush`](#group__av_1ga2c6f150d843783ca66fe0881a9f8192a) | Flushes buffered frames. This method should be called after decoding until false is returned. |

---

#### VideoDecoder 

```cpp
VideoDecoder(AVStream * stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `AVStream *` |  |

---

#### ~VideoDecoder 

```cpp
virtual ~VideoDecoder()
```

---

#### create 

```cpp
virtual void create()
```

Create the `AVCodecContext` using default values.

---

#### open 

```cpp
virtual void open()
```

Open the `AVCodecContext`

---

#### close 

```cpp
virtual void close()
```

Close the `AVCodecContext`

---

#### decode 

```cpp
virtual bool decode(AVPacket & ipacket)
```

Decodes a the given input packet. Input packets should use the raw `AVStream` time base. Time base conversion will happen internally. Returns true an output packet was was decoded, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ipacket` | `AVPacket &` |  |

---

#### flush 

```cpp
virtual void flush()
```

Flushes buffered frames. This method should be called after decoding until false is returned.

## VideoEncoder 

> **Extends:** `icy::av::VideoContext`
> **Defined in:** `videoencoder.h`

### Members

| Name | Description |
|------|-------------|
| [`VideoEncoder`](#group__av_1gab3440fe9140ed31e166d57692cd95e1b) |  |
| [`~VideoEncoder`](#group__av_1ga5b4833959dc5406ae4119ed3e657d795) |  |
| [`create`](#group__av_1gaa3eec3187025c245fd4958b6a376e99b) | Create the `AVCodecContext` using default values. |
| [`close`](#group__av_1gaf5cbf120cc0c08fb599a0ea002aa4427) | Close the `AVCodecContext` |
| [`encode`](#group__av_1gaf40c6ac2cee66007cf626350be51834c) | Encode a single video frame. This method is for interleaved video formats. |
| [`encode`](#group__av_1gaa0a72afd76c223b926107f6499fa93f3) | Encode a single video frame. This method is for planar video formats. |
| [`encode`](#group__av_1gac695c7a28cd3eabe9ec19c8abee1b2a9) | Encode a single AVFrame. |
| [`flush`](#group__av_1ga863e6269fe257b1261ee58db99305fb5) | Flush remaining packets to be encoded. This method should be called once before stream closure. |
| [`format`](#group__av_1gab300963e81559e48fd22562211edea3c) |  |

---

#### VideoEncoder 

```cpp
VideoEncoder(AVFormatContext * format)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `format` | `AVFormatContext *` |  |

---

#### ~VideoEncoder 

```cpp
virtual ~VideoEncoder()
```

---

#### create 

```cpp
virtual void create()
```

Create the `AVCodecContext` using default values.

---

#### close 

```cpp
virtual void close()
```

Close the `AVCodecContext`

---

#### encode 

```cpp
virtual bool encode(uint8_t * data, int size, int64_t pts)
```

Encode a single video frame. This method is for interleaved video formats.

#### Parameters
* `data` The raw video frame buffer. 

* `size` The buffer size in bytes. 

* `pts` The presentation timestamp in stream time base units.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `size` | `int` |  |
| `pts` | `int64_t` |  |

---

#### encode 

```cpp
virtual bool encode(uint8_t * data, int linesize, int64_t pts)
```

Encode a single video frame. This method is for planar video formats.

#### Parameters
* `data` Array of per-plane data pointers (up to 4 planes). 

* `linesize` Array of per-plane byte strides. 

* `pts` The presentation timestamp in stream time base units.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t *` |  |
| `linesize` | `int` |  |
| `pts` | `int64_t` |  |

---

#### encode 

```cpp
virtual bool encode(AVFrame * iframe)
```

Encode a single AVFrame.

| Parameter | Type | Description |
|-----------|------|-------------|
| `iframe` | `AVFrame *` |  |

---

#### flush 

```cpp
virtual void flush()
```

Flush remaining packets to be encoded. This method should be called once before stream closure.

---

#### format 

```cpp
AVFormatContext * format
```

