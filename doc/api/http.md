# http 

### Classes

| Name | Description |
|------|-------------|
| [`Authenticator`](#classicy_1_1http_1_1Authenticator) | This is a utility class for working with HTTP authentication (basic or digest) in [http::Request](http::Request) objects. |
| [`BasicAuthenticator`](#classicy_1_1http_1_1BasicAuthenticator) | This is a utility class for working with HTTP Basic Authentication in [http::Request](http::Request) objects. |
| [`Connection`](#classicy_1_1http_1_1Connection) |  |
| [`ConnectionAdapter`](#classicy_1_1http_1_1ConnectionAdapter) | Default HTTP socket adapter for reading and writing HTTP messages |
| [`ProgressSignal`](#classicy_1_1http_1_1ProgressSignal) | HTTP progress signal for upload and download progress notifications. |
| [`ConnectionStream`](#classicy_1_1http_1_1ConnectionStream) | Packet stream wrapper for a HTTP connection. |
| [`Cookie`](#classicy_1_1http_1_1Cookie) | This class represents a HTTP [Cookie](#classicy_1_1http_1_1Cookie). |
| [`FormWriter`](#classicy_1_1http_1_1FormWriter) | [FormWriter](#classicy_1_1http_1_1FormWriter) is a HTTP client connection adapter for writing HTML forms. |
| [`FormPart`](#classicy_1_1http_1_1FormPart) | An implementation of [FormPart](#classicy_1_1http_1_1FormPart). |
| [`FilePart`](#classicy_1_1http_1_1FilePart) | An implementation of [FilePart](#classicy_1_1http_1_1FilePart) for plain files. |
| [`StringPart`](#classicy_1_1http_1_1StringPart) | An implementation of [StringPart](#classicy_1_1http_1_1StringPart) for plain files. |
| [`Message`](#classicy_1_1http_1_1Message) | The base class for [Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) and [Response](./doc/api-undefined.md#classicy_1_1http_1_1Response). |
| [`ChunkedAdapter`](#classicy_1_1http_1_1ChunkedAdapter) | HTTP chunked transfer encoding adapter for streaming responses. |
| [`MultipartAdapter`](#classicy_1_1http_1_1MultipartAdapter) | HTTP multipart encoding adapter for multipart/x-mixed-replace streaming. |
| [`ParserObserver`](#classicy_1_1http_1_1ParserObserver) | Abstract observer interface for HTTP parser events. |
| [`Parser`](#classicy_1_1http_1_1Parser) | HTTP request/response parser using the llhttp library. |

### Members

| Name | Description |
|------|-------------|
| [`HTTP_API`](#group__http_1gacf879ba207cfe7e6b02da3fe19e29047) |  |

---

#### HTTP_API 

```cpp
HTTP_API()
```

## Authenticator 

> **Defined in:** `authenticator.h`

This is a utility class for working with HTTP authentication (basic or digest) in [http::Request](http::Request) objects.

Note: Do not forget to read the entire response stream from the 401 response before sending the authenticated request, otherwise there may be problems if a persistent connection is used.

### Members

| Name | Description |
|------|-------------|
| [`Authenticator`](#group__http_1ga6214df1c0d604a6925a4ff5341a2570e) | Creates an empty [Authenticator](#classicy_1_1http_1_1Authenticator) object. |
| [`Authenticator`](#group__http_1gaab5bbd50fb1b1d53a321ba47b294e2c8) | Creates an [Authenticator](#classicy_1_1http_1_1Authenticator) object with the given username and password. |
| [`~Authenticator`](#group__http_1gaed3172ab7db55f51397c6230f32dea08) | Destroys the [Authenticator](#classicy_1_1http_1_1Authenticator). |
| [`fromUserInfo`](#group__http_1gad1ee6dffdb0fdd9a8276425ea40fafca) | Parses username:password std::string and sets username and password of the credentials object. Throws SyntaxException on invalid user information. |
| [`fromURI`](#group__http_1ga12f51f7f097ca4e3cfc5d434e754a01d) | Extracts username and password from the given URI and sets username and password of the credentials object. Does nothing if URI has no user info part. |
| [`setUsername`](#group__http_1ga7944c4a74c6a4e5dbd3b2c2c1f252fc6) | Sets the username. |
| [`username`](#group__http_1ga365097004eb19ff11797471edaba7a11) | Returns the username. |
| [`setPassword`](#group__http_1ga4699ccc9f01adab04a81a389b3367e80) | Sets the password. |
| [`password`](#group__http_1ga7eeb4f10098784269c96fc6a3146fac8) | Returns the password. |
| [`authenticate`](#group__http_1ga653edb8897e31620c3f51e3b3241dd27) | Inspects WWW-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request). |
| [`updateAuthInfo`](#group__http_1ga518fc10b748ab74fa1d907b16f623802) | Updates internal state (in case of digest authentication) and replaces authentication information in the request accordingly. |
| [`proxyAuthenticate`](#group__http_1gaf3d04876711434a0ec46b0ed92063415) | Inspects Proxy-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request). |
| [`updateProxyAuthInfo`](#group__http_1ga312f80e580ffd52a53e00df22105c0d0) | Updates internal state (in case of digest authentication) and replaces proxy authentication information in the request accordingly. |
| [`_username`](#classicy_1_1http_1_1Authenticator_1a64a71053942ececa00fe9b9cf268587b) |  |
| [`_password`](#classicy_1_1http_1_1Authenticator_1ae53555ba1c43a70dcb204ce2c444f326) |  |
| [`Authenticator`](#classicy_1_1http_1_1Authenticator_1abd2eb9f36c15630e9a53dea83e7f6f6d) |  |
| [`operator=`](#classicy_1_1http_1_1Authenticator_1a24de65ab7924d093c169017739e50808) |  |

---

#### Authenticator 

```cpp
Authenticator()
```

Creates an empty [Authenticator](#classicy_1_1http_1_1Authenticator) object.

---

#### Authenticator 

```cpp
Authenticator(const std::string & username, const std::string & password)
```

Creates an [Authenticator](#classicy_1_1http_1_1Authenticator) object with the given username and password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |
| `password` | `const std::string &` |  |

---

#### ~Authenticator 

```cpp
~Authenticator()
```

Destroys the [Authenticator](#classicy_1_1http_1_1Authenticator).

---

#### fromUserInfo 

```cpp
void fromUserInfo(const std::string & userInfo)
```

Parses username:password std::string and sets username and password of the credentials object. Throws SyntaxException on invalid user information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `userInfo` | `const std::string &` |  |

---

#### fromURI 

```cpp
void fromURI(const http::URL & uri)
```

Extracts username and password from the given URI and sets username and password of the credentials object. Does nothing if URI has no user info part.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `const http::URL &` |  |

---

#### setUsername 

```cpp
inline void setUsername(const std::string & username)
```

Sets the username.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |

---

#### username 

```cpp
inline const std::string & username() const
```

Returns the username.

---

#### setPassword 

```cpp
inline void setPassword(const std::string & password)
```

Sets the password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `password` | `const std::string &` |  |

---

#### password 

```cpp
inline const std::string & password() const
```

Returns the password.

---

#### authenticate 

```cpp
void authenticate(http::Request & request, const http::Response & response)
```

Inspects WWW-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `http::Request &` |  |
| `response` | `const http::Response &` |  |

---

#### updateAuthInfo 

```cpp
void updateAuthInfo(http::Request & request)
```

Updates internal state (in case of digest authentication) and replaces authentication information in the request accordingly.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `http::Request &` |  |

---

#### proxyAuthenticate 

```cpp
void proxyAuthenticate(http::Request & request, const http::Response & response)
```

Inspects Proxy-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `http::Request &` |  |
| `response` | `const http::Response &` |  |

---

#### updateProxyAuthInfo 

```cpp
void updateProxyAuthInfo(http::Request & request)
```

Updates internal state (in case of digest authentication) and replaces proxy authentication information in the request accordingly.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `http::Request &` |  |

---

#### _username 

```cpp
std::string _username
```

---

#### _password 

```cpp
std::string _password
```

---

#### Authenticator 

```cpp
Authenticator(const [Authenticator](#classicy_1_1http_1_1Authenticator) &) = delete
```

---

#### operator= 

```cpp
[Authenticator](#classicy_1_1http_1_1Authenticator) & operator=(const [Authenticator](#classicy_1_1http_1_1Authenticator) &) = delete
```

## BasicAuthenticator 

> **Defined in:** `authenticator.h`

This is a utility class for working with HTTP Basic Authentication in [http::Request](http::Request) objects.

### Members

| Name | Description |
|------|-------------|
| [`BasicAuthenticator`](#group__http_1ga4e2fd63defaafd17ae03fb754c2da8c1) | Creates an empty [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object. |
| [`BasicAuthenticator`](#group__http_1gad6de1b1584d452ed7e06bedc87ca1021) | Creates a [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object with the given username and password. |
| [`BasicAuthenticator`](#group__http_1gaf36d893fa18c895ad0b2fe1f8316de10) | Creates a [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object with the authentication information from the given request. |
| [`BasicAuthenticator`](#group__http_1gae38dcdedcd5cdcd1c3ffd5fe28a7b1a2) | Creates a [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object with the authentication information in the given std::string. The authentication information can be extracted from a [http::Request](http::Request) object by calling [http::Request::getCredentials()](http::Request::getCredentials()). |
| [`~BasicAuthenticator`](#group__http_1ga5f94ae51fc1c7c593b84a03fef64bde4) | Destroys the [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator). |
| [`setUsername`](#group__http_1ga25effc506bb6d0904b86bd4d20add8c7) | Sets the username. |
| [`username`](#group__http_1ga674869333bed1c65b2e38c7700a98a1e) | Returns the username. |
| [`setPassword`](#group__http_1gabc1db19a45f0501e8ca2b29355c14fe0) | Sets the password. |
| [`password`](#group__http_1ga1ed5b0686db8c1701f515e1a41b3d323) | Returns the password. |
| [`authenticate`](#group__http_1gaaae8a1c5cadebe43bb512bac3974c0bc) | Adds authentication information to the given [http::Request](http::Request). |
| [`proxyAuthenticate`](#group__http_1ga4e342080880e1ce658f95e4bf4c6fb23) | Adds proxy authentication information to the given [http::Request](http::Request). |
| [`parseAuthInfo`](#group__http_1ga2e24fc651b06a3d5f24acc93c13039dd) | Extracts username and password from Basic authentication info by base64-decoding authInfo and splitting the resulting std::string at the ':' delimiter. |
| [`_username`](#classicy_1_1http_1_1BasicAuthenticator_1ae250f9409cedb854b665743b54f4ebe1) |  |
| [`_password`](#classicy_1_1http_1_1BasicAuthenticator_1a68885442e186421e6593e67efa0b851c) |  |
| [`BasicAuthenticator`](#classicy_1_1http_1_1BasicAuthenticator_1a888b7294916e29d0b991becccc8397ad) |  |
| [`operator=`](#classicy_1_1http_1_1BasicAuthenticator_1af91ed4021f657eadf41474f6116b3798) |  |

---

#### BasicAuthenticator 

```cpp
BasicAuthenticator()
```

Creates an empty [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object.

---

#### BasicAuthenticator 

```cpp
BasicAuthenticator(const std::string & username, const std::string & password)
```

Creates a [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object with the given username and password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |
| `password` | `const std::string &` |  |

---

#### BasicAuthenticator 

```cpp
explicit BasicAuthenticator(const http::Request & request)
```

Creates a [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object with the authentication information from the given request.

Throws a NotAuthenticatedException if the request does not contain basic authentication information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const http::Request &` |  |

---

#### BasicAuthenticator 

```cpp
explicit BasicAuthenticator(const std::string & authInfo)
```

Creates a [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) object with the authentication information in the given std::string. The authentication information can be extracted from a [http::Request](http::Request) object by calling [http::Request::getCredentials()](http::Request::getCredentials()).

| Parameter | Type | Description |
|-----------|------|-------------|
| `authInfo` | `const std::string &` |  |

---

#### ~BasicAuthenticator 

```cpp
~BasicAuthenticator()
```

Destroys the [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator).

---

#### setUsername 

```cpp
void setUsername(const std::string & username)
```

Sets the username.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |

---

#### username 

```cpp
const std::string & username() const
```

Returns the username.

---

#### setPassword 

```cpp
void setPassword(const std::string & password)
```

Sets the password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `password` | `const std::string &` |  |

---

#### password 

```cpp
const std::string & password() const
```

Returns the password.

---

#### authenticate 

```cpp
void authenticate(http::Request & request) const
```

Adds authentication information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `http::Request &` |  |

---

#### proxyAuthenticate 

```cpp
void proxyAuthenticate(http::Request & request) const
```

Adds proxy authentication information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `http::Request &` |  |

---

#### parseAuthInfo 

```cpp
void parseAuthInfo(const std::string & authInfo)
```

Extracts username and password from Basic authentication info by base64-decoding authInfo and splitting the resulting std::string at the ':' delimiter.

| Parameter | Type | Description |
|-----------|------|-------------|
| `authInfo` | `const std::string &` |  |

---

#### _username 

```cpp
std::string _username
```

---

#### _password 

```cpp
std::string _password
```

---

#### BasicAuthenticator 

```cpp
BasicAuthenticator(const [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) &) = delete
```

---

#### operator= 

```cpp
[BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) & operator=(const [BasicAuthenticator](#classicy_1_1http_1_1BasicAuthenticator) &) = delete
```

## Connection 

> **Extends:** `icy::net::SocketAdapter`
> **Subclasses:** `icy::http::ClientConnection`, `icy::http::ServerConnection`
> **Defined in:** `connection.h`

### Members

| Name | Description |
|------|-------------|
| [`Connection`](#classicy_1_1http_1_1Connection_1a5c3755531888c5d1117a5191da11d031) |  |
| [`~Connection`](#classicy_1_1http_1_1Connection_1a34b5926baf06240530f2194af8865071) |  |
| [`onHeaders`](#classicy_1_1http_1_1Connection_1a336fbc30418a29d18531050fab48823a) |  |
| [`onPayload`](#classicy_1_1http_1_1Connection_1ac1f37546ff78e3af23bc14d22f89a332) |  |
| [`onComplete`](#classicy_1_1http_1_1Connection_1a6eb7d0d86a109307c1549eaf9316daa7) |  |
| [`onClose`](#classicy_1_1http_1_1Connection_1ab597f00ecbf6ddd4e9582f98eff9e34b) |  |
| [`send`](#classicy_1_1http_1_1Connection_1ae8aa01bb6b3c6bb53c484e04f391a4d7) | Send raw data to the peer. |
| [`sendHeader`](#classicy_1_1http_1_1Connection_1a1de9285b70b16e7301ef399dea4e4fe8) | Send the outdoing HTTP header. |
| [`close`](#classicy_1_1http_1_1Connection_1a7c40813c43b7e09e0b52ec8f33378dc9) | Close the connection and schedule the object for deferred deletion. |
| [`closed`](#classicy_1_1http_1_1Connection_1aeaf613f9c94f3e93b981b4929f5a2406) | Return true if the connection is closed. |
| [`error`](#classicy_1_1http_1_1Connection_1ae097a4b0be0ce96f55adb7f099eb21d4) | Return the error object if any. |
| [`shouldSendHeader`](#classicy_1_1http_1_1Connection_1a38cfd15a7f82a369eaebddf5e4d5bc0c) | Return true if headers should be automatically sent. |
| [`shouldSendHeader`](#classicy_1_1http_1_1Connection_1ad8e7071bd3cd8162ddfa739ed4a30f94) | Set true to prevent auto-sending HTTP headers. |
| [`replaceAdapter`](#classicy_1_1http_1_1Connection_1a0eb7c6200dd3c7e6d78a96f0100b1d9c) | Assign the new [ConnectionAdapter](#classicy_1_1http_1_1ConnectionAdapter) and setup the chain The flow is: [Connection](#classicy_1_1http_1_1Connection) <-> [ConnectionAdapter](#classicy_1_1http_1_1ConnectionAdapter) <-> Socket |
| [`secure`](#classicy_1_1http_1_1Connection_1a48910a2f80c7b8a69c9507085fee7849) | Return true if the connection uses TLS/SSL. |
| [`socket`](#classicy_1_1http_1_1Connection_1a3394e7681197ee4ba5e481a6a5b24d7b) | Return the underlying socket pointer. |
| [`adapter`](#classicy_1_1http_1_1Connection_1a653678bd18a45bca9f28e127b9e16b6c) | Return the underlying adapter pointer. |
| [`request`](#classicy_1_1http_1_1Connection_1a43cc676959b5eb284d9f67a911bbca36) | The HTTP request headers. |
| [`response`](#classicy_1_1http_1_1Connection_1a69d78cf331ae34a8f57a226f4c78d6f6) | The HTTP response headers. |
| [`incomingHeader`](#classicy_1_1http_1_1Connection_1ab004764f209c4825536cd3a7d2c6d87e) |  |
| [`outgoingHeader`](#classicy_1_1http_1_1Connection_1a26be5aa620c2b5e98f33b6216c2921ab) |  |
| [`_socket`](#classicy_1_1http_1_1Connection_1a384c77e03a59df46c65e8b1895b03ff3) |  |
| [`_adapter`](#classicy_1_1http_1_1Connection_1a1433532f714a93503af92757c7a1c0e9) |  |
| [`_request`](#classicy_1_1http_1_1Connection_1aa8acc765dd7da86268d4534053be3497) |  |
| [`_response`](#classicy_1_1http_1_1Connection_1a6bce4856f6f0f6daba2ae58e620dd392) |  |
| [`_error`](#classicy_1_1http_1_1Connection_1a65dddb1c2b352b6e129457c70f762b7c) |  |
| [`_closed`](#classicy_1_1http_1_1Connection_1a930e46e3b07a1919b617a6e037959e99) |  |
| [`_shouldSendHeader`](#classicy_1_1http_1_1Connection_1a4895da546cdb23fbd3fd8be4fe2f6bb9) |  |
| [`setError`](#classicy_1_1http_1_1Connection_1aebeaf7cb7d32b02026a0f608d4ffd4a9) | Set the internal error. Note: Setting the error does not `[close()](#classicy_1_1http_1_1Connection_1a7c40813c43b7e09e0b52ec8f33378dc9)` the connection. |
| [`onSocketConnect`](#classicy_1_1http_1_1Connection_1a80bb29189a18393f6d758bff6532bd7a) | [net::SocketAdapter](./doc/api-net.md#classicy_1_1net_1_1SocketAdapter) interface |
| [`onSocketRecv`](#classicy_1_1http_1_1Connection_1aec7195288b5ec70dc9013727ec06660d) |  |
| [`onSocketError`](#classicy_1_1http_1_1Connection_1aa7caac1e4f068bd8d95d3bccdd8eda08) |  |
| [`onSocketClose`](#classicy_1_1http_1_1Connection_1a6b8fcdccf44d735c04c9d705192a0ed6) |  |
| [`Ptr`](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) |  |

---

#### Connection 

```cpp
Connection(const [net::TCPSocket::Ptr](./doc/api-net.md#group__net_1ga2ac4375e7cda1d8bffa0b1750f84bc22) & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const [net::TCPSocket::Ptr](./doc/api-net.md#group__net_1ga2ac4375e7cda1d8bffa0b1750f84bc22) &` |  |

---

#### ~Connection 

```cpp
virtual ~Connection()
```

---

#### onHeaders 

```cpp
void onHeaders()
```

---

#### onPayload 

```cpp
void onPayload(const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) &)
```

---

#### onComplete 

```cpp
void onComplete()
```

---

#### onClose 

```cpp
void onClose()
```

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Send raw data to the peer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

#### sendHeader 

```cpp
virtual ssize_t sendHeader()
```

Send the outdoing HTTP header.

---

#### close 

```cpp
virtual void close()
```

Close the connection and schedule the object for deferred deletion.

---

#### closed 

```cpp
bool closed() const
```

Return true if the connection is closed.

---

#### error 

```cpp
[icy::Error](./doc/api-base.md#structicy_1_1Error) error() const
```

Return the error object if any.

---

#### shouldSendHeader 

```cpp
bool shouldSendHeader() const
```

Return true if headers should be automatically sent.

---

#### shouldSendHeader 

```cpp
void shouldSendHeader(bool flag)
```

Set true to prevent auto-sending HTTP headers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### replaceAdapter 

```cpp
virtual void replaceAdapter([net::SocketAdapter](./doc/api-net.md#classicy_1_1net_1_1SocketAdapter) * adapter)
```

Assign the new [ConnectionAdapter](#classicy_1_1http_1_1ConnectionAdapter) and setup the chain The flow is: [Connection](#classicy_1_1http_1_1Connection) <-> [ConnectionAdapter](#classicy_1_1http_1_1ConnectionAdapter) <-> Socket

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `[net::SocketAdapter](./doc/api-net.md#classicy_1_1net_1_1SocketAdapter) *` |  |

---

#### secure 

```cpp
bool secure() const
```

Return true if the connection uses TLS/SSL.

---

#### socket 

```cpp
[net::TCPSocket::Ptr](./doc/api-net.md#group__net_1ga2ac4375e7cda1d8bffa0b1750f84bc22) & socket()
```

Return the underlying socket pointer.

---

#### adapter 

```cpp
[net::SocketAdapter](./doc/api-net.md#classicy_1_1net_1_1SocketAdapter) * adapter() const
```

Return the underlying adapter pointer.

---

#### request 

```cpp
[Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) & request()
```

The HTTP request headers.

---

#### response 

```cpp
[Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) & response()
```

The HTTP response headers.

---

#### incomingHeader 

```cpp
[http::Message](#classicy_1_1http_1_1Message) * incomingHeader()
```

---

#### outgoingHeader 

```cpp
[http::Message](#classicy_1_1http_1_1Message) * outgoingHeader()
```

---

#### _socket 

```cpp
[net::TCPSocket::Ptr](./doc/api-net.md#group__net_1ga2ac4375e7cda1d8bffa0b1750f84bc22) _socket
```

---

#### _adapter 

```cpp
[net::SocketAdapter](./doc/api-net.md#classicy_1_1net_1_1SocketAdapter) * _adapter
```

---

#### _request 

```cpp
[Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) _request
```

---

#### _response 

```cpp
[Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) _response
```

---

#### _error 

```cpp
[icy::Error](./doc/api-base.md#structicy_1_1Error) _error
```

---

#### _closed 

```cpp
bool _closed
```

---

#### _shouldSendHeader 

```cpp
bool _shouldSendHeader
```

---

#### setError 

```cpp
virtual void setError(const [icy::Error](./doc/api-base.md#structicy_1_1Error) & err)
```

Set the internal error. Note: Setting the error does not `[close()](#classicy_1_1http_1_1Connection_1a7c40813c43b7e09e0b52ec8f33378dc9)` the connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [icy::Error](./doc/api-base.md#structicy_1_1Error) &` |  |

---

#### onSocketConnect 

```cpp
virtual void onSocketConnect([net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) & socket)
```

[net::SocketAdapter](./doc/api-net.md#classicy_1_1net_1_1SocketAdapter) interface

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) &` |  |

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv([net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) & socket, const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) & buffer, const [net::Address](./doc/api-net.md#classicy_1_1net_1_1Address) & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) &` |  |
| `buffer` | `const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) &` |  |
| `peerAddress` | `const [net::Address](./doc/api-net.md#classicy_1_1net_1_1Address) &` |  |

---

#### onSocketError 

```cpp
virtual void onSocketError([net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) & socket, const [icy::Error](./doc/api-base.md#structicy_1_1Error) & error)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) &` |  |
| `error` | `const [icy::Error](./doc/api-base.md#structicy_1_1Error) &` |  |

---

#### onSocketClose 

```cpp
virtual void onSocketClose([net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) &` |  |

---

#### Ptr 

```cpp
std::shared_ptr< [Connection](#classicy_1_1http_1_1Connection) > Ptr()
```

## ConnectionAdapter 

> **Extends:** `icy::http::ParserObserver`, `icy::net::SocketAdapter`
> **Defined in:** `connection.h`

Default HTTP socket adapter for reading and writing HTTP messages

### Members

| Name | Description |
|------|-------------|
| [`ConnectionAdapter`](#classicy_1_1http_1_1ConnectionAdapter_1a38b33fa02e3469a0f2ed662bfd5475ce) |  |
| [`~ConnectionAdapter`](#classicy_1_1http_1_1ConnectionAdapter_1adf323289c48fe19dddc2bd2b9eab9329) |  |
| [`send`](#classicy_1_1http_1_1ConnectionAdapter_1a616bb6dad71a6d043ed3bb5fdc578452) | Sends the given data buffer to the connected peer. Returns the number of bytes sent or -1 on error. No exception will be thrown. For TCP sockets the given peer address must match the connected peer address. |
| [`removeReceiver`](#classicy_1_1http_1_1ConnectionAdapter_1a41260efb10ce48a72c8f412b81d47fb0) | Remove the given receiver. |
| [`parser`](#classicy_1_1http_1_1ConnectionAdapter_1a1a4df38a8eb53687fbdf13177dd9cc1c) |  |
| [`connection`](#classicy_1_1http_1_1ConnectionAdapter_1a19a7b1d36d33282658598c7f76c3e29c) |  |
| [`_connection`](#classicy_1_1http_1_1ConnectionAdapter_1a65d1641ca2afc4b4bb93e841dd6b1658) |  |
| [`_parser`](#classicy_1_1http_1_1ConnectionAdapter_1a02352190d724739060c896e0acc743c0) |  |
| [`onSocketRecv`](#classicy_1_1http_1_1ConnectionAdapter_1af675696c310d06f65e3110c206b1991c) | SocketAdapter interface. |
| [`onParserHeader`](#classicy_1_1http_1_1ConnectionAdapter_1a611f889d794434f91f6b36c60a995bc5) | HTTP [Parser](#classicy_1_1http_1_1Parser) interface. |
| [`onParserHeadersEnd`](#classicy_1_1http_1_1ConnectionAdapter_1a0c0c55b0735b96facd54a715841f83d5) |  |
| [`onParserChunk`](#classicy_1_1http_1_1ConnectionAdapter_1a5c1792adab1361782d3492b8701b80f2) |  |
| [`onParserError`](#classicy_1_1http_1_1ConnectionAdapter_1ae7b5f7ab06604e51f8feee248df8aaa1) |  |
| [`onParserEnd`](#classicy_1_1http_1_1ConnectionAdapter_1ac45acb1992c50b24b438f830f4e8a245) |  |

---

#### ConnectionAdapter 

```cpp
ConnectionAdapter([Connection](#classicy_1_1http_1_1Connection) * connection, llhttp_type_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection](#classicy_1_1http_1_1Connection) *` |  |
| `type` | `llhttp_type_t` |  |

---

#### ~ConnectionAdapter 

```cpp
virtual ~ConnectionAdapter()
```

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

#### removeReceiver 

```cpp
virtual void removeReceiver([SocketAdapter](./doc/api-net.md#group__net_1ga35ca78ac04abbd6828af02fb1b65e6d1) * adapter)
```

Remove the given receiver.

By default this function does nothing unless the given receiver matches the current receiver.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `[SocketAdapter](./doc/api-net.md#group__net_1ga35ca78ac04abbd6828af02fb1b65e6d1) *` |  |

---

#### parser 

```cpp
[Parser](#classicy_1_1http_1_1Parser) & parser()
```

---

#### connection 

```cpp
[Connection](#classicy_1_1http_1_1Connection) * connection()
```

---

#### _connection 

```cpp
[Connection](#classicy_1_1http_1_1Connection) * _connection
```

---

#### _parser 

```cpp
[Parser](#classicy_1_1http_1_1Parser) _parser
```

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv([net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) & socket, const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) & buffer, const [net::Address](./doc/api-net.md#classicy_1_1net_1_1Address) & peerAddress)
```

SocketAdapter interface.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) &` |  |
| `buffer` | `const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) &` |  |
| `peerAddress` | `const [net::Address](./doc/api-net.md#classicy_1_1net_1_1Address) &` |  |

---

#### onParserHeader 

```cpp
virtual void onParserHeader(const std::string & name, const std::string & value)
```

HTTP [Parser](#classicy_1_1http_1_1Parser) interface.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### onParserHeadersEnd 

```cpp
virtual void onParserHeadersEnd(bool upgrade)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `upgrade` | `bool` |  |

---

#### onParserChunk 

```cpp
virtual void onParserChunk(const char * buf, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const char *` |  |
| `len` | `size_t` |  |

---

#### onParserError 

```cpp
virtual void onParserError(const [icy::Error](./doc/api-base.md#structicy_1_1Error) & err)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [icy::Error](./doc/api-base.md#structicy_1_1Error) &` |  |

---

#### onParserEnd 

```cpp
virtual void onParserEnd()
```

## ProgressSignal 

> **Extends:** `icy::Signal< void(const double &)>`
> **Defined in:** `connection.h`

HTTP progress signal for upload and download progress notifications.

### Members

| Name | Description |
|------|-------------|
| [`sender`](#classicy_1_1http_1_1ProgressSignal_1aac7fe5b5998e92c2ebfc4be61490f7ba) |  |
| [`current`](#classicy_1_1http_1_1ProgressSignal_1a8b1acf12889f7fe46ffac81a07802e7a) |  |
| [`total`](#classicy_1_1http_1_1ProgressSignal_1abae987c641001e503f4df3e4901f74f3) |  |
| [`ProgressSignal`](#classicy_1_1http_1_1ProgressSignal_1a096bf69d4c5dfdb71eb5e16c84b14d91) |  |
| [`progress`](#classicy_1_1http_1_1ProgressSignal_1a312d627b4aa2afa7922d352c21803c04) |  |
| [`update`](#classicy_1_1http_1_1ProgressSignal_1ac3373e381f6b191a3e88d7b449ab7b94) |  |

---

#### sender 

```cpp
void * sender
```

---

#### current 

```cpp
uint64_t current
```

---

#### total 

```cpp
uint64_t total
```

---

#### ProgressSignal 

```cpp
inline ProgressSignal()
```

---

#### progress 

```cpp
inline double progress() const
```

---

#### update 

```cpp
inline void update(int nread)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `nread` | `int` |  |

## ConnectionStream 

> **Extends:** `icy::net::SocketAdapter`
> **Defined in:** `connection.h`

Packet stream wrapper for a HTTP connection.

### Members

| Name | Description |
|------|-------------|
| [`Outgoing`](#classicy_1_1http_1_1ConnectionStream_1adc72f42dbbaddafd85a2d69cc80cecaf) | The Outgoing stream is responsible for packetizing raw application data into the agreed upon HTTP format and sending it to the peer. |
| [`Incoming`](#classicy_1_1http_1_1ConnectionStream_1a4e3af4a978d6dd5ec04653634226dcb5) | The Incoming stream emits incoming HTTP packets for processing by the application. |
| [`IncomingProgress`](#classicy_1_1http_1_1ConnectionStream_1a5b613af4b34bbecd05d8c059a12b032d) | Fired on download progress. |
| [`OutgoingProgress`](#classicy_1_1http_1_1ConnectionStream_1a7aa44deb62021bc3e2aa65c295f9c843) | Fired on upload progress. |
| [`ConnectionStream`](#classicy_1_1http_1_1ConnectionStream_1a107853c5a7d4f01a61caaae580c9ef7e) |  |
| [`~ConnectionStream`](#classicy_1_1http_1_1ConnectionStream_1aede98da6f993e7e012fd07f4de282c6a) |  |
| [`send`](#classicy_1_1http_1_1ConnectionStream_1aa2f2b6fecfbdead658d483c5870bf599) | Send data via the Outgoing stream. |
| [`connection`](#classicy_1_1http_1_1ConnectionStream_1a8e5f5ce43a3b7a6e642e4ef764cd5e5c) | Return a reference to the underlying connection. |
| [`_connection`](#classicy_1_1http_1_1ConnectionStream_1a49879d5fc6bf781f090fe04e48170030) |  |
| [`onSocketRecv`](#classicy_1_1http_1_1ConnectionStream_1ace624b22e27976e49e405f61772ffc6d) |  |

---

#### Outgoing 

```cpp
[PacketStream](./doc/api-base.md#classicy_1_1PacketStream) Outgoing
```

The Outgoing stream is responsible for packetizing raw application data into the agreed upon HTTP format and sending it to the peer.

---

#### Incoming 

```cpp
[PacketStream](./doc/api-base.md#classicy_1_1PacketStream) Incoming
```

The Incoming stream emits incoming HTTP packets for processing by the application.

This is useful for example when writing incoming data to a file.

---

#### IncomingProgress 

```cpp
[ProgressSignal](#classicy_1_1http_1_1ProgressSignal) IncomingProgress
```

Fired on download progress.

---

#### OutgoingProgress 

```cpp
[ProgressSignal](#classicy_1_1http_1_1ProgressSignal) OutgoingProgress
```

Fired on upload progress.

---

#### ConnectionStream 

```cpp
ConnectionStream([Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) connection)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb)` |  |

---

#### ~ConnectionStream 

```cpp
virtual ~ConnectionStream()
```

---

#### send 

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Send data via the Outgoing stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

#### connection 

```cpp
[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) connection()
```

Return a reference to the underlying connection.

---

#### _connection 

```cpp
[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) _connection
```

---

#### onSocketRecv 

```cpp
virtual void onSocketRecv([net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) & socket, const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) & buffer, const [net::Address](./doc/api-net.md#classicy_1_1net_1_1Address) & peerAddress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](./doc/api-net.md#classicy_1_1net_1_1Socket) &` |  |
| `buffer` | `const [MutableBuffer](./doc/api-base.md#classicy_1_1MutableBuffer) &` |  |
| `peerAddress` | `const [net::Address](./doc/api-net.md#classicy_1_1net_1_1Address) &` |  |

## Cookie 

> **Defined in:** `cookie.h`

This class represents a HTTP [Cookie](#classicy_1_1http_1_1Cookie).

A cookie is a small amount of information sent by a Web server to a Web browser, saved by the browser, and later sent back to the server. A cookie's value can uniquely identify a client, so cookies are commonly used for session management.

A cookie has a name, a single value, and optional attributes such as a comment, path and domain qualifiers, a maximum age, and a version number.

This class supports both the Version 0 (by Netscape) and Version 1 (by RFC 2109) cookie specifications. By default, cookies are created using Version 0 to ensure the best interoperability.

### Members

| Name | Description |
|------|-------------|
| [`Cookie`](#group__http_1ga7fe027bf50ad3e074e3394cee3da9f08) | Creates an empty [Cookie](#classicy_1_1http_1_1Cookie). |
| [`Cookie`](#group__http_1ga027e2a2f4f74f8a9908e4558c7749912) | Creates a cookie with the given name. The cookie never expires. |
| [`Cookie`](#group__http_1ga691e1a1ad74b1dcb68045335491965b5) | Creates a cookie from the given [NVCollection](./doc/api-base.md#classicy_1_1NVCollection). |
| [`Cookie`](#group__http_1gaa2944b0e82cf14e9001cb33706aa1197) | Creates a cookie with the given name and value. The cookie never expires. |
| [`Cookie`](#group__http_1ga12f322ffcbc6b333f022ea1c646a89c9) | Creates the [Cookie](#classicy_1_1http_1_1Cookie) by copying another one. |
| [`~Cookie`](#group__http_1gaa1237673ec2466ff08022b689b3149ab) | Destroys the [Cookie](#classicy_1_1http_1_1Cookie). |
| [`operator=`](#group__http_1gaf79ee72cc2c039956c3b383b51ba67f6) | Assigns a cookie. |
| [`setVersion`](#group__http_1ga0503c149157a6c1f7d3c2d4232e91eff) | Sets the version of the cookie. |
| [`setName`](#group__http_1gad8867c1d8c54497acc3b251b9086f73d) | Sets the name of the cookie. |
| [`setValue`](#group__http_1gae819d97814ae5c7b225e5678215a8c93) | Sets the value of the cookie. |
| [`setComment`](#group__http_1ga2ab87e18725de24a714047755a571624) | Sets the comment for the cookie. |
| [`setDomain`](#group__http_1gada05d2d453ce8bd5ce9b98fe78344af5) | Sets the domain for the cookie. |
| [`setPath`](#group__http_1gac36b773a25ed2a55dfd327218d42d2d1) | Sets the path for the cookie. |
| [`setSecure`](#group__http_1ga6f7cc92be1f67f178283bdf898f93cee) | Sets the value of the secure flag for the cookie. |
| [`setMaxAge`](#group__http_1ga9cbe3e7dcb2b1ddfd5b5ef015dc3d04d) | Sets the maximum age in seconds for the cookie. |
| [`setHttpOnly`](#group__http_1gabe8656c52f52eaf3efe62df8144bfe72) | Sets the HttpOnly flag for the cookie. |
| [`toString`](#group__http_1ga687bf0e8583f850b75df212a4daf25a2) | Returns a std::string representation of the cookie, suitable for use in a Set-Cookie header. |
| [`escape`](#group__http_1ga5dc9fb3d11c40a91b69c562135a4a5f8) | Escapes the given std::string by replacing all non-alphanumeric characters with escape sequences in the form xx, where xx is the hexadecimal character code. |
| [`unescape`](#group__http_1gaae63bf7998ccc36d20ed8dbea8da6961) | Unescapes the given std::string by replacing all escape sequences in the form xx with the respective characters. |
| [`getVersion`](#classicy_1_1http_1_1Cookie_1a6bba92249ab2db70324e8563e944de61) | Returns the version of the cookie, which is either 0 or 1. |
| [`getName`](#classicy_1_1http_1_1Cookie_1a19a1d78ead5a8f7bd2e857dcaeb01afe) | Returns the name of the cookie. |
| [`getValue`](#classicy_1_1http_1_1Cookie_1a2ce5ad4ace4ef72042b5d66ba1073e52) | Returns the value of the cookie. |
| [`getComment`](#classicy_1_1http_1_1Cookie_1ada9b4df4051c57243a739984aa91941e) | Returns the comment for the cookie. |
| [`getDomain`](#classicy_1_1http_1_1Cookie_1a0b19831c3a5651e2b168445dbb324d09) | Returns the domain for the cookie. |
| [`getPath`](#classicy_1_1http_1_1Cookie_1a15927613e58d7b0020941fb0f26560ad) | Returns the path for the cookie. |
| [`getSecure`](#classicy_1_1http_1_1Cookie_1af1af920b2dc5cc4d2e12d12ab7221b2f) | Returns the value of the secure flag for the cookie. |
| [`getMaxAge`](#classicy_1_1http_1_1Cookie_1acf9466fafc045014b7a8645b9e02d0ed) | Returns the maximum age in seconds for the cookie. |
| [`getHttpOnly`](#classicy_1_1http_1_1Cookie_1a1ed3b24d77125d5162c70c744912005c) | Returns true if the cookie's HttpOnly flag is set. |
| [`_version`](#classicy_1_1http_1_1Cookie_1a61ce322d16cd4c14ec21d1ea06a2cb56) |  |
| [`_name`](#classicy_1_1http_1_1Cookie_1a728a3510c99e41f6cff7ce31b3f5283c) |  |
| [`_value`](#classicy_1_1http_1_1Cookie_1a4d80ecfe24a6851659a012cc453e2ee5) |  |
| [`_comment`](#classicy_1_1http_1_1Cookie_1a572f89925a594ecbc5376ea49c633a07) |  |
| [`_domain`](#classicy_1_1http_1_1Cookie_1a885055b5581eefb8aef0d661f69093a2) |  |
| [`_path`](#classicy_1_1http_1_1Cookie_1add987dcc3bb598b21d90b5e5e5018e77) |  |
| [`_secure`](#classicy_1_1http_1_1Cookie_1a90e66610d5aee9f2d6fc79dedbcd60c9) |  |
| [`_maxAge`](#classicy_1_1http_1_1Cookie_1a42224de596cc7c463e3ed2ca48c50376) |  |
| [`_httpOnly`](#classicy_1_1http_1_1Cookie_1adc9af08a4ec098de43dd4264a3c028f7) |  |

---

#### Cookie 

```cpp
Cookie()
```

Creates an empty [Cookie](#classicy_1_1http_1_1Cookie).

---

#### Cookie 

```cpp
explicit Cookie(const std::string & name)
```

Creates a cookie with the given name. The cookie never expires.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### Cookie 

```cpp
explicit Cookie(const NVCollection & nvc)
```

Creates a cookie from the given [NVCollection](./doc/api-base.md#classicy_1_1NVCollection).

| Parameter | Type | Description |
|-----------|------|-------------|
| `nvc` | `const NVCollection &` |  |

---

#### Cookie 

```cpp
Cookie(const std::string & name, const std::string & value)
```

Creates a cookie with the given name and value. The cookie never expires.

Note: If value contains whitespace or non-alphanumeric characters, the value should be escaped by calling [escape()](#group__http_1ga5dc9fb3d11c40a91b69c562135a4a5f8) before passing it to the constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### Cookie 

```cpp
Cookie(const Cookie & cookie)
```

Creates the [Cookie](#classicy_1_1http_1_1Cookie) by copying another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookie` | `const Cookie &` |  |

---

#### ~Cookie 

```cpp
~Cookie()
```

Destroys the [Cookie](#classicy_1_1http_1_1Cookie).

---

#### operator= 

```cpp
Cookie & operator=(const Cookie & cookie)
```

Assigns a cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookie` | `const Cookie &` |  |

---

#### setVersion 

```cpp
void setVersion(int version)
```

Sets the version of the cookie.

Version must be either 0 (denoting a Netscape cookie) or 1 (denoting a RFC 2109 cookie).

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `int` |  |

---

#### setName 

```cpp
void setName(const std::string & name)
```

Sets the name of the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setValue 

```cpp
void setValue(const std::string & value)
```

Sets the value of the cookie.

According to the cookie specification, the size of the value should not exceed 4 Kbytes.

Note: If value contains whitespace or non-alphanumeric characters, the value should be escaped by calling [escape()](#group__http_1ga5dc9fb3d11c40a91b69c562135a4a5f8) prior to passing it to [setName()](#group__http_1gad8867c1d8c54497acc3b251b9086f73d).

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### setComment 

```cpp
void setComment(const std::string & comment)
```

Sets the comment for the cookie.

Comments are only supported for version 1 cookies.

| Parameter | Type | Description |
|-----------|------|-------------|
| `comment` | `const std::string &` |  |

---

#### setDomain 

```cpp
void setDomain(const std::string & domain)
```

Sets the domain for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `domain` | `const std::string &` |  |

---

#### setPath 

```cpp
void setPath(const std::string & path)
```

Sets the path for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### setSecure 

```cpp
void setSecure(bool secure)
```

Sets the value of the secure flag for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `secure` | `bool` |  |

---

#### setMaxAge 

```cpp
void setMaxAge(int maxAge)
```

Sets the maximum age in seconds for the cookie.

A value of -1 causes the cookie to never expire on the client.

A value of 0 deletes the cookie on the client.

| Parameter | Type | Description |
|-----------|------|-------------|
| `maxAge` | `int` |  |

---

#### setHttpOnly 

```cpp
void setHttpOnly(bool flag)
```

Sets the HttpOnly flag for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### toString 

```cpp
std::string toString() const
```

Returns a std::string representation of the cookie, suitable for use in a Set-Cookie header.

---

#### escape 

```cpp
static std::string escape(const std::string & str)
```

Escapes the given std::string by replacing all non-alphanumeric characters with escape sequences in the form xx, where xx is the hexadecimal character code.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |

---

#### unescape 

```cpp
static std::string unescape(const std::string & str)
```

Unescapes the given std::string by replacing all escape sequences in the form xx with the respective characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |

---

#### getVersion 

```cpp
inline int getVersion() const
```

Returns the version of the cookie, which is either 0 or 1.

---

#### getName 

```cpp
inline const std::string & getName() const
```

Returns the name of the cookie.

---

#### getValue 

```cpp
inline const std::string & getValue() const
```

Returns the value of the cookie.

---

#### getComment 

```cpp
inline const std::string & getComment() const
```

Returns the comment for the cookie.

---

#### getDomain 

```cpp
inline const std::string & getDomain() const
```

Returns the domain for the cookie.

---

#### getPath 

```cpp
inline const std::string & getPath() const
```

Returns the path for the cookie.

---

#### getSecure 

```cpp
inline bool getSecure() const
```

Returns the value of the secure flag for the cookie.

---

#### getMaxAge 

```cpp
inline int getMaxAge() const
```

Returns the maximum age in seconds for the cookie.

---

#### getHttpOnly 

```cpp
inline bool getHttpOnly() const
```

Returns true if the cookie's HttpOnly flag is set.

---

#### _version 

```cpp
int _version
```

---

#### _name 

```cpp
std::string _name
```

---

#### _value 

```cpp
std::string _value
```

---

#### _comment 

```cpp
std::string _comment
```

---

#### _domain 

```cpp
std::string _domain
```

---

#### _path 

```cpp
std::string _path
```

---

#### _secure 

```cpp
bool _secure
```

---

#### _maxAge 

```cpp
int _maxAge
```

---

#### _httpOnly 

```cpp
bool _httpOnly
```

## FormWriter 

> **Extends:** `icy::NVCollection`, `icy::PacketStreamAdapter`, `icy::basic::Startable`
> **Defined in:** `form.h`

[FormWriter](#classicy_1_1http_1_1FormWriter) is a HTTP client connection adapter for writing HTML forms.

This class runs in its own thread so as not to block the event loop while uploading big files. Class members are not synchronized hence they should not be accessed while the form is sending, not that there would be any reason to do so.

### Members

| Name | Description |
|------|-------------|
| [`emitter`](#classicy_1_1http_1_1FormWriter_1ab2466a31197f14952d9591b1d5c4a1ad) | The outgoing packet emitter. |
| [`~FormWriter`](#classicy_1_1http_1_1FormWriter_1ab41f26d58a5eceed4d3dca809b2fffd6) | Destroys the [FormWriter](#classicy_1_1http_1_1FormWriter). |
| [`addPart`](#classicy_1_1http_1_1FormWriter_1aa9e9d1c89a622c3411d899249e66c95f) | Adds an part/attachment (file upload) to the form. |
| [`start`](#classicy_1_1http_1_1FormWriter_1aa5f7896cd6fdc03332ca24ce61a21cc9) | Starts the sending thread. |
| [`stop`](#classicy_1_1http_1_1FormWriter_1ada956df3e4d829e6f8089bcb8131f4b6) | Stops the sending thread. |
| [`complete`](#classicy_1_1http_1_1FormWriter_1ac09b25eb5461de108430eec710d68675) | Returns true if the request is complete. |
| [`cancelled`](#classicy_1_1http_1_1FormWriter_1a9f19254603100e55352b7550d76d5c73) | Returns true if the request is cancelled. |
| [`prepareSubmit`](#classicy_1_1http_1_1FormWriter_1aa5769359cbcdf8addf6da8dc3f45d283) | Prepares the outgoing HTTP request object for submitting the form. |
| [`calculateMultipartContentLength`](#classicy_1_1http_1_1FormWriter_1a98bac9fd0d6a81bfda140833d5c8856d) | Processes the entire stream and calculates the content length. Not used for chunked encoding. |
| [`writeUrl`](#classicy_1_1http_1_1FormWriter_1ae991f982b59ea236144c4afc93a1f212) | Writes "application/x-www-form-urlencoded" encoded data to the client connection. |
| [`writeMultipartChunk`](#classicy_1_1http_1_1FormWriter_1a52a768c3b4f38dd4d626f9d3f0e69c9f) | Writes the next multipart "multipart/form-data" encoded to the client connection. This method is non-blocking, and is suitable for use with the event loop. |
| [`writeAsync`](#classicy_1_1http_1_1FormWriter_1a316ac15771f371e398cd9897cf983675) | Called asynchronously by the [Runner](./doc/api-base.md#classicy_1_1Runner) to write the next message chunk. |
| [`setEncoding`](#classicy_1_1http_1_1FormWriter_1aff90cfef483b9111d1ecd987744e0371) | Sets the encoding used for posting the form. |
| [`encoding`](#classicy_1_1http_1_1FormWriter_1a3d3d1bd0b01df47b5e02abe86cccd03b) | Returns the encoding used for posting the form. |
| [`setBoundary`](#classicy_1_1http_1_1FormWriter_1a385dfb4313fd4b63c90326edb3d8bf2a) | Sets the boundary to use for separating form parts. Must be set before [prepareSubmit()](#classicy_1_1http_1_1FormWriter_1aa5769359cbcdf8addf6da8dc3f45d283) is called. |
| [`boundary`](#classicy_1_1http_1_1FormWriter_1a08c75f60ff2b194f6eca60124cd5ac7c) | Returns the MIME boundary used for writing multipart form data. |
| [`connection`](#classicy_1_1http_1_1FormWriter_1a98c95aa2b0d393861ff17d32dcb28a8e) | The associated HTTP client connection. |
| [`ENCODING_URL`](#classicy_1_1http_1_1FormWriter_1ab1279705132688896d236c8bf2834c73) | "application/x-www-form-urlencoded" |
| [`ENCODING_MULTIPART_FORM`](#classicy_1_1http_1_1FormWriter_1a75a64c9f952d48e6ae3d9b5f0c5505bc) | "multipart/form-data" |
| [`ENCODING_MULTIPART_RELATED`](#classicy_1_1http_1_1FormWriter_1ab0414bdc2f08c691369b5b22147f6067) | "multipart/related" [http://tools.ietf.org/html/rfc2387](http://tools.ietf.org/html/rfc2387) |
| [`create`](#classicy_1_1http_1_1FormWriter_1ad38b14995dac6b4b9b571dbb8b395d5a) | Creates the [FormWriter](#classicy_1_1http_1_1FormWriter) that uses the given connection and encoding type. |
| [`_stream`](#classicy_1_1http_1_1FormWriter_1afc74cd5d30a0746914dbb46425dc577d) |  |
| [`_runner`](#classicy_1_1http_1_1FormWriter_1a4e248609907dafe539ecedc19742fc20) |  |
| [`_encoding`](#classicy_1_1http_1_1FormWriter_1affbac44d4b4e31504c159879ad4aeeea) |  |
| [`_boundary`](#classicy_1_1http_1_1FormWriter_1a6bdb766483b5dc3ec566ba5dd8c62d0a) |  |
| [`_parts`](#classicy_1_1http_1_1FormWriter_1a203d49a219f8dd1db20b68d5c8f7f762) |  |
| [`_filesLength`](#classicy_1_1http_1_1FormWriter_1a80fe080459a5ab2bd023748d81173c4c) |  |
| [`_writeState`](#classicy_1_1http_1_1FormWriter_1a6d4721907f8d77d40d79b97a522c2ac0) |  |
| [`_initial`](#classicy_1_1http_1_1FormWriter_1ae9379de89bcbc2827acc2e98834b592f) |  |
| [`_complete`](#classicy_1_1http_1_1FormWriter_1a0937540b2e3ebfa1b33cdb955384cbb3) |  |
| [`FormWriter`](#classicy_1_1http_1_1FormWriter_1adf5052c2ffe130f08be80851af30a8bf) | Creates the [FormWriter](#classicy_1_1http_1_1FormWriter) that uses the given encoding. |
| [`FormWriter`](#classicy_1_1http_1_1FormWriter_1ab3749cfb874876b80cab0409f6dfada9) |  |
| [`operator=`](#classicy_1_1http_1_1FormWriter_1ab01743f0fd0b5cd020c691f58d90df0f) |  |
| [`writePartHeader`](#classicy_1_1http_1_1FormWriter_1a284dfa7aaa9df8733b3478c50f7cc1aa) | Writes the message boundary std::string, followed by the message header to the output stream. |
| [`writeEnd`](#classicy_1_1http_1_1FormWriter_1ae821aa326c6156c6ab81809b75bfdff9) | Writes the final boundary std::string to the output stream. |
| [`updateProgress`](#classicy_1_1http_1_1FormWriter_1a8d47d8f0ecc8dfe4cab1ce61c90a091f) | Updates the upload progress via the associated [ConnectionStream](#classicy_1_1http_1_1ConnectionStream) object. |

---

#### emitter 

```cpp
[PacketSignal](./doc/api-base.md#group__base_1ga979ef14c59bd1db94d095fed71247f79) emitter
```

The outgoing packet emitter.

---

#### ~FormWriter 

```cpp
virtual ~FormWriter()
```

Destroys the [FormWriter](#classicy_1_1http_1_1FormWriter).

---

#### addPart 

```cpp
void addPart(const std::string & name, [FormPart](#classicy_1_1http_1_1FormPart) * part)
```

Adds an part/attachment (file upload) to the form.

The form takes ownership of the [FilePart](#classicy_1_1http_1_1FilePart) and deletes it when it is no longer needed. The part will only be sent if the encoding set for the form is "multipart/form-data"

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `part` | `[FormPart](#classicy_1_1http_1_1FormPart) *` |  |

---

#### start 

```cpp
virtual void start()
```

Starts the sending thread.

---

#### stop 

```cpp
virtual void stop()
```

Stops the sending thread.

---

#### complete 

```cpp
bool complete() const
```

Returns true if the request is complete.

---

#### cancelled 

```cpp
bool cancelled() const
```

Returns true if the request is cancelled.

---

#### prepareSubmit 

```cpp
void prepareSubmit()
```

Prepares the outgoing HTTP request object for submitting the form.

---

#### calculateMultipartContentLength 

```cpp
uint64_t calculateMultipartContentLength()
```

Processes the entire stream and calculates the content length. Not used for chunked encoding.

---

#### writeUrl 

```cpp
void writeUrl(std::ostream & ostr)
```

Writes "application/x-www-form-urlencoded" encoded data to the client connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

#### writeMultipartChunk 

```cpp
void writeMultipartChunk()
```

Writes the next multipart "multipart/form-data" encoded to the client connection. This method is non-blocking, and is suitable for use with the event loop.

---

#### writeAsync 

```cpp
void writeAsync()
```

Called asynchronously by the [Runner](./doc/api-base.md#classicy_1_1Runner) to write the next message chunk.

---

#### setEncoding 

```cpp
void setEncoding(const std::string & encoding)
```

Sets the encoding used for posting the form.

| Parameter | Type | Description |
|-----------|------|-------------|
| `encoding` | `const std::string &` |  |

---

#### encoding 

```cpp
const std::string & encoding() const
```

Returns the encoding used for posting the form.

---

#### setBoundary 

```cpp
void setBoundary(const std::string & boundary)
```

Sets the boundary to use for separating form parts. Must be set before [prepareSubmit()](#classicy_1_1http_1_1FormWriter_1aa5769359cbcdf8addf6da8dc3f45d283) is called.

| Parameter | Type | Description |
|-----------|------|-------------|
| `boundary` | `const std::string &` |  |

---

#### boundary 

```cpp
const std::string & boundary() const
```

Returns the MIME boundary used for writing multipart form data.

---

#### connection 

```cpp
[ConnectionStream](#classicy_1_1http_1_1ConnectionStream) & connection()
```

The associated HTTP client connection.

---

#### ENCODING_URL 

```cpp
const char * ENCODING_URL = "application/x-www-form-urlencoded"
```

"application/x-www-form-urlencoded"

---

#### ENCODING_MULTIPART_FORM 

```cpp
const char * ENCODING_MULTIPART_FORM = "multipart/form-data"
```

"multipart/form-data"

---

#### ENCODING_MULTIPART_RELATED 

```cpp
const char * ENCODING_MULTIPART_RELATED = "multipart/related"
```

"multipart/related" [http://tools.ietf.org/html/rfc2387](http://tools.ietf.org/html/rfc2387)

---

#### create 

```cpp
static [FormWriter](#classicy_1_1http_1_1FormWriter) * create([ConnectionStream](#classicy_1_1http_1_1ConnectionStream) & conn, const std::string & encoding)
```

Creates the [FormWriter](#classicy_1_1http_1_1FormWriter) that uses the given connection and encoding type.

Encoding must be either "application/x-www-form-urlencoded" (which is the default) or "multipart/form-data".

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ConnectionStream](#classicy_1_1http_1_1ConnectionStream) &` |  |
| `encoding` | `const std::string &` |  |

---

#### _stream 

```cpp
[ConnectionStream](#classicy_1_1http_1_1ConnectionStream) & _stream
```

---

#### _runner 

```cpp
std::shared_ptr< [Runner](./doc/api-base.md#classicy_1_1Runner) > _runner
```

---

#### _encoding 

```cpp
std::string _encoding
```

---

#### _boundary 

```cpp
std::string _boundary
```

---

#### _parts 

```cpp
[PartQueue](#classicy_1_1http_1_1FormWriter_1a29ed3d4115741fce5e6460464f9ea1c3) _parts
```

---

#### _filesLength 

```cpp
uint64_t _filesLength
```

---

#### _writeState 

```cpp
int _writeState
```

---

#### _initial 

```cpp
bool _initial
```

---

#### _complete 

```cpp
bool _complete
```

---

#### FormWriter 

```cpp
FormWriter([ConnectionStream](#classicy_1_1http_1_1ConnectionStream) & conn, std::shared_ptr< [Runner](./doc/api-base.md#classicy_1_1Runner) > runner, const std::string & encoding)
```

Creates the [FormWriter](#classicy_1_1http_1_1FormWriter) that uses the given encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ConnectionStream](#classicy_1_1http_1_1ConnectionStream) &` |  |
| `runner` | `std::shared_ptr< [Runner](./doc/api-base.md#classicy_1_1Runner) >` |  |
| `encoding` | `const std::string &` |  |

---

#### FormWriter 

```cpp
FormWriter(const [FormWriter](#classicy_1_1http_1_1FormWriter) &) = delete
```

---

#### operator= 

```cpp
[FormWriter](#classicy_1_1http_1_1FormWriter) & operator=(const [FormWriter](#classicy_1_1http_1_1FormWriter) &) = delete
```

---

#### writePartHeader 

```cpp
void writePartHeader(const [NVCollection](./doc/api-base.md#classicy_1_1NVCollection) & header, std::ostream & ostr)
```

Writes the message boundary std::string, followed by the message header to the output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `const [NVCollection](./doc/api-base.md#classicy_1_1NVCollection) &` |  |
| `ostr` | `std::ostream &` |  |

---

#### writeEnd 

```cpp
void writeEnd(std::ostream & ostr)
```

Writes the final boundary std::string to the output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

#### updateProgress 

```cpp
virtual void updateProgress(int nread)
```

Updates the upload progress via the associated [ConnectionStream](#classicy_1_1http_1_1ConnectionStream) object.

| Parameter | Type | Description |
|-----------|------|-------------|
| `nread` | `int` |  |

## FormPart 

> **Subclasses:** `icy::http::FilePart`, `icy::http::StringPart`
> **Defined in:** `form.h`

An implementation of [FormPart](#classicy_1_1http_1_1FormPart).

### Members

| Name | Description |
|------|-------------|
| [`FormPart`](#classicy_1_1http_1_1FormPart_1addfd1e7e0464b40e773a806e1d30ec4f) | Creates the [FormPart](#classicy_1_1http_1_1FormPart) with the given MIME type. |
| [`~FormPart`](#classicy_1_1http_1_1FormPart_1a1771d5947b6f1c8d4595f70d6d2ba79a) | Destroys the [FormPart](#classicy_1_1http_1_1FormPart). |
| [`reset`](#classicy_1_1http_1_1FormPart_1ac1b2db21062b5770195b6217da2ee7f6) | Reset the internal state and write position to the start. |
| [`writeChunk`](#classicy_1_1http_1_1FormPart_1a10b82f2f86dbd9dd27fdd3321d4d09f0) | Writes a form data chunk to the given HTTP client connection. Returns true if there is more data to be written. |
| [`write`](#classicy_1_1http_1_1FormPart_1afcc4b878645c0b9b1a25986827f8002a) | Writes the form data to the given HTTP client connection. |
| [`write`](#classicy_1_1http_1_1FormPart_1a9364dc83c243a4334a3157f7d7856bca) | Writes the form data to the given output stream. |
| [`headers`](#classicy_1_1http_1_1FormPart_1a16a42c613efc74d90591aad48992c5f2) | Returns a [NVCollection](./doc/api-base.md#classicy_1_1NVCollection) containing additional header fields for the part. |
| [`initialWrite`](#classicy_1_1http_1_1FormPart_1a0aeef9590266d57540c1c381f81bc3d9) | Returns true if this is the initial write. |
| [`contentType`](#classicy_1_1http_1_1FormPart_1a5b33a860f6532933b853ef1bb79b53fe) | Returns the MIME type for this part or attachment. |
| [`length`](#classicy_1_1http_1_1FormPart_1ab2077d4568fc862c92a28f690553670e) | Returns the length of the current part. |
| [`_contentType`](#classicy_1_1http_1_1FormPart_1a99b49cdefb7941eaa59014a6ffac990c) |  |
| [`_length`](#classicy_1_1http_1_1FormPart_1aafe2123975b4ceb0be87e6ee57593d11) |  |
| [`_headers`](#classicy_1_1http_1_1FormPart_1a1610eaa082d3154399710dba0bc4b1e4) |  |
| [`_initialWrite`](#classicy_1_1http_1_1FormPart_1a4e0d100858d98915b243591998133d4a) |  |

---

#### FormPart 

```cpp
FormPart(const std::string & contentType)
```

Creates the [FormPart](#classicy_1_1http_1_1FormPart) with the given MIME type.

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |

---

#### ~FormPart 

```cpp
virtual ~FormPart()
```

Destroys the [FormPart](#classicy_1_1http_1_1FormPart).

---

#### reset 

```cpp
virtual void reset()
```

Reset the internal state and write position to the start.

---

#### writeChunk 

```cpp
bool writeChunk([FormWriter](#classicy_1_1http_1_1FormWriter) & writer)
```

Writes a form data chunk to the given HTTP client connection. Returns true if there is more data to be written.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#classicy_1_1http_1_1FormWriter) &` |  |

---

#### write 

```cpp
void write([FormWriter](#classicy_1_1http_1_1FormWriter) & writer)
```

Writes the form data to the given HTTP client connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#classicy_1_1http_1_1FormWriter) &` |  |

---

#### write 

```cpp
void write(std::ostream & ostr)
```

Writes the form data to the given output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

#### headers 

```cpp
[NVCollection](./doc/api-base.md#classicy_1_1NVCollection) & headers()
```

Returns a [NVCollection](./doc/api-base.md#classicy_1_1NVCollection) containing additional header fields for the part.

---

#### initialWrite 

```cpp
virtual bool initialWrite() const
```

Returns true if this is the initial write.

---

#### contentType 

```cpp
const std::string & contentType() const
```

Returns the MIME type for this part or attachment.

---

#### length 

```cpp
uint64_t length() const
```

Returns the length of the current part.

---

#### _contentType 

```cpp
std::string _contentType
```

---

#### _length 

```cpp
uint64_t _length
```

---

#### _headers 

```cpp
[NVCollection](./doc/api-base.md#classicy_1_1NVCollection) _headers
```

---

#### _initialWrite 

```cpp
bool _initialWrite
```

## FilePart 

> **Extends:** `icy::http::FormPart`
> **Defined in:** `form.h`

An implementation of [FilePart](#classicy_1_1http_1_1FilePart) for plain files.

### Members

| Name | Description |
|------|-------------|
| [`FilePart`](#classicy_1_1http_1_1FilePart_1a3965f033a1a66974801bf1ecb2a65e43) | Creates the [FilePart](#classicy_1_1http_1_1FilePart) for the given path. |
| [`FilePart`](#classicy_1_1http_1_1FilePart_1a0543f767567ba948d3f02b3de6020011) | Creates the [FilePart](#classicy_1_1http_1_1FilePart) for the given path and MIME type. |
| [`FilePart`](#classicy_1_1http_1_1FilePart_1a18dea4dba60832a3b9ab08fd1fc0a6fe) | Creates the [FilePart](#classicy_1_1http_1_1FilePart) for the given path and MIME type. The given filename is used as part filename (see [filename()](#classicy_1_1http_1_1FilePart_1ad2765df66b173afe804630d0294b57d2)) only. |
| [`~FilePart`](#classicy_1_1http_1_1FilePart_1ad1cba409e3ebbb99a8d90a70eaf68938) | Destroys the [FilePart](#classicy_1_1http_1_1FilePart). |
| [`open`](#classicy_1_1http_1_1FilePart_1a3254f30104cd2cba62847b430ba1a6b7) | Opens the file. |
| [`reset`](#classicy_1_1http_1_1FilePart_1a8e47a18cf77f0ddfbbdb95e4be54502d) | Reset the internal state and write position to the start. |
| [`writeChunk`](#classicy_1_1http_1_1FilePart_1a79720d0fc2bd52c655162db91b154605) | Writes a form data chunk to the given HTTP client connection. Returns true if there is more data to be written. |
| [`write`](#classicy_1_1http_1_1FilePart_1a4aed2e1707e335c71c14a974aeaad47e) | Writes the form data to the given HTTP client connection. |
| [`write`](#classicy_1_1http_1_1FilePart_1ac83ece1e9f7236b20b7c17fe885ab6d9) | Writes the form data to the given output stream. |
| [`filename`](#classicy_1_1http_1_1FilePart_1ad2765df66b173afe804630d0294b57d2) | Returns the filename portion of the path. |
| [`stream`](#classicy_1_1http_1_1FilePart_1a8ef4a43e06b5e9aecd8e008fe06bbd41) | Returns the file input stream. |
| [`length`](#classicy_1_1http_1_1FilePart_1a7ffcef3b5fb5e96d86e6f8de0e84aab1) | Returns the length of the current part. |
| [`_path`](#classicy_1_1http_1_1FilePart_1ac84b6cb214c304f0685e3db164c5e9d4) |  |
| [`_filename`](#classicy_1_1http_1_1FilePart_1a5a022ac797b9194b540ce9e5128f54d4) |  |
| [`_istr`](#classicy_1_1http_1_1FilePart_1ac44819c6c5d495ad370e0773ba15f341) |  |
| [`_fileSize`](#classicy_1_1http_1_1FilePart_1a732d6ec638877661788fcd8ffbf7d23a) |  |

---

#### FilePart 

```cpp
FilePart(const std::string & path)
```

Creates the [FilePart](#classicy_1_1http_1_1FilePart) for the given path.

The MIME type is set to application/octet-stream.

Throws an FileException if the file cannot be opened.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### FilePart 

```cpp
FilePart(const std::string & path, const std::string & contentType)
```

Creates the [FilePart](#classicy_1_1http_1_1FilePart) for the given path and MIME type.

Throws an FileException if the file cannot be opened.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `contentType` | `const std::string &` |  |

---

#### FilePart 

```cpp
FilePart(const std::string & path, const std::string & filename, const std::string & contentType)
```

Creates the [FilePart](#classicy_1_1http_1_1FilePart) for the given path and MIME type. The given filename is used as part filename (see [filename()](#classicy_1_1http_1_1FilePart_1ad2765df66b173afe804630d0294b57d2)) only.

Throws an FileException if the file cannot be opened.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `filename` | `const std::string &` |  |
| `contentType` | `const std::string &` |  |

---

#### ~FilePart 

```cpp
virtual ~FilePart()
```

Destroys the [FilePart](#classicy_1_1http_1_1FilePart).

---

#### open 

```cpp
virtual void open()
```

Opens the file.

Throws an FileException if the file cannot be opened.

---

#### reset 

```cpp
virtual void reset()
```

Reset the internal state and write position to the start.

---

#### writeChunk 

```cpp
virtual bool writeChunk([FormWriter](#classicy_1_1http_1_1FormWriter) & writer)
```

Writes a form data chunk to the given HTTP client connection. Returns true if there is more data to be written.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#classicy_1_1http_1_1FormWriter) &` |  |

---

#### write 

```cpp
virtual void write([FormWriter](#classicy_1_1http_1_1FormWriter) & writer)
```

Writes the form data to the given HTTP client connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#classicy_1_1http_1_1FormWriter) &` |  |

---

#### write 

```cpp
virtual void write(std::ostream & ostr)
```

Writes the form data to the given output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

#### filename 

```cpp
const std::string & filename() const
```

Returns the filename portion of the path.

---

#### stream 

```cpp
std::ifstream & stream()
```

Returns the file input stream.

---

#### length 

```cpp
virtual uint64_t length() const
```

Returns the length of the current part.

---

#### _path 

```cpp
std::string _path
```

---

#### _filename 

```cpp
std::string _filename
```

---

#### _istr 

```cpp
std::ifstream _istr
```

---

#### _fileSize 

```cpp
uint64_t _fileSize
```

## StringPart 

> **Extends:** `icy::http::FormPart`
> **Defined in:** `form.h`

An implementation of [StringPart](#classicy_1_1http_1_1StringPart) for plain files.

### Members

| Name | Description |
|------|-------------|
| [`StringPart`](#classicy_1_1http_1_1StringPart_1a0b5884a2d863ceb7cce47c351d4de3cf) | Creates the [StringPart](#classicy_1_1http_1_1StringPart) for the given string. |
| [`StringPart`](#classicy_1_1http_1_1StringPart_1a54d19c7cbabf1a1052dcdbb9c7bce61c) | Creates the [StringPart](#classicy_1_1http_1_1StringPart) for the given string and MIME type. |
| [`~StringPart`](#classicy_1_1http_1_1StringPart_1a377cfef62f1d4c5088d4dc61b9a2689e) | Destroys the [StringPart](#classicy_1_1http_1_1StringPart). |
| [`writeChunk`](#classicy_1_1http_1_1StringPart_1a1a8a1a3f648f0edb715cf5ac42275f14) | Writes a form data chunk to the given HTTP client connection. Returns true if there is more data to be written. |
| [`write`](#classicy_1_1http_1_1StringPart_1a73600ffb13f790118e3ae97ca028f88d) | Writes the form data to the given HTTP client connection. |
| [`write`](#classicy_1_1http_1_1StringPart_1a346d908a6b89c22a370ae88c616d48a1) | Writes the form data to the given output stream. |
| [`length`](#classicy_1_1http_1_1StringPart_1a25bbb142d981ca318d46c212d56a7faa) | Returns the length of the current part. |
| [`_data`](#classicy_1_1http_1_1StringPart_1a96f0a59bb5b72c46ec183f3e490b9071) |  |

---

#### StringPart 

```cpp
StringPart(const std::string & path)
```

Creates the [StringPart](#classicy_1_1http_1_1StringPart) for the given string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### StringPart 

```cpp
StringPart(const std::string & data, const std::string & contentType)
```

Creates the [StringPart](#classicy_1_1http_1_1StringPart) for the given string and MIME type.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const std::string &` |  |
| `contentType` | `const std::string &` |  |

---

#### ~StringPart 

```cpp
virtual ~StringPart()
```

Destroys the [StringPart](#classicy_1_1http_1_1StringPart).

---

#### writeChunk 

```cpp
virtual bool writeChunk([FormWriter](#classicy_1_1http_1_1FormWriter) & writer)
```

Writes a form data chunk to the given HTTP client connection. Returns true if there is more data to be written.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#classicy_1_1http_1_1FormWriter) &` |  |

---

#### write 

```cpp
virtual void write([FormWriter](#classicy_1_1http_1_1FormWriter) & writer)
```

Writes the form data to the given HTTP client connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#classicy_1_1http_1_1FormWriter) &` |  |

---

#### write 

```cpp
virtual void write(std::ostream & ostr)
```

Writes the form data to the given output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

#### length 

```cpp
virtual uint64_t length() const
```

Returns the length of the current part.

---

#### _data 

```cpp
std::string _data
```

## Message 

> **Extends:** `icy::NVCollection`
> **Subclasses:** `icy::http::Request`, `icy::http::Response`
> **Defined in:** `message.h`

The base class for [Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) and [Response](./doc/api-undefined.md#classicy_1_1http_1_1Response).

Defines the common properties of all HTTP messages. These are version, content length, content type and transfer encoding.

### Members

| Name | Description |
|------|-------------|
| [`setVersion`](#group__http_1ga723cfacd1de2e59acd576eb63010d6fc) | Sets the HTTP version for this message. |
| [`getVersion`](#group__http_1gaeaf5238f777cf31201cb8bc342283221) | Returns the HTTP version for this message. |
| [`setContentLength`](#group__http_1ga831735515c44e91de1ffc304f30c9d49) | Sets the Content-Length header. |
| [`getContentLength`](#group__http_1ga9fbd841b869e2f71c976f26e42848ed7) | Returns the content length for this message, which may be UNKNOWN_CONTENT_LENGTH if no Content-Length header is present. |
| [`hasContentLength`](#group__http_1gad90dd4a412c227b05b071b26c71aaf35) | Returns true if a Content-Length header is present. |
| [`setTransferEncoding`](#group__http_1ga44b4439cc2b005a82ad7d118a53e0e0b) | Sets the transfer encoding for this message. |
| [`getTransferEncoding`](#group__http_1ga886838e78abe992b994489cd5809285f) | Returns the transfer encoding used for this message. |
| [`setChunkedTransferEncoding`](#group__http_1ga1093aa082297c1b15f018f7cda41096e) | If flag is true, sets the Transfer-Encoding header to chunked. Otherwise, removes the Transfer-Encoding header. |
| [`isChunkedTransferEncoding`](#group__http_1gac8677cc8b307cf7172a3299116acad30) | Returns true if the Transfer-Encoding header is set and its value is chunked. |
| [`setContentType`](#group__http_1ga0bc22314e98367be2ef4a72ddc626b94) | Sets the content type for this message. |
| [`getContentType`](#group__http_1gaab4a46e024093c712047548b6297f5f1) | Returns the content type for this message. |
| [`setKeepAlive`](#group__http_1ga093099a4721005e4705804f0f9aa096e) | Sets the value of the [Connection](#classicy_1_1http_1_1Connection) header field. |
| [`getKeepAlive`](#group__http_1ga12e6a3de621471dbf9fa8576902f972c) | Returns true if |
| [`write`](#group__http_1ga9ebc2305bb2173419345a1d2e21efd19) | Writes the message header to the given output stream. |
| [`write`](#group__http_1ga2e51af3e0cfe140aa95411b1c14e22a9) | Writes the message header to the given output string. |
| [`Message`](#group__http_1ga928fa708b89d544c3b6f5039b787bb7e) | Creates the [Message](#classicy_1_1http_1_1Message) with version HTTP/1.0. |
| [`Message`](#group__http_1gabcb10a4c47593f94c9431c0f21cbf114) | Creates the [Message](#classicy_1_1http_1_1Message) and sets the version. |
| [`~Message`](#group__http_1ga71ef669503290a7f386ee2dc91e09b53) | Destroys the [Message](#classicy_1_1http_1_1Message). |
| [`HTTP_1_0`](#group__http_1ga8c0f2b43a4b1c18ba98968d142739a37) |  |
| [`HTTP_1_1`](#group__http_1gaa38f6879b68e53a02cdee386dbf340f9) |  |
| [`IDENTITY_TRANSFER_ENCODING`](#group__http_1ga627d303cd0ebef230387cd3b6ddad541) |  |
| [`CHUNKED_TRANSFER_ENCODING`](#group__http_1gad3eaedbb64235c47e3c7b6faef5a2e35) |  |
| [`UNKNOWN_CONTENT_LENGTH`](#group__http_1ga256c2a1ecf1f2aa9ef6207b5c41c81f7) |  |
| [`UNKNOWN_CONTENT_TYPE`](#group__http_1gaacddb05e323c1d3fd83ee313be6299ea) |  |
| [`CONTENT_LENGTH`](#group__http_1gab76ed9f16753de667e4f9597e0c4a330) |  |
| [`CONTENT_TYPE`](#group__http_1ga4c8e78b1c7b3c89e89b552b4b8988ef6) |  |
| [`TRANSFER_ENCODING`](#group__http_1ga5504149b9e387202df43caa69891bab0) |  |
| [`CONNECTION`](#group__http_1ga53ce770c798004418fdea2d965337af8) |  |
| [`CONNECTION_KEEP_ALIVE`](#group__http_1gaa3b3ded50adcca5eb9c5af3ea1ec19b2) |  |
| [`CONNECTION_CLOSE`](#group__http_1ga5a7acda79eab95b2987e9f96162e0a77) |  |
| [`EMPTY`](#group__http_1ga51519fdace0bc2b729421d86fedbfe18) |  |
| [`_version`](#classicy_1_1http_1_1Message_1aa2feba1d1b0438c328d36653d09b05d1) |  |

---

#### setVersion 

```cpp
void setVersion(const std::string & version)
```

Sets the HTTP version for this message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### getVersion 

```cpp
const std::string & getVersion() const
```

Returns the HTTP version for this message.

---

#### setContentLength 

```cpp
void setContentLength(uint64_t length)
```

Sets the Content-Length header.

If length is UNKNOWN_CONTENT_LENGTH, removes the Content-Length header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `length` | `uint64_t` |  |

---

#### getContentLength 

```cpp
uint64_t getContentLength() const
```

Returns the content length for this message, which may be UNKNOWN_CONTENT_LENGTH if no Content-Length header is present.

---

#### hasContentLength 

```cpp
bool hasContentLength() const
```

Returns true if a Content-Length header is present.

---

#### setTransferEncoding 

```cpp
void setTransferEncoding(const std::string & transferEncoding)
```

Sets the transfer encoding for this message.

The value should be either IDENTITY_TRANSFER_CODING or CHUNKED_TRANSFER_CODING.

| Parameter | Type | Description |
|-----------|------|-------------|
| `transferEncoding` | `const std::string &` |  |

---

#### getTransferEncoding 

```cpp
const std::string & getTransferEncoding() const
```

Returns the transfer encoding used for this message.

Normally, this is the value of the Transfer-Encoding header field. If no such field is present, returns IDENTITY_TRANSFER_CODING.

---

#### setChunkedTransferEncoding 

```cpp
void setChunkedTransferEncoding(bool flag)
```

If flag is true, sets the Transfer-Encoding header to chunked. Otherwise, removes the Transfer-Encoding header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### isChunkedTransferEncoding 

```cpp
bool isChunkedTransferEncoding() const
```

Returns true if the Transfer-Encoding header is set and its value is chunked.

---

#### setContentType 

```cpp
void setContentType(const std::string & contentType)
```

Sets the content type for this message.

Specify NO_CONTENT_TYPE to remove the Content-Type header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |

---

#### getContentType 

```cpp
const std::string & getContentType() const
```

Returns the content type for this message.

If no Content-Type header is present, returns UNKNOWN_CONTENT_TYPE.

---

#### setKeepAlive 

```cpp
void setKeepAlive(bool keepAlive)
```

Sets the value of the [Connection](#classicy_1_1http_1_1Connection) header field.

The value is set to "Keep-Alive" if keepAlive is true, or to "Close" otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `keepAlive` | `bool` |  |

---

#### getKeepAlive 

```cpp
bool getKeepAlive() const
```

Returns true if

* the message has a [Connection](#classicy_1_1http_1_1Connection) header field and its value is "Keep-Alive"

* the message is a HTTP/1.1 message and not [Connection](#classicy_1_1http_1_1Connection) header is set Returns false otherwise.

---

#### write 

```cpp
virtual void write(std::ostream & ostr) const
```

Writes the message header to the given output stream.

The format is one name-value pair per line, with name and value separated by a colon and lines delimited by a carriage return and a linefeed character. See RFC 2822 for details.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

#### write 

```cpp
virtual void write(std::string & str) const
```

Writes the message header to the given output string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |

---

#### Message 

```cpp
Message()
```

Creates the [Message](#classicy_1_1http_1_1Message) with version HTTP/1.0.

---

#### Message 

```cpp
Message(const std::string & version)
```

Creates the [Message](#classicy_1_1http_1_1Message) and sets the version.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### ~Message 

```cpp
virtual ~Message()
```

Destroys the [Message](#classicy_1_1http_1_1Message).

---

#### HTTP_1_0 

```cpp
const std::string HTTP_1_0 = "HTTP/1.0"
```

---

#### HTTP_1_1 

```cpp
const std::string HTTP_1_1 = "HTTP/1.1"
```

---

#### IDENTITY_TRANSFER_ENCODING 

```cpp
const std::string IDENTITY_TRANSFER_ENCODING = "identity"
```

---

#### CHUNKED_TRANSFER_ENCODING 

```cpp
const std::string CHUNKED_TRANSFER_ENCODING = "chunked"
```

---

#### UNKNOWN_CONTENT_LENGTH 

```cpp
const int UNKNOWN_CONTENT_LENGTH = -1
```

---

#### UNKNOWN_CONTENT_TYPE 

```cpp
const std::string UNKNOWN_CONTENT_TYPE
```

---

#### CONTENT_LENGTH 

```cpp
const std::string CONTENT_LENGTH = "Content-Length"
```

---

#### CONTENT_TYPE 

```cpp
const std::string CONTENT_TYPE = "Content-Type"
```

---

#### TRANSFER_ENCODING 

```cpp
const std::string TRANSFER_ENCODING = "Transfer-Encoding"
```

---

#### CONNECTION 

```cpp
const std::string CONNECTION = "Connection"
```

---

#### CONNECTION_KEEP_ALIVE 

```cpp
const std::string CONNECTION_KEEP_ALIVE = "Keep-Alive"
```

---

#### CONNECTION_CLOSE 

```cpp
const std::string CONNECTION_CLOSE = "Close"
```

---

#### EMPTY 

```cpp
const std::string EMPTY
```

---

#### _version 

```cpp
std::string _version
```

## ChunkedAdapter 

> **Extends:** `icy::PacketProcessor`
> **Defined in:** `packetizers.h`

HTTP chunked transfer encoding adapter for streaming responses.

### Members

| Name | Description |
|------|-------------|
| [`connection`](#classicy_1_1http_1_1ChunkedAdapter_1aab1ffaf4caf52c43f9a886f613626ee7) |  |
| [`contentType`](#classicy_1_1http_1_1ChunkedAdapter_1a42c0bc9dd9c269c4cb07f4312757e27b) |  |
| [`frameSeparator`](#classicy_1_1http_1_1ChunkedAdapter_1a1ab13fa89e2582407844aeff3c47b3a9) |  |
| [`initial`](#classicy_1_1http_1_1ChunkedAdapter_1a8c694d7668b8ac9062561d57f764c8df) |  |
| [`nocopy`](#classicy_1_1http_1_1ChunkedAdapter_1a35f74aabaea9e37fb8b90f8880fa9e15) |  |
| [`emitter`](#classicy_1_1http_1_1ChunkedAdapter_1abcc47b70a4402dcdf6544143881fed7d) |  |
| [`ChunkedAdapter`](#classicy_1_1http_1_1ChunkedAdapter_1a38782e1645dd6fc4f62637cc38bf5cbd) |  |
| [`ChunkedAdapter`](#classicy_1_1http_1_1ChunkedAdapter_1a0992d011faef2ac9b957753eccb9523f) |  |
| [`~ChunkedAdapter`](#classicy_1_1http_1_1ChunkedAdapter_1abf8b0cdc814e1c8f0c8f98d8afdf1397) |  |
| [`emitHeader`](#classicy_1_1http_1_1ChunkedAdapter_1a994d502823366aaef2833945c6f9d555) | Sets HTTP headers for the initial response. This method must not include the final carriage return. |
| [`process`](#classicy_1_1http_1_1ChunkedAdapter_1a2d56caf2f5823f852b640663968d57f4) | This method performs processing on the given packet and emits the result. |

---

#### connection 

```cpp
[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) connection
```

---

#### contentType 

```cpp
std::string contentType
```

---

#### frameSeparator 

```cpp
std::string frameSeparator
```

---

#### initial 

```cpp
bool initial
```

---

#### nocopy 

```cpp
bool nocopy
```

---

#### emitter 

```cpp
[PacketSignal](./doc/api-base.md#group__base_1ga979ef14c59bd1db94d095fed71247f79) emitter
```

---

#### ChunkedAdapter 

```cpp
inline ChunkedAdapter([Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) connection, const std::string & frameSeparator, bool nocopy)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb)` |  |
| `frameSeparator` | `const std::string &` |  |
| `nocopy` | `bool` |  |

---

#### ChunkedAdapter 

```cpp
inline ChunkedAdapter(const std::string & contentType, const std::string & frameSeparator, bool nocopy)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |
| `frameSeparator` | `const std::string &` |  |
| `nocopy` | `bool` |  |

---

#### ~ChunkedAdapter 

```cpp
virtual inline ~ChunkedAdapter()
```

---

#### emitHeader 

```cpp
virtual inline void emitHeader()
```

Sets HTTP headers for the initial response. This method must not include the final carriage return.

---

#### process 

```cpp
virtual inline void process([IPacket](./doc/api-base.md#classicy_1_1IPacket) & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](./doc/api-base.md#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `[IPacket](./doc/api-base.md#classicy_1_1IPacket) &` |  |

## MultipartAdapter 

> **Extends:** `icy::PacketProcessor`
> **Defined in:** `packetizers.h`

HTTP multipart encoding adapter for multipart/x-mixed-replace streaming.

### Members

| Name | Description |
|------|-------------|
| [`connection`](#classicy_1_1http_1_1MultipartAdapter_1a907005a65c54c74e58cdba7c529aa107) |  |
| [`contentType`](#classicy_1_1http_1_1MultipartAdapter_1a28482bfd82b764ab36ebba40465d6bd8) |  |
| [`isBase64`](#classicy_1_1http_1_1MultipartAdapter_1a9d95a8cdd1e6a1d164aeaba94b986c3f) |  |
| [`initial`](#classicy_1_1http_1_1MultipartAdapter_1a7c8957e88432dbaad3f86ae512c3e23e) |  |
| [`emitter`](#classicy_1_1http_1_1MultipartAdapter_1a73a88cc955c203e10227dc1e72319fc6) |  |
| [`MultipartAdapter`](#classicy_1_1http_1_1MultipartAdapter_1a299d4d132f1778bfadc62dff2a0ed210) |  |
| [`MultipartAdapter`](#classicy_1_1http_1_1MultipartAdapter_1a3a4c616e3dbe183d1ba9f08043a47987) |  |
| [`~MultipartAdapter`](#classicy_1_1http_1_1MultipartAdapter_1aa60e5cd062c7768f7636a851f5154076) |  |
| [`emitHeader`](#classicy_1_1http_1_1MultipartAdapter_1a5c48095fcace4608150c12d3e918a4b7) |  |
| [`emitChunkHeader`](#classicy_1_1http_1_1MultipartAdapter_1a98b86bd888dc6d41962ab88c46d3729d) | Sets HTTP header for the current chunk. |
| [`process`](#classicy_1_1http_1_1MultipartAdapter_1aafa137c5fc3cfa81a0ed7bb3de2ba3c4) | This method performs processing on the given packet and emits the result. |

---

#### connection 

```cpp
[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) connection
```

---

#### contentType 

```cpp
std::string contentType
```

---

#### isBase64 

```cpp
bool isBase64
```

---

#### initial 

```cpp
bool initial
```

---

#### emitter 

```cpp
[PacketSignal](./doc/api-base.md#group__base_1ga979ef14c59bd1db94d095fed71247f79) emitter
```

---

#### MultipartAdapter 

```cpp
inline MultipartAdapter([Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb) connection, bool base64)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection::Ptr](#classicy_1_1http_1_1Connection_1a2143146501601ea9816068f83731e1fb)` |  |
| `base64` | `bool` |  |

---

#### MultipartAdapter 

```cpp
inline MultipartAdapter(const std::string & contentType, bool base64)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |
| `base64` | `bool` |  |

---

#### ~MultipartAdapter 

```cpp
virtual inline ~MultipartAdapter()
```

---

#### emitHeader 

```cpp
virtual inline void emitHeader()
```

---

#### emitChunkHeader 

```cpp
virtual inline void emitChunkHeader()
```

Sets HTTP header for the current chunk.

---

#### process 

```cpp
virtual inline void process([IPacket](./doc/api-base.md#classicy_1_1IPacket) & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](./doc/api-base.md#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `[IPacket](./doc/api-base.md#classicy_1_1IPacket) &` |  |

## ParserObserver 

> **Subclasses:** `icy::http::ConnectionAdapter`
> **Defined in:** `parser.h`

Abstract observer interface for HTTP parser events.

### Members

| Name | Description |
|------|-------------|
| [`onParserHeader`](#classicy_1_1http_1_1ParserObserver_1a510985b7152fedee52ebb93488faf6c7) |  |
| [`onParserHeadersEnd`](#classicy_1_1http_1_1ParserObserver_1a7988d91facef489ffb4f39419a788f13) |  |
| [`onParserChunk`](#classicy_1_1http_1_1ParserObserver_1a2dca049067fa3587f1ab3e1756b00202) |  |
| [`onParserEnd`](#classicy_1_1http_1_1ParserObserver_1ae2b96db1a35fd47c09a4d1b9c2eca40c) |  |
| [`onParserError`](#classicy_1_1http_1_1ParserObserver_1a7f55842368057de8a80ecf698f19bf1c) |  |

---

#### onParserHeader 

```cpp
void onParserHeader(const std::string & name, const std::string & value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### onParserHeadersEnd 

```cpp
void onParserHeadersEnd(bool upgrade)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `upgrade` | `bool` |  |

---

#### onParserChunk 

```cpp
void onParserChunk(const char * data, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### onParserEnd 

```cpp
void onParserEnd()
```

---

#### onParserError 

```cpp
void onParserError(const [Error](./doc/api-base.md#structicy_1_1Error) & err)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [Error](./doc/api-base.md#structicy_1_1Error) &` |  |

## Parser 

> **Defined in:** `parser.h`

HTTP request/response parser using the llhttp library.

### Members

| Name | Description |
|------|-------------|
| [`Parser`](#classicy_1_1http_1_1Parser_1aa8e33ac970406b210115f11bf311b264) |  |
| [`Parser`](#classicy_1_1http_1_1Parser_1a3ec4c7513082d1c024cbbb9bb3643ed8) |  |
| [`Parser`](#classicy_1_1http_1_1Parser_1a71919167f74dc474380e6e9d6c11fb2a) |  |
| [`~Parser`](#classicy_1_1http_1_1Parser_1a5a5601800541bf19f61a711a95c3eb3d) |  |
| [`parse`](#classicy_1_1http_1_1Parser_1af7b1e45278cbc1581daad785b1cedab5) | Parse a HTTP packet. |
| [`reset`](#classicy_1_1http_1_1Parser_1a78f9760a7041e1abf6c18b7bfe45f372) | Reset the internal state. |
| [`complete`](#classicy_1_1http_1_1Parser_1a5d34867367516a57d5c7b66419ef5f2d) | Returns true if parsing is complete, either in success or error. |
| [`upgrade`](#classicy_1_1http_1_1Parser_1a1c61be572464f18b0859c5642617b4ff) | Returns true if the connection should be upgraded. |
| [`setRequest`](#classicy_1_1http_1_1Parser_1a88ddd77c698950e4f1dc67808e680da8) |  |
| [`setResponse`](#classicy_1_1http_1_1Parser_1ad6aed0467f8fcdbd9b9a2caa885401ac) |  |
| [`setObserver`](#classicy_1_1http_1_1Parser_1ae102cf86a6376a3cdaab320bf5eaefae) |  |
| [`message`](#classicy_1_1http_1_1Parser_1a4fee2c58d459db5eb9d51d19ba903543) |  |
| [`observer`](#classicy_1_1http_1_1Parser_1ab3c0294c7cf94c797f36232579e73fac) |  |
| [`_observer`](#classicy_1_1http_1_1Parser_1ab42977a9639b8058d82de95625466afc) |  |
| [`_request`](#classicy_1_1http_1_1Parser_1a4981cb205d6bb8d818da9f83335894ec) |  |
| [`_response`](#classicy_1_1http_1_1Parser_1a65ae85b18c21ebe24f566e625e1fa649) |  |
| [`_message`](#classicy_1_1http_1_1Parser_1ad783cae492f8af491ac50f84ed46a0ba) |  |
| [`_parser`](#classicy_1_1http_1_1Parser_1afed4d47dd0d25723bf57c9caebc45754) |  |
| [`_settings`](#classicy_1_1http_1_1Parser_1ab557933e9caf9a34373696696b5d1dba) |  |
| [`_type`](#classicy_1_1http_1_1Parser_1a9c96529de5705acd6a648e49a90c88a6) |  |
| [`_wasHeaderValue`](#classicy_1_1http_1_1Parser_1a1355086b7cec9416fe9bc0769d252053) |  |
| [`_lastHeaderField`](#classicy_1_1http_1_1Parser_1aeeac236dee38ff52b5582052dcd32564) |  |
| [`_lastHeaderValue`](#classicy_1_1http_1_1Parser_1ad5a5ae35c21488224635872e273967bb) |  |
| [`_complete`](#classicy_1_1http_1_1Parser_1a3f868ba7ed8140a65dcc88d30d149440) |  |
| [`_upgrade`](#classicy_1_1http_1_1Parser_1acabe4267aa0ac4a5dbf190d2ed72fc48) |  |
| [`_error`](#classicy_1_1http_1_1Parser_1a382f5184f8e9ed9bd52267dca6c62495) |  |
| [`init`](#classicy_1_1http_1_1Parser_1a73260ac0e403453672228fd6547866ff) |  |
| [`onURL`](#classicy_1_1http_1_1Parser_1aa5cb6c93db76ffce3859f110ef859cba) | Callbacks. |
| [`onHeader`](#classicy_1_1http_1_1Parser_1a39c3e8ebe19b3fc465ab97450c357a86) |  |
| [`onHeadersEnd`](#classicy_1_1http_1_1Parser_1a2f638fec24c8fd048b9c18c9455fe5d5) |  |
| [`onBody`](#classicy_1_1http_1_1Parser_1a8ce2f4f80661f8571b99f1e4cf461f4e) |  |
| [`onMessageEnd`](#classicy_1_1http_1_1Parser_1aabdf6e5c001616ca452b8ca11731314a) |  |
| [`onError`](#classicy_1_1http_1_1Parser_1ad7b06c0d34c03998b252e8c2120141bc) |  |

---

#### Parser 

```cpp
Parser([http::Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) * response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `[http::Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) *` |  |

---

#### Parser 

```cpp
Parser([http::Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) * request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) *` |  |

---

#### Parser 

```cpp
Parser(llhttp_type_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `llhttp_type_t` |  |

---

#### ~Parser 

```cpp
~Parser()
```

---

#### parse 

```cpp
size_t parse(const char * data, size_t length)
```

Parse a HTTP packet.

Returns true of the message is complete, false if incomplete. Reset the parser state for a new message

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `length` | `size_t` |  |

---

#### reset 

```cpp
void reset()
```

Reset the internal state.

---

#### complete 

```cpp
bool complete() const
```

Returns true if parsing is complete, either in success or error.

---

#### upgrade 

```cpp
bool upgrade() const
```

Returns true if the connection should be upgraded.

---

#### setRequest 

```cpp
void setRequest([http::Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) * request)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) *` |  |

---

#### setResponse 

```cpp
void setResponse([http::Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) * response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `[http::Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) *` |  |

---

#### setObserver 

```cpp
void setObserver([ParserObserver](#classicy_1_1http_1_1ParserObserver) * observer)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `observer` | `[ParserObserver](#classicy_1_1http_1_1ParserObserver) *` |  |

---

#### message 

```cpp
[http::Message](#classicy_1_1http_1_1Message) * message()
```

---

#### observer 

```cpp
[ParserObserver](#classicy_1_1http_1_1ParserObserver) * observer() const
```

---

#### _observer 

```cpp
[ParserObserver](#classicy_1_1http_1_1ParserObserver) * _observer
```

---

#### _request 

```cpp
[http::Request](./doc/api-undefined.md#classicy_1_1http_1_1Request) * _request
```

---

#### _response 

```cpp
[http::Response](./doc/api-undefined.md#classicy_1_1http_1_1Response) * _response
```

---

#### _message 

```cpp
[http::Message](#classicy_1_1http_1_1Message) * _message
```

---

#### _parser 

```cpp
llhttp_t _parser
```

---

#### _settings 

```cpp
llhttp_settings_t _settings
```

---

#### _type 

```cpp
llhttp_type_t _type
```

---

#### _wasHeaderValue 

```cpp
bool _wasHeaderValue
```

---

#### _lastHeaderField 

```cpp
std::string _lastHeaderField
```

---

#### _lastHeaderValue 

```cpp
std::string _lastHeaderValue
```

---

#### _complete 

```cpp
bool _complete
```

---

#### _upgrade 

```cpp
bool _upgrade
```

---

#### _error 

```cpp
[Error](./doc/api-base.md#structicy_1_1Error) _error
```

---

#### init 

```cpp
void init()
```

---

#### onURL 

```cpp
void onURL(const std::string & value)
```

Callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### onHeader 

```cpp
void onHeader(const std::string & name, const std::string & value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### onHeadersEnd 

```cpp
void onHeadersEnd()
```

---

#### onBody 

```cpp
void onBody(const char * buf, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const char *` |  |
| `len` | `size_t` |  |

---

#### onMessageEnd 

```cpp
void onMessageEnd()
```

---

#### onError 

```cpp
void onError(llhttp_errno_t errnum, const std::string & message)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `errnum` | `llhttp_errno_t` |  |
| `message` | `const std::string &` |  |

