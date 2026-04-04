#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

if ! command -v docker >/dev/null 2>&1; then
    echo "docker is required to validate the Fedora SRPM build" >&2
    exit 1
fi

fedora_image="${FEDORA_IMAGE:-fedora:41}"
host_uid="${HOST_UID:-$(id -u)}"
host_gid="${HOST_GID:-$(id -g)}"

docker run --rm \
    -e HOST_UID="$host_uid" \
    -e HOST_GID="$host_gid" \
    -e CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-1}" \
    -v "$repo_root:/workspace" \
    -w /workspace \
    "$fedora_image" \
    bash -lc '
        set -euo pipefail
        dnf install -y make rpm-build curl shadow-utils sudo >/dev/null

        group_name=$(awk -F: -v gid="$HOST_GID" "\$3 == gid { print \$1; exit }" /etc/group)
        if [ -z "$group_name" ]; then
            groupadd -g "$HOST_GID" buildergroup
            group_name=buildergroup
        fi

        if ! id -u builder >/dev/null 2>&1; then
            useradd -m -u "$HOST_UID" -g "$group_name" builder
        fi

        sudo -u builder make package-rpm-srpm
    '
