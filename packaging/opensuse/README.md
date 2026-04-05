These are openSUSE / OBS package seeds for `llhttp` and `icey`.

Why two packages:
- `icey` can use system `llhttp`, but openSUSE does not currently provide it in the pool.
- `llhttp` therefore lands first, then `icey` can build against `pkgconfig(libllhttp)`.

Layout:
- `llhttp/`: OBS service, spec, and changelog for `llhttp 9.3.1`
- `icey/`: OBS service, spec, and changelog for `icey 2.4.2`

Current build policy:
- `icey` uses system dependencies
- FFmpeg stays enabled
- WebRTC stays disabled until `libdatachannel` is packaged for Factory

Submission flow:
1. Copy each package directory into its own `src.opensuse.org` package repo.
2. Run the OBS source services there.
3. Push the branches and open the corresponding review / submit requests.
