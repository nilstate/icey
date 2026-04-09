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

  git fetch origin main --no-tags
  commit_ref="${GITHUB_SHA:-HEAD}"
  git merge-base --is-ancestor "$commit_ref" origin/main || {
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
