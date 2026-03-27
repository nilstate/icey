# Media Server Stack

This is the full self-hosted path:

- HTTP server
- Symple signalling
- WebRTC session control
- embedded TURN
- browser UI

in one C++ binary.

That is what makes the deployable `icey` media server interesting. It is not a thin demo around a managed service. It is the whole stack in one place.

The deployable product surface now lives outside the LGPL core tree in the separate `icey-cli` app layer. This recipe documents the architecture and bring-up shape the app is built around.

## Express Path

If you want the shortest path to a running browser demo, start with the published `icey-cli` media server image:

```bash
docker run --rm --network host 0state/icey-media-server-demo:latest
```

Then open `http://localhost:4500`.
In the default `stream` mode, click `Watch` on the `icey` peer.

## Source Path

If you want the source-backed path instead, build the separate `icey-cli` app against this repo:

```bash
cmake -S /path/to/icey-cli -B /path/to/icey-cli/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DICEY_SOURCE_DIR=/path/to/icey
cmake --build /path/to/icey-cli/build --target icey-server

cd /path/to/icey-cli/web
npm install
npm run build
```

## What Runs

The server exposes:

- HTTP for the web UI and simple API endpoints
- WebSocket for Symple signalling
- WebRTC media sessions
- TURN relay on a second port unless disabled

The browser side is the bundled `icey-cli/web/` UI build.

## Modes

The deployable server has three real modes now:

| Mode | Browser sends | Server sends | What it is for |
| --- | --- | --- | --- |
| `stream` | call control | H.264 + Opus | server-originated media |
| `record` | H.264 video | call control | browser-to-server recording |
| `relay` | one source caller | encoded relay to viewers | browser-originated live fanout |

These are not aspirational docs modes. They are the modes the app actually implements.

## Build It

Build the server:

```bash
cmake -S /path/to/icey-cli -B /path/to/icey-cli/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DICEY_SOURCE_DIR=/path/to/icey
cmake --build /path/to/icey-cli/build --target icey-server
```

Build the UI:

```bash
cd /path/to/icey-cli/web
npm install
npm run build
```

Then run the server:

```bash
/path/to/icey-cli/build/src/server/icey-server \
  --web-root /path/to/icey-cli/web/dist \
  --source /path/to/video.mp4
```

## Ports And Public Addressing

By default:

- HTTP / WS: `4500`
- TURN: `3478`

If the host has a direct public IP, life is easy.

If it does not, pay attention to the relay address the TURN server should advertise. That is the same `externalIP` problem described in the TURN docs, and it matters just as much here.

If relay looks broken under real NAT conditions, check the TURN public address story before touching the media code.

## Browser Flow

The normal browser path is:

1. load the UI over HTTP
2. connect to Symple over WebSocket
3. discover the server peer via presence
4. place a call
5. the media path depends on mode

That last step is the main difference:

- `stream`: server sends the source media to the browser
- `record`: browser sends video to the server and the server records it
- `relay`: first active caller becomes the source, later callers become viewers

## The Important Runtime Pieces

The deployable server works because it is sitting on top of the rest of icey, not re-implementing it.

- `http` serves the UI and status endpoints
- `symple` carries signalling and presence
- `webrtc` owns session control and track send/receive
- `turn` handles relay
- `av` handles capture, encode, decode, and mux
- `base` and `net` hold the runtime together underneath

If you are debugging the stack, debug it in those layers. Do not treat the app as one monolithic mystery box.

## Deployment Checklist

- OpenSSL is installed and discoverable
- FFmpeg is installed and discoverable
- the UI has been built and `--web-root` points at the actual `dist/`
- `--record-dir` is writable if you use record mode
- TURN port is reachable if you expect relay under hard NATs
- the source path is real if you use stream mode

## Local Bring-Up

For simple local work:

- run with `--no-turn` if you do not need relay
- use a local source file in `stream` mode
- verify `record` mode before trying to debug `relay`

That ordering saves time because it separates:

- signalling
- media send path
- media receive path
- relay path

instead of testing all four at once.

## Good Next Stops

- [WebRTC guide](../modules/webrtc.md) for the session and track layers
- [TURN guide](../modules/turn.md) for relay behavior
- [HTTP Lifecycle](../concepts/http-lifecycle.md) for the server side of the UI and signalling transport
- the separate `icey-cli` app README for the app-local operational surface
