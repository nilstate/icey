#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

version="${1:-$(tr -d '[:space:]' < VERSION)}"
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "expected plain semantic version like 2.4.0" >&2
    exit 1
fi

rpmbuild_bin="${RPMBUILD:-rpmbuild}"
stage_root="${ICEY_RPM_STAGE_DIR:-$repo_root/build/package/rpm}"
topdir="$stage_root/rpmbuild"
archive_url="https://github.com/nilstate/icey/archive/refs/tags/${version}.tar.gz"
source_archive="$topdir/SOURCES/icey-${version}.tar.gz"
spec_file="$repo_root/packaging/rpm/icey.spec"

if ! command -v "$rpmbuild_bin" >/dev/null 2>&1; then
    echo "rpmbuild is required to generate the SRPM staging tree" >&2
    exit 1
fi

mkdir -p \
    "$topdir/BUILD" \
    "$topdir/BUILDROOT" \
    "$topdir/RPMS" \
    "$topdir/SOURCES" \
    "$topdir/SPECS" \
    "$topdir/SRPMS"

curl --fail --location --retry 3 --retry-delay 1 --output "$source_archive" "$archive_url"
cp "$spec_file" "$topdir/SPECS/icey.spec"

"$rpmbuild_bin" -bs "$topdir/SPECS/icey.spec" --define "_topdir $topdir"

echo "staged RPM sources under $topdir"
echo "source archive: $source_archive"
find "$topdir/SRPMS" -maxdepth 1 -type f -name '*.src.rpm' -print
