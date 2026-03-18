# TURN Server

Demonstrates a STUN/TURN relay server implementing RFC 5766. The `RelayServer` class implements `ServerObserver` to handle authentication and allocation lifecycle events.

Authentication uses the long-term credential mechanism from RFC 5389: an MD5 hash of `username:realm:password` is used to verify the MESSAGE-INTEGRITY attribute on incoming requests. Credentials are hardcoded for demonstration purposes.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake. Requires the `crypto` module.

## Usage

```
./turnserver
```

The server listens on `0.0.0.0:3478` (the standard TURN port). Edit the constants at the top of `turnserver.cpp` to change the bind address, external IP, or credentials.

- `SERVER_EXTERNAL_IP` - must be set to your public IP in production
- `SERVER_USERNAME` / `SERVER_PASSWORD` - credentials clients use to authenticate
- `ENABLE_AUTHENTICATION` - when disabled, all requests are authorized without HMAC verification

Send `SIGINT` (Ctrl+C) to shut down.
