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

size=$(wc -c < "$archive_path" | tr -d '[:space:]')
if command -v openssl >/dev/null 2>&1; then
    rmd160=$(openssl dgst -rmd160 "$archive_path" | awk '{print $NF}')
else
    echo "need openssl to compute the MacPorts rmd160 checksum" >&2
    exit 1
fi

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

perl -0pi -e 's/github.setup\s+nilstate\s+icey\s+\d+\.\d+\.\d+/github.setup        nilstate icey '"$version"'/g' packaging/macports/Portfile
perl -0pi -e 's/^([[:space:]]*rmd160[[:space:]]+)[0-9a-f]{40}( \\\\)$/\1'"$rmd160"'\2/m' packaging/macports/Portfile
perl -0pi -e 's/^([[:space:]]*sha256[[:space:]]+)[0-9a-f]{64}( \\\\)$/\1'"$sha256"'\2/m' packaging/macports/Portfile
perl -0pi -e 's/^([[:space:]]*size[[:space:]]+)\d+$/\1'"$size"'/m' packaging/macports/Portfile

echo "updated packaging/macports/Portfile for $version"
echo "rmd160: $rmd160"
echo "sha256: $sha256"
echo "size: $size"
