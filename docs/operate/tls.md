# TLS

WebRTC requires HTTPS in production browsers. If the page is served over plain HTTP from a non-localhost origin, browsers will refuse to grant camera/microphone access and WebRTC negotiation will fail silently.

Two options: terminate TLS at a reverse proxy, or serve TLS directly from icey-server.

:::tabs

::tab{title="Reverse Proxy (recommended)"}

Put nginx, Caddy, or any TLS-terminating proxy in front of icey-server. The proxy handles certificates. icey-server stays on plain HTTP.

### Caddy

Caddy handles certificates automatically via Let's Encrypt.

```
media.example.com {
    reverse_proxy localhost:4500
}
```

That is the entire config. Caddy provisions and renews the certificate.

### nginx

```nginx
server {
    listen 443 ssl;
    server_name media.example.com;

    ssl_certificate     /etc/letsencrypt/live/media.example.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/media.example.com/privkey.pem;

    location / {
        proxy_pass http://127.0.0.1:4500;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

:::warning
The `Upgrade` and `Connection` headers are required for WebSocket. Without them, Symple signalling will not connect and the browser will not discover the server peer.
:::

### Certificate setup with certbot

:::steps

1. Install certbot

   ```bash
   sudo apt install certbot
   ```

2. Get a certificate

   ```bash
   sudo certbot certonly --standalone -d media.example.com
   ```

3. Point your proxy config at the certificate files

4. Set up automatic renewal

   ```bash
   sudo certbot renew --dry-run
   ```

:::

::

::tab{title="Direct TLS"}

icey-server can serve HTTPS and WSS directly if you provide certificate files. This avoids the proxy layer but means you manage certificates yourself.

For most deployments, a reverse proxy is simpler and more maintainable. Use direct TLS when you cannot run a proxy (embedded devices, single-binary edge deployments).

### CLI

```bash
icey-server \
  --source rtsp://camera.local/stream1 \
  --tls-cert /etc/icey-server/certs/fullchain.pem \
  --tls-key /etc/icey-server/certs/privkey.pem
```

### Config

```json
{
  "http": {
    "host": "0.0.0.0",
    "port": 4500
  },
  "tls": {
    "cert": "/etc/icey-server/certs/fullchain.pem",
    "key": "/etc/icey-server/certs/privkey.pem"
  },
  "media": {
    "mode": "stream",
    "source": "rtsp://camera.local/stream1"
  }
}
```

Run `icey-server --doctor` before starting the service. The preflight checks verify that both files exist and that the certificate/key pair can initialize the direct TLS listener cleanly.

::

:::

## TURN And TLS

TURN operates on its own port (default 3478) and is not affected by the HTTP TLS configuration. TURN has its own security model based on long-term credentials (RFC 5389).

Browsers connect to TURN over UDP or TCP. TURN-over-TLS (port 5349) is not currently supported. For environments that block all non-HTTPS traffic, use a TCP TURN allocation on port 443 behind a multiplexing proxy.
