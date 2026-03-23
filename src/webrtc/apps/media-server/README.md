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

- **stream** (default): server pushes file/camera to browser via WebRTC (H.264 + Opus)
- **record**: browser sends H.264 video to the server, which decodes it and writes MP4 files to disk
- **relay**: not implemented; incoming calls are rejected with an explicit reason

## Features

- **Video + Audio**: H.264 Constrained Baseline (browser-safe) + Opus at 48kHz
- **Embedded TURN**: RFC 5766 relay on port 3478 — works through symmetric NATs
- **Adaptive bitrate**: REMB feedback adjusts encoder bitrate in real-time
- **Per-session isolation**: each peer gets its own capture/encoder pipeline in stream mode and its own recorder pipeline in record mode
- **Server-side recording**: record mode writes timestamped MP4 files under `--record-dir`
- **Zero latency**: `ultrafast` preset + `zerolatency` tune for real-time H.264

## CLI options

```text
media-server [options]
  -c, --config <path>     Config file (default: ./config.json)
  --port <port>           HTTP/WS port (default: 4500)
  --turn-port <port>      TURN port (default: 3478)
  --mode <mode>           stream|record|relay
  --source <path>         Media source file or device
  --record-dir <path>     Output directory for record mode (default: ./recordings)
  --web-root <path>       Path to web UI dist/ directory
  --no-turn               Disable embedded TURN server
```

## Web UI development

```bash
cd web
npm run dev
```

Vite dev server runs on port 5173 and proxies `/ws` and `/api` to the C++ server on port 4500. Run both in parallel for hot-reload development.

## Architecture

```text
Browser ─── WSS /ws ──── Symple v4 (signalling, presence, rooms)
        ─── GET /   ──── Static files (Vite build output)
        ─── GET /api ─── REST status
        ─── WebRTC  ──── Media (stream: H.264 + Opus out, record: H.264 in)
        ─── TURN    ──── NAT traversal (embedded, port 3478)
```

## Dependencies

All existing icey modules:

```text
media-server
├── webrtc (libdatachannel, track, mediabridge)
├── symple (server, protocol)
├── turn (server, allocations)
├── av (MediaCapture, VideoPacketEncoder, AudioPacketEncoder)
├── http (Server, WebSocket)
├── stun (STUN message parsing)
├── net, crypto, json, base
```

Frontend: `symple-client` + `symple-player` (npm, bundled with Vite).
