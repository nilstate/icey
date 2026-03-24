# TURN Server

Demonstrates a STUN/TURN relay server implementing RFC 5766. The `RelayServer` class implements `ServerObserver` to handle authentication and allocation lifecycle events.

Authentication uses the long-term credential mechanism from RFC 5389: an MD5 hash of `username:realm:password` is used to verify the MESSAGE-INTEGRITY attribute on incoming requests. Credentials are hardcoded for demonstration purposes.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake. Requires the `crypto` module.

## What This Sample Covers

- long-term credential authentication
- allocation creation and expiry
- relay address advertisement
- the combined STUN/TURN server path on the standard TURN port

## Usage

```
./turnserver
```

The server listens on `0.0.0.0:3478` (the standard TURN port). Edit the constants at the top of `turnserver.cpp` to change the bind address, external IP, or credentials.

- `SERVER_EXTERNAL_IP` - must be set to your public IP in production
- `SERVER_USERNAME` / `SERVER_PASSWORD` - credentials clients use to authenticate
- `ENABLE_AUTHENTICATION` - when disabled, all requests are authorized without HMAC verification

## Quick Local Test

The easiest way to prove the relay path is alive is with coturn's test client:

```bash
turnutils_uclient -u username -w password 127.0.0.1
```

Get that working before you pull a full browser stack into the problem.

## Production Notes

- Replace the hard-coded credentials with a real auth backend.
- Set `SERVER_EXTERNAL_IP` to the relay IP clients should actually use.
- Leave authentication enabled outside local bring-up.

Send `SIGINT` (Ctrl+C) to shut down.

## See Also

- [TURN Server recipe](../../../../doc/recipes/turn-server.md)
- [TURN guide](../../../../doc/modules/turn.md)
- [STUN guide](../../../../doc/modules/stun.md)
