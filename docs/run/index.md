# See It Work

icey-server is a single C++ binary for browser-facing realtime media over WebRTC. No Node.js, no third-party services, no multi-container ceremony. One binary, two ports, live video.

## 60-Second Path

:::steps

1. Run the server

   ```bash
   docker run --rm --network host 0state/icey-server:latest
   ```

2. Open a browser

   Go to `http://localhost:4500`.

3. Watch the stream

   Click **Watch** on the `icey` peer. You should see live video in under a second.

:::

That is the entire setup. The container includes the server binary, a bundled web UI, and a test source. Everything else — signalling, TURN relay, session control — is built in.

## Browser Flow

What happened in the browser when you clicked Watch:

1. The web UI loaded over HTTP from the server.
2. A WebSocket connected to `/ws` for Symple signalling.
3. The browser discovered the `icey` server peer via presence.
4. A call was placed. SDP offer/answer and ICE candidates exchanged over the signalling channel.
5. The media path opened — in stream mode, the server started sending H.264 + Opus to the browser.

If the browser cannot see the server peer, you have a signalling problem (step 2-3). If it sees the peer but video never appears, you have a media or TURN problem (step 4-5). See the [troubleshooting guide](../operate/troubleshoot.md).

## What Just Happened

The binary you just ran is not a wrapper around external services. It is the whole stack:

- **HTTP** serves the web UI and REST status endpoints
- **Symple** carries WebSocket signalling and peer presence
- **WebRTC** handles media session negotiation and RTP transport
- **TURN** relays media through NATs when direct paths fail
- **AV** captures, encodes, and packetizes H.264 video and Opus audio

Each of those is an independent icey library module. The server assembles them into one binary. If you want to understand how, read the [architecture](../concepts/architecture.md) page. If you want to use those modules in your own C++ application, start with the [HTTP Server recipe](../recipes/http-server.md).

## Three Modes

icey-server has three operational modes. The default is `stream`.

:::tabs

::tab{title="Stream"}
Server pushes a local file, camera, or RTSP source to the browser over WebRTC.

```bash
icey-server --mode stream --source /path/to/video.mp4
```

This is the default. If you ran the Docker command above, this is what you saw.
::

::tab{title="Record"}
Browser sends H.264 video to the server, which decodes and writes MP4 files to disk.

```bash
icey-server --mode record --record-dir ./recordings
```

Open the browser, grant camera access, and click **Call**. The server writes timestamped MP4 files under the recording directory.
::

::tab{title="Relay"}
First active browser caller becomes the live source. Later callers receive that source via server-side relay.

```bash
icey-server --mode relay
```

No server-side capture or encoding. The server forwards the first caller's encoded media to every subsequent viewer.
::

:::

Each mode is covered in detail on the [modes page](modes.md).

## Where To Go Next

:::card-group{cols="3"}

::card{title="Install" href="install"}
Docker, release binaries, Homebrew, APT, AUR, Nix, or build from source. Every install path in one place.
::

::card{title="Build With icey" href="../recipes/http-server"}
Use the library modules in your own C++ application. FetchContent, CMake targets, your first program.
::

::card{title="Operate" href="../operate/config"}
Config file reference, deployment guides, TLS, TURN, health endpoints, and troubleshooting.
::

:::
