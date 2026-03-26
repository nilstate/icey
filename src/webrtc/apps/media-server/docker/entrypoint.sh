#!/bin/sh
set -eu

mode="${ICEY_MODE:-stream}"

set -- /usr/local/bin/media-server \
  --port "${ICEY_PORT:-4500}" \
  --turn-port "${ICEY_TURN_PORT:-3478}" \
  --mode "${mode}" \
  --web-root "${ICEY_WEB_ROOT:-/app/web}"

case "${mode}" in
  stream)
    set -- "$@" --source "${ICEY_SOURCE:-/app/media/test.mp4}"
    ;;
  record)
    set -- "$@" --record-dir "${ICEY_RECORD_DIR:-/app/recordings}"
    ;;
  relay)
    ;;
  *)
    echo "Unsupported ICEY_MODE: ${mode}" >&2
    exit 1
    ;;
esac

if [ "${ICEY_DISABLE_TURN:-0}" = "1" ]; then
  set -- "$@" --no-turn
fi

exec "$@"
