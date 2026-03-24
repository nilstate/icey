# Browser To Recorder

This is the reverse of the "stream to browser" path.

The pipeline is:

```text
browser -> WebRtcTrackReceiver -> VideoDecoder -> MultiplexPacketEncoder -> file
```

It matters because it proves the receive side is real: not just signalling, not just an open peer connection, but actual browser-originated media getting decoded and written to disk.

## What The Recorder Actually Does

The current `media-recorder` sample:

1. connects to Symple
2. creates a receive-capable `PeerSession`
3. auto-accepts incoming calls
4. waits for `Active`
5. attaches to `videoReceiver()`
6. decodes H.264
7. lazily opens the MP4 mux once the first decodable frame arrives
8. writes the file until the call ends

That lazy mux open is important. It means the output file takes its real dimensions and pixel format from the stream that actually arrived, instead of from hard-coded guesses.

It also means the sample can survive a browser joining mid-GOP. If the first packets are not decodable yet, it waits for the first usable keyframe instead of creating garbage output.

## Minimal Shape

The session side:

```cpp
session.IncomingCall += [&](const std::string&) {
    session.accept();
};

session.StateChanged += [&](wrtc::PeerSession::State state) {
    if (state == wrtc::PeerSession::State::Active)
        startRecording();
    else if (state == wrtc::PeerSession::State::Ended)
        stopRecording();
};
```

The receive path is driven from the receiver emitter:

```cpp
session->media().videoReceiver().emitter +=
    packetSlot(this, &MediaRecorder::onEncodedVideo);
```

Then:

- `VideoDecoder` turns encoded H.264 into decoded frames
- `MultiplexPacketEncoder` writes MP4 output

## Why The Sample Still Configures Video Codec

Even though the recorder is receive-only, the session config still declares the codec it is prepared to receive cleanly. The WebRTC layer is now strict about codec setup instead of inventing defaults behind your back.

That is a good thing. It makes negotiation and failure modes honest.

## What To Watch For

- Do not assume the first browser packet is decodable.
- Do not open the mux before you know the real decoded frame shape.
- Stop cleanly on `Ended` so the file is finalized properly.

Those three rules are why the sample is useful instead of just being a demo that "usually works."

## Browser Side

Any browser client that can:

- call the recorder peer over Symple
- offer H.264 video
- complete the normal SDP and ICE flow

can drive this path.

The intended local pairing is with the Symple player stack, but the important part is the protocol shape, not the specific UI.

## Good Next Stops

- [WebRTC guide](../modules/webrtc.md) for the wider send/receive model
- [PacketStream](../concepts/packetstream.md) for how the decode and mux stages fit together
- [`media-recorder`](../../src/webrtc/samples/media-recorder/README.md) for the runnable sample
- [Media server stack](media-server-stack.md) if you want this receive-and-record flow as part of a single deployed service

