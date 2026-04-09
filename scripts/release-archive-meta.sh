#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 1 ]; then
    echo "usage: $0 <archive-url>" >&2
    exit 1
fi

archive_url="$1"
tmp_dir=$(mktemp -d)
archive_path="$tmp_dir/archive.tar.gz"
trap 'rm -rf "$tmp_dir"' EXIT

curl --fail --location --retry 3 --retry-delay 1 --output "$archive_path" "$archive_url"

if command -v sha256sum >/dev/null 2>&1; then
    sha256=$(sha256sum "$archive_path" | awk '{print $1}')
elif command -v shasum >/dev/null 2>&1; then
    sha256=$(shasum -a 256 "$archive_path" | awk '{print $1}')
elif command -v openssl >/dev/null 2>&1; then
    sha256=$(openssl dgst -sha256 "$archive_path" | awk '{print $NF}')
else
    echo "need sha256sum, shasum, or openssl to compute the archive sha256" >&2
    exit 1
fi

if command -v sha512sum >/dev/null 2>&1; then
    sha512=$(sha512sum "$archive_path" | awk '{print $1}')
elif command -v shasum >/dev/null 2>&1; then
    sha512=$(shasum -a 512 "$archive_path" | awk '{print $1}')
elif command -v openssl >/dev/null 2>&1; then
    sha512=$(openssl dgst -sha512 "$archive_path" | awk '{print $NF}')
else
    echo "need sha512sum, shasum, or openssl to compute the archive sha512" >&2
    exit 1
fi

if command -v openssl >/dev/null 2>&1; then
    rmd160=$(openssl dgst -rmd160 "$archive_path" | awk '{print $NF}')
else
    rmd160=""
fi

size=$(wc -c < "$archive_path" | tr -d '[:space:]')

printf 'ARCHIVE_URL=%q\n' "$archive_url"
printf 'ARCHIVE_SHA256=%q\n' "$sha256"
printf 'ARCHIVE_SHA512=%q\n' "$sha512"
printf 'ARCHIVE_RMD160=%q\n' "$rmd160"
printf 'ARCHIVE_SIZE=%q\n' "$size"
