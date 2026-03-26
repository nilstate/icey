.PHONY: docs docs-install docs-xml docs-api-md docs-site docs-check docs-dev docs-docker clean-docs package-conan package-vcpkg media-server-docker

DOCS_NPM = npm --prefix doc
DOCS_RUN = $(DOCS_NPM) run
CONAN ?= conan
VCPKG ?= vcpkg
VCPKG_MAX_CONCURRENCY ?= 1
ICEY_VCPKG_SOURCE_PATH ?= $(CURDIR)

## Build the Sourcey site from prose docs + Doxygen XML
docs: docs-site

## Install pinned docs toolchain
docs-install:
	$(DOCS_NPM) install

## Generate Doxygen XML only
docs-xml:
	mkdir -p build/doxygen
	doxygen Doxyfile

## Regenerate the optional doc/api markdown mirror from Doxygen XML
docs-api-md: docs-install docs-xml
	find doc/api -maxdepth 1 -type f -name '*.md' -delete
	$(DOCS_NPM) exec -- moxygen "$(CURDIR)/build/doxygen/xml" -g -o "$(CURDIR)/doc/api/%s.md" -n -a -l cpp -q --source-root "$(CURDIR)"

## Build Sourcey static site
docs-site: docs-install docs-xml
	$(DOCS_RUN) site:build

## Regenerate markdown, build the site, and validate overview quality
docs-check: docs-api-md
	$(DOCS_RUN) site:build
	node doc/scripts/check-api-quality.mjs

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

## Build and run the media-server Docker demo
media-server-docker:
	cd src/webrtc/apps/media-server/docker && docker compose up --build
