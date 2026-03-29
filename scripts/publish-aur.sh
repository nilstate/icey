#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
AUR_SRC_DIR="${AUR_SRC_DIR:-$ROOT_DIR/packaging/arch}"
AUR_REPO_DIR="${AUR_REPO_DIR:?Set AUR_REPO_DIR to a checked-out AUR package repository}"

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
