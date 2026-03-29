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
tmp_dir=$(mktemp -d)
archive_path="$tmp_dir/${version}.tar.gz"
trap 'rm -rf "$tmp_dir"' EXIT

curl --fail --location --retry 3 --retry-delay 1 --output "$archive_path" "$archive_url"

if command -v sha256sum >/dev/null 2>&1; then
    sha256=$(sha256sum "$archive_path" | awk '{print $1}')
elif command -v shasum >/dev/null 2>&1; then
    sha256=$(shasum -a 256 "$archive_path" | awk '{print $1}')
elif command -v openssl >/dev/null 2>&1; then
    sha256=$(openssl dgst -sha256 "$archive_path" | awk '{print $NF}')
else
    echo "need sha256sum, shasum, or openssl to compute the archive hash" >&2
    exit 1
fi

perl -0pi -e 's#url = "https://github.com/nilstate/icey/archive/refs/tags/\d+\.\d+\.\d+\.tar\.gz"#url = "https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz"#' packaging/spack/package.py
perl -0pi -e 's/version\("\d+\.\d+\.\d+", sha256="[0-9a-f]+"\)/version("'"$version"'", sha256="'"$sha256"'")/' packaging/spack/package.py

echo "updated packaging/spack/package.py for $version"
echo "sha256: $sha256"
