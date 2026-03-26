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
    doc/getting-started.md
    doc/installation.md
    doc/modules/av.md
    doc/modules/base.md
    doc/modules/http.md
    doc/modules/net.md
    doc/modules/webrtc.md
    llms.txt
)

printf '%s\n' "$version" > VERSION

perl -0pi -e 's/version = "\d+\.\d+\.\d+"/version = "'"$version"'"/' packaging/conan/conanfile.py
perl -0pi -e 's/"version": "\d+\.\d+\.\d+"/"version": "'"$version"'"/' packaging/vcpkg/icey/vcpkg.json

for file in "${docs[@]}"; do
    perl -0pi -e 's/GIT_TAG v?\d+\.\d+\.\d+/GIT_TAG '"$version"'/g' "$file"
done

echo "synced release metadata to $version"
echo "next: make release-check VERSION=$version"
