# TURN Deployment

icey-server includes an embedded RFC 5766 TURN relay. It runs on a second port alongside the HTTP/WebSocket server. No external TURN service needed.

TURN exists for one reason: when the browser and server cannot establish a direct UDP path (symmetric NATs, restrictive firewalls, carrier-grade NAT), the TURN relay forwards media through a reachable address.

## When You Need TURN

- **LAN or same-network**: You probably do not. Direct UDP works. Start with `--no-turn` and add it later.
- **Server has a public IP, browser is behind NAT**: STUN (built into WebRTC) usually handles this. TURN is a fallback.
- **Both sides behind NAT**: You need TURN. This is the common production case.
- **Restrictive corporate firewalls**: You need TURN, possibly over TCP.

## Configuration

:::tabs

::tab{title="LAN / Development"}

Disable TURN. Direct paths work on the local network.

```bash
icey-server --no-turn --source ./demo.mp4
```

Or in config:

```json
{
  "turn": {
    "enabled": false
  }
}
```

::

::tab{title="Public Server"}

Server has a direct public IP. Set `externalIp` to that address.

```bash
icey-server --turn-external-ip 203.0.113.50 --source ./demo.mp4
```

Or in config:

```json
{
  "turn": {
    "enabled": true,
    "externalIp": "203.0.113.50"
  }
}
```

Make sure port 3478 (UDP + TCP) is open in your firewall.

::

::tab{title="Cloud / NAT"}

Server is behind a NAT (AWS, GCP, most VPS providers). The server's network interface has a private IP, but there is a public IP mapped to it.

```bash
icey-server --turn-external-ip <YOUR_PUBLIC_IP> --source ./demo.mp4
```

:::warning
This is where most people get stuck. If `externalIp` is not set, TURN allocations advertise the private address. Browsers outside the network get an address they cannot reach, and media fails silently. The signalling will look fine. The browser will show "connecting." Video will never appear.
:::

To find your public IP on common cloud providers:

```bash
# AWS
curl -s http://169.254.169.254/latest/meta-data/public-ipv4

# GCP
curl -s -H "Metadata-Flavor: Google" http://metadata.google.internal/computeMetadata/v1/instance/network-interfaces/0/access-configs/0/external-ip

# Generic
curl -s https://ifconfig.me
```

::

:::

## Firewall Rules

| Port | Protocol | Direction | Purpose |
| --- | --- | --- | --- |
| 3478 | UDP | Inbound | TURN relay (primary) |
| 3478 | TCP | Inbound | TURN relay (fallback) |

TURN uses UDP by default. TCP is a fallback for networks that block UDP. Both should be open.

## Troubleshooting TURN

If browsers connect to signalling but video never appears:

1. Check that `externalIp` is set and correct. This is the cause 90% of the time.
2. Check that port 3478 is reachable from outside. Use `nc -u <ip> 3478` to test.
3. Check the server logs for TURN allocation errors.
4. Try forcing TCP TURN in the browser's ICE config to rule out UDP firewall issues.

If the browser cannot even see the server peer in the UI, the problem is signalling, not TURN. Check the WebSocket connection first.

## Library Users

If you are building your own TURN server using icey's `turn` module rather than using the embedded server in `icey-server`, see the [TURN module guide](../modules/turn.md) and the [TURN server recipe](../recipes/turn-server.md).
