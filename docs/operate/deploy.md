# Deploy

How to run `icey-server` in production. Pick the deployment method that fits your infrastructure.

:::tabs

::tab{title="Docker Compose"}

```yaml
version: "3.8"
services:
  icey:
    image: 0state/icey:0.1.0
    network_mode: host
    volumes:
      - ./config.json:/etc/icey-server/config.json:ro
      - ./recordings:/data/recordings
    command: >
      --config /etc/icey-server/config.json
      --source /path/to/video.mp4
      --record-dir /data/recordings
    restart: unless-stopped
```

:::note
`network_mode: host` is the simplest path. It avoids port mapping and NAT complications. If you need bridge networking, you must publish ports 4500 and 3478 and set `turn.externalIp` to the host's public IP.
:::

::

::tab{title="systemd"}

Create `/etc/systemd/system/icey-server.service`:

```ini
[Unit]
Description=icey-server media runtime
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
ExecStart=/usr/bin/icey-server \
  --config /etc/icey-server/config.json
Restart=on-failure
RestartSec=5
User=icey
Group=icey
WorkingDirectory=/var/lib/icey-server

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now icey-server
sudo journalctl -u icey-server -f
```

::

::tab{title="Kubernetes"}

Minimal deployment. Adjust resource limits and storage for your workload.

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: icey-server
spec:
  replicas: 1
  selector:
    matchLabels:
      app: icey-server
  template:
    metadata:
      labels:
        app: icey-server
    spec:
      containers:
      - name: icey-server
        image: 0state/icey:0.1.0
        ports:
        - containerPort: 4500
          name: http
        - containerPort: 3478
          name: turn
          protocol: UDP
        args:
        - --config
        - /etc/icey-server/config.json
        volumeMounts:
        - name: config
          mountPath: /etc/icey-server
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
      volumes:
      - name: config
        configMap:
          name: icey-server-config
```

:::warning
WebRTC and TURN require UDP. If your cluster uses a network policy that blocks UDP, TURN relay will not work. You may need `hostNetwork: true` or a UDP-capable load balancer.
:::

::

:::

## Bring-Up Order

If you are deploying for the first time, validate each layer before adding the next.

:::steps

1. Stream mode, local file, no TURN

   ```bash
   icey-server --source ./demo.mp4 --no-turn
   ```

   This validates HTTP serving, Symple signalling, and the server-to-browser media path. If the browser cannot see the server peer, you have a signalling problem. If video does not play, you have a media path problem. Neither involves TURN.

2. Record mode

   ```bash
   icey-server --mode record --record-dir ./recordings --no-turn
   ```

   This validates the browser-to-server media path. Grant camera access in the browser, click Call, and check that MP4 files appear in the recording directory.

3. Relay mode

   ```bash
   icey-server --mode relay --no-turn
   ```

   This validates the full bidirectional path. First browser becomes the source, second browser becomes the viewer.

4. Enable TURN

   Remove `--no-turn`. Set `--turn-external-ip` to your public IP. Test from a browser outside your local network.

   If relay fails under NAT, check the [TURN deployment guide](turn.md) before touching the media code.

:::

## Pre-Flight Checklist

Before starting the server in any environment:

- OpenSSL is installed and discoverable by the binary
- FFmpeg is installed and discoverable by the binary
- `--web-root` points at the actual built `dist/` directory (or the Docker image includes it)
- `--record-dir` is writable if you use record mode
- `--source` points to a real file, device, or reachable RTSP URL if you use stream mode
- TURN port (3478) is reachable from outside if you expect relay through NATs

## Ports And Firewalls

| Service | Port | Protocol | Required |
| --- | --- | --- | --- |
| HTTP / WebSocket | 4500 | TCP | Always |
| TURN relay | 3478 | UDP + TCP | Unless `--no-turn` |

If you are behind a firewall, both ports must be reachable from the browser. TURN uses UDP by default with TCP fallback.

## TLS

See the [TLS guide](tls.md) for HTTPS and WSS termination options.
