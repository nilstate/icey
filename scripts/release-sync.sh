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
cd "$repo_root"

if ! grep -Eq "^## \\[$version\\]" CHANGELOG.md; then
    echo "CHANGELOG.md is missing a section for [$version]" >&2
    exit 1
fi

python3 "$repo_root/scripts/release-sync-packaging.py" "$repo_root" "$version"

docs=(
    README.md
    docs/modules/av.md
    docs/modules/base.md
    docs/modules/http.md
    docs/modules/net.md
    docs/modules/webrtc.md
    llms.txt
)

printf '%s\n' "$version" > VERSION
perl -0pi -e 's/^PROJECT_NUMBER[[:space:]]*=.*$/PROJECT_NUMBER         = '"$version"'/m' Doxyfile

perl -0pi -e 's/version = "\d+\.\d+\.\d+"/version = "'"$version"'"/' packaging/conan/conanfile.py
cat > packaging/conan/conandata.yml <<EOF
sources:
  "$version":
    url: "https://github.com/nilstate/icey/archive/refs/tags/$version.tar.gz"
    sha256: "@RELEASE_ARCHIVE_SHA256@"
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
    sha256: "@RELEASE_ARCHIVE_SHA256@"
EOF
perl -0pi -e 's/"version": "\d+\.\d+\.\d+"/"version": "'"$version"'"/' packaging/vcpkg/icey/vcpkg.json
perl -0pi -e 's/^pkgver=\d+\.\d+\.\d+$/pkgver='"$version"'/m' packaging/arch/PKGBUILD
perl -0pi -e 's/^pkgrel=\d+$/pkgrel=1/m' packaging/arch/PKGBUILD
perl -0pi -e "s#^sha256sums=.*\$#sha256sums=('\@RELEASE_ARCHIVE_SHA256\@')#m" packaging/arch/PKGBUILD
perl -0pi -e 's/^[[:space:]]*pkgver = \d+\.\d+\.\d+$/\tpkgver = '"$version"'/m' packaging/arch/.SRCINFO
perl -0pi -e 's/^[[:space:]]*pkgrel = \d+$/\tpkgrel = 1/m' packaging/arch/.SRCINFO
perl -0pi -e 's#^[[:space:]]*source = [^[:space:]]+$#\tsource = icey-'"$version"'.tar.gz::https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz#m' packaging/arch/.SRCINFO
perl -0pi -e 's/^[[:space:]]*sha256sums = \S+$/\tsha256sums = \@RELEASE_ARCHIVE_SHA256\@/m' packaging/arch/.SRCINFO
perl -0pi -e 's/REF "\d+\.\d+\.\d+"/REF "'"$version"'"/' packaging/vcpkg/icey/portfile.cmake
perl -0pi -e 's/SHA512 \S+/SHA512 \@RELEASE_ARCHIVE_SHA512\@/' packaging/vcpkg/icey/portfile.cmake
perl -0pi -e 's#url "https://github.com/nilstate/icey/archive/refs/tags/\d+\.\d+\.\d+\.tar\.gz"#url "https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz"#' packaging/homebrew/Formula/icey.rb
perl -0pi -e 's/version "\d+\.\d+\.\d+"/version "'"$version"'"/' packaging/homebrew/Formula/icey.rb
perl -0pi -e 's/sha256 "[^"]+"/sha256 "\@RELEASE_ARCHIVE_SHA256\@"/' packaging/homebrew/Formula/icey.rb
perl -0pi -e 's/^Version:\s+\d+\.\d+\.\d+$/Version:        '"$version"'/m' packaging/rpm/icey.spec
perl -0pi -e 's/^pkgver=\d+\.\d+\.\d+$/pkgver='"$version"'/m' packaging/alpine/APKBUILD
perl -0pi -e 's/^\S+[[:space:]]+icey-\d+\.\d+\.\d+\.tar\.gz$/\@RELEASE_ARCHIVE_SHA512\@  icey-'"$version"'.tar.gz/m' packaging/alpine/APKBUILD
perl -0pi -e 's/github.setup\s+nilstate\s+icey\s+\d+\.\d+\.\d+/github.setup        nilstate icey '"$version"'/g' packaging/macports/Portfile
python3 - <<'PY'
from pathlib import Path
import re

path = Path("packaging/macports/Portfile")
source = path.read_text()
for field, token in (
    ("rmd160", "@RELEASE_MACPORTS_RMD160@"),
    ("sha256", "@RELEASE_MACPORTS_SHA256@"),
    ("size", "@RELEASE_MACPORTS_SIZE@"),
):
    prefix = "checksums[ \\t]+" if field == "rmd160" else ""
    pattern = rf"^([ \t]*{prefix}{field}[ \t]+)\S+"
    source, count = re.subn(pattern, lambda match: match.group(1) + token, source, flags=re.M)
    if count != 1:
        raise SystemExit(f"expected one MacPorts {field} field, found {count}")
path.write_text(source)
PY
perl -0pi -e 's#url = "https://github.com/nilstate/icey/archive/refs/tags/\d+\.\d+\.\d+\.tar\.gz"#url = "https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz"#' packaging/spack/package.py
perl -0pi -e 's/version\("\d+\.\d+\.\d+", sha256="[^"]+"\)/version("'"$version"'", sha256="\@RELEASE_ARCHIVE_SHA256\@")/' packaging/spack/package.py
perl -0pi -e 's/\{\% set version = "\d+\.\d+\.\d+" \%\}/{% set version = "'"$version"'" %}/' packaging/conda-forge/meta.yaml
perl -0pi -e 's/^  sha256: \S+$/  sha256: \@RELEASE_ARCHIVE_SHA256\@/m' packaging/conda-forge/meta.yaml

for file in "${docs[@]}"; do
    perl -0pi -e 's/GIT_TAG v?\d+\.\d+\.\d+/GIT_TAG '"$version"'/g' "$file"
done

echo "synced pre-tag release templates from VERSION=$version"
