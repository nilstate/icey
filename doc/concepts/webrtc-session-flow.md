# WebRTC Session Flow

If you strip away the browser, the codecs, and the demo UI, the hard part of WebRTC integration is still the same:

- who owns call state
- who moves signalling messages
- when media should actually start

In Icey, that split is explicit.

## The Split

There are three different jobs here:

- `SignallingInterface` moves SDP, ICE candidates, and call-control messages
- `PeerSession` owns the call lifecycle
- `MediaBridge` owns the actual audio/video tracks and the sender/receiver adapters

That separation is deliberate.

The signaller is transport.

The session is state.

The media bridge is media.

If you keep those roles separate in your head, the module is much easier to use and much easier to debug.

## The Session States

`PeerSession` now has explicit phases:

| State | Meaning |
| --- | --- |
| `Idle` | no active call |
| `OutgoingInit` | local side sent `call:init`; waiting for remote response |
| `IncomingInit` | remote side initiated the call; waiting for local `accept()` or `reject()` |
| `Negotiating` | peer connection exists and SDP / ICE are being exchanged |
| `Active` | transport is up and media can flow |
| `Ending` | teardown is in progress |
| `Ended` | terminal state before returning to `Idle` |

That matters because the old vague model of "there is probably a call happening now" is where signalling races and media attach bugs come from.

## The Outgoing Path

The normal outgoing flow is:

```text
call(peer)
  -> OutgoingInit
  -> remote accept
  -> create PeerConnection
  -> attach tracks
  -> create and send SDP offer
  -> exchange ICE candidates
  -> Active
```

`PeerSession` owns that flow. Your application code should not be manually recreating the same state machine on the side.

## The Incoming Path

The incoming flow is the mirror image:

```text
remote call:init
  -> IncomingInit
  -> application decides accept or reject
  -> accept()
  -> create PeerConnection
  -> attach tracks
  -> wait for remote offer
  -> answer
  -> exchange ICE candidates
  -> Active
```

The important detail is that `accept()` is not just a courtesy message. It is the transition that causes the session to stand up the real connection state.

## What The Signaller Actually Does

`SignallingInterface` transports:

- `call:init`
- `accept`
- `reject`
- `hangup`
- SDP offer / answer
- ICE candidates

That is all.

It does **not** own the session state machine.

This matters because people often over-credit their signalling layer. If your application is treating the signaller as the thing that "has the call", you are going to end up duplicating `PeerSession` badly.

`SympleSignaller` is just one transport implementation of that interface.

## When To Attach Media

This is the rule people get wrong most often:

Attach or start the live media pipeline on `Active`, not on `call:init`, not on `accept()`, and not on "the peer connection probably exists now."

The right shape is:

```cpp
session.StateChanged += [&](wrtc::PeerSession::State state) {
    if (state == wrtc::PeerSession::State::Active)
        startStreamingOrRecording();
    else if (state == wrtc::PeerSession::State::Ended)
        stopStreamingOrRecording();
};
```

That rule exists because the session state is the first point where the transport is actually ready, not just half-negotiated.

## `MediaBridge` Is The Common Case

`MediaBridge` exists because most applications want the same shape:

- up to one video sender
- up to one audio sender
- up to one video receiver
- up to one audio receiver

It is the thing that gives `PeerSession` a sane media surface without forcing you to manually wire every track by hand.

Use lower-level track APIs when you need them. Do not bypass `MediaBridge` just because you saw the types in the header.

## Data Channel

The data channel is part of the session config, not a separate ad hoc transport bolted on later.

That means:

- enable it explicitly in the config
- treat `DataReceived` as session-level state
- do not assume every call has one

Also: the data channel being open does not mean media is active, and media being active does not mean you chose to enable a data channel. Keep those concerns separate.

## Candidate Ordering

The current code is stricter and safer here than it used to be.

Remote ICE candidates can arrive before remote SDP is installed. `PeerSession` queues them until the SDP side is ready, instead of assuming the network and signalling always arrive in the pretty order you hoped for.

That matters more in real deployments than in ideal local tests.

## The Two Clean Patterns

### Send path

```text
accept or call
  -> Active
  -> start capture / encode / sender pipeline
  -> Ended
  -> stop pipeline
```

### Receive path

```text
accept or call
  -> Active
  -> attach receiver / decode / recorder pipeline
  -> Ended
  -> stop pipeline and finalize output
```

That is the shape the current samples and `media-server` are built around.

## Common Mistakes

### Starting media too early

If you start the pipeline before `Active`, you are usually racing the transport.

### Treating the signaller as the session owner

It is not.

### Rebuilding negotiation logic in application code

If you are manually juggling `call:init`, `accept`, SDP, and ICE on the side, you are probably working around `PeerSession` instead of using it.

### Forgetting teardown symmetry

If you start pipelines on `Active`, stop them on `Ended`. Make the cleanup shape match the startup shape.

## Why This Page Exists

The WebRTC module has a lot of moving parts, but the session model is not supposed to feel mysterious.

The whole point of `PeerSession` is to keep:

- signalling transport
- session state
- media attachment

from bleeding into each other.

That is what lets the module stay usable without turning into a pile of browser-era incidental complexity.

## Where To Go Next

- [WebRTC](../modules/webrtc.md) for the full module guide
- [Webcam To Browser](../recipes/webrtc-webcam-to-browser.md) for the clean send path
- [Browser To Recorder](../recipes/webrtc-browser-to-recorder.md) for the clean receive path
- [Media Server Stack](../recipes/media-server-stack.md) for the full deployed shape

