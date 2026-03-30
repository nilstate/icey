.PHONY: docs docs-install docs-xml docs-api-md docs-site docs-check docs-dev docs-docker clean-docs package-conan package-vcpkg package-arch package-homebrew package-debian-source package-nix package-rpm-srpm package-fedora-srpm package-alpine-apkbuild release release-check release-pin release-pin-vcpkg release-pin-arch release-pin-homebrew release-pin-alpine release-pin-macports release-pin-spack release-pin-conda

DOCS_NPM = npm --prefix docs
DOCS_RUN = $(DOCS_NPM) run
CONAN ?= conan
VCPKG ?= vcpkg
MAKEPKG ?= makepkg
BREW ?= brew
DPKG_BUILDPACKAGE ?= dpkg-buildpackage
NIX ?= nix
NIX_BUILD_FLAGS ?=
RPMBUILD ?= rpmbuild
VCPKG_MAX_CONCURRENCY ?= 1
ICEY_VCPKG_SOURCE_PATH ?= $(CURDIR)
ICEY_DEBIAN_STAGE_DIR ?= $(CURDIR)/build/package/debian
ICEY_RPM_STAGE_DIR ?= $(CURDIR)/build/package/rpm

## Build the Sourcey site from prose docs + Doxygen XML
docs: docs-site

## Install pinned docs toolchain
docs-install:
	$(DOCS_NPM) install

## Generate Doxygen XML only
docs-xml:
	mkdir -p build/doxygen
	doxygen Doxyfile

## Regenerate the optional docs/api markdown mirror from Doxygen XML
docs-api-md: docs-install docs-xml
	find docs/api -maxdepth 1 -type f -name '*.md' -delete
	$(DOCS_NPM) exec -- moxygen "$(CURDIR)/build/doxygen/xml" -g -o "$(CURDIR)/docs/api/%s.md" -n -a -l cpp -q --source-root "$(CURDIR)"

## Build Sourcey static site
docs-site: docs-install docs-xml
	$(DOCS_RUN) site:build

## Regenerate markdown, build the site, and validate overview quality
docs-check: docs-api-md
	$(DOCS_RUN) site:build
	node docs/scripts/check-api-quality.mjs

## Dev server with live reload
docs-dev: docs-install docs-xml
	$(DOCS_RUN) site:dev

## Rebuild docs inside the dedicated docs container
docs-docker:
	docker build -f Dockerfile.docs -t icey-docs .
	docker run --rm -u "$$(id -u):$$(id -g)" -v "$$(pwd):/workspace" -w /workspace icey-docs make docs

## Clean generated docs artifacts
clean-docs:
	rm -rf build/doxygen dist

## Build the local Conan package from packaging/conan
package-conan:
	$(CONAN) create packaging/conan --build=missing -s compiler.cppstd=20

## Install icey through the local vcpkg overlay port
package-vcpkg:
	ICEY_VCPKG_SOURCE_PATH="$(ICEY_VCPKG_SOURCE_PATH)" VCPKG_MAX_CONCURRENCY="$(VCPKG_MAX_CONCURRENCY)" $(VCPKG) install icey --overlay-ports="$(CURDIR)/packaging/vcpkg"

## Build the local Arch package from packaging/arch
package-arch:
	cd packaging/arch && $(MAKEPKG) --force --cleanbuild --syncdeps

## Install the tap-local Homebrew formulae from packaging/homebrew
package-homebrew:
	$(BREW) install --formula ./packaging/homebrew/Formula/libdatachannel.rb
	$(BREW) install --formula ./packaging/homebrew/Formula/icey.rb

## Build a Debian source package / PPA seed under build/package/debian
package-debian-source:
	ICEY_DEBIAN_STAGE_DIR="$(ICEY_DEBIAN_STAGE_DIR)" DPKG_BUILDPACKAGE="$(DPKG_BUILDPACKAGE)" ./scripts/package-debian-source.sh

## Build the repo-root Nix flake package
package-nix:
	$(NIX) build $(NIX_BUILD_FLAGS) .#icey

## Build an SRPM staging tree under build/package/rpm
package-rpm-srpm:
	ICEY_RPM_STAGE_DIR="$(ICEY_RPM_STAGE_DIR)" RPMBUILD="$(RPMBUILD)" ./scripts/package-rpm-srpm.sh

## Validate the RPM SRPM flow inside a Fedora container
package-fedora-srpm:
	CMAKE_BUILD_PARALLEL_LEVEL="$(CMAKE_BUILD_PARALLEL_LEVEL)" ./scripts/package-fedora-srpm.sh

## Validate the Alpine APKBUILD inside an Alpine container
package-alpine-apkbuild:
	CMAKE_BUILD_PARALLEL_LEVEL="$(CMAKE_BUILD_PARALLEL_LEVEL)" ./scripts/package-alpine-apkbuild.sh

## Sync release metadata for VERSION, package recipes, and FetchContent examples
release:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-sync.sh "$(VERSION)"

## Verify release metadata is internally consistent
release-check:
	@if [ -n "$(VERSION)" ]; then ./scripts/release-check.sh "$(VERSION)"; else ./scripts/release-check.sh; fi

## Pin release archive hashes for all package-manager recipes
release-pin: release-pin-vcpkg release-pin-arch release-pin-homebrew release-pin-alpine release-pin-macports release-pin-spack release-pin-conda

## After pushing a git tag, pin the vcpkg fallback archive ref and sha512
release-pin-vcpkg:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-vcpkg VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-vcpkg.sh "$(VERSION)"

## After pushing a git tag, pin the Arch release archive sha256 and SRCINFO
release-pin-arch:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-arch VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-arch.sh "$(VERSION)"

## After pushing a git tag, pin the Homebrew formula source sha256
release-pin-homebrew:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-homebrew VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-homebrew.sh "$(VERSION)"

## After pushing a git tag, pin the Alpine archive sha512
release-pin-alpine:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-alpine VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-alpine.sh "$(VERSION)"

## After pushing a git tag, pin the MacPorts archive checksums and size
release-pin-macports:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-macports VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-macports.sh "$(VERSION)"

## After pushing a git tag, pin the Spack recipe version sha256
release-pin-spack:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-spack VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-spack.sh "$(VERSION)"

## After pushing a git tag, pin the conda-forge recipe sha256
release-pin-conda:
	@if [ -z "$(VERSION)" ]; then echo "usage: make release-pin-conda VERSION=2.4.0" >&2; exit 1; fi
	./scripts/release-pin-conda.sh "$(VERSION)"
