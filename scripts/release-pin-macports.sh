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

eval "$(
    RELEASE_REQUIRE_REMOTE_TAG=1 \
    RELEASE_FETCH_ARCHIVE_META=1 \
    bash "$repo_root"/scripts/release-manifest.sh "$version"
)"

if [ -z "$RELEASE_MACPORTS_RMD160" ]; then
    echo "need openssl to compute the MacPorts rmd160 checksum" >&2
    exit 1
fi

perl -0pi -e 's/github.setup\s+nilstate\s+icey\s+\d+\.\d+\.\d+/github.setup        nilstate icey '"$version"'/g' packaging/macports/Portfile
tmp_portfile=$(mktemp)
awk -v rmd160="$RELEASE_MACPORTS_RMD160" -v sha256="$RELEASE_MACPORTS_SHA256" -v size="$RELEASE_MACPORTS_SIZE" '
    /^checksums[[:space:]]+rmd160[[:space:]]+[0-9a-f]{40}[[:space:]]+\\$/ {
        print "checksums           rmd160  " rmd160 " \\"
        if (getline <= 0) {
            exit 1
        }
        print "                    sha256  " sha256 " \\"
        if (getline <= 0) {
            exit 1
        }
        print "                    size    " size
        next
    }
    { print }
' packaging/macports/Portfile > "$tmp_portfile"
mv "$tmp_portfile" packaging/macports/Portfile

grep -Eq '^checksums[[:space:]]+rmd160[[:space:]]+'"${RELEASE_MACPORTS_RMD160}"' \\$' packaging/macports/Portfile \
    || { echo "failed to update packaging/macports/Portfile rmd160" >&2; exit 1; }
grep -Eq '^[[:space:]]+sha256[[:space:]]+'"${RELEASE_MACPORTS_SHA256}"' \\$' packaging/macports/Portfile \
    || { echo "failed to update packaging/macports/Portfile sha256" >&2; exit 1; }
grep -Eq '^[[:space:]]+size[[:space:]]+'"${RELEASE_MACPORTS_SIZE}"'$' packaging/macports/Portfile \
    || { echo "failed to update packaging/macports/Portfile size" >&2; exit 1; }

echo "updated packaging/macports/Portfile for $version"
echo "rmd160: $RELEASE_MACPORTS_RMD160"
echo "sha256: $RELEASE_MACPORTS_SHA256"
echo "size: $RELEASE_MACPORTS_SIZE"
