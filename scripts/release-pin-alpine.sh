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

perl -0pi -e 's/^pkgver=\d+\.\d+\.\d+$/pkgver='"$version"'/m' packaging/alpine/APKBUILD
perl -0pi -e 's/^(source="\$pkgname-\$pkgver\.tar\.gz::https:\/\/github\.com\/nilstate\/icey\/archive\/refs\/tags\/)\$pkgver(\.tar\.gz")$/${1}\$pkgver${2}/m' packaging/alpine/APKBUILD
perl -0pi -e 's/^\S+[[:space:]]+icey-\d+\.\d+\.\d+\.tar\.gz$/'"$RELEASE_ARCHIVE_SHA512"'  icey-'"$version"'.tar.gz/m' packaging/alpine/APKBUILD

echo "updated packaging/alpine/APKBUILD for $version"
echo "sha512: $RELEASE_ARCHIVE_SHA512"
