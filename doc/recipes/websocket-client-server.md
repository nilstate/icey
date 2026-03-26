# WebSocket Client And Server

Use this when the job is not "serve HTTP" but "upgrade and speak frames."

icey's WebSocket support lives in the HTTP module for a reason. The handshake is HTTP. After that, the connection is upgraded and the frame adapter takes over. That is the whole model.

## The Shape

Client side:

```text
createConnectionT(ws:// or wss://)
  -> start()
  -> HTTP handshake
  -> Connect fires when the WebSocket handshake is complete
  -> send/receive framed payload
```

Server side:

```text
HTTP request with Upgrade: websocket
  -> replaceAdapter(ws::ConnectionAdapter)
  -> send 101 Switching Protocols
  -> connection enters Upgraded mode
  -> frame I/O from there on
```

If you need the underlying connection rules, read [HTTP Lifecycle](../concepts/http-lifecycle.md). This page is about the usable WebSocket path.

## Client

The current sample does exactly this:

```cpp
auto conn = http::createConnectionT<http::ClientConnection>(
    http::URL("ws://echo.websocket.events"));

conn->Connect += [&]() {
    conn->send("Hello!", 6, http::ws::Text);
};

conn->Payload += [](const MutableBuffer& buf) {
    std::string data(bufferCast<const char*>(buf), buf.size());
    std::cout << "Received: " << data << '\n';
};

conn->Close += [](http::Connection&) {
    uv_stop(uv::defaultLoop());
};

conn->start();
```

Two rules matter here:

- `start()` starts the handshake
- `Connect` fires after the WebSocket handshake succeeds, not after bare TCP connect

That second rule is important. It means application code does not race the handshake.

## Frame Types

Outgoing frame type is controlled by the `flags` argument to `send()`:

```cpp
conn->send(text.data(), text.size(), http::ws::Text);
conn->send(binary.data(), binary.size(), http::ws::Binary);
```

You are sending payload. The adapter handles framing.

## Server Upgrade

On the server side, upgrade is explicit:

```cpp
srv.Connection += [](http::ServerConnection::Ptr conn) {
    if (conn->request().get("Upgrade") == "websocket") {
        conn->replaceAdapter(
            std::make_unique<http::ws::ConnectionAdapter>(
                conn.get(), http::ws::ServerSide));
        return;
    }

    conn->response().setStatus(http::StatusCode::OK);
    conn->sendHeader();
    conn->close();
};
```

After that point:

- the connection stays upgraded until close
- HTTP keep-alive rules stop applying
- outgoing `send()` goes through the WebSocket adapter
- incoming `Payload` is application payload, not raw frame bytes

That last point is one of the main benefits of the adapter layer.

## Fragmentation And Partial Frames

The application does not have to reassemble WebSocket fragmentation by hand.

icey's framer:

- buffers partial frames split across TCP reads
- reassembles fragmented messages
- enforces the RFC 6455 control-frame rules
- applies masking rules automatically based on client or server mode

`Payload` receives complete application messages, not raw frame fragments.

That is how it should be. Fragmentation is transport detail, not application protocol.

## Ping, Pong, And Close

Ping and pong are handled inside the adapter.

- incoming `Ping` gets a `Pong`
- incoming `Pong` is consumed internally
- close frames drive the close handshake

If you want to close explicitly, call `shutdown()` on the adapter:

```cpp
auto* adapter = static_cast<http::ws::ConnectionAdapter*>(conn->adapter());
adapter->shutdown(
    static_cast<uint16_t>(http::ws::CloseStatusCode::NormalClose),
    "goodbye");
```

That is different from just tearing down the underlying TCP socket. It gives the peer a proper WebSocket close frame first.

## Client vs Server Responsibilities

icey handles the RFC rules that should not be application boilerplate:

- client payload masking
- server-side unmasked frame enforcement
- RSV and opcode validation
- control-frame length and FIN rules
- fragmentation reassembly

Application code still owns:

- what messages mean
- whether payload is text or binary
- when to close
- how to react to errors or disconnects

## Debugging Order

If a WebSocket path is not working, debug it in this order:

1. plain HTTP reachability
2. handshake success
3. upgrade on the server
4. framed send path
5. application payload handling

Do not start by debugging your JSON message layer if the upgrade never completed.

## Good Next Stops

- [HTTP guide](../modules/http.md) for the full HTTP and WebSocket surface
- [HTTP Lifecycle](../concepts/http-lifecycle.md) for upgrade and connection-state detail
- [HTTP Server](http-server.md) for the plain request/response path around it
- [`wsclient`](../../src/http/samples/wsclient/) for the runnable client sample
