# HTTP Echo Server

Demonstrates the icey HTTP server by echoing back the request body in the response. The header file (`httpechoserver.h`) contains several server variants:

- **raiseEchoServer()** - single-threaded HTTP echo
- **raiseHTTPSEchoServer()** - single-threaded HTTPS echo (used by default)
- **runMulticoreEchoServers()** - one HTTP server per CPU core using separate libuv loops
- **raiseBenchmarkServer()** - minimal response for HTTP benchmarking
- **runMulticoreBenchmarkServers()** - multicore variant of the benchmark server

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## What This Sample Is Good For

- proving the plain HTTP server path works
- bringing up HTTPS quickly with a local certificate
- checking request body handling through `Payload`
- comparing the normal echo path with the stripped benchmark server path in the same source tree

## Usage

```
./httpechoserver <key.pem> <cert.pem>
```

The key and certificate files are required for the HTTPS server. Listens on port 1337.

On Linux, the libuv thread pool size is automatically scaled to match the number of CPU cores.

Send `SIGINT` (Ctrl+C) to shut down.

Test with: `curl -k -d "hello" https://localhost:1337/`

## Notes

- The default executable path is the HTTPS variant. If you want the plain HTTP or multicore variants, read `httpechoserver.h`.
- `raiseBenchmarkServer()` and `runMulticoreBenchmarkServers()` exist to exercise the lean server path. The cross-stack performance harness lives under [`src/http/perf/`](../../perf/README.md), and the reportable microbenchmarks live under [`src/http/bench/`](../../bench/).

## See Also

- [HTTP Server recipe](../../../../docs/recipes/http-server.md)
- [HTTP guide](../../../../docs/modules/http.md)
- [HTTP Lifecycle](../../../../docs/concepts/http-lifecycle.md)
