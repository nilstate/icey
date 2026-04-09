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

perl -0pi -e 's/^pkgver=\d+\.\d+\.\d+$/pkgver='"$version"'/m' packaging/arch/PKGBUILD
perl -0pi -e "s#^sha256sums=\\('[0-9a-f]+'\\)\$#sha256sums=('${ARCHIVE_SHA256}')#m" packaging/arch/PKGBUILD
perl -0pi -e 's/^[[:space:]]*pkgver = \d+\.\d+\.\d+$/\tpkgver = '"$version"'/m' packaging/arch/.SRCINFO
perl -0pi -e 's#^[[:space:]]*source = [^[:space:]]+$#\tsource = icey-'"$version"'.tar.gz::https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz#m' packaging/arch/.SRCINFO
perl -0pi -e 's/^[[:space:]]*sha256sums = [0-9a-f]+$/\tsha256sums = '"$ARCHIVE_SHA256"'/m' packaging/arch/.SRCINFO

echo "updated packaging/arch/PKGBUILD for $version"
echo "sha256: $ARCHIVE_SHA256"
