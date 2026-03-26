#!/usr/bin/env bash
set -euo pipefail

fail() {
    echo "$1" >&2
    exit 1
}

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

version="${1:-$(tr -d '[:space:]' < VERSION)}"
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    fail "expected plain semantic version like 2.4.0"
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

current_version=$(tr -d '[:space:]' < VERSION)
[ "$current_version" = "$version" ] || fail "VERSION contains $current_version, expected $version"

grep -Eq '^    version = "'"$version"'"$' packaging/conan/conanfile.py \
    || fail "packaging/conan/conanfile.py is not synced to $version"
grep -Eq '^[[:space:]]*"version": "'"$version"'"' packaging/vcpkg/icey/vcpkg.json \
    || fail "packaging/vcpkg/icey/vcpkg.json is not synced to $version"

for file in "${docs[@]}"; do
    grep -Eq 'GIT_TAG '"$version"'([^0-9]|$)' "$file" \
        || fail "$file does not reference GIT_TAG $version"
    if grep -En 'GIT_TAG ' "$file" | grep -Ev 'GIT_TAG '"$version"'([^0-9]|$)' >/dev/null; then
        fail "$file still contains a stale GIT_TAG reference"
    fi
done

grep -Eq "^## \\[$version\\]" CHANGELOG.md || fail "CHANGELOG.md is missing [$version]"

section=$(
    awk '/^## \['"$version"'\]/{found=1; next} /^## \[/{if(found) exit} found{print}' CHANGELOG.md
)
printf '%s\n' "$section" | grep -Eq '[^[:space:]]' \
    || fail "CHANGELOG.md [$version] section is empty"

echo "release metadata is in sync for $version"
