#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FORMULA_SRC_DIR="${FORMULA_SRC_DIR:-$ROOT_DIR/packaging/homebrew/Formula}"
TAP_REPO_DIR="${TAP_REPO_DIR:?Set TAP_REPO_DIR to a checked-out Homebrew tap repository}"

for formula in icey.rb libdatachannel.rb; do
  src="$FORMULA_SRC_DIR/$formula"
  dst="$TAP_REPO_DIR/Formula/$formula"

  if [[ ! -f "$src" ]]; then
    echo "Homebrew formula missing: $src" >&2
    exit 1
  fi

  install -d "$(dirname "$dst")"
  cp "$src" "$dst"
  echo "Published Homebrew formula to $dst"
done
