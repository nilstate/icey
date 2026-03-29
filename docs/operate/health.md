# Health And Monitoring

icey-server exposes REST endpoints for health checks, readiness probes, and runtime status.

## Endpoints

| Endpoint | Purpose |
| --- | --- |
| `GET /api/health` | Liveness check. Returns 200 if the process is running. |
| `GET /api/ready` | Readiness probe. Returns the structured preflight report and sends 503 when `ready` is false. |
| `GET /api/status` | Runtime status: preflight state, mode, version, session counts, `uptimeSec`, peer identity, stream/record paths, and intelligence state. |
| `GET /api/config` | Browser-facing runtime config: mode, product/service identity, HTTP host/scheme, TURN credentials, and STUN URLs. |

## Response Examples

:::code-group

```json title="GET /api/health"
{
  "status": "ok",
  "product": "icey",
  "service": "icey-server",
  "version": "0.1.1"
}
```

```json title="GET /api/ready"
{
  "status": "degraded",
  "ready": true,
  "product": "icey",
  "service": "icey-server",
  "mode": "stream",
  "checks": [
    {
      "name": "config",
      "result": "pass",
      "detail": "Loaded config from /etc/icey-server/config.json"
    },
    {
      "name": "turn",
      "result": "warn",
      "detail": "TURN is enabled without externalIp while binding to a wildcard host"
    }
  ],
  "warnings": [
    "TURN externalIp is unset; remote NAT traversal may fail outside local or host-network testing."
  ]
}
```

```json title="GET /api/status"
{
  "status": "ok",
  "ready": true,
  "product": "icey",
  "service": "icey-server",
  "mode": "stream",
  "version": "0.1.1",
  "peer": {
    "id": "server",
    "name": "Icey Server",
    "type": "server"
  },
  "runtime": {
    "state": "running"
  },
  "uptimeSec": 3600,
  "sessions": {
    "active": 2,
    "total": 15
  },
  "stream": {
    "source": "rtsp://camera.local/stream1",
    "sourceKind": "rtsp",
    "loop": false
  },
  "record": {
    "dir": "./recordings"
  },
  "intelligence": {
    "vision": true,
    "speech": true
  }
}
```

```json title="GET /api/config"
{
  "status": "ok",
  "mode": "stream",
  "product": "icey",
  "service": "icey-server",
  "version": "0.1.1",
  "http": {
    "host": "0.0.0.0",
    "scheme": "https",
    "tls": true
  },
  "turn": {
    "enabled": true,
    "host": "203.0.113.50",
    "port": 3478,
    "username": "icey",
    "credential": "icey"
  },
  "stun": {
    "urls": [
      "stun:stun.l.google.com:19302"
    ]
  }
}
```

:::

## Integration

### Kubernetes

Use `/api/health` for liveness and `/api/ready` for readiness:

```yaml
livenessProbe:
  httpGet:
    path: /api/health
    port: 4500
  periodSeconds: 10
readinessProbe:
  httpGet:
    path: /api/ready
    port: 4500
  periodSeconds: 5
```

### Docker Compose

```yaml
healthcheck:
  test: ["CMD", "curl", "-f", "http://localhost:4500/api/health"]
  interval: 10s
  timeout: 3s
  retries: 3
```

### Monitoring

Poll `/api/status` for session counts, `uptimeSec`, readiness state, and intelligence state. Use `/api/ready` when you want the preflight report and warnings as part of orchestration.

Key things to watch:

- **Active session count** — a sudden drop may indicate a crash or network issue
- **`uptimeSec`** — a reset to zero means the process restarted
- **`ready` + `checks`** — if readiness flips false, the preflight report tells you which check failed
- **Intelligence state** — if you expect vision/speech events but they are disabled or warned inactive, check mode and config

## Structured Logs

icey-server logs to stderr. Log output includes timestamps, severity, and module context. In production, redirect to your log aggregator:

```bash
icey-server --source ./demo.mp4 2>&1 | your-log-shipper
```

Or with systemd, logs go to journald automatically:

```bash
journalctl -u icey-server -f
```
