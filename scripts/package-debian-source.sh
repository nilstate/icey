#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

version="${1:-$(tr -d '[:space:]' < VERSION)}"
if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "expected plain semantic version like 2.4.0" >&2
    exit 1
fi

stage_root="${ICEY_DEBIAN_STAGE_DIR:-$repo_root/build/package/debian}"
distribution="${DEBIAN_DISTRIBUTION:-unstable}"
dpkg_buildpackage="${DPKG_BUILDPACKAGE:-dpkg-buildpackage}"
work_dir="$stage_root/icey-$version"
orig_tarball="$stage_root/icey_${version}.orig.tar.gz"
tmp_dir=$(mktemp -d)
trap 'rm -rf "$tmp_dir"' EXIT

download_extract() {
    local url="$1"
    local sha256="$2"
    local destination="$3"
    local archive="$tmp_dir/archive.tar.gz"

    rm -f "$archive"
    rm -rf "$destination"
    mkdir -p "$destination"
    curl --fail --location --retry 3 --retry-delay 1 --output "$archive" "$url"

    local actual
    if command -v sha256sum >/dev/null 2>&1; then
        actual=$(sha256sum "$archive" | awk '{print $1}')
    elif command -v shasum >/dev/null 2>&1; then
        actual=$(shasum -a 256 "$archive" | awk '{print $1}')
    elif command -v openssl >/dev/null 2>&1; then
        actual=$(openssl dgst -sha256 "$archive" | awk '{print $NF}')
    else
        echo "need sha256sum, shasum, or openssl to verify vendor archives" >&2
        exit 1
    fi

    if [ "$actual" != "$sha256" ]; then
        echo "sha256 mismatch for $url: expected $sha256 got $actual" >&2
        exit 1
    fi

    tar -xzf "$archive" -C "$destination" --strip-components=1
}

mkdir -p "$stage_root"
rm -rf "$work_dir"

rsync -a \
    --exclude '.git/' \
    --exclude '.github/' \
    --exclude '.plans/' \
    --exclude '.venv/' \
    --exclude 'venv/' \
    --exclude '__pycache__/' \
    --exclude '_deps/' \
    --exclude 'build/' \
    --exclude 'build-*/' \
    --exclude 'dist/' \
    --exclude 'docs/dist/' \
    --exclude 'docs/node_modules/' \
    --exclude 'packaging/arch/pkg/' \
    --exclude 'packaging/arch/src/' \
    --exclude 'packaging/arch/*.pkg.tar.*' \
    --exclude 'packaging/arch/*.src.tar.*' \
    "$repo_root/" "$work_dir/"

rm -rf "$work_dir/debian"
cp -a "$repo_root/packaging/debian/debian" "$work_dir/debian"
perl -0pi -e 's/^icey \(\d+\.\d+\.\d+-\d+\) .*/icey ('"$version"'-1) '"$distribution"'; urgency=medium/m' "$work_dir/debian/changelog"
perl -0pi -e 's/^ -- .*$/ -- 0state OSS <oss\@0state.com>  '"$(date -R)"'/m' "$work_dir/debian/changelog"

mkdir -p "$work_dir/vendor"
download_extract "https://dist.libuv.org/dist/v1.50.0/libuv-v1.50.0.tar.gz" \
    "6dc2b39aa7fa66c04e2e015bf47738b84e1c6b86b8987d57dd432d149d77ee25" \
    "$work_dir/vendor/libuv"
download_extract "https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.1.tar.gz" \
    "3c163891446e529604b590f9ad097b2e98b5ef7e4d3ddcf1cf98b62ca668f23e" \
    "$work_dir/vendor/llhttp"
download_extract "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz" \
    "17e88863f3600672ab49182f217281b6fc4d3c762bde361935e436a95214d05c" \
    "$work_dir/vendor/zlib"
download_extract "https://github.com/paullouisageneau/libdatachannel/archive/refs/tags/v0.24.1.tar.gz" \
    "e6fc363497a41b5dce38602937c12d30e5e536943cf09c5ee5671c8f206eee08" \
    "$work_dir/vendor/libdatachannel"
mkdir -p "$work_dir/vendor/libdatachannel/deps"
download_extract "https://github.com/SergiusTheBest/plog/archive/94899e0b926ac1b0f4750bfbd495167b4a6ae9ef.tar.gz" \
    "92a08bce559b5f28aa88d3fd9071567414b9f43a83fe2a05a6dd14f1da536072" \
    "$work_dir/vendor/libdatachannel/deps/plog"

rm -f "$orig_tarball"
tar --exclude="icey-$version/debian" -czf "$orig_tarball" -C "$stage_root" "icey-$version"

(
    cd "$work_dir"
    if command -v dh >/dev/null 2>&1; then
        "$dpkg_buildpackage" -S -us -uc -d
    else
        dpkg-source -b .
        echo "warning: dh is not installed; generated the source package without a .changes file" >&2
    fi
)

echo "staged Debian source package in $stage_root"
echo "orig tarball: $orig_tarball"
