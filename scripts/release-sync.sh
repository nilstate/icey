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

if ! grep -Eq "^## \\[$version\\]" CHANGELOG.md; then
    echo "CHANGELOG.md is missing a section for [$version]" >&2
    exit 1
fi

docs=(
    README.md
    docs/getting-started.md
    docs/installation.md
    docs/modules/av.md
    docs/modules/base.md
    docs/modules/http.md
    docs/modules/net.md
    docs/modules/webrtc.md
    llms.txt
)

printf '%s\n' "$version" > VERSION

perl -0pi -e 's/version = "\d+\.\d+\.\d+"/version = "'"$version"'"/' packaging/conan/conanfile.py
perl -0pi -e 's/"version": "\d+\.\d+\.\d+"/"version": "'"$version"'"/' packaging/vcpkg/icey/vcpkg.json
perl -0pi -e 's/^pkgver=\d+\.\d+\.\d+$/pkgver='"$version"'/m' packaging/arch/PKGBUILD
perl -0pi -e 's/^pkgrel=\d+$/pkgrel=1/m' packaging/arch/PKGBUILD
perl -0pi -e "s#^sha256sums=\\('[0-9a-f]+'\\)\$#sha256sums=('0000000000000000000000000000000000000000000000000000000000000000')#m" packaging/arch/PKGBUILD
perl -0pi -e 's/^[[:space:]]*pkgver = \d+\.\d+\.\d+$/\tpkgver = '"$version"'/m' packaging/arch/.SRCINFO
perl -0pi -e 's/^[[:space:]]*pkgrel = \d+$/\tpkgrel = 1/m' packaging/arch/.SRCINFO
perl -0pi -e 's#^[[:space:]]*source = [^[:space:]]+$#\tsource = icey-'"$version"'.tar.gz::https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz#m' packaging/arch/.SRCINFO
perl -0pi -e 's/^[[:space:]]*sha256sums = [0-9a-f]+$/\tsha256sums = 0000000000000000000000000000000000000000000000000000000000000000/m' packaging/arch/.SRCINFO

for file in "${docs[@]}"; do
    perl -0pi -e 's/GIT_TAG v?\d+\.\d+\.\d+/GIT_TAG '"$version"'/g' "$file"
done

echo "synced release metadata to $version"
echo "next: make release-check VERSION=$version"
echo "then: make release-pin VERSION=$version"
