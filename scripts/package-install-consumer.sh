#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cmake_bin=${CMAKE:-cmake}
build_type=${BUILD_TYPE:-Release}
work_dir=${ICEY_INSTALL_CONSUMER_WORK_DIR:-"$repo_root/build/package/install-consumer"}

rm -rf "$work_dir"
mkdir -p "$work_dir"

cmake_args=(
    -DICEY_SOURCE_DIR="$repo_root"
    -DICEY_BINARY_DIR="$work_dir"
    -DICEY_CONSUMER_PROJECT_DIR="$repo_root/tests/consumer"
    -DICEY_BUILD_TYPE="$build_type"
)

if [[ -n "${CC:-}" ]]; then
    cmake_args+=(-DICEY_C_COMPILER="$CC")
fi

if [[ -n "${CXX:-}" ]]; then
    cmake_args+=(-DICEY_CXX_COMPILER="$CXX")
fi

if [[ -n "${CFLAGS:-}" ]]; then
    cmake_args+=(-DICEY_C_FLAGS="$CFLAGS")
fi

if [[ -n "${CXXFLAGS:-}" ]]; then
    cmake_args+=(-DICEY_CXX_FLAGS="$CXXFLAGS")
fi

if [[ -n "${LDFLAGS:-}" ]]; then
    cmake_args+=(-DICEY_EXE_LINKER_FLAGS="$LDFLAGS")
fi

if [[ -n "${OPENSSL_ROOT_DIR:-}" ]]; then
    cmake_args+=(-DICEY_OPENSSL_ROOT_DIR="$OPENSSL_ROOT_DIR")
fi

if [[ -n "${CMAKE_GENERATOR:-}" ]]; then
    cmake_args+=(-DICEY_CMAKE_GENERATOR="$CMAKE_GENERATOR")
fi

if [[ -n "${CMAKE_GENERATOR_PLATFORM:-}" ]]; then
    cmake_args+=(-DICEY_CMAKE_GENERATOR_PLATFORM="$CMAKE_GENERATOR_PLATFORM")
fi

"$cmake_bin" "${cmake_args[@]}" -P "$repo_root/tests/cmake/installtree_consumer_test.cmake"
