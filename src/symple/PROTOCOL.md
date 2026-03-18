# Symple Protocol v4

> Real-time messaging and presence over WebSocket

## Overview

Symple is a lightweight protocol for peer presence, room-based messaging, and real-time communication over native WebSocket. It provides peer discovery, message routing, and presence tracking with minimal overhead.

v4 uses plain WebSocket with JSON messages. No Socket.IO, no Engine.IO, no polling fallbacks.

## Transport

- **WebSocket** (RFC 6455) over TCP
- **TLS** recommended for production (`wss://`)
- **Endpoint**: `ws://host:port/` or `wss://host:port/`
- **Keepalive**: native WebSocket ping/pong frames (no application-level heartbeat)
- **Encoding**: all messages are UTF-8 JSON text frames

## Connection Lifecycle

```
Client                              Server
  |                                    |
  |--- WebSocket connect ------------->|
  |                                    |
  |--- {"type":"auth", ...} --------->|  First message must be auth
  |                                    |  Server validates credentials
  |<-- {"type":"welcome", ...} -------|  Success: peer assigned, rooms joined
  |  or                                |
  |<-- {"type":"error", ...} ---------|  Failure: connection closed
  |                                    |
  |<-- presence broadcasts ----------->|  Peer online/offline notifications
  |<-- messages ---------------------->|  Routed messages between peers
  |                                    |
  |--- {"type":"close"} ------------->|  Graceful disconnect
  |--- WebSocket close --------------->|
```

## Message Format

Every message is a JSON object with a `type` field. All other fields depend on the type.

```json
{
  "type": "string",
  ...
}
```

## Message Types

### auth (client -> server)

First message after WebSocket connect. Must be sent before any other message. Server closes the connection if auth is not received within 10 seconds.

```json
{
  "type": "auth",
  "user": "alice",
  "name": "Alice",
  "token": "optional-auth-token",
  "data": {}
}
```

| Field   | Required | Description |
|---------|----------|-------------|
| `type`  | yes      | `"auth"` |
| `user`  | yes      | User identifier (used for addressing and room membership) |
| `name`  | no       | Display name (defaults to `user`) |
| `token` | no       | Authentication token (required if server has authentication enabled) |
| `data`  | no       | Arbitrary peer data (capabilities, status, etc.) |

### welcome (server -> client)

Sent after successful authentication. Contains the assigned peer object with the server-generated session ID.

```json
{
  "type": "welcome",
  "peer": {
    "id": "a1b2c3d4",
    "user": "alice",
    "name": "Alice",
    "online": true,
    "host": "192.168.1.100"
  },
  "status": 200
}
```

| Field    | Description |
|----------|-------------|
| `peer`   | The authenticated peer object |
| `peer.id`| Server-assigned session ID (unique per connection) |
| `status` | HTTP-style status code (200 = success) |

After receiving `welcome`, the client is automatically joined to a room named after their `user` field. This enables direct messaging by user identity.

### error (server -> client)

Sent on authentication failure or protocol error. Server closes the connection after sending this.

```json
{
  "type": "error",
  "status": 401,
  "message": "Authentication failed: invalid token"
}
```

### message (bidirectional, routed by server)

The core message type. Server routes based on the `to` field.

```json
{
  "type": "message",
  "id": "msg_abc123",
  "from": "alice|a1b2c3d4",
  "to": "bob|x9y8z7w6",
  "subtype": "call:offer",
  "data": {
    "sdp": "v=0\r\n..."
  }
}
```

| Field     | Required | Description |
|-----------|----------|-------------|
| `type`    | yes      | `"message"` |
| `id`      | yes      | Unique message ID (client-generated) |
| `from`    | yes      | Sender address (`user\|id` format, set by client) |
| `to`      | no       | Recipient address or room name. Omit for broadcast to all rooms. |
| `subtype` | no       | Application-defined subtype (e.g. `"call:offer"`, `"chat"`) |
| `data`    | no       | Message payload (any JSON value) |

**Routing rules:**

| `to` value | Behaviour |
|------------|-----------|
| `"user\|id"` | Direct to specific peer session |
| `"user"` | Direct to any session of that user |
| omitted | Broadcast to all rooms the sender has joined (excluding sender) |
| `["room1", "room2"]` | Broadcast to specific rooms |

### presence (bidirectional, routed by server)

Peer status updates. Server broadcasts to all rooms the peer has joined.

```json
{
  "type": "presence",
  "from": "alice|a1b2c3d4",
  "data": {
    "id": "a1b2c3d4",
    "user": "alice",
    "name": "Alice",
    "online": true,
    "status": "available"
  },
  "probe": false
}
```

| Field   | Required | Description |
|---------|----------|-------------|
| `type`  | yes      | `"presence"` |
| `from`  | yes      | Sender address |
| `to`    | no       | Specific recipient (for directed presence) |
| `data`  | yes      | Peer object with at least `id`, `user`, `online` |
| `probe` | no       | If `true`, requests the recipient to send their presence back |

**Automatic presence events:**
- Server broadcasts presence with `online: true` when a peer authenticates
- Server broadcasts presence with `online: false` when a peer disconnects
- Client can send presence probes to discover who's online

### command (bidirectional, routed by server)

Structured commands with node/action addressing.

```json
{
  "type": "command",
  "from": "alice|a1b2c3d4",
  "to": "bob|x9y8z7w6",
  "node": "media:video",
  "action": "start",
  "data": {}
}
```

### event (bidirectional, routed by server)

Named events with arbitrary data.

```json
{
  "type": "event",
  "from": "alice|a1b2c3d4",
  "to": "bob|x9y8z7w6",
  "name": "typing",
  "data": {}
}
```

### join (client -> server)

Join a room. The server confirms with a status response.

```json
{"type": "join", "room": "public"}
```

Server response:

```json
{"type": "join:ok", "room": "public"}
```

### leave (client -> server)

Leave a room.

```json
{"type": "leave", "room": "public"}
```

Server response:

```json
{"type": "leave:ok", "room": "public"}
```

### close (client -> server)

Graceful disconnect. Client should send this before closing the WebSocket.

```json
{"type": "close"}
```

## Addressing

Peers are addressed using the format `user|id`:

- `user` - the user identifier (from auth)
- `id` - the server-assigned session ID (from welcome)
- `user|id` - fully qualified address (routes to specific session)
- `user` alone - routes to any active session for that user

Examples:
- `"alice|a1b2c3d4"` - specific session
- `"alice"` - any session for user alice

## Rooms

Rooms are named groups of peers. Messages sent without a `to` field are broadcast to all rooms the sender has joined.

- Every peer automatically joins a room named after their `user` field
- Peers can join additional rooms via `join` messages
- Room names are arbitrary strings
- Server may restrict room access via configuration (`dynamicRooms` setting)

## Authentication

Two modes:

**Anonymous** (default): client provides `user` field, server assigns a session. No token required.

**Token-based**: client provides `user` and `token` fields. Server validates the token against a session store (Redis or custom backend). Failed auth results in an `error` message and connection close.

## Server Configuration

| Setting | Default | Description |
|---------|---------|-------------|
| `port` | 4500 | Listen port |
| `authentication` | false | Require token authentication |
| `dynamicRooms` | true | Allow clients to join/leave rooms |
| `sessionTTL` | -1 | Session timeout in minutes (-1 = no expiry) |
| `redis` | undefined | Redis URL for session store and pub/sub scaling |
| `tls.cert` | undefined | TLS certificate path |
| `tls.key` | undefined | TLS private key path |

## Scaling

For horizontal scaling across multiple server processes, use Redis pub/sub:

- Each server process connects to the same Redis instance
- Messages addressed to peers on other processes are published to Redis
- Each process subscribes and delivers to its local connections
- Session data (tokens, TTLs) stored in Redis

Single-process deployments don't need Redis.

## Compatibility

v4 is a breaking change from v3:

| Feature | v3 (Socket.IO) | v4 (WebSocket) |
|---------|----------------|----------------|
| Transport | Socket.IO (Engine.IO + polling fallback) | Native WebSocket |
| Handshake | Socket.IO connect event + auth middleware | `auth` message as first frame |
| Heartbeat | Engine.IO ping/pong (application bytes) | WebSocket ping/pong (protocol frames) |
| Events | `socket.emit('message', data)` | `ws.send(JSON.stringify(data))` |
| Rooms | Socket.IO rooms (server-managed sets) | Server-managed sets (same semantics) |
| Binary | Not used | Not used |

The Symple message format (`type`, `from`, `to`, `data`) is unchanged. Client application code (CallManager, WebRTCPlayer, roster management) is unaffected; only the transport layer changes.
