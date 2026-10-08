#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
version=$(tr -d '[:space:]' < "$repo_root/VERSION")
[[ "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || {
    echo "VERSION must contain a plain semantic version" >&2
    exit 1
}

if [[ -n "${RELEASE_RENDER_ROOT:-}" ]]; then
    render_root=$(cd "$RELEASE_RENDER_ROOT" && pwd)
    manifest_file="$render_root/release-manifest.env"
    [[ -f "$manifest_file" ]] || { echo "missing verified release manifest" >&2; exit 1; }
    source "$manifest_file"
    [[ "$RELEASE_VERSION" == "$version" ]] || { echo "release render version mismatch" >&2; exit 1; }
    [[ "$RELEASE_SOURCE_COMMIT" == "$(git -C "$repo_root" rev-parse "${version}^{commit}")" ]] || {
        echo "release render source revision mismatch" >&2
        exit 1
    }
    remote_commit=$(git -C "$repo_root" ls-remote --refs --tags origin "refs/tags/${version}" | awk 'NR == 1 { print $1 }')
    [[ "$remote_commit" == "$RELEASE_SOURCE_COMMIT" ]] || {
        echo "remote release tag no longer matches the render" >&2
        exit 1
    }
    RELEASE_OUTPUT_ROOT="$render_root" RELEASE_MANIFEST_FILE="$manifest_file" \
        bash "$repo_root/scripts/release-check.sh" "$version" >&2
    printf '%s\n' "$render_root"
    exit 0
fi

# Resolve the immutable tag and both archive digests once for every renderer.
manifest=$(
    RELEASE_REQUIRE_REMOTE_TAG=1 RELEASE_FETCH_ARCHIVE_META=1 \
        bash "$repo_root/scripts/release-manifest.sh" "$version"
)
eval "$manifest"
[[ "$RELEASE_LOCAL_TAG_COMMIT" == "$RELEASE_REMOTE_TAG_COMMIT" && -n "$RELEASE_LOCAL_TAG_COMMIT" ]] || {
    echo "local and remote release tags disagree for $version" >&2
    exit 1
}

mkdir -p "$repo_root/.stage"
render_root=$(mktemp -d "$repo_root/.stage/release-${version}.XXXXXX")
git -C "$repo_root" archive "$version" | tar -x -C "$render_root"
[[ "$(tr -d '[:space:]' < "$render_root/VERSION")" == "$version" ]] || {
    echo "release tag and VERSION disagree" >&2
    exit 1
}
printf '%s\n' "$manifest" > "$render_root/release-manifest.env"
printf 'RELEASE_SOURCE_COMMIT=%q\n' "$RELEASE_LOCAL_TAG_COMMIT" >> "$render_root/release-manifest.env"

export RELEASE_OUTPUT_ROOT="$render_root"
export RELEASE_MANIFEST_FILE="$render_root/release-manifest.env"
for channel in conan vcpkg arch homebrew alpine macports spack conda; do
    bash "$repo_root/scripts/release-pin-${channel}.sh" "$version" >&2
done
bash "$repo_root/scripts/release-check.sh" "$version" >&2

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
    printf 'render_root=%s\n' "$render_root" >> "$GITHUB_OUTPUT"
fi
printf '%s\n' "$render_root"
