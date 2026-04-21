These are openSUSE / OBS package seeds for `llhttp` and `icey`.

Why two packages:
- `icey` can use system `llhttp`, but openSUSE does not currently provide it in the pool.
- `llhttp` therefore lands first, then `icey` can build against `pkgconfig(libllhttp)`.

Layout:
- `llhttp/`: OBS service, spec, and changelog for `llhttp 9.3.1`
- `icey/`: OBS service, spec, and changelog for `icey 2.4.5`

Current build policy:
- `icey` uses system dependencies
- FFmpeg stays enabled
- WebRTC stays disabled until `libdatachannel` is packaged for Factory
- `icey.spec` already sets `URL: https://0state.com/icey/` so the eventual
  package page backlinks to the project site

Current state as of 2026-04-21:
- `https://src.opensuse.org/0state/llhttp` and
  `https://src.opensuse.org/0state/icey` are live and current
- both repos now have pending transfer requests into the `c_cpp` org, which
  maps to OBS project `devel:libraries:c_c++`
- local `osc` is installed at `/home/kam/.venvs/osc/bin/osc`, but this machine
  still does not have working OBS basic-auth credentials for direct
  `api.opensuse.org` submitrequest work

Submission flow:
1. Keep the pending `0state/*` -> `c_cpp/*` transfer requests moving until the
   devel-project maintainers accept them.
2. Treat `c_cpp` / `devel:libraries:c_c++` as the canonical devel path.
3. Land `llhttp` first, because `icey` builds against `pkgconfig(libllhttp)`.
4. Land `icey` after `llhttp`, with WebRTC still disabled until
   `libdatachannel` exists in Factory.
5. Forward accepted devel-project packages to `openSUSE:Factory`.
