#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

if ! command -v docker >/dev/null 2>&1; then
    echo "docker is required to validate the Alpine APKBUILD" >&2
    exit 1
fi

alpine_image="${ALPINE_IMAGE:-alpine:3.21}"
host_uid="${HOST_UID:-$(id -u)}"
host_gid="${HOST_GID:-$(id -g)}"
source_mode="${ICEY_ALPINE_SOURCE_MODE:-release}"
package_workspace="$repo_root/packaging/alpine"
temp_dir=""

cleanup() {
    if [ -n "$temp_dir" ] && [ -d "$temp_dir" ]; then
        rm -rf "$temp_dir"
    fi
}
trap cleanup EXIT

case "$source_mode" in
    release)
        ;;
    worktree)
        pkgver=$(awk -F= '/^pkgver=/{print $2; exit}' "$package_workspace/APKBUILD")
        archive_name="icey-$pkgver.tar.gz"
        archive_root="icey-$pkgver"
        temp_dir=$(mktemp -d)
        package_workspace="$temp_dir/alpine"
        mkdir -p "$package_workspace"
        cp -a "$repo_root/packaging/alpine/." "$package_workspace/"

        git -C "$repo_root" ls-files --cached --others --exclude-standard -z \
            | while IFS= read -r -d '' path; do
                case "$path" in
                    packaging/alpine/src/*|packaging/alpine/pkg/*|packaging/alpine/packages/*)
                        continue
                        ;;
                esac
                printf '%s\0' "$path"
            done \
            | tar -C "$repo_root" --null --verbatim-files-from --transform "s|^|$archive_root/|" \
                -czf "$package_workspace/$archive_name" -T -

        sed -i "s|^source=.*$|source=\"$archive_name\"|" "$package_workspace/APKBUILD"
        ;;
    *)
        echo "unsupported ICEY_ALPINE_SOURCE_MODE: $source_mode" >&2
        echo "expected one of: release, worktree" >&2
        exit 1
        ;;
esac

docker run --rm \
    -e HOST_UID="$host_uid" \
    -e HOST_GID="$host_gid" \
    -e CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-1}" \
    -e ICEY_ALPINE_SOURCE_MODE="$source_mode" \
    -v "$repo_root:/workspace" \
    -v "$package_workspace:/package" \
    -w /workspace \
    "$alpine_image" \
    sh -lc '
        set -euo pipefail
        apk add --no-cache alpine-sdk cmake ninja ffmpeg-dev libuv-dev llhttp-dev linux-headers minizip-dev openssl-dev pkgconf zlib-dev git sudo >/dev/null

        group_name=$(awk -F: -v gid="$HOST_GID" "\$3 == gid { print \$1; exit }" /etc/group)
        if [ -z "$group_name" ]; then
            addgroup -g "$HOST_GID" buildergroup >/dev/null
            group_name=buildergroup
        fi

        if ! id -u builder >/dev/null 2>&1; then
            adduser -D -u "$HOST_UID" -G "$group_name" builder >/dev/null
        fi
        addgroup builder abuild >/dev/null 2>&1 || true
        mkdir -p /etc/sudoers.d
        printf "builder ALL=(ALL) NOPASSWD: ALL\n" >/etc/sudoers.d/builder
        chmod 440 /etc/sudoers.d/builder

        su builder -c "
            mkdir -p ~/.abuild &&
            yes | abuild-keygen -a -i -n >/dev/null 2>&1 &&
            cd /package &&
            if [ \"$ICEY_ALPINE_SOURCE_MODE\" = release ]; then
                before=\$(sha256sum APKBUILD | cut -d\" \" -f1) &&
                abuild checksum &&
                after=\$(sha256sum APKBUILD | cut -d\" \" -f1) &&
                if [ \"\$before\" != \"\$after\" ]; then
                    git -C /workspace diff -- packaging/alpine/APKBUILD
                    exit 1
                fi
            else
                abuild checksum >/dev/null
            fi &&
            abuild -r
        "
    '
