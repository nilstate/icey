# WebRTC

> Lightweight WebRTC media transport for Icey

[![CI](https://github.com/sourcey/icey/actions/workflows/ci.yml/badge.svg)](https://github.com/sourcey/icey/actions/workflows/ci.yml)

**Repository**: [https://github.com/sourcey/icey](https://github.com/sourcey/icey)
**Dependencies**: Icey (base, net, crypto, av, symple), [libdatachannel](https://github.com/paullouisageneau/libdatachannel), OpenSSL 3.x, FFmpeg 5+
**Licence**: LGPL-2.1+

WebRTC media transport without Google's libwebrtc. Uses libdatachannel for ICE, DTLS-SRTP, and data channels; Icey's AV module for FFmpeg encode/decode; and Symple for signalling. Point the ICE config at your Icey TURN server for fully self-hosted relay.

## Architecture

Three layers, each independently usable:

### Layer 1: Track functions (`track.h`)

Factory functions that create libdatachannel tracks with the correct RTP packetizer/depacketizer chains.

```cpp
#include "icy/webrtc/track.h"

auto pc = std::make_shared<rtc::PeerConnection>(config);

// Create a video track with H.264 packetizer + NACK + PLI + REMB
auto video = wrtc::createVideoTrack(pc,
    av::VideoCodec("H264", "libx264", 1280, 720, 30),
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
    wrtc::setupReceiveTrack(track);  // auto-selects H264/H265/VP8/Opus/PCMU depacketizer
});
```

### Layer 2: Stream adapters (`tracksender.h`, `trackreceiver.h`)

Per-track PacketStream integration. One sender or receiver per track.

```cpp
#include "icy/webrtc/tracksender.h"
#include "icy/webrtc/trackreceiver.h"

// Send: capture -> encode -> WebRTC
wrtc::WebRtcTrackSender videoSender(video);

PacketStream sendStream;
sendStream.attachSource(capture);
sendStream.attach(encoder, 1, true);
sendStream.attach(&videoSender, 5, false);
sendStream.start();

// Receive: WebRTC -> decode -> record
wrtc::WebRtcTrackReceiver videoReceiver;
pc->onTrack([&](std::shared_ptr<rtc::Track> track) {
    wrtc::setupReceiveTrack(track);
    videoReceiver.bind(track);
});

PacketStream recvStream;
recvStream.attachSource(&videoReceiver, false, true);
recvStream.attach(decoder, 1, true);
recvStream.start();
```

`WebRtcTrackSender` converts FFmpeg microsecond timestamps to RTP clock rates, handles send errors gracefully, and only passes packets downstream on successful send (safe to chain a recorder after it).

`WebRtcTrackReceiver` emits owning `VideoPacket`/`AudioPacket` instances that downstream processors can safely queue asynchronously.

### Layer 3: Convenience (`mediabridge.h`, `peersession.h`)

`MediaBridge` creates tracks and adapters for the common case. Video-only, audio-only, or both.

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

`PeerSession` adds Symple call signalling. Speaks the same protocol as `symple-client-player`'s CallManager in the browser.

```cpp
#include "icy/webrtc/peersession.h"

wrtc::PeerSession session(sympleClient, {
    .rtcConfig = { .iceServers = { "turn:your-server.com:3478" } },
    .mediaOpts = {
        .videoCodec = av::VideoCodec("H264", "libx264", 1280, 720, 30),
        .audioCodec = av::AudioCodec("opus", "libopus", 2, 48000)
    }
});

session.IncomingCall += [&](const std::string& peerId) {
    session.accept();  // or session.reject()
};

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
    auto vc = codec->toVideoCodec(1280, 720, 30);  // -> av::VideoCodec
}

// Direct mapping
wrtc::CodecNegotiator::rtpToFfmpeg("H264");   // "libx264"
wrtc::CodecNegotiator::ffmpegToRtp("libopus"); // "opus"
wrtc::CodecNegotiator::clockRate("H264");       // 90000
```

## Building

```bash
cmake -B build \
    -DWITH_OPENSSL=ON \
    -DWITH_FFMPEG=ON \
    -DWITH_LIBDATACHANNEL=ON \
    -DBUILD_TESTS=ON
cmake --build build
```

libdatachannel is fetched automatically via CMake FetchContent (v0.24.1). It brings libjuice (ICE), usrsctp (data channels), and libsrtp (SRTP) as bundled submodules. OpenSSL is shared with Icey.

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
Icey handles: media capture, FFmpeg codecs, signalling, TURN relay, PacketStream pipelines.

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
