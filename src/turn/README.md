# TURN Module

RFC 5766 TURN relay server and client support, including RFC 6062 TCP relays.

- Namespace: `icy::turn`
- CMake target: `icey::turn`
- Primary headers: `include/icy/turn/server/server.h`, `client/client.h`, `client/tcpclient.h`
- Directory layout: `include/` for the public API, `src/` for client/server implementations, `samples/` for runnable server code, `tests/` for allocation and permission coverage
- This module builds on `stun` and `net`

Read next:
- [TURN module guide](../../doc/modules/turn.md)
- [TURN server recipe](../../doc/recipes/turn-server.md)
- [STUN module guide](../../doc/modules/stun.md)
