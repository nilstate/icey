#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
upstream_dir=${ICEY_CONAN_UPSTREAM_DIR:-}
conan_bin=${CONAN:-conan}
version=$(tr -d '[:space:]' < "$repo_root/VERSION")
recipe_name=icey

if [[ -z "$upstream_dir" ]]; then
    echo "set ICEY_CONAN_UPSTREAM_DIR to a conan-center-index checkout" >&2
    exit 1
fi

if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "VERSION must contain a plain semantic version" >&2
    exit 1
fi

upstream_dir=$(cd "$upstream_dir" && pwd)
source_recipe_dir="$repo_root/packaging/conan-center-index/recipes/$recipe_name"
target_recipe_dir="$upstream_dir/recipes/$recipe_name"

if [[ ! -d "$source_recipe_dir" ]]; then
    echo "missing source ConanCenter recipe: $source_recipe_dir" >&2
    exit 1
fi

if [[ ! -d "$upstream_dir/.git" || ! -d "$upstream_dir/recipes" ]]; then
    echo "expected a conan-center-index git checkout with recipes/: $upstream_dir" >&2
    exit 1
fi

rm -rf "$target_recipe_dir"
mkdir -p "$target_recipe_dir"
cp -R "$source_recipe_dir/." "$target_recipe_dir/"

"$conan_bin" create "$target_recipe_dir/all" --version="$version" --build=missing -s compiler.cppstd=20

echo "staged upstream ConanCenter recipe in $upstream_dir"
echo "validated:"
echo "  recipes/$recipe_name/config.yml"
echo "  recipes/$recipe_name/all/conandata.yml"
echo "  recipes/$recipe_name/all/conanfile.py"
echo "  recipes/$recipe_name/all/test_package"
echo
echo "git status summary:"
git -C "$upstream_dir" status --short -- "recipes/$recipe_name"
