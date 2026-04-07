{#symplemodule}

# symple

Realtime messaging protocol for presence, rooms, and WebRTC signalling.

### Namespaces

| Name | Description |
|------|-------------|
| [`smpl`](#smpl) | Symple protocol messages, peers, client, and server helpers. |

{#smpl}

# smpl

Symple protocol messages, peers, client, and server helpers.

### Classes

| Name | Description |
|------|-------------|
| [`Client`](#client-5) | Symple v4 client. |
| [`Command`](#command) | Symple command message with a node path and action verb. |
| [`Event`](#event-2) | Symple event message carrying a named occurrence with a timestamp. |
| [`Form`](#form) | Interactive form with pages, sections, and fields for command data exchange. |
| [`FormElement`](#formelement) | Base element within a Symple form. |
| [`FormField`](#formfield) | [Form](#form) input field with typed values and selectable options. |
| [`Message`](#message-10) | Base Symple protocol message with addressing, status, data, and notes. |
| [`Peer`](#peer-1) | Symple peer record containing identity, presence, and custom fields. |
| [`Presence`](#presence) | Symple presence message indicating a peer's online status. |
| [`Roster`](#roster-2) | The [Roster](#roster-2) provides a registry for active network peers indexed by session ID. |
| [`Server`](#server-9) | Symple v4 server. |
| [`ServerPeer`](#serverpeer) | Per-connection state for a connected Symple peer. |
| [`Address`](#address-13) | The [Address](#address-13) structure is an endpoint identifier for a peer on the network. The format is like so: user\|id |
| [`ClientState`](#clientstate-1) | [Client](#client-5) connection states. |

{#client-5}

## Client

```cpp
#include <icy/symple/client.h>
```

> **Inherits:** [`Stateful< ClientState >`](base.md#stateful), [`Signal< void(IPacket &)>`](base.md#signal)

Symple v4 client.

Connects to a Symple server over plain WebSocket (no Socket.IO). Handles authentication, presence, roster management, rooms, and message routing.

Usage: [smpl::Client](#client-5) client({ .host = "localhost", .port = 4500, .user = "alice", .name = "Alice" }); client.start();

client += packetSlot(&handler, &Handler::onMessage); client.Announce += slot(&handler, &Handler::onAnnounce);

Messages are emitted as polymorphic [IPacket](base.md#ipacket) via PacketSignal:

* [smpl::Message](#message-10) (type "message")

* [smpl::Presence](#presence) (type "presence")

* [smpl::Event](#event-2) (type "event")

* [smpl::Command](#command) (type "command")

Use [packetSlot()](base.md#packetslot) with the specific type to filter.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(const int &)>` | [`Announce`](#announce)  | Authentication response status (200 = success, 401 = failed). |
| `Signal< void(Peer &)>` | [`PeerConnected`](#peerconnected)  | A peer has come online. |
| `Signal< void(Peer &)>` | [`PeerDisconnected`](#peerdisconnected)  | A peer has gone offline. |
| `Signal< void(Peer &)>` | [`CreatePresence`](#createpresence)  | Modify the outgoing peer object before presence broadcast. |

---

{#announce}

#### Announce

```cpp
Signal< void(const int &)> Announce
```

Authentication response status (200 = success, 401 = failed).

---

{#peerconnected}

#### PeerConnected

```cpp
Signal< void(Peer &)> PeerConnected
```

A peer has come online.

---

{#peerdisconnected}

#### PeerDisconnected

```cpp
Signal< void(Peer &)> PeerDisconnected
```

A peer has gone offline.

---

{#createpresence}

#### CreatePresence

```cpp
Signal< void(Peer &)> CreatePresence
```

Modify the outgoing peer object before presence broadcast.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Client`](#client-6)  |  |
|  | [`Client`](#client-7)  | Default constructor with default [Options](#options-14). |
| `void` | [`start`](#start-16)  | Start the Symple client. |
| `void` | [`stop`](#stop-13)  | Stop the Symple client. |
| `ssize_t` | [`send`](#send-17) `virtual` | Send a Symple message. Sets the `from` field automatically. |
| `ssize_t` | [`send`](#send-18) `virtual` | Send a string message (parsed as JSON). |
| `ssize_t` | [`respond`](#respond-1) `virtual` | Swap to/from and send. |
| `ssize_t` | [`sendPresence`](#sendpresence) `virtual` | Broadcast presence to joined rooms. |
| `ssize_t` | [`sendPresence`](#sendpresence-1) `virtual` | Send directed presence to a specific peer. |
| `int` | [`joinRoom`](#joinroom) `virtual` | Join a room on the server. |
| `int` | [`leaveRoom`](#leaveroom) `virtual` | Leave a room on the server. |
| `bool` | [`isOnline`](#isonline) `const` | Return true if in Online state. |
| `std::string` | [`ourID`](#ourid) `const` | Return the session ID assigned by the server. |
| `Peer *` | [`ourPeer`](#ourpeer)  | Return the local peer object (null if offline). |
| `StringVec` | [`rooms`](#rooms) `const` | Return joined rooms. |
| `Roster &` | [`roster`](#roster)  | Return the roster of online peers. |
| `const Options &` | [`options`](#options-12) `const` | Return the current client options. |
| `void` | [`setOptions`](#setoptions)  | Replace the client options while the client is closed. This is the only supported way to reconfigure a default-constructed client. |
| `void` | [`setError`](#seterror-6)  | Set an error and transition to [Error](base.md#error) state. |

---

{#client-6}

#### Client

```cpp
Client(const Options & options, uv::Loop * loop)
```

---

{#client-7}

#### Client

```cpp
Client()
```

Default constructor with default [Options](#options-14).

---

{#start-16}

#### start

```cpp
void start()
```

Start the Symple client.

---

{#stop-13}

#### stop

```cpp
void stop()
```

Stop the Symple client.

---

{#send-17}

#### send

`virtual`

```cpp
virtual ssize_t send(Message & message)
```

Send a Symple message. Sets the `from` field automatically.

---

{#send-18}

#### send

`virtual`

```cpp
virtual ssize_t send(const std::string & message)
```

Send a string message (parsed as JSON).

---

{#respond-1}

#### respond

`virtual`

```cpp
virtual ssize_t respond(Message & message)
```

Swap to/from and send.

---

{#sendpresence}

#### sendPresence

`virtual`

```cpp
virtual ssize_t sendPresence(bool probe)
```

Broadcast presence to joined rooms.

---

{#sendpresence-1}

#### sendPresence

`virtual`

```cpp
virtual ssize_t sendPresence(const Address & to, bool probe)
```

Send directed presence to a specific peer.

---

{#joinroom}

#### joinRoom

`virtual`

```cpp
virtual int joinRoom(const std::string & room)
```

Join a room on the server.

---

{#leaveroom}

#### leaveRoom

`virtual`

```cpp
virtual int leaveRoom(const std::string & room)
```

Leave a room on the server.

---

{#isonline}

#### isOnline

`const`

```cpp
bool isOnline() const
```

Return true if in Online state.

---

{#ourid}

#### ourID

`const`

```cpp
std::string ourID() const
```

Return the session ID assigned by the server.

---

{#ourpeer}

#### ourPeer

```cpp
Peer * ourPeer()
```

Return the local peer object (null if offline).

---

{#rooms}

#### rooms

`const`

```cpp
StringVec rooms() const
```

Return joined rooms.

---

{#roster}

#### roster

```cpp
Roster & roster()
```

Return the roster of online peers.

---

{#options-12}

#### options

`const`

```cpp
const Options & options() const
```

Return the current client options.

---

{#setoptions}

#### setOptions

```cpp
void setOptions(Options options)
```

Replace the client options while the client is closed. This is the only supported way to reconfigure a default-constructed client. 
#### Exceptions
* `std::logic_error` if the client is currently connecting, online, or reconnecting.

---

{#seterror-6}

#### setError

```cpp
void setError(const std::string & error)
```

Set an error and transition to [Error](base.md#error) state.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`createPresence`](#createpresence-1) `virtual` | Underlying connection state changed. Inherits StateChange from [Stateful<ClientState>](base.md#stateful). |
| `void` | [`onPresenceData`](#onpresencedata) `virtual` |  |

---

{#createpresence-1}

#### createPresence

`virtual`

```cpp
virtual void createPresence(Presence & p)
```

Underlying connection state changed. Inherits StateChange from [Stateful<ClientState>](base.md#stateful).

---

{#onpresencedata}

#### onPresenceData

`virtual`

```cpp
virtual void onPresenceData(const json::Value & data, bool whiny)
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< ClientData >` | [`_data`](#_data-1)  |  |

---

{#_data-1}

#### _data

```cpp
std::unique_ptr< ClientData > _data
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`doConnect`](#doconnect)  |  |
| `void` | [`onTransportError`](#ontransporterror)  |  |
| `void` | [`onSocketRecv`](#onsocketrecv-9)  |  |
| `void` | [`onSocketClose`](#onsocketclose-6)  |  |
| `void` | [`onSocketError`](#onsocketerror-4)  |  |
| `void` | [`onWelcome`](#onwelcome)  |  |
| `void` | [`onServerMessage`](#onservermessage)  |  |
| `void` | [`startReconnect`](#startreconnect)  |  |
| `void` | [`reset`](#reset-15)  |  |
| `void` | [`syncDesiredRooms`](#syncdesiredrooms)  |  |
| `ssize_t` | [`sendJson`](#sendjson)  |  |
| `std::string` | [`buildUrl`](#buildurl) `const` |  |

---

{#doconnect}

#### doConnect

```cpp
void doConnect()
```

---

{#ontransporterror}

#### onTransportError

```cpp
void onTransportError(const icy::Error & error)
```

---

{#onsocketrecv-9}

#### onSocketRecv

```cpp
void onSocketRecv(const std::string & data)
```

---

{#onsocketclose-6}

#### onSocketClose

```cpp
void onSocketClose()
```

---

{#onsocketerror-4}

#### onSocketError

```cpp
void onSocketError(const std::string & error)
```

---

{#onwelcome}

#### onWelcome

```cpp
void onWelcome(const json::Value & msg)
```

---

{#onservermessage}

#### onServerMessage

```cpp
void onServerMessage(const json::Value & msg)
```

---

{#startreconnect}

#### startReconnect

```cpp
void startReconnect()
```

---

{#reset-15}

#### reset

```cpp
void reset()
```

---

{#syncdesiredrooms}

#### syncDesiredRooms

```cpp
void syncDesiredRooms()
```

---

{#sendjson}

#### sendJson

```cpp
ssize_t sendJson(const json::Value & msg)
```

---

{#buildurl}

#### buildUrl

`const`

```cpp
std::string buildUrl() const
```

{#clientdata}

## ClientData

```cpp
#include <src/symple/src/client/detail.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Options` | [`options`](#options-13)  |  |
| `uv::Loop *` | [`loop`](#loop-8)  |  |
| `http::ClientConnection::Ptr` | [`ws`](#ws-1)  |  |
| `Roster` | [`roster`](#roster-1)  |  |
| `std::string` | [`ourID`](#ourid-1)  |  |
| `std::unordered_set< std::string >` | [`currentRooms`](#currentrooms)  | Authoritative rooms from welcome / acks. |
| `std::unordered_set< std::string >` | [`desiredRooms`](#desiredrooms)  | Rooms the client wants persisted across reconnects. |
| `std::unordered_set< std::string >` | [`pendingJoins`](#pendingjoins)  | Join requests sent but not yet acknowledged. |
| `std::unordered_set< std::string >` | [`pendingLeaves`](#pendingleaves)  | Leave requests sent but not yet acknowledged. |
| `int` | [`announceStatus`](#announcestatus)  |  |
| `Timer` | [`reconnectTimer`](#reconnecttimer)  |  |
| `int` | [`reconnectCount`](#reconnectcount)  |  |
| `bool` | [`wasOnline`](#wasonline)  |  |
| `bool` | [`closing`](#closing-2)  |  |

---

{#options-13}

#### options

```cpp
Options options
```

---

{#loop-8}

#### loop

```cpp
uv::Loop * loop
```

---

{#ws-1}

#### ws

```cpp
http::ClientConnection::Ptr ws
```

---

{#roster-1}

#### roster

```cpp
Roster roster
```

---

{#ourid-1}

#### ourID

```cpp
std::string ourID
```

---

{#currentrooms}

#### currentRooms

```cpp
std::unordered_set< std::string > currentRooms
```

Authoritative rooms from welcome / acks.

---

{#desiredrooms}

#### desiredRooms

```cpp
std::unordered_set< std::string > desiredRooms
```

Rooms the client wants persisted across reconnects.

---

{#pendingjoins}

#### pendingJoins

```cpp
std::unordered_set< std::string > pendingJoins
```

Join requests sent but not yet acknowledged.

---

{#pendingleaves}

#### pendingLeaves

```cpp
std::unordered_set< std::string > pendingLeaves
```

Leave requests sent but not yet acknowledged.

---

{#announcestatus}

#### announceStatus

```cpp
int announceStatus = 0
```

---

{#reconnecttimer}

#### reconnectTimer

```cpp
Timer reconnectTimer
```

---

{#reconnectcount}

#### reconnectCount

```cpp
int reconnectCount = 0
```

---

{#wasonline}

#### wasOnline

```cpp
bool wasOnline = false
```

---

{#closing-2}

#### closing

```cpp
bool closing = false
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ClientData`](#clientdata-1) `inline` `explicit` |  |

---

{#clientdata-1}

#### ClientData

`inline` `explicit`

```cpp
inline explicit ClientData(Options opts, uv::Loop * targetLoop)
```

{#options-14}

## Options

```cpp
#include <icy/symple/client.h>
```

Connection and authentication options for the Symple client.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`host`](#host-2)  |  |
| `uint16_t` | [`port`](#port-2)  |  |
| `bool` | [`secure`](#secure-1)  | Use wss:// instead of ws://. |
| `bool` | [`reconnection`](#reconnection)  | Auto-reconnect on disconnect. |
| `int` | [`reconnectAttempts`](#reconnectattempts)  | 0 = unlimited |
| `int` | [`reconnectDelay`](#reconnectdelay)  | Milliseconds between retries. |
| `std::string` | [`user`](#user-1)  | User identifier (required) |
| `std::string` | [`name`](#name-12)  | Display name. |
| `std::string` | [`type`](#type-19)  | [Peer](#peer-1) type. |
| `std::string` | [`token`](#token)  | Auth token (optional) |

---

{#host-2}

#### host

```cpp
std::string host = "127.0.0.1"
```

---

{#port-2}

#### port

```cpp
uint16_t port = 4500
```

---

{#secure-1}

#### secure

```cpp
bool secure = false
```

Use wss:// instead of ws://.

---

{#reconnection}

#### reconnection

```cpp
bool reconnection = true
```

Auto-reconnect on disconnect.

---

{#reconnectattempts}

#### reconnectAttempts

```cpp
int reconnectAttempts = 0
```

0 = unlimited

---

{#reconnectdelay}

#### reconnectDelay

```cpp
int reconnectDelay = 3000
```

Milliseconds between retries.

---

{#user-1}

#### user

```cpp
std::string user
```

User identifier (required)

---

{#name-12}

#### name

```cpp
std::string name
```

Display name.

---

{#type-19}

#### type

```cpp
std::string type
```

[Peer](#peer-1) type.

---

{#token}

#### token

```cpp
std::string token
```

Auth token (optional)

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Options`](#options-15)  | Defaulted constructor. |

---

{#options-15}

#### Options

```cpp
Options() = default
```

Defaulted constructor.

{#command}

## Command

```cpp
#include <icy/symple/command.h>
```

> **Inherits:** [`Message`](#message-10)

Symple command message with a node path and action verb.

The `node` field is a colon-delimited path (e.g. "camera:zoom:in"). Individual path segments can be retrieved with [param()](#param).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Command`](#command-1)  | Constructs an empty command with type set to "command". |
|  | [`Command`](#command-2)  | Constructs a command from a JSON value. |
|  | [`Command`](#command-3)  | Copy constructor. |
| `std::string` | [`node`](#node) `const` | Returns the colon-delimited node path (e.g. "camera:zoom"). |
| `std::string` | [`action`](#action-2) `const` | Returns the action verb (defaults to "execute"). |
| `void` | [`setNode`](#setnode)  | Sets the node path field. |
| `void` | [`setAction`](#setaction)  | Sets the action verb field. |
| `bool` | [`valid`](#valid-11) `virtual` `const` | Returns true if the base message is valid and the `node` field is set. |
| `std::string` | [`param`](#param) `const` | Returns the nth colon-separated segment of the node path (1-based). Throws std::out_of_range if n exceeds the number of segments. |
| `std::vector< std::string >` | [`params`](#params)  | Returns all colon-separated segments of the node path. |
| `bool` | [`matches`](#matches-3) `const` | Returns true if the node path matches the given pattern. Uses wildcard node matching via [util::matchNodes](base.md#matchnodes). |

---

{#command-1}

#### Command

```cpp
Command()
```

Constructs an empty command with type set to "command".

---

{#command-2}

#### Command

```cpp
Command(const json::Value & root)
```

Constructs a command from a JSON value. 
#### Parameters
* `root` JSON object to initialise from.

---

{#command-3}

#### Command

```cpp
Command(const Command & root)
```

Copy constructor. 
#### Parameters
* `root` Source command.

---

{#node}

#### node

`const`

```cpp
std::string node() const
```

Returns the colon-delimited node path (e.g. "camera:zoom").

---

{#action-2}

#### action

`const`

```cpp
std::string action() const
```

Returns the action verb (defaults to "execute").

---

{#setnode}

#### setNode

```cpp
void setNode(std::string_view node)
```

Sets the node path field. 
#### Parameters
* `node` Colon-delimited node path string.

---

{#setaction}

#### setAction

```cpp
void setAction(std::string_view action)
```

Sets the action verb field. 
#### Parameters
* `action` Action string (e.g. "execute", "get", "set").

---

{#valid-11}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if the base message is valid and the `node` field is set.

---

{#param}

#### param

`const`

```cpp
std::string param(int n) const
```

Returns the nth colon-separated segment of the node path (1-based). Throws std::out_of_range if n exceeds the number of segments. 
#### Parameters
* `n` 1-based segment index. 

#### Returns
The nth path segment.

---

{#params}

#### params

```cpp
std::vector< std::string > params()
```

Returns all colon-separated segments of the node path. 
#### Returns
Vector of path segment strings.

---

{#matches-3}

#### matches

`const`

```cpp
bool matches(std::string_view xnode) const
```

Returns true if the node path matches the given pattern. Uses wildcard node matching via [util::matchNodes](base.md#matchnodes). 
#### Parameters
* `xnode` Pattern to match against (colon-delimited, supports wildcards).

{#event-2}

## Event

```cpp
#include <icy/symple/event.h>
```

> **Inherits:** [`Message`](#message-10)

Symple event message carrying a named occurrence with a timestamp.

The `name` field identifies the event. The `time` field is stored as a Unix timestamp (seconds since epoch).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Event`](#event-3)  | Constructs an event with type "event" and time set to now. |
|  | [`Event`](#event-4)  | Constructs an event from a JSON value; sets missing time to now. |
|  | [`Event`](#event-5)  | Copy constructor; preserves or sets missing time to now. |
| `bool` | [`valid`](#valid-12) `virtual` `const` | Returns true if the base message is valid and the `name` field is set. |
| `std::string` | [`name`](#name-13) `const` | Returns the event name string. |
| `std::time_t` | [`time`](#time-4) `const` | Returns the event timestamp as a Unix time_t value. |
| `void` | [`setName`](#setname-2)  | Sets the event name field. |
| `void` | [`setTime`](#settime)  | Sets the event timestamp. |

---

{#event-3}

#### Event

```cpp
Event()
```

Constructs an event with type "event" and time set to now.

---

{#event-4}

#### Event

```cpp
Event(const json::Value & root)
```

Constructs an event from a JSON value; sets missing time to now. 
#### Parameters
* `root` JSON object to initialise from.

---

{#event-5}

#### Event

```cpp
Event(const Event & root)
```

Copy constructor; preserves or sets missing time to now. 
#### Parameters
* `root` Source event.

---

{#valid-12}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if the base message is valid and the `name` field is set.

---

{#name-13}

#### name

`const`

```cpp
std::string name() const
```

Returns the event name string.

---

{#time-4}

#### time

`const`

```cpp
std::time_t time() const
```

Returns the event timestamp as a Unix time_t value.

---

{#setname-2}

#### setName

```cpp
void setName(std::string_view name)
```

Sets the event name field. 
#### Parameters
* `name` [Event](#event-2) name string.

---

{#settime}

#### setTime

```cpp
void setTime(std::time_t time)
```

Sets the event timestamp. 
#### Parameters
* `time` Unix timestamp (seconds since epoch).

{#form}

## Form

```cpp
#include <icy/symple/form.h>
```

> **Inherits:** [`FormElement`](#formelement)

Interactive form with pages, sections, and fields for command data exchange.

Attach a [Form](#form) to a [Command](#command) message to carry structured input/output. The `action` field governs the exchange direction; `partial` enables live/auto-complete field submission.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Form`](#form-1)  | Constructs an unbound form (root pointer is null). |
|  | [`Form`](#form-2)  | Constructs a form bound to the given JSON node. Sets the `type` field to "form". |
|  | [`Form`](#form-3)  | Constructs a form from a [Command](#command), binding to `command["form"]`. Sets the `type` field to "form". |
| `bool` | [`valid`](#valid-13)  | Returns true if the element is valid, non-empty, and has no errors. |
| `std::string` | [`action`](#action-3) `const` | Returns the form action string (defaults to "form"). |
| `bool` | [`partial`](#partial) `const` | Returns true if this is a partial form submission. |
| `void` | [`setAction`](#setaction-1)  | Sets the form action field. Throws std::invalid_argument for unrecognised values. Possible values: `form` - Form-processing entity requests form completion. `submit` - Form-submitting entity is sending data. `cancel` - Form-submitting entity cancelled submission. `result` - Form-processing entity is returning data. |
| `void` | [`setPartial`](#setpartial)  | Marks the form as a partial section for live/auto-complete updates. Partial forms transmit only the changed fields rather than the entire form payload. |

---

{#form-1}

#### Form

```cpp
Form()
```

Constructs an unbound form (root pointer is null).

---

{#form-2}

#### Form

```cpp
Form(json::Value & root)
```

Constructs a form bound to the given JSON node. Sets the `type` field to "form". 
#### Parameters
* `root` JSON node to bind to.

---

{#form-3}

#### Form

```cpp
Form(Command & root)
```

Constructs a form from a [Command](#command), binding to `command["form"]`. Sets the `type` field to "form". 
#### Parameters
* `root` Parent command message.

---

{#valid-13}

#### valid

```cpp
bool valid()
```

Returns true if the element is valid, non-empty, and has no errors.

---

{#action-3}

#### action

`const`

```cpp
std::string action() const
```

Returns the form action string (defaults to "form").

---

{#partial}

#### partial

`const`

```cpp
bool partial() const
```

Returns true if this is a partial form submission.

---

{#setaction-1}

#### setAction

```cpp
void setAction(std::string_view action)
```

Sets the form action field. Throws std::invalid_argument for unrecognised values. Possible values: `form` - Form-processing entity requests form completion. `submit` - Form-submitting entity is sending data. `cancel` - Form-submitting entity cancelled submission. `result` - Form-processing entity is returning data. 
#### Parameters
* `action` Action string.

---

{#setpartial}

#### setPartial

```cpp
void setPartial(bool flag)
```

Marks the form as a partial section for live/auto-complete updates. Partial forms transmit only the changed fields rather than the entire form payload. 
#### Parameters
* `flag` True to mark as partial.

{#formelement}

## FormElement

```cpp
#include <icy/symple/form.h>
```

> **Subclassed by:** [`Form`](#form), [`FormField`](#formfield)

Base element within a Symple form.

[FormElement](#formelement) wraps a reference to an external JSON node and provides typed accessors for the common `type`, `id`, `label`, and `elements` fields. Pages, sections, and fields all derive from this base.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FormElement`](#formelement-1)  | Constructs an unbound element (root pointer is null). |
|  | [`FormElement`](#formelement-2)  | Constructs an element bound to the given JSON node. |
|  | [`FormElement`](#formelement-3)  | Copy constructor; copies the root pointer reference (shallow). |
| `FormElement &` | [`operator=`](#operator-29)  | Copy-assigns the root pointer reference. |
| `std::string` | [`type`](#type-21) `const` | Returns the element type string. |
| `std::string` | [`id`](#id-6) `const` | Returns the element ID string. |
| `std::string` | [`label`](#label) `const` | Returns the display label string. |
| `void` | [`setType`](#settype-1)  | Sets the element type. Possible values: page, section, text, text-multi, list, list-multi, checkbox, media, custom |
| `void` | [`setId`](#setid)  | Sets the element ID field. |
| `void` | [`setLabel`](#setlabel)  | Sets the display label field. |
| `void` | [`setHint`](#sethint)  | Sets the hint/description field shown below the element. |
| `void` | [`setError`](#seterror-7)  | Sets an optional validation error message. |
| `FormElement` | [`addPage`](#addpage)  | Appends a page child element and returns a handle to it. |
| `FormElement` | [`addSection`](#addsection)  | Appends a section child element and returns a handle to it. |
| `FormField` | [`addField`](#addfield)  | Appends a typed field child element and returns a handle to it. Throws std::invalid_argument if type is not a recognised field type. |
| `FormField` | [`getField`](#getfield)  | Searches child elements for the field with the given ID. |
| `bool` | [`getField`](#getfield-1)  | Populates a [FormField](#formfield) by searching child elements for the given ID. |
| `bool` | [`hasField`](#hasfield)  | Returns true if any child element has an ID matching the given value. |
| `void` | [`setLive`](#setlive)  | Sets the live flag on this element. Live elements are used to submit partial form sections (e.g. for auto-complete) without sending the entire form. |
| `bool` | [`live`](#live) `const` | Returns true if this field is live, meaning the form-processing entity should auto-update this field's value whenever it changes. |
| `bool` | [`clearElements`](#clearelements)  | Removes all child elements whose ID matches the given value. |
| `void` | [`clear`](#clear-3)  | Clears all fields from the underlying JSON node. |
| `bool` | [`valid`](#valid-14) `const` | Returns true if the form element is valid. |
| `int` | [`numElements`](#numelements)  | Returns the number of child elements. |
| `bool` | [`hasErrors`](#haserrors)  | Returns true if any fields have errors. |
| `bool` | [`hasPages`](#haspages)  | Returns true if the form has multiple pages. |
| `json::Value &` | [`root`](#root-4) `const` | Returns a reference to the underlying JSON node. Throws std::runtime_error if the root pointer is null. |

---

{#formelement-1}

#### FormElement

```cpp
FormElement()
```

Constructs an unbound element (root pointer is null).

---

{#formelement-2}

#### FormElement

```cpp
FormElement(json::Value & root, std::string_view type, std::string_view id, std::string_view label)
```

Constructs an element bound to the given JSON node. 
#### Parameters
* `root` JSON node this element refers to. 

* `type` Optional element type string. 

* `id` Optional element ID string. 

* `label` Optional display label string.

---

{#formelement-3}

#### FormElement

```cpp
FormElement(const FormElement & r)
```

Copy constructor; copies the root pointer reference (shallow). 
#### Parameters
* `r` Source element.

---

{#operator-29}

#### operator=

```cpp
FormElement & operator=(const FormElement & r)
```

Copy-assigns the root pointer reference. 
#### Parameters
* `r` Source element.

---

{#type-21}

#### type

`const`

```cpp
std::string type() const
```

Returns the element type string.

---

{#id-6}

#### id

`const`

```cpp
std::string id() const
```

Returns the element ID string.

---

{#label}

#### label

`const`

```cpp
std::string label() const
```

Returns the display label string.

---

{#settype-1}

#### setType

```cpp
void setType(std::string_view type)
```

Sets the element type. Possible values: page, section, text, text-multi, list, list-multi, checkbox, media, custom 
#### Parameters
* `type` Element type string.

---

{#setid}

#### setId

```cpp
void setId(std::string_view id)
```

Sets the element ID field. 
#### Parameters
* `id` Element ID string.

---

{#setlabel}

#### setLabel

```cpp
void setLabel(std::string_view text)
```

Sets the display label field. 
#### Parameters
* `text` Label text.

---

{#sethint}

#### setHint

```cpp
void setHint(std::string_view text)
```

Sets the hint/description field shown below the element. 
#### Parameters
* `text` Hint text.

---

{#seterror-7}

#### setError

```cpp
void setError(std::string_view error)
```

Sets an optional validation error message. 
#### Parameters
* `error` [Error](base.md#error) text to display.

---

{#addpage}

#### addPage

```cpp
FormElement addPage(std::string_view id, std::string_view label)
```

Appends a page child element and returns a handle to it. 
#### Parameters
* `id` Optional page ID. 

* `label` Optional page label. 

#### Returns
[FormElement](#formelement) referencing the new page node.

---

{#addsection}

#### addSection

```cpp
FormElement addSection(std::string_view id, std::string_view label)
```

Appends a section child element and returns a handle to it. 
#### Parameters
* `id` Optional section ID. 

* `label` Optional section label. 

#### Returns
[FormElement](#formelement) referencing the new section node.

---

{#addfield}

#### addField

```cpp
FormField addField(std::string_view type, std::string_view id, std::string_view label)
```

Appends a typed field child element and returns a handle to it. Throws std::invalid_argument if type is not a recognised field type. 
#### Parameters
* `type` Field type (e.g. "text", "list", "boolean"). 

* `id` Optional field ID. 

* `label` Optional field label. 

#### Returns
[FormField](#formfield) referencing the new field node.

---

{#getfield}

#### getField

```cpp
FormField getField(std::string_view id, bool partial)
```

Searches child elements for the field with the given ID. 
#### Parameters
* `id` Field ID to search for. 

* `partial` If true, performs a substring match. 

#### Returns
[FormField](#formfield) handle (may be invalid if not found).

---

{#getfield-1}

#### getField

```cpp
bool getField(std::string_view id, FormField & field, bool partial)
```

Populates a [FormField](#formfield) by searching child elements for the given ID. 
#### Parameters
* `id` Field ID to search for. 

* `field` Output field handle. 

* `partial` If true, performs a substring match. 

#### Returns
True if the field was found.

---

{#hasfield}

#### hasField

```cpp
bool hasField(std::string_view id, bool partial)
```

Returns true if any child element has an ID matching the given value. 
#### Parameters
* `id` ID string to search for. 

* `partial` If true, a substring match is sufficient.

---

{#setlive}

#### setLive

```cpp
void setLive(bool flag)
```

Sets the live flag on this element. Live elements are used to submit partial form sections (e.g. for auto-complete) without sending the entire form. 
#### Parameters
* `flag` True to enable live updates.

---

{#live}

#### live

`const`

```cpp
bool live() const
```

Returns true if this field is live, meaning the form-processing entity should auto-update this field's value whenever it changes.

---

{#clearelements}

#### clearElements

```cpp
bool clearElements(std::string_view id, bool partial)
```

Removes all child elements whose ID matches the given value. 
#### Parameters
* `id` ID string to match against. 

* `partial` If true, removes elements whose ID contains the string. 

#### Returns
True if at least one element was removed.

---

{#clear-3}

#### clear

```cpp
void clear()
```

Clears all fields from the underlying JSON node.

---

{#valid-14}

#### valid

`const`

```cpp
bool valid() const
```

Returns true if the form element is valid.

---

{#numelements}

#### numElements

```cpp
int numElements()
```

Returns the number of child elements.

---

{#haserrors}

#### hasErrors

```cpp
bool hasErrors()
```

Returns true if any fields have errors.

---

{#haspages}

#### hasPages

```cpp
bool hasPages()
```

Returns true if the form has multiple pages.

---

{#root-4}

#### root

`const`

```cpp
json::Value & root() const
```

Returns a reference to the underlying JSON node. Throws std::runtime_error if the root pointer is null.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `json::Value *` | [`_root`](#_root)  | The root pointer is just a reference to the externally managed JSON value memory. |

---

{#_root}

#### _root

```cpp
json::Value * _root
```

The root pointer is just a reference to the externally managed JSON value memory.

{#formfield}

## FormField

```cpp
#include <icy/symple/form.h>
```

> **Inherits:** [`FormElement`](#formelement)

[Form](#form) input field with typed values and selectable options.

Values are stored as strings in a JSON array. Typed accessors (intValue, doubleValue, boolValue) parse the first string on demand.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FormField`](#formfield-1)  | Constructs an unbound field (root pointer is null). |
|  | [`FormField`](#formfield-2)  | Constructs a field bound to the given JSON node. |
| `void` | [`addOption`](#addoption)  | Adds a labelled option for list-based fields. |
| `void` | [`addOption`](#addoption-1)  | Adds an option whose key and display value are identical. |
| `void` | [`setValue`](#setvalue-4)  | Replaces all current values with a single string value. |
| `void` | [`setValue`](#setvalue-5)  | Replaces all current values with a single integer value. |
| `void` | [`setValue`](#setvalue-6)  | Replaces all current values with a single double value. |
| `void` | [`setValue`](#setvalue-7)  | Replaces all current values with a single boolean value. |
| `void` | [`addValue`](#addvalue)  | Appends a string value to the values array. |
| `void` | [`addValue`](#addvalue-1)  | Appends an integer value to the values array. |
| `void` | [`addValue`](#addvalue-2)  | Appends a double value to the values array. |
| `void` | [`addValue`](#addvalue-3)  | Appends a boolean value to the values array. |
| `json::Value &` | [`values`](#values)  | Returns a reference to the JSON array of all values. |
| `std::string` | [`value`](#value-4) `const` | Returns the first value as a string. Most field types (except multi-value) only use a single value. |
| `int` | [`intValue`](#intvalue) `const` | Returns the first value parsed as an integer. |
| `double` | [`doubleValue`](#doublevalue) `const` | Returns the first value parsed as a double. |
| `bool` | [`boolValue`](#boolvalue) `const` | Returns the first value parsed as a boolean. Treats "1", "true", and "on" as true; all other strings as false. |

---

{#formfield-1}

#### FormField

```cpp
FormField()
```

Constructs an unbound field (root pointer is null).

---

{#formfield-2}

#### FormField

```cpp
FormField(json::Value & root, std::string_view type, std::string_view id, std::string_view label)
```

Constructs a field bound to the given JSON node. 
#### Parameters
* `root` JSON node to bind to. 

* `type` Field type string (e.g. "text", "list"). 

* `id` Optional field ID. 

* `label` Optional display label.

---

{#addoption}

#### addOption

```cpp
void addOption(std::string_view key, std::string_view value)
```

Adds a labelled option for list-based fields. 
#### Parameters
* `key` Option key sent on submit. 

* `value` Display label for the option.

---

{#addoption-1}

#### addOption

```cpp
void addOption(std::string_view value)
```

Adds an option whose key and display value are identical. 
#### Parameters
* `value` Option string.

---

{#setvalue-4}

#### setValue

```cpp
void setValue(std::string_view value)
```

Replaces all current values with a single string value. 
#### Parameters
* `value` String value to set.

---

{#setvalue-5}

#### setValue

```cpp
void setValue(int value)
```

Replaces all current values with a single integer value. 
#### Parameters
* `value` Integer value to set.

---

{#setvalue-6}

#### setValue

```cpp
void setValue(double value)
```

Replaces all current values with a single double value. 
#### Parameters
* `value` Double value to set.

---

{#setvalue-7}

#### setValue

```cpp
void setValue(bool value)
```

Replaces all current values with a single boolean value. 
#### Parameters
* `value` Boolean value to set.

---

{#addvalue}

#### addValue

```cpp
void addValue(std::string_view value)
```

Appends a string value to the values array. 
#### Parameters
* `value` String value to append.

---

{#addvalue-1}

#### addValue

```cpp
void addValue(int value)
```

Appends an integer value to the values array. 
#### Parameters
* `value` Integer value to append.

---

{#addvalue-2}

#### addValue

```cpp
void addValue(double value)
```

Appends a double value to the values array. 
#### Parameters
* `value` Double value to append.

---

{#addvalue-3}

#### addValue

```cpp
void addValue(bool value)
```

Appends a boolean value to the values array. 
#### Parameters
* `value` Boolean value to append.

---

{#values}

#### values

```cpp
json::Value & values()
```

Returns a reference to the JSON array of all values.

---

{#value-4}

#### value

`const`

```cpp
std::string value() const
```

Returns the first value as a string. Most field types (except multi-value) only use a single value.

---

{#intvalue}

#### intValue

`const`

```cpp
int intValue() const
```

Returns the first value parsed as an integer.

---

{#doublevalue}

#### doubleValue

`const`

```cpp
double doubleValue() const
```

Returns the first value parsed as a double.

---

{#boolvalue}

#### boolValue

`const`

```cpp
bool boolValue() const
```

Returns the first value parsed as a boolean. Treats "1", "true", and "on" as true; all other strings as false.

{#message-10}

## Message

```cpp
#include <icy/symple/message.h>
```

> **Inherits:** `Value`, [`IPacket`](base.md#ipacket)
> **Subclassed by:** [`Command`](#command), [`Event`](#event-2), [`Presence`](#presence)

Base Symple protocol message with addressing, status, data, and notes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Message`](#message-11)  | Constructs a new message with a random ID and type set to "message". |
|  | [`Message`](#message-12)  | Constructs a message from a JSON value; sets missing id/type fields. |
|  | [`Message`](#message-13)  | Copy constructor; preserves or generates id/type fields. |
| `std::unique_ptr< IPacket >` | [`clone`](#clone-18) `virtual` `const` | Returns a heap-allocated copy of this message. |
| `bool` | [`valid`](#valid-15) `virtual` `const` | Returns true if the message has both `type` and `id` fields. |
| `void` | [`clear`](#clear-4) `virtual` | Clears all JSON fields from this message. |
| `void` | [`clearData`](#cleardata) `virtual` | Clears the `data` sub-object. |
| `void` | [`clearNotes`](#clearnotes) `virtual` | Clears the `notes` array. |
| `std::string` | [`type`](#type-22) `const` | Returns the message type string (defaults to "message"). |
| `std::string` | [`id`](#id-7) `const` | Returns the message ID string. |
| `Address` | [`to`](#to) `const` | Returns the recipient address parsed from the `to` field. |
| `Address` | [`from`](#from) `const` | Returns the sender address parsed from the `from` field. |
| `std::string` | [`toUser`](#touser) `const` | Returns the user component of the `to` address without constructing an [Address](#address-13). |
| `std::string` | [`toId`](#toid) `const` | Returns the id component of the `to` address without constructing an [Address](#address-13). |
| `std::string` | [`fromUser`](#fromuser) `const` | Returns the user component of the `from` address without constructing an [Address](#address-13). |
| `std::string` | [`fromId`](#fromid) `const` | Returns the id component of the `from` address without constructing an [Address](#address-13). |
| `int` | [`status`](#status-3) `const` | Returns the HTTP status code, or -1 if not set. |
| `void` | [`setType`](#settype-2)  | Sets the message type field. |
| `void` | [`setTo`](#setto)  | Sets the `to` field from a peer's address. |
| `void` | [`setTo`](#setto-1)  | Sets the `to` field from an address object. |
| `void` | [`setTo`](#setto-2)  | Sets the `to` field from an address string. |
| `void` | [`setFrom`](#setfrom)  | Sets the `from` field from a peer's address. |
| `void` | [`setFrom`](#setfrom-1)  | Sets the `from` field from an address object. |
| `void` | [`setFrom`](#setfrom-2)  | Sets the `from` field from an address string. |
| `void` | [`setStatus`](#setstatus-1)  | HTTP status codes are used to describe the message response. Throws std::invalid_argument if code is outside [101, 504]. |
| `json::Value &` | [`notes`](#notes)  | Returns a reference to the `notes` JSON array. |
| `void` | [`setNote`](#setnote)  | Replaces all notes with a single note. |
| `void` | [`addNote`](#addnote)  | Appends a note to the `notes` array. |
| `json::Value` | [`data`](#data-4) `const` | Returns a copy of the named data field. |
| `json::Value &` | [`data`](#data-5)  | Returns a reference to the named data field (creates it if absent). |
| `json::Value &` | [`setData`](#setdata)  | Creates or replaces a named data field; returns a reference to it. |
| `void` | [`setData`](#setdata-1)  | Sets a named data field to a C-string value. |
| `void` | [`setData`](#setdata-2)  | Sets a named data field to a string value. |
| `void` | [`setData`](#setdata-3)  | Sets a named data field to a JSON value. |
| `void` | [`setData`](#setdata-4)  | Sets a named data field to an integer value. |
| `void` | [`removeData`](#removedata)  | Removes a named field from the `data` sub-object. |
| `bool` | [`hasData`](#hasdata)  | Returns true if the named field exists in the `data` sub-object. |
| `ssize_t` | [`read`](#read-13) `virtual` | Deserialises the message from a raw buffer. |
| `ssize_t` | [`read`](#read-14) `virtual` | Deserialises the message from a JSON string. |
| `void` | [`write`](#write-28) `virtual` `const` | Serialises the message as JSON into a buffer. |
| `bool` | [`isRequest`](#isrequest) `const` | Returns true if no status code has been set (i.e. [status()](#status-3) == -1). |
| `size_t` | [`size`](#size-11) `virtual` `const` | Returns the serialised JSON size in bytes. |
| `void` | [`print`](#print-16) `virtual` `const` | Pretty-prints the message JSON to the given stream. |
| `const char *` | [`className`](#classname-8) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |
| `char *` | [`data`](#data-6) `virtual` `const` `inline` | The packet data pointer for buffered packets. |
| `bool` | [`hasData`](#hasdata-1) `virtual` `const` `inline` | Returns true if the packet has a non-null data pointer. |

---

{#message-11}

#### Message

```cpp
Message()
```

Constructs a new message with a random ID and type set to "message".

---

{#message-12}

#### Message

```cpp
Message(const json::Value & root)
```

Constructs a message from a JSON value; sets missing id/type fields. 
#### Parameters
* `root` JSON object to initialise from.

---

{#message-13}

#### Message

```cpp
Message(const Message & root)
```

Copy constructor; preserves or generates id/type fields. 
#### Parameters
* `root` Source message.

---

{#clone-18}

#### clone

`virtual` `const`

```cpp
virtual std::unique_ptr< IPacket > clone() const
```

Returns a heap-allocated copy of this message.

---

{#valid-15}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if the message has both `type` and `id` fields.

---

{#clear-4}

#### clear

`virtual`

```cpp
virtual void clear()
```

Clears all JSON fields from this message.

---

{#cleardata}

#### clearData

`virtual`

```cpp
virtual void clearData()
```

Clears the `data` sub-object.

---

{#clearnotes}

#### clearNotes

`virtual`

```cpp
virtual void clearNotes()
```

Clears the `notes` array.

---

{#type-22}

#### type

`const`

```cpp
std::string type() const
```

Returns the message type string (defaults to "message").

---

{#id-7}

#### id

`const`

```cpp
std::string id() const
```

Returns the message ID string.

---

{#to}

#### to

`const`

```cpp
Address to() const
```

Returns the recipient address parsed from the `to` field.

---

{#from}

#### from

`const`

```cpp
Address from() const
```

Returns the sender address parsed from the `from` field.

---

{#touser}

#### toUser

`const`

```cpp
std::string toUser() const
```

Returns the user component of the `to` address without constructing an [Address](#address-13).

---

{#toid}

#### toId

`const`

```cpp
std::string toId() const
```

Returns the id component of the `to` address without constructing an [Address](#address-13).

---

{#fromuser}

#### fromUser

`const`

```cpp
std::string fromUser() const
```

Returns the user component of the `from` address without constructing an [Address](#address-13).

---

{#fromid}

#### fromId

`const`

```cpp
std::string fromId() const
```

Returns the id component of the `from` address without constructing an [Address](#address-13).

---

{#status-3}

#### status

`const`

```cpp
int status() const
```

Returns the HTTP status code, or -1 if not set.

---

{#settype-2}

#### setType

```cpp
void setType(std::string_view type)
```

Sets the message type field. 
#### Parameters
* `type` Type string (e.g. "message", "command").

---

{#setto}

#### setTo

```cpp
void setTo(const Peer & to)
```

Sets the `to` field from a peer's address. 
#### Parameters
* `to` Destination peer.

---

{#setto-1}

#### setTo

```cpp
void setTo(const Address & to)
```

Sets the `to` field from an address object. 
#### Parameters
* `to` Destination address.

---

{#setto-2}

#### setTo

```cpp
void setTo(std::string_view to)
```

Sets the `to` field from an address string. 
#### Parameters
* `to` Destination address string.

---

{#setfrom}

#### setFrom

```cpp
void setFrom(const Peer & from)
```

Sets the `from` field from a peer's address. 
#### Parameters
* `from` Sender peer.

---

{#setfrom-1}

#### setFrom

```cpp
void setFrom(const Address & from)
```

Sets the `from` field from an address object. 
#### Parameters
* `from` Sender address.

---

{#setfrom-2}

#### setFrom

```cpp
void setFrom(std::string_view from)
```

Sets the `from` field from an address string. 
#### Parameters
* `from` Sender address string.

---

{#setstatus-1}

#### setStatus

```cpp
void setStatus(int code)
```

HTTP status codes are used to describe the message response. Throws std::invalid_argument if code is outside [101, 504]. 
#### Parameters
* `code` HTTP status code. 

**See also**: [http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html](http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html)

---

{#notes}

#### notes

```cpp
json::Value & notes()
```

Returns a reference to the `notes` JSON array.

---

{#setnote}

#### setNote

```cpp
void setNote(std::string_view type, std::string_view text)
```

Replaces all notes with a single note. 
#### Parameters
* `type` Note severity: `info`, `warn`, or `error`. 

* `text` Note message text.

---

{#addnote}

#### addNote

```cpp
void addNote(std::string_view type, std::string_view text)
```

Appends a note to the `notes` array. 
#### Parameters
* `type` Note severity: `info`, `warn`, or `error`. 

* `text` Note message text.

---

{#data-4}

#### data

`const`

```cpp
json::Value data(std::string_view name) const
```

Returns a copy of the named data field. 
#### Parameters
* `name` Field name within `data`.

---

{#data-5}

#### data

```cpp
json::Value & data(std::string_view name)
```

Returns a reference to the named data field (creates it if absent). 
#### Parameters
* `name` Field name within `data`.

---

{#setdata}

#### setData

```cpp
json::Value & setData(std::string_view name)
```

Creates or replaces a named data field; returns a reference to it. 
#### Parameters
* `name` Field name within `data`.

---

{#setdata-1}

#### setData

```cpp
void setData(std::string_view name, const char * data)
```

Sets a named data field to a C-string value. 
#### Parameters
* `name` Field name within `data`. 

* `data` String value to assign.

---

{#setdata-2}

#### setData

```cpp
void setData(std::string_view name, std::string_view data)
```

Sets a named data field to a string value. 
#### Parameters
* `name` Field name within `data`. 

* `data` String value to assign.

---

{#setdata-3}

#### setData

```cpp
void setData(std::string_view name, const json::Value & data)
```

Sets a named data field to a JSON value. 
#### Parameters
* `name` Field name within `data`. 

* `data` JSON value to assign.

---

{#setdata-4}

#### setData

```cpp
void setData(std::string_view name, int data)
```

Sets a named data field to an integer value. 
#### Parameters
* `name` Field name within `data`. 

* `data` Integer value to assign.

---

{#removedata}

#### removeData

```cpp
void removeData(std::string_view name)
```

Removes a named field from the `data` sub-object. 
#### Parameters
* `name` Field name to remove.

---

{#hasdata}

#### hasData

```cpp
bool hasData(std::string_view name)
```

Returns true if the named field exists in the `data` sub-object. 
#### Parameters
* `name` Field name to look up.

---

{#read-13}

#### read

`virtual`

```cpp
virtual ssize_t read(const ConstBuffer & buf)
```

Deserialises the message from a raw buffer. 
#### Parameters
* `buf` Buffer containing the JSON payload. 

#### Returns
Number of bytes consumed.

---

{#read-14}

#### read

`virtual`

```cpp
virtual ssize_t read(const std::string & root)
```

Deserialises the message from a JSON string. 
#### Parameters
* `root` JSON string to parse. 

#### Returns
Number of characters consumed.

---

{#write-28}

#### write

`virtual` `const`

```cpp
virtual void write(Buffer & buf) const
```

Serialises the message as JSON into a buffer. 
#### Parameters
* `buf` Buffer to append to.

---

{#isrequest}

#### isRequest

`const`

```cpp
bool isRequest() const
```

Returns true if no status code has been set (i.e. [status()](#status-3) == -1).

---

{#size-11}

#### size

`virtual` `const`

```cpp
virtual size_t size() const
```

Returns the serialised JSON size in bytes.

---

{#print-16}

#### print

`virtual` `const`

```cpp
virtual void print(std::ostream & os) const
```

Pretty-prints the message JSON to the given stream. 
#### Parameters
* `os` Output stream.

---

{#classname-8}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

---

{#data-6}

#### data

`virtual` `const` `inline`

```cpp
virtual inline char * data() const
```

The packet data pointer for buffered packets.

---

{#hasdata-1}

#### hasData

`virtual` `const` `inline`

```cpp
virtual inline bool hasData() const
```

Returns true if the packet has a non-null data pointer.

{#peer-1}

## Peer

```cpp
#include <icy/symple/peer.h>
```

> **Inherits:** `Value`

Symple peer record containing identity, presence, and custom fields.

A [Peer](#peer-1) object may also contain arbitrary application data set by the client to share with other peers on the network. **See also**: [Address](#address-13) for further methods and [basic](base.md#basic) accessors.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Peer`](#peer-2)  | Constructs an empty peer with type set to "Peer". |
|  | [`Peer`](#peer-3)  | Copy constructor; preserves type field. |
| `Peer &` | [`operator=`](#operator-30)  | Copy-assigns peer data from another peer. |
|  | [`Peer`](#peer-4)  | Constructs a peer from a raw JSON value. |
| `Address` | [`address`](#address-17) `const` | Returns the peer's address (user + session ID). |
| `std::string` | [`id`](#id-8) `const` | Returns the session ID assigned by the server. |
| `std::string` | [`user`](#user-2) `const` | Returns the user identifier. |
| `std::string` | [`name`](#name-14) `const` | Returns the display name. |
| `std::string` | [`type`](#type-23) `const` | Returns the peer type string (e.g. "Peer", "bot"). |
| `std::string` | [`host`](#host-3) `const` | Returns the host address associated with this peer. |
| `void` | [`setID`](#setid-1)  | Sets the session ID field. |
| `void` | [`setUser`](#setuser)  | Sets the user identifier field. |
| `void` | [`setName`](#setname-3)  | Sets the display name field. |
| `void` | [`setType`](#settype-3)  | Sets the peer type field. |
| `void` | [`setHost`](#sethost-2)  | Sets the host address field. |
| `bool` | [`valid`](#valid-16) `virtual` | Updates the peer from the given data object. |
| `void` | [`print`](#print-17) `const` | Writes the peer's JSON representation to the given stream. |
| `const char *` | [`className`](#classname-9) `virtual` `const` `inline` |  |

---

{#peer-2}

#### Peer

```cpp
Peer()
```

Constructs an empty peer with type set to "Peer".

---

{#peer-3}

#### Peer

```cpp
Peer(const Peer & r)
```

Copy constructor; preserves type field. 
#### Parameters
* `r` Source peer.

---

{#operator-30}

#### operator=

```cpp
Peer & operator=(const Peer & r)
```

Copy-assigns peer data from another peer. 
#### Parameters
* `r` Source peer.

---

{#peer-4}

#### Peer

```cpp
Peer(const json::Value & r)
```

Constructs a peer from a raw JSON value. 
#### Parameters
* `r` JSON object containing peer fields.

---

{#address-17}

#### address

`const`

```cpp
Address address() const
```

Returns the peer's address (user + session ID). 
#### Returns
[Address](#address-13) constructed from the `user` and `id` fields.

---

{#id-8}

#### id

`const`

```cpp
std::string id() const
```

Returns the session ID assigned by the server.

---

{#user-2}

#### user

`const`

```cpp
std::string user() const
```

Returns the user identifier.

---

{#name-14}

#### name

`const`

```cpp
std::string name() const
```

Returns the display name.

---

{#type-23}

#### type

`const`

```cpp
std::string type() const
```

Returns the peer type string (e.g. "Peer", "bot").

---

{#host-3}

#### host

`const`

```cpp
std::string host() const
```

Returns the host address associated with this peer.

---

{#setid-1}

#### setID

```cpp
void setID(std::string_view id)
```

Sets the session ID field. 
#### Parameters
* `id` Session ID string.

---

{#setuser}

#### setUser

```cpp
void setUser(std::string_view user)
```

Sets the user identifier field. 
#### Parameters
* `user` User identifier string.

---

{#setname-3}

#### setName

```cpp
void setName(std::string_view name)
```

Sets the display name field. 
#### Parameters
* `name` Display name string.

---

{#settype-3}

#### setType

```cpp
void setType(std::string_view type)
```

Sets the peer type field. 
#### Parameters
* `type` Type string.

---

{#sethost-2}

#### setHost

```cpp
void setHost(std::string_view host)
```

Sets the host address field. 
#### Parameters
* `host` Host address string.

---

{#valid-16}

#### valid

`virtual`

```cpp
virtual bool valid()
```

Updates the peer from the given data object.

Returns true if the peer has `id`, `user`, and `type` fields.

---

{#print-17}

#### print

`const`

```cpp
void print(std::ostream & os) const
```

Writes the peer's JSON representation to the given stream. 
#### Parameters
* `os` Output stream.

---

{#classname-9}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

{#presence}

## Presence

```cpp
#include <icy/symple/presence.h>
```

> **Inherits:** [`Message`](#message-10)

Symple presence message indicating a peer's online status.

[Presence](#presence) messages carry peer data in the `data` field. When `probe` is true the recipient should respond with their own presence.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Presence`](#presence-1)  | Constructs a presence message with type set to "presence". |
|  | [`Presence`](#presence-2)  | Constructs a presence message from a JSON value. |
|  | [`Presence`](#presence-3)  | Copy constructor. |
| `bool` | [`isProbe`](#isprobe)  | Returns true if this is a presence probe request. Recipients of a probe should send back their own presence. |
| `void` | [`setProbe`](#setprobe)  | Sets or clears the probe flag on this presence message. |

---

{#presence-1}

#### Presence

```cpp
Presence()
```

Constructs a presence message with type set to "presence".

---

{#presence-2}

#### Presence

```cpp
Presence(const json::Value & root)
```

Constructs a presence message from a JSON value. 
#### Parameters
* `root` JSON object to initialise from.

---

{#presence-3}

#### Presence

```cpp
Presence(const Presence & root)
```

Copy constructor. 
#### Parameters
* `root` Source presence message.

---

{#isprobe}

#### isProbe

```cpp
bool isProbe()
```

Returns true if this is a presence probe request. Recipients of a probe should send back their own presence.

---

{#setprobe}

#### setProbe

```cpp
void setProbe(bool flag)
```

Sets or clears the probe flag on this presence message. 
#### Parameters
* `flag` True to mark this as a probe.

{#roster-2}

## Roster

```cpp
#include <icy/symple/roster.h>
```

> **Inherits:** [`string, Peer >`](base.md#keyedstore)

The [Roster](#roster-2) provides a registry for active network peers indexed by session ID.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(Peer &)>` | [`PeerAdded`](#peeradded)  | Lifecycle signals for external observers (samples, UI). |
| `Signal< void(constPeer &)>` | [`PeerRemoved`](#peerremoved)  |  |

---

{#peeradded}

#### PeerAdded

```cpp
Signal< void(Peer &)> PeerAdded
```

Lifecycle signals for external observers (samples, UI).

---

{#peerremoved}

#### PeerRemoved

```cpp
Signal< void(constPeer &)> PeerRemoved
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Roster`](#roster-3)  |  |
| `Peer *` | [`getByHost`](#getbyhost)  | Returns the first peer which matches the given host address. |
| `Map` | [`peers`](#peers) `const` | Returns a deep copy of the peer map. |
| `void` | [`print`](#print-18) `const` |  |
| `constchar *` | [`className`](#classname-10) `virtual` `const` `inline` |  |

---

{#roster-3}

#### Roster

```cpp
Roster()
```

---

{#getbyhost}

#### getByHost

```cpp
Peer * getByHost(std::string_view host)
```

Returns the first peer which matches the given host address.

---

{#peers}

#### peers

`const`

```cpp
Map peers() const
```

Returns a deep copy of the peer map.

---

{#print-18}

#### print

`const`

```cpp
void print(std::ostream & os) const
```

---

{#classname-10}

#### className

`virtual` `const` `inline`

```cpp
virtual inline constchar * className() const
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onAdd`](#onadd) `inline` |  |
| `void` | [`onRemove`](#onremove) `inline` |  |

---

{#onadd}

#### onAdd

`inline`

```cpp
inline void onAdd(const std::string &, Peer * peer)
```

---

{#onremove}

#### onRemove

`inline`

```cpp
inline void onRemove(const std::string &, Peer * peer)
```

{#server-9}

## Server

```cpp
#include <icy/symple/server.h>
```

Symple v4 server.

Accepts WebSocket connections, authenticates peers, manages rooms, and routes messages. Implements the Symple v4 protocol over native WebSocket.

Usage: [smpl::Server](#server-9) server; server.start({.port = 4500});

// Optional: custom authentication server.Authenticate += []([ServerPeer](#serverpeer)& peer, const [json::Value](json.md#value)& auth, bool& allowed, std::vector<std::string>& rooms) { allowed = (auth.value("token", "") == "secret"); rooms.push_back("team-a"); };

The server also serves as an HTTP server, so you can serve static files (e.g. a web UI) on the same port.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(ServerPeer &, const json::Value &auth, bool &allowed, std::vector< std::string > &rooms)>` | [`Authenticate`](#authenticate-2)  | Custom authentication hook. Set `allowed` to false to reject the peer. Append to `rooms` to assign team/group memberships. If not connected, all peers with a valid `user` field are accepted. |
| `Signal< void(ServerPeer &)>` | [`PeerConnected`](#peerconnected-1)  | [Peer](#peer-1) authenticated and online. |
| `Signal< void(ServerPeer &)>` | [`PeerDisconnected`](#peerdisconnected-1)  | [Peer](#peer-1) disconnected. |

---

{#authenticate-2}

#### Authenticate

```cpp
Signal< void(ServerPeer &, const json::Value &auth, bool &allowed, std::vector< std::string > &rooms)> Authenticate
```

Custom authentication hook. Set `allowed` to false to reject the peer. Append to `rooms` to assign team/group memberships. If not connected, all peers with a valid `user` field are accepted.

---

{#peerconnected-1}

#### PeerConnected

```cpp
Signal< void(ServerPeer &)> PeerConnected
```

[Peer](#peer-1) authenticated and online.

---

{#peerdisconnected-1}

#### PeerDisconnected

```cpp
Signal< void(ServerPeer &)> PeerDisconnected
```

[Peer](#peer-1) disconnected.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Server`](#server-10)  | Constructs a server using the given event loop. |
|  | [`Server`](#server-11)  | Deleted constructor. |
| `void` | [`start`](#start-17)  | Starts the server with the given options. Begins accepting WebSocket connections on opts.host:opts.port. |
| `void` | [`start`](#start-18)  | Starts the server with a custom HTTP factory for non-WebSocket requests. The Symple server handles WebSocket upgrades internally; any other HTTP request (e.g. static files, REST API) is delegated to this factory. |
| `void` | [`stop`](#stop-14)  | Broadcasts a shutdown notice to all peers, closes the listen socket, and releases all internal state. Safe to call more than once. |
| `void` | [`broadcast`](#broadcast)  | Broadcast a message to all peers in a room (excluding sender). |
| `void` | [`broadcastRooms`](#broadcastrooms)  | Broadcast to multiple rooms with per-recipient dedup. |
| `bool` | [`sendTo`](#sendto)  | Send a message to a specific peer by session ID. |
| `bool` | [`sendToUser`](#sendtouser)  | Send a message to any peer with the given user name. |
| `ServerPeer *` | [`getPeer`](#getpeer)  | Get a connected peer by session ID. |
| `std::vector< ServerPeer * >` | [`getPeersInRoom`](#getpeersinroom)  | Get all peers in a room. |
| `size_t` | [`peerCount`](#peercount) `const` | Number of connected, authenticated peers. |
| `void` | [`addVirtualPeer`](#addvirtualpeer)  | Register a virtual peer that receives messages via callback. |
| `void` | [`removeVirtualPeer`](#removevirtualpeer)  | Remove a virtual peer by session ID. |
| `http::Server &` | [`httpServer`](#httpserver) `inline` | Access the underlying HTTP server (e.g. to serve static files). |
| `uv::Loop *` | [`loop`](#loop-9) `const` `inline` | [Event](#event-2) loop that owns the Symple server and all peer connections. |

---

{#server-10}

#### Server

```cpp
Server(uv::Loop * loop)
```

Constructs a server using the given event loop. 
#### Parameters
* `loop` libuv event loop; defaults to [uv::defaultLoop()](uv.md#defaultloop).

---

{#server-11}

#### Server

```cpp
Server(const Server &) = delete
```

Deleted constructor.

---

{#start-17}

#### start

```cpp
void start(const Options & opts)
```

Starts the server with the given options. Begins accepting WebSocket connections on opts.host:opts.port. 
#### Parameters
* `opts` [Server](#server-9) configuration options.

---

{#start-18}

#### start

```cpp
void start(const Options & opts, std::unique_ptr< http::ServerConnectionFactory > httpFactory)
```

Starts the server with a custom HTTP factory for non-WebSocket requests. The Symple server handles WebSocket upgrades internally; any other HTTP request (e.g. static files, REST API) is delegated to this factory. 
#### Parameters
* `opts` [Server](#server-9) configuration options. 

* `httpFactory` Factory for HTTP responders; may be nullptr.

---

{#stop-14}

#### stop

```cpp
void stop()
```

Broadcasts a shutdown notice to all peers, closes the listen socket, and releases all internal state. Safe to call more than once.

---

{#broadcast}

#### broadcast

```cpp
void broadcast(const std::string & room, const json::Value & msg, const std::string & excludeId)
```

Broadcast a message to all peers in a room (excluding sender).

---

{#broadcastrooms}

#### broadcastRooms

```cpp
void broadcastRooms(const std::unordered_set< std::string > & rooms, const json::Value & msg, const std::string & excludeId)
```

Broadcast to multiple rooms with per-recipient dedup.

---

{#sendto}

#### sendTo

```cpp
bool sendTo(const std::string & peerId, const json::Value & msg)
```

Send a message to a specific peer by session ID.

---

{#sendtouser}

#### sendToUser

```cpp
bool sendToUser(const std::string & user, const json::Value & msg)
```

Send a message to any peer with the given user name.

---

{#getpeer}

#### getPeer

```cpp
ServerPeer * getPeer(const std::string & id)
```

Get a connected peer by session ID.

---

{#getpeersinroom}

#### getPeersInRoom

```cpp
std::vector< ServerPeer * > getPeersInRoom(const std::string & room)
```

Get all peers in a room.

---

{#peercount}

#### peerCount

`const`

```cpp
size_t peerCount() const
```

Number of connected, authenticated peers.

---

{#addvirtualpeer}

#### addVirtualPeer

```cpp
void addVirtualPeer(const Peer & peer, const std::vector< std::string > & rooms, std::function< void(const json::Value &)> handler)
```

Register a virtual peer that receives messages via callback.

The virtual peer appears in presence broadcasts and is routable like any WebSocket-connected peer. Messages addressed to it are delivered via the callback instead of a WebSocket connection.

#### Parameters
* `peer` [Peer](#peer-1) data (must have "id", "user", "name" fields). 

* `rooms` Rooms the virtual peer joins. 

* `handler` Called when a message is routed to this peer.

---

{#removevirtualpeer}

#### removeVirtualPeer

```cpp
void removeVirtualPeer(const std::string & peerId)
```

Remove a virtual peer by session ID.

---

{#httpserver}

#### httpServer

`inline`

```cpp
inline http::Server & httpServer()
```

Access the underlying HTTP server (e.g. to serve static files).

---

{#loop-9}

#### loop

`const` `inline`

```cpp
inline uv::Loop * loop() const
```

[Event](#event-2) loop that owns the Symple server and all peer connections.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Options` | [`_opts`](#_opts)  |  |
| `uv::Loop *` | [`_loop`](#_loop-3)  |  |
| `std::unique_ptr< http::Server >` | [`_http`](#_http)  |  |
| `std::unique_ptr< PeerRegistry >` | [`_peerRegistry`](#_peerregistry)  |  |
| `std::unique_ptr< RoomIndex >` | [`_roomIndex`](#_roomindex)  |  |
| `std::mutex` | [`_mutex`](#_mutex-16)  |  |
| `std::atomic< bool >` | [`_shuttingDown`](#_shuttingdown)  |  |
| `std::unique_ptr< http::ServerConnectionFactory >` | [`_httpFallback`](#_httpfallback)  | Fallback factory for non-WebSocket HTTP requests. |

---

{#_opts}

#### _opts

```cpp
Options _opts
```

---

{#_loop-3}

#### _loop

```cpp
uv::Loop * _loop = nullptr
```

---

{#_http}

#### _http

```cpp
std::unique_ptr< http::Server > _http
```

---

{#_peerregistry}

#### _peerRegistry

```cpp
std::unique_ptr< PeerRegistry > _peerRegistry
```

---

{#_roomindex}

#### _roomIndex

```cpp
std::unique_ptr< RoomIndex > _roomIndex
```

---

{#_mutex-16}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_shuttingdown}

#### _shuttingDown

```cpp
std::atomic< bool > _shuttingDown {false}
```

---

{#_httpfallback}

#### _httpFallback

```cpp
std::unique_ptr< http::ServerConnectionFactory > _httpFallback
```

Fallback factory for non-WebSocket HTTP requests.

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onAuth`](#onauth)  |  |
| `void` | [`onMessage`](#onmessage)  |  |
| `void` | [`onJoin`](#onjoin)  |  |
| `void` | [`onLeave`](#onleave)  |  |
| `void` | [`onDisconnect`](#ondisconnect)  |  |
| `void` | [`route`](#route)  |  |
| `bool` | [`deliver`](#deliver)  |  |
| `bool` | [`deliverSerialized`](#deliverserialized)  |  |
| `void` | [`sendPresenceSnapshot`](#sendpresencesnapshot)  |  |

---

{#onauth}

#### onAuth

```cpp
void onAuth(ServerPeer & peer, const json::Value & msg, std::unique_lock< std::mutex > & lock)
```

---

{#onmessage}

#### onMessage

```cpp
void onMessage(ServerPeer & peer, json::Value msg)
```

---

{#onjoin}

#### onJoin

```cpp
void onJoin(ServerPeer & peer, const std::string & room)
```

---

{#onleave}

#### onLeave

```cpp
void onLeave(ServerPeer & peer, const std::string & room)
```

---

{#ondisconnect}

#### onDisconnect

```cpp
void onDisconnect(ServerPeer & peer, std::unique_lock< std::mutex > & lock)
```

---

{#route}

#### route

```cpp
void route(ServerPeer & sender, const json::Value & msg)
```

---

{#deliver}

#### deliver

```cpp
bool deliver(const std::string & peerId, const json::Value & msg)
```

---

{#deliverserialized}

#### deliverSerialized

```cpp
bool deliverSerialized(const std::string & peerId, const char * data, size_t len, const json::Value & msg)
```

---

{#sendpresencesnapshot}

#### sendPresenceSnapshot

```cpp
void sendPresenceSnapshot(ServerPeer & recipient, const std::unordered_set< std::string > & rooms, std::string_view excludeId)
```

{#peerregistry}

## PeerRegistry

```cpp
#include <src/symple/src/server/detail.h>
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`add`](#add-3)  |  |
| `void` | [`addVirtual`](#addvirtual)  |  |
| `void` | [`bind`](#bind-4)  |  |
| `void` | [`unbind`](#unbind)  |  |
| `void` | [`erase`](#erase)  |  |
| `void` | [`eraseVirtual`](#erasevirtual)  |  |
| `void` | [`clear`](#clear-5)  |  |
| `ServerPeer *` | [`find`](#find)  |  |
| `const ServerPeer *` | [`find`](#find-1) `const` |  |
| `VirtualPeer *` | [`findVirtual`](#findvirtual)  |  |
| `const VirtualPeer *` | [`findVirtual`](#findvirtual-1) `const` |  |
| `ServerPeer *` | [`findByConnection`](#findbyconnection)  |  |
| `const ServerPeer *` | [`findByConnection`](#findbyconnection-1) `const` |  |
| `size_t` | [`size`](#size-12) `const` |  |
| `const std::unordered_map< std::string, std::unique_ptr< ServerPeer > > &` | [`peers`](#peers-1) `const` `inline` |  |

---

{#add-3}

#### add

```cpp
void add(std::string id, std::unique_ptr< ServerPeer > peer)
```

---

{#addvirtual}

#### addVirtual

```cpp
void addVirtual(std::string id, VirtualPeer peer)
```

---

{#bind-4}

#### bind

```cpp
void bind(http::ServerConnection & conn, const std::string & id)
```

---

{#unbind}

#### unbind

```cpp
void unbind(http::ServerConnection & conn)
```

---

{#erase}

#### erase

```cpp
void erase(const std::string & id)
```

---

{#erasevirtual}

#### eraseVirtual

```cpp
void eraseVirtual(const std::string & id)
```

---

{#clear-5}

#### clear

```cpp
void clear()
```

---

{#find}

#### find

```cpp
ServerPeer * find(const std::string & id)
```

---

{#find-1}

#### find

`const`

```cpp
const ServerPeer * find(const std::string & id) const
```

---

{#findvirtual}

#### findVirtual

```cpp
VirtualPeer * findVirtual(const std::string & id)
```

---

{#findvirtual-1}

#### findVirtual

`const`

```cpp
const VirtualPeer * findVirtual(const std::string & id) const
```

---

{#findbyconnection}

#### findByConnection

```cpp
ServerPeer * findByConnection(http::ServerConnection & conn)
```

---

{#findbyconnection-1}

#### findByConnection

`const`

```cpp
const ServerPeer * findByConnection(http::ServerConnection & conn) const
```

---

{#size-12}

#### size

`const`

```cpp
size_t size() const
```

---

{#peers-1}

#### peers

`const` `inline`

```cpp
inline const std::unordered_map< std::string, std::unique_ptr< ServerPeer > > & peers() const
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unordered_map< std::string, std::unique_ptr< ServerPeer > >` | [`_peers`](#_peers)  |  |
| `std::unordered_map< std::string, VirtualPeer >` | [`_virtualPeers`](#_virtualpeers)  |  |
| `std::unordered_map< http::ServerConnection *, std::string >` | [`_connToPeer`](#_conntopeer)  |  |

---

{#_peers}

#### _peers

```cpp
std::unordered_map< std::string, std::unique_ptr< ServerPeer > > _peers
```

---

{#_virtualpeers}

#### _virtualPeers

```cpp
std::unordered_map< std::string, VirtualPeer > _virtualPeers
```

---

{#_conntopeer}

#### _connToPeer

```cpp
std::unordered_map< http::ServerConnection *, std::string > _connToPeer
```

{#roomindex}

## RoomIndex

```cpp
#include <src/symple/src/server/detail.h>
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`join`](#join)  |  |
| `void` | [`leave`](#leave)  |  |
| `void` | [`leaveAll`](#leaveall)  |  |
| `void` | [`clear`](#clear-6)  |  |
| `const MemberSet *` | [`members`](#members) `const` |  |
| `std::unordered_set< std::string >` | [`collectRecipients`](#collectrecipients) `const` |  |

---

{#join}

#### join

```cpp
void join(const std::string & room, const std::string & peerId)
```

---

{#leave}

#### leave

```cpp
void leave(const std::string & room, const std::string & peerId)
```

---

{#leaveall}

#### leaveAll

```cpp
void leaveAll(const std::string & peerId)
```

---

{#clear-6}

#### clear

```cpp
void clear()
```

---

{#members}

#### members

`const`

```cpp
const MemberSet * members(const std::string & room) const
```

---

{#collectrecipients}

#### collectRecipients

`const`

```cpp
std::unordered_set< std::string > collectRecipients(const std::unordered_set< std::string > & rooms, std::string_view excludeId) const
```

### Public Types

| Name | Description |
|------|-------------|
| [`MemberSet`](#memberset)  |  |

---

{#memberset}

#### MemberSet

```cpp
std::unordered_set< std::string > MemberSet()
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::unordered_map< std::string, MemberSet >` | [`_rooms`](#_rooms)  |  |

---

{#_rooms}

#### _rooms

```cpp
std::unordered_map< std::string, MemberSet > _rooms
```

{#options-16}

## Options

```cpp
#include <icy/symple/server.h>
```

[Configuration](base.md#configuration) options for the Symple server.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`host`](#host-4)  |  |
| `uint16_t` | [`port`](#port-3)  |  |
| `net::TCPSocket::Ptr` | [`socket`](#socket-7)  | Optional pre-created listen socket (e.g. SSLSocket for HTTPS/WSS). |
| `bool` | [`authentication`](#authentication)  | Require token in auth message. |
| `bool` | [`dynamicRooms`](#dynamicrooms)  | Allow clients to join/leave rooms. |
| `size_t` | [`maxConnections`](#maxconnections)  | Max WebSocket connections (0 = unlimited) |
| `size_t` | [`maxMessageSize`](#maxmessagesize)  | Max message payload in bytes (64KB default) |
| `double` | [`rateLimit`](#ratelimit)  | Messages per rate window. |
| `double` | [`rateSeconds`](#rateseconds)  | Rate window in seconds. |

---

{#host-4}

#### host

```cpp
std::string host = "0.0.0.0"
```

---

{#port-3}

#### port

```cpp
uint16_t port = 4500
```

---

{#socket-7}

#### socket

```cpp
net::TCPSocket::Ptr socket
```

Optional pre-created listen socket (e.g. SSLSocket for HTTPS/WSS).

---

{#authentication}

#### authentication

```cpp
bool authentication = false
```

Require token in auth message.

---

{#dynamicrooms}

#### dynamicRooms

```cpp
bool dynamicRooms = true
```

Allow clients to join/leave rooms.

---

{#maxconnections}

#### maxConnections

```cpp
size_t maxConnections = 0
```

Max WebSocket connections (0 = unlimited)

---

{#maxmessagesize}

#### maxMessageSize

```cpp
size_t maxMessageSize = 64 * 1024
```

Max message payload in bytes (64KB default)

---

{#ratelimit}

#### rateLimit

```cpp
double rateLimit = 100.0
```

Messages per rate window.

---

{#rateseconds}

#### rateSeconds

```cpp
double rateSeconds = 10.0
```

Rate window in seconds.

{#routingpolicy}

## RoutingPolicy

```cpp
#include <src/symple/src/server/detail.h>
```

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`sharesAnyRoom`](#sharesanyroom) `static` |  |
| `bool` | [`canDirectMessage`](#candirectmessage) `static` |  |
| `bool` | [`canDirectMessage`](#candirectmessage-1) `static` |  |
| `bool` | [`canBroadcastToRoom`](#canbroadcasttoroom) `static` |  |

---

{#sharesanyroom}

#### sharesAnyRoom

`static`

```cpp
static bool sharesAnyRoom(const std::unordered_set< std::string > & a, const std::unordered_set< std::string > & b)
```

---

{#candirectmessage}

#### canDirectMessage

`static`

```cpp
static bool canDirectMessage(const ServerPeer & sender, const ServerPeer & recipient)
```

---

{#candirectmessage-1}

#### canDirectMessage

`static`

```cpp
static bool canDirectMessage(const ServerPeer & sender, const VirtualPeer & recipient)
```

---

{#canbroadcasttoroom}

#### canBroadcastToRoom

`static`

```cpp
static bool canBroadcastToRoom(const ServerPeer & sender, const std::string & room)
```

{#virtualpeer}

## VirtualPeer

```cpp
#include <src/symple/src/server/detail.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Peer` | [`peer`](#peer-5)  |  |
| `std::unordered_set< std::string >` | [`rooms`](#rooms-1)  |  |
| `std::function< void(const json::Value &)>` | [`handler`](#handler)  |  |

---

{#peer-5}

#### peer

```cpp
Peer peer
```

---

{#rooms-1}

#### rooms

```cpp
std::unordered_set< std::string > rooms
```

---

{#handler}

#### handler

```cpp
std::function< void(const json::Value &)> handler
```

{#serverpeer}

## ServerPeer

```cpp
#include <icy/symple/server.h>
```

Per-connection state for a connected Symple peer.

Created by the server after successful authentication. Holds the peer data, room memberships, and a reference to the underlying WebSocket connection for sending.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ServerPeer`](#serverpeer-1)  | Constructs a peer bound to the given server-side connection. |
| `void` | [`send`](#send-19)  | Serialises and sends a JSON message over the WebSocket connection. Logs a warning if the send fails; does not throw. |
| `void` | [`sendSerialized`](#sendserialized)  | Sends a pre-serialized JSON payload over the WebSocket connection. Use this on fanout paths that already serialized once. |
| `void` | [`join`](#join-1)  | Adds this peer to the named room (local tracking only). |
| `void` | [`leave`](#leave-1)  | Removes this peer from the named room (local tracking only). |
| `void` | [`leaveAll`](#leaveall-1)  | Removes this peer from all rooms (local tracking only). |
| `Peer &` | [`peer`](#peer-6) `inline` | Returns a mutable reference to the peer data object. |
| `const Peer &` | [`peer`](#peer-7) `const` `inline` | Returns a const reference to the peer data object. |
| `std::string` | [`id`](#id-9) `const` | Returns the session ID assigned to this peer. |
| `const std::unordered_set< std::string > &` | [`rooms`](#rooms-2) `const` `inline` | Returns the set of room names this peer is currently joined to. |
| `bool` | [`authenticated`](#authenticated) `const` `inline` | Returns true if the peer has completed authentication. |
| `void` | [`setAuthenticated`](#setauthenticated) `inline` | Marks the peer as authenticated or unauthenticated. |
| `void` | [`setPeer`](#setpeer) `inline` | Replaces the peer's data object. |
| `http::ServerConnection &` | [`connection`](#connection-10) `inline` | Returns a reference to the underlying server connection. |
| `bool` | [`checkRate`](#checkrate) `inline` | Per-peer rate limiter. Returns false if message should be dropped. |
| `void` | [`setRateLimit`](#setratelimit) `inline` | Configures the per-peer rate limit. |

---

{#serverpeer-1}

#### ServerPeer

```cpp
ServerPeer(http::ServerConnection & conn)
```

Constructs a peer bound to the given server-side connection. 
#### Parameters
* `conn` The underlying WebSocket server connection.

---

{#send-19}

#### send

```cpp
void send(const json::Value & msg)
```

Serialises and sends a JSON message over the WebSocket connection. Logs a warning if the send fails; does not throw. 
#### Parameters
* `msg` JSON value to send.

---

{#sendserialized}

#### sendSerialized

```cpp
void sendSerialized(const char * data, size_t len)
```

Sends a pre-serialized JSON payload over the WebSocket connection. Use this on fanout paths that already serialized once.

---

{#join-1}

#### join

```cpp
void join(const std::string & room)
```

Adds this peer to the named room (local tracking only). 
#### Parameters
* `room` Room name to join.

---

{#leave-1}

#### leave

```cpp
void leave(const std::string & room)
```

Removes this peer from the named room (local tracking only). 
#### Parameters
* `room` Room name to leave.

---

{#leaveall-1}

#### leaveAll

```cpp
void leaveAll()
```

Removes this peer from all rooms (local tracking only).

---

{#peer-6}

#### peer

`inline`

```cpp
inline Peer & peer()
```

Returns a mutable reference to the peer data object.

---

{#peer-7}

#### peer

`const` `inline`

```cpp
inline const Peer & peer() const
```

Returns a const reference to the peer data object.

---

{#id-9}

#### id

`const`

```cpp
std::string id() const
```

Returns the session ID assigned to this peer.

---

{#rooms-2}

#### rooms

`const` `inline`

```cpp
inline const std::unordered_set< std::string > & rooms() const
```

Returns the set of room names this peer is currently joined to.

---

{#authenticated}

#### authenticated

`const` `inline`

```cpp
inline bool authenticated() const
```

Returns true if the peer has completed authentication.

---

{#setauthenticated}

#### setAuthenticated

`inline`

```cpp
inline void setAuthenticated(bool v)
```

Marks the peer as authenticated or unauthenticated. 
#### Parameters
* `v` True to mark as authenticated.

---

{#setpeer}

#### setPeer

`inline`

```cpp
inline void setPeer(const Peer & p)
```

Replaces the peer's data object. 
#### Parameters
* `p` New peer data.

---

{#connection-10}

#### connection

`inline`

```cpp
inline http::ServerConnection & connection()
```

Returns a reference to the underlying server connection.

---

{#checkrate}

#### checkRate

`inline`

```cpp
inline bool checkRate()
```

Per-peer rate limiter. Returns false if message should be dropped.

---

{#setratelimit}

#### setRateLimit

`inline`

```cpp
inline void setRateLimit(double rate, double seconds)
```

Configures the per-peer rate limit. 
#### Parameters
* `rate` Maximum messages allowed per window. 

* `seconds` Duration of the rate window in seconds.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `http::ServerConnection &` | [`_conn`](#_conn)  |  |
| `Peer` | [`_peer`](#_peer-1)  |  |
| `std::unordered_set< std::string >` | [`_rooms`](#_rooms-1)  |  |
| `RateLimiter` | [`_rateLimiter`](#_ratelimiter)  | 100 messages per 10 seconds default |
| `bool` | [`_authenticated`](#_authenticated)  |  |

---

{#_conn}

#### _conn

```cpp
http::ServerConnection & _conn
```

---

{#_peer-1}

#### _peer

```cpp
Peer _peer
```

---

{#_rooms-1}

#### _rooms

```cpp
std::unordered_set< std::string > _rooms
```

---

{#_ratelimiter}

#### _rateLimiter

```cpp
RateLimiter _rateLimiter {100.0, 10.0}
```

100 messages per 10 seconds default

---

{#_authenticated}

#### _authenticated

```cpp
bool _authenticated = false
```

{#address-13}

## Address

```cpp
#include <icy/symple/address.h>
```

The [Address](#address-13) structure is an endpoint identifier for a peer on the network. The format is like so: user|id

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`user`](#user)  |  |
| `std::string` | [`id`](#id-5)  |  |

---

{#user}

#### user

```cpp
std::string user
```

---

{#id-5}

#### id

```cpp
std::string id
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Address`](#address-14)  | Constructs an empty (invalid) address. |
|  | [`Address`](#address-15)  | Parses an address string of the form `user\|id`. |
|  | [`Address`](#address-16)  | Constructs an address from explicit user and session ID components. |
| `bool` | [`parse`](#parse-2)  | Parses an address string of the form `user\|id`. Populates the `user` and `id` fields. |
| `bool` | [`valid`](#valid-10) `const` | Returns true if at least one of `user` or `id` is non-empty. |
| `void` | [`print`](#print-15) `const` | Writes the address in `user\|id` format to the given stream. |
| `std::string` | [`toString`](#tostring-9) `const` | Returns the address as a string in `user\|id` format. |
| `bool` | [`operator==`](#operator-27) `const` | Compares two addresses for equality (both user and id must match). |
| `bool` | [`operator==`](#operator-28) `const` | Compares this address against a string in `user\|id` format without allocating. |

---

{#address-14}

#### Address

```cpp
Address()
```

Constructs an empty (invalid) address.

---

{#address-15}

#### Address

```cpp
Address(std::string_view addr)
```

Parses an address string of the form `user|id`. 
#### Parameters
* `addr` [Address](#address-13) string to parse.

---

{#address-16}

#### Address

```cpp
Address(const std::string & user, const std::string & id)
```

Constructs an address from explicit user and session ID components. 
#### Parameters
* `user` User identifier. 

* `id` Session ID.

---

{#parse-2}

#### parse

```cpp
bool parse(std::string_view addr)
```

Parses an address string of the form `user|id`. Populates the `user` and `id` fields. 
#### Parameters
* `addr` [Address](#address-13) string to parse. 

#### Returns
True if the result is a valid address.

---

{#valid-10}

#### valid

`const`

```cpp
bool valid() const
```

Returns true if at least one of `user` or `id` is non-empty.

---

{#print-15}

#### print

`const`

```cpp
void print(std::ostream & os) const
```

Writes the address in `user|id` format to the given stream. 
#### Parameters
* `os` Output stream.

---

{#tostring-9}

#### toString

`const`

```cpp
std::string toString() const
```

Returns the address as a string in `user|id` format. 
#### Returns
Serialised address string.

---

{#operator-27}

#### operator==

`const`

```cpp
bool operator==(const Address & r) const
```

Compares two addresses for equality (both user and id must match). 
#### Parameters
* `r` [Address](#address-13) to compare against.

---

{#operator-28}

#### operator==

`const`

```cpp
bool operator==(const std::string & r) const
```

Compares this address against a string in `user|id` format without allocating. 
#### Parameters
* `r` String to compare against.

{#clientstate-1}

## ClientState

```cpp
#include <icy/symple/client.h>
```

> **Inherits:** [`State`](base.md#state)

[Client](#client-5) connection states.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`str`](#str-3) `const` `inline` |  |

---

{#str-3}

#### str

`const` `inline`

```cpp
inline std::string str(unsigned int id) const
```

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-20)  |  |

---

{#type-20}

#### Type

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `Closed` |  |
| `Connecting` |  |
| `Authenticating` |  |
| `Online` |  |
| `Error` |  |

