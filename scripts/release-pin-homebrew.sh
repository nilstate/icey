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

archive_url="https://github.com/nilstate/icey/archive/refs/tags/${version}.tar.gz"
eval "$("$repo_root"/scripts/release-archive-meta.sh "$archive_url")"

perl -0pi -e 's#url "https://github.com/nilstate/icey/archive/refs/tags/\d+\.\d+\.\d+\.tar\.gz"#url "https://github.com/nilstate/icey/archive/refs/tags/'"$version"'.tar.gz"#' packaging/homebrew/Formula/icey.rb
perl -0pi -e 's/version "\d+\.\d+\.\d+"/version "'"$version"'"/' packaging/homebrew/Formula/icey.rb
perl -0pi -e 's/sha256 "[0-9a-f]+"/sha256 "'"$ARCHIVE_SHA256"'"/' packaging/homebrew/Formula/icey.rb

echo "updated packaging/homebrew/Formula/icey.rb for $version"
echo "sha256: $ARCHIVE_SHA256"
