# Troubleshoot

Common failure modes and how to fix them. Start with the symptom, follow the tree.

## Diagnostics

Run `icey-server --doctor` before you start debugging browser symptoms. It prints the same structured preflight report exposed by `/api/ready` and exits non-zero when the server is not ready to start.

The report checks:

- config load validity
- OpenSSL runtime availability
- FFmpeg support in the current build
- bundled web UI presence at the resolved `webRoot`
- HTTP/TURN port conflicts
- source readiness for `stream` mode
- direct TLS certificate/key validity
- TURN external IP warnings
- intelligence configured outside `stream` mode

## Failure Trees

:::accordion{title="Browser cannot see the server peer"}

**Symptom**: The web UI loads but the peer list is empty or the server peer never appears.

This is a **signalling problem**, not a media problem. TURN is not involved yet.

1. Check that the WebSocket connection succeeded. Open browser DevTools → Network → WS. You should see an active connection to `/ws`.
2. If the WebSocket is not connecting, check that the server is actually running and the port is reachable.
3. If you are behind a reverse proxy, make sure it passes `Upgrade` and `Connection` headers. Without them, the WebSocket handshake fails silently and falls back to long-polling or just dies.
4. Check the server logs for Symple connection errors.

:::

:::accordion{title="Video never appears (stream mode)"}

**Symptom**: The browser sees the server peer, you click Watch, but video never plays. The UI may show "connecting" indefinitely.

1. **Check TURN first.** If the server is behind a NAT and `--turn-external-ip` is not set, the browser gets an unreachable relay address. This is the cause 90% of the time. See the [TURN deployment guide](turn.md).
2. Check the server logs for WebRTC session state changes. The session should progress to `Active`. If it stalls at `Connecting`, the ICE candidate exchange is failing.
3. Check the browser console for ICE connection failures.
4. If you are on localhost, try `--no-turn` to rule out TURN as the issue.
5. Check that the source file exists and is readable. If `--source` points to a missing file, capture will fail silently and no media will be sent.

:::

:::accordion{title="Recording produces no files (record mode)"}

**Symptom**: Browser connects, camera is active, but no MP4 files appear in the recording directory.

1. Check that `--record-dir` points to a writable location. icey-server creates the directory if needed, but the process still needs permission to create and write files there.
2. The muxer initializes on the first keyframe. If the browser sends video but the first keyframe has not arrived yet, there will be a short delay.
3. Check the server logs for decoder or muxer errors.
4. Make sure the browser is actually sending video. Check the WebRTC stats in the browser (chrome://webrtc-internals) for outbound RTP packets.

:::

:::accordion{title="TURN relay fails"}

**Symptom**: Everything works on localhost but fails when the browser is on a different network.

1. Is `--turn-external-ip` set to the correct public IP? This is the most common cause. See [TURN deployment](turn.md).
2. Is port 3478 (UDP + TCP) open in the firewall? Test with `nc -u <public-ip> 3478`.
3. Check the server logs for TURN allocation errors.
4. Try forcing TCP in the ICE config to rule out UDP being blocked.

:::

:::accordion{title="TLS / certificate errors"}

**Symptom**: Browser refuses to connect, shows mixed content warnings, or WebRTC fails because the page is not HTTPS.

1. WebRTC requires HTTPS in production (non-localhost). If you are serving over HTTP from a real hostname, browsers will block camera access and ICE negotiation.
2. If you are using direct TLS, run `icey-server --doctor`. It validates that both the certificate and key exist and that the keypair can initialize the HTTPS/WSS listen socket.
3. If you are using a reverse proxy, check that the proxy config includes `Upgrade` and `Connection` headers for WebSocket passthrough.
4. If certificates are expired, `certbot renew` and restart the proxy.
5. See the [TLS guide](tls.md) for full setup instructions.

:::

:::accordion{title="Codec mismatch / no supported codecs"}

**Symptom**: Session connects but media negotiation fails. Browser console may show SDP errors or codec negotiation failures.

1. icey-server uses H.264 Constrained Baseline + Opus. These are universally supported in modern browsers.
2. If you see codec errors, check that FFmpeg was built with `libx264` and `libopus` support. Run `ffmpeg -codecs | grep -E 'x264|opus'` to verify.
3. Check that the browser is not running in a restricted mode that disables H.264 (some enterprise policies do this).

:::

:::accordion{title="High latency or poor video quality"}

**Symptom**: Video plays but with noticeable delay or visual artifacts.

1. The default encoder uses `ultrafast` preset and `zerolatency` tune. If you changed these, revert to defaults.
2. Check if REMB bitrate adaptation is working. If the network is constrained, the encoder should reduce bitrate automatically. Check WebRTC stats for bitrate changes.
3. If latency is consistently high, check whether media is being relayed through TURN when a direct path is available. TURN adds a hop.
4. On slow hardware, encoding 1280x720 at 30fps may saturate the CPU. Reduce resolution or frame rate in the config.

:::

## Debugging Approach

`icey-server` is not a monolith — it is an assembly of independent library modules. Debug it in those layers:

- `http` serves the UI and status endpoints
- `symple` carries signalling and presence
- `webrtc` owns session control and track send/receive
- `turn` handles relay
- `av` handles capture, encode, decode, and mux
- `base` and `net` hold the runtime together underneath

If something breaks, isolate which layer is failing. Do not treat the binary as one opaque mystery box.

## Still Stuck

Check the server logs first. Every failure above produces log output that points to the cause. If the logs are not helpful, file an issue at [github.com/nilstate/icey-cli](https://github.com/nilstate/icey-cli/issues) with the log output and your config (redact credentials).
