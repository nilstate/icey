# Media Server

Single C++ binary: WebRTC media streaming + Symple signalling + TURN relay + web UI.

No Node.js runtime, no third-party services. One binary, two ports (HTTP/WS + TURN).

## Media Server Demo

If you need the shortest end-to-end path, use the packaged [Media Server Demo](docker/README.md):

```bash
cd src/webrtc/apps/media-server/docker
docker compose up --build
```

Then open `http://localhost:4500` and click `Watch` on the `Media Server` peer.

## Native Quick Start

```bash
# Build the C++ server
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_APPLICATIONS=ON
cmake --build build --target media-server

# Build the web UI
cd src/webrtc/apps/media-server/web
npm install
npm run build

# Run
./build/bin/media-server --web-root src/webrtc/apps/media-server/web/dist --source /path/to/video.mp4
```

Open `http://localhost:4500` in a browser.

Validated browser smoke:
- Chromium engine via Playwright using the system Google Chrome binary
- Firefox via Playwright

WebKit smoke is wired in the test harness, but the Playwright WebKit/WPE runtime on this Linux host is not treated as authoritative for publish-path support. Do not claim Safari support until it has been validated on Apple platforms.

The `media-server` target is built when the `webrtc` prerequisites are available: OpenSSL and FFmpeg must be installed or discoverable by CMake, and libdatachannel is fetched automatically.

## Modes

- **stream** (default): server pushes file/camera to browser via WebRTC (H.264 + Opus)
- **record**: browser sends H.264 video to the server, which decodes it and writes MP4 files to disk
- **relay**: the first active caller becomes the live source; later callers receive that source via server-side encoded relay fanout

## Mode Behavior

| Mode | Browser sends | Server sends | Notes |
| --- | --- | --- | --- |
| `stream` | call control | H.264 + Opus | source comes from the local file or device configured with `--source` |
| `record` | H.264 video | call control | output goes to timestamped MP4 files under `--record-dir` |
| `relay` | one active source call | encoded media to viewers | first active caller becomes source; later callers become viewers |

## Features

- **Video + Audio**: H.264 Constrained Baseline (browser-safe) + Opus at 48kHz
- **Embedded TURN**: RFC 5766 relay on port 3478 — works through symmetric NATs
- **Adaptive bitrate**: REMB feedback adjusts encoder bitrate in real-time
- **Per-session isolation**: each peer gets its own capture/encoder pipeline in stream mode and its own recorder pipeline in record mode
- **Server-side relay**: relay mode forwards one active browser source to all connected viewers without decoding/re-encoding
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

## Browser Counterpart

The intended counterpart is the bundled web UI. It handles:

- Symple presence and discovery
- call placement
- SDP and ICE transport
- playback or source publishing depending on mode

If the browser cannot even see the server peer, you have a signalling problem, not a media problem.

## Web UI development

```bash
cd web
npm run dev
```

Vite dev server runs on port 5173 and proxies `/ws` and `/api` to the C++ server on port 4500. Run both in parallel for hot-reload development.

## Bring-Up Order

If you are bringing the full stack up from scratch, do it in this order:

1. `stream` mode with a local source file and `--no-turn`
2. `record` mode
3. `relay` mode
4. TURN-enabled external/NAT testing

That keeps signalling, send path, receive path, relay, and public addressing problems separated instead of mixing them together on day one.

## Architecture

```text
Browser ─── WSS /ws ──── Symple v4 (signalling, presence, rooms)
        ─── GET /   ──── Static files (Vite build output)
        ─── GET /api ─── REST status
        ─── WebRTC  ──── Media (stream: H.264 + Opus out, record: H.264 in, relay: one browser in -> many browsers out)
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

## See Also

- [Media Server Stack recipe](../../../../doc/recipes/media-server-stack.md)
- [WebRTC guide](../../../../doc/modules/webrtc.md)
- [TURN guide](../../../../doc/modules/turn.md)
