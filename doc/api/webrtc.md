{#webrtcmodule}

# webrtc

WebRTC media transport via libdatachannel; peer sessions, media bridge, codec negotiation.

### Classes

| Name | Description |
|------|-------------|
| [`SignallingInterface`](#signallinginterface) | Transport-agnostic signalling interface for WebRTC session setup. |
| [`MediaBridge`](#mediabridge) | Convenience wrapper that creates WebRTC tracks on a PeerConnection and exposes per-track sender/receiver adapters for [PacketStream](base.md#packetstream) integration. |
| [`PeerSession`](#peersession) | Manages a WebRTC peer connection lifecycle over any signalling transport that implements [SignallingInterface](#signallinginterface). |
| [`WebRtcTrackSender`](#webrtctracksender) | [PacketProcessor](base.md#packetprocessor) that sends encoded media to a single libdatachannel Track via sendFrame(). |
| [`WebRtcTrackReceiver`](#webrtctrackreceiver) | [PacketStreamAdapter](base.md#packetstreamadapter) that receives depacketized frames from a single remote libdatachannel Track and emits them as VideoPacket or AudioPacket into a [PacketStream](base.md#packetstream). |
| [`CodecNegotiator`](#codecnegotiator) | Maps RTP codec names to FFmpeg encoders and queries FFmpeg at runtime to determine what codecs are available. |
| [`TrackHandle`](#trackhandle) | Result of creating a track: the track itself plus its RTP config. Keep the config around - you need it for [WebRtcTrackSender](#webrtctracksender). |
| [`CodecSpec`](#codecspec) | Canonical description of a codec supported by Icey's WebRTC helpers. |
| [`NegotiatedCodec`](#negotiatedcodec) | Result of codec negotiation between a remote SDP offer and the local FFmpeg codec inventory. |

### Enumerations

| Name | Description |
|------|-------------|
| [`CodecMediaType`](#codecmediatype)  | Whether a codec carries audio or video media. |
| [`CodecId`](#codecid)  | Stable codec identifiers used across negotiation and track setup. |

---

{#codecmediatype}

#### CodecMediaType

```cpp
enum CodecMediaType
```

Whether a codec carries audio or video media.

| Value | Description |
|-------|-------------|
| `Video` | Video RTP payload. |
| `Audio` | Audio RTP payload. |

---

{#codecid}

#### CodecId

```cpp
enum CodecId
```

Stable codec identifiers used across negotiation and track setup.

| Value | Description |
|-------|-------------|
| `Unknown` | Unrecognized or unsupported codec. |
| `H264` | H.264 / AVC. |
| `H265` | H.265 / HEVC. |
| `VP8` | Google VP8. |
| `VP9` | Google VP9. |
| `AV1` | AOMedia AV1. |
| `Opus` | Opus audio. |
| `PCMU` | G.711 mu-law audio. |
| `PCMA` | G.711 A-law audio. |
| `G722` | G.722 wideband audio. |
| `AAC` | AAC audio. |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `TrackHandle` | [`createVideoTrack`](#createvideotrack)  | Create a video send track on a PeerConnection. |
| `TrackHandle` | [`createAudioTrack`](#createaudiotrack)  | Create an audio send track on a PeerConnection. |
| `std::shared_ptr< rtc::Track >` | [`createVideoReceiveTrack`](#createvideoreceivetrack)  | Create a pure receive-side video track on a PeerConnection. |
| `std::shared_ptr< rtc::Track >` | [`createAudioReceiveTrack`](#createaudioreceivetrack)  | Create a pure receive-side audio track on a PeerConnection. |
| `bool` | [`setupReceiveTrack`](#setupreceivetrack)  | Set up the receive-side media handler chain on a remote track. |
| `uint32_t` | [`generateSsrc`](#generatessrc)  | Generate a random SSRC. |
| `const char *` | [`stateToString`](#statetostring)  | Convert a [PeerSession::State](#state-4) to a lowercase C string for logging. |

---

{#createvideotrack}

#### createVideoTrack

```cpp
TrackHandle createVideoTrack(std::shared_ptr< rtc::PeerConnection > pc, const av::VideoCodec & codec, uint32_t ssrc, const std::string & cname, const std::string & mid, rtc::Description::Direction direction, unsigned nackBuffer, std::function< void()> onPli, std::function< void(unsigned int)> onRemb, int payloadType)
```

Create a video send track on a PeerConnection.

Sets up the full outgoing media handler chain: Packetizer → SrReporter → NackResponder → PliHandler → RembHandler

The packetizer is selected based on the codec: H264 → H264RtpPacketizer (Annex-B long start sequence) H265 → H265RtpPacketizer (Annex-B long start sequence) VP8/VP9/other → generic RtpPacketizer

#### Parameters
* `pc` PeerConnection to add the track to. 

* `codec` Video codec. Must name a supported RTP codec or FFmpeg encoder (e.g. "H264" or "libx264"). 

* `ssrc` RTP SSRC. 0 = auto-generate. 

* `cname` RTCP CNAME. Empty = "icey". 

* `mid` MID to use for the track when answering an existing offer. 

* `direction` Direction to advertise for the negotiated m-line. 

* `nackBuffer` Max packets stored for NACK retransmission. 

* `onPli` Callback when remote peer requests a keyframe. Connect to your encoder to force IDR. 

* `onRemb` Callback when remote peer reports bandwidth estimate. Bitrate in bits/sec. 

* `payloadType` Explicit RTP payload type to reuse when answering an offer. -1 = use the codec's default/preferred type. 

#### Returns
[TrackHandle](#trackhandle) with the track and its RTP config.

---

{#createaudiotrack}

#### createAudioTrack

```cpp
TrackHandle createAudioTrack(std::shared_ptr< rtc::PeerConnection > pc, const av::AudioCodec & codec, uint32_t ssrc, const std::string & cname, const std::string & mid, rtc::Description::Direction direction, int payloadType)
```

Create an audio send track on a PeerConnection.

Sets up the outgoing media handler chain: AudioRtpPacketizer → SrReporter

The packetizer clock rate is selected based on codec: opus → 48kHz, PCMU/PCMA → 8kHz, etc.

#### Parameters
* `pc` PeerConnection to add the track to. 

* `codec` Audio codec. Must name a supported RTP codec or FFmpeg encoder (e.g. "opus" or "libopus"). 

* `ssrc` RTP SSRC. 0 = auto-generate. 

* `cname` RTCP CNAME. Empty = "icey". 

* `mid` MID to use for the track when answering an existing offer. 

* `direction` Direction to advertise for the negotiated m-line. 

* `payloadType` Explicit RTP payload type to reuse when answering an offer. -1 = use the codec's default/preferred type. 

#### Returns
[TrackHandle](#trackhandle) with the track and its RTP config.

---

{#createvideoreceivetrack}

#### createVideoReceiveTrack

```cpp
std::shared_ptr< rtc::Track > createVideoReceiveTrack(std::shared_ptr< rtc::PeerConnection > pc, const av::VideoCodec & codec, const std::string & mid, rtc::Description::Direction direction, int payloadType)
```

Create a pure receive-side video track on a PeerConnection.

Unlike [createVideoTrack()](#createvideotrack), this does not add a local SSRC or sender packetizer chain. It is for answers that only receive remote media.

---

{#createaudioreceivetrack}

#### createAudioReceiveTrack

```cpp
std::shared_ptr< rtc::Track > createAudioReceiveTrack(std::shared_ptr< rtc::PeerConnection > pc, const av::AudioCodec & codec, const std::string & mid, rtc::Description::Direction direction, int payloadType)
```

Create a pure receive-side audio track on a PeerConnection.

Unlike [createAudioTrack()](#createaudiotrack), this does not add a local SSRC or sender packetizer chain. It is for answers that only receive remote media.

---

{#setupreceivetrack}

#### setupReceiveTrack

```cpp
bool setupReceiveTrack(std::shared_ptr< rtc::Track > track)
```

Set up the receive-side media handler chain on a remote track.

Selects the correct depacketizer based on the track's SDP codec: Video: H264RtpDepacketizer, H265RtpDepacketizer, or generic Audio: OpusRtpDepacketizer, PCMURtpDepacketizer, etc.

Also chains an RtcpReceivingSession for receiver reports.

Call this from PeerConnection::onTrack() before binding a [WebRtcTrackReceiver](#webrtctrackreceiver).

#### Parameters
* `track` The remote track from onTrack callback. 

#### Returns
True when a supported depacketizer was installed.

---

{#generatessrc}

#### generateSsrc

```cpp
uint32_t generateSsrc()
```

Generate a random SSRC.

---

{#statetostring}

#### stateToString

```cpp
const char * stateToString(PeerSession::State state)
```

Convert a [PeerSession::State](#state-4) to a lowercase C string for logging. 
#### Parameters
* `state` [State](base.md#state) value to convert. 

#### Returns
One of: "idle", "outgoing-init", "incoming-init", "negotiating", "active", "ending", "ended".

{#signallinginterface}

## SignallingInterface

```cpp
#include <icy/webrtc/signalling.h>
```

> **Subclassed by:** [`SympleServerSignaller`](webrtcsupport.md#sympleserversignaller), [`SympleSignaller`](webrtcsupport.md#symplesignaller), [`WebSocketSignaller`](webrtcsupport.md#websocketsignaller)

Transport-agnostic signalling interface for WebRTC session setup.

Implementations handle the exchange of SDP offers/answers and ICE candidates between peers over whatever transport is available: Symple, plain WebSocket, REST, MQTT, carrier pigeon, etc.

[PeerSession](#peersession) takes a reference to this interface. Implement it to plug in your own signalling backend.

The three message categories:

* SDP: offer/answer exchange (the session description)

* Candidate: trickle ICE candidates

* Control: call lifecycle (init, accept, reject, hangup)

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ThreadSignal< void(const std::string &, const std::string &, const std::string &)>` | [`SdpReceived`](#sdpreceived)  | Fires when an SDP offer or answer arrives from a remote peer. Parameters: peerId, type ("offer"/"answer"), sdp. |
| `ThreadSignal< void(const std::string &, const std::string &, const std::string &)>` | [`CandidateReceived`](#candidatereceived)  | Fires when an ICE candidate arrives from a remote peer. Parameters: peerId, candidate, mid. |
| `ThreadSignal< void(const std::string &, const std::string &, const std::string &)>` | [`ControlReceived`](#controlreceived)  | Fires when a control message arrives from a remote peer. Parameters: peerId, type ("init"/"accept"/"reject"/"hangup"), reason. |

---

{#sdpreceived}

#### SdpReceived

```cpp
ThreadSignal< void(const std::string &, const std::string &, const std::string &)> SdpReceived
```

Fires when an SDP offer or answer arrives from a remote peer. Parameters: peerId, type ("offer"/"answer"), sdp.

---

{#candidatereceived}

#### CandidateReceived

```cpp
ThreadSignal< void(const std::string &, const std::string &, const std::string &)> CandidateReceived
```

Fires when an ICE candidate arrives from a remote peer. Parameters: peerId, candidate, mid.

---

{#controlreceived}

#### ControlReceived

```cpp
ThreadSignal< void(const std::string &, const std::string &, const std::string &)> ControlReceived
```

Fires when a control message arrives from a remote peer. Parameters: peerId, type ("init"/"accept"/"reject"/"hangup"), reason.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`sendSdp`](#sendsdp)  | Send an SDP offer or answer to the remote peer. |
| `void` | [`sendCandidate`](#sendcandidate)  | Send an ICE candidate to the remote peer. |
| `void` | [`sendControl`](#sendcontrol)  | Send a control message to the remote peer. |

---

{#sendsdp}

#### sendSdp

```cpp
void sendSdp(const std::string & peerId, const std::string & type, const std::string & sdp)
```

Send an SDP offer or answer to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` "offer" or "answer". 

* `sdp` The SDP string.

---

{#sendcandidate}

#### sendCandidate

```cpp
void sendCandidate(const std::string & peerId, const std::string & candidate, const std::string & mid)
```

Send an ICE candidate to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `candidate` The candidate string (from RTCIceCandidate). 

* `mid` The sdpMid value.

---

{#sendcontrol}

#### sendControl

```cpp
void sendControl(const std::string & peerId, const std::string & type, const std::string & reason)
```

Send a control message to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` Control type: "init", "accept", "reject", "hangup". 

* `reason` Optional reason string (for reject/hangup).

{#mediabridge}

## MediaBridge

```cpp
#include <icy/webrtc/mediabridge.h>
```

Convenience wrapper that creates WebRTC tracks on a PeerConnection and exposes per-track sender/receiver adapters for [PacketStream](base.md#packetstream) integration.

For video-only, leave audioCodec default (disabled). For audio-only, leave videoCodec default (disabled). For data-channel-only, don't create a [MediaBridge](#mediabridge) at all.

For full control over individual tracks, use [createVideoTrack()](#createvideotrack), [createAudioTrack()](#createaudiotrack), [WebRtcTrackSender](#webrtctracksender), and [WebRtcTrackReceiver](#webrtctrackreceiver) directly instead of this class.

The sender and receiver adapter objects are stable for the lifetime of the [MediaBridge](#mediabridge). [attach()](#attach) and [detach()](#detach) rebind or unbind the underlying WebRTC tracks without invalidating references previously obtained from [videoSender()](#videosender), [audioSender()](#audiosender), [videoReceiver()](#videoreceiver), or [audioReceiver()](#audioreceiver).

Example - send camera to browser:

[MediaBridge](#mediabridge) bridge; bridge.attach(pc, {.videoCodec = {"H264", "libx264", 1280, 720, 30}});

[PacketStream](base.md#packetstream) stream; stream.attachSource(capture); stream.attach(encoder, 1, true); stream.attach(&bridge.videoSender(), 5, false); stream.start();

Example - receive from browser and record:

bridge.videoReceiver().emitter += packetSlot(&recorder, &Recorder::onEncodedVideo);

The receiver emits owning encoded packets. Feed those into a decoder or recorder callback; see `samples/media-recorder` for a complete example.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ThreadSignal< void()>` | [`KeyframeRequested`](#keyframerequested)  | Remote peer requests a keyframe. Connect to encoder to force IDR. |
| `ThreadSignal< void(unsigned int)>` | [`BitrateEstimate`](#bitrateestimate)  | Remote peer reports estimated bandwidth (bits/sec). |

---

{#keyframerequested}

#### KeyframeRequested

```cpp
ThreadSignal< void()> KeyframeRequested
```

Remote peer requests a keyframe. Connect to encoder to force IDR.

---

{#bitrateestimate}

#### BitrateEstimate

```cpp
ThreadSignal< void(unsigned int)> BitrateEstimate
```

Remote peer reports estimated bandwidth (bits/sec).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MediaBridge`](#mediabridge-1)  | Construct a detached bridge with stable sender and receiver adapters. |
|  | [`~MediaBridge`](#mediabridge-2)  | Destroy the bridge and release any attached PeerConnection state. |
|  | [`MediaBridge`](#mediabridge-3)  | Deleted copy constructor; [MediaBridge](#mediabridge) owns live track and adapter state. |
| `void` | [`attach`](#attach)  | Create tracks on the PeerConnection and set up handler chains. Only creates tracks for codecs with a non-empty encoder name. |
| `void` | [`detach`](#detach)  | Detach all tracks and adapters. |
| `void` | [`requestKeyframe`](#requestkeyframe)  | Request an immediate keyframe (IDR) from the remote sender. Sends a PLI (Picture Loss Indication) RTCP message on the video track. No-op if no video track is attached. |
| `void` | [`requestBitrate`](#requestbitrate)  | Request that the remote sender reduce to a target bitrate. Sends a TMMBR RTCP message on the video track. |
| `WebRtcTrackSender &` | [`videoSender`](#videosender)  | Video send processor. Attach to a [PacketStream](base.md#packetstream) after a VideoEncoder. Throws if no video track was created. |
| `WebRtcTrackSender &` | [`audioSender`](#audiosender)  | Audio send processor. Attach to a [PacketStream](base.md#packetstream) after an AudioEncoder. Throws if no audio track was created. |
| `WebRtcTrackReceiver &` | [`videoReceiver`](#videoreceiver)  | Video receive adapter. Attach as a [PacketStream](base.md#packetstream) source. Only valid after a remote video track arrives. |
| `WebRtcTrackReceiver &` | [`audioReceiver`](#audioreceiver)  | Audio receive adapter. Attach as a [PacketStream](base.md#packetstream) source. Only valid after a remote audio track arrives. |
| `std::shared_ptr< rtc::Track >` | [`videoTrack`](#videotrack) `const` | The underlying libdatachannel video track, or nullptr if none was created. |
| `std::shared_ptr< rtc::Track >` | [`audioTrack`](#audiotrack) `const` | The underlying libdatachannel audio track, or nullptr if none was created. |
| `bool` | [`hasVideo`](#hasvideo) `const` | True if a video track was created at [attach()](#attach). |
| `bool` | [`hasAudio`](#hasaudio) `const` | True if an audio track was created at [attach()](#attach). |
| `bool` | [`attached`](#attached) `const` | True if [attach()](#attach) has been called and a PeerConnection is held. |

---

{#mediabridge-1}

#### MediaBridge

```cpp
MediaBridge()
```

Construct a detached bridge with stable sender and receiver adapters.

---

{#mediabridge-2}

#### ~MediaBridge

```cpp
~MediaBridge()
```

Destroy the bridge and release any attached PeerConnection state.

---

{#mediabridge-3}

#### MediaBridge

```cpp
MediaBridge(const MediaBridge &) = delete
```

Deleted copy constructor; [MediaBridge](#mediabridge) owns live track and adapter state.

---

{#attach}

#### attach

```cpp
void attach(std::shared_ptr< rtc::PeerConnection > pc, const Options & opts)
```

Create tracks on the PeerConnection and set up handler chains. Only creates tracks for codecs with a non-empty encoder name.

---

{#detach}

#### detach

```cpp
void detach()
```

Detach all tracks and adapters.

---

{#requestkeyframe}

#### requestKeyframe

```cpp
void requestKeyframe()
```

Request an immediate keyframe (IDR) from the remote sender. Sends a PLI (Picture Loss Indication) RTCP message on the video track. No-op if no video track is attached.

---

{#requestbitrate}

#### requestBitrate

```cpp
void requestBitrate(unsigned int bitrate)
```

Request that the remote sender reduce to a target bitrate. Sends a TMMBR RTCP message on the video track. 
#### Parameters
* `bitrate` Target bitrate in bits per second.

---

{#videosender}

#### videoSender

```cpp
WebRtcTrackSender & videoSender()
```

Video send processor. Attach to a [PacketStream](base.md#packetstream) after a VideoEncoder. Throws if no video track was created.

---

{#audiosender}

#### audioSender

```cpp
WebRtcTrackSender & audioSender()
```

Audio send processor. Attach to a [PacketStream](base.md#packetstream) after an AudioEncoder. Throws if no audio track was created.

---

{#videoreceiver}

#### videoReceiver

```cpp
WebRtcTrackReceiver & videoReceiver()
```

Video receive adapter. Attach as a [PacketStream](base.md#packetstream) source. Only valid after a remote video track arrives.

---

{#audioreceiver}

#### audioReceiver

```cpp
WebRtcTrackReceiver & audioReceiver()
```

Audio receive adapter. Attach as a [PacketStream](base.md#packetstream) source. Only valid after a remote audio track arrives.

---

{#videotrack}

#### videoTrack

`const`

```cpp
std::shared_ptr< rtc::Track > videoTrack() const
```

The underlying libdatachannel video track, or nullptr if none was created.

---

{#audiotrack}

#### audioTrack

`const`

```cpp
std::shared_ptr< rtc::Track > audioTrack() const
```

The underlying libdatachannel audio track, or nullptr if none was created.

---

{#hasvideo}

#### hasVideo

`const`

```cpp
bool hasVideo() const
```

True if a video track was created at [attach()](#attach).

---

{#hasaudio}

#### hasAudio

`const`

```cpp
bool hasAudio() const
```

True if an audio track was created at [attach()](#attach).

---

{#attached}

#### attached

`const`

```cpp
bool attached() const
```

True if [attach()](#attach) has been called and a PeerConnection is held.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::shared_ptr< rtc::PeerConnection >` | [`_pc`](#_pc)  |  |
| `WebRtcTrackSender` | [`_videoSender`](#_videosender)  |  |
| `WebRtcTrackSender` | [`_audioSender`](#_audiosender)  |  |
| `TrackHandle` | [`_videoHandle`](#_videohandle)  |  |
| `TrackHandle` | [`_audioHandle`](#_audiohandle)  |  |
| `WebRtcTrackReceiver` | [`_videoReceiver`](#_videoreceiver)  |  |
| `WebRtcTrackReceiver` | [`_audioReceiver`](#_audioreceiver)  |  |
| `std::shared_ptr< rtc::Track >` | [`_videoReceiveTrack`](#_videoreceivetrack)  |  |
| `std::shared_ptr< rtc::Track >` | [`_audioReceiveTrack`](#_audioreceivetrack)  |  |
| `std::mutex` | [`_mutex`](#_mutex-17)  |  |

---

{#_pc}

#### _pc

```cpp
std::shared_ptr< rtc::PeerConnection > _pc
```

---

{#_videosender}

#### _videoSender

```cpp
WebRtcTrackSender _videoSender
```

---

{#_audiosender}

#### _audioSender

```cpp
WebRtcTrackSender _audioSender
```

---

{#_videohandle}

#### _videoHandle

```cpp
TrackHandle _videoHandle
```

---

{#_audiohandle}

#### _audioHandle

```cpp
TrackHandle _audioHandle
```

---

{#_videoreceiver}

#### _videoReceiver

```cpp
WebRtcTrackReceiver _videoReceiver
```

---

{#_audioreceiver}

#### _audioReceiver

```cpp
WebRtcTrackReceiver _audioReceiver
```

---

{#_videoreceivetrack}

#### _videoReceiveTrack

```cpp
std::shared_ptr< rtc::Track > _videoReceiveTrack
```

---

{#_audioreceivetrack}

#### _audioReceiveTrack

```cpp
std::shared_ptr< rtc::Track > _audioReceiveTrack
```

---

{#_mutex-17}

#### _mutex

```cpp
std::mutex _mutex
```

{#options-17}

## Options

```cpp
#include <icy/webrtc/mediabridge.h>
```

[Configuration](base.md#configuration) options for the WebRTC media bridge.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `av::VideoCodec` | [`videoCodec`](#videocodec-6)  | Video codec for the send track. Leave both name and encoder empty to skip creating a video track. |
| `av::AudioCodec` | [`audioCodec`](#audiocodec-5)  | Audio codec for the send track. Leave both name and encoder empty to skip creating an audio track. |
| `uint32_t` | [`videoSsrc`](#videossrc)  | 0 = auto-generate |
| `uint32_t` | [`audioSsrc`](#audiossrc)  | 0 = auto-generate |
| `int` | [`videoPayloadType`](#videopayloadtype)  | Reuse negotiated offer payload type when answering, -1 = default. |
| `int` | [`audioPayloadType`](#audiopayloadtype)  | Reuse negotiated offer payload type when answering, -1 = default. |
| `std::string` | [`cname`](#cname)  | CNAME for RTCP (auto if empty) |
| `std::string` | [`videoMid`](#videomid)  | Explicit MID for the negotiated video m-line when answering an offer. |
| `std::string` | [`audioMid`](#audiomid)  | Explicit MID for the negotiated audio m-line when answering an offer. |
| `rtc::Description::Direction` | [`videoDirection`](#videodirection)  |  |
| `rtc::Description::Direction` | [`audioDirection`](#audiodirection)  |  |
| `unsigned` | [`nackBufferSize`](#nackbuffersize)  | Max RTP packets retained for video NACK retransmission. |

---

{#videocodec-6}

#### videoCodec

```cpp
av::VideoCodec videoCodec
```

Video codec for the send track. Leave both name and encoder empty to skip creating a video track.

---

{#audiocodec-5}

#### audioCodec

```cpp
av::AudioCodec audioCodec
```

Audio codec for the send track. Leave both name and encoder empty to skip creating an audio track.

---

{#videossrc}

#### videoSsrc

```cpp
uint32_t videoSsrc = 0
```

0 = auto-generate

---

{#audiossrc}

#### audioSsrc

```cpp
uint32_t audioSsrc = 0
```

0 = auto-generate

---

{#videopayloadtype}

#### videoPayloadType

```cpp
int videoPayloadType = -1
```

Reuse negotiated offer payload type when answering, -1 = default.

---

{#audiopayloadtype}

#### audioPayloadType

```cpp
int audioPayloadType = -1
```

Reuse negotiated offer payload type when answering, -1 = default.

---

{#cname}

#### cname

```cpp
std::string cname
```

CNAME for RTCP (auto if empty)

---

{#videomid}

#### videoMid

```cpp
std::string videoMid
```

Explicit MID for the negotiated video m-line when answering an offer.

---

{#audiomid}

#### audioMid

```cpp
std::string audioMid
```

Explicit MID for the negotiated audio m-line when answering an offer.

---

{#videodirection}

#### videoDirection

```cpp
rtc::Description::Direction videoDirection = rtc::Description::Direction::SendRecv
```

---

{#audiodirection}

#### audioDirection

```cpp
rtc::Description::Direction audioDirection = rtc::Description::Direction::SendRecv
```

---

{#nackbuffersize}

#### nackBufferSize

```cpp
unsigned nackBufferSize = 512
```

Max RTP packets retained for video NACK retransmission.

{#peersession}

## PeerSession

```cpp
#include <icy/webrtc/peersession.h>
```

Manages a WebRTC peer connection lifecycle over any signalling transport that implements [SignallingInterface](#signallinginterface).

Works with [SympleSignaller](webrtcsupport.md#symplesignaller) (Symple call protocol), [WebSocketSignaller](webrtcsupport.md#websocketsignaller) (plain JSON over WSS), or any custom implementation.

Media is optional. Set `[Config::media](#media-2)` codecs to enable tracks. Leave codec encoders empty for data-channel-only sessions.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ThreadSignal< void(State)>` | [`StateChanged`](#statechanged)  | Emitted whenever the session state changes. Parameter: new [State](base.md#state) value. |
| `ThreadSignal< void(const std::string &)>` | [`IncomingCall`](#incomingcall)  | Emitted when a remote peer initiates a call (state transitions to IncomingInit). Parameter: remote peer identifier. |
| `ThreadSignal< void(rtc::message_variant)>` | [`DataReceived`](#datareceived)  | Emitted when a message arrives on the data channel. Parameter: rtc::message_variant (string or binary). |

---

{#statechanged}

#### StateChanged

```cpp
ThreadSignal< void(State)> StateChanged
```

Emitted whenever the session state changes. Parameter: new [State](base.md#state) value.

---

{#incomingcall}

#### IncomingCall

```cpp
ThreadSignal< void(const std::string &)> IncomingCall
```

Emitted when a remote peer initiates a call (state transitions to IncomingInit). Parameter: remote peer identifier.

---

{#datareceived}

#### DataReceived

```cpp
ThreadSignal< void(rtc::message_variant)> DataReceived
```

Emitted when a message arrives on the data channel. Parameter: rtc::message_variant (string or binary).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`PeerSession`](#peersession-1)  | Construct with any signalling implementation. The signaller must outlive this [PeerSession](#peersession). |
|  | [`PeerSession`](#peersession-2)  | Deleted copy constructor; [PeerSession](#peersession) owns live signalling and RTC callbacks. |
| `void` | [`call`](#call)  | Initiate an outgoing call to a remote peer. Sends a "init" control message and transitions to OutgoingInit. |
| `void` | [`accept`](#accept)  | Accept an incoming call. Creates the PeerConnection, sends "accept", and transitions to Negotiating. |
| `void` | [`reject`](#reject)  | Reject an incoming call. Sends a "reject" control message and transitions to Ended. |
| `void` | [`hangup`](#hangup)  | Terminate any non-idle call phase. Sends a "hangup" control message, closes the PeerConnection, and transitions to Ended. Safe to call from any non-Idle/Ended state. |
| `void` | [`sendData`](#senddata-2)  | Send a UTF-8 string message over the data channel. Silently dropped if the data channel is not open. |
| `void` | [`sendData`](#senddata-3)  | Send raw binary data over the data channel. Silently dropped if the data channel is not open. |
| `State` | [`state`](#state-3) `const` | Current session state. Thread-safe. |
| `std::string` | [`remotePeerId`](#remotepeerid) `const` | Identifier of the remote peer for the current or most recent call. Empty when Idle. |
| `MediaBridge &` | [`media`](#media)  | Media bridge for this session. Valid for the lifetime of the [PeerSession](#peersession). |
| `const MediaBridge &` | [`media`](#media-1) `const` | Media bridge for this session. Valid for the lifetime of the [PeerSession](#peersession). |
| `std::shared_ptr< rtc::PeerConnection >` | [`peerConnection`](#peerconnection)  | The underlying PeerConnection, or nullptr when Idle/Ended. |
| `std::shared_ptr< rtc::DataChannel >` | [`dataChannel`](#datachannel)  | The data channel, or nullptr if none is open. |

---

{#peersession-1}

#### PeerSession

```cpp
PeerSession(SignallingInterface & signaller, const Config & config)
```

Construct with any signalling implementation. The signaller must outlive this [PeerSession](#peersession).

---

{#peersession-2}

#### PeerSession

```cpp
PeerSession(const PeerSession &) = delete
```

Deleted copy constructor; [PeerSession](#peersession) owns live signalling and RTC callbacks.

---

{#call}

#### call

```cpp
void call(const std::string & peerId)
```

Initiate an outgoing call to a remote peer. Sends a "init" control message and transitions to OutgoingInit. 
#### Parameters
* `peerId` Remote peer identifier passed to the signaller. 

#### Exceptions
* `std::logic_error` if not currently in the Idle state.

---

{#accept}

#### accept

```cpp
void accept()
```

Accept an incoming call. Creates the PeerConnection, sends "accept", and transitions to Negotiating. 
#### Exceptions
* `std::logic_error` if not currently in the IncomingInit state.

---

{#reject}

#### reject

```cpp
void reject(const std::string & reason)
```

Reject an incoming call. Sends a "reject" control message and transitions to Ended. 
#### Parameters
* `reason` Human-readable reason string forwarded to the remote peer. 

#### Exceptions
* `std::logic_error` if not currently in the IncomingInit state.

---

{#hangup}

#### hangup

```cpp
void hangup(const std::string & reason)
```

Terminate any non-idle call phase. Sends a "hangup" control message, closes the PeerConnection, and transitions to Ended. Safe to call from any non-Idle/Ended state. 
#### Parameters
* `reason` Human-readable reason string forwarded to the remote peer.

---

{#senddata-2}

#### sendData

```cpp
void sendData(const std::string & message)
```

Send a UTF-8 string message over the data channel. Silently dropped if the data channel is not open. 
#### Parameters
* `message` String to send.

---

{#senddata-3}

#### sendData

```cpp
void sendData(const std::byte * data, size_t size)
```

Send raw binary data over the data channel. Silently dropped if the data channel is not open. 
#### Parameters
* `data` Pointer to the byte buffer. 

* `size` Number of bytes to send.

---

{#state-3}

#### state

`const`

```cpp
State state() const
```

Current session state. Thread-safe.

---

{#remotepeerid}

#### remotePeerId

`const`

```cpp
std::string remotePeerId() const
```

Identifier of the remote peer for the current or most recent call. Empty when Idle.

---

{#media}

#### media

```cpp
MediaBridge & media()
```

Media bridge for this session. Valid for the lifetime of the [PeerSession](#peersession).

---

{#media-1}

#### media

`const`

```cpp
const MediaBridge & media() const
```

Media bridge for this session. Valid for the lifetime of the [PeerSession](#peersession).

---

{#peerconnection}

#### peerConnection

```cpp
std::shared_ptr< rtc::PeerConnection > peerConnection()
```

The underlying PeerConnection, or nullptr when Idle/Ended.

---

{#datachannel}

#### dataChannel

```cpp
std::shared_ptr< rtc::DataChannel > dataChannel()
```

The data channel, or nullptr if none is open.

### Public Types

| Name | Description |
|------|-------------|
| [`State`](#state-4)  | High-level lifecycle phases for a single peer-to-peer call session. |

---

{#state-4}

#### State

```cpp
enum State
```

High-level lifecycle phases for a single peer-to-peer call session.

| Value | Description |
|-------|-------------|
| `Idle` | No active call. |
| `OutgoingInit` | Outgoing call announced, waiting for accept/reject. |
| `IncomingInit` | Incoming call announced, waiting for accept/reject. |
| `Negotiating` | PeerConnection exists and SDP/ICE negotiation is in progress. |
| `Active` | Media or data is flowing. |
| `Ending` | Local teardown is in progress. |
| `Ended` | Call ended (transient; auto-resets to Idle) |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `SignallingInterface &` | [`_signaller`](#_signaller)  |  |
| `Config` | [`_config`](#_config)  |  |
| `MediaBridge` | [`_media`](#_media)  |  |
| `State` | [`_state`](#_state-3)  |  |
| `std::string` | [`_remotePeerId`](#_remotepeerid)  |  |
| `std::shared_ptr< rtc::PeerConnection >` | [`_pc`](#_pc-1)  |  |
| `std::shared_ptr< rtc::DataChannel >` | [`_dc`](#_dc)  |  |
| `std::shared_ptr< CallbackGuard >` | [`_callbackGuard`](#_callbackguard)  |  |
| `bool` | [`_remoteDescriptionSet`](#_remotedescriptionset)  |  |
| `std::vector< PendingCandidate >` | [`_pendingRemoteCandidates`](#_pendingremotecandidates)  |  |
| `Synchronizer` | [`_callbackSync`](#_callbacksync)  |  |
| `std::deque< std::function< void()> >` | [`_pendingCallbacks`](#_pendingcallbacks)  |  |
| `std::mutex` | [`_callbackMutex`](#_callbackmutex)  |  |
| `std::mutex` | [`_mutex`](#_mutex-18)  |  |

---

{#_signaller}

#### _signaller

```cpp
SignallingInterface & _signaller
```

---

{#_config}

#### _config

```cpp
Config _config
```

---

{#_media}

#### _media

```cpp
MediaBridge _media
```

---

{#_state-3}

#### _state

```cpp
State _state = 
```

---

{#_remotepeerid}

#### _remotePeerId

```cpp
std::string _remotePeerId
```

---

{#_pc-1}

#### _pc

```cpp
std::shared_ptr< rtc::PeerConnection > _pc
```

---

{#_dc}

#### _dc

```cpp
std::shared_ptr< rtc::DataChannel > _dc
```

---

{#_callbackguard}

#### _callbackGuard

```cpp
std::shared_ptr< CallbackGuard > _callbackGuard = std::make_shared<CallbackGuard>()
```

---

{#_remotedescriptionset}

#### _remoteDescriptionSet

```cpp
bool _remoteDescriptionSet = false
```

---

{#_pendingremotecandidates}

#### _pendingRemoteCandidates

```cpp
std::vector< PendingCandidate > _pendingRemoteCandidates
```

---

{#_callbacksync}

#### _callbackSync

```cpp
Synchronizer _callbackSync
```

---

{#_pendingcallbacks}

#### _pendingCallbacks

```cpp
std::deque< std::function< void()> > _pendingCallbacks
```

---

{#_callbackmutex}

#### _callbackMutex

```cpp
std::mutex _callbackMutex
```

---

{#_mutex-18}

#### _mutex

```cpp
std::mutex _mutex
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onSdpReceived`](#onsdpreceived)  |  |
| `void` | [`onCandidateReceived`](#oncandidatereceived)  |  |
| `void` | [`onControlReceived`](#oncontrolreceived)  |  |
| `std::shared_ptr< rtc::PeerConnection >` | [`createPeerConnection`](#createpeerconnection)  |  |
| `void` | [`setupPeerConnectionCallbacks`](#setuppeerconnectioncallbacks)  |  |
| `void` | [`beginEndCall`](#beginendcall)  |  |
| `void` | [`finishEndCall`](#finishendcall)  |  |
| `void` | [`transitionEndedToIdle`](#transitionendedtoidle)  |  |
| `void` | [`enqueueCallback`](#enqueuecallback)  |  |
| `void` | [`drainCallbacks`](#draincallbacks)  |  |

---

{#onsdpreceived}

#### onSdpReceived

```cpp
void onSdpReceived(const std::string & peerId, const std::string & type, const std::string & sdp)
```

---

{#oncandidatereceived}

#### onCandidateReceived

```cpp
void onCandidateReceived(const std::string & peerId, const std::string & candidate, const std::string & mid)
```

---

{#oncontrolreceived}

#### onControlReceived

```cpp
void onControlReceived(const std::string & peerId, const std::string & type, const std::string & reason)
```

---

{#createpeerconnection}

#### createPeerConnection

```cpp
std::shared_ptr< rtc::PeerConnection > createPeerConnection(bool createDataChannel, const MediaBridge::Options * mediaOpts)
```

---

{#setuppeerconnectioncallbacks}

#### setupPeerConnectionCallbacks

```cpp
void setupPeerConnectionCallbacks(const std::shared_ptr< rtc::PeerConnection > & pc)
```

---

{#beginendcall}

#### beginEndCall

```cpp
void beginEndCall(const std::string & reason, std::shared_ptr< rtc::PeerConnection > & pc, std::shared_ptr< rtc::DataChannel > & dc)
```

---

{#finishendcall}

#### finishEndCall

```cpp
void finishEndCall()
```

---

{#transitionendedtoidle}

#### transitionEndedToIdle

```cpp
void transitionEndedToIdle()
```

---

{#enqueuecallback}

#### enqueueCallback

```cpp
void enqueueCallback(std::function< void()> callback)
```

---

{#draincallbacks}

#### drainCallbacks

```cpp
void drainCallbacks()
```

{#callbackguard}

## CallbackGuard

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::atomic< bool >` | [`alive`](#alive-1)  |  |

---

{#alive-1}

#### alive

```cpp
std::atomic< bool > alive {true}
```

{#config}

## Config

```cpp
#include <icy/webrtc/peersession.h>
```

[Configuration](base.md#configuration) for WebRTC peer session establishment.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `rtc::Configuration` | [`rtcConfig`](#rtcconfig)  | libdatachannel connection options, ICE servers, and transport settings. |
| `MediaConfig` | [`media`](#media-2)  | Desired media codecs and directions for the session. |
| `bool` | [`enableDataChannel`](#enabledatachannel)  | True to create a data channel on outgoing calls and accept one on incoming calls. |
| `std::string` | [`dataChannelLabel`](#datachannellabel)  | Label to use for the application data channel. |

---

{#rtcconfig}

#### rtcConfig

```cpp
rtc::Configuration rtcConfig
```

libdatachannel connection options, ICE servers, and transport settings.

---

{#media-2}

#### media

```cpp
MediaConfig media
```

Desired media codecs and directions for the session.

---

{#enabledatachannel}

#### enableDataChannel

```cpp
bool enableDataChannel = true
```

True to create a data channel on outgoing calls and accept one on incoming calls.

---

{#datachannellabel}

#### dataChannelLabel

```cpp
std::string dataChannelLabel = "data"
```

Label to use for the application data channel.

{#mediaconfig}

## MediaConfig

```cpp
#include <icy/webrtc/peersession.h>
```

[Configuration](base.md#configuration) for WebRTC peer session establishment.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `av::VideoCodec` | [`videoCodec`](#videocodec-7)  | Desired video codec for send/receive negotiation. |
| `av::AudioCodec` | [`audioCodec`](#audiocodec-6)  | Desired audio codec for send/receive negotiation. |
| `rtc::Description::Direction` | [`videoDirection`](#videodirection-1)  |  |
| `rtc::Description::Direction` | [`audioDirection`](#audiodirection-1)  |  |

---

{#videocodec-7}

#### videoCodec

```cpp
av::VideoCodec videoCodec
```

Desired video codec for send/receive negotiation.

---

{#audiocodec-6}

#### audioCodec

```cpp
av::AudioCodec audioCodec
```

Desired audio codec for send/receive negotiation.

---

{#videodirection-1}

#### videoDirection

```cpp
rtc::Description::Direction videoDirection = rtc::Description::Direction::SendRecv
```

---

{#audiodirection-1}

#### audioDirection

```cpp
rtc::Description::Direction audioDirection = rtc::Description::Direction::SendRecv
```

{#pendingcandidate}

## PendingCandidate

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`candidate`](#candidate)  |  |
| `std::string` | [`mid`](#mid)  |  |

---

{#candidate}

#### candidate

```cpp
std::string candidate
```

---

{#mid}

#### mid

```cpp
std::string mid
```

{#webrtctracksender}

## WebRtcTrackSender

```cpp
#include <icy/webrtc/tracksender.h>
```

> **Inherits:** [`PacketProcessor`](base.md#packetprocessor)

[PacketProcessor](base.md#packetprocessor) that sends encoded media to a single libdatachannel Track via sendFrame().

Bind to one track (video or audio). Accepts the corresponding packet type from the [PacketStream](base.md#packetstream) and converts timestamps from FFmpeg microseconds to the track's RTP clock rate.

Usage: auto vh = createVideoTrack(pc, codec); [WebRtcTrackSender](#webrtctracksender) videoSender(vh);

[PacketStream](base.md#packetstream) stream; stream.attachSource(capture); stream.attach(encoder, 1, true); stream.attach(&videoSender, 5, false); stream.start();

Only emits the packet downstream on successful send, so a chained recorder won't record frames that failed to transmit.

Accepts only the packet type that matches the bound track. Non-matching packets are passed through unchanged so mixed audio/video [PacketStream](base.md#packetstream) chains can share one source cleanly.

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
|  | [`WebRtcTrackSender`](#webrtctracksender-1)  | Construct an unbound sender. Call [bind()](#bind-5) before use. |
|  | [`WebRtcTrackSender`](#webrtctracksender-2) `explicit` | Construct bound to a track handle from [createVideoTrack()](#createvideotrack) or [createAudioTrack()](#createaudiotrack). |
| `void` | [`bind`](#bind-5)  | Bind to a track. Can be called to rebind to a different track. |
| `void` | [`unbind`](#unbind-1)  | Unbind from the current track. |
| `void` | [`process`](#process-7) `virtual` | Send an encoded media frame to the bound WebRTC track. Converts the FFmpeg microsecond timestamp to an RTP timestamp using the track's clock rate, then calls rtc::Track::sendFrame(). Only forwards the packet downstream on a successful send. |
| `bool` | [`accepts`](#accepts-3) `virtual` | Return true only for the packet type that matches the bound track. |
| `void` | [`onStreamStateChange`](#onstreamstatechange-6) `virtual` | Called by the [PacketStream](base.md#packetstream) when stream state changes. Logs when the stream is stopping; no other action is taken. |
| `bool` | [`isVideo`](#isvideo) `const` | True if this sender is bound to a video track. |
| `bool` | [`bound`](#bound) `const` | True if bound to any track. |

---

{#webrtctracksender-1}

#### WebRtcTrackSender

```cpp
WebRtcTrackSender()
```

Construct an unbound sender. Call [bind()](#bind-5) before use.

---

{#webrtctracksender-2}

#### WebRtcTrackSender

`explicit`

```cpp
explicit WebRtcTrackSender(const TrackHandle & handle)
```

Construct bound to a track handle from [createVideoTrack()](#createvideotrack) or [createAudioTrack()](#createaudiotrack).

---

{#bind-5}

#### bind

```cpp
void bind(const TrackHandle & handle)
```

Bind to a track. Can be called to rebind to a different track.

---

{#unbind-1}

#### unbind

```cpp
void unbind()
```

Unbind from the current track.

---

{#process-7}

#### process

`virtual`

```cpp
virtual void process(IPacket & packet)
```

Send an encoded media frame to the bound WebRTC track. Converts the FFmpeg microsecond timestamp to an RTP timestamp using the track's clock rate, then calls rtc::Track::sendFrame(). Only forwards the packet downstream on a successful send. 
#### Parameters
* `packet` An [av::VideoPacket](av.md#videopacket) or [av::AudioPacket](av.md#audiopacket) carrying the encoded frame data and a microsecond timestamp.

---

{#accepts-3}

#### accepts

`virtual`

```cpp
virtual bool accepts(IPacket * packet)
```

Return true only for the packet type that matches the bound track. 
#### Parameters
* `packet` Packet to test. May be nullptr. 

#### Returns
True if the packet can be processed by this sender.

---

{#onstreamstatechange-6}

#### onStreamStateChange

`virtual`

```cpp
virtual void onStreamStateChange(const PacketStreamState & state)
```

Called by the [PacketStream](base.md#packetstream) when stream state changes. Logs when the stream is stopping; no other action is taken. 
#### Parameters
* `state` New [PacketStream](base.md#packetstream) state.

---

{#isvideo}

#### isVideo

`const`

```cpp
bool isVideo() const
```

True if this sender is bound to a video track.

---

{#bound}

#### bound

`const`

```cpp
bool bound() const
```

True if bound to any track.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::shared_ptr< rtc::Track >` | [`_track`](#_track)  |  |
| `std::shared_ptr< rtc::RtpPacketizationConfig >` | [`_rtpConfig`](#_rtpconfig)  |  |
| `std::atomic< TrackKind >` | [`_kind`](#_kind)  |  |
| `std::mutex` | [`_mutex`](#_mutex-19)  |  |

---

{#_track}

#### _track

```cpp
std::shared_ptr< rtc::Track > _track
```

---

{#_rtpconfig}

#### _rtpConfig

```cpp
std::shared_ptr< rtc::RtpPacketizationConfig > _rtpConfig
```

---

{#_kind}

#### _kind

```cpp
std::atomic< TrackKind > _kind {TrackKind::Unbound}
```

---

{#_mutex-19}

#### _mutex

```cpp
std::mutex _mutex
```

{#webrtctrackreceiver}

## WebRtcTrackReceiver

```cpp
#include <icy/webrtc/trackreceiver.h>
```

> **Inherits:** [`PacketStreamAdapter`](base.md#packetstreamadapter)

[PacketStreamAdapter](base.md#packetstreamadapter) that receives depacketized frames from a single remote libdatachannel Track and emits them as VideoPacket or AudioPacket into a [PacketStream](base.md#packetstream).

Call [setupReceiveTrack()](#setupreceivetrack) on the track first to install the correct depacketizer, then bind this receiver to it.

Usage: pc->onTrack([&](shared_ptr<rtc::Track> track) { if (setupReceiveTrack(track)) videoReceiver.bind(track); });

videoReceiver.emitter += packetSlot(&recorder, &Recorder::onEncodedVideo);

Emits owning packets - the frame data is copied, so downstream processors can safely queue packets asynchronously.

Emits VideoPacket for video tracks, AudioPacket for audio tracks. Use those packets to drive a decoder, recorder, or custom pipeline. See `samples/media-recorder` for a complete receive -> decode -> file example.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-9)  |  |

---

{#emitter-9}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WebRtcTrackReceiver`](#webrtctrackreceiver-1)  | Construct an unbound receiver. Call [bind()](#bind-6) to attach a remote track. |
| `void` | [`bind`](#bind-6)  | Bind to a remote track. Must be called after [setupReceiveTrack()](#setupreceivetrack) returned true. Installs an onFrame callback that converts each depacketized frame to a VideoPacket or AudioPacket and emits it on the [PacketStream](base.md#packetstream). The track type (video/audio) is detected from the SDP description. |

---

{#webrtctrackreceiver-1}

#### WebRtcTrackReceiver

```cpp
WebRtcTrackReceiver()
```

Construct an unbound receiver. Call [bind()](#bind-6) to attach a remote track.

---

{#bind-6}

#### bind

```cpp
void bind(std::shared_ptr< rtc::Track > track)
```

Bind to a remote track. Must be called after [setupReceiveTrack()](#setupreceivetrack) returned true. Installs an onFrame callback that converts each depacketized frame to a VideoPacket or AudioPacket and emits it on the [PacketStream](base.md#packetstream). The track type (video/audio) is detected from the SDP description. 
#### Parameters
* `track` Remote track from the PeerConnection::onTrack callback.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Synchronizer` | [`_dispatch`](#_dispatch)  |  |
| `std::mutex` | [`_mutex`](#_mutex-20)  |  |
| `std::deque< std::unique_ptr< IPacket > >` | [`_pending`](#_pending)  |  |
| `std::shared_ptr< DispatchState >` | [`_state`](#_state-4)  |  |
| `uint64_t` | [`_generation`](#_generation)  |  |

---

{#_dispatch}

#### _dispatch

```cpp
Synchronizer _dispatch
```

---

{#_mutex-20}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_pending}

#### _pending

```cpp
std::deque< std::unique_ptr< IPacket > > _pending
```

---

{#_state-4}

#### _state

```cpp
std::shared_ptr< DispatchState > _state = std::make_shared<DispatchState>()
```

---

{#_generation}

#### _generation

```cpp
uint64_t _generation = 0
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`enqueue`](#enqueue)  |  |
| `void` | [`flushPending`](#flushpending)  |  |

---

{#enqueue}

#### enqueue

```cpp
void enqueue(std::unique_ptr< IPacket > packet)
```

---

{#flushpending}

#### flushPending

```cpp
void flushPending()
```

{#dispatchstate}

## DispatchState

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::atomic< bool >` | [`alive`](#alive-2)  |  |
| `std::atomic< uint64_t >` | [`generation`](#generation)  |  |

---

{#alive-2}

#### alive

```cpp
std::atomic< bool > alive {true}
```

---

{#generation}

#### generation

```cpp
std::atomic< uint64_t > generation {0}
```

{#codecnegotiator}

## CodecNegotiator

```cpp
#include <icy/webrtc/codecnegotiator.h>
```

Maps RTP codec names to FFmpeg encoders and queries FFmpeg at runtime to determine what codecs are available.

This is stateless - all methods are static. Call negotiate*() with a list of RTP codec names offered by the remote peer, and it returns the best match that FFmpeg can encode.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::optional< NegotiatedCodec >` | [`negotiateVideo`](#negotiatevideo) `static` | Negotiate the best video codec from a list of offered RTP codec names. Returns the first match that FFmpeg can encode, in preference order: H264 > VP8 > VP9 > AV1 > H265 |
| `std::optional< NegotiatedCodec >` | [`negotiateAudio`](#negotiateaudio) `static` | Negotiate the best audio codec from a list of offered RTP codec names. Returns the first match that FFmpeg can encode, in preference order: opus > PCMU > PCMA |
| `bool` | [`hasEncoder`](#hasencoder) `static` | Check if FFmpeg has an encoder for the given codec name. Accepts both FFmpeg names ("libx264") and RTP names ("H264"). |
| `std::string` | [`rtpToFfmpeg`](#rtptoffmpeg) `static` | Map an RTP codec name to the preferred FFmpeg encoder name. Returns empty string if no mapping exists. |
| `std::string` | [`ffmpegToRtp`](#ffmpegtortp) `static` | Map an FFmpeg encoder name to the RTP codec name. Returns empty string if no mapping exists. |
| `uint32_t` | [`clockRate`](#clockrate-2) `static` | Get the standard RTP clock rate for a codec. |
| `int` | [`defaultPayloadType`](#defaultpayloadtype) `static` | Get the default RTP payload type for a codec. Returns 0 for dynamic payload types (caller should assign). |
| `std::optional< CodecSpec >` | [`specFromRtp`](#specfromrtp) `static` | Return the canonical codec spec for an RTP name, if known. |
| `std::optional< CodecSpec >` | [`specFromFfmpeg`](#specfromffmpeg) `static` | Return the canonical codec spec for an FFmpeg encoder name, if known. |
| `std::optional< CodecSpec >` | [`specFromVideoCodec`](#specfromvideocodec) `static` | Resolve the canonical codec spec from an explicit video codec config. Prefers the FFmpeg encoder when present, otherwise falls back to RTP name. |
| `std::optional< CodecSpec >` | [`specFromAudioCodec`](#specfromaudiocodec) `static` | Resolve the canonical codec spec from an explicit audio codec config. Prefers the FFmpeg encoder when present, otherwise falls back to RTP name. |
| `CodecSpec` | [`requireVideoSpec`](#requirevideospec) `static` | Resolve a strict canonical video codec spec or throw. |
| `CodecSpec` | [`requireAudioSpec`](#requireaudiospec) `static` | Resolve a strict canonical audio codec spec or throw. |
| `av::VideoCodec` | [`resolveWebRtcVideoCodec`](#resolvewebrtcvideocodec) `static` | Resolve a browser-safe WebRTC video codec config from an explicit codec. |
| `av::AudioCodec` | [`resolveWebRtcAudioCodec`](#resolvewebrtcaudiocodec) `static` | Resolve a browser-safe WebRTC audio codec config from an explicit codec. |
| `std::optional< CodecSpec >` | [`detectCodec`](#detectcodec) `static` | Detect the first known codec present in an SDP snippet for the given media type. |

---

{#negotiatevideo}

#### negotiateVideo

`static`

```cpp
static std::optional< NegotiatedCodec > negotiateVideo(const std::vector< std::string > & offeredCodecs)
```

Negotiate the best video codec from a list of offered RTP codec names. Returns the first match that FFmpeg can encode, in preference order: H264 > VP8 > VP9 > AV1 > H265

---

{#negotiateaudio}

#### negotiateAudio

`static`

```cpp
static std::optional< NegotiatedCodec > negotiateAudio(const std::vector< std::string > & offeredCodecs)
```

Negotiate the best audio codec from a list of offered RTP codec names. Returns the first match that FFmpeg can encode, in preference order: opus > PCMU > PCMA

---

{#hasencoder}

#### hasEncoder

`static`

```cpp
static bool hasEncoder(const std::string & name)
```

Check if FFmpeg has an encoder for the given codec name. Accepts both FFmpeg names ("libx264") and RTP names ("H264").

---

{#rtptoffmpeg}

#### rtpToFfmpeg

`static`

```cpp
static std::string rtpToFfmpeg(const std::string & rtpName)
```

Map an RTP codec name to the preferred FFmpeg encoder name. Returns empty string if no mapping exists.

---

{#ffmpegtortp}

#### ffmpegToRtp

`static`

```cpp
static std::string ffmpegToRtp(const std::string & ffmpegName)
```

Map an FFmpeg encoder name to the RTP codec name. Returns empty string if no mapping exists.

---

{#clockrate-2}

#### clockRate

`static`

```cpp
static uint32_t clockRate(const std::string & rtpName)
```

Get the standard RTP clock rate for a codec.

---

{#defaultpayloadtype}

#### defaultPayloadType

`static`

```cpp
static int defaultPayloadType(const std::string & rtpName)
```

Get the default RTP payload type for a codec. Returns 0 for dynamic payload types (caller should assign).

---

{#specfromrtp}

#### specFromRtp

`static`

```cpp
static std::optional< CodecSpec > specFromRtp(const std::string & rtpName)
```

Return the canonical codec spec for an RTP name, if known.

---

{#specfromffmpeg}

#### specFromFfmpeg

`static`

```cpp
static std::optional< CodecSpec > specFromFfmpeg(const std::string & ffmpegName)
```

Return the canonical codec spec for an FFmpeg encoder name, if known.

---

{#specfromvideocodec}

#### specFromVideoCodec

`static`

```cpp
static std::optional< CodecSpec > specFromVideoCodec(const av::VideoCodec & codec)
```

Resolve the canonical codec spec from an explicit video codec config. Prefers the FFmpeg encoder when present, otherwise falls back to RTP name.

---

{#specfromaudiocodec}

#### specFromAudioCodec

`static`

```cpp
static std::optional< CodecSpec > specFromAudioCodec(const av::AudioCodec & codec)
```

Resolve the canonical codec spec from an explicit audio codec config. Prefers the FFmpeg encoder when present, otherwise falls back to RTP name.

---

{#requirevideospec}

#### requireVideoSpec

`static`

```cpp
static CodecSpec requireVideoSpec(const av::VideoCodec & codec)
```

Resolve a strict canonical video codec spec or throw.

---

{#requireaudiospec}

#### requireAudioSpec

`static`

```cpp
static CodecSpec requireAudioSpec(const av::AudioCodec & codec)
```

Resolve a strict canonical audio codec spec or throw.

---

{#resolvewebrtcvideocodec}

#### resolveWebRtcVideoCodec

`static`

```cpp
static av::VideoCodec resolveWebRtcVideoCodec(const av::VideoCodec & codec)
```

Resolve a browser-safe WebRTC video codec config from an explicit codec.

---

{#resolvewebrtcaudiocodec}

#### resolveWebRtcAudioCodec

`static`

```cpp
static av::AudioCodec resolveWebRtcAudioCodec(const av::AudioCodec & codec)
```

Resolve a browser-safe WebRTC audio codec config from an explicit codec.

---

{#detectcodec}

#### detectCodec

`static`

```cpp
static std::optional< CodecSpec > detectCodec(std::string_view sdp, CodecMediaType mediaType)
```

Detect the first known codec present in an SDP snippet for the given media type.

{#trackhandle}

## TrackHandle

```cpp
#include <icy/webrtc/track.h>
```

Result of creating a track: the track itself plus its RTP config. Keep the config around - you need it for [WebRtcTrackSender](#webrtctracksender).

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::shared_ptr< rtc::Track >` | [`track`](#track)  | The libdatachannel track added to the PeerConnection. |
| `std::shared_ptr< rtc::RtpPacketizationConfig >` | [`rtpConfig`](#rtpconfig)  | RTP packetization state required by [WebRtcTrackSender](#webrtctracksender). |

---

{#track}

#### track

```cpp
std::shared_ptr< rtc::Track > track
```

The libdatachannel track added to the PeerConnection.

---

{#rtpconfig}

#### rtpConfig

```cpp
std::shared_ptr< rtc::RtpPacketizationConfig > rtpConfig
```

RTP packetization state required by [WebRtcTrackSender](#webrtctracksender).

{#codecspec}

## CodecSpec

```cpp
#include <icy/webrtc/codecnegotiator.h>
```

Canonical description of a codec supported by Icey's WebRTC helpers.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `CodecId` | [`id`](#id-9)  | Stable codec identifier. |
| `CodecMediaType` | [`mediaType`](#mediatype)  | Audio or video media kind. |
| `std::string` | [`rtpName`](#rtpname)  | Canonical RTP codec name. |
| `std::string` | [`ffmpegName`](#ffmpegname)  | Preferred FFmpeg encoder name. |
| `uint32_t` | [`clockRate`](#clockrate)  | RTP clock rate in Hz. |
| `int` | [`payloadType`](#payloadtype)  | Default static or preferred dynamic payload type. |
| `std::string` | [`fmtp`](#fmtp)  | Canonical fmtp line for SDP generation. |

---

{#id-9}

#### id

```cpp
CodecId id = 
```

Stable codec identifier.

---

{#mediatype}

#### mediaType

```cpp
CodecMediaType mediaType = 
```

Audio or video media kind.

---

{#rtpname}

#### rtpName

```cpp
std::string rtpName
```

Canonical RTP codec name.

---

{#ffmpegname}

#### ffmpegName

```cpp
std::string ffmpegName
```

Preferred FFmpeg encoder name.

---

{#clockrate}

#### clockRate

```cpp
uint32_t clockRate = 0
```

RTP clock rate in Hz.

---

{#payloadtype}

#### payloadType

```cpp
int payloadType = 0
```

Default static or preferred dynamic payload type.

---

{#fmtp}

#### fmtp

```cpp
std::string fmtp
```

Canonical fmtp line for SDP generation.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`valid`](#valid-15) `const` `inline` |  |

---

{#valid-15}

#### valid

`const` `inline`

```cpp
inline bool valid() const
```

{#negotiatedcodec}

## NegotiatedCodec

```cpp
#include <icy/webrtc/codecnegotiator.h>
```

Result of codec negotiation between a remote SDP offer and the local FFmpeg codec inventory.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`rtpName`](#rtpname-1)  | RTP codec name (e.g. "H264", "VP8", "opus") |
| `std::string` | [`ffmpegName`](#ffmpegname-1)  | FFmpeg encoder name (e.g. "libx264", "libvpx", "libopus") |
| `int` | [`payloadType`](#payloadtype-1)  | RTP payload type from SDP. |
| `uint32_t` | [`clockRate`](#clockrate-1)  | RTP clock rate (90000 for video, 48000 for opus) |
| `std::string` | [`fmtp`](#fmtp-1)  | Format parameters (e.g. "profile-level-id=42e01f") |

---

{#rtpname-1}

#### rtpName

```cpp
std::string rtpName
```

RTP codec name (e.g. "H264", "VP8", "opus")

---

{#ffmpegname-1}

#### ffmpegName

```cpp
std::string ffmpegName
```

FFmpeg encoder name (e.g. "libx264", "libvpx", "libopus")

---

{#payloadtype-1}

#### payloadType

```cpp
int payloadType = 0
```

RTP payload type from SDP.

---

{#clockrate-1}

#### clockRate

```cpp
uint32_t clockRate = 0
```

RTP clock rate (90000 for video, 48000 for opus)

---

{#fmtp-1}

#### fmtp

```cpp
std::string fmtp
```

Format parameters (e.g. "profile-level-id=42e01f")

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `av::VideoCodec` | [`toVideoCodec`](#tovideocodec) `const` | Create an [av::VideoCodec](av.md#videocodec) from this negotiation result. Width, height, fps default to 0 (caller should set these). |
| `av::VideoCodec` | [`toWebRtcVideoCodec`](#towebrtcvideocodec) `const` | Create an [av::VideoCodec](av.md#videocodec) configured for WebRTC browser playback. Sets low-latency options: ultrafast preset, zerolatency tune, constrained baseline profile for H.264, and appropriate defaults for VP8/VP9/AV1. |
| `av::AudioCodec` | [`toAudioCodec`](#toaudiocodec) `const` | Create an [av::AudioCodec](av.md#audiocodec) from this negotiation result. Channels default to 2, sampleRate to the RTP clock rate. |
| `av::AudioCodec` | [`toWebRtcAudioCodec`](#towebrtcaudiocodec) `const` | Create an [av::AudioCodec](av.md#audiocodec) configured for WebRTC browser playback. Forces 48000 Hz for Opus, sets appropriate options. |

---

{#tovideocodec}

#### toVideoCodec

`const`

```cpp
av::VideoCodec toVideoCodec(int width, int height, double fps) const
```

Create an [av::VideoCodec](av.md#videocodec) from this negotiation result. Width, height, fps default to 0 (caller should set these).

---

{#towebrtcvideocodec}

#### toWebRtcVideoCodec

`const`

```cpp
av::VideoCodec toWebRtcVideoCodec(int width, int height, double fps) const
```

Create an [av::VideoCodec](av.md#videocodec) configured for WebRTC browser playback. Sets low-latency options: ultrafast preset, zerolatency tune, constrained baseline profile for H.264, and appropriate defaults for VP8/VP9/AV1.

---

{#toaudiocodec}

#### toAudioCodec

`const`

```cpp
av::AudioCodec toAudioCodec(int channels, int sampleRate) const
```

Create an [av::AudioCodec](av.md#audiocodec) from this negotiation result. Channels default to 2, sampleRate to the RTP clock rate.

---

{#towebrtcaudiocodec}

#### toWebRtcAudioCodec

`const`

```cpp
av::AudioCodec toWebRtcAudioCodec(int channels) const
```

Create an [av::AudioCodec](av.md#audiocodec) configured for WebRTC browser playback. Forces 48000 Hz for Opus, sets appropriate options.

