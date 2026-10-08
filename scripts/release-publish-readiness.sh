#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root_dir"
version="$(tr -d '[:space:]' < VERSION)"

# The first main push prepares a version before its immutable source tag exists.
# That revision must not publish package recipes with placeholder hashes.
if [[ "${GITHUB_EVENT_NAME:-}" == "push" ]]; then
  if [[ -z "$(git ls-remote --refs --tags origin "refs/tags/$version")" ]] ||
     grep -Eq 'sha256: "0{64}"' packaging/conan/conandata.yml; then
    echo "Release $version is not finalized; package publication is deferred."
    [[ -z "${GITHUB_OUTPUT:-}" ]] || echo "ready=false" >> "$GITHUB_OUTPUT"
    exit 0
  fi
fi

bash ./scripts/validate-release-tag.sh
bash ./scripts/release-check.sh "$version"
[[ -z "${GITHUB_OUTPUT:-}" ]] || echo "ready=true" >> "$GITHUB_OUTPUT"
echo "Release $version is ready for package publication."
