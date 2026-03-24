# Symple Console

Interactive console client for the Symple real-time messaging protocol. Connects to a Symple server over WebSocket and provides a command-line interface for sending messages, joining/leaving rooms, and listing online contacts.

User input is read on a separate thread and dispatched to the libuv event loop via an IPC sync queue, keeping the networking thread non-blocking.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake. Requires the `symple` module.

## Usage

```
./sympleconsole -host localhost -port 4500 -token <token> -user <userid> -name <displayname>
```

Options:
- `-host` - Symple server hostname or IP
- `-port` - Symple server port
- `-token` - authentication token
- `-user` - user ID to register with
- `-name` - display name
- `-type` - user type (optional)
- `-help` - print help

## Quick Local Flow

1. start a Symple server
2. run one console as `alice`
3. run another as `bob`
4. join the same room from both clients
5. send direct and room messages from the interactive prompt

That is enough to prove:

- auth and presence
- room joins and leaves
- direct messaging
- room fanout

Interactive commands once connected:
- `M` - compose and send a message
- `J` - join a room
- `L` - leave a room
- `C` - list online contacts
- `Q` - quit

Set `USE_SSL` to `1` in the source to use SSL/TLS transport.

## See Also

- [Symple guide](../../../../doc/modules/symple.md)
- [WebRTC guide](../../../../doc/modules/webrtc.md) if you are using Symple for call signalling
