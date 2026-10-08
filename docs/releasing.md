# Releasing Icey

`VERSION` is the Icey library version. CMake, release recipes, documentation examples, and the Debian changelog derive from it. Icey Server is a separate product with its own `VERSION`; its `ICEY_VERSION` pins an exact published Icey tag.

## One library tag

1. Edit `VERSION` and write the matching, nonempty `CHANGELOG.md` section. Run `make release VERSION="$(tr -d '[:space:]' < VERSION)"` to sync derived metadata. Commit the result with a conventional subject and get it reviewed on `main`.
2. On that committed revision, run `bash scripts/release-publish-readiness.sh`. It must leave no release metadata diff. Run the relevant build and packaging checks before merging.
3. Fetch `main`, check that its `VERSION` and changelog are the reviewed release, then push one new plain semantic-version tag at that exact commit:

   ```bash
   git fetch origin main
   version="$(git show origin/main:VERSION | tr -d '[:space:]')"
   git tag "$version" origin/main
   git push origin "refs/tags/$version"
   ```

The tag triggers the GitHub release and verified Docker image, Homebrew and AUR formula publication, and a signed Debian source upload to the Launchpad PPA. The tag workflows derive and check archive hashes from the immutable source archive. There is no post-tag recipe commit or second tag push. Missing credentials fail the relevant publish job visibly.

Launchpad accepts a source upload before it builds and publishes binaries. Check the published `libicey2` and `libicey-dev` versions, not just the source upload. A Debian-only packaging correction can reuse the accepted original tarball with the next Debian revision through the manual `Publish Debian Source` workflow on `main`. That workflow copies only `packaging/debian/debian` from the current checkout into the revision.

## Package registries

The repository's `packaging/` files are release recipes, not evidence that an external registry has updated. Homebrew, AUR, Docker Hub, GitHub Releases, and the PPA are owned channels. vcpkg, ConanCenter, MacPorts, Spack, nixpkgs, conda-forge, Alpine, Fedora, openSUSE, and xrepo require their own review or publication. Submit updates from the tagged archive's exact checksum and read back the registry after its maintainers merge them. The independently versioned Rust crates should only be published when their own bytes and dependency contract change; publish `icey-sys` before `icey`.

Icey Server is released only after its pinned Icey tag is available. Push one `v`-prefixed server tag at the reviewed server commit. Its tag workflow builds Linux assets once, uses the built binary and web files for the Docker image, and publishes Homebrew, AUR, and the signed APT repository. Check release assets, the image digest and health endpoint, formula versions, the AUR entry, and the APT `InRelease` signature and `Packages` entry. Windows package templates require a verified Windows archive before winget, Scoop, or Chocolatey publication.

Use the server's `scripts/check-distribution.py --strict` for owned-channel drift and its JSON output for the full registry inventory. A failed upstream review or pending Launchpad build is not a completed listing. Content and documentation updates follow their incremental publish path and do not need a runtime image.

## Related pages

- [Install](run/install.md)
- [Contributing](contributing.md)
- [Repository README](https://github.com/nilstate/icey/blob/main/README.md)
