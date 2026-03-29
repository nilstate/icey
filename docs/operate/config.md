# Config Reference

`icey-server` reads configuration from a JSON file. CLI flags override config values. If neither is set, built-in defaults apply.

Default config path: `./config.json`. Override with `--config /path/to/config.json`.

## Sections

:::accordion{title="http — Server bind settings"}

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `host` | string | `"0.0.0.0"` | Bind address |
| `port` | number | `4500` | HTTP and WebSocket port |

```json
{
  "http": {
    "host": "0.0.0.0",
    "port": 4500
  }
}
```

:::

:::accordion{title="tls — Direct HTTPS/WSS serving"}

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `cert` | string | `""` | Path to the TLS certificate file |
| `key` | string | `""` | Path to the TLS private key file |

```json
{
  "tls": {
    "cert": "/etc/icey-server/certs/fullchain.pem",
    "key": "/etc/icey-server/certs/privkey.pem"
  }
}
```

:::note
Direct TLS is optional. If both fields are empty, icey-server serves plain HTTP/WSS on the primary port. If either field is set, both must be valid and loadable or `--doctor` will fail the preflight.
:::

:::

:::accordion{title="turn — Embedded TURN relay"}

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `enabled` | bool | `true` | Enable the embedded TURN server |
| `port` | number | `3478` | TURN listen port |
| `realm` | string | `"0state.com"` | TURN authentication realm |
| `externalIp` | string | `""` | Public IP to advertise in TURN allocations |

```json
{
  "turn": {
    "enabled": true,
    "port": 3478,
    "realm": "0state.com",
    "externalIp": ""
  }
}
```

:::warning
If you are behind a NAT and need relay to work for external browsers, `externalIp` must be your public IP. Without it, TURN allocations advertise the private address and remote browsers cannot connect. This is the single most common deployment failure.
:::

:::

:::accordion{title="media — Mode, source, and encoding"}

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `mode` | string | `"stream"` | `stream`, `record`, or `relay` |
| `source` | string | `""` | File path, device path, or RTSP URL (stream mode) |
| `recordDir` | string | `"./recordings"` | Output directory for record mode |
| `loop` | bool | `true` | Loop the source in stream mode |

### media.video

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `codec` | string | `"libx264"` | Video codec |
| `width` | number | `1280` | Output width |
| `height` | number | `720` | Output height |
| `fps` | number | `30` | Frame rate |
| `bitrate` | number | `2000000` | Target bitrate in bits/sec (adapts via REMB) |

### media.audio

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `codec` | string | `"libopus"` | Audio codec |
| `channels` | number | `2` | Channel count |
| `sampleRate` | number | `48000` | Sample rate in Hz |
| `bitrate` | number | `128000` | Audio bitrate in bits/sec |

```json
{
  "media": {
    "mode": "stream",
    "source": "/path/to/video.mp4",
    "recordDir": "./recordings",
    "loop": true,
    "video": {
      "codec": "libx264",
      "width": 1280,
      "height": 720,
      "fps": 30,
      "bitrate": 2000000
    },
    "audio": {
      "codec": "libopus",
      "channels": 2,
      "sampleRate": 48000,
      "bitrate": 128000
    }
  }
}
```

:::

:::accordion{title="media.intelligence — Vision and speech analysis"}

Intelligence branches run as side pipelines on decoded media. They emit events over the signalling channel without affecting transport.

:::note
These stages currently activate only in `stream` mode. If you keep them configured in `record` or `relay`, icey-server leaves them inactive and reports that state as a warning in `--doctor` and `/api/ready`.
:::

### media.intelligence.vision

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `enabled` | bool | `false` | Enable vision analysis |
| `everyNthFrame` | number | `6` | Sample every Nth decoded frame |
| `minIntervalUsec` | number | `200000` | Minimum interval between samples (μs) |
| `queueDepth` | number | `8` | Analysis queue depth |

### media.intelligence.vision.motion

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `gridWidth` | number | `32` | Motion detection grid width |
| `gridHeight` | number | `18` | Motion detection grid height |
| `warmupFrames` | number | `2` | Frames before detection activates |
| `threshold` | number | `0.08` | Motion threshold (0.0–1.0) |
| `cooldownUsec` | number | `500000` | Cooldown between motion events (μs) |

### media.intelligence.speech

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `enabled` | bool | `false` | Enable speech analysis |
| `queueDepth` | number | `32` | Analysis queue depth |
| `startThreshold` | number | `0.045` | VAD start threshold |
| `stopThreshold` | number | `0.02` | VAD stop threshold |
| `minSilenceUsec` | number | `250000` | Minimum silence to end speech (μs) |
| `updateIntervalUsec` | number | `250000` | Status update interval (μs) |

:::

:::accordion{title="webRoot — Web UI path"}

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| `webRoot` | string | `"./web/dist"` | Path to the built web UI directory |

The server serves static files from this directory at `/`. If the directory does not exist or does not contain `index.html`, the web UI will not load but the API and WebSocket endpoints will still work.

Packaged release artifacts and Debian packages bundle the web UI under `share/icey-server/web`. If you leave `webRoot` unset and `./web/dist` is missing, `icey-server` falls back to that packaged path automatically.

:::

## Config Examples

:::code-group

```json title="Minimal (stream a file)"
{
  "media": {
    "source": "/path/to/video.mp4"
  }
}
```

```json title="RTSP camera, production TURN"
{
  "turn": {
    "externalIp": "203.0.113.50"
  },
  "media": {
    "source": "rtsp://192.168.1.100:554/stream1",
    "loop": false
  }
}
```

```json title="Record mode, no TURN"
{
  "turn": {
    "enabled": false
  },
  "media": {
    "mode": "record",
    "recordDir": "/data/recordings"
  }
}
```

```json title="Low-bandwidth stream"
{
  "media": {
    "source": "/path/to/video.mp4",
    "video": {
      "width": 640,
      "height": 360,
      "fps": 15,
      "bitrate": 500000
    },
    "audio": {
      "bitrate": 64000
    }
  }
}
```

:::
