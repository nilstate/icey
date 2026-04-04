#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
conan_bin=${CONAN:-conan}
version=$(tr -d '[:space:]' < "$repo_root/VERSION")

if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "VERSION must contain a plain semantic version" >&2
    exit 1
fi

exec "$conan_bin" create "$repo_root/packaging/conan-center-index/recipes/icey/all" --version="$version" --build=missing -s compiler.cppstd=20
