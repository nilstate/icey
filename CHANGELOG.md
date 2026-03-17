# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/), and this project adheres to [Semantic Versioning](https://semver.org/).

## [2.0.1] - Unreleased

### Fixed

- macOS build: added missing `<unistd.h>` and `<cstdio>` includes in platform.cpp
- Windows build: added missing `<ostream>` include in error.h (MSVC string_view needs full definition)
- Windows build: replaced `SIGKILL` with `SIGTERM` in process.h (SIGKILL not defined on MSVC)
- Windows build: added `<csignal>` to application.cpp and process.h
- macOS linker: removed unnecessary `-ldl` from Apple config in LibSourcey.cmake
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

## [2.0.0] - Unreleased

### Build system

- Rewrote CMake build system with modern target-based dependencies and `scy_add_module()` pattern
- Replaced vendored sources with CMake FetchContent (libuv, zlib, llhttp, nlohmann/json)
- CMake 3.21 minimum, with proper `find_package(LibSourcey)` and FetchContent consumer support
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
