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

perl -0pi -e 's/^pkgver=\d+\.\d+\.\d+$/pkgver='"$version"'/m' packaging/alpine/APKBUILD
perl -0pi -e 's/^(source="\$pkgname-\$pkgver\.tar\.gz::https:\/\/github\.com\/nilstate\/icey\/archive\/refs\/tags\/)\$pkgver(\.tar\.gz")$/${1}\$pkgver${2}/m' packaging/alpine/APKBUILD
perl -0pi -e 's/^[0-9a-f]{128}[[:space:]]+icey-\d+\.\d+\.\d+\.tar\.gz$/'"$ARCHIVE_SHA512"'  icey-'"$version"'.tar.gz/m' packaging/alpine/APKBUILD

echo "updated packaging/alpine/APKBUILD for $version"
echo "sha512: $ARCHIVE_SHA512"
