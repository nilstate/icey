# LibSourcey v2 - Modernisation Plan

> C++20, modern dependencies, production-ready quality.

**Status:** Ready for 2.0 release. All phases complete.

CMake 3.21 minimum, deps via FetchContent (libuv 1.50, llhttp 9.2.1, zlib 1.3.1, nlohmann/json 3.11.3, OpenSSL 3.x, FFmpeg 5+).

---

## Completed Work (2.0)

### Phase 1: Build System Overhaul - DONE

Target-based CMake with `scy_add_module()`, `install(EXPORT)`, CMakePresets.json, GNUInstallDirs. WebRTC removed. All dead cmake/vendor files deleted.

### Phase 2: Dependency Updates - DONE

- libuv 1.11 -> 1.50 (FetchContent)
- OpenSSL 1.x -> 3.x (full API migration: EVP, TLS_method, removed locking callbacks)
- http_parser -> llhttp 9.2.1
- nlohmann/json 2.1 -> 3.11.3
- zlib 1.2.8 -> 1.3.1
- FFmpeg API migrated to 5+/6+/7+ (send/receive, codecpar, iterator APIs)
- OpenCV FindModule updated for 4.x
- Dead deps removed: Poco, wxWidgets, rtaudio, dshow, msvc headers

### Phase 3: C++ Modernisation - DONE (2.0 scope)

- `#pragma once` on all headers
- `[[nodiscard]]`, `noexcept`, `enum class` throughout
- `typedef` -> `using`, `NULL` -> `nullptr`
- `std::string_view` (util, http/url), structured bindings, `constexpr` constants
- `std::filesystem` replaces custom path handling
- `std::chrono` replaces custom timing
- `std::shared_mutex` for read-heavy patterns (collection.h, signal.h)
- Raw `new`/`delete` -> smart pointers across all modules
- Signal `StopPropagation` exception replaced with `bool` return
- `IPacket::source` removed, typed `avptr` for AV pipeline

### Phase 4: Module-by-Module - DONE

#### base

Bugs fixed, asserts replaced with throws throughout (handle, stream, packetqueue, packetstream, datetime, timer, runner, thread, numeric, random, logger, streammanager, diagnosticmanager, application, util). Dead `#if 0` blocks removed. IPC timeout added. Signal slot ID uniqueness enforced. UV_EOF handled as graceful close. Tests: buffer, random, base64, datetime, error, stateful, util strings, platform, filesystem, thread, signal, timer pause/resume, timer one-shot, IPC round-trip, logger filtering.

#### crypto

Full OpenSSL 3.x migration. RAII wrappers for EVP_MD_CTX, EVP_CIPHER_CTX, X509. Asserts replaced. Tests: X509, HMAC, SHA-256/512, cipher edge cases, hash edge cases, hex, file checksum.

#### net

OpenSSL 3.x migration. ALPN + SNI support added. Hostname verification via `SSL_set1_host()` + `SSLSocket::connect(host, port)`. TLS 1.2 minimum enforced. All asserts replaced across socketemitter, sslsocket, socketadapter, packetsocket, udpsocket, address. Dead `#if 0` blocks removed (socketemitter.h, packetsocket.h/.cpp). Tests: IPv6, address ops, service resolution.

#### http

llhttp migration. WebSocket RFC 6455 compliance (RSV, opcode, mask, ping/pong/close). Raw `new`/`delete` -> `unique_ptr` throughout. Download progress tracking via `ClientConnection::IncomingProgress`. Dead `#if 0` removed (client.h, util.cpp). Tests: URL, cookie, authenticator, credentials, StringPart.

#### json, socketio, symple

Modernised, tests added for all.

#### av

Full FFmpeg 5+/6+/7+ migration (send/receive API, codecpar, iterator APIs, device enumeration). All asserts replaced across encoders, decoders, contexts, buffers, devicemanager, packet. Thread safety: mutex on PTS, atomics on MediaCapture flags, lock guards on create/close. Fixed MediaCapture loop seek bug. RAII via constructor/destructor pattern. Dead code removed (deprecated/ directory, `#if 0` blocks). Tests: Format, FormatRegistry, FPSCounter.

#### stun/turn

Fixed ChannelNumber factory bug, permission lifetime, bit shift UB, Server::stop() null. Tests added for both.

#### util -> merged into base

Chrono timing fix, UserManager deleted, all headers moved to `include/scy/`. Tests: Timeout, TimedToken, RateLimiter, IRegistry.

#### sched

Trigger ownership migrated to `unique_ptr`. Asserts replaced.

#### archo

`std::filesystem::path` migration. Path traversal protection in zip extraction. Fixed closeCurrentFile bug. Tests added.

#### pacm

SHA256 checksums (was MD5). SSL verification enabled. Path traversal protection. Download progress re-enabled. `latestSDKAsset` logic simplified. All asserts replaced. Dead code removed.

### Phase 5: Testing - DONE

Tests added for all 15 modules. Sanitizer builds (ASan, TSan, UBSan). Code coverage reporting. Samples cleaned.

### Phase 6: CI/CD & Quality - DONE

GitHub Actions (Linux GCC 13/14 + Clang 18, macOS, Windows MSVC 2022, sanitizers, coverage). Docker Ubuntu 24.04. `.clang-format` + `.clang-tidy`. Entire codebase formatted.

### Phase 7: Documentation & Packaging - DONE

README updated. Doxygen config. BUILD.md. vcpkg port. CPack. GitHub Releases with changelog. Version 2.0.0.

---

## Remaining for 2.0 (non-blocking)

### Test gaps - ALL FILLED

All tests passing. Standalone VideoEncoder, VideoDecoder, AudioDecoder tests added and verified with FFmpeg 6.1.

### Tests added for 2.0

- PacketStream overflow handling (fast producer stress test with bounded SyncPacketQueue)
- HTTP request/response serialization round-trip (GET, POST, headers, parse-back)
- WebSocket frame encoding/decoding (text, binary, client masking, control frames)
- pacm module tests (Package JSON round-trip, asset selection, LocalPackage state, manifest, errors, InstallationState)

---

## Post-2.0 Roadmap

Major features deferred to future releases.

### 2.1 - Networking

- Connection timeout support (read/write/connect)
- HTTP proxy (CONNECT, SOCKS5)
- HTTP connection pooling and keep-alive
- TLS 1.3 (0-RTT, session tickets, key update)
- Certificate pinning
- OCSP stapling
- ~~WebSocket fragmentation + partial frame buffering~~ DONE (2.0)
- ~~HTTP backpressure signalling~~ DONE (2.0, Stream write queue high water mark)

### 2.2 - AV

- Hardware acceleration (NVENC, QuickSync, VideoToolbox, VAAPI)
- HLS/DASH output
- Rate control modes (VBR, CRF, two-pass)
- Subtitle stream handling
- Public seek API
- Encoding statistics/metrics
- VideoPacket/AudioPacket ownership semantics

### 2.3 - C++ Modernisation

- `std::jthread` + `std::stop_token` replacing Thread/Runner
- Coroutine-based scheduling (C++20)
- `std::span` for pointer + size pairs
- `std::format` replacing ostringstream
- `concept` constraints replacing SFINAE
- Signal/slot delegate simplification
- GarbageCollector/deleteLater audit
- Error strategy (scy::Error vs std::error_code vs std::expected)
- Mutex deadlock audit
- `std::optional` for nullable returns
- MutableBuffer/ConstBuffer -> `std::span<std::byte>` evaluation
- spdlog evaluation

### 2.4 - Package Manager

- Package signature verification (RSA/ECDSA)
- Dependency resolution
- Rollback on failed install
- Mirror failover
- Incremental/delta updates
- Download resume
- Version range constraints
- Remote package caching
- CLI credential handling (not command-line args)

### 2.5 - Quality

- Fuzz testing (HTTP, STUN, JSON parsers)
- Benchmark suite (nanobench or Google Benchmark)
- clang-tidy modernize passes
- minizip-ng API update

---

## Module Decisions

| Module | Status | Decision |
|--------|--------|----------|
| base | Core | Keep |
| net | Core | Keep |
| crypto | Core | Keep |
| http | Core | Keep |
| json | Core | Keep |
| socketio | Active | Keep |
| symple | Active | Evaluate |
| av | Optional | Keep |
| archo | Optional | Keep |
| stun | Optional | Keep |
| turn | Optional | Keep |
| sched | Optional | Keep or extract |
| util | Merged | Into base |
| webrtc | Removed | Dropped |
| pacm | External | Keep |
| pluga | External | Keep |

---

## Architecture Notes

### Build System

`scy_add_module(name DEPENDS dep1 dep2 PACKAGES Pkg::Lib)` is the core abstraction. Auto-discovery via `subdirlist`, platform filtering, install/export generation. External module plugin via `src/.gitignore` whitelist.

### Custom GC & Deferred Deletion

`GarbageCollector`/`deleteLater` exists because libuv callbacks capture `void* data` - deferred deletion on the event loop thread prevents use-after-free. Post-2.0: evaluate whether modern libuv + smart pointers can replace this.
