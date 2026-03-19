# Media Server

Single C++ binary: WebRTC media streaming + Symple signalling + TURN relay + web UI.

No Node.js runtime, no third-party services. One binary, two ports (HTTP/WS + TURN).

## Quick start

```bash
# Build the C++ server
cmake -B build \
    -DWITH_OPENSSL=ON \
    -DWITH_FFMPEG=ON \
    -DWITH_LIBDATACHANNEL=ON \
    -DBUILD_APPLICATIONS=ON
cmake --build build --target media-server

# Build the web UI
cd src/webrtc/apps/media-server/web
npm install
npm run build

# Run
./build/bin/media-server --web-root src/webrtc/apps/media-server/web/dist --source /path/to/video.mp4
```

Open `http://localhost:4500` in a browser.

## Modes

- **stream** (default): server pushes file/camera to browser via WebRTC
- **record**: browser sends camera to server, server writes to disk
- **relay**: SFU; server forwards tracks between peers

## CLI options

```
media-server [options]
  -c, --config <path>     Config file (default: ./config.json)
  --port <port>           HTTP/WS port (default: 4500)
  --turn-port <port>      TURN port (default: 3478)
  --mode <mode>           stream|record|relay
  --source <path>         Media source file or device
  --web-root <path>       Path to web UI dist/ directory
  --no-turn               Disable embedded TURN server
  --no-tls                Disable TLS
```

## Web UI development

```bash
cd web
npm run dev
```

Vite dev server runs on port 5173 and proxies `/ws` and `/api` to the C++ server on port 4500. Run both in parallel for hot-reload development.

## Architecture

```
Browser ─── WSS /ws ──── Symple v4 (signalling, presence, rooms)
        ─── GET /   ──── Static files (Vite build output)
        ─── GET /api ─── REST status
        ─── WebRTC  ──── Media (H.264/VP8 + Opus via libdatachannel)
        ─── TURN    ──── NAT traversal (embedded, port 3478)
```

## Dependencies

All existing icey modules:

```
media-server
├── webrtc (libdatachannel, track, mediabridge)
├── symple (server, protocol)
├── av (MediaCapture, VideoEncoder, MultiplexEncoder)
├── http (Server, WebSocket)
├── turn (Server, allocations)
├── net, crypto, json, base
```

Frontend: `symple-client` + `symple-client-player` (npm, bundled with Vite).
