# Data Echo

WebRTC data channel echo server. Connects to a Symple server, accepts incoming calls, and echoes back any data channel messages. No media tracks, no FFmpeg.

Use this to verify the full WebRTC signalling and transport stack: Symple call protocol, libdatachannel ICE/DTLS/SCTP, and PeerSession lifecycle.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake and the `webrtc` module prerequisites are available.

## Requirements

- Running Symple server (`symple-server` from the symple repo)

## Usage

```
# Start symple server
cd symple/symple-server && npm start

# Start echo server
./data-echo -host 127.0.0.1 -port 4500 -user echo-server -name "Data Echo"
```

Connect from a browser using `symple-client` + `RTCPeerConnection`. Send messages on the data channel; they'll be echoed back.
