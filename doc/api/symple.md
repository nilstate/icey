# symple 

### Classes

| Name | Description |
|------|-------------|
| [`Client`](#classicy_1_1smpl_1_1Client) |  |
| [`TCPClient`](#classicy_1_1smpl_1_1TCPClient) |  |
| [`SSLClient`](#classicy_1_1smpl_1_1SSLClient) |  |
| [`Command`](#classicy_1_1smpl_1_1Command) |  |
| [`Event`](#classicy_1_1smpl_1_1Event) |  |
| [`FormElement`](#classicy_1_1smpl_1_1FormElement) |  |
| [`Form`](#classicy_1_1smpl_1_1Form) |  |
| [`FormField`](#classicy_1_1smpl_1_1FormField) |  |
| [`Message`](#classicy_1_1smpl_1_1Message) |  |
| [`Peer`](#classicy_1_1smpl_1_1Peer) | This class represents a peer on the network. A [Peer](#classicy_1_1smpl_1_1Peer) object may also contain arbitrary data set by the client to share with other peers on the network. **See also**: [Address](#structicy_1_1smpl_1_1Address) for further methods and [basic](./doc/api-undefined.md#namespaceicy_1_1basic) accessors. |
| [`Presence`](#classicy_1_1smpl_1_1Presence) |  |
| [`Roster`](#classicy_1_1smpl_1_1Roster) | The [Roster](#classicy_1_1smpl_1_1Roster) provides a registry for active network peers indexed by session ID. |
| [`Address`](#structicy_1_1smpl_1_1Address) | The [Address](#structicy_1_1smpl_1_1Address) structure is an endpoint identifier for a peer on the network. The format is like so: user|id |
| [`Options`](#structicy_1_1smpl_1_1Client_1_1Options) |  |
| [`Filter`](#structicy_1_1smpl_1_1Filter) |  |

### Members

| Name | Description |
|------|-------------|
| [`Symple_API`](#group__symple_1gabbd11164ccdcea231be5cb5fe26438ea) |  |

---

#### Symple_API 

```cpp
Symple_API()
```

## Client 

> **Extends:** `icy::sockio::Client`
> **Subclasses:** `icy::smpl::SSLClient`, `icy::smpl::TCPClient`
> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Client`](#group__symple_1ga5b46d09313a3360148246158e11dce4c) |  |
| [`~Client`](#group__symple_1ga1eb6bbd30e54e4c0e918c7ce43fadeec) |  |
| [`connect`](#group__symple_1gaa8c97bf06d3fc80ceea252a2a611c5c7) |  |
| [`close`](#group__symple_1gae27e4556310a4b935ffe78b62bbe3a9c) |  |
| [`send`](#group__symple_1ga4dffb71d16288852dbeb54e38ce9cc82) | Send a message. May be a polymorphic [Command](#classicy_1_1smpl_1_1Command), [Presence](#classicy_1_1smpl_1_1Presence), [Event](#classicy_1_1smpl_1_1Event) or other ... |
| [`send`](#group__symple_1ga6e121b7e49c36023f49190c9ab5bebef) | Send a string message. The message must be a valid Symple message otherwise it will not be delivered. |
| [`createTransaction`](#group__symple_1ga51add0f918bee39b510fad050c973db9) | Create a Transaction object with the given message which will notify on Ack response from the server. |
| [`respond`](#group__symple_1gaea0ec59bcf35da1f20a3cd46adf1e4f4) | Swap the 'to' and 'from' fields and send the given message. |
| [`sendPresence`](#group__symple_1gaa2b2c55f34473f05b397c88e6a1f1e19) | Broadcast presence to the user group scope. The outgoing [Presence](#classicy_1_1smpl_1_1Presence) object may be modified via the CreatePresence signal. |
| [`sendPresence`](#group__symple_1gaaf17806e06f596c196e007c5e98a0ea6) | Send directed presence to the given peer. |
| [`joinRoom`](#group__symple_1ga426afee6e4914560d5b82fb9098898d6) | Join the given room. |
| [`leaveRoom`](#group__symple_1ga7d696ae2396d9da0d87c7d0ed464b579) | Leave the given room. |
| [`ourID`](#group__symple_1gaeb1cebadd1f004d6b259c4831ff0901a) | Return the session ID of our current peer object. Return an empty string when offline. |
| [`rooms`](#group__symple_1ga468d33d5531670856a74208ecca2e8f6) | Return a list of rooms the client has joined. |
| [`ourPeer`](#group__symple_1ga680bfb6b0db4f75e9a863b61b61d734b) | Return the peer object for the current session, or throws an exception when offline. |
| [`roster`](#group__symple_1ga6998f1375a4767e630a98f60cb304300) | Return the roster which stores all online peers. |
| [`persistence`](#group__symple_1ga7ee28acdc40837302c167b543fc56a15) | Return the persistence manager which stores long lived messages. |
| [`options`](#group__symple_1ga6272271fea7d57a5719e2da9163b3224) | Return a reference to the client options object. |
| [`operator>>`](#group__symple_1ga991c5b0bbcca7d16536b0dccce4107d2) | [Stream](./doc/api-base.md#classicy_1_1Stream) operator alias for [send()](#group__symple_1ga4dffb71d16288852dbeb54e38ce9cc82). |
| [`onPresenceData`](#group__symple_1gaac8e9b77fda748176c2c99787b4d4355) | Update the roster from the given client object. |
| [`announce`](#group__symple_1gadf0ac053e026a7c792d00ac165cb1298) | Called when a new connection is established to announce and authenticate the peer on the server. |
| [`reset`](#group__symple_1gafe1d7b3e16488e4a61f6fc045601dc2a) | Resets variables and data at the beginning and end of each session. |
| [`createPresence`](#group__symple_1gace2ee9e3f211f1fb9979953c6d0dfa34) | Creates a [Presence](#classicy_1_1smpl_1_1Presence) object. |
| [`emit`](#group__symple_1ga805c44b00ddf3dfeba913dea6a03330d) | Override PacketSignal::emit. |
| [`onOnline`](#group__symple_1ga34e2a7ff6cc2292a40102a0d51917cd6) |  |
| [`onAnnounceState`](#group__symple_1gae30976d64dc3e2b64b470eb0774fdf16) |  |
| [`Announce`](#group__symple_1ga0228d93d0cedc21b61f12a580bb7c6b9) | Signals Notifies the outside application about the response status code of our [announce()](#group__symple_1gadf0ac053e026a7c792d00ac165cb1298) call. Possible status codes are: // - 200: Authentication success |
| [`PeerConnected`](#group__symple_1ga01d918d49dcf259d612543a353a7e98e) | Signals when a peer connects. |
| [`PeerDisconnected`](#group__symple_1ga317fcf03f379f086db4b77bbc58ba7f6) | Signals when a peer disconnects. |
| [`CreatePresence`](#group__symple_1ga026c3952b78bd1b4b4f18d9bd815e94e) | Called by [createPresence()](#group__symple_1gace2ee9e3f211f1fb9979953c6d0dfa34) so outside classes can modify the outgoing [Peer](#classicy_1_1smpl_1_1Peer) JSON object. |
| [`_roster`](#group__symple_1ga8a6dc184e85eb02e1180a6ff11af402f) |  |
| [`_ourID`](#group__symple_1ga7aa335a7730f803fabab6a0ac7bf629f) |  |
| [`_persistence`](#group__symple_1ga0ecdbbbdc4f57cac6e7b5fda9072f5fa) |  |
| [`_options`](#group__symple_1gadfb98483f4d159dc718c788f787d6adf) |  |
| [`_rooms`](#group__symple_1ga362e9d47266c26dcf32e1603e6d39a85) |  |
| [`_announceStatus`](#group__symple_1gacece49388fbe130519f6b58232dd8067) |  |

---

#### Client 

```cpp
Client(const net::Socket::Ptr & socket, const Options & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::Socket::Ptr &` |  |
| `options` | `const Options &` |  |

---

#### ~Client 

```cpp
virtual ~Client()
```

---

#### connect 

```cpp
virtual void connect()
```

---

#### close 

```cpp
virtual void close()
```

---

#### send 

```cpp
virtual int send(Message & message, bool ack)
```

Send a message. May be a polymorphic [Command](#classicy_1_1smpl_1_1Command), [Presence](#classicy_1_1smpl_1_1Presence), [Event](#classicy_1_1smpl_1_1Event) or other ...

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `Message &` |  |
| `ack` | `bool` |  |

---

#### send 

```cpp
virtual int send(const std::string & message, bool ack)
```

Send a string message. The message must be a valid Symple message otherwise it will not be delivered.

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `const std::string &` |  |
| `ack` | `bool` |  |

---

#### createTransaction 

```cpp
sockio::Transaction * createTransaction(Message & message)
```

Create a Transaction object with the given message which will notify on Ack response from the server.

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `Message &` |  |

---

#### respond 

```cpp
virtual int respond(Message & message, bool ack)
```

Swap the 'to' and 'from' fields and send the given message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `Message &` |  |
| `ack` | `bool` |  |

---

#### sendPresence 

```cpp
virtual int sendPresence(bool probe)
```

Broadcast presence to the user group scope. The outgoing [Presence](#classicy_1_1smpl_1_1Presence) object may be modified via the CreatePresence signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `probe` | `bool` |  |

---

#### sendPresence 

```cpp
virtual int sendPresence(const Address & to, bool probe)
```

Send directed presence to the given peer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `to` | `const Address &` |  |
| `probe` | `bool` |  |

---

#### joinRoom 

```cpp
virtual int joinRoom(const std::string & room)
```

Join the given room.

| Parameter | Type | Description |
|-----------|------|-------------|
| `room` | `const std::string &` |  |

---

#### leaveRoom 

```cpp
virtual int leaveRoom(const std::string & room)
```

Leave the given room.

| Parameter | Type | Description |
|-----------|------|-------------|
| `room` | `const std::string &` |  |

---

#### ourID 

```cpp
virtual std::string ourID() const
```

Return the session ID of our current peer object. Return an empty string when offline.

---

#### rooms 

```cpp
StringVec rooms() const
```

Return a list of rooms the client has joined.

---

#### ourPeer 

```cpp
virtual Peer * ourPeer()
```

Return the peer object for the current session, or throws an exception when offline.

---

#### roster 

```cpp
virtual Roster & roster()
```

Return the roster which stores all online peers.

---

#### persistence 

```cpp
virtual PersistenceT & persistence()
```

Return the persistence manager which stores long lived messages.

---

#### options 

```cpp
Client::Options & options()
```

Return a reference to the client options object.

---

#### operator>> 

```cpp
virtual Client & operator>>(Message & message)
```

[Stream](./doc/api-base.md#classicy_1_1Stream) operator alias for [send()](#group__symple_1ga4dffb71d16288852dbeb54e38ce9cc82).

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `Message &` |  |

---

#### onPresenceData 

```cpp
virtual void onPresenceData(const json::value & data, bool whiny)
```

Update the roster from the given client object.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const json::value &` |  |
| `whiny` | `bool` |  |

---

#### announce 

```cpp
virtual int announce()
```

Called when a new connection is established to announce and authenticate the peer on the server.

---

#### reset 

```cpp
virtual void reset()
```

Resets variables and data at the beginning and end of each session.

---

#### createPresence 

```cpp
virtual void createPresence(Presence & p)
```

Creates a [Presence](#classicy_1_1smpl_1_1Presence) object.

| Parameter | Type | Description |
|-----------|------|-------------|
| `p` | `Presence &` |  |

---

#### emit 

```cpp
virtual void emit(IPacket & packet)
```

Override PacketSignal::emit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### onOnline 

```cpp
virtual void onOnline()
```

---

#### onAnnounceState 

```cpp
virtual void onAnnounceState(void * sender, TransactionState & state, const TransactionState &)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `state` | `TransactionState &` |  |

---

#### Announce 

```cpp
Signal< void(const int &)> Announce
```

Signals Notifies the outside application about the response status code of our [announce()](#group__symple_1gadf0ac053e026a7c792d00ac165cb1298) call. Possible status codes are: // - 200: Authentication success

* 401: Authentication failed

* 400: Bad request data

* 500: Server not found

---

#### PeerConnected 

```cpp
Signal< void(Peer &)> PeerConnected
```

Signals when a peer connects.

---

#### PeerDisconnected 

```cpp
Signal< void(Peer &)> PeerDisconnected
```

Signals when a peer disconnects.

---

#### CreatePresence 

```cpp
Signal< void(Peer &)> CreatePresence
```

Called by [createPresence()](#group__symple_1gace2ee9e3f211f1fb9979953c6d0dfa34) so outside classes can modify the outgoing [Peer](#classicy_1_1smpl_1_1Peer) JSON object.

---

#### _roster 

```cpp
Roster _roster
```

---

#### _ourID 

```cpp
std::string _ourID
```

---

#### _persistence 

```cpp
PersistenceT _persistence
```

---

#### _options 

```cpp
Client::Options _options
```

---

#### _rooms 

```cpp
StringVec _rooms
```

---

#### _announceStatus 

```cpp
int _announceStatus
```

## Options 

> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__symple_1ga7dd4fee81e4318912e9d82a1e35c6550) |  |
| [`host`](#group__symple_1ga05d0bdf5ab5eeefec7d0019bb110bb58) |  |
| [`port`](#group__symple_1ga1b77c19ba3010b95adab9572de8812d4) |  |
| [`reconnection`](#group__symple_1ga8388c0e51b717793da8e136f24ef4df6) | Weather or not to reconnect if disconnected from the server. |
| [`reconnectAttempts`](#group__symple_1gaad73b34182da94bd80d394d6c2cf8d59) | The number of times to attempt to reconnect if disconnected from the server. (0 = unlimited) |
| [`user`](#group__symple_1gace5d72442ca0ddd400242766dcbe0075) |  |
| [`name`](#group__symple_1ga937a706d0ff802b29881200dd0f9b140) |  |
| [`type`](#group__symple_1ga3a94f72a2f8e72b1bb84f11630f9cd9a) |  |
| [`token`](#group__symple_1gae46b1ce6b2a9c5e8ae983cf36137d18d) |  |

---

#### Options 

```cpp
inline Options()
```

---

#### host 

```cpp
std::string host = "127.0.0.1"
```

---

#### port 

```cpp
uint16_t port = 4500
```

---

#### reconnection 

```cpp
bool reconnection = true
```

Weather or not to reconnect if disconnected from the server.

---

#### reconnectAttempts 

```cpp
int reconnectAttempts = 0
```

The number of times to attempt to reconnect if disconnected from the server. (0 = unlimited)

---

#### user 

```cpp
std::string user
```

---

#### name 

```cpp
std::string name
```

---

#### type 

```cpp
std::string type
```

---

#### token 

```cpp
std::string token
```

## TCPClient 

> **Extends:** `icy::smpl::Client`
> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`TCPClient`](#group__symple_1gafc1541506c7fded90f495818835590b4) |  |

---

#### TCPClient 

```cpp
TCPClient(const Client::Options & options, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `const Client::Options &` |  |
| `loop` | `uv::Loop *` |  |

## SSLClient 

> **Extends:** `icy::smpl::Client`
> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`SSLClient`](#group__symple_1ga03327af0bc01ae4e7a56affb83991aad) |  |

---

#### SSLClient 

```cpp
SSLClient(const Client::Options & options, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `const Client::Options &` |  |
| `loop` | `uv::Loop *` |  |

## Command 

> **Extends:** `icy::smpl::Message`
> **Defined in:** `command.h`

### Members

| Name | Description |
|------|-------------|
| [`Command`](#group__symple_1ga1d83f2b5a9be1780e3b77f5c96b34a9a) |  |
| [`Command`](#group__symple_1ga91132752945caa9ada473cb1f820bd39) |  |
| [`Command`](#group__symple_1ga2f980c97f7fdfee4882f8923cb2853c2) |  |
| [`~Command`](#group__symple_1ga964d5a75ca62c76ff2c27e9236f90e00) |  |
| [`node`](#group__symple_1gac5f9bb1710c74e1f8281c267ecf04c5f) |  |
| [`action`](#group__symple_1ga81df2719cd693fbda9e76dff7027a654) |  |
| [`setNode`](#group__symple_1ga904c2957be56206f513c33f92f89bcad) |  |
| [`setAction`](#group__symple_1ga276a75d047efd959d77cfe473f8a1c58) |  |
| [`valid`](#group__symple_1gad76ae409faa59e3300da80f7c745aec2) |  |
| [`param`](#group__symple_1ga1611d5b3bdcc91574bae67dfbde437ae) |  |
| [`params`](#group__symple_1ga43384ed4f7303ac921bcf65e6f18db26) |  |
| [`matches`](#group__symple_1gae80a575c75e000c040d320ead0896323) |  |

---

#### Command 

```cpp
Command()
```

---

#### Command 

```cpp
Command(const json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const json::value &` |  |

---

#### Command 

```cpp
Command(const Command & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const Command &` |  |

---

#### ~Command 

```cpp
virtual ~Command()
```

---

#### node 

```cpp
std::string node() const
```

---

#### action 

```cpp
std::string action() const
```

---

#### setNode 

```cpp
void setNode(const std::string & node)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `node` | `const std::string &` |  |

---

#### setAction 

```cpp
void setAction(const std::string & action)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `action` | `const std::string &` |  |

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### param 

```cpp
std::string param(int n) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `int` |  |

---

#### params 

```cpp
std::vector< std::string > params()
```

---

#### matches 

```cpp
bool matches(const std::string & xnode) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `xnode` | `const std::string &` |  |

## Event 

> **Extends:** `icy::smpl::Message`
> **Defined in:** `event.h`

### Members

| Name | Description |
|------|-------------|
| [`Event`](#group__symple_1gafe163cd025c2c0dafa7e779df5f5962d) |  |
| [`Event`](#group__symple_1ga801abee141af76f34f96bd282fa9f1d3) |  |
| [`Event`](#group__symple_1gae6aaee01e127e7de476f49d0ecb9f8c7) |  |
| [`~Event`](#group__symple_1ga9b9bb5b13b492ec09f88e72249a83e3b) |  |
| [`valid`](#group__symple_1ga365f48210b899bf487ac334a6b50bb6a) |  |
| [`name`](#group__symple_1gaf4898557f256d37c2ec91dae453ba244) |  |
| [`time`](#group__symple_1ga3f9805f507320d2a864fe81eaac155b5) |  |
| [`setName`](#group__symple_1ga97e213f54bcc541345da6867f3f083ed) |  |
| [`setTime`](#group__symple_1gab3e4b9fa8d12289de33477c9ac68330a) |  |

---

#### Event 

```cpp
Event()
```

---

#### Event 

```cpp
Event(const json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const json::value &` |  |

---

#### Event 

```cpp
Event(const Event & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const Event &` |  |

---

#### ~Event 

```cpp
virtual ~Event()
```

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### name 

```cpp
std::string name() const
```

---

#### time 

```cpp
std::time_t time() const
```

---

#### setName 

```cpp
void setName(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setTime 

```cpp
void setTime(std::time_t time)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | `std::time_t` |  |

## FormElement 

> **Subclasses:** `icy::smpl::Form`, `icy::smpl::FormField`
> **Defined in:** `form.h`

### Members

| Name | Description |
|------|-------------|
| [`FormElement`](#group__symple_1ga7e047bc6b6e46abe01a946a82745ee11) |  |
| [`FormElement`](#group__symple_1ga3ef7f1f4de265f10ddcc83061e21ab77) |  |
| [`FormElement`](#group__symple_1ga94f85597aa5b95ef7dcc246ef41787b9) |  |
| [`operator=`](#group__symple_1ga0d7c97e98878eb69ebd8830df6a01e58) |  |
| [`~FormElement`](#group__symple_1ga2c7bf72c2e0e6650fe27a68cd5e1beec) |  |
| [`type`](#group__symple_1ga3b53f1858ddcc524004726f4d6a463b4) |  |
| [`id`](#group__symple_1ga0eacc59669c12ecf4faf45395cc08f17) |  |
| [`label`](#group__symple_1ga0bf79407646db8313d409ccc9a2f4ba8) |  |
| [`setType`](#group__symple_1gae723f824695861e34898005f225e12b7) | Possible "type" values page, section, text, text-multi, list, list-multi, checkbox, media, custom |
| [`setId`](#group__symple_1gaba4de61935c0d3795d537964f47c33d0) |  |
| [`setLabel`](#group__symple_1ga52798e6759dd37b1116720a37a2c967d) |  |
| [`setHint`](#group__symple_1ga1a9595540a1969bb17a3115be6e26df6) |  |
| [`setError`](#group__symple_1gaf621f96173fcbe523d419fd4a3a87f3e) | Sets and optional validation error message. |
| [`addPage`](#group__symple_1ga432f56acec954ed84897f7fc01059813) |  |
| [`addSection`](#group__symple_1gab0619bb63ac7241e61c36f69bbc4ed07) |  |
| [`addField`](#group__symple_1ga474c9727e83a9ce0e0b206d246379d6c) |  |
| [`getField`](#group__symple_1gac9715a4fffff97d29092698755242cab) |  |
| [`getField`](#group__symple_1gad7d9fdada931140ef706d58036afb0fe) |  |
| [`hasField`](#group__symple_1gae28386fbc730fd60fea605384563dc1c) | Returns true if the given [Address](#structicy_1_1smpl_1_1Address) matches any of the internal form element IDs. If the partial flag is set then substring matches will be counted. |
| [`setLive`](#group__symple_1ga88878fc79bdeed108b06703d29fab26b) | Live fields or elements are used to submit partial sections a form, without sending the entire form. |
| [`live`](#group__symple_1gace9f6fd60c701c643d660971c159ce43) | Returns true if this field is live, meaning the form-processing entity should auto-update this field's value whenever it changes. |
| [`clearElements`](#group__symple_1gac2ee4183b9d5465c0036ee4a4c38e562) | Clears child elements matching the given ID. |
| [`clear`](#group__symple_1ga4cf4750e263530a525b89755b20677c6) | Clear the entire form. |
| [`valid`](#group__symple_1ga47cf107cf15ea93bdb9f377253f76aa6) | Returns true if the form element is valid. |
| [`numElements`](#group__symple_1gaae3b7577b27fb174619fc68801607ea1) | Returns the number of child elements. |
| [`hasErrors`](#group__symple_1ga50aa0c3f85c44063771f28b75de0a428) | Returns true if any fields have errors. |
| [`hasPages`](#group__symple_1ga61e65544fa1f0357bc0721864cc54768) | Returns true if the form has multiple pages. |
| [`root`](#group__symple_1gaade1f7bef1e9dcc71718a22cbc7ffa2d) |  |
| [`_root`](#group__symple_1ga16c6666fcbd0fa750e6e14b2464a3ad5) | The root pointer is just a reference to the externally managed JSON value memory. |

---

#### FormElement 

```cpp
FormElement()
```

---

#### FormElement 

```cpp
FormElement(json::value & root, const std::string & type, const std::string & id, const std::string & label)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |
| `type` | `const std::string &` |  |
| `id` | `const std::string &` |  |
| `label` | `const std::string &` |  |

---

#### FormElement 

```cpp
FormElement(const FormElement & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const FormElement &` |  |

---

#### operator= 

```cpp
FormElement & operator=(const FormElement & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const FormElement &` |  |

---

#### ~FormElement 

```cpp
virtual ~FormElement()
```

---

#### type 

```cpp
std::string type() const
```

---

#### id 

```cpp
std::string id() const
```

---

#### label 

```cpp
std::string label() const
```

---

#### setType 

```cpp
void setType(const std::string & type)
```

Possible "type" values page, section, text, text-multi, list, list-multi, checkbox, media, custom

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### setId 

```cpp
void setId(const std::string & id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### setLabel 

```cpp
void setLabel(const std::string & text)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `const std::string &` |  |

---

#### setHint 

```cpp
void setHint(const std::string & text)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `const std::string &` |  |

---

#### setError 

```cpp
void setError(const std::string & error)
```

Sets and optional validation error message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const std::string &` |  |

---

#### addPage 

```cpp
FormElement addPage(const std::string & id, const std::string & label)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `label` | `const std::string &` |  |

---

#### addSection 

```cpp
FormElement addSection(const std::string & id, const std::string & label)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `label` | `const std::string &` |  |

---

#### addField 

```cpp
FormField addField(const std::string & type, const std::string & id, const std::string & label)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `id` | `const std::string &` |  |
| `label` | `const std::string &` |  |

---

#### getField 

```cpp
FormField getField(const std::string & id, bool partial)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `partial` | `bool` |  |

---

#### getField 

```cpp
bool getField(const std::string & id, FormField & field, bool partial)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `field` | `FormField &` |  |
| `partial` | `bool` |  |

---

#### hasField 

```cpp
bool hasField(const std::string & id, bool partial)
```

Returns true if the given [Address](#structicy_1_1smpl_1_1Address) matches any of the internal form element IDs. If the partial flag is set then substring matches will be counted.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `partial` | `bool` |  |

---

#### setLive 

```cpp
void setLive(bool flag)
```

Live fields or elements are used to submit partial sections a form, without sending the entire form.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### live 

```cpp
bool live() const
```

Returns true if this field is live, meaning the form-processing entity should auto-update this field's value whenever it changes.

---

#### clearElements 

```cpp
bool clearElements(const std::string & id, bool partial)
```

Clears child elements matching the given ID.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `partial` | `bool` |  |

---

#### clear 

```cpp
void clear()
```

Clear the entire form.

---

#### valid 

```cpp
bool valid() const
```

Returns true if the form element is valid.

---

#### numElements 

```cpp
int numElements()
```

Returns the number of child elements.

---

#### hasErrors 

```cpp
bool hasErrors()
```

Returns true if any fields have errors.

---

#### hasPages 

```cpp
bool hasPages()
```

Returns true if the form has multiple pages.

---

#### root 

```cpp
json::value & root() const
```

---

#### _root 

```cpp
json::value * _root
```

The root pointer is just a reference to the externally managed JSON value memory.

## Form 

> **Extends:** `icy::smpl::FormElement`
> **Defined in:** `form.h`

### Members

| Name | Description |
|------|-------------|
| [`Form`](#group__symple_1ga68e4093955870a7823a1a0e2fb32781a) |  |
| [`Form`](#group__symple_1gab2fed5a941e921052b77cf4e94120f22) |  |
| [`Form`](#group__symple_1gaa8a3c6e4f9bca2257fdae351d7621a45) |  |
| [`~Form`](#group__symple_1ga78e6a83ae030945f05c990ea856d7281) |  |
| [`valid`](#group__symple_1ga0259ae314e90190494940841638b4a0e) |  |
| [`action`](#group__symple_1ga2974536cdcca932554f986ed1dbd7e58) |  |
| [`partial`](#group__symple_1ga25bd29f8f64828762cfeb0d44a2be46c) |  |
| [`setAction`](#group__symple_1ga8bc4b676485d8e4e28da58b2f941acb9) | Possible "action" values `form` The form-processing entity is asking the form-submitting entity to complete a form. `submit` The form-submitting entity is submitting data to the form-processing entity. `cancel` The form-submitting entity has cancelled submission of data to the form-processing entity. `result` The form-processing entity is returning data to the form-submitting entity. |
| [`setPartial`](#group__symple_1ga828e2ab5535b743420349e7bb1ff59f9) | Notifies the form is a partial section of the form. This is used for transmitting and updating live fields such as for auto-complete. |

---

#### Form 

```cpp
Form()
```

---

#### Form 

```cpp
Form(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### Form 

```cpp
Form(Command & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `Command &` |  |

---

#### ~Form 

```cpp
virtual ~Form()
```

---

#### valid 

```cpp
bool valid()
```

---

#### action 

```cpp
std::string action() const
```

---

#### partial 

```cpp
bool partial() const
```

---

#### setAction 

```cpp
void setAction(const std::string & action)
```

Possible "action" values `form` The form-processing entity is asking the form-submitting entity to complete a form. `submit` The form-submitting entity is submitting data to the form-processing entity. `cancel` The form-submitting entity has cancelled submission of data to the form-processing entity. `result` The form-processing entity is returning data to the form-submitting entity.

| Parameter | Type | Description |
|-----------|------|-------------|
| `action` | `const std::string &` |  |

---

#### setPartial 

```cpp
void setPartial(bool flag)
```

Notifies the form is a partial section of the form. This is used for transmitting and updating live fields such as for auto-complete.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

## FormField 

> **Extends:** `icy::smpl::FormElement`
> **Defined in:** `form.h`

### Members

| Name | Description |
|------|-------------|
| [`FormField`](#group__symple_1gaf9378e4d88099d33f5c73cd6e5301841) |  |
| [`FormField`](#group__symple_1gabe61a857e005af10301b5ac987d0946d) |  |
| [`~FormField`](#group__symple_1gac0c5aeeef91f25f93d981deff77382dd) |  |
| [`addOption`](#group__symple_1ga26777a113bae5c3083906e77b62d13f6) | Adds an option for list based fields. |
| [`addOption`](#group__symple_1gaf3d1bf938ba0cbeade0da2a18543a35d) |  |
| [`setValue`](#group__symple_1gaeee92fee4944e5013ebef43eee2b9183) | Sets the value clearing all other values. |
| [`setValue`](#group__symple_1ga170d9a5ddc51953f406331b6da779bcf) |  |
| [`setValue`](#group__symple_1gaa29f083206c8cbf9005f730f61b55dad) |  |
| [`setValue`](#group__symple_1gaee3ee4b29024eee4fc34e1dded5267b8) |  |
| [`addValue`](#group__symple_1ga7cd9073b9e7c2a476f17104d6e9ce8fb) | Appends a value to the value array. |
| [`addValue`](#group__symple_1ga89b16c8639181a01f676ea4ec1d48cbc) |  |
| [`addValue`](#group__symple_1ga686348a5aacfc900b9a4d3d3aea903ad) |  |
| [`addValue`](#group__symple_1ga89eaf8cd95b3a7f081e30f154720e883) |  |
| [`values`](#group__symple_1ga5f80c4930f8fb7127d42f7af3ceb93aa) | Returns a JSON array of all values. |
| [`value`](#group__symple_1gae0051a3119d85c46034f1167a35fc9e5) | Returns the first value. Most formats (except multi) only require single value. |
| [`intValue`](#group__symple_1ga6806b9902742ea0bfe06e2a4a3e18cb1) |  |
| [`doubleValue`](#group__symple_1ga744a0a50084034c5179a92b6c5538956) |  |
| [`boolValue`](#group__symple_1ga6d0f293a15bce06dc455fc7535619eef) |  |

---

#### FormField 

```cpp
FormField()
```

---

#### FormField 

```cpp
FormField(json::value & root, const std::string & type, const std::string & id, const std::string & label)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |
| `type` | `const std::string &` |  |
| `id` | `const std::string &` |  |
| `label` | `const std::string &` |  |

---

#### ~FormField 

```cpp
virtual ~FormField()
```

---

#### addOption 

```cpp
void addOption(const std::string & key, const std::string & value)
```

Adds an option for list based fields.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### addOption 

```cpp
void addOption(const std::string & value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### setValue 

```cpp
void setValue(const std::string & value)
```

Sets the value clearing all other values.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### setValue 

```cpp
void setValue(int value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `int` |  |

---

#### setValue 

```cpp
void setValue(double value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `double` |  |

---

#### setValue 

```cpp
void setValue(bool value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `bool` |  |

---

#### addValue 

```cpp
void addValue(const std::string & value)
```

Appends a value to the value array.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### addValue 

```cpp
void addValue(int value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `int` |  |

---

#### addValue 

```cpp
void addValue(double value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `double` |  |

---

#### addValue 

```cpp
void addValue(bool value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `bool` |  |

---

#### values 

```cpp
json::value & values()
```

Returns a JSON array of all values.

---

#### value 

```cpp
std::string value() const
```

Returns the first value. Most formats (except multi) only require single value.

---

#### intValue 

```cpp
int intValue() const
```

---

#### doubleValue 

```cpp
double doubleValue() const
```

---

#### boolValue 

```cpp
bool boolValue() const
```

## Message 

> **Extends:** `json::value`, `icy::IPacket`
> **Subclasses:** `icy::smpl::Command`, `icy::smpl::Event`, `icy::smpl::Presence`
> **Defined in:** `message.h`

### Members

| Name | Description |
|------|-------------|
| [`Message`](#group__symple_1ga993977c8fc55c8b8b8ec172b720df218) |  |
| [`Message`](#group__symple_1gaf736d72cc20d17cc96035a9ca153f7f9) |  |
| [`Message`](#group__symple_1gad7ccee50283b43fb6203d5d38655b5a8) |  |
| [`~Message`](#group__symple_1ga6a2eb621f26dda23a4e00cf085e28733) |  |
| [`clone`](#group__symple_1ga7772d48d3c40973f8ba273404efbc0f5) |  |
| [`valid`](#group__symple_1ga6bc8ec508472b3a845517e37a7d9ecee) |  |
| [`clear`](#group__symple_1gab6a00a852821506f2003ea5916e0937f) |  |
| [`clearData`](#group__symple_1ga004941b724014d459a60784e9f638d62) |  |
| [`clearNotes`](#group__symple_1ga2a2409a6b3bbff22b57394f3c64f232f) |  |
| [`type`](#group__symple_1ga238a19ce026c212bbbb53242e22e0b35) |  |
| [`id`](#group__symple_1ga36664179f4acfcacf616fdbae6f42cc3) |  |
| [`to`](#group__symple_1ga91b5760d36713eb890827f069f82b6c5) |  |
| [`from`](#group__symple_1ga97accfebe0c98bf39caa4f92159a52c9) |  |
| [`status`](#group__symple_1gad93c255aca03896f8e91a360022521c8) |  |
| [`setType`](#group__symple_1gad1ff57aeeaa0788b12714750c856aa97) |  |
| [`setTo`](#group__symple_1ga59aff14b0a347efc0d7cd13790aee564) |  |
| [`setTo`](#group__symple_1ga35ebeab4af091f299ea7813dec40b611) |  |
| [`setTo`](#group__symple_1gaa95416e0a9ebdeae0311cfc2f5ec9a7a) |  |
| [`setFrom`](#group__symple_1ga883e4a76d045695cbeb1d95fd6b09a06) |  |
| [`setFrom`](#group__symple_1ga0121323c2d20f1ee7b6ec0b65949dd82) |  |
| [`setFrom`](#group__symple_1gae951b7a32bf75402fc59b2eacd29e262) |  |
| [`setStatus`](#group__symple_1gaaf9e802be207af079eb682439868720b) | HTTP status codes are used to describe the message response. **See also**: [http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html](http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html) |
| [`notes`](#group__symple_1ga423f8746648172da7e2fcaa7d5e61333) |  |
| [`setNote`](#group__symple_1ga2a15db81f2ee696f70977047dc5c1388) | Set a note for the application Possible "type" values: `info`, `warn`, `error` |
| [`addNote`](#group__symple_1gabf23c5c1f2a42649d2c85ca0502bf7d5) |  |
| [`data`](#group__symple_1gaee52f1f411f45da20cb48fea81b141f8) |  |
| [`data`](#group__symple_1ga579e0d948fe9cca7ece181b0e851c72f) |  |
| [`setData`](#group__symple_1ga737ffbb719a355701337880a4e27c754) |  |
| [`setData`](#group__symple_1ga46b69d25c45569ff2c15f5c1982226c0) |  |
| [`setData`](#group__symple_1ga27601dfabf9f11b474046c765f01d0f6) |  |
| [`setData`](#group__symple_1ga24c090297182428c0391e4850439facc) |  |
| [`setData`](#group__symple_1gab75585a9f4e4555c0a7da64926a3a5fd) |  |
| [`removeData`](#group__symple_1gaad4ce633c0a2f4be14c2dc34114aea14) |  |
| [`hasData`](#group__symple_1ga622a343fa5a0c26fe39d0a8822756d79) |  |
| [`read`](#group__symple_1ga41bc3235b35ef5ade9c5bc5e639701b9) | Read/parse to the packet from the given input buffer. The number of bytes read is returned. |
| [`read`](#group__symple_1gacb4307d7c2134e6720b380553e56fa74) |  |
| [`write`](#group__symple_1gac31e3c36ff63b87942058867a282fa29) | Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer. |
| [`isRequest`](#group__symple_1ga8893c0f725efc3aa1f42e0d8de208879) |  |
| [`size`](#group__symple_1gae7dcb4133dd8d5a3492629aabf85b385) | The size of the packet in bytes. |
| [`print`](#group__symple_1gaefb77bc8f9a03adc57f931d0b1d1e05d) |  |
| [`className`](#group__symple_1gaaae1eafc94ace22f9d961cc985ba9984) |  |

---

#### Message 

```cpp
Message()
```

---

#### Message 

```cpp
Message(const json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const json::value &` |  |

---

#### Message 

```cpp
Message(const Message & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const Message &` |  |

---

#### ~Message 

```cpp
virtual ~Message()
```

---

#### clone 

```cpp
virtual IPacket * clone() const
```

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### clear 

```cpp
virtual void clear()
```

---

#### clearData 

```cpp
virtual void clearData()
```

---

#### clearNotes 

```cpp
virtual void clearNotes()
```

---

#### type 

```cpp
std::string type() const
```

---

#### id 

```cpp
std::string id() const
```

---

#### to 

```cpp
Address to() const
```

---

#### from 

```cpp
Address from() const
```

---

#### status 

```cpp
int status() const
```

---

#### setType 

```cpp
void setType(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### setTo 

```cpp
void setTo(const Peer & to)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `to` | `const Peer &` |  |

---

#### setTo 

```cpp
void setTo(const Address & to)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `to` | `const Address &` |  |

---

#### setTo 

```cpp
void setTo(const std::string & to)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `to` | `const std::string &` |  |

---

#### setFrom 

```cpp
void setFrom(const Peer & from)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `from` | `const Peer &` |  |

---

#### setFrom 

```cpp
void setFrom(const Address & from)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `from` | `const Address &` |  |

---

#### setFrom 

```cpp
void setFrom(const std::string & from)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `from` | `const std::string &` |  |

---

#### setStatus 

```cpp
void setStatus(int code)
```

HTTP status codes are used to describe the message response. **See also**: [http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html](http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html)

| Parameter | Type | Description |
|-----------|------|-------------|
| `code` | `int` |  |

---

#### notes 

```cpp
json::value & notes()
```

---

#### setNote 

```cpp
void setNote(const std::string & type, const std::string & text)
```

Set a note for the application Possible "type" values: `info`, `warn`, `error`

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `text` | `const std::string &` |  |

---

#### addNote 

```cpp
void addNote(const std::string & type, const std::string & text)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |
| `text` | `const std::string &` |  |

---

#### data 

```cpp
json::value data(const std::string & name) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### data 

```cpp
json::value & data(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setData 

```cpp
json::value & setData(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setData 

```cpp
void setData(const std::string & name, const char * data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `data` | `const char *` |  |

---

#### setData 

```cpp
void setData(const std::string & name, const std::string & data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `data` | `const std::string &` |  |

---

#### setData 

```cpp
void setData(const std::string & name, const json::value & data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `data` | `const json::value &` |  |

---

#### setData 

```cpp
void setData(const std::string & name, int data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `data` | `int` |  |

---

#### removeData 

```cpp
void removeData(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### hasData 

```cpp
bool hasData(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### read 

```cpp
virtual ssize_t read(const ConstBuffer &)
```

Read/parse to the packet from the given input buffer. The number of bytes read is returned.

---

#### read 

```cpp
virtual ssize_t read(const std::string & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const std::string &` |  |

---

#### write 

```cpp
virtual void write(Buffer &) const
```

Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer.

Todo: It may be prefferable to use our pod types here instead of buffer input, but the current codebase requires that the buffer be dynamically resizable for some protocols...

---

#### isRequest 

```cpp
bool isRequest() const
```

---

#### size 

```cpp
virtual size_t size() const
```

The size of the packet in bytes.

This is the nember of bytes that will be written on a call to [write()](#group__symple_1gac31e3c36ff63b87942058867a282fa29), but may not be the number of bytes that will be consumed by [read()](#group__symple_1ga41bc3235b35ef5ade9c5bc5e639701b9).

---

#### print 

```cpp
virtual void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### className 

```cpp
virtual inline const char * className() const
```

## Peer 

> **Extends:** `json::value`
> **Defined in:** `peer.h`

This class represents a peer on the network. A [Peer](#classicy_1_1smpl_1_1Peer) object may also contain arbitrary data set by the client to share with other peers on the network. **See also**: [Address](#structicy_1_1smpl_1_1Address) for further methods and [basic](./doc/api-undefined.md#namespaceicy_1_1basic) accessors.

### Members

| Name | Description |
|------|-------------|
| [`Peer`](#group__symple_1ga21699ec274f5b4a4609fa1232c7c2759) |  |
| [`Peer`](#group__symple_1gaf1d1b51b210184552aa1e52ed9c1043f) |  |
| [`Peer`](#group__symple_1ga2be2d8f4cc8f1f0a5ce2d7c4187645a1) |  |
| [`~Peer`](#group__symple_1gab6b9fd40ad19b75f1476719938486687) |  |
| [`address`](#group__symple_1gae18b7a663a71100f65a480a309928274) |  |
| [`id`](#group__symple_1ga477d474d5af82b4d59a762efa970bb1a) |  |
| [`user`](#group__symple_1ga9731497468e46249d9640bb38637ebdd) |  |
| [`name`](#group__symple_1ga52cc114ca838238810e3d554425194b7) |  |
| [`type`](#group__symple_1gab1ff841dfeb0d69ac798ad12c55bf43d) |  |
| [`host`](#group__symple_1gae1ac4cb0de0b4845a2f73403b3caa83e) |  |
| [`setID`](#group__symple_1ga0901ffbe8ce396fb01d83553815bad54) |  |
| [`setUser`](#group__symple_1ga6da2b366cdda5ed7dfaa11b69d20ef37) |  |
| [`setName`](#group__symple_1gaae1a1e30ed070876d5dc7d124a99434e) |  |
| [`setType`](#group__symple_1ga6b4ce3657cc8995ad02e7c51939f1d53) |  |
| [`setHost`](#group__symple_1ga1bab6aa0690fff8ca6cbf8749fc0efd8) |  |
| [`valid`](#group__symple_1gad056870e5ed4649ca9d218c9cc6c8df3) | Updates the peer from the given data object. |
| [`print`](#group__symple_1gaba929db58e4c8245ed8e0c61d7f6bc57) |  |
| [`className`](#group__symple_1ga9a99cfc94cb96aef6c338e514a254809) |  |

---

#### Peer 

```cpp
Peer()
```

---

#### Peer 

```cpp
Peer(const Peer & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Peer &` |  |

---

#### Peer 

```cpp
Peer(const json::value & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const json::value &` |  |

---

#### ~Peer 

```cpp
virtual ~Peer()
```

---

#### address 

```cpp
Address address() const
```

---

#### id 

```cpp
std::string id() const
```

---

#### user 

```cpp
std::string user() const
```

---

#### name 

```cpp
std::string name() const
```

---

#### type 

```cpp
std::string type() const
```

---

#### host 

```cpp
std::string host() const
```

---

#### setID 

```cpp
void setID(const std::string & id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### setUser 

```cpp
void setUser(const std::string & user)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `user` | `const std::string &` |  |

---

#### setName 

```cpp
void setName(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setType 

```cpp
void setType(const std::string & type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `const std::string &` |  |

---

#### setHost 

```cpp
void setHost(const std::string & host)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |

---

#### valid 

```cpp
virtual bool valid()
```

Updates the peer from the given data object.

---

#### print 

```cpp
void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### className 

```cpp
virtual inline const char * className() const
```

## Presence 

> **Extends:** `icy::smpl::Message`
> **Defined in:** `presence.h`

### Members

| Name | Description |
|------|-------------|
| [`Presence`](#group__symple_1ga8e1bf8928c147286c6992a68ba000b22) |  |
| [`Presence`](#group__symple_1ga5b4433cbeca31ebc1d47be79a5e35e9f) |  |
| [`Presence`](#group__symple_1gaa1b5a011069c478b0cc808efa0b8e3d2) |  |
| [`~Presence`](#group__symple_1gae3237592f9d4e644c94eb9652b3ddd82) |  |
| [`isProbe`](#group__symple_1ga2a03e7f09b7455a88ebc06bea78da377) |  |
| [`setProbe`](#group__symple_1ga991b723ca51fbcc7d4b0a6e5ee9577ae) |  |

---

#### Presence 

```cpp
Presence()
```

---

#### Presence 

```cpp
Presence(const json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const json::value &` |  |

---

#### Presence 

```cpp
Presence(const Presence & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const Presence &` |  |

---

#### ~Presence 

```cpp
virtual ~Presence()
```

---

#### isProbe 

```cpp
bool isProbe()
```

---

#### setProbe 

```cpp
void setProbe(bool flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

## Roster 

> **Extends:** `icy::LiveCollection< std::string, Peer >`
> **Defined in:** `roster.h`

The [Roster](#classicy_1_1smpl_1_1Roster) provides a registry for active network peers indexed by session ID.

### Members

| Name | Description |
|------|-------------|
| [`Roster`](#group__symple_1ga52cf6d7c73b07459e7231898cf3267c0) |  |
| [`~Roster`](#group__symple_1ga548c497780612733999913ad9be891a6) |  |
| [`getByHost`](#group__symple_1ga2762fe4481b419996e1f6f8ca51a222d) | Returns the first peer which matches the given host address. |
| [`peers`](#group__symple_1ga82d3344e72e6d3cea1acee9929613076) | Returns a copy of the peer map for thread-safe iteration. |
| [`print`](#group__symple_1ga1938a8f8c5784b7cafd64225133a1bae) |  |
| [`className`](#group__symple_1ga626f160e6db0deb2de31b4d6f85a1689) |  |

---

#### Roster 

```cpp
Roster()
```

---

#### ~Roster 

```cpp
virtual ~Roster()
```

---

#### getByHost 

```cpp
Peer * getByHost(const std::string & host)
```

Returns the first peer which matches the given host address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |

---

#### peers 

```cpp
virtual PeerMap peers() const
```

Returns a copy of the peer map for thread-safe iteration.

---

#### print 

```cpp
virtual void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### className 

```cpp
virtual inline const char * className() const
```

## Address 

> **Defined in:** `address.h`

The [Address](#structicy_1_1smpl_1_1Address) structure is an endpoint identifier for a peer on the network. The format is like so: user|id

### Members

| Name | Description |
|------|-------------|
| [`Address`](#group__symple_1gac343658359ea35750fc8db0954b4859b) |  |
| [`Address`](#group__symple_1ga7767484bbc9b2b9eef8ed405a1d6df4e) |  |
| [`Address`](#group__symple_1ga124f7f256546189d7bb40403f15d1aee) |  |
| [`~Address`](#group__symple_1gaec7089a1558336d72614e3ac3c6fe31b) |  |
| [`parse`](#group__symple_1gadce752de542bbfc15cbbc6bd3399a609) |  |
| [`valid`](#group__symple_1gad9683597832591065ff7527d9d0d1e7b) |  |
| [`print`](#group__symple_1gae575d3b61d86073b6644fb77525e12b8) |  |
| [`toString`](#group__symple_1ga3e7d857b2da0971cc034191a13ce4936) |  |
| [`operator==`](#group__symple_1ga8404d6640ae900b3a41bd5bc9eb1ec0e) |  |
| [`operator==`](#group__symple_1gad2e78b790fca7e0f7a234cdb3c39bd3c) |  |
| [`user`](#group__symple_1gab04caf4352786515fca8fce459a56931) |  |
| [`id`](#group__symple_1gae830f59b90e94397743b6f9540a7afe0) |  |

---

#### Address 

```cpp
Address()
```

---

#### Address 

```cpp
Address(const std::string & addr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const std::string &` |  |

---

#### Address 

```cpp
Address(const std::string & user, const std::string & id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `user` | `const std::string &` |  |
| `id` | `const std::string &` |  |

---

#### ~Address 

```cpp
virtual ~Address()
```

---

#### parse 

```cpp
bool parse(const std::string & addr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const std::string &` |  |

---

#### valid 

```cpp
bool valid() const
```

---

#### print 

```cpp
void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### toString 

```cpp
std::string toString() const
```

---

#### operator== 

```cpp
bool operator==(const Address & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Address &` |  |

---

#### operator== 

```cpp
bool operator==(std::string & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `std::string &` |  |

---

#### user 

```cpp
std::string user
```

---

#### id 

```cpp
std::string id
```

## Options 

> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__symple_1ga7dd4fee81e4318912e9d82a1e35c6550) |  |
| [`host`](#group__symple_1ga05d0bdf5ab5eeefec7d0019bb110bb58) |  |
| [`port`](#group__symple_1ga1b77c19ba3010b95adab9572de8812d4) |  |
| [`reconnection`](#group__symple_1ga8388c0e51b717793da8e136f24ef4df6) | Weather or not to reconnect if disconnected from the server. |
| [`reconnectAttempts`](#group__symple_1gaad73b34182da94bd80d394d6c2cf8d59) | The number of times to attempt to reconnect if disconnected from the server. (0 = unlimited) |
| [`user`](#group__symple_1gace5d72442ca0ddd400242766dcbe0075) |  |
| [`name`](#group__symple_1ga937a706d0ff802b29881200dd0f9b140) |  |
| [`type`](#group__symple_1ga3a94f72a2f8e72b1bb84f11630f9cd9a) |  |
| [`token`](#group__symple_1gae46b1ce6b2a9c5e8ae983cf36137d18d) |  |

---

#### Options 

```cpp
inline Options()
```

---

#### host 

```cpp
std::string host = "127.0.0.1"
```

---

#### port 

```cpp
uint16_t port = 4500
```

---

#### reconnection 

```cpp
bool reconnection = true
```

Weather or not to reconnect if disconnected from the server.

---

#### reconnectAttempts 

```cpp
int reconnectAttempts = 0
```

The number of times to attempt to reconnect if disconnected from the server. (0 = unlimited)

---

#### user 

```cpp
std::string user
```

---

#### name 

```cpp
std::string name
```

---

#### type 

```cpp
std::string type
```

---

#### token 

```cpp
std::string token
```

## Filter 

> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Filter`](#group__symple_1ga7e21efcc6d5fc951be79a19e246294dd) |  |
| [`Filter`](#group__symple_1ga1f3cd2015d9c9dd0f710ec47c2d68e03) |  |
| [`flags`](#group__symple_1ga84b2f5acb1cf965cae191acd953dfad4) |  |
| [`path`](#group__symple_1ga54304a93d73847a0a53f8f1da50a62a1) |  |

---

#### Filter 

```cpp
inline Filter(const std::string & path, unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `flags` | `unsigned` |  |

---

#### Filter 

```cpp
inline Filter(unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flags` | `unsigned` |  |

---

#### flags 

```cpp
Bitwise flags
```

---

#### path 

```cpp
std::string path
```

