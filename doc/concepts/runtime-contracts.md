# Runtime Contracts

Icey does a lot of work on one event loop with very little ceremony. That is a big part of why the library is fast. It also means there are a few rules you need to get right.

This page is about those rules.

It is not a tutorial and it is not the API reference. It is the compact version of "what assumptions the runtime is built on."

## The Short Version

- Most `base`, `net`, and `http` objects are loop-affine.
- `Signal<T>` is the fast local path. `ThreadSignal<T>` is the cross-thread path.
- Receive buffers are borrowed.
- `send()` is the borrowed fast path. `sendOwned()` is the retained path.
- `PacketStream` is zero-copy until it crosses an explicit retention boundary.
- `close()` is asynchronous. Treat shutdown as a state change, not as immediate destruction.

If you keep those six rules in your head, most of Icey makes sense.

## Loop Affinity

Most runtime objects belong to one libuv loop thread.

That includes the types that actually do work:

- handles and streams in `base`
- sockets in `net`
- HTTP connections, adapters, and server connections in `http`

The contract is simple:

- construct them on the loop you want to own them
- start, stop, and close them on that same loop
- if another thread needs to interact with them, hop through a queue, a `Synchronizer`, or some other explicit cross-thread handoff

This is deliberate. Icey does not try to make every object transparently thread-safe. That would add locking and hide the real execution model.

If a type is meant to be shared across threads, the docs should say so explicitly. Otherwise assume loop affinity.

## Signals Are Not All The Same

Icey now has a clear split here.

- `Signal<T>` is the default fast path. Use it when emission and subscription live on one thread, which is the normal case inside one libuv loop.
- `ThreadSignal<T>` is for real cross-thread emission or subscription.
- `LocalSignal<T>` is an alias for the same local fast-path semantics and is used where loop-local intent is part of the contract.

That matters for both speed and design. If you reach for `ThreadSignal<T>` everywhere "just to be safe", you are paying for a wider contract than the code actually needs.

It also matters for reasoning about code. A loop-local signal says something useful about the object that owns it.

## Borrowed Buffers Mean Borrowed

This is the most common mistake people make in async code.

When you receive a `MutableBuffer` in `net` or `http`, that buffer is borrowed for the duration of the callback. Nothing more.

That means:

- do not stash the pointer
- do not hand it to another thread
- do not assume it stays valid after the callback returns

If you need the bytes later:

- copy them
- or move them through a queue or retained buffer boundary immediately

That same rule applies higher in the stack when payload callbacks are just exposing transport buffers from below.

## `send()` vs `sendOwned()`

Icey uses both on purpose.

`send()` is the hot path:

- borrowed payload
- minimal overhead
- caller keeps the data alive until the async write is done

`sendOwned()` is the explicit retained path:

- use it when the payload lives in temporary storage
- use it when you just assembled headers, frames, or encrypted output in a local buffer
- use it when the bytes will not outlive the current scope

The rule is not subtle:

- if the data already lives long enough, use `send()`
- if it does not, use `sendOwned()`

This is how Icey stays fast without pretending buffer lifetime is magic.

## `PacketStream` Stays Zero-Copy Until You Cross A Boundary

`PacketStream` is the data plane for most of the interesting parts of Icey. It is also where ownership gets people into trouble if they stop paying attention.

The default rule is:

- packets are borrowed
- processing is synchronous
- nothing gets retained unless an adapter says it does

The first adapter reporting `PacketRetention::Cloned` or `PacketRetention::Retained` is the ownership boundary in that graph.

Common explicit boundaries are:

- `SyncPacketQueue`
- `AsyncPacketQueue`
- `synchronizeOutput()`
- any processor or adapter that clones or retains packets before deferred work

Upstream code may only reuse or free borrowed storage after one of those boundaries, or after the whole synchronous call chain has returned.

If you are wiring media pipelines, this rule matters as much as the codec settings do.

## Closing Is Asynchronous

`close()` in Icey usually means:

- stop accepting new work
- schedule shutdown on the owning loop
- finish whatever libuv or transport cleanup still has to drain
- then emit the close callback or signal

It does not usually mean "the object is gone right now."

That affects how you write shutdown logic:

- keep the owning `shared_ptr` alive until the close path is done
- do not assume a destructor is the shutdown API
- do not call into half-torn-down objects from stale callbacks

The base runtime now does a much better job of making that safe, but the model is still asynchronous by design.

## A Few Rules That Save Real Debugging Time

### Do not mutate live `PacketStream` graphs

Build the graph, then start it. Tear it down after it stops. Changing topology mid-flight is one of the fastest ways to make a clean pipeline confusing.

### Do not widen contracts "just in case"

Use local signals when they are local. Use borrowed send when the payload naturally lives long enough. Use queues only when you actually need a thread hop or retention boundary.

### Do not treat HTTP client `submit()` and connection `send()` as synonyms

They are different on purpose.

- `submit()` starts the HTTP transaction
- `send()` writes bytes on an already-established transaction or upgraded connection

If you mix those up, you are fighting the model instead of using it.

## Where To Go Next

- [Base](../modules/base.md) for the runtime substrate
- [PacketStream](packetstream.md) for the pipeline and ownership model
- [HTTP Lifecycle](http-lifecycle.md) for connection, keep-alive, streaming, and upgrade flow
- [Net](../modules/net.md) for socket-level send and receive rules

