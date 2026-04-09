#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 1 ]; then
    echo "usage: $0 <version>" >&2
    exit 1
fi

version="$1"
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "expected plain semantic version like 2.4.0" >&2
    exit 1
fi

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

archive_url="https://github.com/nilstate/icey/archive/refs/tags/${version}.tar.gz"
eval "$("$repo_root"/scripts/release-archive-meta.sh "$archive_url")"

cat > packaging/conan/conandata.yml <<EOF
sources:
  "$version":
    url: "https://github.com/nilstate/icey/archive/refs/tags/$version.tar.gz"
    sha256: "$ARCHIVE_SHA256"
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
    sha256: "$ARCHIVE_SHA256"
EOF

echo "updated packaging/conan/conandata.yml for $version"
echo "updated packaging/conan-center-index/recipes/icey/all/conandata.yml for $version"
echo "sha256: $ARCHIVE_SHA256"
