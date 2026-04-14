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

perl -0pi -e 's/\{\% set version = "\d+\.\d+\.\d+" \%\}/{% set version = "'"$version"'" %}/' packaging/conda-forge/meta.yaml
perl -0pi -e 's/^  sha256: [0-9a-f]{64}$/  sha256: '"$RELEASE_ARCHIVE_SHA256"'/m' packaging/conda-forge/meta.yaml

echo "updated packaging/conda-forge/meta.yaml for $version"
echo "sha256: $RELEASE_ARCHIVE_SHA256"
