#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
stage_dir=${ICEY_VCPKG_REGISTRY_DIR:-"$repo_root/build/package/vcpkg-registry"}
port_name=icey
port_dir="$repo_root/packaging/vcpkg/$port_name"

if [[ ! -d "$port_dir" ]]; then
    echo "missing vcpkg port directory: $port_dir" >&2
    exit 1
fi

if ! command -v git >/dev/null 2>&1; then
    echo "git is required to compute the vcpkg registry git-tree hash" >&2
    exit 1
fi

version=$(python3 - <<'PY' "$port_dir/vcpkg.json"
import json
import pathlib
import sys

data = json.loads(pathlib.Path(sys.argv[1]).read_text())
print(data["version"])
PY
)

prefix=${port_name:0:1}-
registry_port_dir="$stage_dir/ports/$port_name"
versions_dir="$stage_dir/versions/$prefix"
version_file="$versions_dir/$port_name.json"
baseline_file="$stage_dir/versions/baseline.json"
config_file="$stage_dir/vcpkg-configuration.template.json"

rm -rf "$stage_dir"
mkdir -p "$registry_port_dir" "$versions_dir"
cp -R "$port_dir/." "$registry_port_dir/"

git -C "$stage_dir" init -q
git -C "$stage_dir" config user.name "icey vcpkg staging"
git -C "$stage_dir" config user.email "oss@0state.com"
git -C "$stage_dir" add "ports/$port_name"

root_tree=$(git -C "$stage_dir" write-tree)
port_tree=$(git -C "$stage_dir" ls-tree "$root_tree" "ports/$port_name" | awk '{print $3}')

if [[ -z "$port_tree" ]]; then
    echo "failed to compute git-tree hash for ports/$port_name" >&2
    exit 1
fi

python3 - <<'PY' "$version_file" "$baseline_file" "$version" "$port_tree" "$port_name"
import json
import pathlib
import sys

version_file = pathlib.Path(sys.argv[1])
baseline_file = pathlib.Path(sys.argv[2])
version = sys.argv[3]
port_tree = sys.argv[4]
port_name = sys.argv[5]

version_file.write_text(json.dumps({
    "versions": [
        {
            "version": version,
            "git-tree": port_tree,
        }
    ]
}, indent=2) + "\n")

baseline_file.write_text(json.dumps({
    "default": {
        port_name: {
            "baseline": version,
            "port-version": 0,
        }
    }
}, indent=2) + "\n")
PY

git -C "$stage_dir" add "ports/$port_name" "versions"
git -C "$stage_dir" commit -q -m "Stage $port_name $version"
registry_commit=$(git -C "$stage_dir" rev-parse HEAD)

python3 - <<'PY' "$config_file" "$stage_dir" "$registry_commit" "$port_name"
import json
import pathlib
import sys

config_file = pathlib.Path(sys.argv[1])
stage_dir = pathlib.Path(sys.argv[2]).resolve().as_posix()
registry_commit = sys.argv[3]
port_name = sys.argv[4]

config_file.write_text(json.dumps({
    "default-registry": {
        "kind": "builtin",
        "baseline": "REPLACE_WITH_YOUR_VCPKG_BASELINE_COMMIT",
    },
    "registries": [
        {
            "kind": "git",
            "repository": f"file://{stage_dir}",
            "baseline": registry_commit,
            "packages": [port_name],
        }
    ]
}, indent=2) + "\n")
PY

echo "staged custom vcpkg registry in $stage_dir"
echo "port: ports/$port_name"
echo "baseline: versions/baseline.json"
echo "versions: versions/$prefix/$port_name.json"
echo "git-tree: $port_tree"
echo "registry commit: $registry_commit"
echo "config template: vcpkg-configuration.template.json"
