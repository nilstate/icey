These are openSUSE / OBS package seeds for `llhttp` and `icey`.

Why two packages:
- `icey` can use system `llhttp`, but openSUSE does not currently provide it in the pool.
- `llhttp` therefore lands first, then `icey` can build against `pkgconfig(libllhttp)`.

Layout:
- `llhttp/`: OBS service, spec, and changelog for `llhttp 9.3.1`
- `icey/`: OBS service, spec, and changelog for `icey`

Current build policy:
- `icey` uses system dependencies
- FFmpeg stays enabled
- WebRTC stays disabled until `libdatachannel` is packaged for Factory
- `icey.spec` already sets `URL: https://0state.com/icey/` so the eventual
  package page backlinks to the project site

Source state:
- `https://src.opensuse.org/0state/llhttp` and
  `https://src.opensuse.org/0state/icey` carry the package sources.
- The `icey` spec and service revision come from the root `VERSION` file.
- A source repository is not an OBS or Factory binary listing; those
  submissions must be verified separately.

Submission flow:
1. Keep the pending `0state/*` -> `c_cpp/*` transfer requests moving until the
   devel-project maintainers accept them.
2. Treat `c_cpp` / `devel:libraries:c_c++` as the canonical devel path.
3. Land `llhttp` first, because `icey` builds against `pkgconfig(libllhttp)`.
4. Land `icey` after `llhttp`, with WebRTC still disabled until
   `libdatachannel` exists in Factory.
5. Forward accepted devel-project packages to `openSUSE:Factory`.
