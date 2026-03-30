# Distribution Blowup Plan

> Get icey into every package manager that matters. Every registry page is a backlink to 0state.com/icey. Every mirror multiplies it.

This plan covers everything outside vcpkg and ConanCenter, which are tracked in [package-managers.md](package-managers.md).

---

## Current State

Packaging is no longer hypothetical. AUR and the dedicated Homebrew tap are live, the standalone Nix flake is validated in CI, and the remaining distro recipes now exist in-tree. Packaging CI now exercises release metadata, Debian source-package generation, RPM SRPM staging, and the repo-root flake build on every mainline push.

| Registry | File exists | Published | Blocker |
|---|---|---|---|
| vcpkg | yes | no | see package-managers.md |
| Conan | yes | no | see package-managers.md |
| Homebrew | yes (`packaging/homebrew/`) | yes (`nilstate/homebrew-icey`) | formula needs ongoing release automation from `icey` |
| Arch AUR | yes (`packaging/arch/`) | yes (`icey`) | keep pkgrel and checksum in sync with retags |
| Debian | yes (`packaging/debian/`) | no | Launchpad / sponsor step still external |
| Nix | yes (`flake.nix`, `packaging/nix/`) | yes (`nix build github:nilstate/icey`) | needs nixpkgs PR |
| Fedora/RPM | yes (`packaging/rpm/`) | no | needs SRPM / Bugzilla review |
| Alpine | yes (`packaging/alpine/`) | no | needs aports PR |
| MacPorts | yes (`packaging/macports/`) | no | needs ports submission |
| Spack | yes (`packaging/spack/`) | no | needs upstream PR |
| conda-forge | yes (`packaging/conda-forge/`) | no | needs staged-recipes feedstock |

---

## Pre-Publish Fixes

Before pushing deeper into curated distro repos, keep these source-package surfaces healthy:

1. **vcpkg portfile** — keep `REF` and `SHA512` pinned to the tagged release
2. **Homebrew formula** — keep `icey.rb` and the live tap formula in sync with the tagged tarball hash
3. **Conan test_package/build/** — keep local build artifacts ignored and out of commits

These are the integrity checks. Everything downstream depends on a clean tagged release with real hashes.

---

## Tier 1: Self-Service, No Gatekeepers

These can ship within days. No review process, no sponsorship needed.

### 1. Arch AUR

Keep the published `icey` package on `aur.archlinux.org` in lockstep with the tagged release.

- create an AUR account if needed
- `git clone ssh://aur@aur.archlinux.org/icey.git`
- copy PKGBUILD and .SRCINFO from `packaging/arch/`
- push

Repology auto-indexes AUR packages. This alone gets icey on repology.org.

### 2. Homebrew Tap

Maintain `nilstate/homebrew-icey` as the public tap.

- keep `icey.rb`, `libdatachannel.rb`, and `icey-server.rb` live in the tap
- users install via `brew tap nilstate/icey && brew install icey`
- keep `icey-server` publication in `icey-cli`; keep `icey` publication in `icey`

### 3. Ubuntu PPA (Launchpad)

Use the existing `packaging/debian/` files.

- create a PPA on launchpad.net under nilstate or 0state
- `debuild -S` and `dput` the source package
- target latest Ubuntu LTS + current release
- PPA pages show up on pkgs.org and launchpad search
- GitHub-side source-package build automation and PPA signing-key secrets are now in place; the remaining blocker is registering the public key with the Launchpad account and doing the first upload

### 4. Nix Flake for icey lib

icey-cli already has a flake. icey now also has a repo-root flake for the library.

- output a nix package with split `out` / `dev` outputs
- publish as a standalone flake: `nix build github:nilstate/icey`
- prerequisite for the nixpkgs PR in tier 2

---

## Tier 2: Curated Registry PRs

These require PRs and review, but massively expand reach.

### 5. Nixpkgs

Submit a PR to `github.com/NixOS/nixpkgs`.

- add `pkgs/development/libraries/icey/default.nix`
- include the nix flake from tier 1 as reference
- nixpkgs has 100K+ packages and active contributors
- once merged, appears on search.nixos.org and repology

### 6. Homebrew Core

After the tap proves stability, submit to homebrew-core.

- must meet [Acceptable Formulae](https://docs.brew.sh/Acceptable-Formulae) criteria
- needs notable usage (GitHub stars, dependents, or known adopters)
- homebrew-core formulas appear on formulae.brew.sh — high-traffic, indexed by search engines
- every bottle mirror (china mirrors, corporate caches) carries the metadata

### 7. vcpkg + ConanCenter

Tracked in [package-managers.md](package-managers.md). Highest priority C++ registries.

---

## Tier 3: Distro Repos

Landing in an official distro repo is the biggest backlink multiplier. Debian alone has 400+ mirrors worldwide.

### 8. Fedora

- write a .spec file (use `cmake/icey_package.cmake` as starting point)
- file a Bugzilla review request with SRPM
- once approved, package lands in Fedora and syncs to EPEL (CentOS/Rocky/RHEL)
- register on release-monitoring.org (Anitya) for automatic new-version tracking

### 9. Alpine

- write an APKBUILD
- submit PR to `gitlab.alpinelinux.org/alpine/aports`
- alpine is the default Docker base image — huge container audience
- alpine package search (pkgs.alpinelinux.org) is well-indexed

### 10. Debian Proper

- file an RFP (Request for Package) bug on wnpp
- find a Debian Developer to sponsor the upload
- once in Debian unstable, it syncs to testing → stable
- auto-syncs to Ubuntu universe, eliminating the need for the PPA long-term
- 400+ mirrors worldwide, each hosting package metadata with homepage URL
- appears on packages.debian.org, packages.ubuntu.com, tracker.debian.org

### 11. openSUSE

- submit to openSUSE Build Service (OBS)
- OBS can cross-build for Fedora, Debian, Ubuntu, Arch, and SUSE from one spec
- appears on software.opensuse.org package search

---

## Tier 4: Niche but High-Value

### 12. MacPorts

- write a Portfile
- submit to `github.com/macports/macports-ports`
- loyal macOS user base separate from Homebrew
- appears on ports.macports.org

### 13. Spack

- write a `package.py` recipe
- submit PR to `github.com/spack/spack`
- HPC and scientific computing audience
- appears on packages.spack.io

### 14. conda-forge

- create a feedstock via `conda-forge/staged-recipes`
- ML and scientific Python users pull C++ deps through conda
- appears on anaconda.org/conda-forge

---

## Backlink Mechanics

Every registry landing creates backlinks from multiple sources:

| Source | How | Volume |
|---|---|---|
| Registry page | homepage field → 0state.com/icey | 1 per registry |
| Repology | auto-aggregates once in 2+ repos | 1 page, updated continuously |
| pkgs.org | indexes Debian/Ubuntu/Fedora/Arch/Alpine | 1 per distro |
| Distro package search | packages.debian.org, packages.ubuntu.com, etc | 1 per distro |
| Mirror networks | Debian: 400+, Ubuntu: 300+, Fedora: 200+ | metadata on every mirror |
| Downstream dependents | any project with `depends: icey` | grows organically |
| Search engines | registry pages rank for "icey c++ install" etc | compounds over time |

### Metadata Consistency

Every packaging file must set the homepage to `https://0state.com/icey`. This is the backlink payload. Verify this in:

- `vcpkg.json` → `"homepage"`
- `conanfile.py` → `url` and `homepage`
- Homebrew formula → `homepage`
- PKGBUILD → `url`
- debian/control → `Homepage:`
- .spec → `URL:`
- APKBUILD → `url`
- nix derivation → `meta.homepage`
- Spack package.py → `homepage`
- conda-forge meta.yaml → `home:`
- MacPorts Portfile → `homepage`

---

## Execution Order

1. fix existing packaging files (SHA256, version pins, gitignore)
2. push AUR (immediate, unblocks repology)
3. create homebrew tap (immediate)
4. create Ubuntu PPA (immediate)
5. write nix flake for icey lib
6. submit vcpkg + conan PRs (see package-managers.md)
7. submit nixpkgs PR
8. write fedora .spec, submit review
9. write alpine APKBUILD, submit PR
10. file debian RFP
11. openSUSE OBS submission
12. macports, spack, conda-forge

---

## Definition of Done

- icey installable from at least 6 public registries
- 0state.com/icey as homepage in every package metadata file
- icey appears on repology.org with version parity across registries
- `brew install`, `apt install`, `pacman -S`, `nix build`, `vcpkg install`, `conan install` all work
