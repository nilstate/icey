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

perl -0pi -e 's#url = "https://github.com/nilstate/icey/archive/refs/tags/\d+\.\d+\.\d+\.tar\.gz"#url = "https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz"#' packaging/spack/package.py
perl -0pi -e 's/version\("\d+\.\d+\.\d+", sha256="[0-9a-f]+"\)/version("'"$version"'", sha256="'"$ARCHIVE_SHA256"'")/' packaging/spack/package.py

echo "updated packaging/spack/package.py for $version"
echo "sha256: $ARCHIVE_SHA256"
