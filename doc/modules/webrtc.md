# WebRTC

> WebRTC media transport built on libdatachannel; peer sessions, codec negotiation, and PacketStream integration without Google's libwebrtc.

**[Source →](../../src/webrtc/)**

**CMake target**: `Icey::webrtc`
**Dependencies**: `Icey::base`, `Icey::net`, `Icey::crypto`, `Icey::av`, `Icey::symple`, libdatachannel (auto-fetched), OpenSSL 3.x, FFmpeg 5+
**Licence**: LGPL-2.1+

```cmake
target_link_libraries(myapp PRIVATE Icey::webrtc)
```

---

## Overview

The WebRTC module solves one problem: getting encoded media frames between FFmpeg and a browser (or another C++ peer) over a standards-compliant WebRTC connection, with all the transport machinery handled for you. RTP packetization, NACK retransmission, PLI/REMB RTCP feedback, ICE candidate exchange, DTLS handshake, SRTP keying; none of that is your problem.

What you write is the application logic: what to capture, when to accept a call, what to do when media arrives.

We use [libdatachannel](https://github.com/paullouisageneau/libdatachannel) as the transport pipe. Icey provides the media, signalling, and pipeline glue.

**libdatachannel owns:**

- ICE negotiation (via libjuice)
- DTLS handshake
- SRTP encryption and decryption
- SCTP (data channels)
- RTP packetization and depacketization
- RTCP: SR/RR, NACK, PLI, REMB

**Icey owns:**

- Media capture and FFmpeg encode/decode (`av` module)
- PacketStream pipeline integration
- Codec negotiation between RTP names and FFmpeg encoder names
- Signalling (SDP and ICE candidate exchange) via `SignallingInterface`
- TURN relay server (`turn` module)
- Call lifecycle state machine (`PeerSession`)

---

## Architecture

Three independent layers. Use only what you need; lower layers are fully accessible without the higher ones.

```text
Application
  |
  +-- Signalling transport (SympleSignaller / WebSocketSignaller / custom)
  |
  +-- PeerSession                      Layer 3: call lifecycle state machine
  |     |
  |     +-- MediaBridge                Layer 3: multi-track send/receive wrapper
  |           |
  |           +-- WebRtcTrackSender    Layer 2: PacketProcessor (send side)
  |           +-- WebRtcTrackReceiver  Layer 2: PacketStreamAdapter (receive side)
  |           +-- createVideoTrack()   Layer 1: track + handler chain factory
  |           +-- createAudioTrack()   Layer 1: track + handler chain factory
  |           +-- setupReceiveTrack()  Layer 1: strict depacketizer installation
  |
  +-- AV module (FFmpeg capture / encode / decode / mux)
  +-- TURN module (relay for symmetric NATs)
  |
  [libdatachannel: ICE / DTLS / SRTP / SCTP / RTP / RTCP]
```

**Layer 1** (`track.h`): factory functions that create a libdatachannel track and wire up the correct RTP packetizer/depacketizer handler chain. Use this layer directly when you need full control over individual tracks or want to bypass `MediaBridge` entirely.

**Layer 2** (`tracksender.h`, `trackreceiver.h`): `PacketProcessor` and `PacketStreamAdapter` adapters that connect a libdatachannel track to a `PacketStream`. One sender or receiver per track. This is where FFmpeg timestamps are converted to RTP clock rates.

**Layer 3** (`mediabridge.h`, `peersession.h`): convenience wrappers for the common case. `MediaBridge` creates tracks and adapters for up to one video and one audio track. `PeerSession` adds call signalling over any `SignallingInterface` and drives the call state machine.

### Full send path

```text
av::MediaCapture
    |
    v
av::VideoEncoder          (FFmpeg encode to H.264/VP8/etc.)
    |
    v
wrtc::WebRtcTrackSender   (timestamp conversion, sendFrame)
    |
    v
[libdatachannel]
    +--> H264RtpPacketizer  (NAL unit -> RTP packets)
    +--> RtcpSrReporter     (sender reports)
    +--> RtcpNackResponder  (retransmit on NACK)
    +--> PliHandler         (PLI -> forceKeyframe callback)
    +--> RembHandler        (REMB -> setBitrate callback)
    |
    v
ICE / DTLS-SRTP / network -> browser
```

### Full receive path

```text
browser -> network / ICE / DTLS-SRTP
    |
    v
[libdatachannel]
    +--> H264RtpDepacketizer  (RTP packets -> NAL units)
    +--> RtcpReceivingSession (receiver reports)
    |
    v
wrtc::WebRtcTrackReceiver   (onFrame -> VideoPacket/AudioPacket)
    |
    v
av::VideoDecoder            (FFmpeg decode)
    |
    v
av::MultiplexPacketEncoder  (FFmpeg mux -> file / stream)
```

---

## Usage

### PeerSession Lifecycle

`PeerSession` is the top-level object for a single WebRTC call. It owns a `MediaBridge` and an optional data channel, drives the call state machine, and delegates signalling to whatever `SignallingInterface` you provide.

#### State machine

```text
              call()                 remote accept
  Idle ──────────────► OutgoingInit ───────────────► Negotiating ──ICE connected──► Active
    ▲                     ▲                              ▲                             |
    |                     |                              |                             |
    |                IncomingCall                    accept()                          |
    |                     |                              |                             |
    +──── IncomingInit ◄──┘                hangup / reject / failure                  |
                                             │                                         |
                                             └──────────► Ending ─► Ended ────────────┘
```

| State | Meaning |
| ----- | ------- |
| `Idle` | No active call; ready to place or receive one. |
| `OutgoingInit` | Outgoing call sent (`call:init`); waiting for remote `accept` or `reject`. |
| `IncomingInit` | Remote initiated a call; waiting for `accept()` or `reject()`. |
| `Negotiating` | PeerConnection exists and SDP/ICE negotiation is in progress. |
| `Active` | ICE connected; media is flowing. Wire up your `PacketStream` here. |
| `Ending` | Local teardown is in progress; sockets and tracks are being shut down. |
| `Ended` | Call ended or failed; automatically resets to `Idle`. |

`stateToString(state)` returns a lowercase C string (`"idle"`, `"outgoing-init"`, `"incoming-init"`, `"negotiating"`, `"active"`, `"ending"`, `"ended"`) for logging.

#### PeerSession config

```cpp
#include "icy/webrtc/peersession.h"

wrtc::PeerSession::Config config;

// ICE servers. STUN for NAT traversal, TURN for relay through symmetric NATs.
// See the turn module for running your own TURN server.
config.rtcConfig.iceServers.emplace_back("stun:stun.example.com:3478");
config.rtcConfig.iceServers.emplace_back("turn:turn.example.com:3478");

// Media tracks. Leave encoder empty to skip that track.
// Video-only: omit audioCodec. Audio-only: omit videoCodec.
// Data channel only: omit both.
config.mediaOpts.videoCodec = av::VideoCodec("H264", "libx264", 1280, 720, 30);
config.mediaOpts.audioCodec = av::AudioCodec("opus", "libopus", 2, 48000);

// NACK buffer: number of sent packets retained for retransmission.
config.mediaOpts.nackBufferSize = 512;

// Data channel.
config.enableDataChannel = true;
config.dataChannelLabel = "data";
```

### Making and Receiving Calls

#### Making an outgoing call

```cpp
#include "icy/webrtc/peersession.h"
#include "symplesignaller.h"  // Note: this header is in samples/, not the library include path

smpl::Client client(opts);
client.connect();

wrtc::SympleSignaller signaller(client);
wrtc::PeerSession session(signaller, config);

session.StateChanged += [&](wrtc::PeerSession::State state) {
    std::cout << "State: " << wrtc::stateToString(state) << '\n';
    if (state == wrtc::PeerSession::State::Active) {
        // ICE connected. Start the media pipeline.
        startStreaming(session);
    }
    else if (state == wrtc::PeerSession::State::Ended) {
        stream.stop();
    }
};

// Sends call:init to the remote peer. Transitions to OutgoingInit.
session.call("remote-peer-id");
```

When the remote sends `accept`, `PeerSession` creates the `PeerConnection`, attaches media immediately, and triggers the SDP offer. ICE candidates flow over the signaller in the background. When ICE connects, `StateChanged` fires with `Active`.

#### Receiving an incoming call

```cpp
session.IncomingCall += [&](const std::string& peerId) {
    std::cout << "Call from " << peerId << '\n';
    session.accept();   // or session.reject("busy")
};
```

On `accept()`, `PeerSession` creates the `PeerConnection`, attaches media, sends `call:accept` to the remote, and waits for the remote's SDP offer. When it arrives we send an answer. ICE runs in the background; `Active` fires when connected.

#### Teardown

```cpp
session.hangup();          // sends hangup to remote, closes PeerConnection
// The PeerSession destructor calls hangup("destroyed") automatically if needed
```

#### Signals

| Signal | Parameters | When |
| ------ | ---------- | ---- |
| `StateChanged` | `PeerSession::State` | Any state transition |
| `IncomingCall` | `const std::string& peerId` | Remote sends `call:init` |
| `DataReceived` | `rtc::message_variant` | Data channel message arrives |

#### Accessors

```cpp
session.state();           // PeerSession::State (thread-safe)
session.remotePeerId();    // std::string; empty when Idle
session.media();           // MediaBridge& (always valid)
session.peerConnection();  // shared_ptr<rtc::PeerConnection>; nullptr when Idle
session.dataChannel();     // shared_ptr<rtc::DataChannel>; nullptr if none open
```

### Media Tracks (Send and Receive)

After the session reaches `Active`, wire the `MediaBridge`'s senders and receivers into your `PacketStream`.

#### Sending video to the remote peer

```cpp
#include "icy/av/mediacapture.h"
#include "icy/av/videoencoder.h"
#include "icy/packetstream.h"

// In StateChanged handler, when state == Active:
void startStreaming(wrtc::PeerSession& session)
{
    auto capture = std::make_shared<av::MediaCapture>();
    capture->openFile("video.mp4");
    capture->setLoopInput(true);
    capture->setLimitFramerate(true);

    auto encoder = std::make_shared<av::VideoEncoder>(
        av::VideoCodec("H264", "libx264", 1280, 720, 30));

    PacketStream stream;
    stream.attachSource(capture.get(), false, true);
    stream.attach(encoder, 1, true);
    stream.attach(&session.media().videoSender(), 5, false);
    stream.start();

    capture->start();
}
```

The `videoSender()` is a `WebRtcTrackSender` (a `PacketProcessor`). It sits at the end of the pipeline, converts FFmpeg microsecond timestamps to RTP clock-rate timestamps, and calls `rtc::Track::sendFrame()`.

#### Receiving video from the remote peer

```cpp
// In StateChanged handler, when state == Active:
void startRecording(wrtc::PeerSession& session)
{
    auto decoder = std::make_shared<av::VideoDecoder>();
    auto mux = std::make_shared<av::MultiplexPacketEncoder>("recording.mp4");

    PacketStream stream;
    stream.attachSource(&session.media().videoReceiver(), false, true);
    stream.attach(decoder, 1, true);
    stream.attach(mux, 5, true);
    stream.start();
}
```

`videoReceiver()` is a `WebRtcTrackReceiver` (a `PacketStreamAdapter`). It emits `av::VideoPacket` instances with owning copies of the frame data; downstream processors can safely queue them asynchronously.

#### Sending and receiving audio

```cpp
// Send audio (attach after audio encoder)
stream.attach(&session.media().audioSender(), 5, false);

// Receive audio (attach as source before audio decoder)
stream.attachSource(&session.media().audioReceiver(), false, true);
```

Audio receivers emit `av::AudioPacket`. The codec is whatever was negotiated in the SDP.

#### RTCP feedback

Connect these before the call reaches `Active` so the encoder adapts from the start.

```cpp
// PLI: remote detected packet loss, needs a keyframe.
session.media().KeyframeRequested += [&]() {
    encoder->forceKeyframe();
};

// REMB: remote reports its estimated receive bandwidth.
session.media().BitrateEstimate += [&](unsigned int bps) {
    std::cout << "REMB: " << bps / 1000 << " kbps\n";
    encoder->setBitrate(bps);
};

// You can also request these from your side:
session.media().requestKeyframe();         // send PLI to remote
session.media().requestBitrate(500000);    // send TMMBR at 500 kbps
```

### Codec Negotiation

`CodecNegotiator` maps between RTP codec names and FFmpeg encoder names, and queries FFmpeg at runtime via `avcodec_find_encoder_by_name()` to verify what your build actually supports.

#### Codec table

| RTP name | FFmpeg encoders (in preference order) | Clock rate | Default PT |
| -------- | ------------------------------------- | ---------- | ---------- |
| H264 | libx264, h264_nvenc, h264_vaapi | 90000 | 96 |
| VP8 | libvpx | 90000 | 97 |
| VP9 | libvpx-vp9 | 90000 | 98 |
| AV1 | libaom-av1, libsvtav1, av1_nvenc | 90000 | 99 |
| H265 | libx265, hevc_nvenc | 90000 | 100 |
| opus | libopus | 48000 | 111 |
| PCMU | pcm_mulaw | 8000 | 0 |
| PCMA | pcm_alaw | 8000 | 8 |
| G722 | g722 | 8000 | 9 |

Hardware encoders (`h264_nvenc`, `hevc_nvenc`, `av1_nvenc`) appear lower in the preference order than software encoders by default. Edit the table in `codecnegotiator.cpp` to prefer hardware when available.

#### Negotiating from a remote SDP offer

```cpp
#include "icy/webrtc/codecnegotiator.h"

// Browser offered H264, VP8, VP9. negotiateVideo() picks the first one
// FFmpeg can actually encode.
auto result = wrtc::CodecNegotiator::negotiateVideo({"H264", "VP8", "VP9"});
if (result) {
    // result->rtpName    == "H264"
    // result->ffmpegName == "libx264"  (or h264_nvenc if nvenc is present and preferred)
    // result->clockRate  == 90000

    av::VideoCodec codec = result->toVideoCodec(1280, 720, 30);
    // -> codec.name == "H264", codec.encoder == "libx264"
}

auto audioResult = wrtc::CodecNegotiator::negotiateAudio({"opus", "PCMU"});
if (audioResult) {
    av::AudioCodec codec = audioResult->toAudioCodec(2, 48000);
}
```

The negotiation preference order for video is H264 > VP8 > VP9 > AV1 > H265. For audio it is opus > PCMU > PCMA. These are independent of the order in the offered list.

#### Direct lookup

```cpp
wrtc::CodecNegotiator::rtpToFfmpeg("H264");       // "libx264" (first available)
wrtc::CodecNegotiator::ffmpegToRtp("libopus");    // "opus"
wrtc::CodecNegotiator::clockRate("H264");          // 90000
wrtc::CodecNegotiator::defaultPayloadType("opus"); // 111
wrtc::CodecNegotiator::hasEncoder("libx264");      // true if FFmpeg built with x264
wrtc::CodecNegotiator::hasEncoder("H264");         // also works with RTP names
```

### Signalling

Signalling is the out-of-band channel that exchanges SDP offers/answers and ICE candidates between peers before the DTLS connection is established. We separate it from the transport deliberately; `SignallingInterface` is a pure virtual class and `PeerSession` takes a reference to it, so you can plug in any transport.

The three message categories:

- **SDP**: offer/answer exchange. Contains the session description (codecs, ICE credentials, DTLS fingerprint).
- **Candidate**: trickle ICE candidates. Sent as they are discovered during connection.
- **Control**: call lifecycle. `init`, `accept`, `reject`, `hangup`.

#### Using SympleSignaller

`SympleSignaller` wraps a `smpl::Client` and speaks the `call:*` message protocol defined in `symple-player`'s `CallManager.js`. It is wire-compatible with any browser running `symple-client`.

```cpp
#include "symplesignaller.h"  // Note: this header is in samples/, not the library include path
#include "icy/symple/client.h"

smpl::Client::Options opts;
opts.host = "signalling.example.com";
opts.port = 4500;
opts.user = "streamer";
opts.name = "My Streamer";

smpl::Client client(opts);
client.Announce += [](const int& status) {
    if (status != 200)
        std::cerr << "Auth failed: " << status << '\n';
};
client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
    if (state.id() == smpl::ClientState::Online)
        client.joinRoom("public");
};
client.connect();

wrtc::SympleSignaller signaller(client);
wrtc::PeerSession session(signaller, config);
```

The Symple message `subtype` field carries the call action: `call:init`, `call:accept`, `call:reject`, `call:hangup`, `call:offer`, `call:answer`, `call:candidate`. SDP and ICE data travel in a `data` sub-object. See the `symple` module documentation for the broader protocol.

#### Using WebSocketSignaller

`WebSocketSignaller` uses plain JSON over any WebSocket connection. No Symple, no Socket.IO. The schema is minimal:

```json
{"type": "offer",     "peerId": "...", "sdp": "..."}
{"type": "answer",    "peerId": "...", "sdp": "..."}
{"type": "candidate", "peerId": "...", "candidate": "...", "mid": "..."}
{"type": "init",      "peerId": "..."}
{"type": "accept",    "peerId": "..."}
{"type": "reject",    "peerId": "...", "reason": "..."}
{"type": "hangup",    "peerId": "...", "reason": "..."}
```

```cpp
#include "wssignaller.h"

wrtc::WebSocketSignaller signaller("my-peer-id");

// Wire outgoing messages to your WebSocket write path.
signaller.SendMessage += [&ws](const std::string& json) {
    ws.send(json);
};

// Feed incoming WebSocket messages into the signaller.
ws.onMessage([&signaller](const std::string& json) {
    signaller.receive(json);
});

wrtc::PeerSession session(signaller, config);
```

`receive()` parses the JSON, identifies the message type, and fires `SdpReceived`, `CandidateReceived`, or `ControlReceived` on the signaller; `PeerSession` is connected to those signals and advances the state machine accordingly.

### Data Channels

Data channels are enabled by default (`enableDataChannel = true`). They are created before the SDP offer so the browser's `RTCDataChannel` fires `onopen` when the connection goes active.

```cpp
config.enableDataChannel = true;
config.dataChannelLabel = "control";   // appears as RTCDataChannel.label in the browser
```

#### Sending

```cpp
// UTF-8 text
session.sendData("hello browser");

// Binary
std::vector<std::byte> payload = buildPayload();
session.sendData(payload.data(), payload.size());
```

`sendData()` is a no-op if the data channel is not open. It does not throw. Check `session.state() == Active` if you need to guard against sending before the channel is ready.

#### Receiving

```cpp
session.DataReceived += [](rtc::message_variant msg) {
    if (auto* text = std::get_if<std::string>(&msg)) {
        std::cout << "text: " << *text << '\n';
    }
    else if (auto* bin = std::get_if<rtc::binary>(&msg)) {
        std::cout << "binary: " << bin->size() << " bytes\n";
    }
};
```

If the remote opens a data channel and we haven't created one on our side yet, `PeerSession` adopts it and routes its messages through `DataReceived`.

### Custom Signaller Implementation

To use a different signalling transport (REST, MQTT, carrier pigeon), derive from `SignallingInterface`, implement the three `send*` pure virtual methods, and fire the three signals when messages arrive from the remote side.

```cpp
#include "icy/webrtc/signalling.h"

class MySignaller : public wrtc::SignallingInterface
{
public:
    // PeerSession calls these to send messages to the remote peer.
    void sendSdp(const std::string& peerId,
                 const std::string& type,   // "offer" or "answer"
                 const std::string& sdp) override
    {
        // Serialize and transmit over your transport.
        myTransport.send(buildSdpMessage(peerId, type, sdp));
    }

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override
    {
        myTransport.send(buildCandidateMessage(peerId, candidate, mid));
    }

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override
    {
        myTransport.send(buildControlMessage(peerId, type, reason));
    }

    // Call these when messages arrive from the remote peer.
    // PeerSession is connected to these signals.
    void onMessageFromTransport(const MyMessage& msg)
    {
        if (msg.type == "offer" || msg.type == "answer")
            SdpReceived.emit(msg.peerId, msg.type, msg.sdp);
        else if (msg.type == "candidate")
            CandidateReceived.emit(msg.peerId, msg.candidate, msg.mid);
        else if (msg.type == "init" || msg.type == "accept" ||
                 msg.type == "reject" || msg.type == "hangup")
            ControlReceived.emit(msg.peerId, msg.type, msg.reason);
    }

private:
    MyTransport myTransport;
};
```

Wire it into `PeerSession`:

```cpp
MySignaller signaller;
wrtc::PeerSession session(signaller, config);
```

That is the complete integration point. `PeerSession` drives all state transitions from the signals; your signaller only needs to move bytes between peers.

#### Complete call flow (message sequence)

```text
Caller C++                        Signaller                       Callee (browser)
    |                                 |                                |
    |-- sendControl("init") --------->|-- call:init ------------------>|
    |                                 |                                |  [user clicks accept]
    |<- ControlReceived("accept") ----|<-- call:accept ----------------|
    |                                 |                                |
    |  [createPeerConnection]         |                                |  [createPeerConnection]
    |  [MediaBridge::attach]          |                                |
    |-- sendSdp("offer") ------------>|-- call:offer ---------------->|
    |<- SdpReceived("answer") --------|<-- call:answer ----------------|
    |                                 |                                |
    |-- sendCandidate(c1) ----------->|-- call:candidate(c1) -------->|
    |<- CandidateReceived(c2) --------|<-- call:candidate(c2) ---------|
    |   ... (trickle ICE) ...         |   ... (trickle ICE) ...        |
    |                                 |                                |
    |  [ICE connected -> DTLS]        |                                |
    |  StateChanged(Active)           |                                |  [ontrack fires]
    |  [PacketStream.start()]         |                                |
    |========= SRTP media flowing =====================================>|
    |                                 |                                |
    |-- sendControl("hangup") ------->|-- call:hangup --------------->|
    |  StateChanged(Ended)            |                                |
```

---

## Samples

### [webcam-streamer](../../src/webrtc/samples/webcam-streamer/)

Captures from a camera (or loops a test file), encodes to H.264 with FFmpeg, and streams to any browser peer via WebRTC and Symple signalling. Demonstrates the complete send path: `MediaCapture` source selection, pipeline construction, and graceful teardown on hangup.

The `USE_CAMERA` flag switches between camera capture (using `av::DeviceManager::negotiateVideoCapture()` to find the best resolution the hardware supports) and file loopback. Both paths produce the same downstream pipeline. REMB estimates are logged; wire `BitrateEstimate` to your encoder for adaptive bitrate in production.

**Pipeline**: `av::MediaCapture` → `av::VideoEncoder` → `wrtc::WebRtcTrackSender` → browser

```cpp
#include "icy/application.h"
#include "icy/av/mediacapture.h"
#include "icy/av/videoencoder.h"
#include "icy/packetstream.h"
#include "icy/symple/client.h"
#include "icy/webrtc/peersession.h"
#include "symplesignaller.h"

class WebcamStreamer
{
    smpl::Client client;
    std::unique_ptr<wrtc::SympleSignaller> signaller;
    std::unique_ptr<wrtc::PeerSession> session;
    std::shared_ptr<av::MediaCapture> capture;
    PacketStream stream;

    void createSession()
    {
        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        config.mediaOpts.videoCodec =
            av::VideoCodec("H264", "libx264", 640, 480, 30);
        config.enableDataChannel = false;

        signaller = std::make_unique<wrtc::SympleSignaller>(client);
        session = std::make_unique<wrtc::PeerSession>(*signaller, config);

        session->IncomingCall += [this](const std::string& peerId) {
            session->accept();
        };

        session->StateChanged += [this](wrtc::PeerSession::State state) {
            if (state == wrtc::PeerSession::State::Active)
                startStreaming();
            else if (state == wrtc::PeerSession::State::Ended)
                stream.stop();
        };

        session->media().BitrateEstimate += [](unsigned int bps) {
            std::cout << "REMB: " << bps / 1000 << " kbps\n";
        };
    }

    void startStreaming()
    {
        stream.attachSource(capture.get(), false, true);
        stream.attach(&session->media().videoSender(), 5, false);
        stream.start();
        capture->start();
    }
};
```

### [media-recorder](../../src/webrtc/samples/media-recorder/)

Receives H.264 video from a browser via WebRTC and records it server-side. Demonstrates the real receive path used in Icey today: `WebRtcTrackReceiver` emits encoded frames, a small callback bridge wraps them in FFmpeg `AVPacket`s for `av::VideoDecoder`, and the decoded frames feed `av::MultiplexPacketEncoder` to write MP4 output. Useful for building server-side recording for telehealth, video depositions, or proctoring without cloud vendor lock-in.

The sample configures a video codec in the SDP to signal receive capability to the browser. The `PeerSession` creates an offer/answer that includes a video `m=` section; the browser then sends video to us.

**Pipeline**: browser → `wrtc::WebRtcTrackReceiver` → `av::VideoDecoder` → `av::MultiplexPacketEncoder` → file

The muxer is created lazily on the first decoded frame so width, height, and pixel format are taken from the actual stream rather than guessed up front.

### [file-streamer](../../src/webrtc/samples/file-streamer/)

Reads any FFmpeg-supported media file, loops it at real-time rate, and streams to a browser. Adds a data channel for control commands (seek, pause) from the browser side. Demonstrates mixing media streaming and data channel messaging in the same session.

`capture->setLoopInput(true)` and `setLimitFramerate(true)` together make the file demuxer loop continuously at the file's native frame rate rather than running as fast as FFmpeg allows.

**Pipeline**: `av::MediaCapture` (file, looping) → `wrtc::WebRtcTrackSender` → browser

```cpp
#include "icy/av/mediacapture.h"
#include "icy/packetstream.h"
#include "icy/webrtc/peersession.h"
#include "symplesignaller.h"

// Configure with a data channel for control commands.
config.enableDataChannel = true;
config.dataChannelLabel = "control";

session->DataReceived += [this](rtc::message_variant msg) {
    if (auto* text = std::get_if<std::string>(&msg)) {
        std::cout << "Control: " << *text << '\n';
        // Parse JSON commands: seek, pause, rate, etc.
    }
};

// Capture (file, looping)
capture->openFile(sourceFile);
capture->setLoopInput(true);
capture->setLimitFramerate(true);
```

### [data-echo](../../src/webrtc/samples/data-echo/)

The simplest sample: no media, no FFmpeg, no `PacketStream`. Connects to Symple, waits for incoming calls, and echoes every data channel message back to the sender. Useful as a baseline for testing signalling and data channel connectivity, and as a template for pure data channel applications (remote control, telemetry, chat).

```cpp
#include "icy/webrtc/peersession.h"
#include "symplesignaller.h"

wrtc::PeerSession::Config config;
config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
config.enableDataChannel = true;
config.dataChannelLabel = "echo";
// No media codecs: data channel only.

session->IncomingCall += [this](const std::string& peerId) {
    session->accept();
};

session->DataReceived += [this](rtc::message_variant msg) {
    if (auto* text = std::get_if<std::string>(&msg)) {
        std::cout << "echo: " << *text << '\n';
        session->sendData(*text);
    }
    else if (auto* bin = std::get_if<rtc::binary>(&msg)) {
        session->sendData(
            reinterpret_cast<const std::byte*>(bin->data()), bin->size());
    }
};
```

### [media-server](../../src/webrtc/apps/media-server/)

A complete self-hosted media server in a single binary: Symple signalling server, HTTP static file server, and per-peer WebRTC sessions all in one process. No Node.js, no cloud services; two TCP ports and one binary.

The server registers as a virtual peer in its own Symple network. Browsers discover it via presence, call it, and either receive server-originated media (`stream` mode), send media up to the server for recording (`record` mode), or join the live encoded relay (`relay` mode). Each connecting browser gets its own `MediaSession` and its own media pipeline; stream mode isolates capture + encoder state per peer, record mode isolates decoder + mux state per peer, and relay mode elects one active caller as the source and fans that encoded stream out to the other callers.

**Key design**: `ServerSignaller` implements `SignallingInterface` by routing messages through the embedded Symple server's virtual peer API. This is the reference implementation for server-side custom signalling; it shows exactly what the three `send*` methods need to do and when to fire the three signals.

```cpp
// ServerSignaller routes WebRTC messages through a virtual Symple peer.
class ServerSignaller : public wrtc::SignallingInterface
{
public:
    ServerSignaller(smpl::Server& server, const std::string& serverPeerId);

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override;

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override;

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override;

    // Feed incoming Symple messages here. Fires SdpReceived,
    // CandidateReceived, or ControlReceived as appropriate.
    void onMessage(const json::Value& msg);
};
```

Per-peer session lifecycle: a `call:init` creates a new `MediaSession`; a `PeerDisconnected` event on the Symple server erases it. The `MediaSession` destructor stops the active stream/record pipeline and closes its `PacketStream` cleanly before tearing down the `PeerSession`.

```cpp
// One MediaSession per connected browser.
std::unordered_map<std::string, std::shared_ptr<MediaSession>> _sessions;

// On call:init: create session.
_signaller->ControlReceived += [this](const std::string& peerId,
                                      const std::string& type,
                                      const std::string&) {
    if (type == "init")
        _sessions[peerId] = std::make_shared<MediaSession>(
            peerId, *_signaller, _config, &_relay);
};

// On disconnect: remove session (destructor stops stream).
_symple.PeerDisconnected += [this](smpl::ServerPeer& peer) {
    _relay.unregisterSession(peer.id());
    _sessions.erase(peer.id());
};
```

Configuration is loaded from `config.json` with CLI overrides. The `--mode` flag selects `stream` (file to browsers), `record` (browser video to MP4), or `relay` (first active caller becomes the upstream source and later callers receive that feed). The `--source` flag sets the input file for stream mode. `--record-dir` selects the output directory for record mode. `--web-root` points at the built web UI directory.

```bash
media-server --source video.mp4 --web-root web/dist --port 4500
```

---

## Configuration

### CMake flags

The WebRTC module builds automatically when its dependencies are found. There are no `WITH_*` flags to set. Ensure OpenSSL and FFmpeg are installed system-wide (or pointed to via `FFmpeg_ROOT` / `OPENSSL_ROOT_DIR`), then configure normally:

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
```

libdatachannel is fetched automatically at configure time via CMake FetchContent (v0.24.1). It vendors the following as submodules:

| Library | Role |
| ------- | ---- |
| libjuice | ICE/STUN/TURN client (inside libdatachannel) |
| usrsctp | SCTP for data channels |
| libsrtp2 | SRTP encryption |

OpenSSL is shared with the rest of Icey rather than duplicated.

### FetchContent integration

```cmake
include(FetchContent)
FetchContent_Declare(icey
    GIT_REPOSITORY https://github.com/sourcey/icey.git
    GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(icey)

target_link_libraries(myapp PRIVATE Icey::webrtc)
```

`Icey::webrtc` transitively pulls in `Icey::base`, `Icey::net`, `Icey::crypto`, `Icey::av`, `Icey::symple`, and libdatachannel. You do not need to list them separately.

### Codec flags

The codecs available for negotiation depend entirely on which encoders your FFmpeg build includes. `CodecNegotiator::hasEncoder()` queries FFmpeg at runtime; negotiation silently skips codecs that FFmpeg cannot encode.

| Codec | FFmpeg configure flag |
| ----- | --------------------- |
| H.264 (software) | `--enable-libx264` |
| H.264 (NVENC) | `--enable-nvenc` |
| H.264 (VAAPI) | `--enable-vaapi` |
| VP8 | `--enable-libvpx` |
| VP9 | `--enable-libvpx` |
| AV1 (software) | `--enable-libaom` or `--enable-libsvtav1` |
| AV1 (NVENC) | `--enable-nvenc` |
| H.265 (software) | `--enable-libx265` |
| H.265 (NVENC) | `--enable-nvenc` |
| Opus | `--enable-libopus` |

### ICE and TURN configuration

```cpp
// STUN only (works for ~70% of connections; fails on symmetric NATs)
config.rtcConfig.iceServers.emplace_back("stun:stun.example.com:3478");

// TURN relay (required for ~30% of real-world connections)
config.rtcConfig.iceServers.push_back(
    rtc::IceServer("turn.example.com", 3478, "username", "password",
                   rtc::IceServer::RelayType::TurnUdp));

// TURN over TCP (for networks that block UDP)
config.rtcConfig.iceServers.push_back(
    rtc::IceServer("turn.example.com", 3478, "username", "password",
                   rtc::IceServer::RelayType::TurnTcp));
```

Icey includes a production-grade RFC 5766 TURN server in the `turn` module. See `src/turn/samples/turnserver/` for setup. About 30% of real-world WebRTC connections hit symmetric NATs that require relay; without TURN those connections fail silently after ICE times out.

---

## See Also

- [AV](av.md) — FFmpeg capture, encode, decode, mux; media pipeline integration
- [Symple](symple.md) — real-time messaging, presence, and rooms; `smpl::Client`, `smpl::Server`
- [TURN](turn.md) — self-hosted TURN relay server for symmetric NAT traversal
- [WebRTC in 150 Lines of C++](https://0state.com/writing/webrtc-in-150-lines-of-cpp) — end-to-end walkthrough of the webcam-streamer sample
