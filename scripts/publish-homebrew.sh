#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TAP_REPO_DIR="${TAP_REPO_DIR:?Set TAP_REPO_DIR to a checked-out Homebrew tap repository}"
if [[ -z "${FORMULA_SRC_DIR:-}" ]]; then
  render_root=$(bash "$ROOT_DIR/scripts/release-render.sh")
  FORMULA_SRC_DIR="$render_root/packaging/homebrew/Formula"
fi
if grep -R -q '@RELEASE_' "$FORMULA_SRC_DIR"; then
  echo "Homebrew formula source still contains release template fields" >&2
  exit 1
fi

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
