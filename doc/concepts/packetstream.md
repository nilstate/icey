# PacketStream

`PacketStream` is the data plane running through a lot of icey.

It is how media gets from capture to encoder to network transport. It is how receiver pipelines hand packets to decoders and muxers. It is how the library keeps dataflow explicit instead of hiding it behind callback soup.

If you understand `PacketStream`, a lot of the rest of icey stops looking like magic.

## What It Is

A `PacketStream` has three moving parts:

- one or more sources
- zero or more ordered processors
- one emitter feeding sinks

The shape is simple:

```text
source -> processor -> processor -> emitter -> sinks
```

Or, with multiple producers and a queue boundary:

```text
source A --\
           -> PacketStream -> AsyncPacketQueue -> encoder -> sink
source B --/
```

That is not a metaphor. That is the actual execution model.

## The Core Types

### `IPacket`

Everything moving through the graph is an `IPacket`.

The built-in packet types in `base` are:

- `RawPacket` for raw bytes
- `FlagPacket` for control markers with no payload

If you need something richer, define your own packet type and implement `clone()`.

That `clone()` is not decorative. It is how retained or queued paths keep packet ownership honest.

### `PacketStreamAdapter`

Sources and processors both build on `PacketStreamAdapter`.

The important part is that adapters now declare their retention behavior:

- `Borrowed`
- `Cloned`
- `Retained`

That means the graph has an explicit ownership story instead of depending on comments and luck.

### `PacketProcessor`

A processor is an adapter with `process(IPacket&)`.

Processors run in ascending order. Lower order runs first.

That makes chain assembly straightforward:

- decode first
- then transform
- then encode
- then packetize
- then send

## Ownership Is The Whole Game

The fastest `PacketStream` graph is also the simplest one:

- borrowed packets
- synchronous processors
- no queue boundaries

In that case the caller keeps storage alive until the whole write or emit call returns.

That stops being true the moment you cross a retention boundary.

## The Retention Boundary Rule

This is the rule that matters:

Upstream code may only reuse or free borrowed packet storage after:

- the synchronous graph call has returned
- or the packet has crossed the first adapter reporting `Cloned` or `Retained`

That is the boundary.

In practical terms:

- `rawPacket(buf, len)` is zero-copy if `buf` is mutable
- `rawPacket(const char*, len)` makes an owned copy
- `SyncPacketQueue` clones before deferred dispatch
- `AsyncPacketQueue` clones before hopping to worker-thread processing
- `synchronizeOutput()` inserts a loop-thread queue boundary

If you are not sure whether some downstream stage is async, make the boundary explicit. Do not rely on "it probably finishes before we reuse this buffer."

## Source, Processor, Sink

### Sources

A source owns an emitter and pushes packets into the graph.

Typical examples:

- camera capture
- file capture
- RTP or WebRTC receive side
- a network socket or protocol decoder

If a source supports start and stop, the stream can synchronize its lifecycle with `syncState=true`.

That matters for capture devices and long-lived media readers. It lets the stream own more than just packet flow; it also owns when production begins and ends.

### Processors

A processor transforms or filters packets.

Typical examples:

- encoders
- decoders
- packetizers
- depacketizers
- resamplers
- muxers

Processors should be boring:

- reject unsupported packet types early with `accepts()`
- do their work
- emit the next packet

If they defer work, they need to say so through retention semantics. Hidden async is how clean graphs turn into memory bugs.

### Sinks

Sinks are just slots attached to the stream emitter.

That is one of the reasons `PacketStream` scales across the library. A sink can be:

- a WebRTC track sender
- a recorder
- another `PacketStream`
- a testing probe
- a simple lambda

## Graph Rules

There are two rules worth being blunt about.

### Build the graph before you run it

`PacketStream` lets you compose flexible graphs. It does not mean topology should be casual.

The good pattern is:

1. attach sources
2. attach processors
3. attach sinks
4. add queue boundaries if needed
5. start

Then stop and tear down in the reverse direction.

Do not treat topology mutation during active flow as normal control flow.

### Order is part of the contract

Processor order is not just a convenience. It is how you say what the pipeline means.

If a graph matters, the order values should read like intent:

- `0` for queue or thread hop
- `5` for decode
- `10` for transform
- `20` for encode
- `30` for packetize

You do not need that exact numbering scheme, but you do need deliberate ordering.

## Thread Hops

The default `PacketStream` execution model is synchronous in the caller's thread.

That is good for performance and easy to reason about.

When you need a thread hop, do it explicitly.

### `synchronizeOutput(loop)`

Use this when:

- sources run off-loop
- sinks must run on the libuv loop
- you want one clear handoff point before network or UI work

Internally this inserts a `SyncPacketQueue` near the end of the graph.

### `AsyncPacketQueue`

Use this when:

- the source thread should stay light
- decode, encode, or transformation work is expensive
- downstream stages can safely happen off-source-thread

This is the clean way to move work, not an excuse to make the whole graph vaguely asynchronous.

## Real Patterns

### Webcam to browser

```text
MediaCapture -> VideoPacketEncoder -> WebRtcTrackSender
```

### Browser to recorder

```text
WebRtcTrackReceiver -> VideoDecoder -> MultiplexPacketEncoder
```

### Relay

```text
source session -> encoded packet fanout -> viewer senders
```

The same graph model covers all three. That is one of the reasons icey can keep media code relatively coherent.

## Common Mistakes

### Borrowing across an async boundary

This is the classic one.

If a packet started as borrowed bytes and then crosses a queue or deferred processor, you need an explicit clone or retained representation before that handoff.

### Hiding packet type assumptions

If a processor only works on one packet type, say so in `accepts()`. Do not make the graph discover that by tripping over a `dynamic_cast` later.

### Mutating topology under load

If you need to restructure the graph, stop it first.

### Treating `PacketStream` as a generic event bus

It is a data plane. Use it when packets are actually flowing through a pipeline. Not every callback chain in the codebase needs to become a stream.

## Why This Matters

icey uses `PacketStream` in the places where performance and clarity usually fight each other.

The whole point of the abstraction is that you do not have to choose:

- it stays zero-copy until you say otherwise
- it keeps thread hops explicit
- it keeps packet ownership visible
- it makes the media and transport pipelines composable

That is what gives the library a coherent core instead of five separate async models pretending to be one.

## Where To Go Next

- [Base](../modules/base.md) for the underlying packet and signal APIs
- [Runtime Contracts](runtime-contracts.md) for the loop, signal, and ownership rules around the pipeline
- [WebRTC](../modules/webrtc.md) for the media send and receive layers built on top of it
- [AV](../modules/av.md) for capture, decode, encode, and mux components

