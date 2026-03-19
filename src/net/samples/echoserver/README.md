# Echo Server

Demonstrates the Icey networking layer by running three echo servers simultaneously: TCP, SSL/TLS, and UDP. Each server echoes back any data it receives from connected clients.

The TCP and SSL servers use the templated `EchoServer<SocketT>` class (see `tcpechoserver.h`) which accepts connections and installs a `SocketAdapter` that echoes received data. The UDP server (see `udpechoserver.h`) binds a single socket and echoes datagrams back to the sender's address.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Usage

```
./echoserver <key.pem> <cert.pem>
```

The key and certificate files are required for the SSL server. The servers listen on:

- **TCP** - port 1337
- **SSL** - port 1338
- **UDP** - port 1339

Send `SIGINT` (Ctrl+C) to shut down.

Test with: `echo "hello" | nc localhost 1337`
