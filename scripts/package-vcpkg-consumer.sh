#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cmake_bin=${CMAKE:-cmake}
vcpkg_bin=${VCPKG:-vcpkg}
build_type=${BUILD_TYPE:-Release}
consumer_build_dir=${ICEY_VCPKG_CONSUMER_BUILD_DIR:-"$repo_root/build/package/vcpkg-consumer"}
install_root=${ICEY_VCPKG_INSTALL_ROOT:-"$repo_root/build/package/vcpkg-installed"}
canonical_overlay_ports="$repo_root/packaging/vcpkg"
overlay_ports="$canonical_overlay_ports"

if ! command -v "$vcpkg_bin" >/dev/null 2>&1 && [[ ! -x "$vcpkg_bin" ]]; then
    echo "vcpkg executable not found: $vcpkg_bin" >&2
    exit 1
fi

if [[ -z "${VCPKG_ROOT:-}" ]]; then
    if [[ "$vcpkg_bin" == */* ]]; then
        vcpkg_path=$vcpkg_bin
    else
        vcpkg_path=$(command -v "$vcpkg_bin")
    fi
    export VCPKG_ROOT=$(cd "$(dirname "$vcpkg_path")" && pwd)
fi

toolchain_file="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
if [[ ! -f "$toolchain_file" ]]; then
    echo "vcpkg toolchain file not found: $toolchain_file" >&2
    exit 1
fi

if [[ -z "${ICEY_VCPKG_SOURCE_PATH+x}" ]]; then
    export ICEY_VCPKG_SOURCE_PATH="$repo_root"
fi

if [[ -n "${ICEY_VCPKG_SOURCE_PATH:-}" ]]; then
    if [[ ! -d "$ICEY_VCPKG_SOURCE_PATH" ]]; then
        echo "local icey source tree not found: $ICEY_VCPKG_SOURCE_PATH" >&2
        exit 1
    fi

    local_source_path=$(cd "$ICEY_VCPKG_SOURCE_PATH" && pwd)
    overlay_root=${ICEY_VCPKG_OVERLAY_DIR:-"$repo_root/build/package/vcpkg-overlay"}
    overlay_port_dir="$overlay_root/icey"

    rm -rf "$overlay_root"
    mkdir -p "$overlay_port_dir"
    cp -R "$canonical_overlay_ports/icey/." "$overlay_port_dir/"

    python3 - <<'PY' "$overlay_port_dir/portfile.cmake" "$local_source_path"
from pathlib import Path
import sys

portfile = Path(sys.argv[1])
source_path = Path(sys.argv[2]).resolve().as_posix()
text = portfile.read_text()
start = text.find("vcpkg_from_github(")
end = text.find("\n\nvcpkg_check_features(", start)

if start == -1 or end == -1:
    raise SystemExit("failed to locate vcpkg_from_github block in portfile.cmake")

replacement = (
    f"set(SOURCE_PATH [[{source_path}]])\n"
    "message(STATUS \"Using local icey source tree: ${SOURCE_PATH}\")"
)
portfile.write_text(text[:start] + replacement + text[end:])
PY

    overlay_ports="$overlay_root"
fi

if [[ -z "${VCPKG_TARGET_TRIPLET:-}" ]]; then
    case "$(uname -s)-$(uname -m)" in
        Linux-x86_64) export VCPKG_TARGET_TRIPLET=x64-linux ;;
        Darwin-arm64) export VCPKG_TARGET_TRIPLET=arm64-osx ;;
        Darwin-x86_64) export VCPKG_TARGET_TRIPLET=x64-osx ;;
        MINGW64_NT-*-x86_64|MSYS_NT-*-x86_64|CYGWIN_NT-*-x86_64) export VCPKG_TARGET_TRIPLET=x64-windows ;;
        *)
            echo "Unsupported host for default vcpkg triplet: $(uname -s)-$(uname -m)" >&2
            exit 1
            ;;
    esac
fi

rm -rf "$consumer_build_dir" "$install_root"
mkdir -p "$consumer_build_dir" "$install_root"

"$vcpkg_bin" install icey \
    --overlay-ports="$overlay_ports" \
    --triplet="$VCPKG_TARGET_TRIPLET" \
    --x-install-root="$install_root"

"$cmake_bin" \
    -S "$repo_root/tests/consumer" \
    -B "$consumer_build_dir" \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DCMAKE_TOOLCHAIN_FILE="$toolchain_file" \
    -DVCPKG_TARGET_TRIPLET="$VCPKG_TARGET_TRIPLET" \
    -DVCPKG_INSTALLED_DIR="$install_root"

"$cmake_bin" --build "$consumer_build_dir" --config "$build_type"

consumer_exe="$consumer_build_dir/icey_consumer"
case "$VCPKG_TARGET_TRIPLET" in
    *-windows)
        consumer_exe="$consumer_build_dir/$build_type/icey_consumer.exe"
        ;;
esac

"$consumer_exe"
