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

eval "$(
    RELEASE_REQUIRE_REMOTE_TAG=1 \
    RELEASE_FETCH_ARCHIVE_META=1 \
    bash "$repo_root"/scripts/release-manifest.sh "$version"
)"

perl -0pi -e 's/REF "\d+\.\d+\.\d+"/REF "'"$version"'"/' packaging/vcpkg/icey/portfile.cmake
perl -0pi -e 's/SHA512 [0-9a-f]+/SHA512 '"$RELEASE_ARCHIVE_SHA512"'/i' packaging/vcpkg/icey/portfile.cmake
perl -0pi -e 's/HEAD_REF \w+/HEAD_REF main/' packaging/vcpkg/icey/portfile.cmake

echo "updated packaging/vcpkg/icey/portfile.cmake for $version"
echo "sha512: $RELEASE_ARCHIVE_SHA512"
