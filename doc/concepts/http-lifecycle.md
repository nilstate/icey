# HTTP Lifecycle

icey's HTTP layer is fast because it is not trying to hide what the connection is doing.

There is a parser, a connection, an adapter, a request, a response, and a few very specific state transitions. Once you understand those, the rest of the module reads cleanly.

This page is about that flow.

## Two Verbs, Two Different Jobs

The first thing to get straight is this:

- `submit()` starts an outgoing HTTP transaction
- `send()` writes bytes on a live connection

That split is intentional.

If you find yourself trying to use `send()` to start an HTTP client request, you are using the wrong API.

## The Main Pieces

At the center of the module is `http::Connection`.

It owns:

- the underlying `TCPSocket` or `SSLSocket`
- a `ConnectionAdapter`
- a `Request`
- a `Response`

The parser lives inside the adapter, not inside the connection itself.

The adapter sits between the socket and the connection. On a plain HTTP connection it owns the llhttp parser and turns transport events into:

- headers complete
- payload chunk
- message complete
- parser error

On upgrade, the adapter can be replaced. That is how WebSocket takes over without rebuilding the connection object from scratch.

## Server Flow

The server side has one important idea now: the connection state is explicit.

`ServerConnection` moves through these states:

| State | Meaning |
| --- | --- |
| `ReceivingHeaders` | request line and headers are being parsed |
| `ReceivingBody` | request body is still arriving |
| `DispatchingOrSending` | request is complete and the responder or handler is producing a response |
| `Streaming` | response is intentionally long-lived |
| `Upgraded` | HTTP is over; another protocol now owns the transport |
| `Closing` | shutdown has started |
| `Closed` | done |

That state machine exists because the older boolean-inference model stops holding once you have keep-alive reuse, streaming, and protocol upgrade in the same library.

## A Plain Request/Response

The normal server request looks like this:

```text
TCP accept
  -> parser reads request line and headers
  -> onHeaders()
  -> optional body chunks via onPayload()
  -> onComplete()
  -> handler or responder writes response
  -> keep alive or close
```

In code, `srv.Connection` fires once the request is ready to handle.

That is why the basic server example looks simple: by the time your handler runs, the connection is already past header parsing and is in the dispatch/send phase.

## Request Bodies

Bodies do not change the model much, but they do change timing.

If the request has a body:

- headers arrive first
- payload chunks arrive through `Payload`
- completion only happens after the last chunk

The important rule is the same one from `net`:

- the `MutableBuffer` for payload is borrowed for that callback only

If you need to keep it, copy it or move it across a retained boundary immediately.

## Keep-Alive Reuse

A reusable HTTP server connection does not die after one request. It gets reset and goes back to `ReceivingHeaders`.

That reset matters for correctness:

- request state is cleared
- response state is cleared
- header auto-send state is reset
- idle timing is refreshed

This is one of those areas where "almost reset" produces very ugly bugs. The current server code is strict about returning pooled connections to a real clean parse state.

## Streaming Is Not Just "A Slow Response"

icey treats long-lived streaming responses as a different kind of connection state.

That matters because they should not be reaped like ordinary idle keep-alive sockets.

Use streaming mode when the response is intentionally open:

- chunked event streams
- multipart replace streams
- other producer-driven long-lived responses

Once a server connection enters `Streaming`, the idle reaper backs off. When the stream ends, the connection transitions back into normal HTTP lifecycle rules or closes.

That is very different from simply forgetting to close a one-shot response.

## Upgrade To WebSocket

WebSocket is the cleanest example of why the adapter model exists.

The flow is:

```text
HTTP request with Upgrade headers
  -> HTTP connection validates the upgrade
  -> replaceAdapter(ws::ConnectionAdapter)
  -> send 101 Switching Protocols
  -> mark connection as Upgraded
  -> frame-based I/O takes over
```

After that point, the connection is not "an HTTP connection that also does WebSocket." It is upgraded transport running through the WebSocket adapter on the same underlying socket.

That distinction matters for:

- close semantics
- idle timeout behavior
- payload framing
- partial frame buffering

It is also why upgrade tail bytes had to be handled carefully. If HTTP parsing stops at the handshake boundary but there are already WebSocket bytes in the same TCP read, those bytes still need to make it into the new adapter.

## Client Flow

Client-side HTTP is simpler once you keep the verbs straight.

The flow is:

```text
build URL and request
  -> submit()
  -> connect socket if needed
  -> write request line, headers, optional body
  -> receive response headers
  -> receive payload chunks
  -> complete or close
```

`ClientConnection::submit()` is the transaction entry point.

Then:

- `Headers` fires when response headers are ready
- `Payload` fires for each body chunk
- `Complete` fires once the whole response is received
- `Close` fires when the connection closes

For WebSocket client connections, `Connect` means the WebSocket handshake is complete, not just that the TCP socket connected.

That is the correct contract. Anything else would make the API ambiguous.

## `sendHeader()` and Header Auto-Send

The server and the connection adapter can send headers for you, but this is still explicit enough to reason about.

- `sendHeader()` writes the current outgoing header
- normal connection send paths can auto-send the header first when that is the right thing to do
- `headerAutoSendEnabled` controls that behavior for the next outgoing path

You do not need to micromanage it in the common cases, but you also do not have to guess what the connection is doing.

## Ownership Rules That Matter In Practice

### Incoming payload is borrowed

That applies to:

- HTTP body chunks
- client response payload chunks
- transport receive buffers under the hood

### Plain `send()` is the fast path

Use it when the payload naturally lives long enough.

### `sendOwned()` is for assembled temporary output

Use it for:

- generated headers
- WebSocket frames
- other payloads assembled in temporary storage

This is the same performance contract as the lower `net` layer, just applied to HTTP and upgrade paths.

## The Point Of This Design

The HTTP module does not try to pretend everything is one high-level request abstraction.

It keeps three things explicit:

- transport
- parsing
- message lifecycle

That is why the server can handle:

- normal request/response
- keep-alive reuse
- streaming responses
- WebSocket upgrade

without turning into a giant pile of special cases.

## Where To Go Next

- [HTTP](../modules/http.md) for the full module guide
- [Runtime Contracts](runtime-contracts.md) for loop, buffer, and send ownership rules
- [HTTP performance harness](../../src/http/perf/README.md) if you care about cross-stack throughput runs
- [HTTP microbenchmarks](../../src/http/bench/) if you want reportable hot-path numbers
- [wsclient sample](../../src/http/samples/wsclient/) for the standalone WebSocket client path
