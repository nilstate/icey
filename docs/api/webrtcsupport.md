{#webrtcsupporthelpers}

# webrtcsupport

Shared signalling helpers and sample/app adapters layered on top of the core WebRTC module.

### Classes

| Name | Description |
|------|-------------|
| [`WebSocketSignaller`](#websocketsignaller) | [SignallingInterface](webrtc.md#signallinginterface) implementation using plain JSON over WebSocket. |
| [`SympleSignaller`](#symplesignaller) | [SignallingInterface](webrtc.md#signallinginterface) implementation using the Symple call protocol. |
| [`SympleServerSignaller`](#sympleserversignaller) | Server-side Symple signaller for virtual peers and embedded apps. |

{#websocketsignaller}

## WebSocketSignaller

```cpp
#include <icy/webrtc/support/wssignaller.h>
```

> **Inherits:** [`SignallingInterface`](webrtc.md#signallinginterface)

[SignallingInterface](webrtc.md#signallinginterface) implementation using plain JSON over WebSocket.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(const std::string &)>` | [`SendMessage`](#sendmessage)  |  |

---

{#sendmessage}

#### SendMessage

```cpp
Signal< void(const std::string &)> SendMessage
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WebSocketSignaller`](#websocketsignaller-1) `explicit` |  |
|  | [`WebSocketSignaller`](#websocketsignaller-2)  | Deleted constructor. |
| `void` | [`sendSdp`](#sendsdp-1) `virtual` | Send an SDP offer or answer to the remote peer. |
| `void` | [`sendCandidate`](#sendcandidate-1) `virtual` | Send an ICE candidate to the remote peer. |
| `void` | [`sendControl`](#sendcontrol-1) `virtual` | Send a control message to the remote peer. |
| `void` | [`receive`](#receive)  |  |
| `void` | [`setLocalPeerId`](#setlocalpeerid) `inline` |  |
| `const std::string &` | [`localPeerId`](#localpeerid) `const` `inline` |  |

---

{#websocketsignaller-1}

#### WebSocketSignaller

`explicit`

```cpp
explicit WebSocketSignaller(const std::string & localPeerId)
```

---

{#websocketsignaller-2}

#### WebSocketSignaller

```cpp
WebSocketSignaller(const WebSocketSignaller &) = delete
```

Deleted constructor.

---

{#sendsdp-1}

#### sendSdp

`virtual`

```cpp
virtual void sendSdp(const std::string & peerId, const std::string & type, const std::string & sdp)
```

Send an SDP offer or answer to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` "offer" or "answer". 

* `sdp` The SDP string.

---

{#sendcandidate-1}

#### sendCandidate

`virtual`

```cpp
virtual void sendCandidate(const std::string & peerId, const std::string & candidate, const std::string & mid)
```

Send an ICE candidate to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `candidate` The candidate string (from RTCIceCandidate). 

* `mid` The sdpMid value.

---

{#sendcontrol-1}

#### sendControl

`virtual`

```cpp
virtual void sendControl(const std::string & peerId, const std::string & type, const std::string & reason)
```

Send a control message to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` Control type: "init", "accept", "reject", "hangup". 

* `reason` Optional reason string (for reject/hangup).

---

{#receive}

#### receive

```cpp
void receive(const std::string & json)
```

---

{#setlocalpeerid}

#### setLocalPeerId

`inline`

```cpp
inline void setLocalPeerId(const std::string & id)
```

---

{#localpeerid}

#### localPeerId

`const` `inline`

```cpp
inline const std::string & localPeerId() const
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_localPeerId`](#_localpeerid)  |  |
| `std::mutex` | [`_mutex`](#_mutex-21)  |  |

---

{#_localpeerid}

#### _localPeerId

```cpp
std::string _localPeerId
```

---

{#_mutex-21}

#### _mutex

```cpp
std::mutex _mutex
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`sendJson`](#sendjson-1)  |  |

---

{#sendjson-1}

#### sendJson

```cpp
void sendJson(const json::Value & msg)
```

{#symplesignaller}

## SympleSignaller

```cpp
#include <icy/webrtc/support/symplesignaller.h>
```

> **Inherits:** [`SignallingInterface`](webrtc.md#signallinginterface)

[SignallingInterface](webrtc.md#signallinginterface) implementation using the Symple call protocol.

Speaks the `call:init`, `call:accept`, `call:reject`, `call:offer`, `call:answer`, `call:candidate`, and `call:hangup` envelope over a connected Symple client.

The remote peer identifier on this signalling boundary is the full Symple address string (`user|id`).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`SympleSignaller`](#symplesignaller-1) `explicit` |  |
|  | [`SympleSignaller`](#symplesignaller-2)  | Deleted constructor. |
| `void` | [`sendSdp`](#sendsdp-2) `virtual` | Send an SDP offer or answer to the remote peer. |
| `void` | [`sendCandidate`](#sendcandidate-2) `virtual` | Send an ICE candidate to the remote peer. |
| `void` | [`sendControl`](#sendcontrol-2) `virtual` | Send a control message to the remote peer. |

---

{#symplesignaller-1}

#### SympleSignaller

`explicit`

```cpp
explicit SympleSignaller(smpl::Client & client)
```

---

{#symplesignaller-2}

#### SympleSignaller

```cpp
SympleSignaller(const SympleSignaller &) = delete
```

Deleted constructor.

---

{#sendsdp-2}

#### sendSdp

`virtual`

```cpp
virtual void sendSdp(const std::string & peerId, const std::string & type, const std::string & sdp)
```

Send an SDP offer or answer to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` "offer" or "answer". 

* `sdp` The SDP string.

---

{#sendcandidate-2}

#### sendCandidate

`virtual`

```cpp
virtual void sendCandidate(const std::string & peerId, const std::string & candidate, const std::string & mid)
```

Send an ICE candidate to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `candidate` The candidate string (from RTCIceCandidate). 

* `mid` The sdpMid value.

---

{#sendcontrol-2}

#### sendControl

`virtual`

```cpp
virtual void sendControl(const std::string & peerId, const std::string & type, const std::string & reason)
```

Send a control message to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` Control type: "init", "accept", "reject", "hangup". 

* `reason` Optional reason string (for reject/hangup).

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `smpl::Client &` | [`_client`](#_client)  |  |

---

{#_client}

#### _client

```cpp
smpl::Client & _client
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onMessage`](#onmessage-1)  |  |
| `void` | [`send`](#send-20)  |  |

---

{#onmessage-1}

#### onMessage

```cpp
void onMessage(smpl::Message & msg)
```

---

{#send-20}

#### send

```cpp
void send(const std::string & action, const std::string & to, const json::Value & data)
```

{#sympleserversignaller}

## SympleServerSignaller

```cpp
#include <icy/webrtc/support/sympleserversignaller.h>
```

> **Inherits:** [`SignallingInterface`](webrtc.md#signallinginterface)

Server-side Symple signaller for virtual peers and embedded apps.

This adapter sends signalling messages through a `[smpl::Server](symple.md#server-9)` on behalf of a local virtual peer address, and accepts inbound Symple call messages through `[onMessage()](#onmessage-2)`.

The remote peer identifier on this signalling boundary is the full Symple address string (`user|id`).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`SympleServerSignaller`](#sympleserversignaller-1)  |  |
|  | [`SympleServerSignaller`](#sympleserversignaller-2)  | Deleted constructor. |
| `void` | [`sendSdp`](#sendsdp-3) `virtual` | Send an SDP offer or answer to the remote peer. |
| `void` | [`sendCandidate`](#sendcandidate-3) `virtual` | Send an ICE candidate to the remote peer. |
| `void` | [`sendControl`](#sendcontrol-3) `virtual` | Send a control message to the remote peer. |
| `void` | [`onMessage`](#onmessage-2)  |  |
| `void` | [`setRemoteAddress`](#setremoteaddress)  |  |
| `const std::string &` | [`localAddress`](#localaddress-1) `const` `inline` |  |
| `const std::string &` | [`remoteAddress`](#remoteaddress-1) `const` `inline` |  |

---

{#sympleserversignaller-1}

#### SympleServerSignaller

```cpp
SympleServerSignaller(smpl::Server & server, std::string localAddress, std::string remoteAddress)
```

---

{#sympleserversignaller-2}

#### SympleServerSignaller

```cpp
SympleServerSignaller(const SympleServerSignaller &) = delete
```

Deleted constructor.

---

{#sendsdp-3}

#### sendSdp

`virtual`

```cpp
virtual void sendSdp(const std::string & peerId, const std::string & type, const std::string & sdp)
```

Send an SDP offer or answer to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` "offer" or "answer". 

* `sdp` The SDP string.

---

{#sendcandidate-3}

#### sendCandidate

`virtual`

```cpp
virtual void sendCandidate(const std::string & peerId, const std::string & candidate, const std::string & mid)
```

Send an ICE candidate to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `candidate` The candidate string (from RTCIceCandidate). 

* `mid` The sdpMid value.

---

{#sendcontrol-3}

#### sendControl

`virtual`

```cpp
virtual void sendControl(const std::string & peerId, const std::string & type, const std::string & reason)
```

Send a control message to the remote peer. 
#### Parameters
* `peerId` Remote peer identifier. 

* `type` Control type: "init", "accept", "reject", "hangup". 

* `reason` Optional reason string (for reject/hangup).

---

{#onmessage-2}

#### onMessage

```cpp
void onMessage(const json::Value & msg)
```

---

{#setremoteaddress}

#### setRemoteAddress

```cpp
void setRemoteAddress(std::string remoteAddress)
```

---

{#localaddress-1}

#### localAddress

`const` `inline`

```cpp
inline const std::string & localAddress() const
```

---

{#remoteaddress-1}

#### remoteAddress

`const` `inline`

```cpp
inline const std::string & remoteAddress() const
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `smpl::Server &` | [`_server`](#_server-2)  |  |
| `std::string` | [`_localAddress`](#_localaddress)  |  |
| `std::string` | [`_remoteAddress`](#_remoteaddress)  |  |
| `Synchronizer` | [`_dispatch`](#_dispatch-1)  |  |
| `std::mutex` | [`_mutex`](#_mutex-22)  |  |
| `std::deque< OutboundMessage >` | [`_pending`](#_pending-1)  |  |
| `bool` | [`_closing`](#_closing)  |  |

---

{#_server-2}

#### _server

```cpp
smpl::Server & _server
```

---

{#_localaddress}

#### _localAddress

```cpp
std::string _localAddress
```

---

{#_remoteaddress}

#### _remoteAddress

```cpp
std::string _remoteAddress
```

---

{#_dispatch-1}

#### _dispatch

```cpp
Synchronizer _dispatch
```

---

{#_mutex-22}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_pending-1}

#### _pending

```cpp
std::deque< OutboundMessage > _pending
```

---

{#_closing}

#### _closing

```cpp
bool _closing = false
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`send`](#send-21)  |  |
| `bool` | [`sendNow`](#sendnow)  |  |
| `void` | [`flushPending`](#flushpending-1)  |  |

---

{#send-21}

#### send

```cpp
void send(const std::string & action, const std::string & to, const json::Value & data)
```

---

{#sendnow}

#### sendNow

```cpp
bool sendNow(const std::string & to, const json::Value & msg)
```

---

{#flushpending-1}

#### flushPending

```cpp
void flushPending()
```

{#outboundmessage}

## OutboundMessage

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`peerAddress`](#peeraddress-6)  |  |
| `json::Value` | [`msg`](#msg)  |  |

---

{#peeraddress-6}

#### peerAddress

```cpp
std::string peerAddress
```

---

{#msg}

#### msg

```cpp
json::Value msg
```

