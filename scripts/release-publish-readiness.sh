#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root_dir"
version="$(tr -d '[:space:]' < VERSION)"

# Main and pull-request runs validate that all derived metadata still comes
# from VERSION. Only the immutable version tag may publish packages.
if [[ "${GITHUB_REF_TYPE:-}" != "tag" ]]; then
  bash ./scripts/release-sync.sh "$version"
  git diff --exit-code -- VERSION Doxyfile CHANGELOG.md README.md docs llms.txt packaging || {
    echo "Release metadata has drifted from VERSION=$version" >&2
    exit 1
  }
  [[ -z "${GITHUB_OUTPUT:-}" ]] || echo "ready=false" >> "$GITHUB_OUTPUT"
  exit 0
fi

[[ "${GITHUB_REF_NAME:-}" == "$version" ]] || {
  echo "Tag ${GITHUB_REF_NAME:-<missing>} does not match VERSION=$version" >&2
  exit 1
}
bash ./scripts/validate-release-tag.sh "$version"
make release-finalize VERSION="$version"
[[ -z "${GITHUB_OUTPUT:-}" ]] || echo "ready=true" >> "$GITHUB_OUTPUT"
echo "Release $version is ready for package publication."
