#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 2 ]; then
    echo "usage: $0 <source.changes> <ppa-target>" >&2
    exit 1
fi

changes_file="$1"
ppa_target="$2"
signing_key="${PPA_SIGNING_KEY_ID:?Set PPA_SIGNING_KEY_ID to the GPG key id used for Launchpad uploads}"

if [[ ! -f "$changes_file" ]]; then
    echo "Debian source changes file not found: $changes_file" >&2
    exit 1
fi

if ! command -v debsign >/dev/null 2>&1; then
    echo "debsign is required to sign the Debian source upload" >&2
    exit 1
fi

if ! command -v dput >/dev/null 2>&1; then
    echo "dput is required to publish to Launchpad" >&2
    exit 1
fi

debsign_args=(--re-sign "-k${signing_key}")
if [[ -n "${PPA_GPG_PASSPHRASE:-}" ]]; then
    wrapper="$(mktemp)"
    passphrase_escaped=$(printf '%q' "$PPA_GPG_PASSPHRASE")
    cat >"$wrapper" <<EOF
#!/usr/bin/env bash
exec gpg --batch --yes --pinentry-mode loopback --passphrase ${passphrase_escaped} "\$@"
EOF
    chmod 700 "$wrapper"
    trap 'rm -f "$wrapper"' EXIT
    debsign_args+=("-p$wrapper")
fi

debsign "${debsign_args[@]}" "$changes_file"
dput "$ppa_target" "$changes_file"
