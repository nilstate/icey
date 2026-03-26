# HTTP Performance Harness

Benchmarks Icey's HTTP server against Node.js and Go using [wrk](https://github.com/wg/wrk).

This directory contains the comparative workload harness. The reportable microbenchmarks live under [`../bench/`](../bench/).

## Variants

**Icey:**

- `single` - single-threaded, Connection: close
- `keepalive` - single-threaded, HTTP/1.1 keep-alive
- `multi` - one server per CPU core with SO_REUSEPORT kernel load balancing
- `echo` - single-threaded echo server

**Raw libuv+llhttp (baseline):**

- `raw` - hand-rolled C server, Connection: close (theoretical maximum)
- `raw-keepalive` - same, with keep-alive

**Node.js:**

- `minimal` - single-process, Connection: close
- `keepalive` - single-process, keep-alive
- `echo` - single-process echo
- `cluster` - one worker per CPU core

**Go** (optional, included if `go` is on PATH):

- `minimal` - single-process, Connection: close
- `keepalive` - single-process, keep-alive (goroutine-per-connection)
- `echo` - single-process echo

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PERF=ON -DENABLE_LOGGING=OFF
cmake --build build --target httpperf -j$(nproc)
```

## Run

```bash
# Automated comparison (runs all variants + wrk)
./src/http/perf/benchmark.sh build/http/perf/httpperf

# Or through CMake
cmake --build build --target httpperf_compare

# Manual (run server then benchmark separately)
build/http/perf/httpperf keepalive &
wrk -t4 -c100 -d10s http://localhost:1337/
```

For reportable microbenchmarks, build `BUILD_BENCHMARKS=ON` and run:

```bash
cmake --build build --target httpbench httpparsebench wsbench
./build/http/bench/httpbench --json
```

## Prerequisites

- `wrk`: `sudo apt install wrk`
- `node`: for Node.js comparison
- `go` (optional): for Go comparison
- Build in Release mode with logging disabled for accurate results

## Methodology

The benchmark script runs each server variant with `wrk -t4 -c100 -d10s` (4 threads, 100 connections, 10 seconds). All servers return an empty 200 OK response. `wrk` uses HTTP/1.1 keep-alive by default.

**Connection: close** variants measure per-connection overhead: TCP accept, socket creation, adapter wiring, HTTP parsing, response formatting, and teardown on every request.

**Keep-alive** variants measure per-request overhead only: HTTP parsing, handler dispatch, response formatting, and socket write. Connection setup is amortised across many requests on the same TCP connection. This is the realistic production scenario for most HTTP APIs.

The raw libuv+llhttp baseline is a hand-rolled C server with zero abstractions (no connection management, no header building, no dispatch), representing the theoretical maximum for this I/O stack.

## Results

```text
System: Linux 6.8.0-106-generic x86_64
CPU:    11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz
Cores:  1 vCPU
Node:   v20.19.5
Go:     go1.25.0
wrk:    wrk 4.1.0 [epoll]

wrk -t4 -c100 -d10s --timeout 5s http://localhost:1337/

Connection: close (per-connection overhead)
Server                                 Req/sec     Avg Latency
------------------------------  --------------  --------------
Raw libuv+llhttp (baseline)          18,468          4.88ms
Icey (single-core)             14,330          8.19ms
Go 1.25 (single)                     14,723          6.28ms
Node.js v20 (single)                  9,309         10.71ms

HTTP/1.1 keep-alive (per-request overhead)
Server                                 Req/sec     Avg Latency
------------------------------  --------------  --------------
Raw libuv+llhttp (baseline)          96,088          1.04ms
Icey (keep-alive)              72,209          1.43ms
Go 1.25 (keep-alive)                53,878          2.31ms
Node.js v20 (keep-alive)            45,514          3.56ms
```

With Connection: close, Icey matches Go at ~14,300 req/s (77% of raw libuv). Both pay the same per-connection overhead; the gap to raw libuv is TCP accept, socket allocation, HTTP parsing into data structures, and signal dispatch.

With keep-alive, Icey pulls ahead of Go by 34% (72,209 vs 53,878). Connection setup is amortised; the per-request cost is just parse + format + write. Icey's event loop has less overhead than Go's goroutine scheduler and garbage collector on this tight loop.
