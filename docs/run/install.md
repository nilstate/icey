# Install

Every path to a running `icey-server`. Pick the one that fits your environment.

:::tabs

::tab{title="Docker"}

The fastest path. No build tools, no system dependencies.

```bash
docker run --rm --network host 0state/icey-server:latest
```

Open `http://localhost:4500`.

For production, pin the version:

```bash
docker run --rm --network host 0state/icey-server:0.1.1
```

If you need to override the mode or source in Docker, use environment variables:

```bash
docker run --rm --network host \
  -e ICEY_SOURCE=rtsp://camera.local/stream1 \
  -e ICEY_LOOP=0 \
  0state/icey-server:0.1.1
```

For config-file-driven bring-up or direct `icey-server` flags, use a native `icey-server` install or the repo-backed `nilstate/icey-cli` source path.

::

::tab{title="Release Binary"}

Download a prebuilt binary from the [GitHub releases](https://github.com/nilstate/icey-cli/releases) page.

Linux x86_64:

```bash
curl -L https://github.com/nilstate/icey-cli/releases/latest/download/icey-server-Linux-x86_64.tar.gz \
  | tar xz
cd icey-server-*
./bin/icey-server --source /path/to/video.mp4
```

:::warning
The release binary requires OpenSSL and FFmpeg libraries on the host. If you get missing library errors, install them first or use the Docker path.
:::

::

::tab{title="Homebrew"}

```bash
brew tap nilstate/icey
brew install icey-server
icey-server --source /path/to/video.mp4
```

::

::tab{title="APT (Debian/Ubuntu)"}

```bash
curl -fsSL https://apt.0state.com/icey/icey-archive-keyring.gpg \
  | sudo tee /usr/share/keyrings/icey-archive-keyring.gpg >/dev/null
curl -fsSL https://apt.0state.com/icey/icey-server.list \
  | sudo tee /etc/apt/sources.list.d/icey-server.list >/dev/null
sudo apt update
sudo apt install icey-server
```

```bash
icey-server --source /path/to/video.mp4
```

::

::tab{title="AUR (Arch)"}

```bash
yay -S icey-server
icey-server --source /path/to/video.mp4
```

::

::tab{title="Nix"}

```bash
nix run github:nilstate/icey-cli -- --source /path/to/video.mp4
```

::

::tab{title="Build From Source"}

Build `icey-server` against a local icey checkout:

:::steps

1. Clone both repos

   ```bash
   git clone https://github.com/nilstate/icey.git
   git clone https://github.com/nilstate/icey-cli.git
   ```

2. Build the server

   ```bash
   cd icey-cli
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DICEY_SOURCE_DIR=../icey
   cmake --build build --target icey-server
   ```

3. Build the web UI

   ```bash
   cd web
   npm install
   npm run build
   cd ..
   ```

4. Run it

   ```bash
   ./build/src/server/icey-server --web-root web/dist --source /path/to/video.mp4
   ```

:::

Prerequisites: CMake 3.21+, a C++20 compiler, OpenSSL 3.x, FFmpeg 5+/6+/7+, Node.js (for the web UI build). libdatachannel is fetched automatically.

::

:::

## What You Get

Regardless of install method, you get a single `icey-server` binary. It serves the web UI, runs Symple signalling, handles WebRTC sessions, and optionally runs an embedded TURN relay. No external services required.

Release tarballs and Debian packages bundle the web UI under `share/icey-server/web`. `icey-server` will auto-discover that packaged path when `./web/dist` is not present, so the packaged binary can run without an extra `--web-root` flag.

Default ports:

| Service | Port |
| --- | --- |
| HTTP / WebSocket | 4500 |
| TURN relay | 3478 |

Both are configurable via CLI flags or config file. See the [CLI reference](cli.md) and [config guide](../operate/config.md).

## Library (For C++ Developers)

If you want to use icey as a library in your own C++ project rather than running the server, the [HTTP Server recipe](../recipes/http-server.md) is the shortest end-to-end example. It covers FetchContent, CMake targets, and linking individual modules. The repository [README](https://github.com/nilstate/icey#quick-start) has the full library quick-start.
