# WebRTC

> Lightweight WebRTC media transport for icey

[![CI](https://github.com/nilstate/icey/actions/workflows/ci.yml/badge.svg)](https://github.com/nilstate/icey/actions/workflows/ci.yml)

**Repository**: [https://github.com/nilstate/icey](https://github.com/nilstate/icey)
**Dependencies**: icey (base, net, crypto, av, symple), [libdatachannel](https://github.com/paullouisageneau/libdatachannel), OpenSSL 3.x, FFmpeg 5+
**Licence**: LGPL-2.1+

WebRTC media transport without Google's libwebrtc. Uses libdatachannel for ICE, DTLS-SRTP, and data channels; icey's AV module for FFmpeg encode/decode; and Symple for signalling. Point the ICE config at your icey TURN server for fully self-hosted relay.

Current browser validation for the packaged `icey-cli` media server path:
- Chromium: validated by committed Playwright smoke
- Firefox: validated by committed Playwright smoke
- WebKit: smoke target exists, but Linux Playwright WebKit/WPE is not treated as authoritative for publish-path support
- Safari/macOS: not claimed until validated on Apple platforms

## Architecture

Three layers, each independently usable:

### Layer 1: Track functions (`track.h`)

Factory functions that create libdatachannel tracks with the correct RTP packetizer/depacketizer chains.

```cpp
#include "icy/webrtc/track.h"

auto pc = std::make_shared<rtc::PeerConnection>(config);

// Create a video track with H.264 packetizer + NACK + PLI + REMB
av::VideoCodec codec("H264", "libx264", 1280, 720, 30);
codec.options["preset"] = "ultrafast";
codec.options["tune"] = "zerolatency";
codec.options["profile"] = "baseline";  // required for browser compatibility
auto video = wrtc::createVideoTrack(pc, codec,
    0,        // SSRC (0 = auto)
    {},       // CNAME
    512,      // NACK buffer
    [&]() { encoder.forceKeyframe(); },        // PLI callback
    [&](unsigned bps) { encoder.setBitrate(bps); }  // REMB callback
);

// Create an audio track with Opus packetizer
auto audio = wrtc::createAudioTrack(pc,
    av::AudioCodec("opus", "libopus", 2, 48000));

// Set up receive-side depacketizer on a remote track
pc->onTrack([](std::shared_ptr<rtc::Track> track) {
    if (!wrtc::setupReceiveTrack(track))
        return;
    // bind a WebRtcTrackReceiver here
});
```

### Layer 2: Stream adapters (`tracksender.h`, `trackreceiver.h`)

Per-track PacketStream integration. One sender or receiver per track.

```cpp
#include "icy/webrtc/tracksender.h"
#include "icy/webrtc/trackreceiver.h"
#include "icy/av/videopacketencoder.h"

// Send: capture -> encode -> WebRTC
auto encoder = std::make_shared<av::VideoPacketEncoder>();
capture->getEncoderVideoCodec(encoder->iparams);
encoder->oparams = av::VideoCodec("H264", "libx264", 1280, 720, 30);
encoder->oparams.options["preset"] = "ultrafast";
encoder->oparams.options["tune"] = "zerolatency";
encoder->oparams.options["profile"] = "baseline";

wrtc::WebRtcTrackSender videoSender(video);

PacketStream sendStream;
sendStream.attachSource(capture);
sendStream.attach(encoder, 1, true);
sendStream.attach(&videoSender, 5, false);
sendStream.start();

// Receive: WebRTC -> your decode / record callback
wrtc::WebRtcTrackReceiver videoReceiver;
pc->onTrack([&](std::shared_ptr<rtc::Track> track) {
    if (wrtc::setupReceiveTrack(track))
        videoReceiver.bind(track);
});

videoReceiver.emitter += packetSlot(&recorder, &Recorder::onEncodedVideo);
```

`WebRtcTrackSender` converts FFmpeg microsecond timestamps to RTP clock rates, handles send errors gracefully, and only passes packets downstream on successful send (safe to chain a recorder after it).

`WebRtcTrackReceiver` emits owning encoded `VideoPacket`/`AudioPacket` instances that downstream processors can safely queue asynchronously. For a complete receive -> decode -> MP4 example, see `samples/media-recorder`.

### Layer 3: Convenience (`mediabridge.h`, `peersession.h`)

`MediaBridge` creates tracks and adapters for the common case. Video-only, audio-only, or both. The sender and receiver adapter objects stay stable across attach/detach, so a `PacketStream` can keep a `videoSender()` or `audioSender()` attachment alive across repeated calls while the underlying WebRTC tracks are rebound internally.

```cpp
#include "icy/webrtc/mediabridge.h"

wrtc::MediaBridge bridge;
bridge.attach(pc, {
    .videoCodec = av::VideoCodec("H264", "libx264", 1280, 720, 30),
    .audioCodec = av::AudioCodec("opus", "libopus", 2, 48000)
});

// Per-track senders/receivers for PacketStream
bridge.videoSender();    // WebRtcTrackSender&
bridge.audioSender();    // WebRtcTrackSender&
bridge.videoReceiver();  // WebRtcTrackReceiver&
bridge.audioReceiver();  // WebRtcTrackReceiver&

// RTCP feedback
bridge.KeyframeRequested += slot(&encoder, &Encoder::forceKeyframe);
bridge.BitrateEstimate += slot(&encoder, &Encoder::setBitrate);
```

`PeerSession` adds Symple call signalling. Speaks the same protocol as `symple-player`'s CallManager in the browser.

```cpp
#include "icy/symple/client.h"
#include "icy/webrtc/peersession.h"
#include "icy/webrtc/support/symplesignaller.h"

smpl::Client::Options opts;
opts.host = "localhost";
opts.port = 4500;
opts.user = "streamer";

smpl::Client sympleClient(opts);
wrtc::SympleSignaller signaller(sympleClient);

wrtc::PeerSession session(signaller, {
    .rtcConfig = { .iceServers = { "turn:your-server.com:3478" } },
    .media = {
        .videoCodec = av::VideoCodec("H264", "libx264", 1280, 720, 30),
        .audioCodec = av::AudioCodec("opus", "libopus", 2, 48000)
    }
});

session.IncomingCall += [&](const std::string& peerId) {
    session.accept();  // or session.reject()
};

sympleClient.start();
session.call("remote-peer-id");
```

For custom signalling backends (plain WebSocket, REST, MQTT), use layers 1 and 2 directly.

## Codec negotiation (`codecnegotiator.h`)

Maps between RTP codec names and FFmpeg encoder names, queries FFmpeg at runtime.

```cpp
#include "icy/webrtc/codecnegotiator.h"

// Negotiate best codec from remote SDP offer
auto codec = wrtc::CodecNegotiator::negotiateVideo({"H264", "VP8", "VP9"});
if (codec) {
    // General purpose (file encoding, etc.)
    auto vc = codec->toVideoCodec(1280, 720, 30);

    // WebRTC browser playback (low-latency, browser-safe profile)
    auto wrtcVc = codec->toWebRtcVideoCodec(1280, 720, 30);
}

// Direct mapping
wrtc::CodecNegotiator::rtpToFfmpeg("H264");   // "libx264"
wrtc::CodecNegotiator::ffmpegToRtp("libopus"); // "opus"
wrtc::CodecNegotiator::clockRate("H264");       // 90000
```

## Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build
```

`icey::webrtc` is built when its prerequisites are available: OpenSSL and FFmpeg are discovered from the system (or via `OPENSSL_ROOT_DIR` / `FFmpeg_ROOT`), while libdatachannel is fetched automatically via CMake FetchContent. libdatachannel brings libjuice (ICE), usrsctp (data channels), and libsrtp (SRTP) as bundled submodules.

## How it fits together

```
Application
+-- Symple signalling (SDP + candidate exchange)
+-- PeerSession (call lifecycle, optional)
|   +-- MediaBridge (track management, optional)
|   |   +-- WebRtcTrackSender (PacketProcessor)
|   |   +-- WebRtcTrackReceiver (PacketStreamAdapter)
|   |   +-- createVideoTrack / createAudioTrack
|   |   +-- setupReceiveTrack
|   +-- Data channels
+-- AV module (FFmpeg encode/decode)
+-- TURN server (self-hosted relay)
    +-- icey stun/turn modules (server side)
    +-- libjuice STUN/TURN client (inside libdatachannel)
```

libdatachannel handles: ICE, DTLS, SRTP, SCTP, RTP packetization, RTCP feedback.
icey handles: media capture, FFmpeg codecs, signalling, TURN relay, PacketStream pipelines.

## Files

```
include/icy/webrtc/
  webrtc.h            Module header, DLL exports
  codecnegotiator.h   RTP <-> FFmpeg codec mapping
  track.h             Layer 1: track factory functions
  tracksender.h       Layer 2: per-track PacketProcessor
  trackreceiver.h     Layer 2: per-track PacketStreamAdapter
  mediabridge.h       Layer 3: multi-track convenience wrapper
  peersession.h       Layer 3: Symple call protocol
```
