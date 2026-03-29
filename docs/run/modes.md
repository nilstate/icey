# Modes

icey-server has three operational modes. Each changes what the server does with media and what the browser's role is.

| Mode | Browser sends | Server sends | Use case |
| --- | --- | --- | --- |
| `stream` | call control | H.264 + Opus | server-originated media to browser |
| `record` | H.264 video | call control | browser video captured to disk |
| `relay` | one source caller | encoded relay to viewers | browser-originated live fanout |

Set the mode with `--mode` or in the config file under `media.mode`.

:::tabs

::tab{title="Stream"}

## Stream Mode

The server captures from a local file, camera, or RTSP URL, encodes H.264 + Opus, and pushes it to every connected browser over WebRTC.

```bash
icey-server --mode stream --source /path/to/video.mp4
```

Each browser peer gets its own capture and encoder pipeline. The server starts sending media as soon as the WebRTC session reaches the `Active` state.

### Config

:::code-group

```json title="File source"
{
  "media": {
    "mode": "stream",
    "source": "/path/to/video.mp4",
    "loop": true
  }
}
```

```json title="Camera source"
{
  "media": {
    "mode": "stream",
    "source": "/dev/video0",
    "loop": false
  }
}
```

```json title="RTSP source"
{
  "media": {
    "mode": "stream",
    "source": "rtsp://192.168.1.100:554/stream1",
    "loop": false
  }
}
```

:::

### Video and audio encoding

Default encoding: H.264 Constrained Baseline at 1280x720, 30fps, 2Mbps. Opus at 48kHz stereo, 128kbps.

The encoder uses `ultrafast` preset and `zerolatency` tune for real-time delivery. Bitrate adapts in real-time based on REMB feedback from the browser.

All encoding parameters are configurable in `media.video` and `media.audio`. See the [config reference](../operate/config.md).

::

::tab{title="Record"}

## Record Mode

The browser sends H.264 video to the server. The server decodes it and writes timestamped MP4 files to disk.

```bash
icey-server --mode record --record-dir ./recordings
```

Each browser peer gets its own recorder pipeline. The muxer initializes lazily on the first keyframe, so it survives mid-GOP joining.

### Config

```json
{
  "media": {
    "mode": "record",
    "recordDir": "./recordings"
  }
}
```

:::warning
icey-server creates the recording directory if needed, but the process still needs permission to create and write files there. The server creates one timestamped MP4 per session.
:::

### Browser side

Open the UI, grant camera access, and click **Call**. The browser sends its camera feed to the server. Recording starts automatically.

::

::tab{title="Relay"}

## Relay Mode

The first active browser caller becomes the live source. Every subsequent caller receives that source via server-side relay. No server-side capture or encoding — the server forwards the first caller's encoded media directly to viewers.

```bash
icey-server --mode relay
```

### Config

```json
{
  "media": {
    "mode": "relay"
  }
}
```

### How it works

1. First browser connects and sends media → becomes the source.
2. Later browsers connect → receive the source caller's media.
3. If the source disconnects, the relay stops until a new source connects.

The relay path forwards encoded media without decoding or re-encoding. This keeps latency low and CPU usage minimal.

::

:::

## Intelligence

Real-time intelligence branches for vision (motion detection, frame sampling) and speech (voice activity detection) currently activate only in `stream` mode. They run as side pipelines on decoded media and emit events over the WebSocket signalling channel without affecting the transport path.

If you keep intelligence configured in `record` or `relay` mode, the server leaves those stages inactive and `--doctor` reports that as a warning instead of silently pretending they are active.

Configure or disable these stages in `media.intelligence`. See the [config reference](../operate/config.md).

## Switching Modes

Modes are set at startup, not at runtime. To switch modes, restart the server with a different `--mode` flag or update the config file.

If you are bringing the stack up for the first time, work through the modes in this order:

:::steps

1. **Stream** with a local file and `--no-turn`

   This validates signalling and the server-to-browser media path with the fewest moving parts.

2. **Record**

   This validates the browser-to-server media path and recording pipeline.

3. **Relay**

   This validates the full bidirectional path: browser sends, server relays, other browsers receive.

4. **Enable TURN**

   Once all three modes work on localhost, remove `--no-turn` and test under real NAT conditions.

:::

That ordering keeps signalling, send path, receive path, relay, and public addressing problems separated instead of mixed together on day one.
