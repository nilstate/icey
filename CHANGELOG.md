# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/), and this project adheres to [Semantic Versioning](https://semver.org/).

## [2.2.0] - 2026-03-19

### Added

- Symple v4 server hardening: max connections, per-peer rate limiting (token bucket), max message size, graceful shutdown broadcast with error responses (413/429/503)
- Team/group permission scoping: rooms as permission boundaries, `rooms` field in auth message, `Authenticate` signal passes mutable `rooms` vector for server-side assignment
- Direct message permission check: sender and recipient must share at least one room
- Protocol version in welcome message (`"protocol": "symple/4"`) and room list in welcome response
- ASAN CMake support: `-fsanitize=address` propagates to FetchContent C dependencies via `CMAKE_C_FLAGS`
- `http::Server` overloads accepting `net::TCPSocket::Ptr` for TLS/SSL support alongside `uv::Loop*` constructors
- `smpl::Server` accepts `uv::Loop*` parameter, threads loop through to `http::Server`
- Timer destructor calls `stop()`, timer callback null-checks `req->data` (defense-in-depth)
- `Peer::operator=` with explicit `static_cast<const json::Value&>` (avoids nlohmann template overload)
- Symple module README with server, client, WebRTC signalling, and permissions documentation
- 4 hardening tests: max connections, max message size, rate limiting, graceful shutdown

### Fixed

- Mutex deadlock in `Responder::onPayload`: `connection().close()` for close/auth-reject was called inside the lock; moved to `shouldClose` flag pattern outside lock scope
- Use-after-free in `Server::onConnectionClose`: non-pooled connections were destroyed synchronously during their own callback chain; now deferred via idle callback (found by ASAN)
- `expect()` macro silently skipped side effects in `NDEBUG` mode: `waitFor()` calls inside `expect()` were never executed in Release builds

### Changed

- `http::Server` primary constructor takes `uv::Loop*` instead of `net::TCPSocket::Ptr`; loop is stored as `_loop` member, used in shutdown and deferred destruction
- SympleSignaller and all WebRTC samples updated from `smpl::TCPClient`/`SSLClient` to `smpl::Client`

### Removed

- `socketio` module deleted entirely (replaced by Symple v4 native WebSocket protocol)
- All `socketio` references removed from CMakeLists.txt across symple, webrtc samples

## [2.1.0] - 2026-03-19

### Added - WebRTC Module

- WebRTC module with [libdatachannel](https://github.com/nicertz/libdatachannel) integration (no Google libwebrtc dependency)
- Three-layer architecture: track factories (layer 1), per-track PacketStream adapters (layer 2), MediaBridge + PeerSession convenience wrappers (layer 3)
- Transport-agnostic `SignallingInterface` abstract base class; PeerSession no longer depends on Symple
- `SympleSignaller`: Symple call protocol implementation (call:init/accept/reject/offer/answer/candidate/hangup)
- Four WebRTC sample applications: data-echo, webcam-streamer, file-streamer, media-recorder
- `CodecNegotiator` maps RTP codecs to FFmpeg encoders at runtime
- CI job for WebRTC builds (Ubuntu 24.04, gcc-14, FFmpeg + OpenSSL + libdatachannel)

### Added - Symple v4

- Symple v4 protocol: native WebSocket transport replacing Socket.IO (breaking change from v3)
- C++ Symple server: auth, rooms, routing, presence, shutdown lifecycle
- C++ Symple client: plain WebSocket, auth flow, reconnection, close guard
- Protocol spec (`PROTOCOL.md`): connection lifecycle, message types, addressing, rooms, auth modes, scaling, v3-v4 migration
- 14 integration tests (address, peer, message, presence, roster, server lifecycle, connect, two-peer presence, message routing, auth failure, disconnect presence)
- `test::waitFor()` helper for event-driven async test patterns

### Added - HTTP Performance

- `ConnectionPool`: LIFO stack reuses ServerConnection objects across requests (configurable max size, default 128)
- `ServerConnection::reset()`: swap sockets without reconstruction (zero-alloc steady state)
- `DateCache`: formatted Date header updated once per second via timer
- HTTP/1.1 keep-alive: persistent connections, parser resets via `on_message_begin`, idle timeout (default 30s)
- `IntrusivePtr<T>` and `RefCounted<T>`: intrusive smart pointer with non-atomic embedded refcount (zero allocation overhead vs shared_ptr)
- Go net/http server added to benchmark suite for comparison
- Pre-formatted static status lines for common HTTP codes

### Added - Other

- `USE_SYSTEM_DEPS` CMake option for vcpkg/system package integration (switches FetchContent to find_package for libuv, llhttp, zlib, minizip)
- Vendored nlohmann/json single header (v3.11.3), eliminating FetchContent download
- vcpkg port with full dependency manifest and feature flags
- Symplestreamer sample: camera/file video streaming to symple-client-player via MJPEG over WebSocket
- HTTP benchmark suite and performance section in README
- `SO_REUSEPORT` support via `Server::setReusePort()` for multicore HTTP serving
- `NullSharedMutex` and `LocalSignal` for lock-free signal dispatch on single-threaded event loops
- `ICY_DATA_DIR` compile definition for test/sample data paths

### Changed

- `Handle::Context` changed from `shared_ptr` to `unique_ptr` (sole ownership, eliminates control block allocation per handle)
- `Handle::Context::~Context` clears `handle->data = nullptr` before `uv_close` to prevent use-after-free in pending callbacks
- `Stream::write` callback null-checks `handle->data` before accessing the Stream object
- `http::Server::shutdown()` defers connection destruction via `uv_idle` callback (prevents write callback use-after-free)
- `smpl::Server::shutdown()` uses `_shuttingDown` atomic flag to guard Responder callbacks during destruction
- `smpl::Client::close()` uses `_closing` guard to prevent re-entrant close
- Symple client no longer depends on socketio module; uses native WebSocket via HTTP module
- WebRTC module `DEPENDS` drops symple (signalling is transport-agnostic)
- `SKIP_EXPORT` option added to `icy_add_module` for FetchContent dependency conflicts
- README rewritten: WebRTC hero example, comparison table vs libWebRTC/libdatachannel/GStreamer/Pion, architecture diagram, module table
- Conan recipe added with per-module components and optional FFmpeg/OpenCV features
- `std::endl` replaced with `'\n'` throughout (removes redundant flush from logger macros)
- `std::bind` replaced with lambdas throughout
- Iterator loops converted to range-based for with structured bindings
- `typedef` converted to `using` aliases
- `override` added to virtual methods in derived classes
- `string_view`: converted ~270 read-only `const std::string&` parameters across base, http, crypto, net, symple, av, json, stun, turn modules
- `random`: rewrote PRNG with `std::mt19937` + `std::random_device`, replacing BSD nonlinear additive feedback LFSR
- `datetime`: rewrote internals with C++20 `std::chrono` calendar types (`year_month_day`, `sys_days`, `weekday`), replacing Julian Day floating-point math
- `numeric`: rewrote sprintf-based number formatting with `std::to_chars`, consolidated 50 functions into shared templates
- `util`: replaced 10 `icompare` template overloads (~140 lines) with single `string_view` implementation
- Renamed `json::value` to `json::Value` for codebase consistency
- HTTP server connections container changed from `vector` to `unordered_map` for O(1) lookup
- Single-pass response header assembly with pre-computed reserve
- All net/http signals switched to `LocalSignal`, eliminating mutex acquisition per emit
- `uv_write_t` request pooling via freelist in Stream (avoids heap alloc per write)
- Static status code string lookup in `Response::write`
- Cached peer address after connect (avoids repeated `uv_tcp_getpeername` calls)
- Modernised all sample applications
- Moved test data (`test.mp4`) to top-level `data/` directory shared across modules
- Refreshed README with contributors section, moved build/contributing docs to `doc/`

### Fixed

- `sendHeader()` undefined behavior: response buffer moved from local to member (local `std::string` was freed before async `uv_write` completed)
- `UV_EOF` in `Stream::handleRead()` now triggers graceful close instead of error state
- Symple v4 client `Peer` to json assignment needed `static_cast<const json::Value&>` to avoid nlohmann `type_error.302`
- `-Werror` enabled in CI; fixed all remaining compiler warnings
- `json::findNestedObjectWithProperty` was comparing `it.value()` instead of `it.key()`
- `Configuration::removeAll` iterator invalidation
- `LogStream` stub destructor for `ENABLE_LOGGING=OFF` builds
- `httpclient` sample `StatusCode` enum streaming

### Removed

- WebRTC hard dependency on Symple (signalling is now pluggable via `SignallingInterface`)
- Stale `bench/` directory (duplicate of `src/http/samples/httpbenchmark`)
- Dead commented-out JsonCpp stringify code in json module
- Empty `json.cpp` stub
- Deprecated `thread.h`/`thread.cpp` and `archo` test file
- In-source build artifacts

### Performance

Keep-alive benchmark results (single vCPU, wrk -t4 -c100 -d10s):

| Server | req/s | Latency |
| --- | --- | --- |
| Raw libuv+llhttp | 96,088 | 1.04ms |
| Icey | 72,209 | 1.43ms |
| Go 1.25 net/http | 53,878 | 2.31ms |
| Node.js v20 | 45,514 | 3.56ms |

## [2.0.1] - 2026-03-17

### Fixed

- macOS build: added missing `<unistd.h>` and `<cstdio>` includes in platform.cpp
- Windows build: added missing `<ostream>` include in error.h (MSVC string_view needs full definition)
- Windows build: replaced `SIGKILL` with `SIGTERM` in process.h (SIGKILL not defined on MSVC)
- Windows build: added `<csignal>` to application.cpp and process.h
- macOS linker: removed unnecessary `-ldl` from Apple config in Icey.cmake
- CI: fixed sanitizer env var syntax (split matrix.env into env_name/env_value)
- CI: removed ccache from coverage job (incompatible with gcov instrumentation)
- CI: added `submodules: recursive` to all checkout steps
- CI: added `BUILD_APPLICATIONS=OFF` to all jobs
- pacm: fixed apps/CMakeLists.txt referencing `pacmconsole` instead of `pacm-cli`
- Windows build: added missing `<windows.h>` include in filesystem.cpp
- Windows build: fixed winsock.h/winsock2.h include order conflict in x509certificate.cpp
- Windows build: added zlib include path for minizip in archo module
- Windows CI: fixed ctest syntax (`--config` -> `-C Release`)

### Changed

- Removed 7 dead `#if 0` code blocks (packetstream.cpp, cipher.cpp, turn tests, deprecated thread.h)
- Cleaned up 11 stale TODO/FIXME comments in samples and deprecated code

- Replaced all `assert()` calls in production code with exceptions or early returns (40+ occurrences)
- Removed all `_WIN32_WCE` (Windows CE) compatibility code
- Standardised Windows platform guard from `WIN32` to `_WIN32`
- Fixed `const` on value return types in buffer.h (12 compiler warnings)
- Added `[[fallthrough]]` annotations to base64.cpp switch cases (5 warnings)
- Added `override` to Thread::start() and AsyncLogWriter::run()
- Fixed Thread template constructor init order warning (two-phase init)
- Zero compiler warnings in project code (excluding third-party deps)

### Added

- Cross-platform process test (spawn echo, capture stdout, verify exit code)
- AV test data integrity verification: codec, sample rate, channel count, and duration checks via avformat
- SSL hostname verification integration test (connects to google.com with certificate verification enabled)

## [2.0.0] - Unreleased

### Build system

- Rewrote CMake build system with modern target-based dependencies and `icy_add_module()` pattern
- Replaced vendored sources with CMake FetchContent (libuv, zlib, llhttp, nlohmann/json)
- CMake 3.21 minimum, with proper `find_package(Icey)` and FetchContent consumer support
- CPack packaging support (deb, rpm, tar.gz)
- External module plugin pattern for out-of-tree modules
- Multi-stage Docker build on Ubuntu 24.04 LTS
- GitHub Actions CI replacing CircleCI (Linux GCC/Clang, macOS AppleClang, Windows MSVC)
- CI includes address/undefined sanitizers and code coverage
- `.clang-format` and `.clang-tidy` configurations added

### Language and code modernisation

- C++20 minimum standard (from C++14)
- `#pragma once` replaces include guards throughout
- `using` type aliases replace `typedef`
- `nullptr` replaces `NULL`
- `enum class` replaces unscoped enums
- Smart pointers (unique_ptr, shared_ptr) throughout; RAII wrappers for OpenSSL, FFmpeg, and logger resources
- `std::chrono` replaces `clock_t` timing
- `[[nodiscard]]` on key return values (crypto, net, av, archo modules)
- `noexcept` on all destructors
- `std::invoke_result` replaces deprecated `std::result_of`
- C-style casts replaced with `static_cast`/`reinterpret_cast`
- Entire codebase formatted with clang-format

### Dependencies

- OpenSSL 3.x migration (new provider/context APIs, deprecated function replacements)
- FFmpeg 6.x/7.x migration (avcodec send/receive API, codecpar, channel layouts, `av_*` iterators)
- llhttp replaces http_parser for HTTP/1.x parsing
- nlohmann/json replaces JsonCpp
- libuv 1.50 compatibility (accessor APIs, proper handle close, loop alive checks)
- FindOpenCV.cmake rewritten for OpenCV 4.x

### Security

- pacm: Default checksum algorithm changed from MD5 to SHA256
- pacm: SSL certificate verification enabled (was `initNoVerifyClient`)
- pacm: Path traversal protection added to zip extraction and package metadata
- pacm: Default API endpoint changed from http to https
- net: SSL hostname verification via `SSL_set1_host()`, auto-wired through `SSLSocket::connect(host, port)`
- net: TLS 1.2 enforced as minimum protocol version
- net: ALPN protocol negotiation support (`SSLContext::setALPNProtocols`)
- net: SNI support (`SSLContext::enableSNI`)

### Bug fixes

- STUN: ChannelNumber attribute factory was creating wrong type (ICEUseCandidate instead)
- STUN: Bit shift undefined behavior in UInt32/UInt64Attribute::setBit
- TURN: Permission lifetime corrected to RFC 5766-mandated 5 minutes (was 3)
- TURN: Server::stop() null pointer dereference when sockets not configured
- archo: ZipFile::closeCurrentFile() was calling unzOpenCurrentFile instead of unzCloseCurrentFile
- archo: Compressed/uncompressed file size fields were swapped
- base: once.h missing uv_close() on prepare handle (resource leak)
- base: UV_EOF in Stream::handleRead() now triggers graceful close instead of error state (fixes shutdown() causing spurious errors)
- base: IPC waitForSync() now has configurable timeout (default 5s) preventing infinite hangs
- base: Signal slot ID uniqueness enforced, throws on collision
- base: DateTime constructor/assign validate all components at runtime (was assert-only)
- base: PacketStream destructor warns instead of asserting on non-terminal state
- base: TaskRunner::add() fixed stray semicolon bug that allowed duplicate task insertion
- crypto: EVP_md5 replaced with EVP_sha256 for cipher key derivation
- av: MediaCapture::run() fixed stray semicolon on looping seek (was a no-op)
- av: MultiplexEncoder PTS tracking now mutex-protected across audio/video paths
- av: MediaCapture flags (_stopping, _looping, etc.) changed to std::atomic for thread safety
- av: FFmpeg 6.x const AVOutputFormat compatibility (removed oformat->codec assignment)
- av: Unchecked FFmpeg return values now checked: avformat_write_header, av_write_trailer, avio_close, avcodec_parameters_from_context
- http: Download progress tracking re-enabled via ClientConnection::IncomingProgress signal
- http: WebSocket RFC 6455 compliance: RSV bit validation, opcode validation, mask enforcement, ping/pong auto-response, close frame handshake
- http: WebSocket partial frame buffering across TCP segments
- http: WebSocket fragmentation/continuation frame support
- net: SSLContext min proto version was set before null check on context (crash fix)
- net: SSLAdapter SSL_ERROR_WANT_WRITE now calls flushWriteBIO instead of asserting
- pacm: latestSDKAsset() logic simplified (dead code branch removed)
- sched: Scheduler::update() and run() memory leaks fixed (erased tasks without deleting)

### Production safety

- All `assert()` calls in production source code replaced with runtime checks (throws, returns, or warnings) - approximately 110+ asserts across 30 files in base, net, av, crypto, pacm modules
- All stale `#if 0` dead code blocks removed across 13+ source files
- Stream::write() backpressure via configurable high water mark on libuv write queue (default 16MB)
- TaskRunner ownership modernised: `std::deque<std::unique_ptr<Task>>` replaces raw pointer container

### Module changes

- util module merged into base (headers moved, includes updated, downstream deps fixed)
- archo module updated to use `std::filesystem::path`
- Socket.IO protocol v5 compatibility verified
- FindFFmpeg.cmake rewritten to use pkg-config (old version depended on deleted CMake macros)
- pacm and pluga modules fully modernised (noexcept destructors, dead code removed, typos fixed)

### Tests

- base: Timer pause/resume, timer one-shot, IPC round-trip, logger level filtering, PacketStream overflow
- av: Standalone VideoEncoder (synthetic YUV -> libx264), VideoDecoder (test.mp4), AudioDecoder (test.mp4)
- http: Request/response serialisation round-trip, POST request, WebSocket text/binary frame encoding, client masking, control frames
- pacm: Package JSON round-trip, asset version selection, LocalPackage state management, manifest operations, error handling, InstallationState strings

### Removed

- WebRTC module (unmaintainable due to Google API churn)
- Deprecated code: `av/deprecated/`, `UserManager`, `SharedObject`
- Dead connection classes from http/client.h
- CircleCI configuration
- Gitbook documentation tooling
- Vendored libuv, zlib, http-parser sources
- Legacy Poco and Anionu dependencies

## [1.1.4] - 2019-03-15

- Last release of the 1.x series
