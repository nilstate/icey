# LibSourcey v2 Roadmap

> C++20, modern dependencies, production-ready quality.

CMake 3.21 minimum, deps via FetchContent (libuv 1.50, llhttp 9.2.1, zlib 1.3.1, nlohmann/json 3.11.3, OpenSSL 3.x, FFmpeg 5+).

---

## Current: 2.0.1 Release

- [ ] Generate SocketIO API docs (api-socketio.md is empty)
- [ ] Review and update vcpkg port
- [ ] Tag 2.0.1 release with changelog

---

## Post-2.0 Roadmap

### 2.1 - WebRTC Integration (PRIORITY)

Lightweight WebRTC media stack via libdatachannel. Full plan: `WEBRTC_PLAN.md`

- New `src/webrtc/` module: MediaBridge, PeerSession, CodecNegotiator
- libdatachannel via FetchContent (libjuice ICE, libsrtp SRTP, usrsctp data channels)
- FFmpeg encode/decode <> libdatachannel RTP Track bridge
- Symple signalling for SDP/candidate exchange
- libsourcey TURN server as self-hosted relay infrastructure
- Samples: webcam-streamer, file-streamer, media-recorder, data-echo

### 2.2 - Networking

- Connection timeout support (read/write/connect)
- HTTP proxy (CONNECT, SOCKS5)
- HTTP connection pooling and keep-alive
- TLS 1.3 (0-RTT, session tickets, key update)
- Certificate pinning
- OCSP stapling

### 2.3 - AV

- Hardware acceleration (NVENC, QuickSync, VideoToolbox, VAAPI)
- HLS/DASH output
- Rate control modes (VBR, CRF, two-pass)
- Subtitle stream handling
- Public seek API
- Encoding statistics/metrics
- VideoPacket/AudioPacket ownership semantics

### 2.4 - Further C++ Modernisation

- `std::jthread` + `std::stop_token` replacing Thread/Runner
- Coroutine-based scheduling (C++20)
- `std::span` for pointer + size pairs
- `std::format` replacing ostringstream
- `concept` constraints replacing SFINAE
- Signal/slot delegate simplification
- GarbageCollector/deleteLater audit
- Error strategy (scy::Error vs std::error_code vs std::expected)
- `std::optional` for nullable returns
- MutableBuffer/ConstBuffer -> `std::span<std::byte>` evaluation

### 2.5 - Package Manager

- Package signature verification (RSA/ECDSA)
- Dependency resolution
- Rollback on failed install
- Mirror failover
- Incremental/delta updates
- Download resume
- Version range constraints
- Remote package caching
- CLI credential handling (not command-line args)

### 2.6 - Quality

- Fuzz testing (HTTP, STUN, JSON parsers)
- Benchmark suite (nanobench or Google Benchmark)
- clang-tidy modernize passes
- minizip-ng API update

---

## Modules

| Module | Status | Notes |
|--------|--------|-------|
| base | Core | |
| net | Core | |
| crypto | Core | |
| http | Core | |
| json | Core | |
| socketio | Active | |
| symple | Active | WebRTC signalling layer |
| av | Active | FFmpeg 5+/6+/7+ |
| stun | Active | |
| turn | Active | Self-hosted relay |
| archo | Active | |
| sched | Active | |
| webrtc | Planned | libdatachannel, see WEBRTC_PLAN.md |
| pacm | External | Submodule |
| pluga | External | Submodule |

---

## Completed (2.0)

Build system overhaul (target-based CMake, FetchContent). Dependency updates (libuv 1.50, OpenSSL 3.x, llhttp, nlohmann/json 3.11.3, zlib 1.3.1, FFmpeg 5+). C++ modernisation (pragma once, nodiscard, enum class, string_view, filesystem, chrono, shared_mutex, smart pointers). All modules updated, tests for all 15 modules, CI (Linux/macOS/Windows, sanitizers, coverage), documentation, vcpkg port.

### 2.0 C++ modernisation (completed)

- `std::string_view` for read-only params across all modules (~270 conversions)
- `Random` rewritten with `std::mt19937` + `std::random_device`
- `DateTime` internals rewritten with C++20 `std::chrono::year_month_day`, replacing Julian Day math
- `numeric` formatting rewritten with `std::to_chars`, replacing sprintf
- `icompare` consolidated from 10 template overloads to single `string_view` implementation
- POCO license blocks removed from all files
- Zero warnings, zero asserts in production code

---

## Architecture Notes

### Build System

`scy_add_module(name DEPENDS dep1 dep2 PACKAGES Pkg::Lib)` is the core abstraction. Auto-discovery via `subdirlist`, platform filtering, install/export generation. External module plugin via `src/.gitignore` whitelist.

### Custom GC & Deferred Deletion

`GarbageCollector`/`deleteLater` exists because libuv callbacks capture `void* data` - deferred deletion on the event loop thread prevents use-after-free. Post-2.0: evaluate whether modern libuv + smart pointers can replace this.
