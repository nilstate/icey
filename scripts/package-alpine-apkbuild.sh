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

docker run --rm \
    -e HOST_UID="$host_uid" \
    -e HOST_GID="$host_gid" \
    -e CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-1}" \
    -v "$repo_root:/workspace" \
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
            cd /workspace/packaging/alpine &&
            before=\$(sha256sum APKBUILD | cut -d\" \" -f1) &&
            abuild checksum &&
            after=\$(sha256sum APKBUILD | cut -d\" \" -f1) &&
            if [ \"\$before\" != \"\$after\" ]; then
                git diff -- APKBUILD
                exit 1
            fi &&
            abuild -r
        "
    '
