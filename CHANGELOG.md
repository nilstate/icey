# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/), and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [2.4.0] - 2026-03-27

### Added

- Repo-local package manager layouts under `packaging/` with `make package-conan` and `make package-vcpkg` entry points for local consumer validation
- Homebrew tap formulae, Debian / PPA source-package scaffolding, and release hooks for those package surfaces
- Browser smoke coverage for the `src/webrtc/apps/media-server/web/` frontend and media-server interop path
- Reportable microbenchmark runner plus focused parse/probe benches for packet stream, HTTP, WebSocket, and STUN hot paths
- Focused `webrtcbench` and `symplebench` targets covering:
  - WebRTC sender dispatch
  - WebRTC receiver dispatch
  - Symple room fanout
  - Symple client parse/dispatch
- API overview quality checks in the docs toolchain to catch shallow or missing generated reference summaries
- Internal Symple protocol/state helpers for welcome parsing, sanitized presence emission, room reconciliation, and roster presence application, backed by focused unit coverage
- `webrtc_support` as a dedicated support library for Symple/WebSocket signallers, including the server-side virtual-peer adapter now used by samples and `media-server`

### Changed

- WebRTC browser/media-server behavior after 2.3.0: codec negotiation is tighter, browser offer handling is stricter, and `PeerSession` / track flow is more defensive under real browser traffic
- Public package, docs, and sample surface now use lowercase `icey` consistently, with no retained `Icey` CMake/package-manager naming
- Public lifecycle verbs are now standardized on `start()` / `stop()` across HTTP client/server, Symple client/server, and TURN client surfaces; the previous spellings were removed instead of being kept as compatibility aliases
- WebRTC internals are now cut along explicit seams instead of monolithic translation units:
  - `PeerSession` state/signalling/RTC work is split across `peersession.cpp`, `peersessionsignalling.cpp`, and `peersessionrtc.cpp`
  - remote answer scoping lives in `remotemediaplan.cpp`
  - codec support is centralized in `codecregistry.cpp`
  - `media-server` runtime code is split under `apps/media-server/internal/`
- Public Symple and TURN configuration handoff now favors value-style option snapshots instead of live mutable option bags
- Public option mutation is now explicit on package-manager surfaces via `mutableOptions()`, while the public AV encoder interfaces expose const option views
- `PeerSession::Config` now exposes media settings under `config.media`, and answer-session media options are derived explicitly from the remote offer's mids, directions, and payload types instead of relying on looser implicit defaults
- Symple client/server internals are now organized under `src/symple/src/client/` and `src/symple/src/server/`, with shared wire helpers in `src/symple/src/protocol.*`
- API reference coverage has been deepened across the core modules, WebRTC, and the pacm/pluga surfaces, with reorganized Sourcey guides and workflows
- icey docs now consume published `moxygen` / `sourcey` npm releases instead of git-pinned or exact-pinned toolchain versions
- Packaging assets are centralized under `packaging/` instead of spreading Conan and vcpkg files across the repo root
- Installed `icey.pc` metadata now uses the current package-manager-facing description instead of the old networking-only tagline
- Browser support claims are now explicit:
  - Chromium and Firefox are validated by the committed Playwright smoke
  - Playwright WebKit on Linux is treated as non-authoritative for Safari/WebRTC publish-path claims

### Fixed

- Exported CMake package consumers now tolerate dependency target-name differences across FetchContent, Conan, and vcpkg installs
- Repo-local Conan and vcpkg packaging validation now passes sequentially, and installed `icey.h` headers no longer leak absolute build/source paths
- Browser media-server smoke harness failures and related WebRTC sample pipeline regressions that were blocking stricter interop coverage
- Symple-backed WebRTC signalling now enforces full `user|id` peer identity on the public call boundary instead of accepting ambiguous bare user/session forms
- WebRTC receive-side codec detection no longer stringifies track descriptions back into SDP just to rediscover codec/clock data on bind or record paths
- Symple clients now transition cleanly to `Error` on underlying transport failures, including initial WebSocket connect failures, via explicit `http::ClientConnection` error propagation
- pacm JSON handling in the submodule update path, keeping the external package-manager surface aligned with the current docs/API snapshots

## [2.3.0] - 2026-03-23

### Added

- Dedicated protocol fuzz targets under `BUILD_FUZZERS` for the HTTP parser, WebSocket frame parser, STUN message parser, and TURN indication/request parsing
- Build-tree and install-tree consumer validation for the exported CMake package surface
- Dedicated microbenchmark targets under `BUILD_BENCHMARKS`: `signalbench`, `httpbench`, `httpparsebench`, and `wsbench`, plus the separate `BUILD_PERF` `httpperf`/`httpperf_compare` wrk/Node.js/Go comparison harness
- WebRTC loopback/media regression coverage for:
  - data-channel roundtrip
  - encoded H.264 roundtrip
  - encoded Opus roundtrip
  - relay audio fanout
  - relay source handoff
- `media-server` `record` and `relay` modes are now real shipped behaviors rather than placeholder sample code

### Changed

- HTTP benchmark assets moved out of `src/http/samples/httpbenchmark/` into `src/http/perf/`, while `src/http/bench/` now stays microbench-only and `src/http/samples/` contains samples only
- `PacketStream` queue retention and ownership semantics are now explicit at queue boundaries, with deterministic overload/drop accounting and synchronized multi-source passthrough
- `PeerSession` now uses explicit call phases and stricter signaling ordering, and WebRTC track setup requires explicit codec selection instead of fallback codec guessing
- The canonical WebRTC send path used by the samples and `media-server` is now encode -> RTP packetize -> sender, with stable sender bindings across call lifecycles

### Fixed

- Core async lifetime handling in `base` / `net` / `http`, including retained handle/request context, state-driven HTTP server lifecycle, and typed WebSocket parse errors
- STUN/TURN protocol correctness and relay hot paths:
  - correct STUN length/integrity patching
  - correct plain vs XOR address handling
  - IPv6 address attribute wire sizing
  - binary/numeric TURN permission checks and local-IP policy
- Symple routing/state correctness:
  - presence canonicalization
  - room-based authz checks
  - reconnect room restoration
  - virtual-peer copy semantics
- `media-server` relay/signaling behavior:
  - remote ICE candidates queue until remote SDP is installed
  - relay source promotion/handoff works correctly
  - stable media sender lifetime across attach/detach
- Package consumer discovery:
  - exported targets work correctly from both the build tree and install tree
  - `find_package(icey REQUIRED COMPONENTS ...)` correctly marks built components as found

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
- Symplestreamer sample: camera/file video streaming to symple-player via MJPEG over WebSocket
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
- Refreshed README with contributors section, moved build/contributing docs to `docs/`

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
| icey | 72,209 | 1.43ms |
| Go 1.25 net/http | 53,878 | 2.31ms |
| Node.js v20 | 45,514 | 3.56ms |

## [2.0.1] - 2026-03-17

### Fixed

- macOS build: added missing `<unistd.h>` and `<cstdio>` includes in platform.cpp
- Windows build: added missing `<ostream>` include in error.h (MSVC string_view needs full definition)
- Windows build: replaced `SIGKILL` with `SIGTERM` in process.h (SIGKILL not defined on MSVC)
- Windows build: added `<csignal>` to application.cpp and process.h
- macOS linker: removed unnecessary `-ldl` from Apple config in icey.cmake
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
- CMake 3.21 minimum, with proper `find_package(icey)` and FetchContent consumer support
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
