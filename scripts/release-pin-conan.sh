#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 1 ]; then
    echo "usage: $0 <version>" >&2
    exit 1
fi

version="$1"
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "expected plain semantic version in MAJOR.MINOR.PATCH format" >&2
    exit 1
fi

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "${RELEASE_OUTPUT_ROOT:-$repo_root}"

if [[ -n "${RELEASE_MANIFEST_FILE:-}" ]]; then
    source "$RELEASE_MANIFEST_FILE"
else
    manifest="$(
        RELEASE_REQUIRE_REMOTE_TAG=1 \
        RELEASE_FETCH_ARCHIVE_META=1 \
        bash "$repo_root"/scripts/release-manifest.sh "$version"
    )"
    eval "$manifest"
fi
[[ "$RELEASE_VERSION" == "$version" ]] || { echo "release manifest version mismatch" >&2; exit 1; }

cat > packaging/conan/conandata.yml <<EOF
sources:
  "$version":
    url: "https://github.com/nilstate/icey/archive/refs/tags/$version.tar.gz"
    sha256: "$RELEASE_ARCHIVE_SHA256"
EOF

cat > packaging/conan-center-index/recipes/icey/config.yml <<EOF
versions:
  "$version":
    folder: all
EOF

cat > packaging/conan-center-index/recipes/icey/all/conandata.yml <<EOF
sources:
  "$version":
    url: "https://github.com/nilstate/icey/archive/refs/tags/$version.tar.gz"
    sha256: "$RELEASE_ARCHIVE_SHA256"
EOF

echo "updated packaging/conan/conandata.yml for $version"
echo "updated packaging/conan-center-index/recipes/icey/all/conandata.yml for $version"
echo "sha256: $RELEASE_ARCHIVE_SHA256"
