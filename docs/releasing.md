# Releasing

This page is the operator guide for cutting an `icey` release.

Use it when you are preparing a new version, tagging it, and updating the package-manager artifacts that depend on the published GitHub source archive.

## What Changes On A Release

A real `icey` release has two parts:

1. manual release prose
2. mechanical version and archive metadata

The manual part stays in:

- [`CHANGELOG.md`](https://github.com/nilstate/icey/blob/main/CHANGELOG.md)
- [`ROADMAP.md`](https://github.com/nilstate/icey/blob/main/ROADMAP.md)

The mechanical part is handled by the top-level release helpers:

- `make release`
- `make release-check`
- `make release-pin`

## Core Release Flow

Start from a clean, reviewed `main` branch with the release notes already written.

```bash
make release VERSION=2.4.0
git commit -am "release: prepare 2.4.0"
git tag 2.4.0
git push origin main 2.4.0
make release-finalize VERSION=2.4.0
git commit -am "build: pin release archives for 2.4.0"
git push origin main
```

What those steps do:

- `make release` syncs `VERSION`, package recipe versions, Debian source metadata, Conan release metadata, and the public `FetchContent` examples.
- `make release-finalize` pins the release archive hashes and then verifies that `VERSION`, package metadata, docs examples, and the matching `CHANGELOG.md` heading all line up.
- `make release-check` validates the exact live archive hashes and sizes used by each repo-local package recipe, not just that hash fields are non-empty.
- the release tag must be a plain semantic version such as `2.4.0`
- the GitHub source archive for that tag is the thing the package-manager pinning steps use

## Package-Specific Follow-Up

After the tag is pushed, the provider-specific source archives are what get pinned. The release helpers now hash the exact URLs each package manager consumes.

The combined post-tag helper is:

```bash
make release-finalize VERSION=2.4.0
```

That runs:

- `make release-pin-conan VERSION=2.4.0`
- `make release-pin-vcpkg VERSION=2.4.0`
- `make release-pin-arch VERSION=2.4.0`
- `make release-pin-homebrew VERSION=2.4.0`
- `make release-pin-alpine VERSION=2.4.0`
- `make release-pin-macports VERSION=2.4.0`
- `make release-pin-spack VERSION=2.4.0`
- `make release-pin-conda VERSION=2.4.0`
- `make release-check VERSION=2.4.0`

## Conan

Local Conan packaging is version-synced by `make release`, and `make release-pin-conan` pins the post-tag archive hash used by the recipe.

Use it to verify the shipped recipe still builds:

```bash
make package-conan
```

Current scope:

- local recipe path: [`packaging/conan/conanfile.py`](../packaging/conan/conanfile.py)
- local source metadata: [`packaging/conan/conandata.yml`](../packaging/conan/conandata.yml)
- local consumer test: [`packaging/conan/test_package`](../packaging/conan/test_package)

Current non-scope:

- ConanCenter submission is not automated here yet
- if and when ConanCenter packaging is added, it will need its own source-download metadata and review path

## vcpkg

The `vcpkg` port in this repo is an overlay/custom-registry seed, not an upstream `microsoft/vcpkg` port checkout.

Release sequence:

```bash
make release VERSION=2.4.0
git tag 2.4.0
git push origin main 2.4.0
make release-pin-vcpkg VERSION=2.4.0
make package-vcpkg
```

What gets updated:

- [`packaging/vcpkg/icey/vcpkg.json`](../packaging/vcpkg/icey/vcpkg.json) version is synced by `make release`
- [`packaging/vcpkg/icey/portfile.cmake`](../packaging/vcpkg/icey/portfile.cmake) archive ref and SHA512 are pinned by `make release-pin-vcpkg`

## Arch / AUR

The Arch packaging in this repo is the AUR seed:

- [`packaging/arch/PKGBUILD`](../packaging/arch/PKGBUILD)
- [`packaging/arch/.SRCINFO`](../packaging/arch/.SRCINFO)

Release sequence:

```bash
make release VERSION=2.4.0
git tag 2.4.0
git push origin main 2.4.0
make release-pin-arch VERSION=2.4.0
make package-arch
```

What gets updated:

- `pkgver` and `pkgrel` are synced by `make release`
- the source URL is synced by `make release`
- the archive SHA256 is pinned by `make release-pin-arch`

For a real AUR publish, copy the final `PKGBUILD` and `.SRCINFO` into the AUR repo after the hash is pinned and the local `makepkg` run passes.

## Homebrew

The Homebrew packaging in this repo is a tap-local seed:

- [`packaging/homebrew/Formula/libdatachannel.rb`](../packaging/homebrew/Formula/libdatachannel.rb)
- [`packaging/homebrew/Formula/icey.rb`](../packaging/homebrew/Formula/icey.rb)

Release sequence:

```bash
make release VERSION=2.4.0
git tag 2.4.0
git push origin main 2.4.0
make release-pin-homebrew VERSION=2.4.0
brew install --formula ./packaging/homebrew/Formula/libdatachannel.rb
brew install --formula ./packaging/homebrew/Formula/icey.rb
```

What gets updated:

- `version`, source URL, and placeholder SHA are synced by `make release`
- the final release archive SHA256 is pinned by `make release-pin-homebrew`

The `icey` formula depends on the tap-local `libdatachannel` formula because Homebrew core does not currently ship that dependency.

## Debian / PPA

The Debian packaging in this repo is a source-package seed intended for Debian-style apt repositories and Launchpad PPAs:

- [`packaging/debian/debian/control`](../packaging/debian/debian/control)
- [`packaging/debian/debian/rules`](../packaging/debian/debian/rules)
- [`scripts/package-debian-source.sh`](../scripts/package-debian-source.sh)

Release sequence:

```bash
make release VERSION=2.4.0
make release-check VERSION=2.4.0
make package-debian-source
```

For a Launchpad target series, set the distribution explicitly:

```bash
DEBIAN_DISTRIBUTION=noble make package-debian-source
```

What gets updated:

- the Debian changelog version is synced by `make release`
- `make package-debian-source` stages a full source package under `build/package/debian/`
- that staging flow vendors the pinned source tarballs needed for a WebRTC-enabled build where distro packages do not exist yet

## Release Checklist

- update [`CHANGELOG.md`](https://github.com/nilstate/icey/blob/main/CHANGELOG.md) and [`ROADMAP.md`](https://github.com/nilstate/icey/blob/main/ROADMAP.md)
- run `make release VERSION=x.y.z`
- commit the pre-tag release metadata
- create and push the plain semantic-version tag
- run `make release-finalize VERSION=x.y.z`
- commit the pinned package-manager archive hashes and successful final check
- verify the package-manager entry points you intend to publish
- if you are targeting apt or Launchpad, run `make package-debian-source` after the version sync

## Related Pages

- [Installation](build/installation.md)
- [Contributing](contributing.md)
- [Repository README](https://github.com/nilstate/icey/blob/main/README.md)
