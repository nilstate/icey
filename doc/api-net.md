# net 

### Classes

| Name | Description |
|------|-------------|
| [`Address`](#classscy_1_1net_1_1Address) |  |
| [`PacketSocketEmitter`](#classscy_1_1net_1_1PacketSocketEmitter) |  |
| [`Socket`](#classscy_1_1net_1_1Socket) | Base socket implementation from which all sockets derive. |
| [`SocketPacket`](#classscy_1_1net_1_1SocketPacket) | [SocketPacket](#classscy_1_1net_1_1SocketPacket) is the default packet type emitted by sockets. [SocketPacket](#classscy_1_1net_1_1SocketPacket) provides peer address information and a buffer reference for nocopy binary operations. |
| [`SocketAdapter`](#classscy_1_1net_1_1SocketAdapter) | [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) is the abstract interface for all socket classes. A [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) can also be attached to a [Socket](#classscy_1_1net_1_1Socket) in order to override default [Socket](#classscy_1_1net_1_1Socket) callbacks and behaviour, while still maintaining the default [Socket](#classscy_1_1net_1_1Socket) interface (see Socket::setAdapter). |
| [`SocketEmitter`](#classscy_1_1net_1_1SocketEmitter) | [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) class that adds signal callbacks for sockets. Asde from adding a signal interface the class works as a wrapper for the socket instance, and is designed to be used the same way as a `std::unique_ptr` by overriding the ->() operator. |
| [`SSLAdapter`](#classscy_1_1net_1_1SSLAdapter) |  |
| [`SSLContext`](#classscy_1_1net_1_1SSLContext) | This class encapsulates context information for an SSL server or client, such as the certificate verification mode and the location of certificates and private key files, as well as the list of supported ciphers. |
| [`SSLManager`](#classscy_1_1net_1_1SSLManager) | [SSLManager](#classscy_1_1net_1_1SSLManager) is a singleton for holding the default server/client Context and handling callbacks for certificate verification errors and private key passphrases. |
| [`VerificationErrorDetails`](#classscy_1_1net_1_1VerificationErrorDetails) | A utility class for certificate error handling. |
| [`SSLSession`](#classscy_1_1net_1_1SSLSession) | This class encapsulates a SSL session object used with session caching on the client side. |
| [`SSLSocket`](#classscy_1_1net_1_1SSLSocket) | SSL socket implementation. |
| [`TCPSocket`](#classscy_1_1net_1_1TCPSocket) | TCP socket implementation. |
| [`Transaction`](#classscy_1_1net_1_1Transaction) | This class provides request/response functionality for [IPacket](./doc/api-base.md#classscy_1_1IPacket) types emitted from a [Socket](#classscy_1_1net_1_1Socket). |
| [`UDPSocket`](#classscy_1_1net_1_1UDPSocket) | UDP socket implementation. |
| [`AddressBase`](#classscy_1_1net_1_1AddressBase) |  |
| [`IPv4AddressBase`](#classscy_1_1net_1_1IPv4AddressBase) |  |
| [`PacketInfo`](#structscy_1_1net_1_1PacketInfo) | Provides information about packets emitted from a socket. See [SocketPacket](#classscy_1_1net_1_1SocketPacket). |
| [`Ref`](#structscy_1_1net_1_1SocketAdapter_1_1Ref) |  |

### Members

| Name | Description |
|------|-------------|
| [`SCY_HAVE_IPv6`](#group__net_1gab13f08bf90488904c8a33b55bd07d704) | Windows. |
| [`Net_API`](#group__net_1gae68e75c96e9669ffa526fadc5d953721) | Linux. |

---

#### SCY_HAVE_IPv6 

```cpp
SCY_HAVE_IPv6()
```

Windows.

---

#### Net_API 

```cpp
Net_API()
```

Linux.

## Address 

> **Defined in:** `address.h`

### Members

| Name | Description |
|------|-------------|
| [`Family`](#group__net_1gaf91e77d155f2852974bbae5a2da34106) | Possible address families for IP addresses. |
| [`Address`](#group__net_1ga4376b5c4c7a0390b9148741c61331775) | Creates a wildcard (all zero) IPv4 [Address](#classscy_1_1net_1_1Address). |
| [`Address`](#group__net_1ga9738fe42686a855ffaa0ceb14541c42c) | Creates a [Address](#classscy_1_1net_1_1Address) from an IP address and a port number. |
| [`Address`](#group__net_1ga1ebca1dc763be19d20ec69be97bbb3d7) | Creates a [Address](#classscy_1_1net_1_1Address) by copying another one. |
| [`Address`](#group__net_1gac57cc836572c64291fe739745be8e25e) | Creates a [Address](#classscy_1_1net_1_1Address) from a native socket address. |
| [`Address`](#group__net_1gacde53f645ce818600709e240dbf510eb) | Creates a [Address](#classscy_1_1net_1_1Address) from an IP address and a service name or port number. |
| [`Address`](#group__net_1ga94e2bed4c55e369c97a2d229d2b591e6) | Creates a [Address](#classscy_1_1net_1_1Address) from an IP address or host name and a port number/service name. Host name/address and port number must be separated by a colon. In case of an IPv6 address, the address part must be enclosed in brackets. |
| [`~Address`](#group__net_1ga5b9e05ed6187c478f65d0d57ad4cdb35) | Destroys the [Address](#classscy_1_1net_1_1Address). |
| [`operator=`](#group__net_1ga3b3ec108da40cf5fafd15cc21b4388bd) | Assigns another [Address](#classscy_1_1net_1_1Address). |
| [`swap`](#group__net_1ga877434e8e2b5a266d99f9f35be0882b0) | Swaps the [Address](#classscy_1_1net_1_1Address) with another one. |
| [`host`](#group__net_1gad8fbd6353a1f03ef3e0346ef1be64800) | Returns the host IP address. |
| [`port`](#group__net_1ga36e14fd72aceb8470cb7e6d2931ca622) | Returns the port number. |
| [`length`](#group__net_1ga474b01eea594bde98f3dca27ca6d09bd) | Returns the length of the internal native socket address. |
| [`addr`](#group__net_1ga6ea740a58361a1e81ed6319d6529dcf4) | Returns a pointer to the internal native socket address. |
| [`af`](#group__net_1gac1036c47a13a55df8cae09cf4ca82137) | Returns the address family (AF_INET or AF_INET6) of the address. |
| [`toString`](#group__net_1gacc785d126d40fabb5af3e6f71c835084) | Returns a string representation of the address. |
| [`family`](#group__net_1ga7683b700d696248f25f2a125a1e0f645) | Returns the address family of the host's address. |
| [`valid`](#group__net_1ga657136484ebe9fedce2ce01cb5c4497e) | Returns true when the port is set and the address is valid ie. not wildcard. |
| [`operator<`](#group__net_1ga7a1cc2425e33d40740dd5d706de9ebd6) |  |
| [`operator==`](#group__net_1gaa0f9e31b04a54f297c6b970ad8c6719f) |  |
| [`operator!=`](#group__net_1gadf21121f417c9ef8ea3ebd441e6ee72e) |  |
| [`resolveService`](#group__net_1gaef529a989e8e83bfab2cf62b87e6f024) |  |
| [`validateIP`](#group__net_1gabe7ed7441d105e3a4b878594a2d051c1) |  |
| [`init`](#group__net_1ga945edbcf0846406f0ef4e2a09dcf43cf) |  |
| [`_base`](#group__net_1gacff7149edafcdf3b5879a7211175c5b7) |  |

---

#### Family 

```cpp
enum Family
```

Possible address families for IP addresses.

| Value | Description |
|-------|-------------|
| `IPv4` |  |
| `IPv6` |  |

---

#### Address 

```cpp
Address()
```

Creates a wildcard (all zero) IPv4 [Address](#classscy_1_1net_1_1Address).

---

#### Address 

```cpp
Address(const std::string & host, uint16_t port)
```

Creates a [Address](#classscy_1_1net_1_1Address) from an IP address and a port number.

The IP address must either be a domain name, or it must be in dotted decimal (IPv4) or hex string (IPv6) format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `uint16_t` |  |

---

#### Address 

```cpp
Address(const Address & addr)
```

Creates a [Address](#classscy_1_1net_1_1Address) by copying another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const Address &` |  |

---

#### Address 

```cpp
Address(const struct sockaddr * addr, socklen_t length)
```

Creates a [Address](#classscy_1_1net_1_1Address) from a native socket address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const struct sockaddr *` |  |
| `length` | `socklen_t` |  |

---

#### Address 

```cpp
Address(const std::string & host, const std::string & port)
```

Creates a [Address](#classscy_1_1net_1_1Address) from an IP address and a service name or port number.

The IP address must either be a domain name, or it must be in dotted decimal (IPv4) or hex string (IPv6) format.

The given port must either be a decimal port number, or a service name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `const std::string &` |  |

---

#### Address 

```cpp
explicit Address(const std::string & hostAndPort)
```

Creates a [Address](#classscy_1_1net_1_1Address) from an IP address or host name and a port number/service name. Host name/address and port number must be separated by a colon. In case of an IPv6 address, the address part must be enclosed in brackets.

Examples: 192.168.1.10:80

www.sourcey.com:8080

| Parameter | Type | Description |
|-----------|------|-------------|
| `hostAndPort` | `const std::string &` |  |

---

#### ~Address 

```cpp
~Address()
```

Destroys the [Address](#classscy_1_1net_1_1Address).

---

#### operator= 

```cpp
Address & operator=(const Address & addr)
```

Assigns another [Address](#classscy_1_1net_1_1Address).

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const Address &` |  |

---

#### swap 

```cpp
void swap(Address & addr)
```

Swaps the [Address](#classscy_1_1net_1_1Address) with another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `Address &` |  |

---

#### host 

```cpp
std::string host() const
```

Returns the host IP address.

---

#### port 

```cpp
uint16_t port() const
```

Returns the port number.

---

#### length 

```cpp
socklen_t length() const
```

Returns the length of the internal native socket address.

---

#### addr 

```cpp
const struct sockaddr * addr() const
```

Returns a pointer to the internal native socket address.

---

#### af 

```cpp
int af() const
```

Returns the address family (AF_INET or AF_INET6) of the address.

---

#### toString 

```cpp
std::string toString() const
```

Returns a string representation of the address.

---

#### family 

```cpp
Address::Family family() const
```

Returns the address family of the host's address.

---

#### valid 

```cpp
bool valid() const
```

Returns true when the port is set and the address is valid ie. not wildcard.

---

#### operator< 

```cpp
bool operator<(const Address & addr) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const Address &` |  |

---

#### operator== 

```cpp
bool operator==(const Address & addr) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const Address &` |  |

---

#### operator!= 

```cpp
bool operator!=(const Address & addr) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const Address &` |  |

---

#### resolveService 

```cpp
static uint16_t resolveService(const std::string & service)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `service` | `const std::string &` |  |

---

#### validateIP 

```cpp
static bool validateIP(const std::string & address)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const std::string &` |  |

---

#### init 

```cpp
void init(const std::string & host, uint16_t port)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `uint16_t` |  |

---

#### _base 

```cpp
std::shared_ptr< AddressBase > _base
```

## PacketSocketEmitter 

> **Extends:** `scy::net::SocketEmitter`, `scy::Signal< void(IPacket &)>`
> **Subclasses:** `scy::net::Transaction< Message >`, `scy::net::Transaction< PacketT >`
> **Defined in:** `packetsocket.h`

### Members

| Name | Description |
|------|-------------|
| [`PacketSocketEmitter`](#group__net_1ga3140a2a25a6ce7db74108f3111235015) | Creates the [PacketSocketEmitter](#classscy_1_1net_1_1PacketSocketEmitter) This class should have a higher priority than standard sockets so we can parse data packets first. Creates and dispatches a packet utilizing the available creation strategies. For best performance the most used strategies should have the highest priority. |
| [`~PacketSocketEmitter`](#group__net_1ga9d770233ff68edeace9e64b262a61abc) |  |
| [`onSocketRecv`](#group__net_1gadc517059efc11e7b96c9aac48d3621f4) |  |
| [`onPacket`](#group__net_1ga0de3859d43f868dbbdb88fda6e713092) |  |
| [`factory`](#group__net_1gaed6e38fcedd99dba4ad9cb4aee59b071) | The packet factory. |

---

#### PacketSocketEmitter 

```cpp
PacketSocketEmitter(const Socket::Ptr & socket)
```

Creates the [PacketSocketEmitter](#classscy_1_1net_1_1PacketSocketEmitter) This class should have a higher priority than standard sockets so we can parse data packets first. Creates and dispatches a packet utilizing the available creation strategies. For best performance the most used strategies should have the highest priority.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const Socket::Ptr &` |  |

---

#### ~PacketSocketEmitter 

```cpp
virtual ~PacketSocketEmitter()
```

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv(Socket & socket, const MutableBuffer & buffer, const Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const Address &` |  |

---

#### onPacket 

```cpp
virtual void onPacket(IPacket & pkt)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pkt` | `IPacket &` |  |

---

#### factory 

```cpp
PacketFactory factory
```

The packet factory.

## Socket 

> **Extends:** `scy::net::SocketAdapter`
> **Subclasses:** `scy::net::TCPSocket`, `scy::net::UDPSocket`
> **Defined in:** `socket.h`

Base socket implementation from which all sockets derive.

### Members

| Name | Description |
|------|-------------|
| [`Socket`](#group__net_1ga0d1fd3c6568df4d23baf8bac3443b031) |  |
| [`~Socket`](#group__net_1ga6d1ec3118de86711205b220fae9228c5) |  |
| [`connect`](#group__net_1ga26b4c04fb14fb521c3777966f5500814) | Connects to the given peer IP address. |
| [`connect`](#group__net_1gad8ed2e347065221848364977f3f4b0ad) | Resolves and connects to the given host address. |
| [`bind`](#group__net_1gad12fbab04fa447ab0104325d7062af03) | Bind a local address to the socket. The address may be IPv4 or IPv6 (if supported). |
| [`listen`](#group__net_1ga3f113f66f6dcbb1fc2a2c25a86833eaa) | Listens the socket on the given address. |
| [`shutdown`](#group__net_1ga68cca230e61d94ddcb530c8236b8b65a) | Sends the shutdown packet which should result is socket closure via callback. |
| [`close`](#group__net_1gaef3e9d763d40129224c70087c0b8660b) | Closes the underlying socket. |
| [`address`](#group__net_1gadb57bf71ea7475434e304193495a22f8) | The locally bound address. |
| [`peerAddress`](#group__net_1gaa7aad8f6cdfc70fd2d993cc99e65f73c) | The connected peer address. |
| [`transport`](#group__net_1ga1e1b0fbbf92bb3dd962783a8c0e94b3d) | The transport protocol: TCP, UDP or SSLTCP. |
| [`setError`](#group__net_1gaed1d2e8ca6eb5140528c044b675299b4) | Sets the socket error. |
| [`error`](#group__net_1gad2a2feb9ea2e6069d6f7b283b44b2e34) | Return the socket error if any. |
| [`closed`](#group__net_1ga9bf8cf75403f7f833c37ed7fb16443a6) | Returns true if the native socket handle is closed. |
| [`loop`](#group__net_1ga1c0302f62375661291b891972a178b09) | Returns the socket event loop. |
| [`init`](#group__net_1ga0b9b76689ad2ca6a398e909a5424c24b) | Initializes the underlying socket context. |
| [`reset`](#group__net_1ga07e4b137f7019e83ce21fcd2441ddc9b) | Resets the socket context for reuse. |
| [`opaque`](#group__net_1ga1379e3c59f9934f2b2668a5014ab6d6c) | Optional client data pointer. |
| [`_af`](#group__net_1gae27f31658bb7c71f26afd113b066465a) |  |

---

#### Socket 

```cpp
Socket() = default
```

---

#### ~Socket 

```cpp
virtual ~Socket() = default
```

---

#### connect 

```cpp
void connect(const Address & address)
```

Connects to the given peer IP address.

Throws an exception if the address is malformed. Connection errors can be handled via the [Error](./doc/api-base.md#structscy_1_1Error) signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const Address &` |  |

---

#### connect 

```cpp
void connect(const std::string & host, uint16_t port)
```

Resolves and connects to the given host address.

Throws an Exception if the host is malformed. Since the DNS callback is asynchronous implementations need to listen for the [Error](./doc/api-base.md#structscy_1_1Error) signal for handling connection errors.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `uint16_t` |  |

---

#### bind 

```cpp
void bind(const Address & address, unsigned flags)
```

Bind a local address to the socket. The address may be IPv4 or IPv6 (if supported).

Throws an Exception on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const Address &` |  |
| `flags` | `unsigned` |  |

---

#### listen 

```cpp
virtual inline void listen(int backlog)
```

Listens the socket on the given address.

Throws an Exception on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `backlog` | `int` |  |

---

#### shutdown 

```cpp
virtual inline bool shutdown()
```

Sends the shutdown packet which should result is socket closure via callback.

---

#### close 

```cpp
void close()
```

Closes the underlying socket.

---

#### address 

```cpp
Address address() const
```

The locally bound address.

This function will not throw. A Wildcard 0.0.0.0:0 address is returned if the socket is closed or invalid.

---

#### peerAddress 

```cpp
Address peerAddress() const
```

The connected peer address.

This function will not throw. A Wildcard 0.0.0.0:0 address is returned if the socket is closed or invalid.

---

#### transport 

```cpp
net::TransportType transport() const
```

The transport protocol: TCP, UDP or SSLTCP.

---

#### setError 

```cpp
void setError(const [scy::Error](./doc/api-base.md#structscy_1_1Error) & err)
```

Sets the socket error.

Setting the error will result in socket closure.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [scy::Error](./doc/api-base.md#structscy_1_1Error) &` |  |

---

#### error 

```cpp
const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error() const
```

Return the socket error if any.

---

#### closed 

```cpp
bool closed() const
```

Returns true if the native socket handle is closed.

---

#### loop 

```cpp
uv::Loop * loop() const
```

Returns the socket event loop.

---

#### init 

```cpp
void init()
```

Initializes the underlying socket context.

---

#### reset 

```cpp
void reset()
```

Resets the socket context for reuse.

---

#### opaque 

```cpp
void * opaque { nullptr }
```

Optional client data pointer.

The pointer is set to null on initialization but not managed.

---

#### _af 

```cpp
int _af { AF_UNSPEC }
```

## SocketPacket 

> **Extends:** `scy::RawPacket`
> **Defined in:** `socket.h`

[SocketPacket](#classscy_1_1net_1_1SocketPacket) is the default packet type emitted by sockets. [SocketPacket](#classscy_1_1net_1_1SocketPacket) provides peer address information and a buffer reference for nocopy binary operations.

The referenced packet buffer lifetime is only guaranteed for the duration of the receiver callback.

### Members

| Name | Description |
|------|-------------|
| [`SocketPacket`](#group__net_1gaec154054d8c54269655aa062d8fe1ffd) |  |
| [`SocketPacket`](#group__net_1gad327456520e77574cd3cb730ccc0a75a) |  |
| [`packetInfo`](#group__net_1gaf192ea8a4488fdb7ff04961999496954) | Returns the [PacketInfo](#structscy_1_1net_1_1PacketInfo) for this socket packet. |
| [`~SocketPacket`](#group__net_1ga3e883a20f382b845e90c1047e8ff03f1) |  |
| [`print`](#group__net_1ga319694029ed98f048cdaf68330e31807) |  |
| [`clone`](#group__net_1gad4dc225d63a642fb0eb1b43c271c796c) |  |
| [`read`](#group__net_1gae771cf7b5b4a141bb898acb1cafddc62) | Read/parse to the packet from the given input buffer. The number of bytes read is returned. |
| [`write`](#group__net_1gaac943003d23e0dbbedbf120a45ab3078) | Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer. |
| [`className`](#group__net_1ga69704ee8a377cbcd6e06ec3d44d93a24) |  |

---

#### SocketPacket 

```cpp
inline SocketPacket(const Socket::Ptr & socket, const MutableBuffer & buffer, const Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const Socket::Ptr &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const Address &` |  |

---

#### SocketPacket 

```cpp
inline SocketPacket(const SocketPacket & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const SocketPacket &` |  |

---

#### packetInfo 

```cpp
inline PacketInfo * packetInfo() const
```

Returns the [PacketInfo](#structscy_1_1net_1_1PacketInfo) for this socket packet.

---

#### ~SocketPacket 

```cpp
virtual inline ~SocketPacket()
```

---

#### print 

```cpp
virtual inline void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### read 

```cpp
virtual inline ssize_t read(const ConstBuffer &)
```

Read/parse to the packet from the given input buffer. The number of bytes read is returned.

---

#### write 

```cpp
virtual inline void write(Buffer &) const
```

Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer.

Todo: It may be prefferable to use our pod types here instead of buffer input, but the current codebase requires that the buffer be dynamically resizable for some protocols...

---

#### className 

```cpp
virtual inline const char * className() const
```

## SocketAdapter 

> **Subclasses:** `scy::http::Connection`, `scy::http::ConnectionAdapter`, `scy::http::ConnectionStream`, `scy::http::Server`, `scy::net::Socket`, `scy::net::SocketEmitter`, `scy::sockio::Client`
> **Defined in:** `socketadapter.h`

[SocketAdapter](#classscy_1_1net_1_1SocketAdapter) is the abstract interface for all socket classes. A [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) can also be attached to a [Socket](#classscy_1_1net_1_1Socket) in order to override default [Socket](#classscy_1_1net_1_1Socket) callbacks and behaviour, while still maintaining the default [Socket](#classscy_1_1net_1_1Socket) interface (see Socket::setAdapter).

This class also be extended to implement custom processing for received socket data before it is dispatched to the application (see [PacketSocketEmitter](#classscy_1_1net_1_1PacketSocketEmitter) and [Transaction](#classscy_1_1net_1_1Transaction) classes).

### Members

| Name | Description |
|------|-------------|
| [`SocketAdapter`](#group__net_1ga35ca78ac04abbd6828af02fb1b65e6d1) | Creates the [SocketAdapter](#classscy_1_1net_1_1SocketAdapter). |
| [`~SocketAdapter`](#group__net_1gac06480990b5301cb185000fe8bf860e4) | Destroys the [SocketAdapter](#classscy_1_1net_1_1SocketAdapter). |
| [`send`](#group__net_1gae0701649bbdbe02ac987c21dbf180745) | Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address. |
| [`send`](#group__net_1ga4d695c349c6c483b36bc2a406ffa8554) |  |
| [`sendPacket`](#group__net_1ga51b07fc133c1761959118191f392509e) | Sends the given packet to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address. |
| [`sendPacket`](#group__net_1ga7ef73b7c3d563756f492e9abc196c77d) |  |
| [`sendPacket`](#group__net_1ga6d7c711236a3570235353f634f9d1a77) | Sends the given packet to the connected peer. This method provides delegate compatability, and unlike other send methods throws an exception if the underlying socket is closed. |
| [`setSender`](#group__net_1ga566fd551c24638ab22d42f6c6082d85f) | Sets the pointer to the outgoing data adapter. Send methods proxy data to this adapter by default. |
| [`sender`](#group__net_1gaf327b8e4712403322638ea10151dc1d6) | Returns the output [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) pointer. |
| [`addReceiver`](#group__net_1ga255f77677b0e9b2d95b663c413f16fee) | Sets the pointer to the incoming data adapter. Events proxy data to this adapter by default. |
| [`removeReceiver`](#group__net_1ga523f50c94ac42d49d81efb8a4ff81629) | Remove the given receiver. |
| [`hasReceiver`](#group__net_1ga083660a332c15c43e6a1945d59b3f0d7) | Returns true if the given receiver is connected. |
| [`receivers`](#group__net_1ga1e96c66f9d52faadb197b59c75cefb67) | Returns the input [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) pointer. |
| [`onSocketConnect`](#group__net_1ga8dae1c179274b0571d1b5d8040f5afd8) | These virtual methods can be overridden as necessary to intercept socket events before they hit the application. |
| [`onSocketRecv`](#group__net_1ga48f8129547ba9eaac88094e059205529) |  |
| [`onSocketError`](#group__net_1ga5658d5b01a3ff4cbad6010190aefe3b7) |  |
| [`onSocketClose`](#group__net_1gab5d5c13b628ba6a010785f447f9e4fdc) |  |
| [`cleanupReceivers`](#group__net_1gacd031591dc8a642107aeb1f16ad55764) |  |
| [`priority`](#group__net_1ga66462899a27420bf50af004887355e83) | The priority of this adapter for STL sort operations. |
| [`_sender`](#group__net_1ga17dc4823b85f24d94b5127681006439f) |  |
| [`_receivers`](#group__net_1ga16be0b8461e4aeb9bc6fa0acf0447288) |  |
| [`_dirty`](#group__net_1ga69b4294f637aa586d0e247e5c4b8b2a9) |  |

---

#### SocketAdapter 

```cpp
SocketAdapter(SocketAdapter * sender)
```

Creates the [SocketAdapter](#classscy_1_1net_1_1SocketAdapter).

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `SocketAdapter *` |  |

---

#### ~SocketAdapter 

```cpp
virtual ~SocketAdapter()
```

Destroys the [SocketAdapter](#classscy_1_1net_1_1SocketAdapter).

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, const Address & peerAddress, int flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `peerAddress` | `const Address &` |  |
| `flags` | `int` |  |

---

#### sendPacket 

```cpp
virtual ssize_t sendPacket(const IPacket & packet, int flags)
```

Sends the given packet to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `const IPacket &` |  |
| `flags` | `int` |  |

---

#### sendPacket 

```cpp
virtual ssize_t sendPacket(const IPacket & packet, const Address & peerAddress, int flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `const IPacket &` |  |
| `peerAddress` | `const Address &` |  |
| `flags` | `int` |  |

---

#### sendPacket 

```cpp
virtual void sendPacket(IPacket & packet)
```

Sends the given packet to the connected peer. This method provides delegate compatability, and unlike other send methods throws an exception if the underlying socket is closed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### setSender 

```cpp
virtual void setSender(SocketAdapter * adapter)
```

Sets the pointer to the outgoing data adapter. Send methods proxy data to this adapter by default.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `SocketAdapter *` |  |

---

#### sender 

```cpp
SocketAdapter * sender()
```

Returns the output [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) pointer.

---

#### addReceiver 

```cpp
virtual void addReceiver(SocketAdapter * adapter)
```

Sets the pointer to the incoming data adapter. Events proxy data to this adapter by default.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `SocketAdapter *` |  |

---

#### removeReceiver 

```cpp
virtual void removeReceiver(SocketAdapter * adapter)
```

Remove the given receiver.

By default this function does nothing unless the given receiver matches the current receiver.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `SocketAdapter *` |  |

---

#### hasReceiver 

```cpp
virtual bool hasReceiver(SocketAdapter * adapter)
```

Returns true if the given receiver is connected.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `SocketAdapter *` |  |

---

#### receivers 

```cpp
std::vector< SocketAdapter * > receivers()
```

Returns the input [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) pointer.

---

#### onSocketConnect 

```cpp
virtual void onSocketConnect(Socket & socket)
```

These virtual methods can be overridden as necessary to intercept socket events before they hit the application.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv(Socket & socket, const MutableBuffer & buffer, const Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const Address &` |  |

---

#### onSocketError 

```cpp
virtual void onSocketError(Socket & socket, const Error & error)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |
| `error` | `const Error &` |  |

---

#### onSocketClose 

```cpp
virtual void onSocketClose(Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |

---

#### cleanupReceivers 

```cpp
virtual void cleanupReceivers()
```

---

#### priority 

```cpp
int priority = 0
```

The priority of this adapter for STL sort operations.

---

#### _sender 

```cpp
SocketAdapter * _sender
```

---

#### _receivers 

```cpp
std::vector< Ref::ptr_t > _receivers
```

---

#### _dirty 

```cpp
bool _dirty = false
```

## Ref 

> **Defined in:** `socketadapter.h`

### Members

| Name | Description |
|------|-------------|
| [`ptr`](#group__net_1ga7850ea3acb3330d7fb974c45f780f1c4) |  |
| [`alive`](#group__net_1ga5e0d22008a914828d87ecd623872db05) |  |

---

#### ptr 

```cpp
SocketAdapter * ptr
```

---

#### alive 

```cpp
bool alive
```

## SocketEmitter 

> **Extends:** `scy::net::SocketAdapter`
> **Subclasses:** `scy::http::ws::WebSocketAdapter`, `scy::net::PacketSocketEmitter`
> **Defined in:** `socketemitter.h`

[SocketAdapter](#classscy_1_1net_1_1SocketAdapter) class that adds signal callbacks for sockets. Asde from adding a signal interface the class works as a wrapper for the socket instance, and is designed to be used the same way as a `std::unique_ptr` by overriding the ->() operator.

### Members

| Name | Description |
|------|-------------|
| [`SocketEmitter`](#group__net_1ga6a57fd61fe37d7d7aae29fb85d708d0f) | Creates the [SocketEmitter](#classscy_1_1net_1_1SocketEmitter). |
| [`SocketEmitter`](#group__net_1gac0c6fd442346109bae43fd86e490268a) | Copy constructor. |
| [`~SocketEmitter`](#group__net_1ga371482dd5151aea7cb88803946a09144) | Destroys the [SocketAdapter](#classscy_1_1net_1_1SocketAdapter). |
| [`addReceiver`](#group__net_1ga33c730a0439c41d71c1bd673320f71bf) | Adds an input [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) for receiving socket signals. |
| [`removeReceiver`](#group__net_1gaa962167a885ba5f15a3420771f861204) | Removes an input [SocketAdapter](#classscy_1_1net_1_1SocketAdapter). |
| [`swap`](#group__net_1gae5c17082651ccf5847bbec840fa89443) | Swap the underlying socket pointer. |
| [`as`](#group__net_1ga9a944688f65c76fa9fda90d4927e2b3d) | Cast getter for the underlying socket. |
| [`operator->`](#group__net_1gaa66861023d067035a62030280a7c36e6) | Accessor to the underlying socket. |
| [`onSocketConnect`](#group__net_1gaf3ef9945319491216f42833e5791a678) | Internal callback events. |
| [`onSocketRecv`](#group__net_1ga9157f82d8c9d2d849aa173f06a143ddd) |  |
| [`onSocketError`](#group__net_1gaea46a3b9af1d0bed4bd708c1e5f59cbe) |  |
| [`onSocketClose`](#group__net_1ga8b6d7dcdd05f2f7a21f19733239f083e) |  |
| [`Connect`](#group__net_1gaf6b9705a3bd3faf2bfb550dbc7c5249e) | Signals that the socket is connected. |
| [`Recv`](#group__net_1ga6a2406c9364edbff61e1d559e5b6576c) | Signals when data is received by the socket. |
| [`Error`](#group__net_1gaa0ff0673de03635fc0ea26c2be3c2daa) | Signals that the socket is closed in error. This signal will be sent just before the Closed signal. |
| [`Close`](#group__net_1ga950604a2babd224d2b2c7b1f5e00d101) | Signals that the underlying socket is closed. |
| [`impl`](#group__net_1ga0b423a8b6597057e39dcd3dcb0222364) | Pointer to the underlying socket. Sent data will be proxied to this socket. |

---

#### SocketEmitter 

```cpp
SocketEmitter(const Socket::Ptr & socket)
```

Creates the [SocketEmitter](#classscy_1_1net_1_1SocketEmitter).

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const Socket::Ptr &` |  |

---

#### SocketEmitter 

```cpp
SocketEmitter(const SocketEmitter & that)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const SocketEmitter &` |  |

---

#### ~SocketEmitter 

```cpp
virtual ~SocketEmitter()
```

Destroys the [SocketAdapter](#classscy_1_1net_1_1SocketAdapter).

---

#### addReceiver 

```cpp
virtual void addReceiver(SocketAdapter * adapter)
```

Adds an input [SocketAdapter](#classscy_1_1net_1_1SocketAdapter) for receiving socket signals.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `SocketAdapter *` |  |

---

#### removeReceiver 

```cpp
virtual void removeReceiver(SocketAdapter * adapter)
```

Removes an input [SocketAdapter](#classscy_1_1net_1_1SocketAdapter).

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `SocketAdapter *` |  |

---

#### swap 

```cpp
virtual void swap(const Socket::Ptr & socket)
```

Swap the underlying socket pointer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const Socket::Ptr &` |  |

---

#### as 

```cpp
template<class T> inline T * as()
```

Cast getter for the underlying socket.

---

#### operator-> 

```cpp
inline Socket * operator->() const
```

Accessor to the underlying socket.

---

#### onSocketConnect 

```cpp
virtual void onSocketConnect(Socket & socket)
```

Internal callback events.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv(Socket & socket, const MutableBuffer & buffer, const Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |
| `buffer` | `const MutableBuffer &` |  |
| `peerAddress` | `const Address &` |  |

---

#### onSocketError 

```cpp
virtual void onSocketError(Socket & socket, const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |
| `error` | `const [scy::Error](./doc/api-base.md#structscy_1_1Error) &` |  |

---

#### onSocketClose 

```cpp
virtual void onSocketClose(Socket & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `Socket &` |  |

---

#### Connect 

```cpp
Signal< void(Socket &)> Connect
```

Signals that the socket is connected.

---

#### Recv 

```cpp
Signal< void(Socket &, const MutableBuffer &, const Address &)> Recv
```

Signals when data is received by the socket.

---

#### Error 

```cpp
Signal< void(Socket &, const [scy::Error](./doc/api-base.md#structscy_1_1Error) &)> Error
```

Signals that the socket is closed in error. This signal will be sent just before the Closed signal.

---

#### Close 

```cpp
Signal< void(Socket &)> Close
```

Signals that the underlying socket is closed.

---

#### impl 

```cpp
Socket::Ptr impl
```

Pointer to the underlying socket. Sent data will be proxied to this socket.

## SSLAdapter 

> **Defined in:** `ssladapter.h`

### Members

| Name | Description |
|------|-------------|
| [`SSLAdapter`](#group__net_1ga26cc38aebf1d0d1846ae6603ba2be64f) |  |
| [`~SSLAdapter`](#group__net_1ga4639bd49bcad7899337f22ba0ec68084) |  |
| [`initClient`](#group__net_1gaebf891ed98b4c4f2c5edeccfd98cef03) | Initializes the SSL context as a client. |
| [`initServer`](#group__net_1ga707dd2a10f4541d9a38205083a60c08f) | Initializes the SSL context as a server. |
| [`initialized`](#group__net_1ga3051dc8e9a424368aeb91027d4b9d8be) | Returns true when SSL context has been initialized. |
| [`ready`](#group__net_1ga05e325d251acd31625116021c4abcf4a) | Returns true when the handshake is complete. |
| [`handshake`](#group__net_1ga16c72dfccd9cc787ca4d570179de7bb9) | Start/continue the SSL handshake process. |
| [`available`](#group__net_1gaebf583d1844bb5eb37b6c7d94a7cdca1) | Returns the number of bytes available in the SSL buffer for immediate reading. |
| [`shutdown`](#group__net_1gaa2a689ba83da2bcc9d3b8b4b3bab303e) | Issues an orderly SSL shutdown. |
| [`flush`](#group__net_1gabf462204b841bac2edbc2d9c12d36d59) | Flushes the SSL read/write buffers. |
| [`addIncomingData`](#group__net_1ga17018e5db975debd3765e5611a6c5c66) |  |
| [`addOutgoingData`](#group__net_1ga80a755a8a5a1369ace2d8f95740767f7) |  |
| [`addOutgoingData`](#group__net_1ga9d0f1cb4beaf061be33cb162ad674571) |  |
| [`handleError`](#group__net_1ga37281519a34b937cbf763e587e5675c4) |  |
| [`flushReadBIO`](#group__net_1ga8c32288c3e5badc7985dcc0f58ac8336) |  |
| [`flushWriteBIO`](#group__net_1ga5949d6d6339de68d4d38f1b4b07e3360) |  |
| [`_socket`](#group__net_1gae207495c52da9720625af1c1a8234ca0) |  |
| [`_ssl`](#group__net_1gab767ae04b5ac979a1e2d0d337b4c4028) |  |
| [`_readBIO`](#group__net_1ga0a542283ee6c57fcc584a1315cd0b9a7) | The incoming buffer we write encrypted SSL data into. |
| [`_writeBIO`](#group__net_1ga7837fb89f6f46ab90a408290f308ee2a) | The outgoing buffer we write to the socket. |
| [`_bufferOut`](#group__net_1ga3c2e88ec5aa5631dedaf5179bf8dbfa0) | The outgoing payload to be encrypted and sent. |

---

#### SSLAdapter 

```cpp
SSLAdapter(net::SSLSocket * socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `net::SSLSocket *` |  |

---

#### ~SSLAdapter 

```cpp
~SSLAdapter()
```

---

#### initClient 

```cpp
void initClient()
```

Initializes the SSL context as a client.

---

#### initServer 

```cpp
void initServer()
```

Initializes the SSL context as a server.

---

#### initialized 

```cpp
bool initialized() const
```

Returns true when SSL context has been initialized.

---

#### ready 

```cpp
bool ready() const
```

Returns true when the handshake is complete.

---

#### handshake 

```cpp
void handshake()
```

Start/continue the SSL handshake process.

---

#### available 

```cpp
int available() const
```

Returns the number of bytes available in the SSL buffer for immediate reading.

---

#### shutdown 

```cpp
void shutdown()
```

Issues an orderly SSL shutdown.

---

#### flush 

```cpp
void flush()
```

Flushes the SSL read/write buffers.

---

#### addIncomingData 

```cpp
void addIncomingData(const char * data, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### addOutgoingData 

```cpp
void addOutgoingData(const std::string & data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const std::string &` |  |

---

#### addOutgoingData 

```cpp
void addOutgoingData(const char * data, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### handleError 

```cpp
void handleError(int rc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `rc` | `int` |  |

---

#### flushReadBIO 

```cpp
void flushReadBIO()
```

---

#### flushWriteBIO 

```cpp
void flushWriteBIO()
```

---

#### _socket 

```cpp
net::SSLSocket * _socket
```

---

#### _ssl 

```cpp
SSL * _ssl
```

---

#### _readBIO 

```cpp
BIO * _readBIO
```

The incoming buffer we write encrypted SSL data into.

---

#### _writeBIO 

```cpp
BIO * _writeBIO
```

The outgoing buffer we write to the socket.

---

#### _bufferOut 

```cpp
std::vector< char > _bufferOut
```

The outgoing payload to be encrypted and sent.

## SSLContext 

> **Defined in:** `sslcontext.h`

This class encapsulates context information for an SSL server or client, such as the certificate verification mode and the location of certificates and private key files, as well as the list of supported ciphers.

The Context class is also used to control SSL session caching on the server and client side.

### Members

| Name | Description |
|------|-------------|
| [`Usage`](#group__net_1ga2e13fff7d4c75c080e515a427bf15c71) |  |
| [`VerificationMode`](#group__net_1gafd7da63f7077511ce8d4da68f37d255e) |  |
| [`SSLContext`](#group__net_1ga9bc5bae0a271799c171b37d3e6e13735) | Creates a Context. |
| [`SSLContext`](#group__net_1ga9fe3fe5f358b95a71a49a42798ec36ce) | Creates a Context. |
| [`~SSLContext`](#group__net_1gaf88baeb14fd0ee8d539e960f81142043) | Destroys the Context. |
| [`useCertificate`](#group__net_1ga840711ee078f8ee6432aaef8b50b48b9) | Sets the certificate to be used by the Context. |
| [`addChainCertificate`](#group__net_1gac4fe7dff44de813dcdf7daf80ea48d5f) | Adds a certificate for certificate chain validation. |
| [`addVerificationCertificate`](#group__net_1ga553376bb332952ef5c1829fb3600d554) | Sets the private key to be used by the Context. |
| [`sslContext`](#group__net_1ga88c101c4f8f5b4fe3ff3a4e50ff7ccfd) | Returns the underlying OpenSSL SSL Context object. |
| [`usage`](#group__net_1ga9b4562b3b554ae7b584a1cb2ab3825df) | Returns whether the context is for use by a client or by a server and whether TLSv1 is required. |
| [`isForServerUse`](#group__net_1ga5b018180ca35ce83c8d5efa9a33b3d35) | Returns true if the context is for use by a server. |
| [`verificationMode`](#group__net_1ga134311a6874bd856a4ad24fb7605784f) | Returns the verification mode. |
| [`enableSessionCache`](#group__net_1gabcb6479878cbc88959c819b4315d9d5b) | Enable or disable SSL/TLS session caching. For session caching to work, it must be enabled on the server, as well as on the client side. |
| [`enableSessionCache`](#group__net_1gafc6d7cc2fd93fb8e85f793e00b5d2926) | Enables or disables SSL/TLS session caching on the server. For session caching to work, it must be enabled on the server, as well as on the client side. |
| [`sessionCacheEnabled`](#group__net_1ga660b36fa2986687c39716fbf170d5284) | Returns true if the session cache is enabled. |
| [`setSessionCacheSize`](#group__net_1ga866c6d92313d85c8c580e742a722964b) | Sets the maximum size of the server session cache, in number of sessions. The default size (according to OpenSSL documentation) is 1024*20, which may be too large for many applications, especially on embedded platforms with limited memory. |
| [`getSessionCacheSize`](#group__net_1ga57776ff4697488efe841f32fd6334c0e) | Returns the current maximum size of the server session cache. |
| [`setSessionTimeout`](#group__net_1ga4f769feef9c5e604a5f66121e3618e14) | Sets the timeout (in seconds) of cached sessions on the server. A cached session will be removed from the cache if it has not been used for the given number of seconds. |
| [`getSessionTimeout`](#group__net_1ga00d5531a1264b6dbb8079779ba864eca) | Returns the timeout (in seconds) of cached sessions on the server. |
| [`flushSessionCache`](#group__net_1ga9d5e8a57a06cdaa54a8c5782785848c8) | Flushes the SSL session cache on the server. |
| [`disableStatelessSessionResumption`](#group__net_1gae16a7eff1253b2924e5254066e151c6b) | Newer versions of OpenSSL support RFC 4507 tickets for stateless session resumption. |
| [`createSSLContext`](#group__net_1ga93f1f41e4374901ebba9a13a07e6d851) | Create a SSL_CTX object according to Context configuration. |
| [`_usage`](#group__net_1ga171ace8794c0f1b9ff4ceb91ec887cd1) |  |
| [`_mode`](#group__net_1ga9afa80a0f6ca11b832a906cb95180399) |  |
| [`_sslContext`](#group__net_1ga7b1415c11d5ba729dfd704a0ea7183b6) |  |
| [`_extendedVerificationErrorDetails`](#group__net_1gaecb958e393d9723c5fad530d024f1c51) |  |

---

#### Usage 

```cpp
enum Usage
```

| Value | Description |
|-------|-------------|
| `CLIENT_USE` | Context is used by a client. |
| `SERVER_USE` | Context is used by a server. |
| `TLSV1_CLIENT_USE` | Context is used by a client requiring TLSv1. |
| `TLSV1_SERVER_USE` | Context is used by a server requiring TLSv2. |

---

#### VerificationMode 

```cpp
enum VerificationMode
```

| Value | Description |
|-------|-------------|
| `VERIFY_NONE` | Server: The server will not send a client certificate request to the client, so the client will not send a certificate. |
| `VERIFY_RELAXED` | Server: The server sends a client certificate request to the client. The certificate returned (if any) is checked. If the verification process fails, the TLS/SSL handshake is immediately terminated with an alert message containing the reason for the verification failure. |
| `VERIFY_STRICT` | Server: If the client did not return a certificate, the TLS/SSL handshake is immediately terminated with a handshake failure alert. |
| `VERIFY_ONCE` | Server: Only request a client certificate on the initial TLS/SSL handshake. Do not ask for a client certificate again in case of a renegotiation. |

---

#### SSLContext 

```cpp
SSLContext(Usage usage, const std::string & privateKeyFile, const std::string & certificateFile, const std::string & caLocation, VerificationMode verificationMode, int verificationDepth, bool loadDefaultCAs, const std::string & cipherList)
```

Creates a Context.

* usage specifies whether the context is used by a client or server.

* privateKeyFile contains the path to the private key file used for encryption. Can be empty if no private key file is used.

* certificateFile contains the path to the certificate file (in PEM format). If the private key and the certificate are stored in the same file, this can be empty if privateKeyFile is given.

* caLocation contains the path to the file or directory containing the CA/root certificates. Can be empty if the OpenSSL builtin CA certificates are used (see loadDefaultCAs).

* verificationMode specifies whether and how peer certificates are validated.

* verificationDepth sets the upper limit for verification chain sizes. Verification will fail if a certificate chain larger than this is encountered.

* loadDefaultCAs specifies wheter the builtin CA certificates from OpenSSL are used.

* cipherList specifies the supported ciphers in OpenSSL notation.

Note: If the private key is protected by a passphrase, a PrivateKeyPassphraseHandler must have been setup with the [SSLManager](#classscy_1_1net_1_1SSLManager), or the [SSLManager](#classscy_1_1net_1_1SSLManager)'s PrivateKeyPassphraseRequired event must be handled.

| Parameter | Type | Description |
|-----------|------|-------------|
| `usage` | `Usage` |  |
| `privateKeyFile` | `const std::string &` |  |
| `certificateFile` | `const std::string &` |  |
| `caLocation` | `const std::string &` |  |
| `verificationMode` | `VerificationMode` |  |
| `verificationDepth` | `int` |  |
| `loadDefaultCAs` | `bool` |  |
| `cipherList` | `const std::string &` |  |

---

#### SSLContext 

```cpp
SSLContext(Usage usage, const std::string & caLocation, VerificationMode verificationMode, int verificationDepth, bool loadDefaultCAs, const std::string & cipherList)
```

Creates a Context.

* usage specifies whether the context is used by a client or server.

* caLocation contains the path to the file or directory containing the CA/root certificates. Can be empty if the OpenSSL builtin CA certificates are used (see loadDefaultCAs).

* verificationMode specifies whether and how peer certificates are validated.

* verificationDepth sets the upper limit for verification chain sizes. Verification will fail if a certificate chain larger than this is encountered.

* loadDefaultCAs specifies weather the builtin CA certificates from OpenSSL are used.

* cipherList specifies the supported ciphers in OpenSSL notation.

Note that a private key and/or certificate must be specified with usePrivateKey()/useCertificate() before the Context can be used.

| Parameter | Type | Description |
|-----------|------|-------------|
| `usage` | `Usage` |  |
| `caLocation` | `const std::string &` |  |
| `verificationMode` | `VerificationMode` |  |
| `verificationDepth` | `int` |  |
| `loadDefaultCAs` | `bool` |  |
| `cipherList` | `const std::string &` |  |

---

#### ~SSLContext 

```cpp
~SSLContext()
```

Destroys the Context.

---

#### useCertificate 

```cpp
void useCertificate(const crypto::X509Certificate & certificate)
```

Sets the certificate to be used by the Context.

To set-up a complete certificate chain, it might be necessary to call [addChainCertificate()](#group__net_1gac4fe7dff44de813dcdf7daf80ea48d5f) to specify additional certificates.

Note that [useCertificate()](#group__net_1ga840711ee078f8ee6432aaef8b50b48b9) must always be called before usePrivateKey().

| Parameter | Type | Description |
|-----------|------|-------------|
| `certificate` | `const crypto::X509Certificate &` |  |

---

#### addChainCertificate 

```cpp
void addChainCertificate(const crypto::X509Certificate & certificate)
```

Adds a certificate for certificate chain validation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `certificate` | `const crypto::X509Certificate &` |  |

---

#### addVerificationCertificate 

```cpp
void addVerificationCertificate(const crypto::X509Certificate & certificate)
```

Sets the private key to be used by the Context.

Note that [useCertificate()](#group__net_1ga840711ee078f8ee6432aaef8b50b48b9) must always be called before usePrivateKey().

Note: If the private key is protected by a passphrase, a PrivateKeyPassphraseHandler must have been setup with the [SSLManager](#classscy_1_1net_1_1SSLManager), or the [SSLManager](#classscy_1_1net_1_1SSLManager)'s PrivateKeyPassphraseRequired event must be handled. Adds the given certificate to the list of trusted certificates that will be used for verification.

| Parameter | Type | Description |
|-----------|------|-------------|
| `certificate` | `const crypto::X509Certificate &` |  |

---

#### sslContext 

```cpp
inline SSL_CTX * sslContext() const
```

Returns the underlying OpenSSL SSL Context object.

---

#### usage 

```cpp
inline Usage usage() const
```

Returns whether the context is for use by a client or by a server and whether TLSv1 is required.

---

#### isForServerUse 

```cpp
inline bool isForServerUse() const
```

Returns true if the context is for use by a server.

---

#### verificationMode 

```cpp
inline SSLContext::VerificationMode verificationMode() const
```

Returns the verification mode.

---

#### enableSessionCache 

```cpp
void enableSessionCache(bool flag)
```

Enable or disable SSL/TLS session caching. For session caching to work, it must be enabled on the server, as well as on the client side.

The default is disabled session caching.

To enable session caching on the server side, use the two-argument version of this method to specify a session ID context.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### enableSessionCache 

```cpp
void enableSessionCache(bool flag, const std::string & sessionIdContext)
```

Enables or disables SSL/TLS session caching on the server. For session caching to work, it must be enabled on the server, as well as on the client side.

SessionIdContext contains the application's unique session ID context, which becomes part of each session identifier generated by the server within this context. SessionIdContext can be an arbitrary sequence of bytes with a maximum length of SSL_MAX_SSL_SESSION_ID_LENGTH.

A non-empty sessionIdContext should be specified even if session caching is disabled to avoid problems with clients requesting to reuse a session (e.g. Firefox 3.6).

This method may only be called on SERVER_USE Context objects.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |
| `sessionIdContext` | `const std::string &` |  |

---

#### sessionCacheEnabled 

```cpp
bool sessionCacheEnabled() const
```

Returns true if the session cache is enabled.

---

#### setSessionCacheSize 

```cpp
void setSessionCacheSize(size_t size)
```

Sets the maximum size of the server session cache, in number of sessions. The default size (according to OpenSSL documentation) is 1024*20, which may be too large for many applications, especially on embedded platforms with limited memory.

Specifying a size of 0 will set an unlimited cache size.

This method may only be called on SERVER_USE Context objects.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `size_t` |  |

---

#### getSessionCacheSize 

```cpp
size_t getSessionCacheSize() const
```

Returns the current maximum size of the server session cache.

This method may only be called on SERVER_USE Context objects.

---

#### setSessionTimeout 

```cpp
void setSessionTimeout(long seconds)
```

Sets the timeout (in seconds) of cached sessions on the server. A cached session will be removed from the cache if it has not been used for the given number of seconds.

This method may only be called on SERVER_USE Context objects.

| Parameter | Type | Description |
|-----------|------|-------------|
| `seconds` | `long` |  |

---

#### getSessionTimeout 

```cpp
long getSessionTimeout() const
```

Returns the timeout (in seconds) of cached sessions on the server.

This method may only be called on SERVER_USE Context objects.

---

#### flushSessionCache 

```cpp
void flushSessionCache()
```

Flushes the SSL session cache on the server.

This method may only be called on SERVER_USE Context objects.

---

#### disableStatelessSessionResumption 

```cpp
void disableStatelessSessionResumption()
```

Newer versions of OpenSSL support RFC 4507 tickets for stateless session resumption.

The feature can be disabled by calling this method.

---

#### createSSLContext 

```cpp
void createSSLContext()
```

Create a SSL_CTX object according to Context configuration.

---

#### _usage 

```cpp
Usage _usage
```

---

#### _mode 

```cpp
VerificationMode _mode
```

---

#### _sslContext 

```cpp
SSL_CTX * _sslContext
```

---

#### _extendedVerificationErrorDetails 

```cpp
bool _extendedVerificationErrorDetails
```

## SSLManager 

> **Defined in:** `sslmanager.h`

[SSLManager](#classscy_1_1net_1_1SSLManager) is a singleton for holding the default server/client Context and handling callbacks for certificate verification errors and private key passphrases.

### Members

| Name | Description |
|------|-------------|
| [`initializeServer`](#group__net_1ga5476ccf3e346e493c3d053a21f3f2b29) | Initializes the server side of the [SSLManager](#classscy_1_1net_1_1SSLManager) server-side [SSLContext](#classscy_1_1net_1_1SSLContext). |
| [`initializeClient`](#group__net_1ga4472c80b8b9152947333589515621051) | Initializes the client side of the [SSLManager](#classscy_1_1net_1_1SSLManager) with a default client-side [SSLContext](#classscy_1_1net_1_1SSLContext). |
| [`defaultServerContext`](#group__net_1ga5888698b042c4c4a16d69fc98cb0e582) | Returns the default Context used by the server if initialized. |
| [`defaultClientContext`](#group__net_1gaf06768dcfa955ebdd5b9c3977682e6ea) | Returns the default Context used by the client if initialized. |
| [`shutdown`](#group__net_1ga4061d56eaf906bf79a0713310a3841c5) | Shuts down the [SSLManager](#classscy_1_1net_1_1SSLManager) and releases the default Context objects. After a call to [shutdown()](#group__net_1ga4061d56eaf906bf79a0713310a3841c5), the [SSLManager](#classscy_1_1net_1_1SSLManager) can no longer be used. |
| [`instance`](#group__net_1gad3081151d10c5c6c4247f891925f93a7) | Returns the instance of the [SSLManager](#classscy_1_1net_1_1SSLManager) singleton. |
| [`destroy`](#group__net_1ga64402426d6371b93d7a3544da637621f) | Shuts down and destroys the [SSLManager](#classscy_1_1net_1_1SSLManager) singleton instance. |
| [`initNoVerifyClient`](#group__net_1ga9ea94e19abecb2e8227dbda1d577f093) | Initializes a default no verify client context that's useful for testing. |
| [`initNoVerifyServer`](#group__net_1ga71aad45fb6aceb1a551c247c90523e8d) | Initializes a default no verify server context that's useful for testing. |
| [`verifyClientCallback`](#group__net_1gaf4e686f568fceeba5b78df815a050745) | The return value of this method defines how errors in verification are handled. Return 0 to terminate the handshake, or 1 to continue despite the error. |
| [`verifyServerCallback`](#group__net_1gacc7394320818b4d2fe6c75f486876d76) | The return value of this method defines how errors in verification are handled. Return 0 to terminate the handshake, ' or 1 to continue despite the error. |
| [`privateKeyPassphraseCallback`](#group__net_1ga5266f886ac5eb14dcd5e3df0f80c5dc0) | Method is invoked by OpenSSL to retrieve a passwd for an encrypted certificate. The request is delegated to the PrivatekeyPassword event. This method returns the length of the password. |
| [`SSLManager`](#group__net_1gac04b1f45008ee73e39c9e05e3f86c7c8) | Creates the [SSLManager](#classscy_1_1net_1_1SSLManager). |
| [`~SSLManager`](#group__net_1gaa643df995c0323b8c4e4d5f6e0cd8558) | Destroys the [SSLManager](#classscy_1_1net_1_1SSLManager). |
| [`verifyCallback`](#group__net_1gaaca523b350828a78b7dfbd7792e50868) | The return value of this method defines how errors in verification are handled. Return 0 to terminate the handshake, or 1 to continue despite the error. |
| [`ServerVerificationError`](#group__net_1ga5c8d1a58ad63cfe7fd8c6ef8da2bc7bc) | Fired whenever a certificate verification error is detected by the server during a handshake. |
| [`ClientVerificationError`](#group__net_1ga79cecc2be9b8293b79f9dc92b3ab967d) | Fired whenever a certificate verification error is detected by the client during a handshake. |
| [`PrivateKeyPassphraseRequired`](#group__net_1gac92a4d1a788ac605e000055f0c2d1f0b) | Fired when a encrypted certificate is loaded. Not setting the password in the event parameter will result in a failure to load the certificate. |
| [`_defaultServerContext`](#group__net_1ga24b0830bf753c01e75d4fcf73cfb29d5) |  |
| [`_defaultClientContext`](#group__net_1ga800460ae0a45bca4054a039e64475d13) |  |
| [`_mutex`](#group__net_1ga59eebca4de704e16e969adf52f602d68) |  |

---

#### initializeServer 

```cpp
void initializeServer(SSLContext::Ptr ptrContext)
```

Initializes the server side of the [SSLManager](#classscy_1_1net_1_1SSLManager) server-side [SSLContext](#classscy_1_1net_1_1SSLContext).

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptrContext` | `SSLContext::Ptr` |  |

---

#### initializeClient 

```cpp
void initializeClient(SSLContext::Ptr ptrContext)
```

Initializes the client side of the [SSLManager](#classscy_1_1net_1_1SSLManager) with a default client-side [SSLContext](#classscy_1_1net_1_1SSLContext).

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptrContext` | `SSLContext::Ptr` |  |

---

#### defaultServerContext 

```cpp
SSLContext::Ptr defaultServerContext()
```

Returns the default Context used by the server if initialized.

---

#### defaultClientContext 

```cpp
SSLContext::Ptr defaultClientContext()
```

Returns the default Context used by the client if initialized.

---

#### shutdown 

```cpp
void shutdown()
```

Shuts down the [SSLManager](#classscy_1_1net_1_1SSLManager) and releases the default Context objects. After a call to [shutdown()](#group__net_1ga4061d56eaf906bf79a0713310a3841c5), the [SSLManager](#classscy_1_1net_1_1SSLManager) can no longer be used.

Normally, it's not necessary to call this method directly, as this will be called either by [uninitializeSSL()](#group__net_1gab2698107bdb6a2b9a2bcea869e26d50b), or when the [SSLManager](#classscy_1_1net_1_1SSLManager) instance is destroyed.

---

#### instance 

```cpp
static SSLManager & instance()
```

Returns the instance of the [SSLManager](#classscy_1_1net_1_1SSLManager) singleton.

---

#### destroy 

```cpp
static void destroy()
```

Shuts down and destroys the [SSLManager](#classscy_1_1net_1_1SSLManager) singleton instance.

---

#### initNoVerifyClient 

```cpp
static void initNoVerifyClient()
```

Initializes a default no verify client context that's useful for testing.

---

#### initNoVerifyServer 

```cpp
static void initNoVerifyServer(const std::string & privateKeyFile, const std::string & certificateFile)
```

Initializes a default no verify server context that's useful for testing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `privateKeyFile` | `const std::string &` |  |
| `certificateFile` | `const std::string &` |  |

---

#### verifyClientCallback 

```cpp
static int verifyClientCallback(int ok, X509_STORE_CTX * pStore)
```

The return value of this method defines how errors in verification are handled. Return 0 to terminate the handshake, or 1 to continue despite the error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ok` | `int` |  |
| `pStore` | `X509_STORE_CTX *` |  |

---

#### verifyServerCallback 

```cpp
static int verifyServerCallback(int ok, X509_STORE_CTX * pStore)
```

The return value of this method defines how errors in verification are handled. Return 0 to terminate the handshake, ' or 1 to continue despite the error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ok` | `int` |  |
| `pStore` | `X509_STORE_CTX *` |  |

---

#### privateKeyPassphraseCallback 

```cpp
static int privateKeyPassphraseCallback(char * pBuf, int size, int flag, void * userData)
```

Method is invoked by OpenSSL to retrieve a passwd for an encrypted certificate. The request is delegated to the PrivatekeyPassword event. This method returns the length of the password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pBuf` | `char *` |  |
| `size` | `int` |  |
| `flag` | `int` |  |
| `userData` | `void *` |  |

---

#### SSLManager 

```cpp
SSLManager()
```

Creates the [SSLManager](#classscy_1_1net_1_1SSLManager).

---

#### ~SSLManager 

```cpp
~SSLManager()
```

Destroys the [SSLManager](#classscy_1_1net_1_1SSLManager).

---

#### verifyCallback 

```cpp
static int verifyCallback(bool server, int ok, X509_STORE_CTX * pStore)
```

The return value of this method defines how errors in verification are handled. Return 0 to terminate the handshake, or 1 to continue despite the error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `bool` |  |
| `ok` | `int` |  |
| `pStore` | `X509_STORE_CTX *` |  |

---

#### ServerVerificationError 

```cpp
Signal< void(VerificationErrorDetails &)> ServerVerificationError
```

Fired whenever a certificate verification error is detected by the server during a handshake.

---

#### ClientVerificationError 

```cpp
Signal< void(VerificationErrorDetails &)> ClientVerificationError
```

Fired whenever a certificate verification error is detected by the client during a handshake.

---

#### PrivateKeyPassphraseRequired 

```cpp
Signal< void(std::string &)> PrivateKeyPassphraseRequired
```

Fired when a encrypted certificate is loaded. Not setting the password in the event parameter will result in a failure to load the certificate.

---

#### _defaultServerContext 

```cpp
SSLContext::Ptr _defaultServerContext
```

---

#### _defaultClientContext 

```cpp
SSLContext::Ptr _defaultClientContext
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## VerificationErrorDetails 

> **Defined in:** `sslmanager.h`

A utility class for certificate error handling.

### Members

| Name | Description |
|------|-------------|
| [`VerificationErrorDetails`](#group__net_1ga6149cde284d053bbb8ea7f7d4febf260) | Creates the [VerificationErrorDetails](#classscy_1_1net_1_1VerificationErrorDetails). _ignoreError is per default set to false. |
| [`~VerificationErrorDetails`](#group__net_1ga341d067a4369d88415b30f9d6acf9516) | Destroys the [VerificationErrorDetails](#classscy_1_1net_1_1VerificationErrorDetails). |
| [`certificate`](#group__net_1gaa61f1bc21123f77329fcc4c2e9c1b3a5) | Returns the certificate that caused the error. |
| [`errorDepth`](#group__net_1ga7f64fe88500e3df99bd0bd7cd8d2d598) | Returns the position of the certificate in the certificate chain. |
| [`errorNumber`](#group__net_1gac0130d1cbbca92575e2259000f792cef) | Returns the id of the error. |
| [`errorMessage`](#group__net_1gaa13f824914ad36284036dcf6a07d6b2c) | Returns the textual presentation of the errorNumber. |
| [`setIgnoreError`](#group__net_1ga762c71561020eb2eae6842f5cfc6dcd4) | setIgnoreError to true, if a verification error is judged non-fatal by the user. |
| [`getIgnoreError`](#group__net_1ga682fc13cdc647b176d2e0d3ef3e783fb) | returns the value of _ignoreError |
| [`_cert`](#group__net_1ga7bd145c098bbf10aa7b8887a0927dfed) |  |
| [`_errorDepth`](#group__net_1gacaa650f5c1a9317c9e18cb33468aec10) |  |
| [`_errorNumber`](#group__net_1gad62239805ea4bfee112162f09b203163) |  |
| [`_errorMessage`](#group__net_1ga24a7e119f990bdc4965004d070588c30) |  |
| [`_ignoreError`](#group__net_1ga32ad9d20fe3b08310d3b4152ec840bc5) |  |

---

#### VerificationErrorDetails 

```cpp
VerificationErrorDetails(const crypto::X509Certificate & cert, int errDepth, int errNum, const std::string & errMsg)
```

Creates the [VerificationErrorDetails](#classscy_1_1net_1_1VerificationErrorDetails). _ignoreError is per default set to false.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cert` | `const crypto::X509Certificate &` |  |
| `errDepth` | `int` |  |
| `errNum` | `int` |  |
| `errMsg` | `const std::string &` |  |

---

#### ~VerificationErrorDetails 

```cpp
~VerificationErrorDetails()
```

Destroys the [VerificationErrorDetails](#classscy_1_1net_1_1VerificationErrorDetails).

---

#### certificate 

```cpp
inline const crypto::X509Certificate & certificate() const
```

Returns the certificate that caused the error.

---

#### errorDepth 

```cpp
inline int errorDepth() const
```

Returns the position of the certificate in the certificate chain.

---

#### errorNumber 

```cpp
inline int errorNumber() const
```

Returns the id of the error.

---

#### errorMessage 

```cpp
inline const std::string & errorMessage() const
```

Returns the textual presentation of the errorNumber.

---

#### setIgnoreError 

```cpp
inline void setIgnoreError(bool ignoreError)
```

setIgnoreError to true, if a verification error is judged non-fatal by the user.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ignoreError` | `bool` |  |

---

#### getIgnoreError 

```cpp
inline bool getIgnoreError() const
```

returns the value of _ignoreError

---

#### _cert 

```cpp
crypto::X509Certificate _cert
```

---

#### _errorDepth 

```cpp
int _errorDepth
```

---

#### _errorNumber 

```cpp
int _errorNumber
```

---

#### _errorMessage 

```cpp
std::string _errorMessage
```

---

#### _ignoreError 

```cpp
bool _ignoreError
```

## SSLSession 

> **Defined in:** `sslsession.h`

This class encapsulates a SSL session object used with session caching on the client side.

For session caching to work, a client must save the session object from an existing connection, if it wants to reuse it with a future connection.

### Members

| Name | Description |
|------|-------------|
| [`sslSession`](#group__net_1ga6581d3ef3c9d2cb11707fcd0f1848b7e) | Returns the stored OpenSSL SSL_SESSION object. |
| [`SSLSession`](#group__net_1ga432cb2d0fa33e99be752e2f67b05b24e) | Creates a new Session object, using the given SSL_SESSION object. |
| [`~SSLSession`](#group__net_1ga46203b77ac08b39a29c45bd4cb616f96) | Destroys the Session. |
| [`SSLSession`](#group__net_1ga9e95112e1b50fd581979c940f7024655) |  |
| [`_ptr`](#group__net_1ga304e950ce3ef764e602a67033539de62) |  |

---

#### sslSession 

```cpp
SSL_SESSION * sslSession() const
```

Returns the stored OpenSSL SSL_SESSION object.

---

#### SSLSession 

```cpp
SSLSession(SSL_SESSION * ptr)
```

Creates a new Session object, using the given SSL_SESSION object.

The SSL_SESSION's reference count is not changed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `SSL_SESSION *` |  |

---

#### ~SSLSession 

```cpp
~SSLSession()
```

Destroys the Session.

Calls SSL_SESSION_free() on the stored SSL_SESSION object.

---

#### SSLSession 

```cpp
SSLSession()
```

---

#### _ptr 

```cpp
SSL_SESSION * _ptr
```

## SSLSocket 

> **Extends:** `scy::net::TCPSocket`
> **Defined in:** `sslsocket.h`

SSL socket implementation.

### Members

| Name | Description |
|------|-------------|
| [`SSLSocket`](#group__net_1ga621415db5b8ff309cec65201efc086bf) |  |
| [`SSLSocket`](#group__net_1ga6ef56420a681a95cd42e066a842b1b63) |  |
| [`SSLSocket`](#group__net_1ga0d27d1f822710a64478dffc4fad17f98) |  |
| [`~SSLSocket`](#group__net_1ga55a2e9ca93a9fcaddbf2bad40a57966b) |  |
| [`bind`](#group__net_1gaa16ace2e62e4b075bba8920cfbd098ad) | Initialize the [SSLSocket](#classscy_1_1net_1_1SSLSocket) with the given [SSLContext](#classscy_1_1net_1_1SSLContext). |
| [`listen`](#group__net_1ga68e4327abb9e2bdb1ed7e3a6e7c0e49b) | Listens the socket on the given address. |
| [`shutdown`](#group__net_1ga8d6ff3564619b9ea9cb1cc0aa54aa760) | Shuts down the connection by attempting an orderly SSL shutdown, then actually shutting down the TCP connection. |
| [`close`](#group__net_1ga0fe89adb626dcd2aeaba9b941ea3c6cf) | Closes the socket forcefully. |
| [`send`](#group__net_1gabd0a71795b9bfe30bf29447d750c8617) | Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address. |
| [`send`](#group__net_1gace69dc9e5072193cbc7c7a84efd3ad84) |  |
| [`useContext`](#group__net_1ga25cdc10ce54de6edcf57b5612788e23d) | Use the given SSL context for this socket. |
| [`context`](#group__net_1gacd6970a43ee59539f56d29cc503f78ab) | Returns the SSL context used for this socket. |
| [`useSession`](#group__net_1ga73a2c547fb422f580065091121dcba3c) | Sets the SSL session to use for the next connection. Setting a previously saved Session object is necessary to enable session caching. |
| [`currentSession`](#group__net_1gab2d4b703c76423909dd34f47f46e73fe) | Returns the SSL session of the current connection, for reuse in a future connection (if session caching is enabled). |
| [`sessionWasReused`](#group__net_1gab713d571c3f619b39d09686f6299ce14) | Returns true if a reused session was negotiated during the handshake. |
| [`available`](#group__net_1gacbbee7806e51d77e9cbe79c104806f17) | Returns the number of bytes available from the SSL buffer for immediate reading. |
| [`peerCertificate`](#group__net_1ga6d0638ab3739dbb622be0dccf39c5002) | Returns the peer's certificate. |
| [`transport`](#group__net_1ga5c1ccbcbd95f4ffed016be3e42a3e238) | The transport protocol: TCP, UDP or SSLTCP. |
| [`acceptConnection`](#group__net_1ga7192db6b478429219e604abf05c191b9) |  |
| [`onConnect`](#group__net_1ga71746677601b346f690890cdff280fcb) |  |
| [`onRead`](#group__net_1gaeb276dae5d2dd0009507a0fd85aabab2) | Reads raw encrypted SSL data. |
| [`_sslContext`](#group__net_1ga765115a8e31c43eb06a1468685942dd9) |  |
| [`_sslSession`](#group__net_1ga387514f7edb5f125120b2db1e3caffa2) |  |
| [`_sslAdapter`](#group__net_1ga6d18c18a5cf420456474c9e55736a1ac) |  |

---

#### SSLSocket 

```cpp
SSLSocket(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### SSLSocket 

```cpp
SSLSocket(SSLContext::Ptr sslContext, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sslContext` | `SSLContext::Ptr` |  |
| `loop` | `uv::Loop *` |  |

---

#### SSLSocket 

```cpp
SSLSocket(SSLContext::Ptr sslContext, SSLSession::Ptr session, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sslContext` | `SSLContext::Ptr` |  |
| `session` | `SSLSession::Ptr` |  |
| `loop` | `uv::Loop *` |  |

---

#### ~SSLSocket 

```cpp
virtual ~SSLSocket()
```

---

#### bind 

```cpp
virtual void bind(const net::Address & address, unsigned flags)
```

Initialize the [SSLSocket](#classscy_1_1net_1_1SSLSocket) with the given [SSLContext](#classscy_1_1net_1_1SSLContext).

Initializes the socket and establishes a secure connection to the TCP server at the given address.

The SSL handshake is performed when the socket is connected.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const net::Address &` |  |
| `flags` | `unsigned` |  |

---

#### listen 

```cpp
virtual void listen(int backlog)
```

Listens the socket on the given address.

Throws an Exception on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `backlog` | `int` |  |

---

#### shutdown 

```cpp
virtual bool shutdown()
```

Shuts down the connection by attempting an orderly SSL shutdown, then actually shutting down the TCP connection.

---

#### close 

```cpp
virtual void close()
```

Closes the socket forcefully.

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, const net::Address & peerAddress, int flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |
| `flags` | `int` |  |

---

#### useContext 

```cpp
void useContext(SSLContext::Ptr context)
```

Use the given SSL context for this socket.

| Parameter | Type | Description |
|-----------|------|-------------|
| `context` | `SSLContext::Ptr` |  |

---

#### context 

```cpp
SSLContext::Ptr context() const
```

Returns the SSL context used for this socket.

---

#### useSession 

```cpp
void useSession(SSLSession::Ptr session)
```

Sets the SSL session to use for the next connection. Setting a previously saved Session object is necessary to enable session caching.

To remove the currently set session, a nullptr pointer can be given.

Must be called before [connect()](#group__net_1ga54a8edb09f593627d120d15339b4a04d) to be effective.

| Parameter | Type | Description |
|-----------|------|-------------|
| `session` | `SSLSession::Ptr` |  |

---

#### currentSession 

```cpp
SSLSession::Ptr currentSession()
```

Returns the SSL session of the current connection, for reuse in a future connection (if session caching is enabled).

If no connection is established, returns nullptr.

---

#### sessionWasReused 

```cpp
bool sessionWasReused()
```

Returns true if a reused session was negotiated during the handshake.

---

#### available 

```cpp
int available() const
```

Returns the number of bytes available from the SSL buffer for immediate reading.

---

#### peerCertificate 

```cpp
X509 * peerCertificate() const
```

Returns the peer's certificate.

---

#### transport 

```cpp
virtual net::TransportType transport() const
```

The transport protocol: TCP, UDP or SSLTCP.

---

#### acceptConnection 

```cpp
virtual void acceptConnection()
```

---

#### onConnect 

```cpp
virtual void onConnect()
```

---

#### onRead 

```cpp
virtual void onRead(const char * data, size_t len)
```

Reads raw encrypted SSL data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### _sslContext 

```cpp
net::SSLContext::Ptr _sslContext
```

---

#### _sslSession 

```cpp
net::SSLSession::Ptr _sslSession
```

---

#### _sslAdapter 

```cpp
net::SSLAdapter _sslAdapter
```

## TCPSocket 

> **Extends:** `scy::Stream< uv_tcp_t >`, `scy::net::Socket`
> **Subclasses:** `scy::net::SSLSocket`
> **Defined in:** `tcpsocket.h`

TCP socket implementation.

### Members

| Name | Description |
|------|-------------|
| [`TCPSocket`](#group__net_1gaec3b1c6ef0978a45b92a3941550d34cc) |  |
| [`~TCPSocket`](#group__net_1gaf96b67205dcf571df23d167589688858) |  |
| [`shutdown`](#group__net_1gaf4823d66f85cfa1b9fc59a786244c948) | Sends the shutdown packet which should result is socket closure via callback. |
| [`close`](#group__net_1gab486aed3b742f783104b2af9e2d1fd53) | Closes and resets the stream handle. This will close the active socket/pipe and destroy the handle. |
| [`connect`](#group__net_1ga54a8edb09f593627d120d15339b4a04d) | Connects to the given peer IP address. |
| [`connect`](#group__net_1gabdf25f4a4b76b89510b9ac02b50c350c) | Resolves and connects to the given host address. |
| [`send`](#group__net_1gac358d98a73620318e8b8c5774668cb2e) | Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address. |
| [`send`](#group__net_1gac8210c5420cc804749543b3be2d10500) |  |
| [`bind`](#group__net_1gac4f44d12fc31ac77b739e8bb4b68b73c) | Bind a local address to the socket. The address may be IPv4 or IPv6 (if supported). |
| [`listen`](#group__net_1ga3ec7b2814a110d4c6e01ea304ed35f87) | Listens the socket on the given address. |
| [`acceptConnection`](#group__net_1ga32b031a136c7083ed70b9586dbf4cb6c) |  |
| [`setReusePort`](#group__net_1gaca8530109419dfe268c82c22dcc9bcfb) |  |
| [`setNoDelay`](#group__net_1gadded4f43faf2734dfc0a56e1b875c997) |  |
| [`setKeepAlive`](#group__net_1ga159fae0ed680e7c7b981bfab570f3597) |  |
| [`setSimultaneousAccepts`](#group__net_1ga11872bd792fd0a3c7864c04be7c81035) |  |
| [`setMode`](#group__net_1gaecf346cd0d3f3d3b4cd560b5776f84d9) |  |
| [`mode`](#group__net_1ga3415e01ef65890d3e03c6c4bcb716cce) |  |
| [`setError`](#group__net_1ga5f09190f5a798aa81aafe6e14d863f5b) | Set the error and triggers callbacks. |
| [`error`](#group__net_1ga390828725a5262b7d50943d66e511f03) | Return the socket error if any. |
| [`closed`](#group__net_1gad537da936506f40a1750d43f07ceaea5) | Returns true if the native socket handle is closed. |
| [`address`](#group__net_1ga9ca4f02f521e88d7ff93b7da9a817027) | Returns the IP address and port number of the socket. A wildcard address is returned if the socket is not connected. |
| [`peerAddress`](#group__net_1ga452e885bdd0ac8e601c6f8c8d6fd2cc1) | Returns the IP address and port number of the peer socket. A wildcard address is returned if the socket is not connected. |
| [`transport`](#group__net_1gaef3175a72fb606864dc49509992be0f8) | Returns the TCP transport protocol. |
| [`loop`](#group__net_1ga809e5e9d55cf1c9111882f7acfa07909) | Returns the socket event loop. |
| [`self`](#group__net_1gae60bb4b285ed6cf6bbe2a7952dbb33bc) | Return a pointer to the current or derived instance. |
| [`onConnect`](#group__net_1gac2ec475984519b881e9756cf6a2d5cac) |  |
| [`onRead`](#group__net_1gad149233e10e70b3d978e949201eda69d) |  |
| [`onRecv`](#group__net_1ga5cddc3672cd38ebb92e2b699c68599bf) |  |
| [`onError`](#group__net_1ga0f18b40e4845de47e56d67df0ae40b86) | [Error](./doc/api-base.md#structscy_1_1Error) callback. Override to handle errors. The error may be a UV error, or a custom error. |
| [`onClose`](#group__net_1ga9123b0743cd6eb593b093a9ee20fe319) | Close callback. Override to handle closure. |
| [`init`](#group__net_1gab18632622e10ee1c7324b2704cf0a03e) | Initializes the underlying socket context. |
| [`reset`](#group__net_1ga2b0d3646563e08be2697a365e1157e9e) | Resets the socket context for reuse. |
| [`AcceptConnection`](#group__net_1ga22c24cb537c35dbdd0443b5b2d96b374) |  |
| [`_mode`](#group__net_1ga27a244b308b0d1632172cd2a5ebeb8aa) |  |

---

#### TCPSocket 

```cpp
TCPSocket(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~TCPSocket 

```cpp
virtual ~TCPSocket()
```

---

#### shutdown 

```cpp
virtual bool shutdown()
```

Sends the shutdown packet which should result is socket closure via callback.

---

#### close 

```cpp
virtual void close()
```

Closes and resets the stream handle. This will close the active socket/pipe and destroy the handle.

If the stream is already closed this call will have no side-effects.

---

#### connect 

```cpp
virtual void connect(const net::Address & address)
```

Connects to the given peer IP address.

Throws an exception if the address is malformed. Connection errors can be handled via the [Error](./doc/api-base.md#structscy_1_1Error) signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const net::Address &` |  |

---

#### connect 

```cpp
virtual void connect(const std::string & host, uint16_t port)
```

Resolves and connects to the given host address.

Throws an Exception if the host is malformed. Since the DNS callback is asynchronous implementations need to listen for the [Error](./doc/api-base.md#structscy_1_1Error) signal for handling connection errors.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `uint16_t` |  |

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, const net::Address & peerAddress, int flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |
| `flags` | `int` |  |

---

#### bind 

```cpp
virtual void bind(const net::Address & address, unsigned flags)
```

Bind a local address to the socket. The address may be IPv4 or IPv6 (if supported).

Throws an Exception on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const net::Address &` |  |
| `flags` | `unsigned` |  |

---

#### listen 

```cpp
virtual void listen(int backlog)
```

Listens the socket on the given address.

Throws an Exception on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `backlog` | `int` |  |

---

#### acceptConnection 

```cpp
virtual void acceptConnection()
```

---

#### setReusePort 

```cpp
bool setReusePort()
```

---

#### setNoDelay 

```cpp
bool setNoDelay(bool enable)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `enable` | `bool` |  |

---

#### setKeepAlive 

```cpp
bool setKeepAlive(bool enable, int delay)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `enable` | `bool` |  |
| `delay` | `int` |  |

---

#### setSimultaneousAccepts 

```cpp
bool setSimultaneousAccepts(bool enable)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `enable` | `bool` |  |

---

#### setMode 

```cpp
void setMode(SocketMode mode)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `mode` | `SocketMode` |  |

---

#### mode 

```cpp
const SocketMode mode() const
```

---

#### setError 

```cpp
virtual void setError(const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error)
```

Set the error and triggers callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const [scy::Error](./doc/api-base.md#structscy_1_1Error) &` |  |

---

#### error 

```cpp
virtual const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error() const
```

Return the socket error if any.

---

#### closed 

```cpp
virtual bool closed() const
```

Returns true if the native socket handle is closed.

---

#### address 

```cpp
virtual net::Address address() const
```

Returns the IP address and port number of the socket. A wildcard address is returned if the socket is not connected.

---

#### peerAddress 

```cpp
virtual net::Address peerAddress() const
```

Returns the IP address and port number of the peer socket. A wildcard address is returned if the socket is not connected.

---

#### transport 

```cpp
virtual net::TransportType transport() const
```

Returns the TCP transport protocol.

---

#### loop 

```cpp
virtual uv::Loop * loop() const
```

Returns the socket event loop.

---

#### self 

```cpp
virtual void * self()
```

Return a pointer to the current or derived instance.

---

#### onConnect 

```cpp
virtual void onConnect()
```

---

#### onRead 

```cpp
virtual void onRead(const char * data, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### onRecv 

```cpp
virtual void onRecv(const MutableBuffer & buf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const MutableBuffer &` |  |

---

#### onError 

```cpp
virtual void onError(const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error)
```

[Error](./doc/api-base.md#structscy_1_1Error) callback. Override to handle errors. The error may be a UV error, or a custom error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const [scy::Error](./doc/api-base.md#structscy_1_1Error) &` |  |

---

#### onClose 

```cpp
virtual void onClose()
```

Close callback. Override to handle closure.

---

#### init 

```cpp
virtual void init()
```

Initializes the underlying socket context.

---

#### reset 

```cpp
virtual void reset()
```

Resets the socket context for reuse.

---

#### AcceptConnection 

```cpp
Signal< void(const net::TCPSocket::Ptr &)> AcceptConnection
```

---

#### _mode 

```cpp
SocketMode _mode
```

## Transaction 

> **Extends:** `scy::PacketTransaction< PacketT >`, `scy::net::PacketSocketEmitter`
> **Defined in:** `transaction.h`

This class provides request/response functionality for [IPacket](./doc/api-base.md#classscy_1_1IPacket) types emitted from a [Socket](#classscy_1_1net_1_1Socket).

### Members

| Name | Description |
|------|-------------|
| [`Transaction`](#group__net_1gae43259e00099a144f7179053f01b2f01) |  |
| [`send`](#group__net_1ga1571d979ca34fd821ec55540a69201dd) | Starts the transaction timer and sends the request. Overriding classes should implement send logic here. |
| [`cancel`](#group__net_1ga92a0c808ddb8bc5cde490c5f6927b129) | Cancellation means that the agent will not retransmit the request, will not treat the lack of response to be a failure, but will wait the duration of the transaction timeout for a response. |
| [`dispose`](#group__net_1ga4ebc25a93c33e047d2b6c23151a2c1a6) | Schedules the transaction for deferred deletion. |
| [`peerAddress`](#group__net_1ga4dbe158ad4bd2e7a844cfe8571623d71) |  |
| [`~Transaction`](#group__net_1ga5ffa69ee1385312be868b345bc743193) |  |
| [`onPacket`](#group__net_1gaa6c774d7f241760afe5e21cd55eb2970) | Overrides the [PacketSocketEmitter::onPacket](#group__net_1ga0de3859d43f868dbbdb88fda6e713092) callback for checking potential response candidates. |
| [`onResponse`](#group__net_1gae4f8a3d88a8fb944171ea7758e0e95bd) | Called when a successful response match is received. |
| [`checkResponse`](#group__net_1gad7f496deb9796c70a864033645e09121) | Sub classes should derive this method to implement response checking logic. The base implementation only performs address matching. |
| [`_peerAddress`](#group__net_1ga267322b198b169cb769dbc3b74432592) |  |

---

#### Transaction 

```cpp
inline Transaction(const net::Socket::Ptr & socket, const Address & peerAddress, int timeout, int retries)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::Socket::Ptr &` |  |
| `peerAddress` | `const Address &` |  |
| `timeout` | `int` |  |
| `retries` | `int` |  |

---

#### send 

```cpp
virtual inline bool send()
```

Starts the transaction timer and sends the request. Overriding classes should implement send logic here.

---

#### cancel 

```cpp
virtual inline void cancel()
```

Cancellation means that the agent will not retransmit the request, will not treat the lack of response to be a failure, but will wait the duration of the transaction timeout for a response.

---

#### dispose 

```cpp
virtual inline void dispose()
```

Schedules the transaction for deferred deletion.

It is safe to call this function while the transaction is still active, providing the call is made from the same loop thread which the timer is running on.

Protected by the base implementation as this is called by the internal state machine.

---

#### peerAddress 

```cpp
inline Address peerAddress() const
```

---

#### ~Transaction 

```cpp
virtual ~Transaction() = default
```

---

#### onPacket 

```cpp
virtual inline void onPacket(IPacket & packet)
```

Overrides the [PacketSocketEmitter::onPacket](#group__net_1ga0de3859d43f868dbbdb88fda6e713092) callback for checking potential response candidates.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### onResponse 

```cpp
virtual inline void onResponse()
```

Called when a successful response match is received.

---

#### checkResponse 

```cpp
virtual inline bool checkResponse(const PacketT & packet)
```

Sub classes should derive this method to implement response checking logic. The base implementation only performs address matching.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `const PacketT &` |  |

---

#### _peerAddress 

```cpp
Address _peerAddress
```

## UDPSocket 

> **Extends:** `scy::uv::Handle< uv_udp_t >`, `scy::net::Socket`
> **Defined in:** `udpsocket.h`

UDP socket implementation.

### Members

| Name | Description |
|------|-------------|
| [`UDPSocket`](#group__net_1gac832137d0bc5d3397f48186f4265c800) |  |
| [`~UDPSocket`](#group__net_1gac421df9fbeeb69142769802e9a80a178) |  |
| [`connect`](#group__net_1ga89db9f772f64dd169d9363c0adf41932) | Connects to the given peer IP address. |
| [`connect`](#group__net_1ga63bd2be01433407bc8e58fd585432943) | Resolves and connects to the given host address. |
| [`close`](#group__net_1ga0f15d459c3559ed1307fe88c7107e6b0) | Close and destroy the handle. |
| [`bind`](#group__net_1ga914a8a799d51ed173950aade35e914f9) | Bind a local address to the socket. The address may be IPv4 or IPv6 (if supported). |
| [`send`](#group__net_1gaf4e0dc198384fc23e5f575458b92a269) | Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address. |
| [`send`](#group__net_1gaa082e3292be95cc50fb0db7fb327b4c3) |  |
| [`setBroadcast`](#group__net_1ga0ee0352e997a45deff1805bfaa7dbe62) |  |
| [`setMulticastLoop`](#group__net_1gad485a50948ead139d839ec2d53d83707) |  |
| [`setMulticastTTL`](#group__net_1ga1b751c2f4af3dc831bde8ee1c08059d3) |  |
| [`address`](#group__net_1gaf1c03aeaa87edc8b1666368c16eb84c8) | The locally bound address. |
| [`peerAddress`](#group__net_1ga40f222c85f1f1c0ef0da2f751e8aa15c) | The connected peer address. |
| [`transport`](#group__net_1ga1f7e25495d5fdae305009e341c567c5f) | Returns the UDP transport protocol. |
| [`setError`](#group__net_1ga5390d0fb28b4090359e008d541a9ca33) | Set the error and triggers callbacks. |
| [`error`](#group__net_1gad52712581b6bc1b18b72057ca007c025) | Return the socket error if any. |
| [`closed`](#group__net_1ga6358b5af775f5344616e029f95306fb2) | Returns true if the native socket handle is closed. |
| [`loop`](#group__net_1ga3076c8d49eef7dac47b706e096580a07) | Returns the socket event loop. |
| [`self`](#group__net_1gacd160add50e469282daa7c499bebadac) | Return a pointer to the current or derived instance. |
| [`onRecv`](#group__net_1ga1b142a3c18ca885e72d45aff466723a9) |  |
| [`init`](#group__net_1gaa3b285eec570adc6aa71add9c23aff76) | Initializes the underlying socket context. |
| [`reset`](#group__net_1ga426001966462dda0611a52d27e09f295) | Resets the socket context for reuse. |
| [`onError`](#group__net_1ga31d96f40e2012f8069ef47ea3c8d4f4e) | [Error](./doc/api-base.md#structscy_1_1Error) callback. Override to handle errors. The error may be a UV error, or a custom error. |
| [`onClose`](#group__net_1ga45f862d7b7395e07da0a2d3c369dcb80) | Close callback. Override to handle closure. |
| [`recvStart`](#group__net_1ga33d1b77ebd07eade126a299151ef5258) |  |
| [`recvStop`](#group__net_1gaf16dae934756ee7a561a7cc965cde0bd) |  |
| [`onRecv`](#group__net_1gad9b6572bc6425425b294479b51cecfef) |  |
| [`allocRecvBuffer`](#group__net_1ga11234c9a7d898b91b676d5f3944dfbda) |  |
| [`_peer`](#group__net_1ga4b6e88e303adb6fc54a6d496e04d6d7e) |  |
| [`_buffer`](#group__net_1gae4164f1c3392a3ef567cd255191afe78) |  |

---

#### UDPSocket 

```cpp
UDPSocket(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~UDPSocket 

```cpp
virtual ~UDPSocket()
```

---

#### connect 

```cpp
virtual void connect(const net::Address & address)
```

Connects to the given peer IP address.

Throws an exception if the address is malformed. Connection errors can be handled via the [Error](./doc/api-base.md#structscy_1_1Error) signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const net::Address &` |  |

---

#### connect 

```cpp
virtual void connect(const std::string & host, uint16_t port)
```

Resolves and connects to the given host address.

Throws an Exception if the host is malformed. Since the DNS callback is asynchronous implementations need to listen for the [Error](./doc/api-base.md#structscy_1_1Error) signal for handling connection errors.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `uint16_t` |  |

---

#### close 

```cpp
virtual void close()
```

Close and destroy the handle.

---

#### bind 

```cpp
virtual void bind(const net::Address & address, unsigned flags)
```

Bind a local address to the socket. The address may be IPv4 or IPv6 (if supported).

Throws an Exception on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const net::Address &` |  |
| `flags` | `unsigned` |  |

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, const net::Address & peerAddress, int flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `peerAddress` | `const net::Address &` |  |
| `flags` | `int` |  |

---

#### setBroadcast 

```cpp
bool setBroadcast(bool flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### setMulticastLoop 

```cpp
bool setMulticastLoop(bool flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### setMulticastTTL 

```cpp
bool setMulticastTTL(int ttl)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ttl` | `int` |  |

---

#### address 

```cpp
virtual net::Address address() const
```

The locally bound address.

This function will not throw. A Wildcard 0.0.0.0:0 address is returned if the socket is closed or invalid.

---

#### peerAddress 

```cpp
virtual net::Address peerAddress() const
```

The connected peer address.

This function will not throw. A Wildcard 0.0.0.0:0 address is returned if the socket is closed or invalid.

---

#### transport 

```cpp
virtual net::TransportType transport() const
```

Returns the UDP transport protocol.

---

#### setError 

```cpp
virtual void setError(const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error)
```

Set the error and triggers callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const [scy::Error](./doc/api-base.md#structscy_1_1Error) &` |  |

---

#### error 

```cpp
virtual const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error() const
```

Return the socket error if any.

---

#### closed 

```cpp
virtual bool closed() const
```

Returns true if the native socket handle is closed.

---

#### loop 

```cpp
virtual uv::Loop * loop() const
```

Returns the socket event loop.

---

#### self 

```cpp
virtual void * self()
```

Return a pointer to the current or derived instance.

---

#### onRecv 

```cpp
virtual void onRecv(const MutableBuffer & buf, const net::Address & address)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const MutableBuffer &` |  |
| `address` | `const net::Address &` |  |

---

#### init 

```cpp
virtual void init()
```

Initializes the underlying socket context.

---

#### reset 

```cpp
virtual void reset()
```

Resets the socket context for reuse.

---

#### onError 

```cpp
virtual void onError(const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error)
```

[Error](./doc/api-base.md#structscy_1_1Error) callback. Override to handle errors. The error may be a UV error, or a custom error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const [scy::Error](./doc/api-base.md#structscy_1_1Error) &` |  |

---

#### onClose 

```cpp
virtual void onClose()
```

Close callback. Override to handle closure.

---

#### recvStart 

```cpp
virtual bool recvStart()
```

---

#### recvStop 

```cpp
virtual bool recvStop()
```

---

#### onRecv 

```cpp
static void onRecv(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const struct sockaddr * addr, unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_udp_t *` |  |
| `nread` | `ssize_t` |  |
| `buf` | `const uv_buf_t *` |  |
| `addr` | `const struct sockaddr *` |  |
| `flags` | `unsigned` |  |

---

#### allocRecvBuffer 

```cpp
static void allocRecvBuffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_handle_t *` |  |
| `suggested_size` | `size_t` |  |
| `buf` | `uv_buf_t *` |  |

---

#### _peer 

```cpp
net::Address _peer
```

---

#### _buffer 

```cpp
Buffer _buffer
```

## AddressBase 

> **Subclasses:** `scy::net::IPv4AddressBase`

### Members

| Name | Description |
|------|-------------|
| [`host`](#group__net_1ga867db5d11fa5c964830368ac1a4cbb7a) |  |
| [`port`](#group__net_1ga44e7f8716b0b2f0a701d22e3edcf21b3) |  |
| [`family`](#group__net_1ga018e56177bc5c8bf2b02bc88ec6efcfe) |  |
| [`length`](#group__net_1ga9d32a44cd01e1e7e7e082f5c36aee5e1) |  |
| [`addr`](#group__net_1ga532c3627f0f9a78bfa33a31a8951673b) |  |
| [`af`](#group__net_1ga69583a3ceab6d1839acb20d972d7fa7f) |  |
| [`AddressBase`](#group__net_1ga0c4217e82452f7e70750ba1662802a67) |  |
| [`~AddressBase`](#group__net_1gaee163e8131fa554005928e0941c8bf93) |  |
| [`AddressBase`](#group__net_1ga17555486795b6ea35256a3c3f564b4ba) |  |
| [`operator=`](#group__net_1ga4f04abeacdbcd881b8491c5d79f80887) |  |

---

#### host 

```cpp
std::string host() const
```

---

#### port 

```cpp
uint16_t port() const
```

---

#### family 

```cpp
Address::Family family() const
```

---

#### length 

```cpp
socklen_t length() const
```

---

#### addr 

```cpp
const struct sockaddr * addr() const
```

---

#### af 

```cpp
int af() const
```

---

#### AddressBase 

```cpp
inline AddressBase()
```

---

#### ~AddressBase 

```cpp
virtual inline ~AddressBase()
```

---

#### AddressBase 

```cpp
AddressBase(const AddressBase &) = delete
```

---

#### operator= 

```cpp
AddressBase & operator=(const AddressBase &) = delete
```

## IPv4AddressBase 

> **Extends:** `scy::net::AddressBase`

### Members

| Name | Description |
|------|-------------|
| [`IPv4AddressBase`](#group__net_1ga48fc92d2cb95f0b16b6c28b77c7d04d9) |  |
| [`IPv4AddressBase`](#group__net_1ga9130e321cd339b3b9414929e1e6ff6d5) |  |
| [`IPv4AddressBase`](#group__net_1ga88f691bffa5d02c3d34440fab859d2a1) |  |
| [`host`](#group__net_1ga2d455659c2dd8150f39122a4b70c2459) |  |
| [`port`](#group__net_1ga01593bde5ff3a252088beb95619ec343) |  |
| [`family`](#group__net_1ga8200f0c126c892638c7f0818b786a0c6) |  |
| [`length`](#group__net_1gae374547c5843e238cfdfaee8cdd82037) |  |
| [`addr`](#group__net_1ga96ebb36649f22c95d05295b6a1afb699) |  |
| [`af`](#group__net_1ga3468fd918b7a508ed76fb12baf0491e4) |  |
| [`_addr`](#group__net_1ga5f0e8f71be2be18dde24822a37e523ff) |  |

---

#### IPv4AddressBase 

```cpp
inline IPv4AddressBase()
```

---

#### IPv4AddressBase 

```cpp
inline IPv4AddressBase(const struct sockaddr_in * addr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const struct sockaddr_in *` |  |

---

#### IPv4AddressBase 

```cpp
inline IPv4AddressBase(const void * addr, uint16_t port)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const void *` |  |
| `port` | `uint16_t` |  |

---

#### host 

```cpp
virtual inline std::string host() const
```

---

#### port 

```cpp
virtual inline uint16_t port() const
```

---

#### family 

```cpp
virtual inline Address::Family family() const
```

---

#### length 

```cpp
virtual inline socklen_t length() const
```

---

#### addr 

```cpp
virtual inline const struct sockaddr * addr() const
```

---

#### af 

```cpp
virtual inline int af() const
```

---

#### _addr 

```cpp
struct sockaddr_in _addr
```

## PacketInfo 

> **Extends:** `scy::IPacketInfo`
> **Defined in:** `socket.h`

Provides information about packets emitted from a socket. See [SocketPacket](#classscy_1_1net_1_1SocketPacket).

### Members

| Name | Description |
|------|-------------|
| [`PacketInfo`](#group__net_1gac4cee1c5178b1581eee877736e102755) |  |
| [`PacketInfo`](#group__net_1gae0b9f9bcc10f550dfea602f962092247) |  |
| [`clone`](#group__net_1ga92ec5afeaea19e87ea092a48a3a5d5d2) |  |
| [`~PacketInfo`](#group__net_1ga0b05488ca9b52e594d47a21678150869) |  |
| [`socket`](#group__net_1ga679e93ce36b1f3bd031bbc54ea7939db) | The source socket. |
| [`peerAddress`](#group__net_1ga41acad06a100d281506e6bf269b13799) | The originating peer address. For TCP this will always be connected address. |

---

#### PacketInfo 

```cpp
inline PacketInfo(const Socket::Ptr & socket, const Address & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const Socket::Ptr &` |  |
| `peerAddress` | `const Address &` |  |

---

#### PacketInfo 

```cpp
inline PacketInfo(const PacketInfo & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const PacketInfo &` |  |

---

#### clone 

```cpp
virtual inline IPacketInfo * clone() const
```

---

#### ~PacketInfo 

```cpp
virtual inline ~PacketInfo()
```

---

#### socket 

```cpp
Socket::Ptr socket
```

The source socket.

---

#### peerAddress 

```cpp
Address peerAddress
```

The originating peer address. For TCP this will always be connected address.

## Ref 

> **Defined in:** `socketadapter.h`

### Members

| Name | Description |
|------|-------------|
| [`ptr`](#group__net_1ga7850ea3acb3330d7fb974c45f780f1c4) |  |
| [`alive`](#group__net_1ga5e0d22008a914828d87ecd623872db05) |  |

---

#### ptr 

```cpp
SocketAdapter * ptr
```

---

#### alive 

```cpp
bool alive
```

