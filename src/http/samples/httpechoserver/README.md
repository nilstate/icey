# HTTP Echo Server

Demonstrates the Icey HTTP server by echoing back the request body in the response. The header file (`httpechoserver.h`) contains several server variants:

- **raiseEchoServer()** - single-threaded HTTP echo
- **raiseHTTPSEchoServer()** - single-threaded HTTPS echo (used by default)
- **runMulticoreEchoServers()** - one HTTP server per CPU core using separate libuv loops
- **raiseBenchmarkServer()** - minimal response for HTTP benchmarking
- **runMulticoreBenchmarkServers()** - multicore variant of the benchmark server

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Usage

```
./httpechoserver <key.pem> <cert.pem>
```

The key and certificate files are required for the HTTPS server. Listens on port 1337.

On Linux, the libuv thread pool size is automatically scaled to match the number of CPU cores.

Send `SIGINT` (Ctrl+C) to shut down.

Test with: `curl -k -d "hello" https://localhost:1337/`
