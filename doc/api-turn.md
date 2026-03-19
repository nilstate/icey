# turn 

The `turn` module contains TURN (rfc5766) client and server implementations.

### Classes

| Name | Description |
|------|-------------|
| [`Client`](#classicy_1_1turn_1_1Client) |  |
| [`TCPClient`](#classicy_1_1turn_1_1TCPClient) |  |
| [`UDPClient`](#classicy_1_1turn_1_1UDPClient) |  |
| [`FiveTuple`](#classicy_1_1turn_1_1FiveTuple) | The 5-TUPLE consists of a local, a remote address, and the transport protocol used by the client to communicate with the server.                                                           +---------+ |
| [`IAllocation`](#classicy_1_1turn_1_1IAllocation) | All TURN operations revolve around allocations, and all TURN messages are associated with an allocation. An allocation conceptually consists of the following state data: |
| [`Server`](#classicy_1_1turn_1_1Server) | TURN server rfc5766 implementation. |
| [`ServerAllocation`](#classicy_1_1turn_1_1ServerAllocation) |  |
| [`TCPAllocation`](#classicy_1_1turn_1_1TCPAllocation) |  |
| [`TCPConnectionPair`](#classicy_1_1turn_1_1TCPConnectionPair) |  |
| [`UDPAllocation`](#classicy_1_1turn_1_1UDPAllocation) |  |
| [`Request`](#classicy_1_1turn_1_1Request) |  |
| [`ClientState`](#structicy_1_1turn_1_1ClientState) |  |
| [`ClientObserver`](#structicy_1_1turn_1_1ClientObserver) |  |
| [`Options`](#structicy_1_1turn_1_1Client_1_1Options) |  |
| [`TCPClientObserver`](#structicy_1_1turn_1_1TCPClientObserver) |  |
| [`RelayConnectionBinding`](#structicy_1_1turn_1_1RelayConnectionBinding) |  |
| [`Permission`](#structicy_1_1turn_1_1Permission) | TURN permission for a user session. |
| [`ServerOptions`](#structicy_1_1turn_1_1ServerOptions) | [Configuration](./doc/api-base.md#classicy_1_1Configuration) options for the TURN server. |
| [`ServerObserver`](#structicy_1_1turn_1_1ServerObserver) | The [ServerObserver](#structicy_1_1turn_1_1ServerObserver) receives callbacks for and is responsible for managing allocation and bandwidth quotas, authentication methods and authentication. |

## Client 

> **Extends:** `icy::Stateful< ClientState >`, `icy::turn::IAllocation`
> **Subclasses:** `icy::turn::TCPClient`, `icy::turn::UDPClient`
> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Client`](#group__turn_1ga65ce6f16a338792a1783d51edffd71dd) |  |
| [`~Client`](#group__turn_1ga7fc90ccb1863fddf1a63fae16ae9c371) |  |
| [`initiate`](#group__turn_1gadf723a357d7bb4ad4162850e1689d3e3) | Initiates the allocation sequence. |
| [`shutdown`](#group__turn_1ga1e0682ce8d32f7c62946ee7793c1181c) | Shutdown the client and destroy the active allocation. |
| [`sendAllocate`](#group__turn_1ga7d568f600867e83b78f42f61d24e13ef) | Sends the allocation request. |
| [`addPermission`](#group__turn_1ga63590b39064f8a8f96adb89eee134d63) |  |
| [`addPermission`](#group__turn_1gaba5057ecad2da5a048e67a51efc74d0f) | Peer permissions should be added/created before we kick off the allocation sequence, but may be added later. |
| [`sendCreatePermission`](#group__turn_1ga8555bd85d39c6430f2f00f0f684d7628) | Sends a CreatePermission request including all hosts. |
| [`sendChannelBind`](#group__turn_1ga8901df806b203e670fe925fe13dca81e) |  |
| [`sendRefresh`](#group__turn_1gacd3057049693dd485726f0a14b1e5ee0) |  |
| [`sendData`](#group__turn_1ga20f4a32be7ab6b6ee5a787eb3c9320bc) |  |
| [`handleResponse`](#group__turn_1ga0f6ebc4b5830aaa97b69ca66f0999261) |  |
| [`handleAllocateResponse`](#group__turn_1ga68bbf8a80eedb0ba14918aec893448a5) |  |
| [`handleAllocateErrorResponse`](#group__turn_1ga12709199bdbf16d3c7dc0cd82e3a93ab) |  |
| [`handleCreatePermissionResponse`](#group__turn_1gaa22352721ef5518a253a6594877a21eb) |  |
| [`handleCreatePermissionErrorResponse`](#group__turn_1gabbd2963a3eb1021634c55abb70d39d63) |  |
| [`handleRefreshResponse`](#group__turn_1ga144c651f56df26ffc92879219930e848) |  |
| [`handleDataIndication`](#group__turn_1gaadd3916417580152b965c64c32cf73bb) |  |
| [`transportProtocol`](#group__turn_1ga32c28b5280210a450c0e008904a7b6b2) |  |
| [`createTransaction`](#group__turn_1ga88dd79f35429039f830cd1fbfd7f3870) |  |
| [`authenticateRequest`](#group__turn_1ga7a7dac39cdf0e23586d6607ac65b06cb) |  |
| [`sendAuthenticatedTransaction`](#group__turn_1ga44237aae8a9ad5ad3cc4cecc19881e53) |  |
| [`removeTransaction`](#group__turn_1ga9f51f7e1bcbe90260db6c8e862927482) |  |
| [`mappedAddress`](#group__turn_1ga06d529cceb90e17ad9cdac3998cad80e) |  |
| [`relayedAddress`](#group__turn_1ga5708eadcbe76c7c9bc3904d5942e07f5) |  |
| [`closed`](#group__turn_1gafd23179bb00b8ce9dca4aaa666ba8f97) |  |
| [`observer`](#group__turn_1gaba66ee1063e547550dcbb32432b42ec8) |  |
| [`options`](#group__turn_1ga36514efc351b6cca0222702e137b27c9) |  |
| [`setError`](#group__turn_1ga21670cb1ae05473fe43a5e82cb40b754) |  |
| [`onSocketConnect`](#group__turn_1ga926f40e036ce12b00f365ae387b2102b) |  |
| [`onSocketRecv`](#group__turn_1gaf226ce27add87f373131003383569b20) |  |
| [`onSocketClose`](#group__turn_1gacfb499500c58c0392df68342e5aa59e7) |  |
| [`onTransactionProgress`](#group__turn_1ga461a2a0c78dbde5294f0260cadf5410c) |  |
| [`onStateChange`](#group__turn_1ga35a6d685c1ddb38d29e1467278b7bedc) |  |
| [`onTimer`](#group__turn_1ga7fb62ec5f7d6748b7ba6549b86709344) |  |
| [`_observer`](#group__turn_1gab2775eacd7fff57764950d54e7383306) |  |
| [`_options`](#group__turn_1ga7baa7e34fb78a93ed627c6a3b36740ba) |  |
| [`_socket`](#group__turn_1ga53562b6579f7832ad236ce922021852e) |  |
| [`_timer`](#group__turn_1ga1704ecb1572601a2f2863ce70cc4fb97) |  |
| [`_error`](#group__turn_1ga66ea7fae1ca4bed8445763221eb54020) |  |
| [`_mappedAddress`](#group__turn_1ga21a746dd56680daa4e14d19630ba4ecf) |  |
| [`_relayedAddress`](#group__turn_1ga1f3592858503ccb9efc59cad55ce3041) |  |
| [`_realm`](#group__turn_1ga819fcd60d15c7c555792ad91e34df15f) |  |
| [`_nonce`](#group__turn_1ga59aa3850af49ff0ea281fd6cb2168e14) |  |
| [`_pendingIndications`](#group__turn_1ga269602d5f2c548df6a654ad9e7c17144) | A list of queued Send indication packets awaiting server permissions. |
| [`_transactions`](#group__turn_1ga9d80bebccf3f7cdf8d34d1246bf6fb02) | A list containing currently active transactions. |

---

#### Client 

```cpp
Client(ClientObserver & observer, const Options & options, const net::Socket::Ptr & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `observer` | `ClientObserver &` |  |
| `options` | `const Options &` |  |
| `socket` | `const net::Socket::Ptr &` |  |

---

#### ~Client 

```cpp
virtual ~Client()
```

---

#### initiate 

```cpp
virtual void initiate()
```

Initiates the allocation sequence.

---

#### shutdown 

```cpp
virtual void shutdown()
```

Shutdown the client and destroy the active allocation.

---

#### sendAllocate 

```cpp
virtual void sendAllocate()
```

Sends the allocation request.

---

#### addPermission 

```cpp
virtual void addPermission(const IPList & peerIPs)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `peerIPs` | `const IPList &` |  |

---

#### addPermission 

```cpp
virtual void addPermission(const std::string & ip)
```

Peer permissions should be added/created before we kick off the allocation sequence, but may be added later.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ip` | `const std::string &` |  |

---

#### sendCreatePermission 

```cpp
virtual void sendCreatePermission()
```

Sends a CreatePermission request including all hosts.

A CreatePermission request will be sent as soon as the Allocation is created, and at timer x intervals.

---

#### sendChannelBind 

```cpp
virtual void sendChannelBind(const std::string & peerIP)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `peerIP` | `const std::string &` |  |

---

#### sendRefresh 

```cpp
virtual void sendRefresh()
```

---

#### sendData 

```cpp
virtual void sendData(const char * data, size_t size, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `size` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### handleResponse 

```cpp
virtual bool handleResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleAllocateResponse 

```cpp
virtual void handleAllocateResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleAllocateErrorResponse 

```cpp
virtual void handleAllocateErrorResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleCreatePermissionResponse 

```cpp
virtual void handleCreatePermissionResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleCreatePermissionErrorResponse 

```cpp
virtual void handleCreatePermissionErrorResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleRefreshResponse 

```cpp
virtual void handleRefreshResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleDataIndication 

```cpp
virtual void handleDataIndication(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### transportProtocol 

```cpp
virtual int transportProtocol()
```

---

#### createTransaction 

```cpp
virtual stun::Transaction * createTransaction(const net::Socket::Ptr & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::Socket::Ptr &` |  |

---

#### authenticateRequest 

```cpp
virtual void authenticateRequest(stun::Message & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `stun::Message &` |  |

---

#### sendAuthenticatedTransaction 

```cpp
virtual bool sendAuthenticatedTransaction(stun::Transaction * transaction)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `transaction` | `stun::Transaction *` |  |

---

#### removeTransaction 

```cpp
virtual bool removeTransaction(stun::Transaction * transaction)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `transaction` | `stun::Transaction *` |  |

---

#### mappedAddress 

```cpp
net::Address mappedAddress() const
```

---

#### relayedAddress 

```cpp
virtual net::Address relayedAddress() const
```

---

#### closed 

```cpp
bool closed() const
```

---

#### observer 

```cpp
ClientObserver & observer()
```

---

#### options 

```cpp
Options & options()
```

---

#### setError 

```cpp
virtual void setError(const [icy::Error](./doc/api-base.md#structicy_1_1Error) & error)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const [icy::Error](./doc/api-base.md#structicy_1_1Error) &` |  |

---

#### onSocketConnect 

```cpp
virtual void onSocketConnect(net::Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onSocketClose 

```cpp
virtual void onSocketClose(net::Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### onTransactionProgress 

```cpp
virtual void onTransactionProgress(void * sender, TransactionState & state, const TransactionState &)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `state` | `TransactionState &` |  |

---

#### onStateChange 

```cpp
virtual void onStateChange(ClientState & state, const ClientState & oldState)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `ClientState &` |  |
| `oldState` | `const ClientState &` |  |

---

#### onTimer 

```cpp
virtual void onTimer()
```

---

#### _observer 

```cpp
ClientObserver & _observer
```

---

#### _options 

```cpp
Options _options
```

---

#### _socket 

```cpp
net::SocketEmitter _socket
```

---

#### _timer 

```cpp
Timer _timer
```

---

#### _error 

```cpp
[icy::Error](./doc/api-base.md#structicy_1_1Error) _error
```

---

#### _mappedAddress 

```cpp
net::Address _mappedAddress
```

---

#### _relayedAddress 

```cpp
net::Address _relayedAddress
```

---

#### _realm 

```cpp
std::string _realm
```

---

#### _nonce 

```cpp
std::string _nonce
```

---

#### _pendingIndications 

```cpp
std::deque< stun::Message > _pendingIndications
```

A list of queued Send indication packets awaiting server permissions.

---

#### _transactions 

```cpp
std::vector< stun::Transaction * > _transactions
```

A list containing currently active transactions.

## Options 

> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__turn_1gae926546e19d8fd9a7fab86271b96a54f) |  |
| [`software`](#group__turn_1ga5353220869e0f09239060488ecdb7fef) |  |
| [`username`](#group__turn_1gac72217fb6b73d1d050c84379189354a5) |  |
| [`password`](#group__turn_1ga90e75f93e08bd392b9def185c4f29ec1) |  |
| [`timeout`](#group__turn_1ga3b36f9f68df1b465c1046b88529ccf69) |  |
| [`lifetime`](#group__turn_1ga7dc6b1fc053f08ec22d0f2f97084d996) |  |
| [`timerInterval`](#group__turn_1ga50b1abb4fea151715d0c93b5ac675f30) |  |
| [`serverAddr`](#group__turn_1gaca69ae3e4cb351a623def3f8ca67a950) |  |

---

#### Options 

```cpp
inline Options()
```

---

#### software 

```cpp
std::string software
```

---

#### username 

```cpp
std::string username
```

---

#### password 

```cpp
std::string password
```

---

#### timeout 

```cpp
long timeout
```

---

#### lifetime 

```cpp
std::int64_t lifetime
```

---

#### timerInterval 

```cpp
std::int64_t timerInterval
```

---

#### serverAddr 

```cpp
net::Address serverAddr
```

## TCPClient 

> **Extends:** `icy::turn::Client`
> **Defined in:** `tcpclient.h`

### Members

| Name | Description |
|------|-------------|
| [`TCPClient`](#group__turn_1ga66a06f08c0fec14c755537b176dbe6fd) |  |
| [`~TCPClient`](#group__turn_1gaa8bf16affc9175efa5ec96b7638cdd75) |  |
| [`initiate`](#group__turn_1ga21a24b1e8e50a72306595dde1e62b8c8) | Initiates the allocation sequence. |
| [`shutdown`](#group__turn_1ga10311576b628961147df231abeea2e45) | Shutdown the client and destroy the active allocation. |
| [`sendConnectRequest`](#group__turn_1gabc681709fb413701323ae55d99ef86c2) |  |
| [`sendData`](#group__turn_1gabd45ceb62c2d3cf0ccadc05d7ebe6867) |  |
| [`handleResponse`](#group__turn_1ga2d300174a7c09873df9bc36af3a66fe7) |  |
| [`handleConnectResponse`](#group__turn_1gaab0f1e53575195633d0e150b7532bbb9) |  |
| [`handleConnectErrorResponse`](#group__turn_1gaf5be5aa0c7f2a0a346dcdb5b1af90cb0) |  |
| [`handleConnectionBindResponse`](#group__turn_1ga577618f2ce93bc9e8089f84037a48381) |  |
| [`handleConnectionBindErrorResponse`](#group__turn_1gab8e9eb2b935c3af5ad0823c7f6e3d693) |  |
| [`handleConnectionAttemptIndication`](#group__turn_1ga5cee29ca9f9c7ffc3d6580f5ca504f2f) |  |
| [`createAndBindConnection`](#group__turn_1ga3f7d5efb351d7f2b107d3a25d410ce89) |  |
| [`onRelayConnectionConnect`](#group__turn_1ga9434e3aa4723ad58c16e197d12824031) |  |
| [`onRelayDataReceived`](#group__turn_1ga3c4890ab6d10d4f3766a70ea4c6c12aa) |  |
| [`onRelayConnectionError`](#group__turn_1ga12267279e5f527ecbe401f5bc3c96066) |  |
| [`onRelayConnectionClosed`](#group__turn_1gad643fdeeeb3355f0df9bd946029a5791) |  |
| [`freeConnection`](#group__turn_1ga1b8eac8a9103d924fcac1b6284c0c584) |  |
| [`transportProtocol`](#group__turn_1ga1fa35721f9353c21bf1e7b470c836883) |  |
| [`connections`](#group__turn_1ga8080d44f1441c6315d3cfcbae375b979) |  |
| [`className`](#group__turn_1ga57bf3238ed15d6472a35559674673eda) |  |
| [`_observer`](#group__turn_1ga07f4b77e0079d379c598ed2dd7fccb1c) |  |
| [`_connections`](#group__turn_1ga35cdf1b381e1516ae904410722e6bbb1) |  |

---

#### TCPClient 

```cpp
TCPClient(TCPClientObserver & observer, const Client::Options & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `observer` | `TCPClientObserver &` |  |
| `options` | `const Client::Options &` |  |

---

#### ~TCPClient 

```cpp
virtual ~TCPClient()
```

---

#### initiate 

```cpp
virtual void initiate()
```

Initiates the allocation sequence.

---

#### shutdown 

```cpp
virtual void shutdown()
```

Shutdown the client and destroy the active allocation.

---

#### sendConnectRequest 

```cpp
virtual void sendConnectRequest(const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `peerAddress` | `const net::Address &` |  |

---

#### sendData 

```cpp
virtual void sendData(const char * data, size_t size, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `size` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### handleResponse 

```cpp
virtual bool handleResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleConnectResponse 

```cpp
virtual void handleConnectResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleConnectErrorResponse 

```cpp
virtual void handleConnectErrorResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleConnectionBindResponse 

```cpp
virtual void handleConnectionBindResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleConnectionBindErrorResponse 

```cpp
virtual void handleConnectionBindErrorResponse(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### handleConnectionAttemptIndication 

```cpp
virtual void handleConnectionAttemptIndication(const stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const stun::Message &` |  |

---

#### createAndBindConnection 

```cpp
virtual bool createAndBindConnection(uint32_t connectionID, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `connectionID` | `uint32_t` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onRelayConnectionConnect 

```cpp
virtual void onRelayConnectionConnect(net::Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### onRelayDataReceived 

```cpp
virtual void onRelayDataReceived(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onRelayConnectionError 

```cpp
virtual void onRelayConnectionError(net::Socket & socket, const Error & error)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `error` | `const Error &` |  |

---

#### onRelayConnectionClosed 

```cpp
virtual void onRelayConnectionClosed(net::Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### freeConnection 

```cpp
void freeConnection(const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `peerAddress` | `const net::Address &` |  |

---

#### transportProtocol 

```cpp
virtual int transportProtocol()
```

---

#### connections 

```cpp
ConnectionManager & connections()
```

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### _observer 

```cpp
TCPClientObserver & _observer
```

---

#### _connections 

```cpp
ConnectionManager _connections
```

## UDPClient 

> **Extends:** `icy::turn::Client`
> **Defined in:** `udpclient.h`

### Members

| Name | Description |
|------|-------------|
| [`UDPClient`](#group__turn_1gac81111283b7df471a1afe3409587ae20) |  |
| [`~UDPClient`](#group__turn_1gac46009fd65b1194565e6f5b2a25ed3d5) |  |

---

#### UDPClient 

```cpp
UDPClient(ClientObserver & observer, const Options & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `observer` | `ClientObserver &` |  |
| `options` | `const Options &` |  |

---

#### ~UDPClient 

```cpp
virtual ~UDPClient()
```

## FiveTuple 

> **Defined in:** `fivetuple.h`

The 5-TUPLE consists of a local, a remote address, and the transport protocol used by the client to communicate with the server.                                                           +---------+
                                                          |         |
                                                          | External|
                                                        / | Client  |
                                                      //  |         |
                                                     /    |         |
                                                   //     +---------+
                                                  /
                                                //
                +-+                            /
                | |                           /
                | |                         //
 +---------+    | |          +---------+   /              +---------+
 |         |    |N|          |         | //               |         |
 | TURN    |    | |          |         |/                 | External|
 | Client  |----|A|----------|   TURN  |------------------| Client  |
 |         |    | |^        ^|  Server |^                ^|         |
 |         |    |T||        ||         ||                ||         |
 +---------+    | ||        |+---------+|                |+---------+
    ^           | ||        |           |                |
    |           | ||        |           |                |
    |           +-+|        |           |                |
    |              |        |           |                |
    |
               Internal     Internal    External         External
Client         Remote       Local       Local            Remote
Performing     Transport    Transport   Transport        Transport
Allocations    Address      Address     Address          Address

                   |          |            |                |
                   +-----+----+            +--------+-------+
                         |                          |
                         |                          |

                       Internal                External
                       5-Tuple                 5-tuple

### Members

| Name | Description |
|------|-------------|
| [`FiveTuple`](#group__turn_1ga7b1939ff40fd399a6e235149f46c240d) |  |
| [`FiveTuple`](#group__turn_1ga2bf36ada2e32974976906cbef746713d) |  |
| [`FiveTuple`](#group__turn_1ga9b05151cdd2cd10f1420c4a1bc60cd39) |  |
| [`remote`](#group__turn_1gaeb5c122700c33f78f0dc1c3dd76a8ede) |  |
| [`local`](#group__turn_1ga9cadde536d31752438fca9b62d69c463) |  |
| [`transport`](#group__turn_1ga4bb51c63a1d47d47a5ce9bc262f6abeb) |  |
| [`remote`](#group__turn_1ga4cfdde49f1c93769f29f20486a023de8) |  |
| [`local`](#group__turn_1ga6a5743d49cf4a503f4e336a75e6ab1ec) |  |
| [`transport`](#group__turn_1gaf92193a44ae9ae9eb7e490cc5d4ac889) |  |
| [`operator==`](#group__turn_1ga68c363f7fec160aedbc1e43e842cf7ef) |  |
| [`operator<`](#group__turn_1gaa8935b7a8df4ba5590d54571a244fb94) |  |
| [`toString`](#group__turn_1gad03e05d274665545b82f9978f121f9a4) |  |
| [`_remote`](#group__turn_1gacee27ca34748828d5a1e4b876b8a2eba) |  |
| [`_local`](#group__turn_1ga7eec69720319bddd10a38120854d7698) |  |
| [`_transport`](#group__turn_1gafe4ddb63e9ec25724be3301660d26f39) |  |

---

#### FiveTuple 

```cpp
FiveTuple()
```

---

#### FiveTuple 

```cpp
FiveTuple(const net::Address & remote, const net::Address & local, net::TransportType transport)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `remote` | `const net::Address &` |  |
| `local` | `const net::Address &` |  |
| `transport` | `net::TransportType` |  |

---

#### FiveTuple 

```cpp
FiveTuple(const FiveTuple & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const FiveTuple &` |  |

---

#### remote 

```cpp
inline const net::Address & remote() const
```

---

#### local 

```cpp
inline const net::Address & local() const
```

---

#### transport 

```cpp
inline const net::TransportType & transport() const
```

---

#### remote 

```cpp
inline void remote(const net::Address & remote)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `remote` | `const net::Address &` |  |

---

#### local 

```cpp
inline void local(const net::Address & local)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `local` | `const net::Address &` |  |

---

#### transport 

```cpp
inline void transport(const net::TransportType & transport)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `transport` | `const net::TransportType &` |  |

---

#### operator== 

```cpp
bool operator==(const FiveTuple & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const FiveTuple &` |  |

---

#### operator< 

```cpp
bool operator<(const FiveTuple & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const FiveTuple &` |  |

---

#### toString 

```cpp
std::string toString() const
```

---

#### _remote 

```cpp
net::Address _remote
```

---

#### _local 

```cpp
net::Address _local
```

---

#### _transport 

```cpp
net::TransportType _transport
```

## IAllocation 

> **Subclasses:** `icy::turn::Client`, `icy::turn::ServerAllocation`
> **Defined in:** `iallocation.h`

All TURN operations revolve around allocations, and all TURN messages are associated with an allocation. An allocation conceptually consists of the following state data:

* the relayed transport address;

* the 5-tuple: (client's IP address, client's port, server IP address, server port, transport protocol);

* the authentication information;

* the time-to-expiry;

a list of permissions;

* a list of channel to peer bindings.

The relayed transport address is the transport address allocated by the server for communicating with peers, while the 5-tuple describes the communication path between the client and the server. On the client, the 5-tuple uses the client's host transport address; on the server, the 5-tuple uses the client's server-reflexive transport address.

Both the relayed transport address and the 5-tuple MUST be unique across all allocations, so either one can be used to uniquely identify the allocation. The authentication information (e.g., username, password, realm, and nonce) is used to both verify subsequent requests and to compute the message integrity of responses. The username, realm, and nonce values are initially those used in the authenticated Allocate request that creates the allocation, though the server can change the nonce value during the lifetime of the allocation using a 438 (Stale Nonce) reply. Note that, rather than storing the password explicitly, for security reasons, it may be desirable for the server to store the key value, which is an MD5 hash over the username, realm, and password (see [RFC5389]).

The time-to-expiry is the time in seconds left until the allocation expires. Each Allocate or Refresh transaction sets this timer, which then ticks down towards 0. By default, each Allocate or Refresh transaction resets this timer to the default lifetime value of 600 seconds (10 minutes), but the client can request a different value in the Allocate and Refresh request. Allocations can only be refreshed using the Refresh request; sending data to a peer does not refresh an allocation. When an allocation expires, the state data associated with the allocation can be freed.

### Members

| Name | Description |
|------|-------------|
| [`IAllocation`](#group__turn_1gab486a4e544ced3afa2e9af62cbba7300) |  |
| [`~IAllocation`](#group__turn_1ga072f2bd3c27a11c556e95ca2f64a1b96) |  |
| [`updateUsage`](#group__turn_1gabe3a1c0062b6a83c84afb534016a2518) | Updates the allocation's internal timeout and bandwidth usage each time the allocation is used. |
| [`setLifetime`](#group__turn_1gac08a8899932878a7f5a0fdcc0b48a071) | Sets the lifetime of the allocation and resets the timeout. |
| [`setBandwidthLimit`](#group__turn_1ga8e2c86e075882c326d4c9bde49901a07) | Sets the bandwidth limit in bytes for this allocation. |
| [`expired`](#group__turn_1ga853e61b93b00bf9298f2e6b63f09e175) | Returns true if the allocation is expired ie. is timed out or the bandwidth limit has been reached. |
| [`deleted`](#group__turn_1ga9a7c145937ae53982675cef2f30bfb94) | Returns true if the allocation's deleted flag is set and or if the allocation has expired. |
| [`bandwidthLimit`](#group__turn_1gaa53520d7f1d391551df07362f251e12d) |  |
| [`bandwidthUsed`](#group__turn_1ga83e1e4abe2f5f66d276efde8912f07fe) |  |
| [`bandwidthRemaining`](#group__turn_1ga6fae3daac8a208fe57d3415e17f9efb3) |  |
| [`timeRemaining`](#group__turn_1gae4af5f598634519896c21d05b2ae3dff) |  |
| [`tuple`](#group__turn_1ga1654e4d9fa04276e5f9e4bcf890bf744) |  |
| [`username`](#group__turn_1ga9c6bf9973f8f4f0dda5aa9db792ab7cd) |  |
| [`lifetime`](#group__turn_1ga849c4d7c1ee031980c799b30896ea3d0) |  |
| [`permissions`](#group__turn_1gaa8d3ddee83af5feb781b95ef9453e4f1) |  |
| [`relayedAddress`](#group__turn_1ga5541d6593c96ed16b5b82d7ed5e77d7d) |  |
| [`addPermission`](#group__turn_1ga30fd3724a345cc202bc2c147f2fe0a0f) |  |
| [`addPermissions`](#group__turn_1gaa48e79d80a25a5652d6ce7d1a8b20fed) |  |
| [`removePermission`](#group__turn_1ga9204e82ce85c16b73ae5ba216f9d12f3) |  |
| [`removeAllPermissions`](#group__turn_1gac156f8c87e140b20655b276d4fb2b99f) |  |
| [`removeExpiredPermissions`](#group__turn_1ga3b88ba0d8ef79bccb52cf4bdba1b6be9) |  |
| [`hasPermission`](#group__turn_1ga65e660c87fbe3a3ec5ff449a0b271c34) |  |
| [`print`](#group__turn_1ga0bcf2cb6675c36864ac804c12bff53c4) |  |
| [`_mutex`](#group__turn_1gafca7ccdb9a9e02a6cb14727308fa3d9b) |  |
| [`_tuple`](#group__turn_1ga7c8695af0fa8d476fd2b3395c0e7acda) |  |
| [`_username`](#group__turn_1gaf6bbdd7265acb0086e3946281bcab790) |  |
| [`_permissions`](#group__turn_1ga8c38ed137db89cd1bee04ae3a94cf32a) |  |
| [`_lifetime`](#group__turn_1ga4dcd6ce15037d29fd6d9b6a9a2ce470c) |  |
| [`_bandwidthLimit`](#group__turn_1ga3a9549208c623572daf3e0fad84c17c1) |  |
| [`_bandwidthUsed`](#group__turn_1ga3712c06d098b80de261f7476fa7cf445) |  |
| [`_createdAt`](#group__turn_1ga70a95a5e99046c782c602708ab305573) |  |
| [`_updatedAt`](#group__turn_1ga584485c991cbbc5a060b48b778d62b8e) |  |
| [`_deleted`](#group__turn_1ga71472ac047f1522bf0bb6ac08f6e5c69) |  |

---

#### IAllocation 

```cpp
IAllocation(const FiveTuple & tuple, const std::string & username, std::int64_t lifetime)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `tuple` | `const FiveTuple &` |  |
| `username` | `const std::string &` |  |
| `lifetime` | `std::int64_t` |  |

---

#### ~IAllocation 

```cpp
virtual ~IAllocation()
```

---

#### updateUsage 

```cpp
virtual void updateUsage(std::int64_t numBytes)
```

Updates the allocation's internal timeout and bandwidth usage each time the allocation is used.

| Parameter | Type | Description |
|-----------|------|-------------|
| `numBytes` | `std::int64_t` |  |

---

#### setLifetime 

```cpp
virtual void setLifetime(std::int64_t lifetime)
```

Sets the lifetime of the allocation and resets the timeout.

| Parameter | Type | Description |
|-----------|------|-------------|
| `lifetime` | `std::int64_t` |  |

---

#### setBandwidthLimit 

```cpp
virtual void setBandwidthLimit(std::int64_t numBytes)
```

Sets the bandwidth limit in bytes for this allocation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `numBytes` | `std::int64_t` |  |

---

#### expired 

```cpp
virtual bool expired() const
```

Returns true if the allocation is expired ie. is timed out or the bandwidth limit has been reached.

---

#### deleted 

```cpp
virtual bool deleted() const
```

Returns true if the allocation's deleted flag is set and or if the allocation has expired.

This signifies that the allocation is ready to be destroyed via async garbage collection. See [Server::onTimer()](#group__turn_1gad98e27c134b19c27bd2fc446aa927694) and [Client::onTimer()](#group__turn_1ga7fb62ec5f7d6748b7ba6549b86709344)

---

#### bandwidthLimit 

```cpp
virtual std::int64_t bandwidthLimit() const
```

---

#### bandwidthUsed 

```cpp
virtual std::int64_t bandwidthUsed() const
```

---

#### bandwidthRemaining 

```cpp
virtual std::int64_t bandwidthRemaining() const
```

---

#### timeRemaining 

```cpp
virtual std::int64_t timeRemaining() const
```

---

#### tuple 

```cpp
virtual FiveTuple & tuple()
```

---

#### username 

```cpp
virtual std::string username() const
```

---

#### lifetime 

```cpp
virtual std::int64_t lifetime() const
```

---

#### permissions 

```cpp
virtual PermissionList permissions() const
```

---

#### relayedAddress 

```cpp
net::Address relayedAddress() const
```

---

#### addPermission 

```cpp
virtual void addPermission(const std::string & ip)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ip` | `const std::string &` |  |

---

#### addPermissions 

```cpp
virtual void addPermissions(const IPList & ips)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ips` | `const IPList &` |  |

---

#### removePermission 

```cpp
virtual void removePermission(const std::string & ip)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ip` | `const std::string &` |  |

---

#### removeAllPermissions 

```cpp
virtual void removeAllPermissions()
```

---

#### removeExpiredPermissions 

```cpp
virtual void removeExpiredPermissions()
```

---

#### hasPermission 

```cpp
virtual bool hasPermission(const std::string & peerIP)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `peerIP` | `const std::string &` |  |

---

#### print 

```cpp
virtual inline void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _tuple 

```cpp
FiveTuple _tuple
```

---

#### _username 

```cpp
std::string _username
```

---

#### _permissions 

```cpp
PermissionList _permissions
```

---

#### _lifetime 

```cpp
std::int64_t _lifetime
```

---

#### _bandwidthLimit 

```cpp
std::int64_t _bandwidthLimit
```

---

#### _bandwidthUsed 

```cpp
std::int64_t _bandwidthUsed
```

---

#### _createdAt 

```cpp
time_t _createdAt
```

---

#### _updatedAt 

```cpp
time_t _updatedAt
```

---

#### _deleted 

```cpp
bool _deleted
```

## Server 

> **Defined in:** `server.h`

TURN server rfc5766 implementation.

### Members

| Name | Description |
|------|-------------|
| [`Server`](#group__turn_1ga2a8d83c2d9447d113d890b3850ea57fc) |  |
| [`~Server`](#group__turn_1ga4abeb288eb73e7bed7699a4ed532564c) |  |
| [`start`](#group__turn_1ga3f604371cc77d7849381505a586242c1) |  |
| [`stop`](#group__turn_1gaf06ef5bbb67fe0d064273adf1bd47a64) |  |
| [`handleRequest`](#group__turn_1gacb5bac1767407f17246c357bae94d8bc) |  |
| [`handleAuthorizedRequest`](#group__turn_1gaab030a0ce83ea7ce592ee288c8ad3e45) |  |
| [`handleBindingRequest`](#group__turn_1gaa5867d6f3c8126b0c1dd8daedf72455e) |  |
| [`handleAllocateRequest`](#group__turn_1ga3b400ef28f963bbd282b747299762d53) |  |
| [`handleConnectionBindRequest`](#group__turn_1ga9c45a88e010c2141510fdd7adbbaa331) |  |
| [`respond`](#group__turn_1gafcae1c06fb5122cf021d731934c8725b) |  |
| [`respondError`](#group__turn_1gadf23ef7b1ca2346b6b0ed57a865674aa) |  |
| [`allocations`](#group__turn_1gac5a3297389a4f1b4f7f8a8297f38d7d4) | Returns a snapshot copy of the allocation map for safe iteration. Callers receive raw pointers that are valid only while the corresponding unique_ptr in _allocations is alive. |
| [`addAllocation`](#group__turn_1ga0f9507b513b5b0608085de25aa3b1862) |  |
| [`removeAllocation`](#group__turn_1gaef7c8d35bd764c75388dce31f63d3eae) |  |
| [`getAllocation`](#group__turn_1ga58132f7190c65ecad4f141eef0f3c971) |  |
| [`getTCPAllocation`](#group__turn_1ga77c93ca11da8f7de8a0b95c048c8e8c2) |  |
| [`getTCPSocket`](#group__turn_1ga67b8b43fd81875a89642793685adc25a) |  |
| [`releaseTCPSocket`](#group__turn_1gac17bef322ec362bfb5b7bf6c2a8fa6a3) |  |
| [`observer`](#group__turn_1gae00e2da8460fa504d5c184a30153c808) |  |
| [`options`](#group__turn_1gac50bc8f6103c1f019a729e1fd8ab600d) |  |
| [`udpSocket`](#group__turn_1gadc7036d9cd7736d8dddeb01a08344bfa) |  |
| [`tcpSocket`](#group__turn_1ga87a51e97cb7537c4218e85d6c7f76eed) |  |
| [`timer`](#group__turn_1ga52cc24e000f44454546ddfccdc85b197) |  |
| [`onTCPAcceptConnection`](#group__turn_1gac7e0f1a43edbd4c6fb38536454e09825) |  |
| [`onTCPSocketClosed`](#group__turn_1ga0795966f2c9e6d86f5d3ed508409958f) |  |
| [`onSocketRecv`](#group__turn_1ga54e3e322a963a2a51287a650f76d7826) |  |
| [`onTimer`](#group__turn_1gad98e27c134b19c27bd2fc446aa927694) |  |
| [`_mutex`](#group__turn_1gae8b03ceb4deeae0c484c11832947a558) |  |
| [`_observer`](#group__turn_1gae3092b782905e055347186b2a5b1743f) |  |
| [`_options`](#group__turn_1ga8f6b7686421cf3346129c41b9033bf83) |  |
| [`_udpSocket`](#group__turn_1ga90b2575c1704eebdf810d3651c346f5b) |  |
| [`_tcpSocket`](#group__turn_1ga01eda07ea349c90434e83546fd43b84f) |  |
| [`_tcpSockets`](#group__turn_1ga8176a75aa704535baedb19739c234dc2) |  |
| [`_allocations`](#group__turn_1ga7752bede01f2191dc4f951638ab7463a) |  |
| [`_timer`](#group__turn_1ga8cbb0a7dc682401cc9bff193537628a7) |  |

---

#### Server 

```cpp
Server(ServerObserver & observer, const ServerOptions & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `observer` | `ServerObserver &` |  |
| `options` | `const ServerOptions &` |  |

---

#### ~Server 

```cpp
virtual ~Server()
```

---

#### start 

```cpp
virtual void start()
```

---

#### stop 

```cpp
virtual void stop()
```

---

#### handleRequest 

```cpp
void handleRequest(Request & request, AuthenticationState state)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |
| `state` | `AuthenticationState` |  |

---

#### handleAuthorizedRequest 

```cpp
void handleAuthorizedRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleBindingRequest 

```cpp
void handleBindingRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleAllocateRequest 

```cpp
void handleAllocateRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleConnectionBindRequest 

```cpp
void handleConnectionBindRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### respond 

```cpp
void respond(Request & request, stun::Message & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |
| `response` | `stun::Message &` |  |

---

#### respondError 

```cpp
void respondError(Request & request, int errorCode, const char * errorDesc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |
| `errorCode` | `int` |  |
| `errorDesc` | `const char *` |  |

---

#### allocations 

```cpp
std::map< FiveTuple, ServerAllocation * > allocations() const
```

Returns a snapshot copy of the allocation map for safe iteration. Callers receive raw pointers that are valid only while the corresponding unique_ptr in _allocations is alive.

---

#### addAllocation 

```cpp
void addAllocation(std::unique_ptr< ServerAllocation > alloc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `alloc` | `std::unique_ptr< ServerAllocation >` |  |

---

#### removeAllocation 

```cpp
void removeAllocation(ServerAllocation * alloc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `alloc` | `ServerAllocation *` |  |

---

#### getAllocation 

```cpp
ServerAllocation * getAllocation(const FiveTuple & tuple)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `tuple` | `const FiveTuple &` |  |

---

#### getTCPAllocation 

```cpp
TCPAllocation * getTCPAllocation(const uint32_t & connectionID)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `connectionID` | `const uint32_t &` |  |

---

#### getTCPSocket 

```cpp
net::TCPSocket::Ptr getTCPSocket(const net::Address & remoteAddr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `remoteAddr` | `const net::Address &` |  |

---

#### releaseTCPSocket 

```cpp
void releaseTCPSocket(const net::Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::Socket &` |  |

---

#### observer 

```cpp
ServerObserver & observer()
```

---

#### options 

```cpp
ServerOptions & options()
```

---

#### udpSocket 

```cpp
net::UDPSocket & udpSocket()
```

---

#### tcpSocket 

```cpp
net::TCPSocket & tcpSocket()
```

---

#### timer 

```cpp
Timer & timer()
```

---

#### onTCPAcceptConnection 

```cpp
void onTCPAcceptConnection(const net::TCPSocket::Ptr & sock)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sock` | `const net::TCPSocket::Ptr &` |  |

---

#### onTCPSocketClosed 

```cpp
void onTCPSocketClosed(net::Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### onSocketRecv 

```cpp
void onSocketRecv(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onTimer 

```cpp
void onTimer()
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _observer 

```cpp
ServerObserver & _observer
```

---

#### _options 

```cpp
ServerOptions _options
```

---

#### _udpSocket 

```cpp
net::SocketEmitter _udpSocket
```

---

#### _tcpSocket 

```cpp
net::SocketEmitter _tcpSocket
```

---

#### _tcpSockets 

```cpp
std::vector< net::SocketEmitter > _tcpSockets
```

---

#### _allocations 

```cpp
ServerAllocationMap _allocations
```

---

#### _timer 

```cpp
Timer _timer
```

## ServerAllocation 

> **Extends:** `icy::turn::IAllocation`
> **Subclasses:** `icy::turn::TCPAllocation`, `icy::turn::UDPAllocation`
> **Defined in:** `serverallocation.h`

### Members

| Name | Description |
|------|-------------|
| [`ServerAllocation`](#group__turn_1ga84047d7fa94573c8b4c9e90d7b2ca35e) |  |
| [`~ServerAllocation`](#group__turn_1ga5a72f2a758d972ead206cd52fcc47132) |  |
| [`handleRequest`](#group__turn_1ga196225684548a5af62dbaf4536a40a6c) |  |
| [`handleRefreshRequest`](#group__turn_1ga067a994e4f4b78f1208d946b526b7782) |  |
| [`handleCreatePermission`](#group__turn_1ga123f3fce597db5b9303583eb95c830dc) |  |
| [`onTimer`](#group__turn_1gae360050350229d817fda8d6317531862) | Asynchronous timer callback for updating the allocation permissions and state etc. If this call returns false the allocation will be deleted. |
| [`timeRemaining`](#group__turn_1ga86f851d76c345f440db48a3b8cc373ea) |  |
| [`maxTimeRemaining`](#group__turn_1gac32ad9173f59a4ee7fff11ae498a96f6) |  |
| [`server`](#group__turn_1gac782e622e15da990801af994dbfa7bfa) |  |
| [`print`](#group__turn_1ga7e83857083889f67c5f29a754a34aa24) |  |
| [`refreshDeleteRequested`](#group__turn_1ga5872b7dd0f15d787034ef80180d54995) | Returns true if the refresh request set lifetime to 0, signaling the parent [Server](#classicy_1_1turn_1_1Server) to delete this allocation. |
| [`ServerAllocation`](#group__turn_1gae436d8a65e9a1a8a6f22cdeb46967690) | NonCopyable and NonMovable. |
| [`operator=`](#group__turn_1gafc5723b65e7b03176bd894f75717676c) |  |
| [`_maxLifetime`](#group__turn_1gaffd29cbaf5c7b75d02f538b1f9535f97) |  |
| [`_server`](#group__turn_1ga8c62f3c453e32f9c710ac172ad633aca) |  |
| [`_refreshDeleteRequested`](#group__turn_1gaad617080bf2c8caf7d3843838aaeef87) |  |

---

#### ServerAllocation 

```cpp
ServerAllocation(Server & server, const FiveTuple & tuple, const std::string & username, std::int64_t lifetime)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `Server &` |  |
| `tuple` | `const FiveTuple &` |  |
| `username` | `const std::string &` |  |
| `lifetime` | `std::int64_t` |  |

---

#### ~ServerAllocation 

```cpp
virtual ~ServerAllocation()
```

---

#### handleRequest 

```cpp
virtual bool handleRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleRefreshRequest 

```cpp
virtual void handleRefreshRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleCreatePermission 

```cpp
virtual void handleCreatePermission(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### onTimer 

```cpp
virtual bool onTimer()
```

Asynchronous timer callback for updating the allocation permissions and state etc. If this call returns false the allocation will be deleted.

---

#### timeRemaining 

```cpp
virtual std::int64_t timeRemaining() const
```

---

#### maxTimeRemaining 

```cpp
virtual std::int64_t maxTimeRemaining() const
```

---

#### server 

```cpp
virtual Server & server()
```

---

#### print 

```cpp
virtual void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### refreshDeleteRequested 

```cpp
inline bool refreshDeleteRequested() const
```

Returns true if the refresh request set lifetime to 0, signaling the parent [Server](#classicy_1_1turn_1_1Server) to delete this allocation.

---

#### ServerAllocation 

```cpp
ServerAllocation(const ServerAllocation &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
ServerAllocation & operator=(const ServerAllocation &) = delete
```

---

#### _maxLifetime 

```cpp
uint32_t _maxLifetime
```

---

#### _server 

```cpp
Server & _server
```

---

#### _refreshDeleteRequested 

```cpp
bool _refreshDeleteRequested = false
```

## TCPAllocation 

> **Extends:** `icy::turn::ServerAllocation`
> **Defined in:** `tcpallocation.h`

### Members

| Name | Description |
|------|-------------|
| [`TCPAllocation`](#group__turn_1ga530362865a2ac00cec08e42dc73403a9) |  |
| [`~TCPAllocation`](#group__turn_1ga312379a655b3f96bcdb99e79fb689710) |  |
| [`handleRequest`](#group__turn_1ga6c454d8d13f6c5a006731cf8e5d9b517) |  |
| [`handleConnectRequest`](#group__turn_1ga581cc086264aaebb23867da082b69d08) |  |
| [`handleConnectionBindRequest`](#group__turn_1ga90d0b7dc91442cd9b165f0bbd597130e) |  |
| [`sendPeerConnectResponse`](#group__turn_1gadfc50816e7433c9290781b9f906499ca) | Sends a Connect request response to control. |
| [`sendToControl`](#group__turn_1ga938af5d30b8c6f5d61812f8a241a0a0f) |  |
| [`control`](#group__turn_1ga6b941622915a8607a4cedc7ee171437c) |  |
| [`relayedAddress`](#group__turn_1ga7b3153537c0a9fd9453277443dcba25f) |  |
| [`pairs`](#group__turn_1ga883345d6de7c95b47330a0b0daaeb19b) |  |
| [`onTimer`](#group__turn_1ga263e247e447a7eaa989711bbd78a249a) | Asynchronous timer callback for updating the allocation permissions and state etc. If this call returns false the allocation will be deleted. |
| [`onPeerAccept`](#group__turn_1ga9c6c658f89aa5c1ebc89a80f12d0a570) | Accepts incoming peer sockets for ConnectionBind requests. |
| [`onControlClosed`](#group__turn_1gad7d9a3b524c3439fc19642ce621d306c) | Callback for handling controll connection destruction. The allocation will be deleted. |
| [`_control`](#group__turn_1ga76a78eca6ac1212ddfbf6512213e6901) |  |
| [`_acceptor`](#group__turn_1ga6ae65a9eaa6379fa513984c4a30522e4) |  |
| [`_pairs`](#group__turn_1ga865b233b3ce8e50c49c91c9fccc27920) |  |

---

#### TCPAllocation 

```cpp
TCPAllocation(Server & server, const net::Socket::Ptr & control, const FiveTuple & tuple, const std::string & username, const uint32_t & lifetime)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `Server &` |  |
| `control` | `const net::Socket::Ptr &` |  |
| `tuple` | `const FiveTuple &` |  |
| `username` | `const std::string &` |  |
| `lifetime` | `const uint32_t &` |  |

---

#### ~TCPAllocation 

```cpp
virtual ~TCPAllocation()
```

---

#### handleRequest 

```cpp
virtual bool handleRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleConnectRequest 

```cpp
void handleConnectRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleConnectionBindRequest 

```cpp
void handleConnectionBindRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### sendPeerConnectResponse 

```cpp
void sendPeerConnectResponse(TCPConnectionPair * pair, bool success)
```

Sends a Connect request response to control.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pair` | `TCPConnectionPair *` |  |
| `success` | `bool` |  |

---

#### sendToControl 

```cpp
int sendToControl(stun::Message & message)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `stun::Message &` |  |

---

#### control 

```cpp
net::TCPSocket & control()
```

---

#### relayedAddress 

```cpp
virtual net::Address relayedAddress() const
```

---

#### pairs 

```cpp
TCPConnectionPairMap & pairs()
```

---

#### onTimer 

```cpp
virtual bool onTimer()
```

Asynchronous timer callback for updating the allocation permissions and state etc. If this call returns false the allocation will be deleted.

---

#### onPeerAccept 

```cpp
void onPeerAccept(const net::TCPSocket::Ptr & sock)
```

Accepts incoming peer sockets for ConnectionBind requests.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sock` | `const net::TCPSocket::Ptr &` |  |

---

#### onControlClosed 

```cpp
void onControlClosed(net::Socket & socket)
```

Callback for handling controll connection destruction. The allocation will be deleted.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### _control 

```cpp
net::SocketEmitter _control
```

---

#### _acceptor 

```cpp
net::SocketEmitter _acceptor
```

---

#### _pairs 

```cpp
TCPConnectionPairMap _pairs
```

## TCPConnectionPair 

> **Defined in:** `tcpconnectionpair.h`

### Members

| Name | Description |
|------|-------------|
| [`TCPConnectionPair`](#group__turn_1gaef6a16730e04ad6935b74d7688d434ff) |  |
| [`~TCPConnectionPair`](#group__turn_1ga624ca19946f081cf19f75b0b185f88bd) |  |
| [`doPeerConnect`](#group__turn_1ga5fa2395970db2f91d0c39b2344293b4a) | Initiates an outgoing TCP connection to the peer for Connect requests. |
| [`makeDataConnection`](#group__turn_1ga9e9fa7894dcb1ba7aead4a9ce89ad2a8) | Binds the client <> peer relay pipe once the ConnectionBind request is successfull. |
| [`setPeerSocket`](#group__turn_1gabb87674d00d60b47ec5799578c4303e8) |  |
| [`setClientSocket`](#group__turn_1gabe1aec80b35fe3f161fc38515f139b00) |  |
| [`onPeerConnectSuccess`](#group__turn_1ga5a5912bc68b1a4c405a1347ea8fb82bb) | Connection success callback for Connect request. |
| [`onPeerConnectError`](#group__turn_1gabcbaa9d2da0811338030f6368c5eecea) | Connection error callback for Connect request. |
| [`onClientDataReceived`](#group__turn_1ga7850f0162f9bdd3d1d7b853a59324ddd) |  |
| [`onPeerDataReceived`](#group__turn_1gae93b12998cb75c2784b8b4a83b8ed78e) |  |
| [`onConnectionClosed`](#group__turn_1ga4f90949f22078df17775d5907451fa47) | Callback for handing either client or peer connections which result in the destruction of the [TCPConnectionPair](#classicy_1_1turn_1_1TCPConnectionPair). |
| [`startTimeout`](#group__turn_1ga36352c6ddc19d8bdfef9968df7dcc165) | Starts the ConnectionBind request timeout. |
| [`requestDeletion`](#group__turn_1ga2ac9623b2eccd557b9a56fae2cd0beca) | Signals the parent allocation to handle deletion instead of destroying ourselves directly. |
| [`expired`](#group__turn_1gae31259009aac3c03ea440ec0a4bb8ff7) | Return true if the peer `ConnectionBind` request timed out. |
| [`TCPConnectionPair`](#group__turn_1ga051968b4d67df635b32673ba0241177e) | NonCopyable and NonMovable. |
| [`operator=`](#group__turn_1gaac0eea313143b657b08277ed2aed266b) |  |
| [`allocation`](#group__turn_1ga5f8ce0ee36a5bc87f55d76a95d4ffdc6) |  |
| [`client`](#group__turn_1ga7299639b74d587a37787494d14674388) | The client socket, nullptr to start. |
| [`peer`](#group__turn_1ga519217dcc39475da5dc9e616266fbbaf) | The client socket, nullptr to start. |
| [`earlyPeerData`](#group__turn_1ga7d517bff868ad4aeb1a191abb6d440df) | Stores early peer > client data. |
| [`connectionID`](#group__turn_1gae745074629773597cef0ea7c2ed059c7) | The unique connection ID. |
| [`isDataConnection`](#group__turn_1gaa0975b408c5100d385ab97ab54492441) | True when p2p relay is flowing. |
| [`pendingDelete`](#group__turn_1ga234ee6b25a6996fc2c61e3d05d6deef9) | True when this pair has been flagged for deletion. |
| [`timeout`](#group__turn_1gad2382d7f6eabb79e3ddd8b5ae7fe17f1) | The ConnectionBind request timeout counter. |
| [`transactionID`](#group__turn_1gaa13970f4c75250ead1dccdfb573c3af5) |  |

---

#### TCPConnectionPair 

```cpp
TCPConnectionPair(TCPAllocation & allocation)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `allocation` | `TCPAllocation &` |  |

---

#### ~TCPConnectionPair 

```cpp
virtual ~TCPConnectionPair()
```

---

#### doPeerConnect 

```cpp
bool doPeerConnect(const net::Address & peerAddr)
```

Initiates an outgoing TCP connection to the peer for Connect requests.

| Parameter | Type | Description |
|-----------|------|-------------|
| `peerAddr` | `const net::Address &` |  |

---

#### makeDataConnection 

```cpp
bool makeDataConnection()
```

Binds the client <> peer relay pipe once the ConnectionBind request is successfull.

---

#### setPeerSocket 

```cpp
void setPeerSocket(const net::TCPSocket::Ptr & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::TCPSocket::Ptr &` |  |

---

#### setClientSocket 

```cpp
void setClientSocket(const net::TCPSocket::Ptr & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::TCPSocket::Ptr &` |  |

---

#### onPeerConnectSuccess 

```cpp
void onPeerConnectSuccess(net::Socket & socket)
```

Connection success callback for Connect request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### onPeerConnectError 

```cpp
void onPeerConnectError(net::Socket & socket, const Error & error)
```

Connection error callback for Connect request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `error` | `const Error &` |  |

---

#### onClientDataReceived 

```cpp
void onClientDataReceived(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onPeerDataReceived 

```cpp
void onPeerDataReceived(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onConnectionClosed 

```cpp
void onConnectionClosed(net::Socket & socket)
```

Callback for handing either client or peer connections which result in the destruction of the [TCPConnectionPair](#classicy_1_1turn_1_1TCPConnectionPair).

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |

---

#### startTimeout 

```cpp
void startTimeout()
```

Starts the ConnectionBind request timeout.

If no ConnectionBind request associated with this peer data connection is received after 30 seconds, the peer data connection MUST be closed.

---

#### requestDeletion 

```cpp
void requestDeletion()
```

Signals the parent allocation to handle deletion instead of destroying ourselves directly.

---

#### expired 

```cpp
bool expired() const
```

Return true if the peer `ConnectionBind` request timed out.

---

#### TCPConnectionPair 

```cpp
TCPConnectionPair(const TCPConnectionPair &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
TCPConnectionPair & operator=(const TCPConnectionPair &) = delete
```

---

#### allocation 

```cpp
TCPAllocation & allocation
```

---

#### client 

```cpp
net::SocketEmitter client
```

The client socket, nullptr to start.

---

#### peer 

```cpp
net::SocketEmitter peer
```

The client socket, nullptr to start.

---

#### earlyPeerData 

```cpp
Buffer earlyPeerData
```

Stores early peer > client data.

---

#### connectionID 

```cpp
uint32_t connectionID
```

The unique connection ID.

---

#### isDataConnection 

```cpp
bool isDataConnection
```

True when p2p relay is flowing.

---

#### pendingDelete 

```cpp
bool pendingDelete = false
```

True when this pair has been flagged for deletion.

---

#### timeout 

```cpp
Timeout timeout
```

The ConnectionBind request timeout counter.

---

#### transactionID 

```cpp
stun::TransactionID transactionID
```

## UDPAllocation 

> **Extends:** `icy::turn::ServerAllocation`
> **Defined in:** `udpallocation.h`

### Members

| Name | Description |
|------|-------------|
| [`UDPAllocation`](#group__turn_1gad25035e1f05871ee31e609469e6f3e1c) |  |
| [`~UDPAllocation`](#group__turn_1gacc331eb4c29175167d482de8fd682b2b) |  |
| [`onPeerDataReceived`](#group__turn_1gaf3723305361367129ac0dbcaad73b8dd) |  |
| [`handleRequest`](#group__turn_1gaadfa6b2dd671c0b9de943c687b4e507b) |  |
| [`handleSendIndication`](#group__turn_1ga96ad3bfc1e1c63895c320c92f14462ee) |  |
| [`send`](#group__turn_1ga1b4d823f104e4b9e324ade9497604b0f) |  |
| [`relayedAddress`](#group__turn_1ga14647342910901c835819da6c0dfd7bc) |  |
| [`_relaySocket`](#group__turn_1gabfed35bd8ab375ca916f357ddbf97595) |  |

---

#### UDPAllocation 

```cpp
UDPAllocation(Server & server, const FiveTuple & tuple, const std::string & username, const uint32_t & lifetime)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `Server &` |  |
| `tuple` | `const FiveTuple &` |  |
| `username` | `const std::string &` |  |
| `lifetime` | `const uint32_t &` |  |

---

#### ~UDPAllocation 

```cpp
virtual ~UDPAllocation()
```

---

#### onPeerDataReceived 

```cpp
void onPeerDataReceived(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### handleRequest 

```cpp
virtual bool handleRequest(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### handleSendIndication 

```cpp
void handleSendIndication(Request & request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `Request &` |  |

---

#### send 

```cpp
ssize_t send(const char * data, size_t size, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `size` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### relayedAddress 

```cpp
virtual net::Address relayedAddress() const
```

---

#### _relaySocket 

```cpp
net::SocketEmitter _relaySocket
```

## Request 

> **Extends:** `icy::stun::Message`
> **Defined in:** `types.h`

### Members

| Name | Description |
|------|-------------|
| [`Request`](#group__turn_1ga9449353e6e8218f409dc6cad8137bad1) |  |
| [`transport`](#group__turn_1gad73229b9a0ed2c9766a35e6464921d83) |  |
| [`localAddress`](#group__turn_1gaa5eaa3335646687346a79d16bf44d500) |  |
| [`remoteAddress`](#group__turn_1ga063c2acad8fd94c729479e6ad8b3b9f7) |  |
| [`hash`](#group__turn_1ga220ca9d82c58f5ade3bf21ab77250590) |  |

---

#### Request 

```cpp
inline Request(const stun::Message & message, net::TransportType transport, const net::Address & localAddress, const net::Address & remoteAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `const stun::Message &` |  |
| `transport` | `net::TransportType` |  |
| `localAddress` | `const net::Address &` |  |
| `remoteAddress` | `const net::Address &` |  |

---

#### transport 

```cpp
net::TransportType transport
```

---

#### localAddress 

```cpp
net::Address localAddress
```

---

#### remoteAddress 

```cpp
net::Address remoteAddress
```

---

#### hash 

```cpp
std::string hash
```

## ClientState 

> **Extends:** `icy::State`
> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__turn_1ga969e50bad1780d036f55059904ac2a1e) |  |
| [`toString`](#group__turn_1ga64488300060951e76776bd292ee76701) |  |

---

#### Type 

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` |  |
| `Allocating` |  |
| `Authorizing` |  |
| `Success` |  |
| `Failed` |  |

---

#### toString 

```cpp
virtual inline std::string toString() const
```

## ClientObserver 

> **Subclasses:** `icy::turn::TCPClientObserver`
> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`onClientStateChange`](#group__turn_1ga2f8c99aadad178228ca800c56c66a53a) |  |
| [`onRelayDataReceived`](#group__turn_1ga289952ed0e63f289511dcb4e89d8b993) |  |
| [`onAllocationFailed`](#group__turn_1gaf843439ada68af5d0a8c44b9e6f84cef) |  |
| [`onAllocationDeleted`](#group__turn_1ga2fa9e57662dbf68959503085fa781d02) |  |
| [`onAllocationPermissionsCreated`](#group__turn_1ga26b4ea2e7a7f1a728218bda75b670c89) |  |
| [`onTransactionResponse`](#group__turn_1ga0b96125a01852fdcd24ecdc7e9eba5af) | All received transaction responses will be routed here after local processing so the observer can easily implement extra functionality. |
| [`onTimer`](#group__turn_1gac3b5b3eae27058463dc291d16883bbb6) | Fires after the client's internal timer callback. Handy for performing extra async cleanup tasks. |

---

#### onClientStateChange 

```cpp
void onClientStateChange(Client & client, ClientState & state, const ClientState & oldState)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |
| `state` | `ClientState &` |  |
| `oldState` | `const ClientState &` |  |

---

#### onRelayDataReceived 

```cpp
void onRelayDataReceived(Client & client, const char * data, size_t size, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |
| `data` | `const char *` |  |
| `size` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onAllocationFailed 

```cpp
virtual inline void onAllocationFailed(Client & client, int errorCode, const std::string & reason)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |
| `errorCode` | `int` |  |
| `reason` | `const std::string &` |  |

---

#### onAllocationDeleted 

```cpp
virtual inline void onAllocationDeleted(Client & client, const stun::Transaction & transaction)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |
| `transaction` | `const stun::Transaction &` |  |

---

#### onAllocationPermissionsCreated 

```cpp
virtual inline void onAllocationPermissionsCreated(Client & client, const PermissionList & permissions)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |
| `permissions` | `const PermissionList &` |  |

---

#### onTransactionResponse 

```cpp
virtual inline void onTransactionResponse(Client & client, const stun::Transaction & transaction)
```

All received transaction responses will be routed here after local processing so the observer can easily implement extra functionality.

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |
| `transaction` | `const stun::Transaction &` |  |

---

#### onTimer 

```cpp
virtual inline void onTimer(Client & client)
```

Fires after the client's internal timer callback. Handy for performing extra async cleanup tasks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `Client &` |  |

## Options 

> **Defined in:** `client.h`

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__turn_1gae926546e19d8fd9a7fab86271b96a54f) |  |
| [`software`](#group__turn_1ga5353220869e0f09239060488ecdb7fef) |  |
| [`username`](#group__turn_1gac72217fb6b73d1d050c84379189354a5) |  |
| [`password`](#group__turn_1ga90e75f93e08bd392b9def185c4f29ec1) |  |
| [`timeout`](#group__turn_1ga3b36f9f68df1b465c1046b88529ccf69) |  |
| [`lifetime`](#group__turn_1ga7dc6b1fc053f08ec22d0f2f97084d996) |  |
| [`timerInterval`](#group__turn_1ga50b1abb4fea151715d0c93b5ac675f30) |  |
| [`serverAddr`](#group__turn_1gaca69ae3e4cb351a623def3f8ca67a950) |  |

---

#### Options 

```cpp
inline Options()
```

---

#### software 

```cpp
std::string software
```

---

#### username 

```cpp
std::string username
```

---

#### password 

```cpp
std::string password
```

---

#### timeout 

```cpp
long timeout
```

---

#### lifetime 

```cpp
std::int64_t lifetime
```

---

#### timerInterval 

```cpp
std::int64_t timerInterval
```

---

#### serverAddr 

```cpp
net::Address serverAddr
```

## TCPClientObserver 

> **Extends:** `icy::turn::ClientObserver`
> **Defined in:** `tcpclient.h`

### Members

| Name | Description |
|------|-------------|
| [`onRelayConnectionCreated`](#group__turn_1ga9cadceec9bf3a4687f3b32ea67404e8f) |  |
| [`onRelayConnectionError`](#group__turn_1ga37d33d68190c4578c19b0bf376ad8f63) |  |
| [`onRelayConnectionClosed`](#group__turn_1gaa0e978e987f8b59662356bf9d51b4616) |  |
| [`onRelayConnectionBindingFailed`](#group__turn_1ga31a9a774ee2e1b13ca87e108bf6cf76e) |  |
| [`onPeerConnectionAttempt`](#group__turn_1gad6938fba6074bc0e1ca499cf90bb84ca) |  |

---

#### onRelayConnectionCreated 

```cpp
void onRelayConnectionCreated(TCPClient & client, const net::TCPSocket::Ptr & socket, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `TCPClient &` |  |
| `socket` | `const net::TCPSocket::Ptr &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onRelayConnectionError 

```cpp
virtual inline void onRelayConnectionError(TCPClient & client, const net::TCPSocket::Ptr & socket, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `TCPClient &` |  |
| `socket` | `const net::TCPSocket::Ptr &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onRelayConnectionClosed 

```cpp
void onRelayConnectionClosed(TCPClient & client, const net::TCPSocket::Ptr & socket, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `TCPClient &` |  |
| `socket` | `const net::TCPSocket::Ptr &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onRelayConnectionBindingFailed 

```cpp
virtual inline void onRelayConnectionBindingFailed(TCPClient & client, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `TCPClient &` |  |
| `peerAddress` | `const net::Address &` |  |

---

#### onPeerConnectionAttempt 

```cpp
virtual inline bool onPeerConnectionAttempt(TCPClient & client, const net::Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `client` | `TCPClient &` |  |
| `peerAddress` | `const net::Address &` |  |

## RelayConnectionBinding 

> **Defined in:** `tcpclient.h`

### Members

| Name | Description |
|------|-------------|
| [`connectionID`](#group__turn_1ga2c6b83886801342cdf489851451f02cb) |  |
| [`peerAddress`](#group__turn_1gab40590fa9feb9154fbd96678304cf5ce) |  |

---

#### connectionID 

```cpp
uint32_t connectionID
```

---

#### peerAddress 

```cpp
net::Address peerAddress
```

## Permission 

> **Defined in:** `permission.h`

TURN permission for a user session.

### Members

| Name | Description |
|------|-------------|
| [`Permission`](#group__turn_1ga1d84a2f95323c85990c9464c4a9d3256) |  |
| [`refresh`](#group__turn_1ga0b8b830b544ab68a84f3214563347080) |  |
| [`operator==`](#group__turn_1gabf7857f22a589a833b5aa79391b57830) |  |
| [`ip`](#group__turn_1gac3d4b7db7da99f5df332df065c3bbeba) |  |
| [`timeout`](#group__turn_1ga7f9706e123e06bacdfc229e9f2d4b503) |  |

---

#### Permission 

```cpp
inline Permission(const std::string & ip)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ip` | `const std::string &` |  |

---

#### refresh 

```cpp
inline void refresh()
```

---

#### operator== 

```cpp
inline bool operator==(const std::string & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const std::string &` |  |

---

#### ip 

```cpp
std::string ip
```

---

#### timeout 

```cpp
Timeout timeout
```

## ServerOptions 

> **Defined in:** `server.h`

[Configuration](./doc/api-base.md#classicy_1_1Configuration) options for the TURN server.

### Members

| Name | Description |
|------|-------------|
| [`ServerOptions`](#group__turn_1ga23b935569a27d3f3dcf0f78e8a5ca280) |  |
| [`software`](#group__turn_1gac2291c280fe0980304bca18ab44f8731) |  |
| [`realm`](#group__turn_1gaabb449fc0c3e896938868e9eba870ba4) |  |
| [`allocationDefaultLifetime`](#group__turn_1ga8d82709c768778e902772af859826ee6) |  |
| [`allocationMaxLifetime`](#group__turn_1ga7230b59c2f23a9a850a9a74c28a9114f) |  |
| [`allocationMaxPermissions`](#group__turn_1gab877b8fafddc71ed3de320159ff614e5) |  |
| [`timerInterval`](#group__turn_1ga30c595ee93bb3dfee3cd52faff6a4222) |  |
| [`earlyMediaBufferSize`](#group__turn_1ga9122325d7162eb5b1f23888d63cd282d) |  |
| [`listenAddr`](#group__turn_1gabd4aa0f66ce2fc218e80810f3c762d76) | The TCP and UDP bind() address. |
| [`externalIP`](#group__turn_1ga88891ec27dede5dc6be7219ed26c9b2a) | The external public facing IP address of the server. |
| [`enableTCP`](#group__turn_1ga1d0c462bf8701333710f2f1d4eb0f18a) |  |
| [`enableUDP`](#group__turn_1gaf34f6a53ee1dd805c81af5556deecdb9) |  |

---

#### ServerOptions 

```cpp
inline ServerOptions()
```

---

#### software 

```cpp
std::string software
```

---

#### realm 

```cpp
std::string realm
```

---

#### allocationDefaultLifetime 

```cpp
uint32_t allocationDefaultLifetime
```

---

#### allocationMaxLifetime 

```cpp
uint32_t allocationMaxLifetime
```

---

#### allocationMaxPermissions 

```cpp
int allocationMaxPermissions
```

---

#### timerInterval 

```cpp
int timerInterval
```

---

#### earlyMediaBufferSize 

```cpp
int earlyMediaBufferSize
```

---

#### listenAddr 

```cpp
net::Address listenAddr
```

The TCP and UDP bind() address.

---

#### externalIP 

```cpp
std::string externalIP
```

The external public facing IP address of the server.

---

#### enableTCP 

```cpp
bool enableTCP
```

---

#### enableUDP 

```cpp
bool enableUDP
```

## ServerObserver 

> **Defined in:** `server.h`

The [ServerObserver](#structicy_1_1turn_1_1ServerObserver) receives callbacks for and is responsible for managing allocation and bandwidth quotas, authentication methods and authentication.

### Members

| Name | Description |
|------|-------------|
| [`onServerAllocationCreated`](#group__turn_1gae40bc9446a48541dcbd436ed8fb6e1ff) |  |
| [`onServerAllocationRemoved`](#group__turn_1ga080fc643212385979ab6b66210bc15c1) |  |
| [`authenticateRequest`](#group__turn_1ga36ad126902d380f3b569e6a7c70837a1) | The observer class can implement authentication using the long-term credential mechanism of [RFC5389]. The class design is such that authentication can be preformed asynchronously against a remote database, or locally. The default implementation returns true to all requests. |

---

#### onServerAllocationCreated 

```cpp
void onServerAllocationCreated(Server * server, IAllocation * alloc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `Server *` |  |
| `alloc` | `IAllocation *` |  |

---

#### onServerAllocationRemoved 

```cpp
void onServerAllocationRemoved(Server * server, IAllocation * alloc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `Server *` |  |
| `alloc` | `IAllocation *` |  |

---

#### authenticateRequest 

```cpp
AuthenticationState authenticateRequest(Server * server, Request & request)
```

The observer class can implement authentication using the long-term credential mechanism of [RFC5389]. The class design is such that authentication can be preformed asynchronously against a remote database, or locally. The default implementation returns true to all requests.

To mitigate either intentional or unintentional denial-of-service attacks against the server by clients with valid usernames and passwords, it is RECOMMENDED that the server impose limits on both the number of allocations active at one time for a given username and on the amount of bandwidth those allocations can use. The server should reject new allocations that would exceed the limit on the allowed number of allocations active at one time with a 486 (Allocation Quota Exceeded) (see Section 6.2), and should discard application data traffic that exceeds the bandwidth quota.

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `Server *` |  |
| `request` | `Request &` |  |

