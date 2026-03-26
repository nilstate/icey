# TURN Server

Use this page when you need a real relay, not a STUN-only best-case path.

TURN is what gets you through the ugly NAT cases that direct peer-to-peer never will. icey's TURN server is built on the same runtime and socket layer as the rest of the library, so you can run relay, signalling, and application code in one process if you want to.

If TURN is new territory, read [STUN](../modules/stun.md) first. TURN messages are STUN messages with allocation and relay semantics layered on top.

## What You Actually Need

A TURN server that works in practice needs four things right:

- bind address
- `realm`
- credentials
- the right public relay address

That last one is where people usually waste time.

If your process binds to a private address but clients are out on the public internet, set `externalIP` correctly or the server will advertise the wrong relay address.

## Minimal Server

```cpp
#include "icy/turn/server/server.h"

using namespace icy;

class MyServer : public turn::ServerObserver {
public:
    turn::Server server;

    MyServer(const turn::ServerOptions& opts)
        : server(*this, opts) {}

    turn::AuthenticationState authenticateRequest(turn::Server*, turn::Request& req) override;
    void onServerAllocationCreated(turn::Server*, turn::IAllocation*) override {}
    void onServerAllocationRemoved(turn::Server*, turn::IAllocation*) override {}
};

turn::ServerOptions opts;
opts.realm = "example.com";
opts.listenAddr = net::Address("0.0.0.0", 3478);
opts.externalIP = "203.0.113.1";
opts.enableUDP = true;
opts.enableTCP = true;

MyServer srv(opts);
srv.server.start();
```

That gives you the actual TURN server. The rest of the work is auth policy and deployment detail.

## Authentication

icey's TURN server uses the standard long-term credential path from RFC 5389.

The usual flow is:

1. client sends unauthenticated `Allocate`
2. server responds `401`
3. client retries with `USERNAME`, `REALM`, `NONCE`, and `MESSAGE-INTEGRITY`
4. server verifies the HMAC and proceeds

Your observer decides what to do:

- `Authorized`
- `NotAuthorized`
- `QuotaReached`
- `Authenticating`

That last state matters if you want to do real async auth without blocking the loop.

## The `externalIP` Rule

This is the thing to get right before anything else.

Use `externalIP` when:

- the server binds to `10.x`, `172.16-31.x`, or `192.168.x`
- the box is behind cloud or host networking indirection
- clients need a public relay address

Do not leave it empty and hope ICE will work it out later. TURN needs to tell the client the truth up front.

## Local Permissions

icey can auto-grant permissions for local and RFC 1918 peers:

```cpp
opts.enableLocalIPPermissions = true;
```

That is useful for development and mixed LAN/public deployments.

It is not a substitute for understanding TURN permissions. It is a convenience for the cases where local probes and local peers are normal.

## Testing It

The included sample is still the fastest way to prove the server path works:

- [`turnserver`](../../src/turn/samples/turnserver/README.md)

You can hit it with coturn's `turnutils_uclient`:

```bash
turnutils_uclient -u username -w password 127.0.0.1
```

That gives you a much faster signal than trying to debug a full browser stack before the relay basics are sound.

## Production Notes

- Replace the hard-coded sample credentials with a real auth backend.
- Set `externalIP` to the public relay IP the client should use.
- Keep UDP and TCP enabled unless you have a strong reason not to.
- Watch allocation counts and lifetimes if you care about quotas and abuse.
- Treat `CreatePermission` and relay traffic as part of the real data plane, not as optional control-plane noise.

## Good Next Stops

- [TURN guide](../modules/turn.md) for the full server and client surface
- [STUN guide](../modules/stun.md) for the shared message model
- [`turnserver`](../../src/turn/samples/turnserver/README.md) for the sample binary
- [Media server stack](media-server-stack.md) if TURN is part of a full self-hosted browser media deployment
