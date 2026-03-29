#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
AUR_SRC_DIR="${AUR_SRC_DIR:-$ROOT_DIR/packaging/arch}"
AUR_REPO_DIR="${AUR_REPO_DIR:?Set AUR_REPO_DIR to a checked-out AUR package repository}"

pkg_release() {
  local file="$1"
  local pkgver
  local pkgrel
  pkgver="$(sed -n 's/^pkgver=\(.*\)$/\1/p' "$file" | head -n1)"
  pkgrel="$(sed -n 's/^pkgrel=\(.*\)$/\1/p' "$file" | head -n1)"

  if [[ -z "$pkgver" || -z "$pkgrel" ]]; then
    echo "failed to parse pkgver/pkgrel from $file" >&2
    exit 1
  fi

  printf '%s-%s\n' "$pkgver" "$pkgrel"
}

src_release="$(pkg_release "$AUR_SRC_DIR/PKGBUILD")"
if [[ -f "$AUR_REPO_DIR/PKGBUILD" ]]; then
  dst_release="$(pkg_release "$AUR_REPO_DIR/PKGBUILD")"
  latest_release="$(printf '%s\n%s\n' "$src_release" "$dst_release" | sort -V | tail -n1)"
  if [[ "$latest_release" != "$src_release" ]]; then
    echo "Skipping AUR publish: remote package release $dst_release is newer than source $src_release"
    exit 0
  fi
fi

for file in PKGBUILD .SRCINFO; do
  src="$AUR_SRC_DIR/$file"
  dst="$AUR_REPO_DIR/$file"

  if [[ ! -f "$src" ]]; then
    echo "AUR file missing: $src" >&2
    exit 1
  fi

  cp "$src" "$dst"
  echo "Published AUR package file to $dst"
done
