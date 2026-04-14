#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo_root"

tag="${1:-}"
version="$(tr -d '[:space:]' < VERSION)"

if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
  echo "VERSION must contain a plain semantic version, got: $version" >&2
  exit 1
fi

head_commit="$(git rev-parse HEAD)"

if [[ -n "$tag" ]]; then
  case "$tag" in
    [0-9]*.[0-9]*.[0-9]*) ;;
    *)
      echo "expected a plain semantic version tag, got: $tag" >&2
      exit 1
      ;;
  esac

  if [[ "$version" != "$tag" ]]; then
    echo "VERSION contains $version, expected $tag" >&2
    exit 1
  fi

  if [[ -n "${GITHUB_EVENT_PATH:-}" && -f "${GITHUB_EVENT_PATH:-}" ]]; then
    forced="$(
      python3 - "$GITHUB_EVENT_PATH" <<'PY'
import json
import sys

with open(sys.argv[1], "r", encoding="utf-8") as fh:
    event = json.load(fh)

print("true" if event.get("forced") else "false")
PY
    )"
    if [[ "$forced" == "true" ]]; then
      echo "refusing force-updated release tag $tag; semver release tags must be immutable" >&2
      exit 1
    fi
  fi

  eval "$(
    RELEASE_REQUIRE_REMOTE_TAG=1 \
    bash "$repo_root"/scripts/release-manifest.sh "$version"
  )"

  if [[ -n "$RELEASE_LOCAL_TAG_COMMIT" && "$RELEASE_LOCAL_TAG_COMMIT" != "$head_commit" ]]; then
    echo "local tag $tag points at $RELEASE_LOCAL_TAG_COMMIT, expected $head_commit" >&2
    exit 1
  fi

  if [[ "$RELEASE_REMOTE_TAG_COMMIT" != "$head_commit" ]]; then
    echo "remote tag $tag points at $RELEASE_REMOTE_TAG_COMMIT, expected $head_commit" >&2
    exit 1
  fi

  git fetch origin main --no-tags
  git merge-base --is-ancestor "$head_commit" origin/main || {
    echo "release tags must point to commits reachable from origin/main" >&2
    exit 1
  }
fi

grep -Eq "^## \\[$version\\]" CHANGELOG.md || {
  echo "CHANGELOG.md is missing [$version]" >&2
  exit 1
}

section="$(
  awk '/^## \['"$version"'\]/{found=1; next} /^## \[/{if(found) exit} found{print}' CHANGELOG.md
)"
printf '%s\n' "$section" | grep -Eq '[^[:space:]]' || {
  echo "CHANGELOG.md [$version] section is empty" >&2
  exit 1
}

echo "release metadata is valid for $version"
