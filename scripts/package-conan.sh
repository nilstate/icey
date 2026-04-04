#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
conan_bin=${CONAN:-conan}

if [[ -z "${ICEY_CONAN_SOURCE_PATH+x}" ]]; then
    export ICEY_CONAN_SOURCE_PATH="$repo_root"
fi

exec "$conan_bin" create "$repo_root/packaging/conan" --build=missing -s compiler.cppstd=20
