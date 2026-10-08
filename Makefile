.PHONY: docs docs-install docs-xml docs-api-md docs-site docs-check docs-dev docs-docker clean-docs package-conan package-vcpkg package-arch package-homebrew package-debian-source package-nix package-rpm-srpm package-fedora-srpm package-alpine-apkbuild release release-check release-pin release-finalize

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
	node docs/scripts/sanitize-api-markdown.mjs

## Build Sourcey static site
docs-site: docs-install docs-xml
	$(DOCS_RUN) site:build

## Regenerate markdown, build the site, and validate overview quality
docs-check: docs-api-md
	$(DOCS_RUN) site:build
	node docs/scripts/check-api-quality.mjs
	$(DOCS_RUN) site:check

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

## Build the Conan recipe rendered from the released archive
package-conan:
	@set -e; render_root="$$(bash ./scripts/release-render.sh)"; \
	  $(CONAN) create "$$render_root/packaging/conan" --build=missing -s compiler.cppstd=20

## Install icey through the local vcpkg overlay port
package-vcpkg:
	@set -e; render_root="$$(bash ./scripts/release-render.sh)"; \
	  ICEY_VCPKG_SOURCE_PATH="$(ICEY_VCPKG_SOURCE_PATH)" VCPKG_MAX_CONCURRENCY="$(VCPKG_MAX_CONCURRENCY)" $(VCPKG) install icey --overlay-ports="$$render_root/packaging/vcpkg"

## Build the local Arch package from packaging/arch
package-arch:
	@set -e; render_root="$$(bash ./scripts/release-render.sh)"; \
	  cd "$$render_root/packaging/arch" && $(MAKEPKG) --force --cleanbuild --syncdeps

## Install the tap-local Homebrew formulae from packaging/homebrew
package-homebrew:
	@set -e; render_root="$$(bash ./scripts/release-render.sh)"; \
	  $(BREW) install --formula "$$render_root/packaging/homebrew/Formula/libdatachannel.rb" && \
	  $(BREW) install --formula "$$render_root/packaging/homebrew/Formula/icey.rb"

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
	@if [ -z "$(VERSION)" ]; then echo "usage: make release VERSION=<semver>" >&2; exit 1; fi
	./scripts/release-sync.sh "$(VERSION)"

## Verify release metadata is internally consistent
release-check:
	@if [ -n "$(VERSION)" ] && [ "$(VERSION)" != "$$(tr -d '[:space:]' < VERSION)" ]; then echo "VERSION does not match the requested release" >&2; exit 1; fi
	./scripts/release-publish-readiness.sh

## Pin release archive hashes for all package-manager recipes
release-pin: release-finalize

## After pushing a git tag, pin archive hashes and verify the release metadata
release-finalize:
	@if [ -n "$(VERSION)" ] && [ "$(VERSION)" != "$$(tr -d '[:space:]' < VERSION)" ]; then echo "VERSION does not match the requested release" >&2; exit 1; fi
	./scripts/release-render.sh
