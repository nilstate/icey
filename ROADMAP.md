# Roadmap

C++20 media stack. CMake 3.21 minimum.

---

## Current: Package Manager Publication

- [ ] Upstream icey to ConanCenter
- [ ] Upstream icey to vcpkg registry
- [ ] Get NixOS/nixpkgs#504863 merged
- [ ] Launchpad binary package publication for PPA
- [ ] Submit Alpine aports PR
- [ ] Submit Fedora Bugzilla review
- [ ] File Debian RFP
- [ ] openSUSE OBS submission
- [ ] MacPorts, Spack, conda-forge upstream PRs

---

## Planned

### Real-Time Intelligence

The core product direction. icey becomes the self-hosted real-time media runtime that AI systems plug into.

#### Launch wedge: RTSP-to-browser with intelligence

- Frame sampling and async detection pipeline on `PacketStream`
- Detection, tracking, OCR, region redaction as explicit transform stages
- JSON event stream alongside live browser video
- Snapshot/clip extraction
- Provider adapters for local inference engines and cloud vision APIs
- Operator metrics: pipeline timing, queue depth, dropped frames, inference latency

#### Follow-on: real-time voice runtime

- Browser mic in over WebRTC
- VAD, turn-taking, interruption semantics
- ASR transcript stream, orchestration hook, TTS return path
- Self-hosted voice stack with transport timing and low-latency browser round-trip

#### Later convergence: multimodal media edge runtime

- Audio + video + browser UI + control + AI transforms
- Telepresence, inspections, kiosks, operations dashboards, edge copilots

### AV

- Hardware acceleration (NVENC, QuickSync, VideoToolbox, VAAPI)
- HLS/DASH output
- Rate control modes (VBR, CRF, two-pass)
- Subtitle stream handling
- Public seek API
- Encoding statistics/metrics

### Networking

- HTTP/2 (h2 over TLS, h2c upgrade, stream multiplexing, header compression, server push)
- HTTP proxy (CONNECT, SOCKS5)
- TLS 1.3 advanced features (0-RTT, session tickets, key update)
- Certificate pinning
- OCSP stapling

### C++ Modernisation

- `std::jthread` + `std::stop_token` replacing Thread/Runner
- Coroutine-based scheduling (C++20)
- `std::span` for pointer + size pairs
- `std::format` replacing ostringstream
- `concept` constraints replacing SFINAE
- Signal/slot delegate simplification
- Error strategy (icy::Error vs std::error_code vs std::expected)

### Package Manager (pacm)

- Package signature verification (RSA/ECDSA)
- Dependency resolution
- Rollback on failed install
- Mirror failover
- Incremental/delta updates

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
| av | Active | FFmpeg 5+/6+/7+/8+ |
| stun | Active | |
| turn | Active | Self-hosted relay |
| archo | Active | |
| sched | Active | |
| webrtc | Active | libdatachannel, browser smoke coverage |
| pacm | External | Submodule |
| graft | External | Submodule |
