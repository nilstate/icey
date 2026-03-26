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

if command -v sha512sum >/dev/null 2>&1; then
    sha512=$(sha512sum "$archive_path" | awk '{print $1}')
elif command -v shasum >/dev/null 2>&1; then
    sha512=$(shasum -a 512 "$archive_path" | awk '{print $1}')
elif command -v openssl >/dev/null 2>&1; then
    sha512=$(openssl dgst -sha512 "$archive_path" | awk '{print $NF}')
else
    echo "need sha512sum, shasum, or openssl to compute the archive hash" >&2
    exit 1
fi

perl -0pi -e 's/REF "\d+\.\d+\.\d+"/REF "'"$version"'"/' packaging/vcpkg/icey/portfile.cmake
perl -0pi -e 's/SHA512 [0-9a-f]+/SHA512 '"$sha512"'/i' packaging/vcpkg/icey/portfile.cmake
perl -0pi -e 's/HEAD_REF \w+/HEAD_REF main/' packaging/vcpkg/icey/portfile.cmake

echo "updated packaging/vcpkg/icey/portfile.cmake for $version"
echo "sha512: $sha512"
