#!/usr/bin/env bash
#
# Icey HTTP Benchmark
#
# Prerequisites:
#   - wrk (https://github.com/wg/wrk)
#   - node (for Node.js comparison)
#   - httpbench binary (built with -DBUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release)
#
# Usage:
#   ./benchmark.sh [path/to/httpbench]
#
# The script runs each server variant, benchmarks with wrk, then prints
# a comparison table.

set -euo pipefail

HTTPBENCH="${1:-./httpbench}"
PORT=1337
URL="http://localhost:${PORT}/"
WRK_DURATION="10s"
WRK_THREADS=4
WRK_CONNECTIONS=100
WRK_TIMEOUT="5s"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RESULTS=()

# Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

die() { echo -e "${RED}Error: $*${NC}" >&2; exit 1; }

command -v wrk >/dev/null 2>&1 || die "wrk not found. Install: sudo apt install wrk"
command -v node >/dev/null 2>&1 || die "node not found."
[[ -x "$HTTPBENCH" ]] || die "httpbench binary not found at: $HTTPBENCH\nBuild with: cmake --build build --target httpbench"
HAS_GO=false
GO_BIN=""
if command -v go >/dev/null 2>&1; then
    HAS_GO=true
    GO_BIN=$(mktemp)
    go build -o "$GO_BIN" "${SCRIPT_DIR}/go-server.go" || { HAS_GO=false; rm -f "$GO_BIN"; }
fi

wait_for_port() {
    local retries=30
    while ! curl -sf -o /dev/null "http://localhost:${PORT}/" 2>/dev/null; do
        retries=$((retries - 1))
        [[ $retries -le 0 ]] && die "Server did not start on port $PORT"
        sleep 0.1
    done
}

kill_server() {
    if [[ -n "${SERVER_PID:-}" ]]; then
        kill "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
        unset SERVER_PID
    fi
    # Also kill anything left on the port
    fuser -k "${PORT}/tcp" 2>/dev/null || true
    sleep 0.3
}

run_wrk() {
    local label="$1"
    echo -e "\n${CYAN}Benchmarking: ${BOLD}${label}${NC}"
    echo "  wrk -t${WRK_THREADS} -c${WRK_CONNECTIONS} -d${WRK_DURATION} --timeout ${WRK_TIMEOUT} ${URL}"

    local output
    output=$(wrk -t${WRK_THREADS} -c${WRK_CONNECTIONS} -d${WRK_DURATION} --timeout ${WRK_TIMEOUT} "${URL}" 2>&1)
    echo "$output"

    # Extract req/sec
    local rps
    rps=$(echo "$output" | grep "Requests/sec:" | awk '{print $2}')
    local latency
    latency=$(echo "$output" | grep "Latency" | awk '{print $2}')
    RESULTS+=("$label|$rps|$latency")
}

cleanup() { kill_server; $HAS_GO && rm -f "$GO_BIN"; }
trap cleanup EXIT

echo -e "${BOLD}============================================${NC}"
echo -e "${BOLD}  Icey HTTP Benchmark${NC}"
echo -e "${BOLD}============================================${NC}"
echo
echo "System: $(uname -srm)"
echo "CPU:    $(grep -m1 'model name' /proc/cpuinfo 2>/dev/null | cut -d: -f2 | xargs || sysctl -n machdep.cpu.brand_string 2>/dev/null)"
echo "Cores:  $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null)"
echo "Node:   $(node --version)"
$HAS_GO && echo "Go:     $(go version | awk '{print $3}')"
echo "wrk:    $(wrk --version 2>&1 | head -1)"
echo

# --- Raw libuv+llhttp baseline ---
kill_server
"$HTTPBENCH" raw &
SERVER_PID=$!
wait_for_port
run_wrk "Raw libuv+llhttp (baseline)"
kill_server

# --- Icey single-core ---
"$HTTPBENCH" single &
SERVER_PID=$!
wait_for_port
run_wrk "Icey (single-core)"
kill_server

# --- Icey multi-core ---
"$HTTPBENCH" multi &
SERVER_PID=$!
wait_for_port
run_wrk "Icey (multi-core)"
kill_server

# --- Node.js single ---
node "${SCRIPT_DIR}/node-server.js" minimal &
SERVER_PID=$!
wait_for_port
run_wrk "Node.js (single)"
kill_server

# --- Node.js cluster ---
node "${SCRIPT_DIR}/node-server.js" cluster &
SERVER_PID=$!
wait_for_port
run_wrk "Node.js (cluster)"
kill_server

# --- Go single ---
if $HAS_GO; then
    "$GO_BIN" minimal &
    SERVER_PID=$!
    wait_for_port
    run_wrk "Go (single)"
    kill_server
fi

# --- Keep-alive benchmarks ---
echo -e "\n${BOLD}--- Keep-Alive Benchmarks ---${NC}"

# --- Raw libuv+llhttp keep-alive ---
"$HTTPBENCH" raw-keepalive &
SERVER_PID=$!
wait_for_port
run_wrk "Raw libuv+llhttp (keep-alive)"
kill_server

# --- Icey keep-alive ---
"$HTTPBENCH" keepalive &
SERVER_PID=$!
wait_for_port
run_wrk "Icey (keep-alive)"
kill_server

# --- Node.js keep-alive ---
node "${SCRIPT_DIR}/node-server.js" keepalive &
SERVER_PID=$!
wait_for_port
run_wrk "Node.js (keep-alive)"
kill_server

# --- Go keep-alive ---
if $HAS_GO; then
    "$GO_BIN" keepalive &
    SERVER_PID=$!
    wait_for_port
    run_wrk "Go (keep-alive)"
    kill_server
fi

# --- Results table ---
echo
echo -e "${BOLD}============================================${NC}"
echo -e "${BOLD}  Results${NC}"
echo -e "${BOLD}============================================${NC}"
printf "\n${BOLD}%-30s %15s %15s${NC}\n" "Server" "Req/sec" "Avg Latency"
printf "%-30s %15s %15s\n" "------------------------------" "---------------" "---------------"
for r in "${RESULTS[@]}"; do
    IFS='|' read -r label rps latency <<< "$r"
    printf "%-30s %15s %15s\n" "$label" "$rps" "$latency"
done
echo
