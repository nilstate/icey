#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

version="${1:-$(tr -d '[:space:]' < VERSION)}"
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "expected plain semantic version like 2.4.0" >&2
    exit 1
fi

tag="$version"
require_remote_tag="${RELEASE_REQUIRE_REMOTE_TAG:-0}"
fetch_archive_meta="${RELEASE_FETCH_ARCHIVE_META:-0}"

head_commit="$(git rev-parse HEAD)"
local_tag_commit=""
if git rev-parse -q --verify "refs/tags/${tag}^{commit}" >/dev/null 2>&1; then
    local_tag_commit="$(git rev-parse "${tag}^{commit}")"
fi

remote_tag_commit="$(
    git ls-remote --refs --tags origin "refs/tags/${tag}" 2>/dev/null \
        | awk 'NR == 1 { print $1 }'
)"

if [[ -z "$remote_tag_commit" && "$require_remote_tag" == "1" ]]; then
    echo "remote tag ${tag} not found on origin" >&2
    exit 1
fi

release_archive_url="https://github.com/nilstate/icey/archive/refs/tags/${version}.tar.gz"
release_archive_sha256=""
release_archive_sha512=""

macports_archive_url="https://github.com/nilstate/icey/archive/${version}/icey-${version}.tar.gz"
macports_archive_rmd160=""
macports_archive_sha256=""
macports_archive_size=""

if [[ "$fetch_archive_meta" == "1" ]]; then
    eval "$("$repo_root"/scripts/release-archive-meta.sh "$release_archive_url")"
    release_archive_sha256="$ARCHIVE_SHA256"
    release_archive_sha512="$ARCHIVE_SHA512"

    eval "$("$repo_root"/scripts/release-archive-meta.sh "$macports_archive_url")"
    macports_archive_rmd160="$ARCHIVE_RMD160"
    macports_archive_sha256="$ARCHIVE_SHA256"
    macports_archive_size="$ARCHIVE_SIZE"
fi

printf 'RELEASE_VERSION=%q\n' "$version"
printf 'RELEASE_TAG=%q\n' "$tag"
printf 'RELEASE_HEAD_COMMIT=%q\n' "$head_commit"
printf 'RELEASE_LOCAL_TAG_COMMIT=%q\n' "$local_tag_commit"
printf 'RELEASE_REMOTE_TAG_COMMIT=%q\n' "$remote_tag_commit"
printf 'RELEASE_ARCHIVE_URL=%q\n' "$release_archive_url"
printf 'RELEASE_ARCHIVE_SHA256=%q\n' "$release_archive_sha256"
printf 'RELEASE_ARCHIVE_SHA512=%q\n' "$release_archive_sha512"
printf 'RELEASE_MACPORTS_ARCHIVE_URL=%q\n' "$macports_archive_url"
printf 'RELEASE_MACPORTS_RMD160=%q\n' "$macports_archive_rmd160"
printf 'RELEASE_MACPORTS_SHA256=%q\n' "$macports_archive_sha256"
printf 'RELEASE_MACPORTS_SIZE=%q\n' "$macports_archive_size"
