# Webcam To Browser

This is one of icey's cleanest end-to-end paths.

The pipeline is:

```text
MediaCapture -> VideoPacketEncoder -> WebRtcTrackSender -> browser
```

Notably, there is no hidden libwebrtc media stack in the middle. icey owns the capture, encode, and stream graph. libdatachannel owns the transport.

## What Handles What

- `smpl::Client` connects to the Symple server
- `SympleSignaller` moves SDP and ICE messages
- `PeerSession` owns the call lifecycle
- `PacketStream` owns the media pipeline
- `VideoPacketEncoder` turns decoded frames into H.264
- `WebRtcTrackSender` pushes encoded packets into the browser track

That is the whole shape.

## The Real Flow

The current `webcam-streamer` sample does this:

:::steps

1. Connect the Symple client and join a room

2. Create a `SympleSignaller` and `PeerSession`

3. Auto-accept incoming calls

4. Build the media pipeline once

5. Start capture and streaming when the session reaches `Active`

6. Stop the pipeline when the call ends

:::

:::warning
Do not start pushing media just because you saw `call:init`. Wait for `PeerSession::State::Active`. That is when the transport is actually ready.
:::

## Minimal Shape

```cpp
wrtc::SympleSignaller signaller(client);
wrtc::PeerSession session(signaller, config);

session.IncomingCall += [&](const std::string&) {
    session.accept();
};

session.StateChanged += [&](wrtc::PeerSession::State state) {
    if (state == wrtc::PeerSession::State::Active)
        startStreaming();
    else if (state == wrtc::PeerSession::State::Ended)
        stopStreaming();
};
```

Then the media side:

```cpp
PacketStream stream;
stream.attachSource(capture.get(), false, true);
stream.attach(encoder, 1, true);
stream.attach(&session.media().videoSender(), 5, false);
```

That is the real send path used by the sample.

## Codec Setup

Do not hand-roll browser codec guesses anymore. Use the codec negotiator:

```cpp
av::VideoCodec videoCodec = wrtc::CodecNegotiator::resolveWebRtcVideoCodec(
    av::VideoCodec("H264", "libx264", width, height, fps));
```

That keeps the WebRTC-facing codec setup aligned with what the rest of the module and the samples actually use.

## Camera vs Test File

The sample can use:

- a real camera
- or the bundled test file

That makes it useful both for real demos and for deterministic local bring-up when you do not want device negotiation in the loop.

The transport and signalling model are the same either way.

## Browser Side

The current intended browser counterpart is the Symple player / call manager flow.

In practice that means:

- the browser sees the streamer peer via Symple presence
- the browser places a call
- `PeerSession` handles the SDP and ICE exchange
- once active, video starts flowing

This is not meant to be a generic signalling demo. It is the concrete "stream H.264 to a browser" path.

## What This Recipe Is Good For

- proving your encode path works
- validating browser playback
- verifying Symple signalling and TURN-assisted connectivity
- building a server-originated video feed with minimal application code

## Good Next Stops

- [WebRTC guide](../modules/webrtc.md) for the full module surface
- [Runtime Contracts](../concepts/runtime-contracts.md) for buffer and loop rules
- [PacketStream](../concepts/packetstream.md) for the pipeline model
- [`webcam-streamer`](https://github.com/nilstate/icey/tree/main/src/webrtc/samples/webcam-streamer/) for the runnable sample
