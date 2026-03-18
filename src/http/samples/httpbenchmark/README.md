# HTTP Benchmark

Benchmarks LibSourcey's HTTP server against Node.js and Go using [wrk](https://github.com/wg/wrk).

## Variants

**LibSourcey:**
- `single` - single-threaded benchmark server
- `multi` - one server per CPU core with SO_REUSEPORT kernel load balancing
- `echo` - single-threaded echo server

**Node.js:**
- `minimal` - single-process minimal response
- `echo` - single-process echo
- `cluster` - one worker per CPU core

**Go** (optional, included if `go` is on PATH):

- `minimal` - single-process minimal response (goroutine-per-connection)
- `echo` - single-process echo

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SAMPLES=ON -DENABLE_LOGGING=OFF
cmake --build build --target httpbenchmark -j$(nproc)
```

## Run

```bash
# Automated comparison (runs all variants + wrk)
./src/http/samples/httpbenchmark/benchmark.sh build/bin/httpbenchmark

# Manual (run server then benchmark separately)
build/bin/httpbenchmark single &
wrk -t4 -c100 -d10s http://localhost:1337/
```

## Prerequisites

- `wrk`: `sudo apt install wrk`
- `node`: for Node.js comparison
- `go` (optional): for Go comparison
- Build in Release mode with logging disabled for accurate results

## Results

```text
System: Linux 6.8.0-106-generic x86_64
CPU:    11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz
Cores:  1
Node:   v20.19.5
wrk:    wrk 4.1.0 [epoll]

wrk -t4 -c100 -d10s --timeout 5s http://localhost:1337/
All servers return an empty 200 with Connection: close.

Server                                 Req/sec     Avg Latency
------------------------------  --------------  --------------
Raw libuv+llhttp (baseline)          19,329.44         4.59ms
LibSourcey (single-core)             13,513.67         8.14ms
LibSourcey (multi-core)              14,222.61         7.72ms
Node.js (single)                      8,777.72        11.44ms
Node.js (cluster)                     5,037.59        20.08ms
```

The raw libuv + llhttp baseline is a hand-rolled C server with zero abstractions (no connection management, no header building, no dispatch), representing the theoretical maximum for this stack. LibSourcey's HTTP layer retains ~70% of that throughput. Node.js uses the same underlying primitives (libuv, llhttp) and reaches ~45%.

Node.js cluster performs worse than single on 1 core due to IPC overhead. On a multi-core system, both `multi` and `cluster` modes scale with core count via SO_REUSEPORT.
