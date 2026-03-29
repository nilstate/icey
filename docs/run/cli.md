# CLI Reference

Full reference for the `icey-server` binary.

```text
icey-server [options]
```

## Flags

:::accordion{title="Server"}

| Flag | Default | Description |
| --- | --- | --- |
| `-c, --config <path>` | `./config.json` | Path to the config file |
| `--host <host>` | `0.0.0.0` | Bind address for the HTTP/WebSocket server |
| `--port <port>` | `4500` | HTTP/WebSocket port |
| `--tls-cert <path>` | (none) | TLS certificate for direct HTTPS/WSS serving |
| `--tls-key <path>` | (none) | TLS private key for direct HTTPS/WSS serving |
| `--web-root <path>` | `./web/dist` | Path to the built web UI directory. If omitted, packaged installs fall back to `../share/icey-server/web` |

:::

:::accordion{title="Media"}

| Flag | Default | Description |
| --- | --- | --- |
| `--mode <mode>` | `stream` | Operational mode: `stream`, `record`, or `relay` |
| `--source <path-or-url>` | (none) | Media source: file path, device path, or RTSP URL |
| `--record-dir <path>` | `./recordings` | Output directory for record mode |
| `--loop` | (from config) | Enable looping in stream mode |
| `--no-loop` | — | Disable looping in stream mode |

:::

:::accordion{title="TURN"}

| Flag | Default | Description |
| --- | --- | --- |
| `--turn-port <port>` | `3478` | TURN relay port |
| `--turn-external-ip <ip>` | (none) | Public IP advertised by the TURN server |
| `--no-turn` | — | Disable the embedded TURN server entirely |

:::warning
If you are behind a NAT and need relay, `--turn-external-ip` must be set to your public IP. Without it, TURN allocations will advertise the private address and browsers outside the network will fail to connect. This is the most common deployment mistake.
:::

:::

:::accordion{title="Info"}

| Flag | Description |
| --- | --- |
| `--doctor` | Run diagnostics and report the health of the local environment |
| `--version` | Print version and exit |
| `-h, --help` | Show help and exit |

:::

## Config File

CLI flags override config file values. If a flag is not set, the config file value is used. If neither is set, the built-in default applies.

The config file is JSON. Here is the full default:

:::code-group

```json title="Minimal"
{
  "media": {
    "mode": "stream",
    "source": "/path/to/video.mp4"
  }
}
```

```json title="Full"
{
  "http": {
    "host": "0.0.0.0",
    "port": 4500
  },
  "tls": {
    "cert": "",
    "key": ""
  },
  "turn": {
    "enabled": true,
    "port": 3478,
    "realm": "0state.com",
    "externalIp": ""
  },
  "media": {
    "mode": "stream",
    "source": "",
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
    },
    "intelligence": {
      "vision": {
        "enabled": false,
        "everyNthFrame": 6,
        "minIntervalUsec": 200000,
        "queueDepth": 8,
        "motion": {
          "gridWidth": 32,
          "gridHeight": 18,
          "warmupFrames": 2,
          "threshold": 0.08,
          "cooldownUsec": 500000
        }
      },
      "speech": {
        "enabled": false,
        "queueDepth": 32,
        "startThreshold": 0.045,
        "stopThreshold": 0.02,
        "minSilenceUsec": 250000,
        "updateIntervalUsec": 250000
      }
    }
  },
  "webRoot": "./web/dist"
}
```

:::

For a detailed breakdown of every config field, see the [config reference](../operate/config.md).

## Operator Endpoints

The running server exposes REST endpoints for health checks and status:

| Endpoint | What it returns |
| --- | --- |
| `GET /api/health` | Liveness metadata: `status`, `product`, `service`, `version` |
| `GET /api/ready` | Structured preflight report and readiness result |
| `GET /api/status` | Preflight state plus mode, peer identity, session counts, `uptimeSec`, stream/record paths, and intelligence state |
| `GET /api/config` | Browser-facing config: mode, HTTP host/scheme, TURN credentials, and STUN URLs |

See the [health and monitoring guide](../operate/health.md) for integration with monitoring systems.

## Examples

Stream a local file with TURN disabled:

```bash
icey-server --source ./demo.mp4 --no-turn
```

Record browser video to a custom directory:

```bash
icey-server --mode record --record-dir /data/recordings
```

Production deployment with explicit TURN address:

```bash
icey-server \
  --source rtsp://192.168.1.100:554/stream1 \
  --turn-external-ip 203.0.113.50 \
  --config /etc/icey-server/config.json
```

Direct HTTPS/WSS without a reverse proxy:

```bash
icey-server \
  --source rtsp://192.168.1.100:554/stream1 \
  --tls-cert /etc/icey-server/certs/fullchain.pem \
  --tls-key /etc/icey-server/certs/privkey.pem
```
