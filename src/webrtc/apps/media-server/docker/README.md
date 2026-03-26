# Media Server Demo

This is the fastest end-to-end path through the Icey media stack:

- C++ media server
- bundled web UI
- Symple signalling over WebSocket
- embedded TURN relay
- real browser playback with a shipped demo source

This is the one place to point people for the demo.

One command. One URL. One click.

## Quick Start

The supported local path is Linux with host networking:

```bash
cd src/webrtc/apps/media-server/docker
docker compose up --build
```

Then open:

```text
http://localhost:4500
```

Click `Watch` on the `Media Server` peer. The container ships with `data/test.mp4` as the default stream source, so the browser should start receiving video immediately.

## What This Runs

The image packages:

- the `media-server` application
- the production web UI build from `src/webrtc/apps/media-server/web/`
- the sample media file at `data/test.mp4`

It reuses the existing local build artifacts instead of recompiling inside Docker, so rebuilds stay cheap.

If your current binary lives in a different build tree, point Docker at it directly:

```bash
ICEY_MEDIA_SERVER_BINARY=build-webrtc-smoke/webrtc/apps/media-server/media-server docker compose build
docker compose up
```

The server starts in `stream` mode by default and listens on:

- HTTP / WebSocket: `4500`
- TURN: `3478` TCP and UDP

## Why Host Networking

This Media Server Demo is meant to prove real WebRTC and TURN behavior, not just serve HTML.

Docker bridge networking hides the host-facing ICE and relay addresses the browser needs. `network_mode: host` keeps the advertised addresses honest on Linux, which is the path that consistently proves the full stack.

## Runtime Overrides

You can switch the app mode or input source with environment variables:

```bash
ICEY_MODE=record docker compose up --build
ICEY_MODE=relay docker compose up --build
ICEY_SOURCE=/app/media/test.mp4 docker compose up --build
```

`record` mode writes files into:

```text
src/webrtc/apps/media-server/docker/recordings/
```

## Notes

- This is the canonical Media Server Demo for the self-hosted media stack.
- For direct local use, open `http://localhost:4500`.
- The image expects the existing local `build-webrtc/.../media-server` binary and `web/dist/` output to be present.
- For local smoke runs, the test harness prefers `build-webrtc-smoke/.../media-server` when that build tree exists.
- If port `4500` or `3478` is already busy on the host, free it first or override `ICEY_PORT` / `ICEY_TURN_PORT`.
- Docker Desktop host networking is not the primary target here. For non-Linux hosts, use the native app README first.

## Related

- [App README](../README.md)
- [Media Server Stack recipe](../../../../../doc/recipes/media-server-stack.md)
