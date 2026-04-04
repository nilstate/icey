#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
upstream_dir=${ICEY_VCPKG_UPSTREAM_DIR:-}

if [[ -z "$upstream_dir" ]]; then
    echo "set ICEY_VCPKG_UPSTREAM_DIR to a microsoft/vcpkg checkout" >&2
    exit 1
fi

upstream_dir=$(cd "$upstream_dir" && pwd)
port_name=icey
source_port_dir="$repo_root/packaging/vcpkg/$port_name"
target_port_dir="$upstream_dir/ports/$port_name"

if [[ ! -d "$source_port_dir" ]]; then
    echo "missing source vcpkg port: $source_port_dir" >&2
    exit 1
fi

if [[ ! -d "$upstream_dir/.git" || ! -d "$upstream_dir/ports" || ! -d "$upstream_dir/versions" ]]; then
    echo "expected a vcpkg git checkout with ports/ and versions/: $upstream_dir" >&2
    exit 1
fi

vcpkg_bin=${VCPKG:-"$upstream_dir/vcpkg"}
if [[ ! -x "$vcpkg_bin" ]]; then
    echo "vcpkg executable not found or not executable: $vcpkg_bin" >&2
    exit 1
fi

rm -rf "$target_port_dir"
mkdir -p "$target_port_dir"
cp -R "$source_port_dir/." "$target_port_dir/"
"$vcpkg_bin" format-manifest "$target_port_dir/vcpkg.json"

(
    cd "$upstream_dir"
    "$vcpkg_bin" x-add-version "$port_name" --overwrite-version --vcpkg-root="$upstream_dir"
)

echo "staged upstream vcpkg port in $upstream_dir"
echo "updated:"
echo "  ports/$port_name"
echo "  versions/${port_name:0:1}-/$port_name.json"
echo "  versions/baseline.json"
echo
echo "git status summary:"
git -C "$upstream_dir" status --short -- "ports/$port_name" "versions/${port_name:0:1}-/$port_name.json" versions/baseline.json
