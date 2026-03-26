# Base Module

Core runtime primitives for icey: event loops, handles, signals, PacketStream, timers, logging, buffers, filesystem helpers, and process/thread utilities.

- Namespaces: `icy`, `icy::uv`, `icy::fs`
- CMake target: `icey::base`
- Primary headers: `include/icy/loop.h`, `signal.h`, `packetstream.h`, `handle.h`, `stream.h`
- Directory layout: `include/` for the public runtime API, `src/` for implementation, `tests/` for lifetime, queue, and contract coverage
- Everything else in icey builds on this module

Read next:
- [Base module guide](../../docs/modules/base.md)
- [Runtime contracts](../../docs/concepts/runtime-contracts.md)
- [PacketStream concept guide](../../docs/concepts/packetstream.md)
