# icey v2 Roadmap

> C++20, modern dependencies, production-ready quality.

CMake 3.21 minimum, deps via FetchContent (libuv 1.50, llhttp 9.2.1, zlib 1.3.1, nlohmann/json 3.11.3, OpenSSL 3.x, FFmpeg 5+).

---

## Current: 2.3.x Release Prep

- [x] Ship the WebRTC module, codec negotiation, canonical media path, and browser smoke coverage
- [x] Land benchmark targets, protocol fuzzers, and exported-package consumer validation
- [x] Finish the WebRTC/Symple runtime cutover:
  - private `webrtc_support` signaller layer
  - split `PeerSession` / `media-server` internals
  - split Symple client/server implementation units
  - WebRTC/Symple hot-path microbenchmarks
- [x] Move the docs toolchain onto published `moxygen` / `sourcey` npm packages and add API quality checks
- [x] Add repo-local Conan / vcpkg packaging layouts and Makefile entry points
- [x] Finish the lowercase `icey` package/docs cutover and clear the final sequential local package verification pass
- [ ] Tag and publish the 2.3.x git release with the current changelog
- [ ] Upstream the icey package recipes to ConanCenter and the vcpkg registry
- [x] Fold the Symple/WebRTC runtime refactor and current pacm/pluga deltas into the release prep notes before tagging

---

## Post-2.0 Roadmap

### 2.1 - WebRTC Integration (Completed)

Lightweight WebRTC media stack via libdatachannel now ships in-tree. See `src/webrtc/README.md`

- `src/webrtc/` module with `MediaBridge`, `PeerSession`, `CodecNegotiator`, and transport-agnostic signalling
- Private `src/webrtc/support/` layer for Symple/WebSocket signallers used by samples and `media-server`, without adding a core `webrtc -> symple` dependency
- libdatachannel via FetchContent (ICE, SRTP, data channels)
- Canonical send path is now capture/encode → RTP packetize → track sender, with browser-offer verification and committed Chromium/Firefox browser smoke coverage
- Samples and apps: webcam-streamer, file-streamer, media-recorder, data-echo, and the `media-server` relay/record flows
- Symple signalling and icey TURN integration are part of the supported path rather than placeholders

### 2.2 - Networking & Performance (Completed / Ongoing polish)

- HTTP connection pooling and keep-alive landed
- Benchmark targets and reportable microbenchmark runner landed
- HTTP comparative workload harness now lives under `src/http/perf/`, separate from reportable microbench targets under `src/http/bench/`
- Protocol fuzz targets for HTTP, WebSocket, STUN, and TURN landed
- Remaining networking work is now higher-end polish rather than base capability:
  - HTTP/2 (h2 over TLS, h2c upgrade, stream multiplexing, header compression, server push)
  - HTTP proxy (CONNECT, SOCKS5)
  - TLS 1.3 advanced features (0-RTT, session tickets, key update)
  - Certificate pinning
  - OCSP stapling

### 2.3 - Docs, Packaging, and Release Surface (Largely completed)

- Sourcey-based docs site, generated API reference, and docs quality checks are in place
- Conan and vcpkg repo-local packaging layouts are in place and pass the final sequential local verification path
- Symple/WebRTC support helpers and runtime internals have been extracted into reusable protocol/state/support layers instead of living inside samples and monolithic sources
- Reportable microbenchmarks now cover WebRTC sender/receiver dispatch plus Symple fanout and client parse/dispatch hot paths
- Remaining work is registry publication and release hygiene:
  - ConanCenter submission
  - vcpkg registry submission
  - git tags / release archives
  - release-note consolidation for recent browser/runtime/docs/submodule work

### 2.4 - AV

- Hardware acceleration (NVENC, QuickSync, VideoToolbox, VAAPI)
- HLS/DASH output
- Rate control modes (VBR, CRF, two-pass)
- Subtitle stream handling
- Public seek API
- Encoding statistics/metrics
- VideoPacket/AudioPacket ownership semantics

### 2.5 - Further C++ Modernisation

- `std::jthread` + `std::stop_token` replacing Thread/Runner
- Coroutine-based scheduling (C++20)
- `std::span` for pointer + size pairs
- `std::format` replacing ostringstream
- `concept` constraints replacing SFINAE
- Signal/slot delegate simplification
- GarbageCollector/deleteLater audit
- Error strategy (icy::Error vs std::error_code vs std::expected)
- `std::optional` for nullable returns
- MutableBuffer/ConstBuffer -> `std::span<std::byte>` evaluation

### 2.6 - Package Manager

- Package signature verification (RSA/ECDSA)
- Dependency resolution
- Rollback on failed install
- Mirror failover
- Incremental/delta updates
- Download resume
- Version range constraints
- Remote package caching
- CLI credential handling (not command-line args)

### 2.7 - Quality

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
| symple | Core | Native WebSocket signalling and room/routing layer |
| av | Active | FFmpeg 5+/6+/7+ |
| stun | Active | |
| turn | Active | Self-hosted relay |
| archo | Active | |
| sched | Active | |
| webrtc | Active | libdatachannel, see src/webrtc/README.md |
| pacm | External | Submodule |
| pluga | External | Submodule |

---

## Completed (2.0 - 2.3)

Build system overhaul (target-based CMake, FetchContent). Dependency updates (libuv 1.50, OpenSSL 3.x, llhttp, nlohmann/json 3.11.3, zlib 1.3.1, FFmpeg 5+). C++ modernisation (pragma once, nodiscard, enum class, string_view, filesystem, chrono, shared_mutex, smart pointers). All modules updated, tests for all 15 modules, CI (Linux/macOS/Windows, sanitizers, coverage), documentation, vcpkg port, WebRTC module, benchmark/fuzz coverage, and exported-package validation.

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

`icy_add_module(name DEPENDS dep1 dep2 PACKAGES Pkg::Lib)` is the core abstraction. Auto-discovery via `subdirlist`, platform filtering, install/export generation. External module plugin via `src/.gitignore` whitelist.

### Custom GC & Deferred Deletion

`GarbageCollector`/`deleteLater` exists because libuv callbacks capture `void* data` - deferred deletion on the event loop thread prevents use-after-free. Post-2.0: evaluate whether modern libuv + smart pointers can replace this.
