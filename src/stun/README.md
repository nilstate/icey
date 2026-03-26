# STUN Module

RFC 5389 STUN message and transaction support for NAT traversal.

- Namespace: `icy::stun`
- CMake target: `icey::stun`
- Primary headers: `include/icy/stun/stun.h`, `message.h`, `attributes.h`, `transaction.h`
- Directory layout: `include/` for the public API, `src/` for the codec and transaction layer, `tests/` for RFC framing and attribute coverage
- TURN builds directly on this module

Read next:
- [STUN module guide](../../doc/modules/stun.md)
- [TURN module guide](../../doc/modules/turn.md)
