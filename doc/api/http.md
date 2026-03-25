{#httpmodule}

# http

HTTP client/server, WebSocket support, form handling, cookies, URL parsing.

### Namespaces

| Name | Description |
|------|-------------|
| [`http`](#http) | HTTP request/response types, parsers, and server/client helpers. |
| [`ws`](#ws) | [WebSocket](#websocket) framing, handshakes, and connection helpers. |

{#http}

# http

HTTP request/response types, parsers, and server/client helpers.

### Classes

| Name | Description |
|------|-------------|
| [`Authenticator`](#authenticator) | This is a utility class for working with HTTP authentication (basic or digest) in [http::Request](http::Request) objects. |
| [`BasicAuthenticator`](#basicauthenticator) | This is a utility class for working with HTTP Basic Authentication in [http::Request](http::Request) objects. |
| [`ChunkedAdapter`](#chunkedadapter) | HTTP chunked transfer encoding adapter for streaming responses. |
| [`Client`](#client) | HTTP client for creating and managing outgoing connections. |
| [`ClientConnection`](#clientconnection) | HTTP client connection for managing request/response lifecycle. |
| [`Connection`](#connection-1) | Base HTTP connection managing socket I/O and message lifecycle |
| [`ConnectionAdapter`](#connectionadapter) | Default HTTP socket adapter for reading and writing HTTP messages |
| [`ConnectionPool`](#connectionpool) | LIFO connection pool for reusing [ServerConnection](#serverconnection) objects. Avoids per-request heap allocation by resetting and reusing connections instead of destroying and recreating them. |
| [`ConnectionStream`](#connectionstream) | Packet stream wrapper for a HTTP connection. |
| [`Cookie`](#cookie) | This class represents a HTTP [Cookie](#cookie). |
| [`FilePart`](#filepart) | An implementation of [FilePart](#filepart) for plain files. |
| [`FormPart`](#formpart) | An implementation of [FormPart](#formpart). |
| [`FormWriter`](#formwriter) | [FormWriter](#formwriter) is a HTTP client connection adapter for writing HTML forms. |
| [`Message`](#message) | The base class for [Request](#request-4) and [Response](#response-1). |
| [`MultipartAdapter`](#multipartadapter) | HTTP multipart encoding adapter for multipart/x-mixed-replace streaming. |
| [`Parser`](#parser-1) | HTTP request/response parser using the llhttp library. |
| [`ParserObserver`](#parserobserver) | Abstract observer interface for HTTP parser events. |
| [`ProgressSignal`](#progresssignal) | HTTP progress signal for upload and download progress notifications. |
| [`Request`](#request-4) | This class encapsulates an HTTP request message. |
| [`Response`](#response-1) | This class encapsulates an HTTP response message. |
| [`Server`](#server) | HTTP server implementation. |
| [`ServerConnection`](#serverconnection) | HTTP server connection. |
| [`ServerConnectionFactory`](#serverconnectionfactory) | This implementation of a [ServerConnectionFactory](#serverconnectionfactory) is used by HTTP [Server](#server) to create [ServerConnection](#serverconnection) objects. |
| [`ServerResponder`](#serverresponder) | The abstract base class for HTTP ServerResponders created by HTTP [Server](#server). |
| [`StringPart`](#stringpart) | An implementation of [StringPart](#stringpart) for plain files. |
| [`URL`](#url) | An RFC 3986 based [URL](#url) parser. Constructors and assignment operators will throw a SyntaxException if the [URL](#url) is invalid. |
| [`DateCache`](#datecache) | Caches the formatted Date header, updated once per second. Avoids per-request time formatting and string allocation. |
| [`Method`](#method) | HTTP request methods. |

### Enumerations

| Name | Description |
|------|-------------|
| [`StatusCode`](#statuscode)  | HTTP [Response](#response-1) Status Codes. |
| [`ServerConnectionState`](#serverconnectionstate)  | HTTP server-side lifecycle phases used by the keep-alive state machine. |
| [`ServerConnectionMode`](#serverconnectionmode)  | Transport mode for server connections before and after protocol upgrade. |

---

{#statuscode}

#### StatusCode

```cpp
enum StatusCode
```

HTTP [Response](#response-1) Status Codes.

| Value | Description |
|-------|-------------|
| `Continue` |  |
| `SwitchingProtocols` |  |
| `OK` |  |
| `Created` |  |
| `Accepted` |  |
| `NonAuthoritative` |  |
| `NoContent` |  |
| `ResetContent` |  |
| `PartialContent` |  |
| `MultipleChoices` |  |
| `MovedPermanently` |  |
| `Found` |  |
| `SeeOther` |  |
| `NotModified` |  |
| `UseProxy` |  |
| `TemporaryRedirect` |  |
| `BadRequest` |  |
| `Unauthorized` |  |
| `PaymentRequired` |  |
| `Forbidden` |  |
| `NotFound` |  |
| `MethodNotAllowed` |  |
| `NotAcceptable` |  |
| `ProxyAuthRequired` |  |
| `RequestTimeout` |  |
| `Conflict` |  |
| `Gone` |  |
| `LengthRequired` |  |
| `PreconditionFailed` |  |
| `EntityTooLarge` |  |
| `UriTooLong` |  |
| `UnsupportedMediaType` |  |
| `RangeNotSatisfiable` |  |
| `ExpectationFailed` |  |
| `UnprocessableEntity` |  |
| `UpgradeRequired` |  |
| `InternalServerError` |  |
| `NotImplemented` |  |
| `BadGateway` |  |
| `Unavailable` |  |
| `GatewayTimeout` |  |
| `VersionNotSupported` |  |

---

{#serverconnectionstate}

#### ServerConnectionState

```cpp
enum ServerConnectionState
```

HTTP server-side lifecycle phases used by the keep-alive state machine.

| Value | Description |
|-------|-------------|
| `ReceivingHeaders` | Parsing request headers. |
| `ReceivingBody` | Receiving request body bytes. |
| `DispatchingOrSending` | Dispatching the responder or sending a normal response. |
| `Streaming` | Sending a long-lived streaming response. |
| `Upgraded` | Upgraded to a non-HTTP protocol such as WebSocket. |
| `Closing` | Close has been requested and teardown is in progress. |
| `Closed` | [Connection](#connection-1) has fully closed. |

---

{#serverconnectionmode}

#### ServerConnectionMode

```cpp
enum ServerConnectionMode
```

Transport mode for server connections before and after protocol upgrade.

| Value | Description |
|-------|-------------|
| `Http` | Standard HTTP request/response mode. |
| `Upgraded` | Upgraded transport mode owned by another protocol adapter. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< ClientConnection::Ptr >` | [`ClientConnectionPtrVec`](#clientconnectionptrvec)  |  |

---

{#clientconnectionptrvec}

#### ClientConnectionPtrVec

```cpp
std::vector< ClientConnection::Ptr > ClientConnectionPtrVec()
```

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`isBasicCredentials`](#isbasiccredentials)  | Returns true if the given Authorization header value uses HTTP Basic authentication.  |
| `bool` | [`isDigestCredentials`](#isdigestcredentials)  | Returns true if the given Authorization header value uses HTTP Digest authentication.  |
| `bool` | [`hasBasicCredentials`](#hasbasiccredentials)  | Returns true if the request contains a Basic Authorization header.  |
| `bool` | [`hasDigestCredentials`](#hasdigestcredentials)  | Returns true if the request contains a Digest Authorization header.  |
| `bool` | [`hasProxyBasicCredentials`](#hasproxybasiccredentials)  | Returns true if the request contains a Basic Proxy-Authorization header.  |
| `bool` | [`hasProxyDigestCredentials`](#hasproxydigestcredentials)  | Returns true if the request contains a Digest Proxy-Authorization header.  |
| `void` | [`extractCredentials`](#extractcredentials)  | Splits a "user:password" user-info string into separate username and password strings. If no ':' is present, the entire string is treated as the username and password is empty.  |
| `void` | [`extractCredentials`](#extractcredentials-1)  | Extracts username and password from the user-info component of a [URL](#url). Does nothing if the [URL](#url) has no user-info part.  |
| `ClientConnection::Ptr` | [`createConnectionT`](#createconnectiont) `inline` | Creates a [ClientConnection](#clientconnection) (or subtype) for the given [URL](#url) without registering it with a [Client](#client) instance. The socket and adapter are chosen based on the [URL](#url) scheme: |
| `ClientConnection::Ptr` | [`createConnection`](#createconnection) `inline` | Creates a [ClientConnection](#clientconnection) for the given [URL](#url) and optionally registers it with a [Client](#client). Equivalent to calling [Client::createConnection()](#createconnection-1) when `client` is non-null.  |
| `const char *` | [`getStatusCodeReason`](#getstatuscodereason)  | Returns the standard reason phrase for the given HTTP status code (e.g. "OK" for [StatusCode::OK](#namespaceicy_1_1http_1aa73f8ae30b4882be20ce0a7e16adc1a4ae0aa021e21dddbd6d8cecec71e9cf564), "Not Found" for [StatusCode::NotFound](#namespaceicy_1_1http_1aa73f8ae30b4882be20ce0a7e16adc1a4a38c300f4fc9ce8a77aad4a30de05cad8)).  |
| `const char *` | [`getStatusCodeString`](#getstatuscodestring)  | Returns a combined "NNN Reason" string for the given HTTP status code (e.g. "200 OK").  |
| `std::string` | [`parseURI`](#parseuri)  | Extracts the URI (path and query) from a raw HTTP request line.  |
| `bool` | [`matchURL`](#matchurl)  | Tests whether a URI matches a glob-style expression.  |
| `std::string` | [`parseCookieItem`](#parsecookieitem)  | Extracts a named attribute from a [Cookie](#cookie) header value.  |
| `bool` | [`splitURIParameters`](#splituriparameters)  | Parses [URL](#url) query parameters from a URI into key-value pairs. Handles percent-decoding of names and values.  |
| `void` | [`splitParameters`](#splitparameters)  | Splits a header-style parameter string into a primary value and named attributes. Attributes are separated by ';'. Enclosing quotes are stripped from values. |
| `void` | [`splitParameters`](#splitparameters-1)  | Splits a substring (defined by iterators) into named attributes. Attributes are separated by ';'. Enclosing quotes are stripped from values.  |

---

{#isbasiccredentials}

#### isBasicCredentials

```cpp
bool isBasicCredentials(std::string_view header)
```

Returns true if the given Authorization header value uses HTTP Basic authentication. 
#### Parameters
* `header` Value of the Authorization or WWW-Authenticate header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `std::string_view` |  |

---

{#isdigestcredentials}

#### isDigestCredentials

```cpp
bool isDigestCredentials(std::string_view header)
```

Returns true if the given Authorization header value uses HTTP Digest authentication. 
#### Parameters
* `header` Value of the Authorization or WWW-Authenticate header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `std::string_view` |  |

---

{#hasbasiccredentials}

#### hasBasicCredentials

```cpp
bool hasBasicCredentials(const http::Request & request)
```

Returns true if the request contains a Basic Authorization header. 
#### Parameters
* `request` HTTP request to inspect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const [http::Request](#request-4) &` |  |

---

{#hasdigestcredentials}

#### hasDigestCredentials

```cpp
bool hasDigestCredentials(const http::Request & request)
```

Returns true if the request contains a Digest Authorization header. 
#### Parameters
* `request` HTTP request to inspect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const [http::Request](#request-4) &` |  |

---

{#hasproxybasiccredentials}

#### hasProxyBasicCredentials

```cpp
bool hasProxyBasicCredentials(const http::Request & request)
```

Returns true if the request contains a Basic Proxy-Authorization header. 
#### Parameters
* `request` HTTP request to inspect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const [http::Request](#request-4) &` |  |

---

{#hasproxydigestcredentials}

#### hasProxyDigestCredentials

```cpp
bool hasProxyDigestCredentials(const http::Request & request)
```

Returns true if the request contains a Digest Proxy-Authorization header. 
#### Parameters
* `request` HTTP request to inspect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const [http::Request](#request-4) &` |  |

---

{#extractcredentials}

#### extractCredentials

```cpp
void extractCredentials(std::string_view userInfo, std::string & username, std::string & password)
```

Splits a "user:password" user-info string into separate username and password strings. If no ':' is present, the entire string is treated as the username and password is empty. 
#### Parameters
* `userInfo` Input string in the form "user:password". 

* `username` Receives the extracted username. 

* `password` Receives the extracted password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `userInfo` | `std::string_view` |  |
| `username` | `std::string &` |  |
| `password` | `std::string &` |  |

---

{#extractcredentials-1}

#### extractCredentials

```cpp
void extractCredentials(const http::URL & uri, std::string & username, std::string & password)
```

Extracts username and password from the user-info component of a [URL](#url). Does nothing if the [URL](#url) has no user-info part. 
#### Parameters
* `uri` [URL](#url) to extract credentials from. 

* `username` Receives the extracted username. 

* `password` Receives the extracted password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `const [http::URL](#url) &` |  |
| `username` | `std::string &` |  |
| `password` | `std::string &` |  |

---

{#createconnectiont}

#### createConnectionT

`inline`

```cpp
template<class ConnectionT> inline ClientConnection::Ptr createConnectionT(const URL & url, uv::Loop * loop)
```

Creates a [ClientConnection](#clientconnection) (or subtype) for the given [URL](#url) without registering it with a [Client](#client) instance. The socket and adapter are chosen based on the [URL](#url) scheme:

* "http" -> TCPSocket

* "https" -> SSLSocket

* "ws" -> TCPSocket + WebSocket adapter

* "wss" -> SSLSocket + WebSocket adapter

#### Parameters
* `ConnectionT` Concrete connection type derived from [ClientConnection](#clientconnection). 

#### Parameters
* `url` Target [URL](#url). Must have a recognised scheme. 

* `loop` Event loop to use. Defaults to the default libuv loop. 

#### Returns
Shared pointer to the created connection. 

#### Exceptions
* `std::runtime_error` if the [URL](#url) scheme is not recognised.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const [URL](#url) &` |  |
| `loop` | `[uv::Loop](uv.md#loop) *` |  |

---

{#createconnection}

#### createConnection

`inline`

```cpp
inline ClientConnection::Ptr createConnection(const URL & url, http::Client * client, uv::Loop * loop)
```

Creates a [ClientConnection](#clientconnection) for the given [URL](#url) and optionally registers it with a [Client](#client). Equivalent to calling [Client::createConnection()](#createconnection-1) when `client` is non-null. 
#### Parameters
* `url` Target [URL](#url). The scheme determines the socket and adapter type. 

* `client` Optional [Client](#client) instance to register the connection with. 

* `loop` Event loop to use. Defaults to the default libuv loop. 

#### Returns
Shared pointer to the created connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const [URL](#url) &` |  |
| `client` | `[http::Client](#client) *` |  |
| `loop` | `[uv::Loop](uv.md#loop) *` |  |

---

{#getstatuscodereason}

#### getStatusCodeReason

```cpp
const char * getStatusCodeReason(StatusCode status)
```

Returns the standard reason phrase for the given HTTP status code (e.g. "OK" for [StatusCode::OK](#namespaceicy_1_1http_1aa73f8ae30b4882be20ce0a7e16adc1a4ae0aa021e21dddbd6d8cecec71e9cf564), "Not Found" for [StatusCode::NotFound](#namespaceicy_1_1http_1aa73f8ae30b4882be20ce0a7e16adc1a4a38c300f4fc9ce8a77aad4a30de05cad8)). 
#### Parameters
* `status` HTTP status code. 

#### Returns
Null-terminated reason phrase string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | `[StatusCode](#statuscode)` |  |

---

{#getstatuscodestring}

#### getStatusCodeString

```cpp
const char * getStatusCodeString(StatusCode status)
```

Returns a combined "NNN Reason" string for the given HTTP status code (e.g. "200 OK"). 
#### Parameters
* `status` HTTP status code. 

#### Returns
Null-terminated status code string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | `[StatusCode](#statuscode)` |  |

---

{#parseuri}

#### parseURI

```cpp
std::string parseURI(std::string_view request)
```

Extracts the URI (path and query) from a raw HTTP request line. 
#### Parameters
* `request` Raw HTTP request line (e.g. "GET /path?q=1 HTTP/1.1"). 

#### Returns
The URI portion (e.g. "/path?q=1").

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `std::string_view` |  |

---

{#matchurl}

#### matchURL

```cpp
bool matchURL(std::string_view uri, std::string_view expression)
```

Tests whether a URI matches a glob-style expression. 
#### Parameters
* `uri` The URI to test. 

* `expression` Pattern to match against. '*' matches any sequence of characters. 

#### Returns
true if the URI matches the expression.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `std::string_view` |  |
| `expression` | `std::string_view` |  |

---

{#parsecookieitem}

#### parseCookieItem

```cpp
std::string parseCookieItem(std::string_view cookie, std::string_view item)
```

Extracts a named attribute from a [Cookie](#cookie) header value. 
#### Parameters
* `cookie` Full [Cookie](#cookie) header value (e.g. "name=value; Path=/; HttpOnly"). 

* `item` Attribute name to find (e.g. "Path"). 

#### Returns
The value of the named attribute, or an empty string if not found.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookie` | `std::string_view` |  |
| `item` | `std::string_view` |  |

---

{#splituriparameters}

#### splitURIParameters

```cpp
bool splitURIParameters(std::string_view uri, NVCollection & out)
```

Parses [URL](#url) query parameters from a URI into key-value pairs. Handles percent-decoding of names and values. 
#### Parameters
* `uri` URI string optionally containing a '?' query component. 

* `out` Collection to populate with parsed parameters. 

#### Returns
true if at least one parameter was parsed; false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `std::string_view` |  |
| `out` | `[NVCollection](base.md#nvcollection) &` |  |

---

{#splitparameters}

#### splitParameters

```cpp
void splitParameters(const std::string & s, std::string & value, NVCollection & parameters)
```

Splits a header-style parameter string into a primary value and named attributes. Attributes are separated by ';'. Enclosing quotes are stripped from values.

Example input: "multipart/mixed; boundary=\"boundary-01234567\"" Output value: "multipart/mixed", parameters: { "boundary" -> "boundary-01234567" }

#### Parameters
* `s` Input string to split. 

* `value` Receives the primary value before the first ';'. 

* `parameters` Receives the parsed attribute key-value pairs.

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `const std::string &` |  |
| `value` | `std::string &` |  |
| `parameters` | `[NVCollection](base.md#nvcollection) &` |  |

---

{#splitparameters-1}

#### splitParameters

```cpp
void splitParameters(const std::string::const_iterator & begin, const std::string::const_iterator & end, NVCollection & parameters)
```

Splits a substring (defined by iterators) into named attributes. Attributes are separated by ';'. Enclosing quotes are stripped from values. 
#### Parameters
* `begin` Iterator to the start of the string to parse. 

* `end` Iterator past the end of the string to parse. 

* `parameters` Receives the parsed attribute key-value pairs.

| Parameter | Type | Description |
|-----------|------|-------------|
| `begin` | `const std::string::const_iterator &` |  |
| `end` | `const std::string::const_iterator &` |  |
| `parameters` | `[NVCollection](base.md#nvcollection) &` |  |

{#authenticator}

## Authenticator

```cpp
#include <icy/http/authenticator.h>
```

This is a utility class for working with HTTP authentication (basic or digest) in [http::Request](http::Request) objects.

Note: Do not forget to read the entire response stream from the 401 response before sending the authenticated request, otherwise there may be problems if a persistent connection is used.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Authenticator`](#authenticator-1)  | Creates an empty [Authenticator](#authenticator) object. |
|  | [`Authenticator`](#authenticator-2)  | Creates an [Authenticator](#authenticator) object with the given username and password. |
|  | [`~Authenticator`](#authenticator-3)  | Destroys the [Authenticator](#authenticator). |
| `void` | [`fromUserInfo`](#fromuserinfo)  | Parses username:password std::string and sets username and password of the credentials object. Throws SyntaxException on invalid user information. |
| `void` | [`fromURI`](#fromuri)  | Extracts username and password from the given URI and sets username and password of the credentials object. Does nothing if URI has no user info part. |
| `void` | [`setUsername`](#setusername)  | Sets the username. |
| `const std::string &` | [`username`](#username) `const` | Returns the username. |
| `void` | [`setPassword`](#setpassword)  | Sets the password. |
| `const std::string &` | [`password`](#password) `const` | Returns the password. |
| `void` | [`authenticate`](#authenticate)  | Inspects WWW-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request). |
| `void` | [`updateAuthInfo`](#updateauthinfo)  | Updates internal state (in case of digest authentication) and replaces authentication information in the request accordingly. |
| `void` | [`proxyAuthenticate`](#proxyauthenticate)  | Inspects Proxy-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request). |
| `void` | [`updateProxyAuthInfo`](#updateproxyauthinfo)  | Updates internal state (in case of digest authentication) and replaces proxy authentication information in the request accordingly. |

---

{#authenticator-1}

#### Authenticator

```cpp
Authenticator()
```

Creates an empty [Authenticator](#authenticator) object.

---

{#authenticator-2}

#### Authenticator

```cpp
Authenticator(const std::string & username, const std::string & password)
```

Creates an [Authenticator](#authenticator) object with the given username and password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |
| `password` | `const std::string &` |  |

---

{#authenticator-3}

#### ~Authenticator

```cpp
~Authenticator()
```

Destroys the [Authenticator](#authenticator).

---

{#fromuserinfo}

#### fromUserInfo

```cpp
void fromUserInfo(std::string_view userInfo)
```

Parses username:password std::string and sets username and password of the credentials object. Throws SyntaxException on invalid user information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `userInfo` | `std::string_view` |  |

---

{#fromuri}

#### fromURI

```cpp
void fromURI(const http::URL & uri)
```

Extracts username and password from the given URI and sets username and password of the credentials object. Does nothing if URI has no user info part.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `const [http::URL](#url) &` |  |

---

{#setusername}

#### setUsername

```cpp
void setUsername(const std::string & username)
```

Sets the username.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |

---

{#username}

#### username

`const`

```cpp
const std::string & username() const
```

Returns the username.

---

{#setpassword}

#### setPassword

```cpp
void setPassword(const std::string & password)
```

Sets the password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `password` | `const std::string &` |  |

---

{#password}

#### password

`const`

```cpp
const std::string & password() const
```

Returns the password.

---

{#authenticate}

#### authenticate

```cpp
void authenticate(http::Request & request, const http::Response & response)
```

Inspects WWW-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |
| `response` | `const [http::Response](#response-1) &` |  |

---

{#updateauthinfo}

#### updateAuthInfo

```cpp
void updateAuthInfo(http::Request & request)
```

Updates internal state (in case of digest authentication) and replaces authentication information in the request accordingly.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |

---

{#proxyauthenticate}

#### proxyAuthenticate

```cpp
void proxyAuthenticate(http::Request & request, const http::Response & response)
```

Inspects Proxy-Authenticate header of the response, initializes the internal state (in case of digest authentication) and adds required information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |
| `response` | `const [http::Response](#response-1) &` |  |

---

{#updateproxyauthinfo}

#### updateProxyAuthInfo

```cpp
void updateProxyAuthInfo(http::Request & request)
```

Updates internal state (in case of digest authentication) and replaces proxy authentication information in the request accordingly.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_username`](#_username)  |  |
| `std::string` | [`_password`](#_password)  |  |

---

{#_username}

#### _username

```cpp
std::string _username
```

---

{#_password}

#### _password

```cpp
std::string _password
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Authenticator`](#authenticator-4)  |  |

---

{#authenticator-4}

#### Authenticator

```cpp
Authenticator(const Authenticator &) = delete
```

{#basicauthenticator}

## BasicAuthenticator

```cpp
#include <icy/http/authenticator.h>
```

This is a utility class for working with HTTP Basic Authentication in [http::Request](http::Request) objects.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`BasicAuthenticator`](#basicauthenticator-1)  | Creates an empty [BasicAuthenticator](#basicauthenticator) object. |
|  | [`BasicAuthenticator`](#basicauthenticator-2)  | Creates a [BasicAuthenticator](#basicauthenticator) object with the given username and password. |
|  | [`BasicAuthenticator`](#basicauthenticator-3) `explicit` | Creates a [BasicAuthenticator](#basicauthenticator) object with the authentication information from the given request. |
|  | [`BasicAuthenticator`](#basicauthenticator-4) `explicit` | Creates a [BasicAuthenticator](#basicauthenticator) object with the authentication information in the given std::string. The authentication information can be extracted from a [http::Request](http::Request) object by calling [http::Request::getCredentials()](http::Request::getCredentials()). |
|  | [`~BasicAuthenticator`](#basicauthenticator-5)  | Destroys the [BasicAuthenticator](#basicauthenticator). |
| `void` | [`setUsername`](#setusername-1)  | Sets the username. |
| `const std::string &` | [`username`](#username-1) `const` | Returns the username. |
| `void` | [`setPassword`](#setpassword-1)  | Sets the password. |
| `const std::string &` | [`password`](#password-1) `const` | Returns the password. |
| `void` | [`authenticate`](#authenticate-1) `const` | Adds authentication information to the given [http::Request](http::Request). |
| `void` | [`proxyAuthenticate`](#proxyauthenticate-1) `const` | Adds proxy authentication information to the given [http::Request](http::Request). |

---

{#basicauthenticator-1}

#### BasicAuthenticator

```cpp
BasicAuthenticator()
```

Creates an empty [BasicAuthenticator](#basicauthenticator) object.

---

{#basicauthenticator-2}

#### BasicAuthenticator

```cpp
BasicAuthenticator(const std::string & username, const std::string & password)
```

Creates a [BasicAuthenticator](#basicauthenticator) object with the given username and password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |
| `password` | `const std::string &` |  |

---

{#basicauthenticator-3}

#### BasicAuthenticator

`explicit`

```cpp
explicit BasicAuthenticator(const http::Request & request)
```

Creates a [BasicAuthenticator](#basicauthenticator) object with the authentication information from the given request.

Throws a NotAuthenticatedException if the request does not contain basic authentication information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const [http::Request](#request-4) &` |  |

---

{#basicauthenticator-4}

#### BasicAuthenticator

`explicit`

```cpp
explicit BasicAuthenticator(const std::string & authInfo)
```

Creates a [BasicAuthenticator](#basicauthenticator) object with the authentication information in the given std::string. The authentication information can be extracted from a [http::Request](http::Request) object by calling [http::Request::getCredentials()](http::Request::getCredentials()).

| Parameter | Type | Description |
|-----------|------|-------------|
| `authInfo` | `const std::string &` |  |

---

{#basicauthenticator-5}

#### ~BasicAuthenticator

```cpp
~BasicAuthenticator()
```

Destroys the [BasicAuthenticator](#basicauthenticator).

---

{#setusername-1}

#### setUsername

```cpp
void setUsername(const std::string & username)
```

Sets the username.

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |

---

{#username-1}

#### username

`const`

```cpp
const std::string & username() const
```

Returns the username.

---

{#setpassword-1}

#### setPassword

```cpp
void setPassword(const std::string & password)
```

Sets the password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `password` | `const std::string &` |  |

---

{#password-1}

#### password

`const`

```cpp
const std::string & password() const
```

Returns the password.

---

{#authenticate-1}

#### authenticate

`const`

```cpp
void authenticate(http::Request & request) const
```

Adds authentication information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |

---

{#proxyauthenticate-1}

#### proxyAuthenticate

`const`

```cpp
void proxyAuthenticate(http::Request & request) const
```

Adds proxy authentication information to the given [http::Request](http::Request).

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`parseAuthInfo`](#parseauthinfo)  | Extracts username and password from Basic authentication info by base64-decoding authInfo and splitting the resulting std::string at the ':' delimiter. |

---

{#parseauthinfo}

#### parseAuthInfo

```cpp
void parseAuthInfo(std::string_view authInfo)
```

Extracts username and password from Basic authentication info by base64-decoding authInfo and splitting the resulting std::string at the ':' delimiter.

| Parameter | Type | Description |
|-----------|------|-------------|
| `authInfo` | `std::string_view` |  |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_username`](#_username-1)  |  |
| `std::string` | [`_password`](#_password-1)  |  |

---

{#_username-1}

#### _username

```cpp
std::string _username
```

---

{#_password-1}

#### _password

```cpp
std::string _password
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`BasicAuthenticator`](#basicauthenticator-6)  |  |

---

{#basicauthenticator-6}

#### BasicAuthenticator

```cpp
BasicAuthenticator(const BasicAuthenticator &) = delete
```

{#chunkedadapter}

## ChunkedAdapter

```cpp
#include <icy/http/packetizers.h>
```

> **Inherits:** [`PacketProcessor`](base.md#packetprocessor)

HTTP chunked transfer encoding adapter for streaming responses.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Connection::Ptr` | [`connection`](#connection)  | HTTP connection to send the initial response header through. |
| `std::string` | [`contentType`](#contenttype)  | Content-Type value for the chunked response. |
| `std::string` | [`frameSeparator`](#frameseparator)  | Optional separator written before each chunk payload. |
| `bool` | [`initial`](#initial)  | True until the first chunk is processed and the header emitted. |
| `bool` | [`nocopy`](#nocopy)  | If true, header/data/footer are emitted as separate packets. |
| `PacketSignal` | [`emitter`](#emitter-5)  |  |

---

{#connection}

#### connection

```cpp
Connection::Ptr connection
```

HTTP connection to send the initial response header through.

---

{#contenttype}

#### contentType

```cpp
std::string contentType
```

Content-Type value for the chunked response.

---

{#frameseparator}

#### frameSeparator

```cpp
std::string frameSeparator
```

Optional separator written before each chunk payload.

---

{#initial}

#### initial

```cpp
bool initial
```

True until the first chunk is processed and the header emitted.

---

{#nocopy}

#### nocopy

```cpp
bool nocopy
```

If true, header/data/footer are emitted as separate packets.

---

{#emitter-5}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ChunkedAdapter`](#chunkedadapter-1) `inline` | Creates a [ChunkedAdapter](#chunkedadapter) that sends its initial response header through the given connection. The content type is read from the connection's outgoing header.  |
|  | [`ChunkedAdapter`](#chunkedadapter-2) `inline` | Creates a [ChunkedAdapter](#chunkedadapter) that emits its own raw HTTP/1.1 200 response header. Use this when no [Connection](#connection-1) object is available.  |
| `void` | [`emitHeader`](#emitheader) `virtual` `inline` | Emits the initial HTTP/1.1 200 OK response headers with chunked transfer encoding. If a connection is set, headers are written through it; otherwise a raw response string is emitted via the packet signal. |
| `void` | [`process`](#process-5) `virtual` `inline` | Encodes an incoming packet as a chunked transfer encoding chunk and emits it. Emits the HTTP response headers on the first call.  |

---

{#chunkedadapter-1}

#### ChunkedAdapter

`inline`

```cpp
inline ChunkedAdapter(Connection::Ptr connection, const std::string & frameSeparator, bool nocopy)
```

Creates a [ChunkedAdapter](#chunkedadapter) that sends its initial response header through the given connection. The content type is read from the connection's outgoing header. 
#### Parameters
* `connection` HTTP connection to use. May be nullptr to emit a raw response instead. 

* `frameSeparator` Optional data prepended to each chunk payload. 

* `nocopy` If true, header and payload are emitted as separate packets (avoids copies).

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection::Ptr](#ptr-12)` |  |
| `frameSeparator` | `const std::string &` |  |
| `nocopy` | `bool` |  |

---

{#chunkedadapter-2}

#### ChunkedAdapter

`inline`

```cpp
inline ChunkedAdapter(const std::string & contentType, const std::string & frameSeparator, bool nocopy)
```

Creates a [ChunkedAdapter](#chunkedadapter) that emits its own raw HTTP/1.1 200 response header. Use this when no [Connection](#connection-1) object is available. 
#### Parameters
* `contentType` Content-Type value for the response. 

* `frameSeparator` Optional data prepended to each chunk payload. 

* `nocopy` If true, header and payload are emitted as separate packets.

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |
| `frameSeparator` | `const std::string &` |  |
| `nocopy` | `bool` |  |

---

{#emitheader}

#### emitHeader

`virtual` `inline`

```cpp
virtual inline void emitHeader()
```

Emits the initial HTTP/1.1 200 OK response headers with chunked transfer encoding. If a connection is set, headers are written through it; otherwise a raw response string is emitted via the packet signal.

---

{#process-5}

#### process

`virtual` `inline`

```cpp
virtual inline void process(IPacket & packet)
```

Encodes an incoming packet as a chunked transfer encoding chunk and emits it. Emits the HTTP response headers on the first call. 
#### Parameters
* `packet` Packet containing the raw payload data. 

#### Exceptions
* `std::invalid_argument` if the packet does not carry data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `[IPacket](base.md#ipacket) &` |  |

{#client}

## Client

```cpp
#include <icy/http/client.h>
```

HTTP client for creating and managing outgoing connections.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `NullSignal` | [`Shutdown`](#shutdown-6)  |  |

---

{#shutdown-6}

#### Shutdown

```cpp
NullSignal Shutdown
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Client`](#client-1)  |  |
| `void` | [`shutdown`](#shutdown-7)  | Shutdown the [Client](#client) and close all connections. |
| `ClientConnection::Ptr` | [`createConnectionT`](#createconnectiont-1) `inline` | Creates and registers a typed client connection for the given [URL](#url). The connection type is inferred from the [URL](#url) scheme (http, https, ws, wss).  |
| `ClientConnection::Ptr` | [`createConnection`](#createconnection-1) `inline` | Creates and registers a [ClientConnection](#clientconnection) for the given [URL](#url). The socket type is chosen based on the [URL](#url) scheme (http/https/ws/wss).  |
| `void` | [`addConnection`](#addconnection) `virtual` | Registers a connection with this client so it is tracked and cleaned up on shutdown.  |
| `void` | [`removeConnection`](#removeconnection) `virtual` | Removes a previously registered connection from the client.  |

---

{#client-1}

#### Client

```cpp
Client()
```

---

{#shutdown-7}

#### shutdown

```cpp
void shutdown()
```

Shutdown the [Client](#client) and close all connections.

---

{#createconnectiont-1}

#### createConnectionT

`inline`

```cpp
template<class ConnectionT> inline ClientConnection::Ptr createConnectionT(const URL & url, uv::Loop * loop)
```

Creates and registers a typed client connection for the given [URL](#url). The connection type is inferred from the [URL](#url) scheme (http, https, ws, wss). 
#### Parameters
* `ConnectionT` Concrete connection type derived from [ClientConnection](#clientconnection). 

#### Parameters
* `url` Target [URL](#url). The scheme determines the socket and adapter type. 

* `loop` Event loop to use. Defaults to the default libuv loop. 

#### Returns
Shared pointer to the created connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const [URL](#url) &` |  |
| `loop` | `[uv::Loop](uv.md#loop) *` |  |

---

{#createconnection-1}

#### createConnection

`inline`

```cpp
inline ClientConnection::Ptr createConnection(const URL & url, uv::Loop * loop)
```

Creates and registers a [ClientConnection](#clientconnection) for the given [URL](#url). The socket type is chosen based on the [URL](#url) scheme (http/https/ws/wss). 
#### Parameters
* `url` Target [URL](#url). 

* `loop` Event loop to use. Defaults to the default libuv loop. 

#### Returns
Shared pointer to the created connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const [URL](#url) &` |  |
| `loop` | `[uv::Loop](uv.md#loop) *` |  |

---

{#addconnection}

#### addConnection

`virtual`

```cpp
virtual void addConnection(ClientConnection::Ptr conn)
```

Registers a connection with this client so it is tracked and cleaned up on shutdown. 
#### Parameters
* `conn` The connection to add.

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ClientConnection::Ptr](#ptr-11)` |  |

---

{#removeconnection}

#### removeConnection

`virtual`

```cpp
virtual void removeConnection(ClientConnection * conn)
```

Removes a previously registered connection from the client. 
#### Parameters
* `conn` Raw pointer to the connection to remove. 

#### Exceptions
* `std::logic_error` if the connection is not tracked by this client.

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ClientConnection](#clientconnection) *` |  |

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `Client &` | [`instance`](#instance-3) `static` | Return the default HTTP [Client](#client) singleton. |
| `void` | [`destroy`](#destroy-1) `static` | Destroys the default HTTP [Client](#client) singleton. |

---

{#instance-3}

#### instance

`static`

```cpp
static Client & instance()
```

Return the default HTTP [Client](#client) singleton.

---

{#destroy-1}

#### destroy

`static`

```cpp
static void destroy()
```

Destroys the default HTTP [Client](#client) singleton.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ClientConnectionPtrVec` | [`_connections`](#_connections)  |  |

---

{#_connections}

#### _connections

```cpp
ClientConnectionPtrVec _connections
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onConnectionClose`](#onconnectionclose)  |  |

---

{#onconnectionclose}

#### onConnectionClose

```cpp
void onConnectionClose(Connection & conn)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[Connection](#connection-1) &` |  |

{#clientconnection}

## ClientConnection

```cpp
#include <icy/http/client.h>
```

> **Inherits:** [`Connection`](#connection-1)

HTTP client connection for managing request/response lifecycle.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `void *` | [`opaque`](#opaque-1)  | Optional unmanaged client data pointer. Not used by the connection internally. |
| `NullSignal` | [`Connect`](#connect-11)  | Status signals. |
| `Signal< void(Response &)>` | [`Headers`](#headers)  | Signals when the response HTTP header has been received. |
| `Signal< void(const MutableBuffer &)>` | [`Payload`](#payload)  | Signals when raw data is received. |
| `Signal< void(const Response &)>` | [`Complete`](#complete)  | Signals when the HTTP transaction is complete. |
| `Signal< void(Connection &)>` | [`Close`](#close-19)  | Signals when the connection is closed. |
| `ProgressSignal` | [`IncomingProgress`](#incomingprogress)  | Signals download progress (0-100%) |

---

{#opaque-1}

#### opaque

```cpp
void * opaque
```

Optional unmanaged client data pointer. Not used by the connection internally.

---

{#connect-11}

#### Connect

```cpp
NullSignal Connect
```

Status signals.

Signals when the client socket is connected and data can flow

---

{#headers}

#### Headers

```cpp
Signal< void(Response &)> Headers
```

Signals when the response HTTP header has been received.

---

{#payload}

#### Payload

```cpp
Signal< void(const MutableBuffer &)> Payload
```

Signals when raw data is received.

---

{#complete}

#### Complete

```cpp
Signal< void(const Response &)> Complete
```

Signals when the HTTP transaction is complete.

---

{#close-19}

#### Close

```cpp
Signal< void(Connection &)> Close
```

Signals when the connection is closed.

---

{#incomingprogress}

#### IncomingProgress

```cpp
ProgressSignal IncomingProgress
```

Signals download progress (0-100%)

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ClientConnection`](#clientconnection-1)  | Creates a [ClientConnection](#clientconnection) to the given [URL](#url), pre-populating the request URI and Host header. The response status is initialised to 502 Bad Gateway until a real response is received.  |
| `void` | [`submit`](#submit) `virtual` | Submits the internal HTTP request. |
| `void` | [`submit`](#submit-1) `virtual` | Submits the given HTTP request, replacing the internal request object. |
| `ssize_t` | [`send`](#send-10) `virtual` | Sends raw data to the peer, initiating a connection first if needed. Data is buffered internally until the connection is established.  |
| `void` | [`setReadStream`](#setreadstream) `virtual` | Sets the output stream to which incoming response body data is written. The stream pointer is owned by the connection and freed with it. Must be called before [submit()](#submit).  |
| `StreamT &` | [`readStream`](#readstream) `inline` | Returns a reference to the read stream cast to the specified type.  |

---

{#clientconnection-1}

#### ClientConnection

```cpp
ClientConnection(const URL & url, const net::TCPSocket::Ptr & socket)
```

Creates a [ClientConnection](#clientconnection) to the given [URL](#url), pre-populating the request URI and Host header. The response status is initialised to 502 Bad Gateway until a real response is received. 
#### Parameters
* `url` Target [URL](#url). Scheme, host, port and path are extracted automatically. 

* `socket` TCP socket to use. Defaults to a plain TCPSocket; pass an SSLSocket for HTTPS.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const [URL](#url) &` |  |
| `socket` | `const [net::TCPSocket::Ptr](net.md#ptr-9) &` |  |

---

{#submit}

#### submit

`virtual`

```cpp
virtual void submit()
```

Submits the internal HTTP request.

Calls [connect()](#connect-12) internally if the socket is not already connecting or connected. The actual request will be sent when the socket is connected. 
#### Exceptions
* `std::runtime_error` if already connecting.

---

{#submit-1}

#### submit

`virtual`

```cpp
virtual void submit(http::Request & req)
```

Submits the given HTTP request, replacing the internal request object.

Calls [connect()](#connect-12) internally if the socket is not already connecting or connected. The actual request will be sent when the socket is connected. 
#### Parameters
* `req` The HTTP request to send. Replaces the internal request. 

#### Exceptions
* `std::runtime_error` if already connecting.

| Parameter | Type | Description |
|-----------|------|-------------|
| `req` | `[http::Request](#request-4) &` |  |

---

{#send-10}

#### send

`virtual`

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Sends raw data to the peer, initiating a connection first if needed. Data is buffered internally until the connection is established. 
#### Parameters
* `data` Pointer to the data buffer. 

* `len` Number of bytes to send. 

* `flags` Socket send flags (unused for HTTP). 

#### Returns
Number of bytes sent or buffered.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

{#setreadstream}

#### setReadStream

`virtual`

```cpp
virtual void setReadStream(std::ostream * os)
```

Sets the output stream to which incoming response body data is written. The stream pointer is owned by the connection and freed with it. Must be called before [submit()](#submit). 
#### Parameters
* `os` Pointer to the output stream. Takes ownership. 

#### Exceptions
* `std::runtime_error` if already connecting.

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream *` |  |

---

{#readstream}

#### readStream

`inline`

```cpp
template<class StreamT> inline StreamT & readStream()
```

Returns a reference to the read stream cast to the specified type. 
#### Parameters
* `StreamT` The concrete stream type to cast to. 

#### Returns
Reference to the stream. 

#### Exceptions
* `std::runtime_error` if no read stream has been set. 

* `std::bad_cast` if the stream is not of type StreamT.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `URL` | [`_url`](#_url)  |  |
| `bool` | [`_connect`](#_connect)  |  |
| `bool` | [`_active`](#_active)  |  |
| `bool` | [`_complete`](#_complete)  |  |
| `std::vector< PendingWrite >` | [`_outgoingBuffer`](#_outgoingbuffer)  |  |
| `std::unique_ptr< std::ostream >` | [`_readStream`](#_readstream)  |  |

---

{#_url}

#### _url

```cpp
URL _url
```

---

{#_connect}

#### _connect

```cpp
bool _connect
```

---

{#_active}

#### _active

```cpp
bool _active
```

---

{#_complete}

#### _complete

```cpp
bool _complete
```

---

{#_outgoingbuffer}

#### _outgoingBuffer

```cpp
std::vector< PendingWrite > _outgoingBuffer
```

---

{#_readstream}

#### _readStream

```cpp
std::unique_ptr< std::ostream > _readStream
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`connect`](#connect-12) `virtual` | Connects to the server endpoint. All sent data is buffered until the connection is made. |
| `http::Message *` | [`incomingHeader`](#incomingheader) `virtual` | Returns the incoming HTTP message header (request or response depending on role). |
| `http::Message *` | [`outgoingHeader`](#outgoingheader) `virtual` | Returns the outgoing HTTP message header (request or response depending on role). |
| `bool` | [`onSocketConnect`](#onsocketconnect-2) `virtual` | [net::SocketAdapter](net.md#socketadapter) interface |

---

{#connect-12}

#### connect

`virtual`

```cpp
virtual void connect()
```

Connects to the server endpoint. All sent data is buffered until the connection is made.

---

{#incomingheader}

#### incomingHeader

`virtual`

```cpp
virtual http::Message * incomingHeader()
```

Returns the incoming HTTP message header (request or response depending on role).

---

{#outgoingheader}

#### outgoingHeader

`virtual`

```cpp
virtual http::Message * outgoingHeader()
```

Returns the outgoing HTTP message header (request or response depending on role).

---

{#onsocketconnect-2}

#### onSocketConnect

`virtual`

```cpp
virtual bool onSocketConnect(net::Socket & socket)
```

[net::SocketAdapter](net.md#socketadapter) interface

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-11)  |  |

---

{#ptr-11}

#### Ptr

```cpp
std::shared_ptr< ClientConnection > Ptr()
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onHeaders`](#onheaders) `virtual` | [Connection](#connection-1) interface. |
| `void` | [`onPayload`](#onpayload) `virtual` | Called for each chunk of incoming response body data. |
| `void` | [`onComplete`](#oncomplete) `virtual` | Called when the full HTTP response has been received. |
| `void` | [`onClose`](#onclose-3) `virtual` | Called when the connection is closed. |

---

{#onheaders}

#### onHeaders

`virtual`

```cpp
virtual void onHeaders()
```

[Connection](#connection-1) interface.

Called when the response headers have been parsed.

---

{#onpayload}

#### onPayload

`virtual`

```cpp
virtual void onPayload(const MutableBuffer & buffer)
```

Called for each chunk of incoming response body data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |

---

{#oncomplete}

#### onComplete

`virtual`

```cpp
virtual void onComplete()
```

Called when the full HTTP response has been received.

---

{#onclose-3}

#### onClose

`virtual`

```cpp
virtual void onClose()
```

Called when the connection is closed.

{#pendingwrite}

## PendingWrite

```cpp
#include <icy/http/client.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Buffer` | [`data`](#data-1)  |  |
| `int` | [`flags`](#flags)  |  |

---

{#data-1}

#### data

```cpp
Buffer data
```

---

{#flags}

#### flags

```cpp
int flags = 0
```

{#connection-1}

## Connection

```cpp
#include <icy/http/connection.h>
```

> **Inherits:** [`SocketAdapter`](net.md#socketadapter)
> **Subclassed by:** [`ClientConnection`](#clientconnection), [`ServerConnection`](#serverconnection)

Base HTTP connection managing socket I/O and message lifecycle

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Connection`](#connection-2)  | Creates a [Connection](#connection-1) using the given TCP socket.  |
| `void` | [`onHeaders`](#onheaders-1)  | Called when the incoming HTTP headers have been fully parsed. |
| `void` | [`onPayload`](#onpayload-1)  | Called for each chunk of incoming body data after headers are complete.  |
| `void` | [`onComplete`](#oncomplete-1)  | Called when the incoming HTTP message is fully received. |
| `void` | [`onClose`](#onclose-4)  | Called when the connection is closed. |
| `ssize_t` | [`send`](#send-11) `virtual` | Send raw data to the peer. |
| `ssize_t` | [`sendOwned`](#sendowned-10) `virtual` | Send an owned payload buffer to the peer. |
| `ssize_t` | [`sendHeader`](#sendheader) `virtual` | Send the outdoing HTTP header. |
| `void` | [`close`](#close-20) `virtual` | Close the connection and schedule the object for deferred deletion. |
| `void` | [`markActive`](#markactive) `virtual` `inline` | Marks the connection as active. [Server](#server) connections override this to refresh the idle timer. |
| `void` | [`beginStreaming`](#beginstreaming) `virtual` `inline` | Explicitly enter long-lived streaming mode. Base connections ignore this; server connections use it to disable keep-alive idle reaping while a response stream is active. |
| `void` | [`endStreaming`](#endstreaming) `virtual` `inline` | Exit long-lived streaming mode. |
| `bool` | [`closed`](#closed-4) `const` | Return true if the connection is closed. |
| `icy::Error` | [`error`](#error-9) `const` | Return the error object if any. |
| `bool` | [`headerAutoSendEnabled`](#headerautosendenabled) `const` | Return true if headers should be automatically sent. |
| `void` | [`setHeaderAutoSendEnabled`](#setheaderautosendenabled)  | Enable or disable automatic header emission for the next outgoing send path. |
| `void` | [`replaceAdapter`](#replaceadapter) `virtual` | Assign the new [ConnectionAdapter](#connectionadapter) and setup the chain. The flow is: [Connection](#connection-1) <-> [ConnectionAdapter](#connectionadapter) <-> Socket. Takes ownership of the adapter (deferred deletion via uv loop). |
| `void` | [`replaceAdapter`](#replaceadapter-1) `virtual` | Overload for nullptr (used in destructor to clear adapter). |
| `bool` | [`secure`](#secure) `const` | Return true if the connection uses TLS/SSL. |
| `net::TCPSocket::Ptr &` | [`socket`](#socket-5)  | Return the underlying socket pointer. |
| `net::SocketAdapter *` | [`adapter`](#adapter) `const` | Return the underlying adapter pointer. |
| `Request &` | [`request`](#request-3)  | The HTTP request headers. |
| `Response &` | [`response`](#response)  | The HTTP response headers. |
| `http::Message *` | [`incomingHeader`](#incomingheader-1)  | Returns the incoming HTTP message header (request or response depending on role). |
| `http::Message *` | [`outgoingHeader`](#outgoingheader-1)  | Returns the outgoing HTTP message header (request or response depending on role). |

---

{#connection-2}

#### Connection

```cpp
Connection(const net::TCPSocket::Ptr & socket)
```

Creates a [Connection](#connection-1) using the given TCP socket. 
#### Parameters
* `socket` The TCP socket to use for I/O. Defaults to a new TCPSocket.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const [net::TCPSocket::Ptr](net.md#ptr-9) &` |  |

---

{#onheaders-1}

#### onHeaders

```cpp
void onHeaders()
```

Called when the incoming HTTP headers have been fully parsed.

---

{#onpayload-1}

#### onPayload

```cpp
void onPayload(const MutableBuffer & buffer)
```

Called for each chunk of incoming body data after headers are complete. 
#### Parameters
* `buffer` Buffer containing the received data chunk.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |

---

{#oncomplete-1}

#### onComplete

```cpp
void onComplete()
```

Called when the incoming HTTP message is fully received.

---

{#onclose-4}

#### onClose

```cpp
void onClose()
```

Called when the connection is closed.

---

{#send-11}

#### send

`virtual`

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Send raw data to the peer.

This is the zero-copy fast path. The caller retains ownership of the payload until the underlying async write completes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

{#sendowned-10}

#### sendOwned

`virtual`

```cpp
virtual ssize_t sendOwned(Buffer && buffer, int flags)
```

Send an owned payload buffer to the peer.

Use this when the payload does not naturally outlive the current scope.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `[Buffer](base.md#buffer-2) &&` |  |
| `flags` | `int` |  |

---

{#sendheader}

#### sendHeader

`virtual`

```cpp
virtual ssize_t sendHeader()
```

Send the outdoing HTTP header.

---

{#close-20}

#### close

`virtual`

```cpp
virtual void close()
```

Close the connection and schedule the object for deferred deletion.

---

{#markactive}

#### markActive

`virtual` `inline`

```cpp
virtual inline void markActive()
```

Marks the connection as active. [Server](#server) connections override this to refresh the idle timer.

---

{#beginstreaming}

#### beginStreaming

`virtual` `inline`

```cpp
virtual inline void beginStreaming()
```

Explicitly enter long-lived streaming mode. Base connections ignore this; server connections use it to disable keep-alive idle reaping while a response stream is active.

---

{#endstreaming}

#### endStreaming

`virtual` `inline`

```cpp
virtual inline void endStreaming()
```

Exit long-lived streaming mode.

---

{#closed-4}

#### closed

`const`

```cpp
bool closed() const
```

Return true if the connection is closed.

---

{#error-9}

#### error

`const`

```cpp
icy::Error error() const
```

Return the error object if any.

---

{#headerautosendenabled}

#### headerAutoSendEnabled

`const`

```cpp
bool headerAutoSendEnabled() const
```

Return true if headers should be automatically sent.

---

{#setheaderautosendenabled}

#### setHeaderAutoSendEnabled

```cpp
void setHeaderAutoSendEnabled(bool enabled)
```

Enable or disable automatic header emission for the next outgoing send path.

| Parameter | Type | Description |
|-----------|------|-------------|
| `enabled` | `bool` |  |

---

{#replaceadapter}

#### replaceAdapter

`virtual`

```cpp
virtual void replaceAdapter(std::unique_ptr< net::SocketAdapter > adapter)
```

Assign the new [ConnectionAdapter](#connectionadapter) and setup the chain. The flow is: [Connection](#connection-1) <-> [ConnectionAdapter](#connectionadapter) <-> Socket. Takes ownership of the adapter (deferred deletion via uv loop).

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `std::unique_ptr< [net::SocketAdapter](net.md#socketadapter) >` |  |

---

{#replaceadapter-1}

#### replaceAdapter

`virtual`

```cpp
virtual void replaceAdapter(std::nullptr_t)
```

Overload for nullptr (used in destructor to clear adapter).

---

{#secure}

#### secure

`const`

```cpp
bool secure() const
```

Return true if the connection uses TLS/SSL.

---

{#socket-5}

#### socket

```cpp
net::TCPSocket::Ptr & socket()
```

Return the underlying socket pointer.

---

{#adapter}

#### adapter

`const`

```cpp
net::SocketAdapter * adapter() const
```

Return the underlying adapter pointer.

---

{#request-3}

#### request

```cpp
Request & request()
```

The HTTP request headers.

---

{#response}

#### response

```cpp
Response & response()
```

The HTTP response headers.

---

{#incomingheader-1}

#### incomingHeader

```cpp
http::Message * incomingHeader()
```

Returns the incoming HTTP message header (request or response depending on role).

---

{#outgoingheader-1}

#### outgoingHeader

```cpp
http::Message * outgoingHeader()
```

Returns the outgoing HTTP message header (request or response depending on role).

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `net::TCPSocket::Ptr` | [`_socket`](#_socket-1)  |  |
| `net::SocketAdapter *` | [`_adapter`](#_adapter)  |  |
| `Request` | [`_request`](#_request)  |  |
| `Response` | [`_response`](#_response)  |  |
| `icy::Error` | [`_error`](#_error-2)  |  |
| `bool` | [`_closed`](#_closed)  |  |
| `bool` | [`_shouldSendHeader`](#_shouldsendheader)  |  |

---

{#_socket-1}

#### _socket

```cpp
net::TCPSocket::Ptr _socket
```

---

{#_adapter}

#### _adapter

```cpp
net::SocketAdapter * _adapter
```

---

{#_request}

#### _request

```cpp
Request _request
```

---

{#_response}

#### _response

```cpp
Response _response
```

---

{#_error-2}

#### _error

```cpp
icy::Error _error
```

---

{#_closed}

#### _closed

```cpp
bool _closed
```

---

{#_shouldsendheader}

#### _shouldSendHeader

```cpp
bool _shouldSendHeader
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`setError`](#seterror-4) `virtual` | Set the internal error. Note: Setting the error does not `[close()](#close-20)` the connection. |
| `bool` | [`onSocketConnect`](#onsocketconnect-3) `virtual` | [net::SocketAdapter](net.md#socketadapter) interface |
| `bool` | [`onSocketRecv`](#onsocketrecv-3) `virtual` | Called when data is received from the socket. Forwards the event to all registered receivers in priority order.  |
| `bool` | [`onSocketError`](#onsocketerror-2) `virtual` | Called when the socket encounters an error. Forwards the event to all registered receivers in priority order.  |
| `bool` | [`onSocketClose`](#onsocketclose-2) `virtual` | Called when the socket is closed. Forwards the event to all registered receivers in priority order.  |

---

{#seterror-4}

#### setError

`virtual`

```cpp
virtual void setError(const icy::Error & err)
```

Set the internal error. Note: Setting the error does not `[close()](#close-20)` the connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [icy::Error](base.md#error) &` |  |

---

{#onsocketconnect-3}

#### onSocketConnect

`virtual`

```cpp
virtual bool onSocketConnect(net::Socket & socket)
```

[net::SocketAdapter](net.md#socketadapter) interface

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |

---

{#onsocketrecv-3}

#### onSocketRecv

`virtual`

```cpp
virtual bool onSocketRecv(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

Called when data is received from the socket. Forwards the event to all registered receivers in priority order. 
#### Parameters
* `socket` The socket that received data. 

* `buffer` The received data buffer. 

* `peerAddress` Address of the sender. 

#### Returns
true to stop propagation to subsequent receivers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |
| `peerAddress` | `const [net::Address](net.md#address) &` |  |

---

{#onsocketerror-2}

#### onSocketError

`virtual`

```cpp
virtual bool onSocketError(net::Socket & socket, const icy::Error & error)
```

Called when the socket encounters an error. Forwards the event to all registered receivers in priority order. 
#### Parameters
* `socket` The socket that encountered the error. 

* `error` [Error](base.md#error) details. 

#### Returns
true to stop propagation to subsequent receivers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |
| `error` | `const [icy::Error](base.md#error) &` |  |

---

{#onsocketclose-2}

#### onSocketClose

`virtual`

```cpp
virtual bool onSocketClose(net::Socket & socket)
```

Called when the socket is closed. Forwards the event to all registered receivers in priority order. 
#### Parameters
* `socket` The socket that was closed. 

#### Returns
true to stop propagation to subsequent receivers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-12)  |  |

---

{#ptr-12}

#### Ptr

```cpp
std::shared_ptr< Connection > Ptr()
```

{#connectionadapter}

## ConnectionAdapter

```cpp
#include <icy/http/connection.h>
```

> **Inherits:** [`ParserObserver`](#parserobserver), [`SocketAdapter`](net.md#socketadapter)

Default HTTP socket adapter for reading and writing HTTP messages

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ConnectionAdapter`](#connectionadapter-1)  | Creates a [ConnectionAdapter](#connectionadapter) for the given connection.  |
| `ssize_t` | [`send`](#send-12) `virtual` | Sends data to the peer, flushing the outgoing HTTP header first if needed.  |
| `ssize_t` | [`sendOwned`](#sendowned-11) `virtual` | Sends an owned payload buffer to the connected peer. |
| `void` | [`removeReceiver`](#removereceiver-2)  | Remove the given receiver. |
| `Parser &` | [`parser`](#parser)  | Returns the HTTP parser instance. |
| `Connection *` | [`connection`](#connection-3)  | Returns the owning [Connection](#connection-1) pointer, or nullptr if detached. |
| `void` | [`reset`](#reset-7)  | Resets the adapter for reuse with a new socket and request. Clears the parser state and re-wires the sender without reallocating. Used by the connection pool to avoid destroying/recreating adapters.  |

---

{#connectionadapter-1}

#### ConnectionAdapter

```cpp
ConnectionAdapter(Connection * connection, llhttp_type_t type)
```

Creates a [ConnectionAdapter](#connectionadapter) for the given connection. 
#### Parameters
* `connection` Owning HTTP connection. 

* `type` [Parser](#parser-1) type: HTTP_REQUEST for server side, HTTP_RESPONSE for client side.

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection](#connection-1) *` |  |
| `type` | `llhttp_type_t` |  |

---

{#send-12}

#### send

`virtual`

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Sends data to the peer, flushing the outgoing HTTP header first if needed. 
#### Parameters
* `data` Pointer to the data buffer. 

* `len` Number of bytes to send. 

* `flags` Send flags (unused for HTTP, used for WebSocket frame type). 

#### Returns
Number of bytes sent, or -1 on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

{#sendowned-11}

#### sendOwned

`virtual`

```cpp
virtual ssize_t sendOwned(Buffer && buffer, int flags)
```

Sends an owned payload buffer to the connected peer.

The buffer is moved through the adapter chain and retained by the transport layer until async write completion.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `[Buffer](base.md#buffer-2) &&` |  |
| `flags` | `int` |  |

---

{#removereceiver-2}

#### removeReceiver

```cpp
void removeReceiver(SocketAdapter * adapter)
```

Remove the given receiver.

By default this function does nothing unless the given receiver matches the current receiver.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adapter` | `[SocketAdapter](net.md#socketadapter-1) *` |  |

---

{#parser}

#### parser

```cpp
Parser & parser()
```

Returns the HTTP parser instance.

---

{#connection-3}

#### connection

```cpp
Connection * connection()
```

Returns the owning [Connection](#connection-1) pointer, or nullptr if detached.

---

{#reset-7}

#### reset

```cpp
void reset(net::SocketAdapter * sender, http::Request * request)
```

Resets the adapter for reuse with a new socket and request. Clears the parser state and re-wires the sender without reallocating. Used by the connection pool to avoid destroying/recreating adapters. 
#### Parameters
* `sender` New socket adapter to send data through. 

* `request` New HTTP request object for the parser to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `[net::SocketAdapter](net.md#socketadapter) *` |  |
| `request` | `[http::Request](#request-4) *` |  |

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Connection *` | [`_connection`](#_connection)  |  |
| `Parser` | [`_parser`](#_parser)  |  |

---

{#_connection}

#### _connection

```cpp
Connection * _connection
```

---

{#_parser}

#### _parser

```cpp
Parser _parser
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`onSocketRecv`](#onsocketrecv-4) `virtual` | SocketAdapter interface. |
| `void` | [`onParserHeader`](#onparserheader) `virtual` | HTTP [Parser](#parser-1) interface. |
| `void` | [`onParserHeadersEnd`](#onparserheadersend) `virtual` | Called when all HTTP headers have been parsed.  |
| `void` | [`onParserChunk`](#onparserchunk) `virtual` | Called for each chunk of body data received.  |
| `void` | [`onParserError`](#onparsererror) `virtual` | Called when a parse error occurs.  |
| `void` | [`onParserEnd`](#onparserend) `virtual` | Called when the HTTP message is fully parsed. |

---

{#onsocketrecv-4}

#### onSocketRecv

`virtual`

```cpp
virtual bool onSocketRecv(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

SocketAdapter interface.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |
| `peerAddress` | `const [net::Address](net.md#address) &` |  |

---

{#onparserheader}

#### onParserHeader

`virtual`

```cpp
virtual void onParserHeader(const std::string & name, const std::string & value)
```

HTTP [Parser](#parser-1) interface.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

{#onparserheadersend}

#### onParserHeadersEnd

`virtual`

```cpp
virtual void onParserHeadersEnd(bool upgrade)
```

Called when all HTTP headers have been parsed. 
#### Parameters
* `upgrade` True if the connection should be upgraded (e.g. to WebSocket).

| Parameter | Type | Description |
|-----------|------|-------------|
| `upgrade` | `bool` |  |

---

{#onparserchunk}

#### onParserChunk

`virtual`

```cpp
virtual void onParserChunk(const char * data, size_t len)
```

Called for each chunk of body data received. 
#### Parameters
* `data` Pointer to the body data chunk. 

* `len` Length of the chunk in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

{#onparsererror}

#### onParserError

`virtual`

```cpp
virtual void onParserError(const icy::Error & err)
```

Called when a parse error occurs. 
#### Parameters
* `err` [Error](base.md#error) details from llhttp.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [icy::Error](base.md#error) &` |  |

---

{#onparserend}

#### onParserEnd

`virtual`

```cpp
virtual void onParserEnd()
```

Called when the HTTP message is fully parsed.

{#connectionpool}

## ConnectionPool

```cpp
#include <icy/http/server.h>
```

LIFO connection pool for reusing [ServerConnection](#serverconnection) objects. Avoids per-request heap allocation by resetting and reusing connections instead of destroying and recreating them.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `ServerConnection::Ptr` | [`acquire`](#acquire) `inline` | Takes a connection from the pool for reuse.  |
| `bool` | [`release`](#release) `inline` | Returns a connection to the pool after use.  |
| `void` | [`setMaxSize`](#setmaxsize) `inline` | Sets the maximum number of connections the pool will hold.  |
| `size_t` | [`size`](#size) `const` `inline` | Returns the current number of connections held in the pool. |

---

{#acquire}

#### acquire

`inline`

```cpp
inline ServerConnection::Ptr acquire()
```

Takes a connection from the pool for reuse. 
#### Returns
A pooled connection, or nullptr if the pool is empty.

---

{#release}

#### release

`inline`

```cpp
inline bool release(ServerConnection::Ptr & conn)
```

Returns a connection to the pool after use. 
#### Parameters
* `conn` The connection to return. 

#### Returns
true if accepted into the pool; false if the pool is full.

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ServerConnection::Ptr](#ptr-13) &` |  |

---

{#setmaxsize}

#### setMaxSize

`inline`

```cpp
inline void setMaxSize(size_t n)
```

Sets the maximum number of connections the pool will hold. 
#### Parameters
* `n` Maximum pool capacity.

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `size_t` |  |

---

{#size}

#### size

`const` `inline`

```cpp
inline size_t size() const
```

Returns the current number of connections held in the pool.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< ServerConnection::Ptr >` | [`_pool`](#_pool)  |  |
| `size_t` | [`_maxSize`](#_maxsize)  |  |

---

{#_pool}

#### _pool

```cpp
std::vector< ServerConnection::Ptr > _pool
```

---

{#_maxsize}

#### _maxSize

```cpp
size_t _maxSize = 128
```

{#connectionstream}

## ConnectionStream

```cpp
#include <icy/http/connection.h>
```

> **Inherits:** [`SocketAdapter`](net.md#socketadapter)

Packet stream wrapper for a HTTP connection.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketStream` | [`Outgoing`](#outgoing)  | The Outgoing stream is responsible for packetizing raw application data into the agreed upon HTTP format and sending it to the peer. |
| `PacketStream` | [`Incoming`](#incoming)  | The Incoming stream emits incoming HTTP packets for processing by the application. |
| `ProgressSignal` | [`IncomingProgress`](#incomingprogress-1)  | Fired on download progress. |
| `ProgressSignal` | [`OutgoingProgress`](#outgoingprogress)  | Fired on upload progress. |

---

{#outgoing}

#### Outgoing

```cpp
PacketStream Outgoing
```

The Outgoing stream is responsible for packetizing raw application data into the agreed upon HTTP format and sending it to the peer.

---

{#incoming}

#### Incoming

```cpp
PacketStream Incoming
```

The Incoming stream emits incoming HTTP packets for processing by the application.

This is useful for example when writing incoming data to a file.

---

{#incomingprogress-1}

#### IncomingProgress

```cpp
ProgressSignal IncomingProgress
```

Fired on download progress.

---

{#outgoingprogress}

#### OutgoingProgress

```cpp
ProgressSignal OutgoingProgress
```

Fired on upload progress.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ConnectionStream`](#connectionstream-1)  | Creates a [ConnectionStream](#connectionstream) wrapping the given HTTP connection. Wires the Outgoing stream emitter to the connection adapter and registers this stream to receive incoming data from the adapter.  |
| `ssize_t` | [`send`](#send-13) `virtual` | Send data via the Outgoing stream. |
| `Connection::Ptr` | [`connection`](#connection-4)  | Return a reference to the underlying connection. |

---

{#connectionstream-1}

#### ConnectionStream

```cpp
ConnectionStream(Connection::Ptr connection)
```

Creates a [ConnectionStream](#connectionstream) wrapping the given HTTP connection. Wires the Outgoing stream emitter to the connection adapter and registers this stream to receive incoming data from the adapter. 
#### Parameters
* `connection` The HTTP connection to wrap.

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection::Ptr](#ptr-12)` |  |

---

{#send-13}

#### send

`virtual`

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

{#connection-4}

#### connection

```cpp
Connection::Ptr connection()
```

Return a reference to the underlying connection.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Connection::Ptr` | [`_connection`](#_connection-1)  |  |

---

{#_connection-1}

#### _connection

```cpp
Connection::Ptr _connection
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`onSocketRecv`](#onsocketrecv-5) `virtual` | Called when data is received from the socket. Forwards the event to all registered receivers in priority order.  |

---

{#onsocketrecv-5}

#### onSocketRecv

`virtual`

```cpp
virtual bool onSocketRecv(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

Called when data is received from the socket. Forwards the event to all registered receivers in priority order. 
#### Parameters
* `socket` The socket that received data. 

* `buffer` The received data buffer. 

* `peerAddress` Address of the sender. 

#### Returns
true to stop propagation to subsequent receivers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |
| `peerAddress` | `const [net::Address](net.md#address) &` |  |

{#cookie}

## Cookie

```cpp
#include <icy/http/cookie.h>
```

This class represents a HTTP [Cookie](#cookie).

A cookie is a small amount of information sent by a Web server to a Web browser, saved by the browser, and later sent back to the server. A cookie's value can uniquely identify a client, so cookies are commonly used for session management.

A cookie has a name, a single value, and optional attributes such as a comment, path and domain qualifiers, a maximum age, and a version number.

This class supports both the Version 0 (by Netscape) and Version 1 (by RFC 2109) cookie specifications. By default, cookies are created using Version 0 to ensure the best interoperability.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Cookie`](#cookie-1)  | Creates an empty [Cookie](#cookie). |
|  | [`Cookie`](#cookie-2) `explicit` | Creates a cookie with the given name. The cookie never expires. |
|  | [`Cookie`](#cookie-3) `explicit` | Creates a cookie from the given [NVCollection](base.md#nvcollection). |
|  | [`Cookie`](#cookie-4)  | Creates a cookie with the given name and value. The cookie never expires. |
|  | [`Cookie`](#cookie-5)  | Creates the [Cookie](#cookie) by copying another one. |
|  | [`~Cookie`](#cookie-6)  | Destroys the [Cookie](#cookie). |
| `Cookie &` | [`operator=`](#operator-14)  | Assigns a cookie. |
| `void` | [`setVersion`](#setversion)  | Sets the version of the cookie. |
| `int` | [`getVersion`](#getversion) `const` `inline` | Returns the version of the cookie, which is either 0 or 1. |
| `void` | [`setName`](#setname)  | Sets the name of the cookie. |
| `const std::string &` | [`getName`](#getname) `const` `inline` | Returns the name of the cookie. |
| `void` | [`setValue`](#setvalue)  | Sets the value of the cookie. |
| `const std::string &` | [`getValue`](#getvalue) `const` `inline` | Returns the value of the cookie. |
| `void` | [`setComment`](#setcomment)  | Sets the comment for the cookie. |
| `const std::string &` | [`getComment`](#getcomment) `const` `inline` | Returns the comment for the cookie. |
| `void` | [`setDomain`](#setdomain)  | Sets the domain for the cookie. |
| `const std::string &` | [`getDomain`](#getdomain) `const` `inline` | Returns the domain for the cookie. |
| `void` | [`setPath`](#setpath)  | Sets the path for the cookie. |
| `const std::string &` | [`getPath`](#getpath) `const` `inline` | Returns the path for the cookie. |
| `void` | [`setSecure`](#setsecure)  | Sets the value of the secure flag for the cookie. |
| `bool` | [`getSecure`](#getsecure) `const` `inline` | Returns the value of the secure flag for the cookie. |
| `void` | [`setMaxAge`](#setmaxage)  | Sets the maximum age in seconds for the cookie. |
| `int` | [`getMaxAge`](#getmaxage) `const` `inline` | Returns the maximum age in seconds for the cookie. |
| `void` | [`setHttpOnly`](#sethttponly)  | Sets the HttpOnly flag for the cookie. |
| `bool` | [`getHttpOnly`](#gethttponly) `const` `inline` | Returns true if the cookie's HttpOnly flag is set. |
| `std::string` | [`toString`](#tostring-5) `const` | Returns a std::string representation of the cookie, suitable for use in a Set-Cookie header. |

---

{#cookie-1}

#### Cookie

```cpp
Cookie()
```

Creates an empty [Cookie](#cookie).

---

{#cookie-2}

#### Cookie

`explicit`

```cpp
explicit Cookie(const std::string & name)
```

Creates a cookie with the given name. The cookie never expires.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

{#cookie-3}

#### Cookie

`explicit`

```cpp
explicit Cookie(const NVCollection & nvc)
```

Creates a cookie from the given [NVCollection](base.md#nvcollection).

| Parameter | Type | Description |
|-----------|------|-------------|
| `nvc` | `const [NVCollection](base.md#nvcollection) &` |  |

---

{#cookie-4}

#### Cookie

```cpp
Cookie(const std::string & name, const std::string & value)
```

Creates a cookie with the given name and value. The cookie never expires.

Note: If value contains whitespace or non-alphanumeric characters, the value should be escaped by calling [escape()](#escape) before passing it to the constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

{#cookie-5}

#### Cookie

```cpp
Cookie(const Cookie & cookie)
```

Creates the [Cookie](#cookie) by copying another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookie` | `const [Cookie](#cookie) &` |  |

---

{#cookie-6}

#### ~Cookie

```cpp
~Cookie()
```

Destroys the [Cookie](#cookie).

---

{#operator-14}

#### operator=

```cpp
Cookie & operator=(const Cookie & cookie)
```

Assigns a cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookie` | `const [Cookie](#cookie) &` |  |

---

{#setversion}

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

{#getversion}

#### getVersion

`const` `inline`

```cpp
inline int getVersion() const
```

Returns the version of the cookie, which is either 0 or 1.

---

{#setname}

#### setName

```cpp
void setName(const std::string & name)
```

Sets the name of the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

{#getname}

#### getName

`const` `inline`

```cpp
inline const std::string & getName() const
```

Returns the name of the cookie.

---

{#setvalue}

#### setValue

```cpp
void setValue(const std::string & value)
```

Sets the value of the cookie.

According to the cookie specification, the size of the value should not exceed 4 Kbytes.

Note: If value contains whitespace or non-alphanumeric characters, the value should be escaped by calling [escape()](#escape) prior to passing it to [setName()](#setname).

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

{#getvalue}

#### getValue

`const` `inline`

```cpp
inline const std::string & getValue() const
```

Returns the value of the cookie.

---

{#setcomment}

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

{#getcomment}

#### getComment

`const` `inline`

```cpp
inline const std::string & getComment() const
```

Returns the comment for the cookie.

---

{#setdomain}

#### setDomain

```cpp
void setDomain(const std::string & domain)
```

Sets the domain for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `domain` | `const std::string &` |  |

---

{#getdomain}

#### getDomain

`const` `inline`

```cpp
inline const std::string & getDomain() const
```

Returns the domain for the cookie.

---

{#setpath}

#### setPath

```cpp
void setPath(const std::string & path)
```

Sets the path for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

{#getpath}

#### getPath

`const` `inline`

```cpp
inline const std::string & getPath() const
```

Returns the path for the cookie.

---

{#setsecure}

#### setSecure

```cpp
void setSecure(bool secure)
```

Sets the value of the secure flag for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `secure` | `bool` |  |

---

{#getsecure}

#### getSecure

`const` `inline`

```cpp
inline bool getSecure() const
```

Returns the value of the secure flag for the cookie.

---

{#setmaxage}

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

{#getmaxage}

#### getMaxAge

`const` `inline`

```cpp
inline int getMaxAge() const
```

Returns the maximum age in seconds for the cookie.

---

{#sethttponly}

#### setHttpOnly

```cpp
void setHttpOnly(bool flag)
```

Sets the HttpOnly flag for the cookie.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

{#gethttponly}

#### getHttpOnly

`const` `inline`

```cpp
inline bool getHttpOnly() const
```

Returns true if the cookie's HttpOnly flag is set.

---

{#tostring-5}

#### toString

`const`

```cpp
std::string toString() const
```

Returns a std::string representation of the cookie, suitable for use in a Set-Cookie header.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`escape`](#escape) `static` | Escapes the given std::string by replacing all non-alphanumeric characters with escape sequences in the form xx, where xx is the hexadecimal character code. |
| `std::string` | [`unescape`](#unescape) `static` | Unescapes the given std::string by replacing all escape sequences in the form xx with the respective characters. |

---

{#escape}

#### escape

`static`

```cpp
static std::string escape(std::string_view str)
```

Escapes the given std::string by replacing all non-alphanumeric characters with escape sequences in the form xx, where xx is the hexadecimal character code.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |

---

{#unescape}

#### unescape

`static`

```cpp
static std::string unescape(std::string_view str)
```

Unescapes the given std::string by replacing all escape sequences in the form xx with the respective characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`_version`](#_version)  |  |
| `std::string` | [`_name`](#_name)  |  |
| `std::string` | [`_value`](#_value)  |  |
| `std::string` | [`_comment`](#_comment)  |  |
| `std::string` | [`_domain`](#_domain)  |  |
| `std::string` | [`_path`](#_path)  |  |
| `bool` | [`_secure`](#_secure)  |  |
| `int` | [`_maxAge`](#_maxage)  |  |
| `bool` | [`_httpOnly`](#_httponly)  |  |

---

{#_version}

#### _version

```cpp
int _version
```

---

{#_name}

#### _name

```cpp
std::string _name
```

---

{#_value}

#### _value

```cpp
std::string _value
```

---

{#_comment}

#### _comment

```cpp
std::string _comment
```

---

{#_domain}

#### _domain

```cpp
std::string _domain
```

---

{#_path}

#### _path

```cpp
std::string _path
```

---

{#_secure}

#### _secure

```cpp
bool _secure
```

---

{#_maxage}

#### _maxAge

```cpp
int _maxAge
```

---

{#_httponly}

#### _httpOnly

```cpp
bool _httpOnly
```

{#filepart}

## FilePart

```cpp
#include <icy/http/form.h>
```

> **Inherits:** [`FormPart`](#formpart)

An implementation of [FilePart](#filepart) for plain files.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FilePart`](#filepart-1)  | Creates the [FilePart](#filepart) for the given path. |
|  | [`FilePart`](#filepart-2)  | Creates the [FilePart](#filepart) for the given path and MIME type. |
|  | [`FilePart`](#filepart-3)  | Creates the [FilePart](#filepart) for the given path and MIME type. The given filename is used as part filename (see [filename()](#filename-1)) only. |
|  | [`~FilePart`](#filepart-4) `virtual` | Destroys the [FilePart](#filepart). |
| `void` | [`open`](#open-4) `virtual` | Opens the file for reading.  |
| `void` | [`reset`](#reset-8) `virtual` | Resets the file stream to the beginning and clears initial-write state. |
| `bool` | [`writeChunk`](#writechunk) `virtual` | Writes the next chunk of the file to the [FormWriter](#formwriter).  |
| `void` | [`write`](#write-2) `virtual` | Writes the entire file content to the [FormWriter](#formwriter).  |
| `void` | [`write`](#write-3) `virtual` | Writes the entire file content to an output stream (used for content-length calculation).  |
| `const std::string &` | [`filename`](#filename-1) `const` | Returns the filename component of the file path (not the full path). |
| `std::ifstream &` | [`stream`](#stream-3)  | Returns a reference to the underlying file input stream. |
| `uint64_t` | [`length`](#length-1) `virtual` `const` | Returns the total file size in bytes. |

---

{#filepart-1}

#### FilePart

```cpp
FilePart(const std::string & path)
```

Creates the [FilePart](#filepart) for the given path.

The MIME type is set to application/octet-stream.

Throws an FileException if the file cannot be opened.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

{#filepart-2}

#### FilePart

```cpp
FilePart(const std::string & path, const std::string & contentType)
```

Creates the [FilePart](#filepart) for the given path and MIME type.

Throws an FileException if the file cannot be opened.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `contentType` | `const std::string &` |  |

---

{#filepart-3}

#### FilePart

```cpp
FilePart(const std::string & path, const std::string & filename, const std::string & contentType)
```

Creates the [FilePart](#filepart) for the given path and MIME type. The given filename is used as part filename (see [filename()](#filename-1)) only.

Throws an FileException if the file cannot be opened.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `filename` | `const std::string &` |  |
| `contentType` | `const std::string &` |  |

---

{#filepart-4}

#### ~FilePart

`virtual`

```cpp
virtual ~FilePart()
```

Destroys the [FilePart](#filepart).

---

{#open-4}

#### open

`virtual`

```cpp
virtual void open()
```

Opens the file for reading. 
#### Exceptions
* `std::runtime_error` if the file cannot be opened.

---

{#reset-8}

#### reset

`virtual`

```cpp
virtual void reset()
```

Resets the file stream to the beginning and clears initial-write state.

---

{#writechunk}

#### writeChunk

`virtual`

```cpp
virtual bool writeChunk(FormWriter & writer)
```

Writes the next chunk of the file to the [FormWriter](#formwriter). 
#### Parameters
* `writer` The [FormWriter](#formwriter) to send the chunk through. 

#### Returns
true if more data remains; false when the file is fully sent.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#formwriter) &` |  |

---

{#write-2}

#### write

`virtual`

```cpp
virtual void write(FormWriter & writer)
```

Writes the entire file content to the [FormWriter](#formwriter). 
#### Parameters
* `writer` The [FormWriter](#formwriter) to send data through.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#formwriter) &` |  |

---

{#write-3}

#### write

`virtual`

```cpp
virtual void write(std::ostream & ostr)
```

Writes the entire file content to an output stream (used for content-length calculation). 
#### Parameters
* `ostr` Output stream to write to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#filename-1}

#### filename

`const`

```cpp
const std::string & filename() const
```

Returns the filename component of the file path (not the full path).

---

{#stream-3}

#### stream

```cpp
std::ifstream & stream()
```

Returns a reference to the underlying file input stream.

---

{#length-1}

#### length

`virtual` `const`

```cpp
virtual uint64_t length() const
```

Returns the total file size in bytes.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_path`](#_path-1)  |  |
| `std::string` | [`_filename`](#_filename-1)  |  |
| `std::ifstream` | [`_istr`](#_istr)  |  |
| `uint64_t` | [`_fileSize`](#_filesize)  |  |

---

{#_path-1}

#### _path

```cpp
std::string _path
```

---

{#_filename-1}

#### _filename

```cpp
std::string _filename
```

---

{#_istr}

#### _istr

```cpp
std::ifstream _istr
```

---

{#_filesize}

#### _fileSize

```cpp
uint64_t _fileSize
```

{#formpart}

## FormPart

```cpp
#include <icy/http/form.h>
```

> **Subclassed by:** [`FilePart`](#filepart), [`StringPart`](#stringpart)

An implementation of [FormPart](#formpart).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FormPart`](#formpart-1)  | Creates the [FormPart](#formpart) with the given MIME content type.  |
|  | [`~FormPart`](#formpart-2) `virtual` | Destroys the [FormPart](#formpart). |
| `void` | [`reset`](#reset-9) `virtual` | Resets the internal state and write position to the beginning. Called by [FormWriter](#formwriter) when retrying or recalculating content length. |
| `bool` | [`writeChunk`](#writechunk-1)  | Writes the next chunk of data to the [FormWriter](#formwriter).  |
| `void` | [`write`](#write-4)  | Writes the entire part data to the [FormWriter](#formwriter) in one call.  |
| `void` | [`write`](#write-5)  | Writes the entire part data to an output stream (used for content-length calculation).  |
| `NVCollection &` | [`headers`](#headers-1)  | Returns the extra MIME headers for this part (e.g. Content-Disposition). |
| `bool` | [`initialWrite`](#initialwrite) `virtual` `const` | Returns true if this is the first write call since construction or [reset()](#reset-9). |
| `const std::string &` | [`contentType`](#contenttype-1) `const` | Returns the MIME content type for this part. |
| `uint64_t` | [`length`](#length-2) `const` | Returns the total byte length of the part data. |

---

{#formpart-1}

#### FormPart

```cpp
FormPart(const std::string & contentType)
```

Creates the [FormPart](#formpart) with the given MIME content type. 
#### Parameters
* `contentType` MIME type for this part (default: "application/octet-stream").

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |

---

{#formpart-2}

#### ~FormPart

`virtual`

```cpp
virtual ~FormPart()
```

Destroys the [FormPart](#formpart).

---

{#reset-9}

#### reset

`virtual`

```cpp
virtual void reset()
```

Resets the internal state and write position to the beginning. Called by [FormWriter](#formwriter) when retrying or recalculating content length.

---

{#writechunk-1}

#### writeChunk

```cpp
bool writeChunk(FormWriter & writer)
```

Writes the next chunk of data to the [FormWriter](#formwriter). 
#### Parameters
* `writer` The [FormWriter](#formwriter) to send the chunk through. 

#### Returns
true if more data remains to be written; false when complete.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#formwriter) &` |  |

---

{#write-4}

#### write

```cpp
void write(FormWriter & writer)
```

Writes the entire part data to the [FormWriter](#formwriter) in one call. 
#### Parameters
* `writer` The [FormWriter](#formwriter) to send data through.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#formwriter) &` |  |

---

{#write-5}

#### write

```cpp
void write(std::ostream & ostr)
```

Writes the entire part data to an output stream (used for content-length calculation). 
#### Parameters
* `ostr` Output stream to write to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#headers-1}

#### headers

```cpp
NVCollection & headers()
```

Returns the extra MIME headers for this part (e.g. Content-Disposition).

---

{#initialwrite}

#### initialWrite

`virtual` `const`

```cpp
virtual bool initialWrite() const
```

Returns true if this is the first write call since construction or [reset()](#reset-9).

---

{#contenttype-1}

#### contentType

`const`

```cpp
const std::string & contentType() const
```

Returns the MIME content type for this part.

---

{#length-2}

#### length

`const`

```cpp
uint64_t length() const
```

Returns the total byte length of the part data.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_contentType`](#_contenttype)  |  |
| `uint64_t` | [`_length`](#_length)  |  |
| `NVCollection` | [`_headers`](#_headers)  |  |
| `bool` | [`_initialWrite`](#_initialwrite)  |  |

---

{#_contenttype}

#### _contentType

```cpp
std::string _contentType
```

---

{#_length}

#### _length

```cpp
uint64_t _length
```

---

{#_headers}

#### _headers

```cpp
NVCollection _headers
```

---

{#_initialwrite}

#### _initialWrite

```cpp
bool _initialWrite
```

{#formwriter}

## FormWriter

```cpp
#include <icy/http/form.h>
```

> **Inherits:** [`NVCollection`](base.md#nvcollection), [`PacketStreamAdapter`](base.md#packetstreamadapter), [`Startable`](base.md#startable)

[FormWriter](#formwriter) is a HTTP client connection adapter for writing HTML forms.

This class runs in its own thread so as not to block the event loop while uploading big files. Class members are not synchronized hence they should not be accessed while the form is sending, not that there would be any reason to do so.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `PacketSignal` | [`emitter`](#emitter-6)  | The outgoing packet emitter. |

---

{#emitter-6}

#### emitter

```cpp
PacketSignal emitter
```

The outgoing packet emitter.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`~FormWriter`](#formwriter-1) `virtual` | Destroys the [FormWriter](#formwriter). |
| `void` | [`addPart`](#addpart)  | Adds a part or file attachment to the multipart form. |
| `void` | [`start`](#start-8) `virtual` | Starts the sending thread. |
| `void` | [`stop`](#stop-7) `virtual` | Stops the sending thread. |
| `bool` | [`complete`](#complete-1) `const` | Returns true if the request is complete. |
| `bool` | [`cancelled`](#cancelled-1) `const` | Returns true if the request is cancelled. |
| `void` | [`prepareSubmit`](#preparesubmit)  | Prepares the outgoing HTTP request object for submitting the form. |
| `uint64_t` | [`calculateMultipartContentLength`](#calculatemultipartcontentlength)  | Processes the entire form body and computes its total byte length. Only meaningful for multipart/form-data when not using chunked encoding.  |
| `void` | [`writeUrl`](#writeurl)  | Writes the complete "application/x-www-form-urlencoded" encoded body to `ostr`. All key-value pairs from the [NVCollection](base.md#nvcollection) base are percent-encoded and joined with '&'.  |
| `void` | [`writeMultipartChunk`](#writemultipartchunk)  | Writes the next pending multipart chunk to the connection. Non-blocking; intended to be called repeatedly from the event loop until all parts have been sent. |
| `void` | [`writeAsync`](#writeasync)  | Writes the next message chunk from the background runner thread. Called by the [Runner](base.md#runner); do not call directly. |
| `void` | [`setEncoding`](#setencoding)  | Sets the MIME encoding used for submitting the form. Must be set before [prepareSubmit()](#preparesubmit) is called.  |
| `const std::string &` | [`encoding`](#encoding) `const` | Returns the encoding used for posting the form. |
| `void` | [`setBoundary`](#setboundary)  | Sets the MIME boundary string used to delimit multipart form parts. If not set, a random boundary is generated by [prepareSubmit()](#preparesubmit). Must be set before [prepareSubmit()](#preparesubmit) is called.  |
| `const std::string &` | [`boundary`](#boundary) `const` | Returns the MIME boundary used for writing multipart form data. |
| `ConnectionStream &` | [`connection`](#connection-5)  | The associated HTTP client connection. |

---

{#formwriter-1}

#### ~FormWriter

`virtual`

```cpp
virtual ~FormWriter()
```

Destroys the [FormWriter](#formwriter).

---

{#addpart}

#### addPart

```cpp
void addPart(const std::string & name, FormPart * part)
```

Adds a part or file attachment to the multipart form.

The [FormWriter](#formwriter) takes ownership of `part` and deletes it when done. Parts are only sent when the encoding is "multipart/form-data". 
#### Parameters
* `name` Form field name for this part. 

* `part` [Part](#part) to add. Ownership is transferred.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `part` | `[FormPart](#formpart) *` |  |

---

{#start-8}

#### start

`virtual`

```cpp
virtual void start()
```

Starts the sending thread.

---

{#stop-7}

#### stop

`virtual`

```cpp
virtual void stop()
```

Stops the sending thread.

---

{#complete-1}

#### complete

`const`

```cpp
bool complete() const
```

Returns true if the request is complete.

---

{#cancelled-1}

#### cancelled

`const`

```cpp
bool cancelled() const
```

Returns true if the request is cancelled.

---

{#preparesubmit}

#### prepareSubmit

```cpp
void prepareSubmit()
```

Prepares the outgoing HTTP request object for submitting the form.

---

{#calculatemultipartcontentlength}

#### calculateMultipartContentLength

```cpp
uint64_t calculateMultipartContentLength()
```

Processes the entire form body and computes its total byte length. Only meaningful for multipart/form-data when not using chunked encoding. 
#### Returns
Total content length in bytes.

---

{#writeurl}

#### writeUrl

```cpp
void writeUrl(std::ostream & ostr)
```

Writes the complete "application/x-www-form-urlencoded" encoded body to `ostr`. All key-value pairs from the [NVCollection](base.md#nvcollection) base are percent-encoded and joined with '&'. 
#### Parameters
* `ostr` Output stream to write to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#writemultipartchunk}

#### writeMultipartChunk

```cpp
void writeMultipartChunk()
```

Writes the next pending multipart chunk to the connection. Non-blocking; intended to be called repeatedly from the event loop until all parts have been sent.

---

{#writeasync}

#### writeAsync

```cpp
void writeAsync()
```

Writes the next message chunk from the background runner thread. Called by the [Runner](base.md#runner); do not call directly.

---

{#setencoding}

#### setEncoding

```cpp
void setEncoding(const std::string & encoding)
```

Sets the MIME encoding used for submitting the form. Must be set before [prepareSubmit()](#preparesubmit) is called. 
#### Parameters
* `encoding` MIME type: ENCODING_URL or ENCODING_MULTIPART_FORM.

| Parameter | Type | Description |
|-----------|------|-------------|
| `encoding` | `const std::string &` |  |

---

{#encoding}

#### encoding

`const`

```cpp
const std::string & encoding() const
```

Returns the encoding used for posting the form.

---

{#setboundary}

#### setBoundary

```cpp
void setBoundary(const std::string & boundary)
```

Sets the MIME boundary string used to delimit multipart form parts. If not set, a random boundary is generated by [prepareSubmit()](#preparesubmit). Must be set before [prepareSubmit()](#preparesubmit) is called. 
#### Parameters
* `boundary` Boundary string (without leading "--").

| Parameter | Type | Description |
|-----------|------|-------------|
| `boundary` | `const std::string &` |  |

---

{#boundary}

#### boundary

`const`

```cpp
const std::string & boundary() const
```

Returns the MIME boundary used for writing multipart form data.

---

{#connection-5}

#### connection

```cpp
ConnectionStream & connection()
```

The associated HTTP client connection.

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`ENCODING_URL`](#encoding_url) `static` | "application/x-www-form-urlencoded" |
| `const char *` | [`ENCODING_MULTIPART_FORM`](#encoding_multipart_form) `static` | "multipart/form-data" |
| `const char *` | [`ENCODING_MULTIPART_RELATED`](#encoding_multipart_related) `static` | "multipart/related" [http://tools.ietf.org/html/rfc2387](http://tools.ietf.org/html/rfc2387) |

---

{#encoding_url}

#### ENCODING_URL

`static`

```cpp
const char * ENCODING_URL
```

"application/x-www-form-urlencoded"

---

{#encoding_multipart_form}

#### ENCODING_MULTIPART_FORM

`static`

```cpp
const char * ENCODING_MULTIPART_FORM
```

"multipart/form-data"

---

{#encoding_multipart_related}

#### ENCODING_MULTIPART_RELATED

`static`

```cpp
const char * ENCODING_MULTIPART_RELATED
```

"multipart/related" [http://tools.ietf.org/html/rfc2387](http://tools.ietf.org/html/rfc2387)

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `FormWriter *` | [`create`](#create-7) `static` | Creates a [FormWriter](#formwriter) for the given connection and encoding. |

---

{#create-7}

#### create

`static`

```cpp
static FormWriter * create(ConnectionStream & conn, const std::string & encoding)
```

Creates a [FormWriter](#formwriter) for the given connection and encoding.

Encoding must be either "application/x-www-form-urlencoded" (which is the default) or "multipart/form-data". 
#### Parameters
* `conn` The HTTP connection stream to write form data to. 

* `encoding` MIME encoding type. 

#### Returns
Heap-allocated [FormWriter](#formwriter). The caller owns the returned pointer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ConnectionStream](#connectionstream) &` |  |
| `encoding` | `const std::string &` |  |

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ConnectionStream &` | [`_stream`](#_stream)  |  |
| `std::shared_ptr< Runner >` | [`_runner`](#_runner)  |  |
| `std::string` | [`_encoding`](#_encoding)  |  |
| `std::string` | [`_boundary`](#_boundary)  |  |
| `PartQueue` | [`_parts`](#_parts)  |  |
| `uint64_t` | [`_filesLength`](#_fileslength)  |  |
| `int` | [`_writeState`](#_writestate)  |  |
| `bool` | [`_initial`](#_initial)  |  |
| `bool` | [`_complete`](#_complete-1)  |  |

---

{#_stream}

#### _stream

```cpp
ConnectionStream & _stream
```

---

{#_runner}

#### _runner

```cpp
std::shared_ptr< Runner > _runner
```

---

{#_encoding}

#### _encoding

```cpp
std::string _encoding
```

---

{#_boundary}

#### _boundary

```cpp
std::string _boundary
```

---

{#_parts}

#### _parts

```cpp
PartQueue _parts
```

---

{#_fileslength}

#### _filesLength

```cpp
uint64_t _filesLength
```

---

{#_writestate}

#### _writeState

```cpp
int _writeState
```

---

{#_initial}

#### _initial

```cpp
bool _initial
```

---

{#_complete-1}

#### _complete

```cpp
bool _complete
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FormWriter`](#formwriter-2)  | Creates the [FormWriter](#formwriter) that uses the given encoding. |
|  | [`FormWriter`](#formwriter-3)  |  |
|  | [`FormWriter`](#formwriter-4)  |  |
| `void` | [`writePartHeader`](#writepartheader)  | Writes the message boundary std::string, followed by the message header to the output stream. |
| `void` | [`writeEnd`](#writeend)  | Writes the final boundary std::string to the output stream. |
| `void` | [`updateProgress`](#updateprogress) `virtual` | Updates the upload progress via the associated [ConnectionStream](#connectionstream) object. |

---

{#formwriter-2}

#### FormWriter

```cpp
FormWriter(ConnectionStream & conn, std::shared_ptr< Runner > runner, const std::string & encoding)
```

Creates the [FormWriter](#formwriter) that uses the given encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ConnectionStream](#connectionstream) &` |  |
| `runner` | `std::shared_ptr< [Runner](base.md#runner) >` |  |
| `encoding` | `const std::string &` |  |

---

{#formwriter-3}

#### FormWriter

```cpp
FormWriter(const FormWriter &) = delete
```

---

{#formwriter-4}

#### FormWriter

```cpp
FormWriter(FormWriter &&) = delete
```

---

{#writepartheader}

#### writePartHeader

```cpp
void writePartHeader(const NVCollection & header, std::ostream & ostr)
```

Writes the message boundary std::string, followed by the message header to the output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `const [NVCollection](base.md#nvcollection) &` |  |
| `ostr` | `std::ostream &` |  |

---

{#writeend}

#### writeEnd

```cpp
void writeEnd(std::ostream & ostr)
```

Writes the final boundary std::string to the output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#updateprogress}

#### updateProgress

`virtual`

```cpp
virtual void updateProgress(int nread)
```

Updates the upload progress via the associated [ConnectionStream](#connectionstream) object.

| Parameter | Type | Description |
|-----------|------|-------------|
| `nread` | `int` |  |

{#part}

## Part

```cpp
#include <icy/http/form.h>
```

Individual part within a multipart form submission.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`name`](#name-5)  |  |
| `std::unique_ptr< FormPart >` | [`part`](#part-1)  |  |

---

{#name-5}

#### name

```cpp
std::string name
```

---

{#part-1}

#### part

```cpp
std::unique_ptr< FormPart > part
```

{#message}

## Message

```cpp
#include <icy/http/message.h>
```

> **Inherits:** [`NVCollection`](base.md#nvcollection)
> **Subclassed by:** [`Request`](#request-4), [`Response`](#response-1)

The base class for [Request](#request-4) and [Response](#response-1).

Defines the common properties of all HTTP messages. These are version, content length, content type and transfer encoding.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`setVersion`](#setversion-1)  | Sets the HTTP version for this message. |
| `const std::string &` | [`getVersion`](#getversion-1) `const` | Returns the HTTP version for this message. |
| `void` | [`setContentLength`](#setcontentlength)  | Sets the Content-Length header. |
| `uint64_t` | [`getContentLength`](#getcontentlength) `const` | Returns the content length for this message, which may be UNKNOWN_CONTENT_LENGTH if no Content-Length header is present. |
| `bool` | [`hasContentLength`](#hascontentlength) `const` | Returns true if a Content-Length header is present. |
| `void` | [`setTransferEncoding`](#settransferencoding)  | Sets the transfer encoding for this message. |
| `const std::string &` | [`getTransferEncoding`](#gettransferencoding) `const` | Returns the transfer encoding used for this message. |
| `void` | [`setChunkedTransferEncoding`](#setchunkedtransferencoding)  | If flag is true, sets the Transfer-Encoding header to chunked. Otherwise, removes the Transfer-Encoding header. |
| `bool` | [`isChunkedTransferEncoding`](#ischunkedtransferencoding) `const` | Returns true if the Transfer-Encoding header is set and its value is chunked. |
| `void` | [`setContentType`](#setcontenttype)  | Sets the content type for this message. |
| `const std::string &` | [`getContentType`](#getcontenttype) `const` | Returns the content type for this message. |
| `void` | [`setKeepAlive`](#setkeepalive-1)  | Sets the value of the [Connection](#connection-1) header field. |
| `bool` | [`getKeepAlive`](#getkeepalive) `const` | Returns true if |
| `void` | [`write`](#write-6) `virtual` `const` | Writes the message header to the given output stream. |
| `void` | [`write`](#write-7) `virtual` `const` | Writes the message header to the given output string. |
| `void` | [`write`](#write-8) `virtual` `const` | Writes the message header directly into a byte buffer. |

---

{#setversion-1}

#### setVersion

```cpp
void setVersion(const std::string & version)
```

Sets the HTTP version for this message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

{#getversion-1}

#### getVersion

`const`

```cpp
const std::string & getVersion() const
```

Returns the HTTP version for this message.

---

{#setcontentlength}

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

{#getcontentlength}

#### getContentLength

`const`

```cpp
uint64_t getContentLength() const
```

Returns the content length for this message, which may be UNKNOWN_CONTENT_LENGTH if no Content-Length header is present.

---

{#hascontentlength}

#### hasContentLength

`const`

```cpp
bool hasContentLength() const
```

Returns true if a Content-Length header is present.

---

{#settransferencoding}

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

{#gettransferencoding}

#### getTransferEncoding

`const`

```cpp
const std::string & getTransferEncoding() const
```

Returns the transfer encoding used for this message.

Normally, this is the value of the Transfer-Encoding header field. If no such field is present, returns IDENTITY_TRANSFER_CODING.

---

{#setchunkedtransferencoding}

#### setChunkedTransferEncoding

```cpp
void setChunkedTransferEncoding(bool flag)
```

If flag is true, sets the Transfer-Encoding header to chunked. Otherwise, removes the Transfer-Encoding header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

{#ischunkedtransferencoding}

#### isChunkedTransferEncoding

`const`

```cpp
bool isChunkedTransferEncoding() const
```

Returns true if the Transfer-Encoding header is set and its value is chunked.

---

{#setcontenttype}

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

{#getcontenttype}

#### getContentType

`const`

```cpp
const std::string & getContentType() const
```

Returns the content type for this message.

If no Content-Type header is present, returns UNKNOWN_CONTENT_TYPE.

---

{#setkeepalive-1}

#### setKeepAlive

```cpp
void setKeepAlive(bool keepAlive)
```

Sets the value of the [Connection](#connection-1) header field.

The value is set to "Keep-Alive" if keepAlive is true, or to "Close" otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `keepAlive` | `bool` |  |

---

{#getkeepalive}

#### getKeepAlive

`const`

```cpp
bool getKeepAlive() const
```

Returns true if

* the message has a [Connection](#connection-1) header field and its value is "Keep-Alive"

* the message is a HTTP/1.1 message and not [Connection](#connection-1) header is set Returns false otherwise.

---

{#write-6}

#### write

`virtual` `const`

```cpp
virtual void write(std::ostream & ostr) const
```

Writes the message header to the given output stream.

The format is one name-value pair per line, with name and value separated by a colon and lines delimited by a carriage return and a linefeed character. See RFC 2822 for details.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#write-7}

#### write

`virtual` `const`

```cpp
virtual void write(std::string & str) const
```

Writes the message header to the given output string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |

---

{#write-8}

#### write

`virtual` `const`

```cpp
virtual void write(Buffer & buf) const
```

Writes the message header directly into a byte buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `[Buffer](base.md#buffer-2) &` |  |

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const std::string` | [`HTTP_1_0`](#http_1_0) `static` |  |
| `const std::string` | [`HTTP_1_1`](#http_1_1) `static` |  |
| `const std::string` | [`IDENTITY_TRANSFER_ENCODING`](#identity_transfer_encoding) `static` |  |
| `const std::string` | [`CHUNKED_TRANSFER_ENCODING`](#chunked_transfer_encoding) `static` |  |
| `const int` | [`UNKNOWN_CONTENT_LENGTH`](#unknown_content_length) `static` |  |
| `const std::string` | [`UNKNOWN_CONTENT_TYPE`](#unknown_content_type) `static` |  |
| `const std::string` | [`CONTENT_LENGTH`](#content_length) `static` |  |
| `const std::string` | [`CONTENT_TYPE`](#content_type) `static` |  |
| `const std::string` | [`TRANSFER_ENCODING`](#transfer_encoding) `static` |  |
| `const std::string` | [`CONNECTION`](#connection-6) `static` |  |
| `const std::string` | [`CONNECTION_KEEP_ALIVE`](#connection_keep_alive) `static` |  |
| `const std::string` | [`CONNECTION_CLOSE`](#connection_close) `static` |  |
| `const std::string` | [`EMPTY`](#empty) `static` |  |

---

{#http_1_0}

#### HTTP_1_0

`static`

```cpp
const std::string HTTP_1_0
```

---

{#http_1_1}

#### HTTP_1_1

`static`

```cpp
const std::string HTTP_1_1
```

---

{#identity_transfer_encoding}

#### IDENTITY_TRANSFER_ENCODING

`static`

```cpp
const std::string IDENTITY_TRANSFER_ENCODING
```

---

{#chunked_transfer_encoding}

#### CHUNKED_TRANSFER_ENCODING

`static`

```cpp
const std::string CHUNKED_TRANSFER_ENCODING
```

---

{#unknown_content_length}

#### UNKNOWN_CONTENT_LENGTH

`static`

```cpp
const int UNKNOWN_CONTENT_LENGTH
```

---

{#unknown_content_type}

#### UNKNOWN_CONTENT_TYPE

`static`

```cpp
const std::string UNKNOWN_CONTENT_TYPE
```

---

{#content_length}

#### CONTENT_LENGTH

`static`

```cpp
const std::string CONTENT_LENGTH
```

---

{#content_type}

#### CONTENT_TYPE

`static`

```cpp
const std::string CONTENT_TYPE
```

---

{#transfer_encoding}

#### TRANSFER_ENCODING

`static`

```cpp
const std::string TRANSFER_ENCODING
```

---

{#connection-6}

#### CONNECTION

`static`

```cpp
const std::string CONNECTION
```

---

{#connection_keep_alive}

#### CONNECTION_KEEP_ALIVE

`static`

```cpp
const std::string CONNECTION_KEEP_ALIVE
```

---

{#connection_close}

#### CONNECTION_CLOSE

`static`

```cpp
const std::string CONNECTION_CLOSE
```

---

{#empty}

#### EMPTY

`static`

```cpp
const std::string EMPTY
```

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_version`](#_version-1)  |  |

---

{#_version-1}

#### _version

```cpp
std::string _version
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Message`](#message-1)  | Creates the [Message](#message) with version HTTP/1.0. |
|  | [`Message`](#message-2)  | Creates the [Message](#message) and sets the version. |
|  | [`~Message`](#message-3) `virtual` | Destroys the [Message](#message). |

---

{#message-1}

#### Message

```cpp
Message()
```

Creates the [Message](#message) with version HTTP/1.0.

---

{#message-2}

#### Message

```cpp
Message(const std::string & version)
```

Creates the [Message](#message) and sets the version.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

{#message-3}

#### ~Message

`virtual`

```cpp
virtual ~Message()
```

Destroys the [Message](#message).

{#multipartadapter}

## MultipartAdapter

```cpp
#include <icy/http/packetizers.h>
```

> **Inherits:** [`PacketProcessor`](base.md#packetprocessor)

HTTP multipart encoding adapter for multipart/x-mixed-replace streaming.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Connection::Ptr` | [`connection`](#connection-7)  | HTTP connection for sending the initial response header. |
| `std::string` | [`contentType`](#contenttype-2)  | Content-Type of each part (e.g. "image/jpeg"). |
| `bool` | [`isBase64`](#isbase64)  | If true, adds "Content-Transfer-Encoding: base64" to each part. |
| `bool` | [`initial`](#initial-1)  | True until the first chunk is processed and the boundary header emitted. |
| `PacketSignal` | [`emitter`](#emitter-7)  |  |

---

{#connection-7}

#### connection

```cpp
Connection::Ptr connection
```

HTTP connection for sending the initial response header.

---

{#contenttype-2}

#### contentType

```cpp
std::string contentType
```

Content-Type of each part (e.g. "image/jpeg").

---

{#isbase64}

#### isBase64

```cpp
bool isBase64
```

If true, adds "Content-Transfer-Encoding: base64" to each part.

---

{#initial-1}

#### initial

```cpp
bool initial
```

True until the first chunk is processed and the boundary header emitted.

---

{#emitter-7}

#### emitter

```cpp
PacketSignal emitter
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MultipartAdapter`](#multipartadapter-1) `inline` | Creates a [MultipartAdapter](#multipartadapter) that sends headers through the given connection. The per-part content type is read from the connection's outgoing header.  |
|  | [`MultipartAdapter`](#multipartadapter-2) `inline` | Creates a [MultipartAdapter](#multipartadapter) that emits its own raw HTTP/1.1 200 response header. Use this when no [Connection](#connection-1) object is available.  |
| `void` | [`emitHeader`](#emitheader-1) `virtual` `inline` | Emits the initial HTTP/1.1 200 OK response with Content-Type multipart/x-mixed-replace. If a connection is set, headers are written through it; otherwise a raw response string is emitted. |
| `void` | [`emitChunkHeader`](#emitchunkheader) `virtual` `inline` | Emits the MIME boundary and per-part headers (Content-Type, optionally Content-Transfer-Encoding) for the next multipart chunk. |
| `void` | [`process`](#process-6) `virtual` `inline` | Wraps the incoming packet as a multipart chunk and emits it downstream. Emits the multipart HTTP response headers on the first call.  |

---

{#multipartadapter-1}

#### MultipartAdapter

`inline`

```cpp
inline MultipartAdapter(Connection::Ptr connection, bool base64)
```

Creates a [MultipartAdapter](#multipartadapter) that sends headers through the given connection. The per-part content type is read from the connection's outgoing header. 
#### Parameters
* `connection` HTTP connection to use for sending the initial multipart header. 

* `[base64](base.md#base64)` If true, indicates parts are base64-encoded.

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection::Ptr](#ptr-12)` |  |
| `base64` | `bool` |  |

---

{#multipartadapter-2}

#### MultipartAdapter

`inline`

```cpp
inline MultipartAdapter(const std::string & contentType, bool base64)
```

Creates a [MultipartAdapter](#multipartadapter) that emits its own raw HTTP/1.1 200 response header. Use this when no [Connection](#connection-1) object is available. 
#### Parameters
* `contentType` Content-Type for each multipart part. 

* `[base64](base.md#base64)` If true, indicates parts are base64-encoded.

| Parameter | Type | Description |
|-----------|------|-------------|
| `contentType` | `const std::string &` |  |
| `base64` | `bool` |  |

---

{#emitheader-1}

#### emitHeader

`virtual` `inline`

```cpp
virtual inline void emitHeader()
```

Emits the initial HTTP/1.1 200 OK response with Content-Type multipart/x-mixed-replace. If a connection is set, headers are written through it; otherwise a raw response string is emitted.

---

{#emitchunkheader}

#### emitChunkHeader

`virtual` `inline`

```cpp
virtual inline void emitChunkHeader()
```

Emits the MIME boundary and per-part headers (Content-Type, optionally Content-Transfer-Encoding) for the next multipart chunk.

---

{#process-6}

#### process

`virtual` `inline`

```cpp
virtual inline void process(IPacket & packet)
```

Wraps the incoming packet as a multipart chunk and emits it downstream. Emits the multipart HTTP response headers on the first call. 
#### Parameters
* `packet` Packet containing the raw payload data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `[IPacket](base.md#ipacket) &` |  |

{#parser-1}

## Parser

```cpp
#include <icy/http/parser.h>
```

HTTP request/response parser using the llhttp library.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Parser`](#parser-2)  | Creates a response parser. The response object is populated as data is parsed.  |
|  | [`Parser`](#parser-3)  | Creates a request parser. The request object is populated as data is parsed.  |
|  | [`Parser`](#parser-4)  | Creates a parser of the given type without binding a message object.  |
|  | [`Parser`](#parser-5)  |  |
|  | [`Parser`](#parser-6)  |  |
| `ParseResult` | [`parse`](#parse)  | Feeds a buffer of raw HTTP data into the parser. |
| `void` | [`reset`](#reset-10)  | Reset the internal state (reinitialises llhttp). Safe to call externally, NOT from inside llhttp callbacks. |
| `void` | [`resetState`](#resetstate)  | Reset internal flags without reinitialising llhttp. Safe to call from inside llhttp callbacks (e.g. on_message_begin). |
| `bool` | [`complete`](#complete-2) `const` | Returns true if parsing is complete, either in success or error. |
| `bool` | [`upgrade`](#upgrade) `const` | Returns true if the connection should be upgraded. |
| `llhttp_type_t` | [`type`](#type-9) `const` `inline` | Returns the parser type (HTTP_REQUEST or HTTP_RESPONSE). |
| `void` | [`setRequest`](#setrequest)  | Binds an HTTP request object to populate during parsing. Must only be called when no message is currently set and type is HTTP_REQUEST.  |
| `void` | [`setResponse`](#setresponse)  | Binds an HTTP response object to populate during parsing. Must only be called when no message is currently set and type is HTTP_RESPONSE.  |
| `void` | [`setObserver`](#setobserver)  | Sets the observer that receives parser events.  |
| `void` | [`clearMessage`](#clearmessage)  | Clear request/response pointers so they can be re-set. Used when resetting a pooled connection for reuse. |
| `http::Message *` | [`message`](#message-4)  | Returns the currently bound message (request or response), or nullptr. |
| `ParserObserver *` | [`observer`](#observer) `const` | Returns the current parser observer, or nullptr if none is set. |

---

{#parser-2}

#### Parser

```cpp
Parser(http::Response * response)
```

Creates a response parser. The response object is populated as data is parsed. 
#### Parameters
* `response` HTTP response object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `[http::Response](#response-1) *` |  |

---

{#parser-3}

#### Parser

```cpp
Parser(http::Request * request)
```

Creates a request parser. The request object is populated as data is parsed. 
#### Parameters
* `request` HTTP request object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) *` |  |

---

{#parser-4}

#### Parser

```cpp
Parser(llhttp_type_t type)
```

Creates a parser of the given type without binding a message object. 
#### Parameters
* `type` Either HTTP_REQUEST or HTTP_RESPONSE.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `llhttp_type_t` |  |

---

{#parser-5}

#### Parser

```cpp
Parser(const Parser &) = delete
```

---

{#parser-6}

#### Parser

```cpp
Parser(Parser &&) = delete
```

---

{#parse}

#### parse

```cpp
ParseResult parse(const char * data, size_t length)
```

Feeds a buffer of raw HTTP data into the parser.

May be called multiple times for streaming data. The parser state persists between calls. On completion or error, the observer is notified. 
#### Parameters
* `data` Pointer to the input data buffer. 

* `length` Number of bytes in the buffer. 

#### Returns
Structured parse result including bytes consumed and terminal state.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `length` | `size_t` |  |

---

{#reset-10}

#### reset

```cpp
void reset()
```

Reset the internal state (reinitialises llhttp). Safe to call externally, NOT from inside llhttp callbacks.

---

{#resetstate}

#### resetState

```cpp
void resetState()
```

Reset internal flags without reinitialising llhttp. Safe to call from inside llhttp callbacks (e.g. on_message_begin).

---

{#complete-2}

#### complete

`const`

```cpp
bool complete() const
```

Returns true if parsing is complete, either in success or error.

---

{#upgrade}

#### upgrade

`const`

```cpp
bool upgrade() const
```

Returns true if the connection should be upgraded.

---

{#type-9}

#### type

`const` `inline`

```cpp
inline llhttp_type_t type() const
```

Returns the parser type (HTTP_REQUEST or HTTP_RESPONSE).

---

{#setrequest}

#### setRequest

```cpp
void setRequest(http::Request * request)
```

Binds an HTTP request object to populate during parsing. Must only be called when no message is currently set and type is HTTP_REQUEST. 
#### Parameters
* `request` The request object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) *` |  |

---

{#setresponse}

#### setResponse

```cpp
void setResponse(http::Response * response)
```

Binds an HTTP response object to populate during parsing. Must only be called when no message is currently set and type is HTTP_RESPONSE. 
#### Parameters
* `response` The response object to populate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `[http::Response](#response-1) *` |  |

---

{#setobserver}

#### setObserver

```cpp
void setObserver(ParserObserver * observer)
```

Sets the observer that receives parser events. 
#### Parameters
* `observer` Observer to notify. May be nullptr to clear.

| Parameter | Type | Description |
|-----------|------|-------------|
| `observer` | `[ParserObserver](#parserobserver) *` |  |

---

{#clearmessage}

#### clearMessage

```cpp
void clearMessage()
```

Clear request/response pointers so they can be re-set. Used when resetting a pooled connection for reuse.

---

{#message-4}

#### message

```cpp
http::Message * message()
```

Returns the currently bound message (request or response), or nullptr.

---

{#observer}

#### observer

`const`

```cpp
ParserObserver * observer() const
```

Returns the current parser observer, or nullptr if none is set.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ParserObserver *` | [`_observer`](#_observer)  |  |
| `http::Request *` | [`_request`](#_request-1)  |  |
| `http::Response *` | [`_response`](#_response-1)  |  |
| `http::Message *` | [`_message`](#_message)  |  |
| `llhttp_t` | [`_parser`](#_parser-1)  |  |
| `llhttp_settings_t` | [`_settings`](#_settings)  |  |
| `llhttp_type_t` | [`_type`](#_type)  |  |
| `bool` | [`_wasHeaderValue`](#_washeadervalue)  |  |
| `std::string` | [`_lastHeaderField`](#_lastheaderfield)  |  |
| `std::string` | [`_lastHeaderValue`](#_lastheadervalue)  |  |
| `bool` | [`_complete`](#_complete-2)  |  |
| `bool` | [`_upgrade`](#_upgrade)  |  |
| `Error` | [`_error`](#_error-3)  |  |
| `ParseResult` | [`_lastResult`](#_lastresult)  |  |
| `MessageScratch` | [`_scratch`](#_scratch)  |  |

---

{#_observer}

#### _observer

```cpp
ParserObserver * _observer
```

---

{#_request-1}

#### _request

```cpp
http::Request * _request
```

---

{#_response-1}

#### _response

```cpp
http::Response * _response
```

---

{#_message}

#### _message

```cpp
http::Message * _message
```

---

{#_parser-1}

#### _parser

```cpp
llhttp_t _parser
```

---

{#_settings}

#### _settings

```cpp
llhttp_settings_t _settings
```

---

{#_type}

#### _type

```cpp
llhttp_type_t _type
```

---

{#_washeadervalue}

#### _wasHeaderValue

```cpp
bool _wasHeaderValue
```

---

{#_lastheaderfield}

#### _lastHeaderField

```cpp
std::string _lastHeaderField
```

---

{#_lastheadervalue}

#### _lastHeaderValue

```cpp
std::string _lastHeaderValue
```

---

{#_complete-2}

#### _complete

```cpp
bool _complete
```

---

{#_upgrade}

#### _upgrade

```cpp
bool _upgrade
```

---

{#_error-3}

#### _error

```cpp
Error _error
```

---

{#_lastresult}

#### _lastResult

```cpp
ParseResult _lastResult
```

---

{#_scratch}

#### _scratch

```cpp
MessageScratch _scratch
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`init`](#init-8)  |  |
| `void` | [`clearBoundMessage`](#clearboundmessage)  |  |
| `void` | [`storeHeader`](#storeheader)  |  |
| `void` | [`applyScratchToBoundMessage`](#applyscratchtoboundmessage)  |  |
| `void` | [`onHeader`](#onheader)  | Callbacks. |
| `void` | [`onHeadersEnd`](#onheadersend)  |  |
| `void` | [`onBody`](#onbody)  |  |
| `void` | [`onMessageEnd`](#onmessageend)  |  |
| `void` | [`onError`](#onerror-3)  |  |

---

{#init-8}

#### init

```cpp
void init()
```

---

{#clearboundmessage}

#### clearBoundMessage

```cpp
void clearBoundMessage()
```

---

{#storeheader}

#### storeHeader

```cpp
void storeHeader(std::string name, std::string value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` |  |
| `value` | `std::string` |  |

---

{#applyscratchtoboundmessage}

#### applyScratchToBoundMessage

```cpp
void applyScratchToBoundMessage()
```

---

{#onheader}

#### onHeader

```cpp
void onHeader(std::string name, std::string value)
```

Callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` |  |
| `value` | `std::string` |  |

---

{#onheadersend}

#### onHeadersEnd

```cpp
void onHeadersEnd()
```

---

{#onbody}

#### onBody

```cpp
void onBody(const char * buf, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const char *` |  |
| `len` | `size_t` |  |

---

{#onmessageend}

#### onMessageEnd

```cpp
void onMessageEnd()
```

---

{#onerror-3}

#### onError

```cpp
void onError(llhttp_errno_t errnum, const std::string & message)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `errnum` | `llhttp_errno_t` |  |
| `message` | `const std::string &` |  |

{#messagescratch}

## MessageScratch

```cpp
#include <icy/http/parser.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`version`](#version-2)  |  |
| `std::string` | [`method`](#method-1)  |  |
| `std::string` | [`uri`](#uri)  |  |
| `std::string` | [`reason`](#reason)  |  |
| `http::StatusCode` | [`status`](#status-2)  |  |
| `std::vector< std::pair< std::string, std::string > >` | [`headers`](#headers-2)  |  |

---

{#version-2}

#### version

```cpp
std::string version
```

---

{#method-1}

#### method

```cpp
std::string method
```

---

{#uri}

#### uri

```cpp
std::string uri
```

---

{#reason}

#### reason

```cpp
std::string reason
```

---

{#status-2}

#### status

```cpp
http::StatusCode status = 
```

---

{#headers-2}

#### headers

```cpp
std::vector< std::pair< std::string, std::string > > headers
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`reset`](#reset-11) `inline` |  |

---

{#reset-11}

#### reset

`inline`

```cpp
inline void reset()
```

{#parseresult}

## ParseResult

```cpp
#include <icy/http/parser.h>
```

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `size_t` | [`bytesConsumed`](#bytesconsumed)  |  |
| `bool` | [`messageComplete`](#messagecomplete)  |  |
| `bool` | [`upgrade`](#upgrade-1)  |  |
| `Error` | [`error`](#error-10)  |  |

---

{#bytesconsumed}

#### bytesConsumed

```cpp
size_t bytesConsumed = 0
```

---

{#messagecomplete}

#### messageComplete

```cpp
bool messageComplete = false
```

---

{#upgrade-1}

#### upgrade

```cpp
bool upgrade = false
```

---

{#error-10}

#### error

```cpp
Error error
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`ok`](#ok) `const` `inline` |  |

---

{#ok}

#### ok

`const` `inline`

```cpp
inline bool ok() const
```

{#parserobserver}

## ParserObserver

```cpp
#include <icy/http/parser.h>
```

> **Subclassed by:** [`ConnectionAdapter`](#connectionadapter)

Abstract observer interface for HTTP parser events.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onParserHeader`](#onparserheader-1)  | Called for each parsed HTTP header name/value pair.  |
| `void` | [`onParserHeadersEnd`](#onparserheadersend-1)  | Called when all HTTP headers have been parsed.  |
| `void` | [`onParserChunk`](#onparserchunk-1)  | Called for each chunk of body data received.  |
| `void` | [`onParserEnd`](#onparserend-1)  | Called when the HTTP message is fully parsed. |
| `void` | [`onParserError`](#onparsererror-1)  | Called when a parse error occurs.  |

---

{#onparserheader-1}

#### onParserHeader

```cpp
void onParserHeader(const std::string & name, const std::string & value)
```

Called for each parsed HTTP header name/value pair. 
#### Parameters
* `name` Header field name. 

* `value` Header field value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

{#onparserheadersend-1}

#### onParserHeadersEnd

```cpp
void onParserHeadersEnd(bool upgrade)
```

Called when all HTTP headers have been parsed. 
#### Parameters
* `upgrade` True if the connection should be upgraded (e.g. to WebSocket).

| Parameter | Type | Description |
|-----------|------|-------------|
| `upgrade` | `bool` |  |

---

{#onparserchunk-1}

#### onParserChunk

```cpp
void onParserChunk(const char * data, size_t len)
```

Called for each chunk of body data received. 
#### Parameters
* `data` Pointer to the body data chunk. 

* `len` Length of the chunk in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

{#onparserend-1}

#### onParserEnd

```cpp
void onParserEnd()
```

Called when the HTTP message is fully parsed.

---

{#onparsererror-1}

#### onParserError

```cpp
void onParserError(const Error & err)
```

Called when a parse error occurs. 
#### Parameters
* `err` [Error](base.md#error) details from llhttp.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `const [Error](base.md#error) &` |  |

{#progresssignal}

## ProgressSignal

```cpp
#include <icy/http/connection.h>
```

> **Inherits:** [`Signal< void(const double &)>`](base.md#signal)

HTTP progress signal for upload and download progress notifications.

Emits a double in the range [0, 100] as data is transferred. Set `total` to the expected byte count before calling `[update()](#update-1)`.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `void *` | [`sender`](#sender-1)  | Optional context pointer identifying the sender. |
| `uint64_t` | [`current`](#current-1)  | Bytes transferred so far. |
| `uint64_t` | [`total`](#total-1)  | Total expected bytes (from Content-Length). |

---

{#sender-1}

#### sender

```cpp
void * sender
```

Optional context pointer identifying the sender.

---

{#current-1}

#### current

```cpp
uint64_t current
```

Bytes transferred so far.

---

{#total-1}

#### total

```cpp
uint64_t total
```

Total expected bytes (from Content-Length).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ProgressSignal`](#progresssignal-1) `inline` |  |
| `double` | [`progress`](#progress) `const` `inline` | Returns the current transfer progress as a percentage (0-100). |
| `void` | [`update`](#update-1) `inline` | Advances the progress counter by `nread` bytes and emits the updated percentage.  |

---

{#progresssignal-1}

#### ProgressSignal

`inline`

```cpp
inline ProgressSignal()
```

---

{#progress}

#### progress

`const` `inline`

```cpp
inline double progress() const
```

Returns the current transfer progress as a percentage (0-100).

---

{#update-1}

#### update

`inline`

```cpp
inline void update(int nread)
```

Advances the progress counter by `nread` bytes and emits the updated percentage. 
#### Parameters
* `nread` Number of bytes just transferred. 

#### Exceptions
* `std::runtime_error` if current would exceed total.

| Parameter | Type | Description |
|-----------|------|-------------|
| `nread` | `int` |  |

{#request-4}

## Request

```cpp
#include <icy/http/request.h>
```

> **Inherits:** [`Message`](#message)

This class encapsulates an HTTP request message.

In addition to the properties common to all HTTP messages, a HTTP request has a method (e.g. GET, HEAD, POST, etc.) and a request URI.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Request`](#request-5)  | Creates a GET / HTTP/1.1 HTTP request. |
|  | [`Request`](#request-6)  | Creates a GET / HTTP/1.x request with the given version (HTTP/1.0 or HTTP/1.1). |
|  | [`Request`](#request-7)  | Creates a HTTP/1.0 request with the given method and URI. |
|  | [`Request`](#request-8)  | Creates a HTTP request with the given method, URI and version. |
|  | [`~Request`](#request-9) `virtual` | Destroys the [Request](#request-4). |
| `void` | [`setMethod`](#setmethod)  | Sets the method. |
| `const std::string &` | [`getMethod`](#getmethod) `const` | Returns the method. |
| `void` | [`setURI`](#seturi)  | Sets the request URI. |
| `void` | [`appendURI`](#appenduri)  | Appends a fragment to the request URI. Used by the parser when llhttp splits the [URL](#url) across callbacks. |
| `const std::string &` | [`getURI`](#geturi) `const` | Returns the request URI. |
| `void` | [`setHost`](#sethost)  | Sets the value of the Host header field. |
| `void` | [`setHost`](#sethost-1)  | Sets the value of the Host header field. |
| `const std::string &` | [`getHost`](#gethost) `const` | Returns the value of the Host header field. |
| `void` | [`setCookies`](#setcookies)  | Adds a [Cookie](#cookie) header with the names and values from cookies. |
| `void` | [`getCookies`](#getcookies) `const` | Fills cookies with the cookies extracted from the [Cookie](#cookie) headers in the request. |
| `void` | [`getURIParameters`](#geturiparameters) `const` | Returns the request URI parameters. |
| `bool` | [`hasCredentials`](#hascredentials) `const` | Returns true if the request contains authentication information in the form of an Authorization header. |
| `void` | [`getCredentials`](#getcredentials) `const` | Returns the authentication scheme and additional authentication information contained in this request. |
| `void` | [`setCredentials`](#setcredentials)  | Sets the authentication scheme and information for this request. |
| `bool` | [`hasProxyCredentials`](#hasproxycredentials) `const` | Returns true if the request contains proxy authentication information in the form of an Proxy-Authorization header. |
| `void` | [`getProxyCredentials`](#getproxycredentials) `const` | Returns the proxy authentication scheme and additional proxy authentication information contained in this request. |
| `void` | [`setProxyCredentials`](#setproxycredentials)  | Sets the proxy authentication scheme and information for this request. |
| `void` | [`write`](#write-9) `virtual` `const` | Writes the HTTP request to the given output stream. |
| `void` | [`write`](#write-10) `virtual` `const` | Writes the HTTP request to the given output string. |
| `void` | [`write`](#write-11) `virtual` `const` | Writes the HTTP request directly into a byte buffer. |

---

{#request-5}

#### Request

```cpp
Request()
```

Creates a GET / HTTP/1.1 HTTP request.

---

{#request-6}

#### Request

```cpp
Request(const std::string & version)
```

Creates a GET / HTTP/1.x request with the given version (HTTP/1.0 or HTTP/1.1).

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

{#request-7}

#### Request

```cpp
Request(const std::string & method, const std::string & uri)
```

Creates a HTTP/1.0 request with the given method and URI.

| Parameter | Type | Description |
|-----------|------|-------------|
| `method` | `const std::string &` |  |
| `uri` | `const std::string &` |  |

---

{#request-8}

#### Request

```cpp
Request(const std::string & method, const std::string & uri, const std::string & version)
```

Creates a HTTP request with the given method, URI and version.

| Parameter | Type | Description |
|-----------|------|-------------|
| `method` | `const std::string &` |  |
| `uri` | `const std::string &` |  |
| `version` | `const std::string &` |  |

---

{#request-9}

#### ~Request

`virtual`

```cpp
virtual ~Request()
```

Destroys the [Request](#request-4).

---

{#setmethod}

#### setMethod

```cpp
void setMethod(const std::string & method)
```

Sets the method.

| Parameter | Type | Description |
|-----------|------|-------------|
| `method` | `const std::string &` |  |

---

{#getmethod}

#### getMethod

`const`

```cpp
const std::string & getMethod() const
```

Returns the method.

---

{#seturi}

#### setURI

```cpp
void setURI(std::string uri)
```

Sets the request URI.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `std::string` |  |

---

{#appenduri}

#### appendURI

```cpp
void appendURI(std::string_view uri)
```

Appends a fragment to the request URI. Used by the parser when llhttp splits the [URL](#url) across callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `std::string_view` |  |

---

{#geturi}

#### getURI

`const`

```cpp
const std::string & getURI() const
```

Returns the request URI.

---

{#sethost}

#### setHost

```cpp
void setHost(const std::string & host)
```

Sets the value of the Host header field.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |

---

{#sethost-1}

#### setHost

```cpp
void setHost(const std::string & host, uint16_t port)
```

Sets the value of the Host header field.

If the given port number is a non-standard port number (other than 80 or 443), it is included in the Host header field.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `uint16_t` |  |

---

{#gethost}

#### getHost

`const`

```cpp
const std::string & getHost() const
```

Returns the value of the Host header field.

Throws a NotFoundException if the request does not have a Host header field.

---

{#setcookies}

#### setCookies

```cpp
void setCookies(const NVCollection & cookies)
```

Adds a [Cookie](#cookie) header with the names and values from cookies.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookies` | `const [NVCollection](base.md#nvcollection) &` |  |

---

{#getcookies}

#### getCookies

`const`

```cpp
void getCookies(NVCollection & cookies) const
```

Fills cookies with the cookies extracted from the [Cookie](#cookie) headers in the request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookies` | `[NVCollection](base.md#nvcollection) &` |  |

---

{#geturiparameters}

#### getURIParameters

`const`

```cpp
void getURIParameters(NVCollection & params) const
```

Returns the request URI parameters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `params` | `[NVCollection](base.md#nvcollection) &` |  |

---

{#hascredentials}

#### hasCredentials

`const`

```cpp
bool hasCredentials() const
```

Returns true if the request contains authentication information in the form of an Authorization header.

---

{#getcredentials}

#### getCredentials

`const`

```cpp
void getCredentials(std::string & scheme, std::string & authInfo) const
```

Returns the authentication scheme and additional authentication information contained in this request.

Throws a std::exception if no authentication information is contained in the request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `std::string &` |  |
| `authInfo` | `std::string &` |  |

---

{#setcredentials}

#### setCredentials

```cpp
void setCredentials(std::string_view scheme, std::string_view authInfo)
```

Sets the authentication scheme and information for this request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `std::string_view` |  |
| `authInfo` | `std::string_view` |  |

---

{#hasproxycredentials}

#### hasProxyCredentials

`const`

```cpp
bool hasProxyCredentials() const
```

Returns true if the request contains proxy authentication information in the form of an Proxy-Authorization header.

---

{#getproxycredentials}

#### getProxyCredentials

`const`

```cpp
void getProxyCredentials(std::string & scheme, std::string & authInfo) const
```

Returns the proxy authentication scheme and additional proxy authentication information contained in this request.

Throws a std::exception if no proxy authentication information is contained in the request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `std::string &` |  |
| `authInfo` | `std::string &` |  |

---

{#setproxycredentials}

#### setProxyCredentials

```cpp
void setProxyCredentials(std::string_view scheme, std::string_view authInfo)
```

Sets the proxy authentication scheme and information for this request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `std::string_view` |  |
| `authInfo` | `std::string_view` |  |

---

{#write-9}

#### write

`virtual` `const`

```cpp
virtual void write(std::ostream & ostr) const
```

Writes the HTTP request to the given output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#write-10}

#### write

`virtual` `const`

```cpp
virtual void write(std::string & str) const
```

Writes the HTTP request to the given output string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |

---

{#write-11}

#### write

`virtual` `const`

```cpp
virtual void write(Buffer & buf) const
```

Writes the HTTP request directly into a byte buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `[Buffer](base.md#buffer-2) &` |  |

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`getCredentials`](#getcredentials-1) `const` | Returns the authentication scheme and additional authentication information contained in the given header of request. |
| `void` | [`setCredentials`](#setcredentials-1)  | Writes the authentication scheme and information for this request to the given header. |

---

{#getcredentials-1}

#### getCredentials

`const`

```cpp
void getCredentials(const std::string & header, std::string & scheme, std::string & authInfo) const
```

Returns the authentication scheme and additional authentication information contained in the given header of request.

Throws a NotAuthenticatedException if no authentication information is contained in the request.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `const std::string &` |  |
| `scheme` | `std::string &` |  |
| `authInfo` | `std::string &` |  |

---

{#setcredentials-1}

#### setCredentials

```cpp
void setCredentials(const std::string & header, std::string_view scheme, std::string_view authInfo)
```

Writes the authentication scheme and information for this request to the given header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `const std::string &` |  |
| `scheme` | `std::string_view` |  |
| `authInfo` | `std::string_view` |  |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_method`](#_method)  |  |
| `std::string` | [`_uri`](#_uri)  |  |

---

{#_method}

#### _method

```cpp
std::string _method
```

---

{#_uri}

#### _uri

```cpp
std::string _uri
```

{#response-1}

## Response

```cpp
#include <icy/http/response.h>
```

> **Inherits:** [`Message`](#message)

This class encapsulates an HTTP response message.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Response`](#response-2)  | Creates the [Response](#response-1) with OK status. |
|  | [`Response`](#response-3)  | Creates the [Response](#response-1) with the given status and reason phrase. |
|  | [`Response`](#response-4)  | Creates the [Response](#response-1) with the given version, status and reason phrase. |
|  | [`Response`](#response-5)  | Creates the [Response](#response-1) with the given status an an appropriate reason phrase. |
|  | [`Response`](#response-6)  | Creates the [Response](#response-1) with the given version, status an an appropriate reason phrase. |
|  | [`~Response`](#response-7) `virtual` | Destroys the [Response](#response-1). |
| `void` | [`setStatus`](#setstatus)  | Sets the HTTP status code. |
| `StatusCode` | [`getStatus`](#getstatus) `const` | Returns the HTTP status code. |
| `void` | [`setReason`](#setreason)  | Sets the HTTP reason phrase. |
| `const std::string &` | [`getReason`](#getreason) `const` | Returns the HTTP reason phrase. |
| `void` | [`setStatusAndReason`](#setstatusandreason)  | Sets the HTTP status code and reason phrase. |
| `void` | [`setDate`](#setdate)  | Sets the Date header to the given date/time value. |
| `Timestamp` | [`getDate`](#getdate) `const` | Returns the value of the Date header. |
| `void` | [`addCookie`](#addcookie)  | Adds the cookie to the response by adding a Set-Cookie header. |
| `void` | [`getCookies`](#getcookies-1) `const` | Returns a vector with all the cookies set in the response header. |
| `void` | [`write`](#write-12) `virtual` `const` | Writes the HTTP response headers to the given output stream. |
| `void` | [`write`](#write-13) `virtual` `const` | Writes the HTTP response headers to the given output string. |
| `void` | [`write`](#write-14) `virtual` `const` | Writes the HTTP response headers directly into a byte buffer. |
| `bool` | [`success`](#success) `virtual` `const` | Returns true if the HTTP response code was successful (< 400). |

---

{#response-2}

#### Response

```cpp
Response()
```

Creates the [Response](#response-1) with OK status.

---

{#response-3}

#### Response

```cpp
Response(StatusCode status, const std::string & reason)
```

Creates the [Response](#response-1) with the given status and reason phrase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | `[StatusCode](#statuscode)` |  |
| `reason` | `const std::string &` |  |

---

{#response-4}

#### Response

```cpp
Response(const std::string & version, StatusCode status, const std::string & reason)
```

Creates the [Response](#response-1) with the given version, status and reason phrase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |
| `status` | `[StatusCode](#statuscode)` |  |
| `reason` | `const std::string &` |  |

---

{#response-5}

#### Response

```cpp
Response(StatusCode status)
```

Creates the [Response](#response-1) with the given status an an appropriate reason phrase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | `[StatusCode](#statuscode)` |  |

---

{#response-6}

#### Response

```cpp
Response(const std::string & version, StatusCode status)
```

Creates the [Response](#response-1) with the given version, status an an appropriate reason phrase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |
| `status` | `[StatusCode](#statuscode)` |  |

---

{#response-7}

#### ~Response

`virtual`

```cpp
virtual ~Response()
```

Destroys the [Response](#response-1).

---

{#setstatus}

#### setStatus

```cpp
void setStatus(StatusCode status)
```

Sets the HTTP status code.

The reason phrase is set according to the status code.

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | `[StatusCode](#statuscode)` |  |

---

{#getstatus}

#### getStatus

`const`

```cpp
StatusCode getStatus() const
```

Returns the HTTP status code.

---

{#setreason}

#### setReason

```cpp
void setReason(const std::string & reason)
```

Sets the HTTP reason phrase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reason` | `const std::string &` |  |

---

{#getreason}

#### getReason

`const`

```cpp
const std::string & getReason() const
```

Returns the HTTP reason phrase.

---

{#setstatusandreason}

#### setStatusAndReason

```cpp
void setStatusAndReason(StatusCode status, const std::string & reason)
```

Sets the HTTP status code and reason phrase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | `[StatusCode](#statuscode)` |  |
| `reason` | `const std::string &` |  |

---

{#setdate}

#### setDate

```cpp
void setDate(const Timestamp & dateTime)
```

Sets the Date header to the given date/time value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const [Timestamp](base.md#timestamp) &` |  |

---

{#getdate}

#### getDate

`const`

```cpp
Timestamp getDate() const
```

Returns the value of the Date header.

---

{#addcookie}

#### addCookie

```cpp
void addCookie(const Cookie & cookie)
```

Adds the cookie to the response by adding a Set-Cookie header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookie` | `const [Cookie](#cookie) &` |  |

---

{#getcookies-1}

#### getCookies

`const`

```cpp
void getCookies(std::vector< Cookie > & cookies) const
```

Returns a vector with all the cookies set in the response header.

May throw an exception in case of a malformed Set-Cookie header.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cookies` | `std::vector< [Cookie](#cookie) > &` |  |

---

{#write-12}

#### write

`virtual` `const`

```cpp
virtual void write(std::ostream & ostr) const
```

Writes the HTTP response headers to the given output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#write-13}

#### write

`virtual` `const`

```cpp
virtual void write(std::string & str) const
```

Writes the HTTP response headers to the given output string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |

---

{#write-14}

#### write

`virtual` `const`

```cpp
virtual void write(Buffer & buf) const
```

Writes the HTTP response headers directly into a byte buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `[Buffer](base.md#buffer-2) &` |  |

---

{#success}

#### success

`virtual` `const`

```cpp
virtual bool success() const
```

Returns true if the HTTP response code was successful (< 400).

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `StatusCode` | [`_status`](#_status)  |  |
| `std::string` | [`_reason`](#_reason)  |  |

---

{#_status}

#### _status

```cpp
StatusCode _status
```

---

{#_reason}

#### _reason

```cpp
std::string _reason
```

{#server}

## Server

```cpp
#include <icy/http/server.h>
```

> **Inherits:** [`SocketAdapter`](net.md#socketadapter)

HTTP server implementation.

This HTTP server is not strictly standards compliant. It was created to be a fast (nocopy where possible) solution for streaming media to web browsers.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `LocalSignal< void(ServerConnection::Ptr)>` | [`Connection`](#connection-8)  | Signals when a new connection has been created. A reference to the new connection object is provided. |
| `LocalSignal< void()>` | [`Shutdown`](#shutdown-8)  | Signals when the server is shutting down. |

---

{#connection-8}

#### Connection

```cpp
LocalSignal< void(ServerConnection::Ptr)> Connection
```

Signals when a new connection has been created. A reference to the new connection object is provided.

---

{#shutdown-8}

#### Shutdown

```cpp
LocalSignal< void()> Shutdown
```

Signals when the server is shutting down.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Server`](#server-1)  | Constructs an HTTP server on the given host and port using an internally created TCP socket.  |
|  | [`Server`](#server-2)  | Constructs an HTTP server on the given address using an internally created TCP socket.  |
|  | [`Server`](#server-3)  | Constructs an HTTP server on the given host and port using a caller-supplied socket. Useful for HTTPS by passing an SSLSocket. The event loop is derived from the socket.  |
|  | [`Server`](#server-4)  | Constructs an HTTP server on the given address using a caller-supplied socket. The event loop is derived from the socket.  |
| `void` | [`start`](#start-9)  | Start the HTTP server. |
| `void` | [`shutdown`](#shutdown-9)  | Shutdown the HTTP server. |
| `void` | [`setReusePort`](#setreuseport-1) `inline` | Enable SO_REUSEPORT for multicore server instances. Must be called before [start()](#start-9). Allows multiple server instances to bind the same address:port with kernel-level load balancing (Linux 3.9+). |
| `void` | [`setMaxPooledConnections`](#setmaxpooledconnections) `inline` | Set the maximum number of pooled connections (default 128). Set to 0 to disable connection pooling entirely. |
| `void` | [`setKeepAliveTimeout`](#setkeepalivetimeout) `inline` | Set the keep-alive idle timeout in seconds (default 30). Connections idle longer than this are closed by the timer. Set to 0 to disable idle timeout. |
| `size_t` | [`connectionCount`](#connectioncount) `const` `inline` | Return the number of active connections (all states). |
| `net::Address &` | [`address`](#address-11)  | Return the server bind address. |
| `const DateCache &` | [`dateCache`](#datecache-1) `const` `inline` | Return the cached Date header for use in responses. |

---

{#server-1}

#### Server

```cpp
Server(const std::string & host, short port, uv::Loop * loop, std::unique_ptr< ServerConnectionFactory > factory)
```

Constructs an HTTP server on the given host and port using an internally created TCP socket. 
#### Parameters
* `host` Bind address (e.g. "0.0.0.0" or "127.0.0.1"). 

* `port` TCP port to listen on. 

* `loop` Event loop to use. Defaults to the default libuv loop. 

* `factory` [Connection](#connection-1) and responder factory. Defaults to the base factory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `short` |  |
| `loop` | `[uv::Loop](uv.md#loop) *` |  |
| `factory` | `std::unique_ptr< [ServerConnectionFactory](#serverconnectionfactory) >` |  |

---

{#server-2}

#### Server

```cpp
Server(const net::Address & address, uv::Loop * loop, std::unique_ptr< ServerConnectionFactory > factory)
```

Constructs an HTTP server on the given address using an internally created TCP socket. 
#### Parameters
* `address` Bind address and port. 

* `loop` Event loop to use. Defaults to the default libuv loop. 

* `factory` [Connection](#connection-1) and responder factory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const [net::Address](net.md#address) &` |  |
| `loop` | `[uv::Loop](uv.md#loop) *` |  |
| `factory` | `std::unique_ptr< [ServerConnectionFactory](#serverconnectionfactory) >` |  |

---

{#server-3}

#### Server

```cpp
Server(const std::string & host, short port, net::TCPSocket::Ptr socket, std::unique_ptr< ServerConnectionFactory > factory)
```

Constructs an HTTP server on the given host and port using a caller-supplied socket. Useful for HTTPS by passing an SSLSocket. The event loop is derived from the socket. 
#### Parameters
* `host` Bind address. 

* `port` TCP port to listen on. 

* `socket` Pre-created socket (e.g. SSLSocket for HTTPS). 

* `factory` [Connection](#connection-1) and responder factory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `short` |  |
| `socket` | `[net::TCPSocket::Ptr](net.md#ptr-9)` |  |
| `factory` | `std::unique_ptr< [ServerConnectionFactory](#serverconnectionfactory) >` |  |

---

{#server-4}

#### Server

```cpp
Server(const net::Address & address, net::TCPSocket::Ptr socket, std::unique_ptr< ServerConnectionFactory > factory)
```

Constructs an HTTP server on the given address using a caller-supplied socket. The event loop is derived from the socket. 
#### Parameters
* `address` Bind address and port. 

* `socket` Pre-created socket (e.g. SSLSocket for HTTPS). 

* `factory` [Connection](#connection-1) and responder factory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const [net::Address](net.md#address) &` |  |
| `socket` | `[net::TCPSocket::Ptr](net.md#ptr-9)` |  |
| `factory` | `std::unique_ptr< [ServerConnectionFactory](#serverconnectionfactory) >` |  |

---

{#start-9}

#### start

```cpp
void start()
```

Start the HTTP server.

---

{#shutdown-9}

#### shutdown

```cpp
void shutdown()
```

Shutdown the HTTP server.

---

{#setreuseport-1}

#### setReusePort

`inline`

```cpp
inline void setReusePort(bool enable)
```

Enable SO_REUSEPORT for multicore server instances. Must be called before [start()](#start-9). Allows multiple server instances to bind the same address:port with kernel-level load balancing (Linux 3.9+).

| Parameter | Type | Description |
|-----------|------|-------------|
| `enable` | `bool` |  |

---

{#setmaxpooledconnections}

#### setMaxPooledConnections

`inline`

```cpp
inline void setMaxPooledConnections(size_t n)
```

Set the maximum number of pooled connections (default 128). Set to 0 to disable connection pooling entirely.

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `size_t` |  |

---

{#setkeepalivetimeout}

#### setKeepAliveTimeout

`inline`

```cpp
inline void setKeepAliveTimeout(int seconds)
```

Set the keep-alive idle timeout in seconds (default 30). Connections idle longer than this are closed by the timer. Set to 0 to disable idle timeout.

| Parameter | Type | Description |
|-----------|------|-------------|
| `seconds` | `int` |  |

---

{#connectioncount}

#### connectionCount

`const` `inline`

```cpp
inline size_t connectionCount() const
```

Return the number of active connections (all states).

---

{#address-11}

#### address

```cpp
net::Address & address()
```

Return the server bind address.

---

{#datecache-1}

#### dateCache

`const` `inline`

```cpp
inline const DateCache & dateCache() const
```

Return the cached Date header for use in responses.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uv::Loop *` | [`_loop`](#_loop-1)  |  |
| `net::Address` | [`_address`](#_address)  |  |
| `net::TCPSocket::Ptr` | [`_socket`](#_socket-2)  |  |
| `Timer` | [`_timer`](#_timer)  |  |
| `std::unique_ptr< ServerConnectionFactory >` | [`_factory`](#_factory)  |  |
| `std::unordered_map< ServerConnection *, ServerConnection::Ptr >` | [`_connections`](#_connections-1)  |  |
| `ConnectionPool` | [`_pool`](#_pool-1)  |  |
| `DateCache` | [`_dateCache`](#_datecache)  |  |
| `int` | [`_keepAliveTimeout`](#_keepalivetimeout)  |  |
| `bool` | [`_reusePort`](#_reuseport)  |  |

---

{#_loop-1}

#### _loop

```cpp
uv::Loop * _loop
```

---

{#_address}

#### _address

```cpp
net::Address _address
```

---

{#_socket-2}

#### _socket

```cpp
net::TCPSocket::Ptr _socket
```

---

{#_timer}

#### _timer

```cpp
Timer _timer
```

---

{#_factory}

#### _factory

```cpp
std::unique_ptr< ServerConnectionFactory > _factory
```

---

{#_connections-1}

#### _connections

```cpp
std::unordered_map< ServerConnection *, ServerConnection::Ptr > _connections
```

---

{#_pool-1}

#### _pool

```cpp
ConnectionPool _pool
```

---

{#_datecache}

#### _dateCache

```cpp
DateCache _dateCache
```

---

{#_keepalivetimeout}

#### _keepAliveTimeout

```cpp
int _keepAliveTimeout {30}
```

---

{#_reuseport}

#### _reusePort

```cpp
bool _reusePort {false}
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< ServerResponder >` | [`createResponder`](#createresponder)  |  |
| `void` | [`onClientSocketAccept`](#onclientsocketaccept)  |  |
| `void` | [`onConnectionReady`](#onconnectionready)  |  |
| `void` | [`onConnectionClose`](#onconnectionclose-1)  |  |
| `bool` | [`onSocketClose`](#onsocketclose-3) `virtual` | Called when the socket is closed. Forwards the event to all registered receivers in priority order.  |
| `void` | [`onTimer`](#ontimer)  |  |
| `uv::Loop *` | [`loop`](#loop-6) `const` `inline` | Return the event loop this server runs on. |

---

{#createresponder}

#### createResponder

```cpp
std::unique_ptr< ServerResponder > createResponder(ServerConnection & conn)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ServerConnection](#serverconnection) &` |  |

---

{#onclientsocketaccept}

#### onClientSocketAccept

```cpp
void onClientSocketAccept(const net::TCPSocket::Ptr & socket)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const [net::TCPSocket::Ptr](net.md#ptr-9) &` |  |

---

{#onconnectionready}

#### onConnectionReady

```cpp
void onConnectionReady(ServerConnection & conn)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ServerConnection](#serverconnection) &` |  |

---

{#onconnectionclose-1}

#### onConnectionClose

```cpp
void onConnectionClose(ServerConnection & conn)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `conn` | `[ServerConnection](#serverconnection) &` |  |

---

{#onsocketclose-3}

#### onSocketClose

`virtual`

```cpp
virtual bool onSocketClose(net::Socket & socket)
```

Called when the socket is closed. Forwards the event to all registered receivers in priority order. 
#### Parameters
* `socket` The socket that was closed. 

#### Returns
true to stop propagation to subsequent receivers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |

---

{#ontimer}

#### onTimer

```cpp
void onTimer()
```

---

{#loop-6}

#### loop

`const` `inline`

```cpp
inline uv::Loop * loop() const
```

Return the event loop this server runs on.

{#serverconnection}

## ServerConnection

```cpp
#include <icy/http/server.h>
```

> **Inherits:** [`Connection`](#connection-1)

HTTP server connection.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `LocalSignal< void(ServerConnection &, const MutableBuffer &)>` | [`Payload`](#payload-1)  | Signals when raw data is received. |
| `LocalSignal< void(ServerConnection &)>` | [`Close`](#close-21)  | Signals when the connection is closed. |

---

{#payload-1}

#### Payload

```cpp
LocalSignal< void(ServerConnection &, const MutableBuffer &)> Payload
```

Signals when raw data is received.

---

{#close-21}

#### Close

```cpp
LocalSignal< void(ServerConnection &)> Close
```

Signals when the connection is closed.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ServerConnection`](#serverconnection-1)  | Creates a [ServerConnection](#serverconnection) attached to the given server and socket.  |
| `Server &` | [`server`](#server-5)  | Returns the owning [Server](#server) instance. |
| `ServerConnectionState` | [`state`](#state-1) `const` `inline` | Returns the current server-side connection state. |
| `ServerConnectionMode` | [`mode`](#mode-1) `const` `inline` | Returns the current transport mode. |
| `bool` | [`upgraded`](#upgraded) `const` `inline` | Returns true if the connection has been upgraded (e.g. to WebSocket). |
| `bool` | [`streaming`](#streaming) `const` `inline` | Returns true if the connection is in long-lived streaming mode. |
| `bool` | [`idleTimeoutEnabled`](#idletimeoutenabled) `const` | Returns true if the server idle timer is allowed to reap this connection. |
| `bool` | [`reusableForPool`](#reusableforpool) `const` | Returns true if the closed connection can be returned to the reuse pool. |
| `void` | [`markActive`](#markactive-1) `virtual` `inline` | Refresh the idle timer. |
| `void` | [`reset`](#reset-12)  | Reset this connection for reuse with a new socket. Called by the connection pool to avoid allocating a new [ServerConnection](#serverconnection). |
| `void` | [`touch`](#touch) `inline` | Update the last activity timestamp. |
| `double` | [`idleSeconds`](#idleseconds) `const` `inline` | Return seconds since last activity. |
| `void` | [`beginStreaming`](#beginstreaming-1) `virtual` | Explicitly mark the response as long-lived streaming. Streaming connections are excluded from the keep-alive idle reaper. |
| `void` | [`endStreaming`](#endstreaming-1) `virtual` | Exit streaming mode and return to the given HTTP state. |
| `void` | [`endStreaming`](#endstreaming-2)  |  |
| `ssize_t` | [`sendHeader`](#sendheader-1) `virtual` | Send the outgoing HTTP header. |
| `void` | [`close`](#close-22) `virtual` | Close the connection with an explicit terminal state transition. |

---

{#serverconnection-1}

#### ServerConnection

```cpp
ServerConnection(Server & server, net::TCPSocket::Ptr socket)
```

Creates a [ServerConnection](#serverconnection) attached to the given server and socket. 
#### Parameters
* `server` The owning HTTP server instance. 

* `socket` The accepted TCP socket for this connection.

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `[Server](#server) &` |  |
| `socket` | `[net::TCPSocket::Ptr](net.md#ptr-9)` |  |

---

{#server-5}

#### server

```cpp
Server & server()
```

Returns the owning [Server](#server) instance.

---

{#state-1}

#### state

`const` `inline`

```cpp
inline ServerConnectionState state() const
```

Returns the current server-side connection state.

---

{#mode-1}

#### mode

`const` `inline`

```cpp
inline ServerConnectionMode mode() const
```

Returns the current transport mode.

---

{#upgraded}

#### upgraded

`const` `inline`

```cpp
inline bool upgraded() const
```

Returns true if the connection has been upgraded (e.g. to WebSocket).

---

{#streaming}

#### streaming

`const` `inline`

```cpp
inline bool streaming() const
```

Returns true if the connection is in long-lived streaming mode.

---

{#idletimeoutenabled}

#### idleTimeoutEnabled

`const`

```cpp
bool idleTimeoutEnabled() const
```

Returns true if the server idle timer is allowed to reap this connection.

---

{#reusableforpool}

#### reusableForPool

`const`

```cpp
bool reusableForPool() const
```

Returns true if the closed connection can be returned to the reuse pool.

---

{#markactive-1}

#### markActive

`virtual` `inline`

```cpp
virtual inline void markActive()
```

Refresh the idle timer.

---

{#reset-12}

#### reset

```cpp
void reset(net::TCPSocket::Ptr socket)
```

Reset this connection for reuse with a new socket. Called by the connection pool to avoid allocating a new [ServerConnection](#serverconnection).

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::TCPSocket::Ptr](net.md#ptr-9)` |  |

---

{#touch}

#### touch

`inline`

```cpp
inline void touch()
```

Update the last activity timestamp.

---

{#idleseconds}

#### idleSeconds

`const` `inline`

```cpp
inline double idleSeconds() const
```

Return seconds since last activity.

---

{#beginstreaming-1}

#### beginStreaming

`virtual`

```cpp
virtual void beginStreaming()
```

Explicitly mark the response as long-lived streaming. Streaming connections are excluded from the keep-alive idle reaper.

---

{#endstreaming-1}

#### endStreaming

`virtual`

```cpp
virtual void endStreaming()
```

Exit streaming mode and return to the given HTTP state.

---

{#endstreaming-2}

#### endStreaming

```cpp
void endStreaming(ServerConnectionState nextState)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `nextState` | `[ServerConnectionState](#serverconnectionstate)` |  |

---

{#sendheader-1}

#### sendHeader

`virtual`

```cpp
virtual ssize_t sendHeader()
```

Send the outgoing HTTP header.

---

{#close-22}

#### close

`virtual`

```cpp
virtual void close()
```

Close the connection with an explicit terminal state transition.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Server &` | [`_server`](#_server)  |  |
| `std::unique_ptr< ServerResponder >` | [`_responder`](#_responder)  |  |
| `std::time_t` | [`_lastActivity`](#_lastactivity)  |  |
| `ServerConnectionState` | [`_state`](#_state-2)  |  |
| `ServerConnectionMode` | [`_mode`](#_mode-2)  |  |

---

{#_server}

#### _server

```cpp
Server & _server
```

---

{#_responder}

#### _responder

```cpp
std::unique_ptr< ServerResponder > _responder
```

---

{#_lastactivity}

#### _lastActivity

```cpp
std::time_t _lastActivity {0}
```

---

{#_state-2}

#### _state

```cpp
ServerConnectionState _state {}
```

---

{#_mode-2}

#### _mode

```cpp
ServerConnectionMode _mode {}
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onHeaders`](#onheaders-2) `virtual` | Called when the incoming HTTP headers have been fully parsed. |
| `void` | [`onPayload`](#onpayload-2) `virtual` | Called for each chunk of incoming body data after headers are complete.  |
| `void` | [`onComplete`](#oncomplete-2) `virtual` | Called when the incoming HTTP message is fully received. |
| `void` | [`onClose`](#onclose-5) `virtual` | Called when the connection is closed. |
| `http::Message *` | [`incomingHeader`](#incomingheader-2) `virtual` | Returns the incoming HTTP message header (request or response depending on role). |
| `http::Message *` | [`outgoingHeader`](#outgoingheader-2) `virtual` | Returns the outgoing HTTP message header (request or response depending on role). |
| `void` | [`setState`](#setstate)  |  |
| `bool` | [`requestHasBody`](#requesthasbody) `const` |  |
| `bool` | [`responseLooksStreaming`](#responselooksstreaming) `const` |  |

---

{#onheaders-2}

#### onHeaders

`virtual`

```cpp
virtual void onHeaders()
```

Called when the incoming HTTP headers have been fully parsed.

---

{#onpayload-2}

#### onPayload

`virtual`

```cpp
virtual void onPayload(const MutableBuffer & buffer)
```

Called for each chunk of incoming body data after headers are complete. 
#### Parameters
* `buffer` Buffer containing the received data chunk.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |

---

{#oncomplete-2}

#### onComplete

`virtual`

```cpp
virtual void onComplete()
```

Called when the incoming HTTP message is fully received.

---

{#onclose-5}

#### onClose

`virtual`

```cpp
virtual void onClose()
```

Called when the connection is closed.

---

{#incomingheader-2}

#### incomingHeader

`virtual`

```cpp
virtual http::Message * incomingHeader()
```

Returns the incoming HTTP message header (request or response depending on role).

---

{#outgoingheader-2}

#### outgoingHeader

`virtual`

```cpp
virtual http::Message * outgoingHeader()
```

Returns the outgoing HTTP message header (request or response depending on role).

---

{#setstate}

#### setState

```cpp
void setState(ServerConnectionState state)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `[ServerConnectionState](#serverconnectionstate)` |  |

---

{#requesthasbody}

#### requestHasBody

`const`

```cpp
bool requestHasBody() const
```

---

{#responselooksstreaming}

#### responseLooksStreaming

`const`

```cpp
bool responseLooksStreaming() const
```

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-13)  |  |

---

{#ptr-13}

#### Ptr

```cpp
std::shared_ptr< ServerConnection > Ptr()
```

{#serverconnectionfactory}

## ServerConnectionFactory

```cpp
#include <icy/http/server.h>
```

This implementation of a [ServerConnectionFactory](#serverconnectionfactory) is used by HTTP [Server](#server) to create [ServerConnection](#serverconnection) objects.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ServerConnectionFactory`](#serverconnectionfactory-1)  |  |
| `ServerConnection::Ptr` | [`createConnection`](#createconnection-2) `virtual` `inline` | Factory method for instantiating the [ServerConnection](#serverconnection) instance using the given Socket. |
| `std::unique_ptr< ServerResponder >` | [`createResponder`](#createresponder-1) `virtual` `inline` | Factory method for instantiating the [ServerResponder](#serverresponder) instance using the given [ServerConnection](#serverconnection). |

---

{#serverconnectionfactory-1}

#### ServerConnectionFactory

```cpp
ServerConnectionFactory() = default
```

---

{#createconnection-2}

#### createConnection

`virtual` `inline`

```cpp
virtual inline ServerConnection::Ptr createConnection(Server & server, const net::TCPSocket::Ptr & socket)
```

Factory method for instantiating the [ServerConnection](#serverconnection) instance using the given Socket.

| Parameter | Type | Description |
|-----------|------|-------------|
| `server` | `[Server](#server) &` |  |
| `socket` | `const [net::TCPSocket::Ptr](net.md#ptr-9) &` |  |

---

{#createresponder-1}

#### createResponder

`virtual` `inline`

```cpp
virtual inline std::unique_ptr< ServerResponder > createResponder(ServerConnection & connection)
```

Factory method for instantiating the [ServerResponder](#serverresponder) instance using the given [ServerConnection](#serverconnection).

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[ServerConnection](#serverconnection) &` |  |

{#serverresponder}

## ServerResponder

```cpp
#include <icy/http/server.h>
```

The abstract base class for HTTP ServerResponders created by HTTP [Server](#server).

Derived classes should override the [onRequest()](#onrequest) method.

A new [ServerResponder](#serverresponder) object can be created for each new HTTP request that is received by the HTTP [Server](#server).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ServerResponder`](#serverresponder-1) `inline` | Creates a [ServerResponder](#serverresponder) for the given connection.  |
| `void` | [`onHeaders`](#onheaders-3) `virtual` `inline` | Called when the incoming request headers have been parsed.  |
| `void` | [`onPayload`](#onpayload-3) `virtual` `inline` | Called for each chunk of incoming request body data.  |
| `void` | [`onRequest`](#onrequest) `virtual` `inline` | Called when the complete HTTP request has been received. Derived classes should write their response here.  |
| `void` | [`onClose`](#onclose-6) `virtual` `inline` | Called when the connection is closed. |
| `ServerConnection &` | [`connection`](#connection-9) `inline` | Returns the underlying server connection. |
| `Request &` | [`request`](#request-10) `inline` | Returns the current HTTP request from the underlying connection. |
| `Response &` | [`response`](#response-8) `inline` | Returns the current HTTP response from the underlying connection. |

---

{#serverresponder-1}

#### ServerResponder

`inline`

```cpp
inline ServerResponder(ServerConnection & connection)
```

Creates a [ServerResponder](#serverresponder) for the given connection. 
#### Parameters
* `connection` The server connection this responder handles.

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[ServerConnection](#serverconnection) &` |  |

---

{#onheaders-3}

#### onHeaders

`virtual` `inline`

```cpp
virtual inline void onHeaders(Request & request)
```

Called when the incoming request headers have been parsed. 
#### Parameters
* `request` The parsed HTTP request with headers populated.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[Request](#request-4) &` |  |

---

{#onpayload-3}

#### onPayload

`virtual` `inline`

```cpp
virtual inline void onPayload(const MutableBuffer & body)
```

Called for each chunk of incoming request body data. 
#### Parameters
* `body` Buffer containing a chunk of the request body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `body` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |

---

{#onrequest}

#### onRequest

`virtual` `inline`

```cpp
virtual inline void onRequest(Request & request, Response & response)
```

Called when the complete HTTP request has been received. Derived classes should write their response here. 
#### Parameters
* `request` The fully received HTTP request. 

* `response` The HTTP response to populate and send.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[Request](#request-4) &` |  |
| `response` | `[Response](#response-1) &` |  |

---

{#onclose-6}

#### onClose

`virtual` `inline`

```cpp
virtual inline void onClose()
```

Called when the connection is closed.

---

{#connection-9}

#### connection

`inline`

```cpp
inline ServerConnection & connection()
```

Returns the underlying server connection.

---

{#request-10}

#### request

`inline`

```cpp
inline Request & request()
```

Returns the current HTTP request from the underlying connection.

---

{#response-8}

#### response

`inline`

```cpp
inline Response & response()
```

Returns the current HTTP response from the underlying connection.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ServerConnection &` | [`_connection`](#_connection-2)  |  |

---

{#_connection-2}

#### _connection

```cpp
ServerConnection & _connection
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ServerResponder`](#serverresponder-2)  |  |

---

{#serverresponder-2}

#### ServerResponder

```cpp
ServerResponder(const ServerResponder &) = delete
```

{#stringpart}

## StringPart

```cpp
#include <icy/http/form.h>
```

> **Inherits:** [`FormPart`](#formpart)

An implementation of [StringPart](#stringpart) for plain files.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`StringPart`](#stringpart-1)  | Creates a [StringPart](#stringpart) with the given data and default content type "application/octet-stream".  |
|  | [`StringPart`](#stringpart-2)  | Creates a [StringPart](#stringpart) with the given data and MIME content type.  |
|  | [`~StringPart`](#stringpart-3) `virtual` | Destroys the [StringPart](#stringpart). |
| `bool` | [`writeChunk`](#writechunk-2) `virtual` | Writes the string data as a single chunk to the [FormWriter](#formwriter).  |
| `void` | [`write`](#write-15) `virtual` | Writes the string data to the [FormWriter](#formwriter).  |
| `void` | [`write`](#write-16) `virtual` | Writes the string data to an output stream.  |
| `uint64_t` | [`length`](#length-3) `virtual` `const` | Returns the byte length of the string data. |

---

{#stringpart-1}

#### StringPart

```cpp
StringPart(const std::string & path)
```

Creates a [StringPart](#stringpart) with the given data and default content type "application/octet-stream". 
#### Parameters
* `path` String data to send as this part.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

{#stringpart-2}

#### StringPart

```cpp
StringPart(const std::string & data, const std::string & contentType)
```

Creates a [StringPart](#stringpart) with the given data and MIME content type. 
#### Parameters
* `data` String data to send as this part. 

* `contentType` MIME type for this part.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const std::string &` |  |
| `contentType` | `const std::string &` |  |

---

{#stringpart-3}

#### ~StringPart

`virtual`

```cpp
virtual ~StringPart()
```

Destroys the [StringPart](#stringpart).

---

{#writechunk-2}

#### writeChunk

`virtual`

```cpp
virtual bool writeChunk(FormWriter & writer)
```

Writes the string data as a single chunk to the [FormWriter](#formwriter). 
#### Parameters
* `writer` The [FormWriter](#formwriter) to send data through. 

#### Returns
false always (string data is sent in a single chunk).

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#formwriter) &` |  |

---

{#write-15}

#### write

`virtual`

```cpp
virtual void write(FormWriter & writer)
```

Writes the string data to the [FormWriter](#formwriter). 
#### Parameters
* `writer` The [FormWriter](#formwriter) to send data through.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[FormWriter](#formwriter) &` |  |

---

{#write-16}

#### write

`virtual`

```cpp
virtual void write(std::ostream & ostr)
```

Writes the string data to an output stream. 
#### Parameters
* `ostr` Output stream to write to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ostr` | `std::ostream &` |  |

---

{#length-3}

#### length

`virtual` `const`

```cpp
virtual uint64_t length() const
```

Returns the byte length of the string data.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_data`](#_data)  |  |

---

{#_data}

#### _data

```cpp
std::string _data
```

{#url}

## URL

```cpp
#include <icy/http/url.h>
```

An RFC 3986 based [URL](#url) parser. Constructors and assignment operators will throw a SyntaxException if the [URL](#url) is invalid.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`URL`](#url-1)  | Creates an empty [URL](#url). |
|  | [`URL`](#url-2)  | Parses the [URL](#url) from a null-terminated string.  |
|  | [`URL`](#url-3)  | Parses the [URL](#url) from a std::string.  |
|  | [`URL`](#url-4)  | Constructs a [URL](#url) from scheme and authority components.  |
|  | [`URL`](#url-5)  | Constructs a [URL](#url) from scheme, authority, and path+query+fragment.  |
|  | [`URL`](#url-6)  | Constructs a [URL](#url) from individual components.  |
|  | [`URL`](#url-7)  |  |
| `URL &` | [`operator=`](#operator-15)  | Assigns a [URL](#url) from another [URL](#url) instance.  |
| `URL &` | [`operator=`](#operator-16)  | Assigns a [URL](#url) from a null-terminated string.  |
| `URL &` | [`operator=`](#operator-17)  | Assigns a [URL](#url) from a std::string.  |
| `bool` | [`parse`](#parse-1)  | Parses and assigns a [URL](#url) from the given string view, resetting all components first.  |
| `std::string` | [`scheme`](#scheme) `const` | Returns the [URL](#url) scheme (e.g. "http", "https", "ws"). Always lowercase. |
| `std::string` | [`userInfo`](#userinfo) `const` | Returns the user info component (e.g. "user:pass" from "http://user:pass@host/"). Returns an empty string if not present. |
| `std::string` | [`host`](#host-1) `const` | Returns the host component (e.g. "example.com"). Returns an empty string if not present. |
| `uint16_t` | [`port`](#port-1) `const` | Returns the port number. If no explicit port was in the [URL](#url), returns the default port for the scheme (80 for http, 443 for https), or 0 if the scheme is unknown. |
| `std::string` | [`authority`](#authority) `const` | Returns the authority component (`userinfo@host:port`). Only includes components that are present. |
| `std::string` | [`path`](#path) `const` | Returns the path component (e.g. "/index.html"). Returns an empty string if not present. |
| `std::string` | [`pathEtc`](#pathetc) `const` | Returns the path, query and fragment combined (e.g. "/path?q=1#frag"). |
| `std::string` | [`query`](#query) `const` | Returns the query string without the leading '?' (e.g. "key=value&foo=bar"). Returns an empty string if not present. |
| `std::string` | [`fragment`](#fragment) `const` | Returns the fragment identifier without the leading '#'. Returns an empty string if not present. |
| `bool` | [`hasSchema`](#hasschema) `const` | Returns true if the [URL](#url) has a scheme component. |
| `bool` | [`hasUserInfo`](#hasuserinfo) `const` | Returns true if the [URL](#url) has a user info component. |
| `bool` | [`hasHost`](#hashost) `const` | Returns true if the [URL](#url) has a host component. |
| `bool` | [`hasPort`](#hasport) `const` | Returns true if an explicit port was specified in the [URL](#url). |
| `bool` | [`hasPath`](#haspath) `const` | Returns true if the [URL](#url) has a path component. |
| `bool` | [`hasQuery`](#hasquery) `const` | Returns true if the [URL](#url) has a query component. |
| `bool` | [`hasFragment`](#hasfragment) `const` | Returns true if the [URL](#url) has a fragment component. |
| `bool` | [`valid`](#valid-1) `const` | Returns true if the [URL](#url) is non-empty and was successfully parsed. |
| `std::string` | [`str`](#str-1) `const` | Returns the original [URL](#url) string as parsed. |

---

{#url-1}

#### URL

```cpp
URL()
```

Creates an empty [URL](#url).

---

{#url-2}

#### URL

```cpp
URL(const char * url)
```

Parses the [URL](#url) from a null-terminated string. 
#### Parameters
* `url` Null-terminated [URL](#url) string to parse.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const char *` |  |

---

{#url-3}

#### URL

```cpp
URL(const std::string & url)
```

Parses the [URL](#url) from a std::string. 
#### Parameters
* `url` [URL](#url) string to parse.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `const std::string &` |  |

---

{#url-4}

#### URL

```cpp
URL(const std::string & scheme, const std::string & authority)
```

Constructs a [URL](#url) from scheme and authority components. 
#### Parameters
* `scheme` [URL](#url) scheme (e.g. "http", "https"). 

* `authority` Host and optional port (e.g. "example.com:8080").

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `const std::string &` |  |
| `authority` | `const std::string &` |  |

---

{#url-5}

#### URL

```cpp
URL(const std::string & scheme, const std::string & authority, const std::string & pathEtc)
```

Constructs a [URL](#url) from scheme, authority, and path+query+fragment. 
#### Parameters
* `scheme` [URL](#url) scheme (e.g. "http"). 

* `authority` Host and optional port. 

* `pathEtc` Path, query and fragment combined (e.g. "/path?q=1#frag").

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `const std::string &` |  |
| `authority` | `const std::string &` |  |
| `pathEtc` | `const std::string &` |  |

---

{#url-6}

#### URL

```cpp
URL(const std::string & scheme, const std::string & authority, const std::string & path, const std::string & query, const std::string & fragment)
```

Constructs a [URL](#url) from individual components. 
#### Parameters
* `scheme` [URL](#url) scheme (e.g. "http"). 

* `authority` Host and optional port. 

* `path` [URL](#url) path (e.g. "/index.html"). 

* `query` Query string without leading '?' (e.g. "key=value"). 

* `fragment` Fragment identifier without leading '#'.

| Parameter | Type | Description |
|-----------|------|-------------|
| `scheme` | `const std::string &` |  |
| `authority` | `const std::string &` |  |
| `path` | `const std::string &` |  |
| `query` | `const std::string &` |  |
| `fragment` | `const std::string &` |  |

---

{#url-7}

#### URL

```cpp
URL(const URL &) = default
```

---

{#operator-15}

#### operator=

```cpp
URL & operator=(const URL & uri)
```

Assigns a [URL](#url) from another [URL](#url) instance. 
#### Parameters
* `uri` Source [URL](#url) to copy from. 

#### Returns
Reference to this [URL](#url).

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `const [URL](#url) &` |  |

---

{#operator-16}

#### operator=

```cpp
URL & operator=(const char * uri)
```

Assigns a [URL](#url) from a null-terminated string. 
#### Parameters
* `uri` Null-terminated [URL](#url) string to parse. 

#### Returns
Reference to this [URL](#url).

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `const char *` |  |

---

{#operator-17}

#### operator=

```cpp
URL & operator=(const std::string & uri)
```

Assigns a [URL](#url) from a std::string. 
#### Parameters
* `uri` [URL](#url) string to parse. 

#### Returns
Reference to this [URL](#url).

| Parameter | Type | Description |
|-----------|------|-------------|
| `uri` | `const std::string &` |  |

---

{#parse-1}

#### parse

```cpp
bool parse(std::string_view url, bool whiny)
```

Parses and assigns a [URL](#url) from the given string view, resetting all components first. 
#### Parameters
* `url` [URL](#url) string to parse. 

* `whiny` If true, throws std::runtime_error on an invalid [URL](#url); otherwise returns false. 

#### Returns
true if the [URL](#url) was parsed successfully; false if invalid and whiny is false.

| Parameter | Type | Description |
|-----------|------|-------------|
| `url` | `std::string_view` |  |
| `whiny` | `bool` |  |

---

{#scheme}

#### scheme

`const`

```cpp
std::string scheme() const
```

Returns the [URL](#url) scheme (e.g. "http", "https", "ws"). Always lowercase.

---

{#userinfo}

#### userInfo

`const`

```cpp
std::string userInfo() const
```

Returns the user info component (e.g. "user:pass" from "http://user:pass@host/"). Returns an empty string if not present.

---

{#host-1}

#### host

`const`

```cpp
std::string host() const
```

Returns the host component (e.g. "example.com"). Returns an empty string if not present.

---

{#port-1}

#### port

`const`

```cpp
uint16_t port() const
```

Returns the port number. If no explicit port was in the [URL](#url), returns the default port for the scheme (80 for http, 443 for https), or 0 if the scheme is unknown.

---

{#authority}

#### authority

`const`

```cpp
std::string authority() const
```

Returns the authority component (`userinfo@host:port`). Only includes components that are present.

---

{#path}

#### path

`const`

```cpp
std::string path() const
```

Returns the path component (e.g. "/index.html"). Returns an empty string if not present.

---

{#pathetc}

#### pathEtc

`const`

```cpp
std::string pathEtc() const
```

Returns the path, query and fragment combined (e.g. "/path?q=1#frag").

---

{#query}

#### query

`const`

```cpp
std::string query() const
```

Returns the query string without the leading '?' (e.g. "key=value&foo=bar"). Returns an empty string if not present.

---

{#fragment}

#### fragment

`const`

```cpp
std::string fragment() const
```

Returns the fragment identifier without the leading '#'. Returns an empty string if not present.

---

{#hasschema}

#### hasSchema

`const`

```cpp
bool hasSchema() const
```

Returns true if the [URL](#url) has a scheme component.

---

{#hasuserinfo}

#### hasUserInfo

`const`

```cpp
bool hasUserInfo() const
```

Returns true if the [URL](#url) has a user info component.

---

{#hashost}

#### hasHost

`const`

```cpp
bool hasHost() const
```

Returns true if the [URL](#url) has a host component.

---

{#hasport}

#### hasPort

`const`

```cpp
bool hasPort() const
```

Returns true if an explicit port was specified in the [URL](#url).

---

{#haspath}

#### hasPath

`const`

```cpp
bool hasPath() const
```

Returns true if the [URL](#url) has a path component.

---

{#hasquery}

#### hasQuery

`const`

```cpp
bool hasQuery() const
```

Returns true if the [URL](#url) has a query component.

---

{#hasfragment}

#### hasFragment

`const`

```cpp
bool hasFragment() const
```

Returns true if the [URL](#url) has a fragment component.

---

{#valid-1}

#### valid

`const`

```cpp
bool valid() const
```

Returns true if the [URL](#url) is non-empty and was successfully parsed.

---

{#str-1}

#### str

`const`

```cpp
std::string str() const
```

Returns the original [URL](#url) string as parsed.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`encode`](#encode-21) `static` | Percent-encodes a string per RFC 3986, preserving unreserved characters (A-Z, a-z, 0-9, '-', '_', '.', '~'). Equivalent to JavaScript's encodeURIComponent().  |
| `std::string` | [`decode`](#decode-10) `static` | Decodes a percent-encoded string per RFC 3986. Equivalent to JavaScript's decodeURIComponent().  |

---

{#encode-21}

#### encode

`static`

```cpp
static std::string encode(std::string_view str)
```

Percent-encodes a string per RFC 3986, preserving unreserved characters (A-Z, a-z, 0-9, '-', '_', '.', '~'). Equivalent to JavaScript's encodeURIComponent(). 
#### Parameters
* `str` Input string to encode. 

#### Returns
Percent-encoded string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |

---

{#decode-10}

#### decode

`static`

```cpp
static std::string decode(std::string_view str)
```

Decodes a percent-encoded string per RFC 3986. Equivalent to JavaScript's decodeURIComponent(). 
#### Parameters
* `str` Percent-encoded input string. 

#### Returns
Decoded string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_buf`](#_buf)  |  |
| `std::string` | [`_scheme`](#_scheme)  |  |
| `std::string` | [`_userInfo`](#_userinfo)  |  |
| `std::string` | [`_host`](#_host)  |  |
| `uint16_t` | [`_port`](#_port)  |  |
| `std::string` | [`_path`](#_path-2)  |  |
| `std::string` | [`_query`](#_query)  |  |
| `std::string` | [`_fragment`](#_fragment)  |  |
| `bool` | [`_hasPort`](#_hasport)  |  |

---

{#_buf}

#### _buf

```cpp
std::string _buf
```

---

{#_scheme}

#### _scheme

```cpp
std::string _scheme
```

---

{#_userinfo}

#### _userInfo

```cpp
std::string _userInfo
```

---

{#_host}

#### _host

```cpp
std::string _host
```

---

{#_port}

#### _port

```cpp
uint16_t _port
```

---

{#_path-2}

#### _path

```cpp
std::string _path
```

---

{#_query}

#### _query

```cpp
std::string _query
```

---

{#_fragment}

#### _fragment

```cpp
std::string _fragment
```

---

{#_hasport}

#### _hasPort

```cpp
bool _hasPort
```

{#datecache}

## DateCache

```cpp
#include <icy/http/server.h>
```

Caches the formatted Date header, updated once per second. Avoids per-request time formatting and string allocation.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `char` | [`buf`](#buf)  |  |
| `size_t` | [`len`](#len)  |  |
| `std::time_t` | [`lastSecond`](#lastsecond)  |  |

---

{#buf}

#### buf

```cpp
char buf {}
```

---

{#len}

#### len

```cpp
size_t len = 0
```

---

{#lastsecond}

#### lastSecond

```cpp
std::time_t lastSecond = 0
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`update`](#update) `inline` | Refreshes the cached Date header string if the current second has changed. No-op if called multiple times within the same second. |
| `const char *` | [`data`](#data-2) `const` `inline` | Returns a pointer to the formatted "Date: ...\r\n" header string. |
| `size_t` | [`size`](#size-1) `const` `inline` | Returns the byte length of the formatted Date header string. |

---

{#update}

#### update

`inline`

```cpp
inline void update()
```

Refreshes the cached Date header string if the current second has changed. No-op if called multiple times within the same second.

---

{#data-2}

#### data

`const` `inline`

```cpp
inline const char * data() const
```

Returns a pointer to the formatted "Date: ...\r\n" header string.

---

{#size-1}

#### size

`const` `inline`

```cpp
inline size_t size() const
```

Returns the byte length of the formatted Date header string.

{#method}

## Method

```cpp
#include <icy/http/request.h>
```

HTTP request methods.

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const std::string` | [`Get`](#get-4) `static` |  |
| `const std::string` | [`Head`](#head) `static` |  |
| `const std::string` | [`Put`](#put) `static` |  |
| `const std::string` | [`Post`](#post-2) `static` |  |
| `const std::string` | [`Options`](#options-3) `static` |  |
| `const std::string` | [`Delete`](#delete) `static` |  |
| `const std::string` | [`Trace`](#trace) `static` |  |
| `const std::string` | [`Connect`](#connect-13) `static` |  |

---

{#get-4}

#### Get

`static`

```cpp
const std::string Get
```

---

{#head}

#### Head

`static`

```cpp
const std::string Head
```

---

{#put}

#### Put

`static`

```cpp
const std::string Put
```

---

{#post-2}

#### Post

`static`

```cpp
const std::string Post
```

---

{#options-3}

#### Options

`static`

```cpp
const std::string Options
```

---

{#delete}

#### Delete

`static`

```cpp
const std::string Delete
```

---

{#trace}

#### Trace

`static`

```cpp
const std::string Trace
```

---

{#connect-13}

#### Connect

`static`

```cpp
const std::string Connect
```

{#ws}

# ws

[WebSocket](#websocket) framing, handshakes, and connection helpers.

### Classes

| Name | Description |
|------|-------------|
| [`ConnectionAdapter`](#connectionadapter-2) | [WebSocket](#websocket) class which belongs to a HTTP [Connection](#connection-1). |
| [`WebSocket`](#websocket) | Standalone [WebSocket](#websocket) class. |
| [`WebSocketAdapter`](#websocketadapter) | [WebSocket](#websocket) protocol adapter for both client and server endpoints. |
| [`WebSocketException`](#websocketexception) |  |
| [`WebSocketFramer`](#websocketframer) | This class implements a [WebSocket](#websocket) parser according to the [WebSocket](#websocket) protocol described in RFC 6455. |

### Enumerations

| Name | Description |
|------|-------------|
| [`Mode`](#mode-2)  | [WebSocket](#websocket) endpoint mode. |
| [`FrameFlags`](#frameflags)  | Frame header flags. |
| [`Opcode`](#opcode)  | Frame header opcodes. |
| [`SendFlags`](#sendflags)  | Combined header flags and opcodes for identifying the payload type of sent frames. |
| [`CloseStatusCode`](#closestatuscode)  | StatusCodes for CLOSE frames sent with shutdown(). |
| [`ErrorCode`](#errorcode)  | These error codes can be obtained from [WebSocket](#websocket) exceptions to determine the exact cause of the error. |
| [`CloseState`](#closestate)  |  |

---

{#mode-2}

#### Mode

```cpp
enum Mode
```

[WebSocket](#websocket) endpoint mode.

| Value | Description |
|-------|-------------|
| `ServerSide` | Server-side [WebSocket](#websocket). |
| `ClientSide` | Client-side [WebSocket](#websocket). |

---

{#frameflags}

#### FrameFlags

```cpp
enum FrameFlags
```

Frame header flags.

| Value | Description |
|-------|-------------|
| `Fin` | FIN bit: final fragment of a multi-fragment message. |
| `Rsv1` | Reserved for future use. Must be zero. |
| `Rsv2` | Reserved for future use. Must be zero. |
| `Rsv3` | Reserved for future use. Must be zero. |

---

{#opcode}

#### Opcode

```cpp
enum Opcode
```

Frame header opcodes.

| Value | Description |
|-------|-------------|
| `Continuation` | Continuation frame. |
| `Text` | Text frame. |
| `Binary` | Binary frame. |
| `Close` | Close connection. |
| `Ping` | Ping frame. |
| `Pong` | Pong frame. |
| `Bitmask` | Bit mask for opcodes. |

---

{#sendflags}

#### SendFlags

```cpp
enum SendFlags
```

Combined header flags and opcodes for identifying the payload type of sent frames.

| Value | Description |
|-------|-------------|
| `Text` |  |
| `Binary` |  |

---

{#closestatuscode}

#### CloseStatusCode

```cpp
enum CloseStatusCode
```

StatusCodes for CLOSE frames sent with shutdown().

| Value | Description |
|-------|-------------|
| `NormalClose` |  |
| `EndpointGoingAway` |  |
| `ProtocolError` |  |
| `PayloadNotAcceptable` |  |
| `Reserved` |  |
| `ReservedNoStatusCode` |  |
| `ReservedAbnormalClose` |  |
| `MalformedPayload` |  |
| `PolicyViolation` |  |
| `PayloadTooBig` |  |
| `ExtensionRequired` |  |
| `UnexpectedCondition` |  |
| `ReservedTLSFailure` |  |

---

{#errorcode}

#### ErrorCode

```cpp
enum ErrorCode
```

These error codes can be obtained from [WebSocket](#websocket) exceptions to determine the exact cause of the error.

| Value | Description |
|-------|-------------|
| `NoHandshake` | No [Connection](#connection-1): Upgrade or Upgrade: websocket header in handshake request. |
| `HandshakeNoVersion` | No Sec-WebSocket-Version header in handshake request. |
| `HandshakeUnsupportedVersion` | Unsupported [WebSocket](#websocket) version requested by client. |
| `HandshakeNoKey` | No Sec-WebSocket-Key header in handshake request. |
| `HandshakeAccept` | No Sec-WebSocket-Accept header or wrong value. |
| `Unauthorized` | The server rejected the username or password for authentication. |
| `PayloadTooBig` | Payload too big for supplied buffer. |
| `IncompleteFrame` | Incomplete frame received. |
| `InvalidRsvBits` | RSV bits set without extension negotiation. |
| `InvalidOpcode` | Unknown or reserved opcode received. |
| `UnmaskedClientFrame` | Client-to-server frame not masked (RFC 6455 violation). |
| `ProtocolViolation` | General RFC 6455 protocol violation. |

---

{#closestate}

#### CloseState

```cpp
enum CloseState
```

| Value | Description |
|-------|-------------|
| `Open` |  |
| `CloseSent` |  |
| `CloseReceived` |  |
| `Closed` |  |

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `constexpr auto` | [`ServerSide`](#serverside) `static` |  |
| `constexpr auto` | [`ClientSide`](#clientside) `static` |  |
| `constexpr char` | [`ProtocolGuid`](#protocolguid)  |  |
| `constexpr char` | [`ProtocolVersion`](#protocolversion)  | The [WebSocket](#websocket) protocol version supported (13). |

---

{#serverside}

#### ServerSide

`static`

```cpp
constexpr auto ServerSide = Mode::ServerSide
```

---

{#clientside}

#### ClientSide

`static`

```cpp
constexpr auto ClientSide = Mode::ClientSide
```

---

{#protocolguid}

#### ProtocolGuid

```cpp
constexpr char ProtocolGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
```

---

{#protocolversion}

#### ProtocolVersion

```cpp
constexpr char ProtocolVersion = "13"
```

The [WebSocket](#websocket) protocol version supported (13).

{#connectionadapter-2}

## ConnectionAdapter

```cpp
#include <icy/http/websocket.h>
```

> **Inherits:** [`WebSocketAdapter`](#websocketadapter)

[WebSocket](#websocket) class which belongs to a HTTP [Connection](#connection-1).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ConnectionAdapter`](#connectionadapter-3)  | Creates a [ConnectionAdapter](#connectionadapter-2) for upgrading an existing HTTP connection to [WebSocket](#websocket). Disables automatic header sending on the underlying connection.  |
| `void` | [`onHandshakeComplete`](#onhandshakecomplete) `virtual` | Called when the [WebSocket](#websocket) handshake completes. Emits the connect event via the socket emitter chain. |

---

{#connectionadapter-3}

#### ConnectionAdapter

```cpp
ConnectionAdapter(Connection * connection, ws::Mode mode)
```

Creates a [ConnectionAdapter](#connectionadapter-2) for upgrading an existing HTTP connection to [WebSocket](#websocket). Disables automatic header sending on the underlying connection. 
#### Parameters
* `connection` The HTTP connection to upgrade. 

* `mode` ServerSide or ClientSide.

| Parameter | Type | Description |
|-----------|------|-------------|
| `connection` | `[Connection](#connection-1) *` |  |
| `mode` | `[ws::Mode](#mode-2)` |  |

---

{#onhandshakecomplete}

#### onHandshakeComplete

`virtual`

```cpp
virtual void onHandshakeComplete()
```

Called when the [WebSocket](#websocket) handshake completes. Emits the connect event via the socket emitter chain.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Connection *` | [`_connection`](#_connection-3)  |  |

---

{#_connection-3}

#### _connection

```cpp
Connection * _connection
```

{#websocket}

## WebSocket

```cpp
#include <icy/http/websocket.h>
```

> **Inherits:** [`WebSocketAdapter`](#websocketadapter)

Standalone [WebSocket](#websocket) class.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WebSocket`](#websocket-1)  | Creates the [WebSocket](#websocket) with the given Socket. The Socket should be a TCPSocket or a SSLSocket, depending on the protocol used (ws or wss). |
| `http::Request &` | [`request`](#request-11)  | Returns the HTTP request used during the [WebSocket](#websocket) handshake. |
| `http::Response &` | [`response`](#response-9)  | Returns the HTTP response received during the [WebSocket](#websocket) handshake. |

---

{#websocket-1}

#### WebSocket

```cpp
WebSocket(const net::Socket::Ptr & socket)
```

Creates the [WebSocket](#websocket) with the given Socket. The Socket should be a TCPSocket or a SSLSocket, depending on the protocol used (ws or wss).

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const [net::Socket::Ptr](net.md#ptr-4) &` |  |

---

{#request-11}

#### request

```cpp
http::Request & request()
```

Returns the HTTP request used during the [WebSocket](#websocket) handshake.

---

{#response-9}

#### response

```cpp
http::Response & response()
```

Returns the HTTP response received during the [WebSocket](#websocket) handshake.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `http::Request` | [`_request`](#_request-2)  |  |
| `http::Response` | [`_response`](#_response-2)  |  |

---

{#_request-2}

#### _request

```cpp
http::Request _request
```

---

{#_response-2}

#### _response

```cpp
http::Response _response
```

### Public Types

| Name | Description |
|------|-------------|
| [`Vec`](#vec-4)  |  |

---

{#vec-4}

#### Vec

```cpp
std::vector< WebSocket > Vec()
```

{#websocketadapter}

## WebSocketAdapter

```cpp
#include <icy/http/websocket.h>
```

> **Inherits:** [`SocketEmitter`](net.md#socketemitter)
> **Subclassed by:** [`ConnectionAdapter`](#connectionadapter-2), [`WebSocket`](#websocket)

[WebSocket](#websocket) protocol adapter for both client and server endpoints.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `net::Socket::Ptr` | [`socket`](#socket-6)  | Pointer to the underlying socket. Sent data will be proxied to this socket. |

---

{#socket-6}

#### socket

```cpp
net::Socket::Ptr socket
```

Pointer to the underlying socket. Sent data will be proxied to this socket.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WebSocketAdapter`](#websocketadapter-1)  | Creates a [WebSocketAdapter](#websocketadapter) using the given socket, mode and HTTP message objects.  |
| `ssize_t` | [`send`](#send-14) `virtual` | Frames and sends data to the peer's address.  |
| `ssize_t` | [`send`](#send-15) `virtual` | Frames and sends data to a specific peer address (for UDP-backed sockets).  |
| `ssize_t` | [`sendOwned`](#sendowned-12) `virtual` | Sends an owned payload buffer to the connected peer. |
| `ssize_t` | [`sendOwned`](#sendowned-13) `virtual` |  |
| `bool` | [`shutdown`](#shutdown-10) `virtual` | Sends a [WebSocket](#websocket) CLOSE frame with the given status code and message, then closes the underlying socket.  |
| `void` | [`sendClientRequest`](#sendclientrequest) `virtual` | [Client](#client) side. |
| `void` | [`handleClientResponse`](#handleclientresponse) `virtual` | Parses the server's HTTP upgrade response and completes the handshake. Any data remaining in the buffer after the HTTP response is re-fed as [WebSocket](#websocket) frames.  |
| `void` | [`handleServerRequest`](#handleserverrequest) `virtual` | [Server](#server) side. |
| `void` | [`onHandshakeComplete`](#onhandshakecomplete-1) `virtual` | Called when the [WebSocket](#websocket) handshake completes. Emits the connect event to downstream handlers. |

---

{#websocketadapter-1}

#### WebSocketAdapter

```cpp
WebSocketAdapter(const net::Socket::Ptr & socket, ws::Mode mode, http::Request & request, http::Response & response)
```

Creates a [WebSocketAdapter](#websocketadapter) using the given socket, mode and HTTP message objects. 
#### Parameters
* `socket` The underlying TCP or SSL socket. 

* `mode` ServerSide or ClientSide. 

* `request` HTTP request used for the handshake. 

* `response` HTTP response used for the handshake.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const [net::Socket::Ptr](net.md#ptr-4) &` |  |
| `mode` | `[ws::Mode](#mode-2)` |  |
| `request` | `[http::Request](#request-4) &` |  |
| `response` | `[http::Response](#response-1) &` |  |

---

{#send-14}

#### send

`virtual`

```cpp
virtual ssize_t send(const char * data, size_t len, int flags)
```

Frames and sends data to the peer's address. 
#### Parameters
* `data` Pointer to the payload. 

* `len` Payload length in bytes. 

* `flags` [ws::SendFlags::Text](#namespaceicy_1_1http_1_1ws_1a366b9e6ee51be48df27ea06f8d20e7a2afdf2a2c6b74320a613fe71490a96e2ae) or [ws::SendFlags::Binary](#namespaceicy_1_1http_1_1ws_1a366b9e6ee51be48df27ea06f8d20e7a2a0ca55f2c2a2e576e0aa1c08a4e845ed7). 

#### Returns
Number of bytes sent, or -1 on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |

---

{#send-15}

#### send

`virtual`

```cpp
virtual ssize_t send(const char * data, size_t len, const net::Address & peerAddr, int flags)
```

Frames and sends data to a specific peer address (for UDP-backed sockets). 
#### Parameters
* `data` Pointer to the payload. 

* `len` Payload length in bytes. 

* `peerAddr` Destination address. 

* `flags` [ws::SendFlags::Text](#namespaceicy_1_1http_1_1ws_1a366b9e6ee51be48df27ea06f8d20e7a2afdf2a2c6b74320a613fe71490a96e2ae) or [ws::SendFlags::Binary](#namespaceicy_1_1http_1_1ws_1a366b9e6ee51be48df27ea06f8d20e7a2a0ca55f2c2a2e576e0aa1c08a4e845ed7). 

#### Returns
Number of bytes sent, or -1 on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `peerAddr` | `const [net::Address](net.md#address) &` |  |
| `flags` | `int` |  |

---

{#sendowned-12}

#### sendOwned

`virtual`

```cpp
virtual ssize_t sendOwned(Buffer && buffer, int flags)
```

Sends an owned payload buffer to the connected peer.

The buffer is moved through the adapter chain and retained by the transport layer until async write completion.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `[Buffer](base.md#buffer-2) &&` |  |
| `flags` | `int` |  |

---

{#sendowned-13}

#### sendOwned

`virtual`

```cpp
virtual ssize_t sendOwned(Buffer && buffer, const net::Address & peerAddr, int flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `[Buffer](base.md#buffer-2) &&` |  |
| `peerAddr` | `const [net::Address](net.md#address) &` |  |
| `flags` | `int` |  |

---

{#shutdown-10}

#### shutdown

`virtual`

```cpp
virtual bool shutdown(uint16_t statusCode, const std::string & statusMessage)
```

Sends a [WebSocket](#websocket) CLOSE frame with the given status code and message, then closes the underlying socket. 
#### Parameters
* `statusCode` [WebSocket](#websocket) close status code (e.g. 1000 for normal close). 

* `statusMessage` Human-readable reason for closing. 

#### Returns
true if the close frame was sent successfully.

| Parameter | Type | Description |
|-----------|------|-------------|
| `statusCode` | `uint16_t` |  |
| `statusMessage` | `const std::string &` |  |

---

{#sendclientrequest}

#### sendClientRequest

`virtual`

```cpp
virtual void sendClientRequest()
```

[Client](#client) side.

Sends the [WebSocket](#websocket) HTTP upgrade request to initiate the handshake. Called automatically on socket connect.

---

{#handleclientresponse}

#### handleClientResponse

`virtual`

```cpp
virtual void handleClientResponse(const MutableBuffer & buffer, const net::Address & peerAddr)
```

Parses the server's HTTP upgrade response and completes the handshake. Any data remaining in the buffer after the HTTP response is re-fed as [WebSocket](#websocket) frames. 
#### Parameters
* `buffer` Buffer containing the server's HTTP response. 

* `peerAddr` Address of the peer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |
| `peerAddr` | `const [net::Address](net.md#address) &` |  |

---

{#handleserverrequest}

#### handleServerRequest

`virtual`

```cpp
virtual void handleServerRequest(const MutableBuffer & buffer, const net::Address & peerAddr)
```

[Server](#server) side.

Parses the client's HTTP upgrade request and sends the 101 response. 
#### Parameters
* `buffer` Buffer containing the client's HTTP upgrade request. 

* `peerAddr` Address of the peer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |
| `peerAddr` | `const [net::Address](net.md#address) &` |  |

---

{#onhandshakecomplete-1}

#### onHandshakeComplete

`virtual`

```cpp
virtual void onHandshakeComplete()
```

Called when the [WebSocket](#websocket) handshake completes. Emits the connect event to downstream handlers.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `WebSocketFramer` | [`framer`](#framer)  |  |
| `http::Request &` | [`_request`](#_request-3)  |  |
| `http::Response &` | [`_response`](#_response-3)  |  |
| `ws::CloseState` | [`_closeState`](#_closestate)  |  |

---

{#framer}

#### framer

```cpp
WebSocketFramer framer
```

---

{#_request-3}

#### _request

```cpp
http::Request & _request
```

---

{#_response-3}

#### _response

```cpp
http::Response & _response
```

---

{#_closestate}

#### _closeState

```cpp
ws::CloseState _closeState {}
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`sendControlFrame`](#sendcontrolframe)  |  |
| `void` | [`resetFrameState`](#resetframestate)  |  |

---

{#sendcontrolframe}

#### sendControlFrame

```cpp
bool sendControlFrame(ws::Opcode opcode, const char * payload, size_t payloadLen, const net::Address & peerAddr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `opcode` | `[ws::Opcode](#opcode)` |  |
| `payload` | `const char *` |  |
| `payloadLen` | `size_t` |  |
| `peerAddr` | `const [net::Address](net.md#address) &` |  |

---

{#resetframestate}

#### resetFrameState

```cpp
void resetFrameState()
```

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`onSocketConnect`](#onsocketconnect-4) `virtual` | Called by the socket on connect; initiates the client handshake. |
| `bool` | [`onSocketRecv`](#onsocketrecv-6) `virtual` | Called by the socket on each received buffer; handles handshake or frame parsing depending on state. |
| `bool` | [`onSocketClose`](#onsocketclose-4) `virtual` | Called by the socket on close; resets framer state. |

---

{#onsocketconnect-4}

#### onSocketConnect

`virtual`

```cpp
virtual bool onSocketConnect(net::Socket & socket)
```

Called by the socket on connect; initiates the client handshake.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |

---

{#onsocketrecv-6}

#### onSocketRecv

`virtual`

```cpp
virtual bool onSocketRecv(net::Socket & socket, const MutableBuffer & buffer, const net::Address & peerAddress)
```

Called by the socket on each received buffer; handles handshake or frame parsing depending on state.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |
| `buffer` | `const [MutableBuffer](base.md#mutablebuffer) &` |  |
| `peerAddress` | `const [net::Address](net.md#address) &` |  |

---

{#onsocketclose-4}

#### onSocketClose

`virtual`

```cpp
virtual bool onSocketClose(net::Socket & socket)
```

Called by the socket on close; resets framer state.

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `[net::Socket](net.md#socket-1) &` |  |

{#websocketexception}

## WebSocketException

```cpp
#include <icy/http/websocket.h>
```

> **Inherits:** `runtime_error`

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WebSocketException`](#websocketexception-1) `inline` |  |
| `ErrorCode` | [`code`](#code) `const` `inline` |  |
| `bool` | [`hasCloseStatus`](#hasclosestatus) `const` `inline` |  |
| `uint16_t` | [`closeStatus`](#closestatus) `const` `inline` |  |

---

{#websocketexception-1}

#### WebSocketException

`inline`

```cpp
inline WebSocketException(ErrorCode code, std::string message, uint16_t closeStatus)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `code` | `[ErrorCode](#errorcode)` |  |
| `message` | `std::string` |  |
| `closeStatus` | `uint16_t` |  |

---

{#code}

#### code

`const` `inline`

```cpp
inline ErrorCode code() const
```

---

{#hasclosestatus}

#### hasCloseStatus

`const` `inline`

```cpp
inline bool hasCloseStatus() const
```

---

{#closestatus}

#### closeStatus

`const` `inline`

```cpp
inline uint16_t closeStatus() const
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ErrorCode` | [`_code`](#_code)  |  |
| `uint16_t` | [`_closeStatus`](#_closestatus)  |  |

---

{#_code}

#### _code

```cpp
ErrorCode _code
```

---

{#_closestatus}

#### _closeStatus

```cpp
uint16_t _closeStatus
```

{#websocketframer}

## WebSocketFramer

```cpp
#include <icy/http/websocket.h>
```

This class implements a [WebSocket](#websocket) parser according to the [WebSocket](#websocket) protocol described in RFC 6455.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`WebSocketFramer`](#websocketframer-1)  | Creates a [WebSocketFramer](#websocketframer) operating in the given endpoint mode. Client-side framers mask outgoing payloads; server-side framers do not.  |
| `size_t` | [`writeFrame`](#writeframe) `virtual` | Encodes `data` into a [WebSocket](#websocket) frame and writes it to `frame`.  |
| `uint64_t` | [`readFrame`](#readframe) `virtual` | Decodes a single [WebSocket](#websocket) frame from `frame`. |
| `bool` | [`handshakeComplete`](#handshakecomplete) `const` | Returns true if the [WebSocket](#websocket) handshake has completed successfully. |
| `void` | [`acceptServerRequest`](#acceptserverrequest)  | [Server](#server) side. |
| `void` | [`createClientHandshakeRequest`](#createclienthandshakerequest)  | [Client](#client) side. |
| `bool` | [`checkClientHandshakeResponse`](#checkclienthandshakeresponse)  | Validates the server's 101 Switching Protocols response.  |
| `void` | [`completeClientHandshake`](#completeclienthandshake)  | Completes the client-side handshake by verifying [Connection](#connection-1), Upgrade and Sec-WebSocket-Accept headers. Advances internal state to "complete".  |

---

{#websocketframer-1}

#### WebSocketFramer

```cpp
WebSocketFramer(ws::Mode mode)
```

Creates a [WebSocketFramer](#websocketframer) operating in the given endpoint mode. Client-side framers mask outgoing payloads; server-side framers do not. 
#### Parameters
* `mode` ServerSide or ClientSide.

| Parameter | Type | Description |
|-----------|------|-------------|
| `mode` | `[ws::Mode](#mode-2)` |  |

---

{#writeframe}

#### writeFrame

`virtual`

```cpp
virtual size_t writeFrame(const char * data, size_t len, int flags, BitWriter & frame)
```

Encodes `data` into a [WebSocket](#websocket) frame and writes it to `frame`. 
#### Parameters
* `data` Pointer to the payload data. 

* `len` Payload length in bytes. 

* `flags` Frame flags: [ws::SendFlags::Text](#namespaceicy_1_1http_1_1ws_1a366b9e6ee51be48df27ea06f8d20e7a2afdf2a2c6b74320a613fe71490a96e2ae), [ws::SendFlags::Binary](#namespaceicy_1_1http_1_1ws_1a366b9e6ee51be48df27ea06f8d20e7a2a0ca55f2c2a2e576e0aa1c08a4e845ed7), or a control frame opcode combined with [FrameFlags::Fin](#namespaceicy_1_1http_1_1ws_1a4317f7f44088d3ab553950bab6b16ab8a0475def81fbd4e70bf7cd07ee307793a). 

* `frame` [BitWriter](base.md#bitwriter) to write the encoded frame into. 

#### Returns
Total number of bytes written to the frame buffer (header + payload).

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `int` |  |
| `frame` | `[BitWriter](base.md#bitwriter) &` |  |

---

{#readframe}

#### readFrame

`virtual`

```cpp
virtual uint64_t readFrame(BitReader & frame, char *& payload)
```

Decodes a single [WebSocket](#websocket) frame from `frame`.

The payload is unmasked in-place in the source buffer; no copy is made. `payload` is set to point at the start of the payload within `frame`'s buffer.

#### Parameters
* `frame` [BitReader](base.md#bitreader) positioned at the start of a frame. 

* `payload` Set to point at the start of the decoded payload. Not null-terminated. 

#### Returns
Payload length in bytes. 

#### Exceptions
* `std::runtime_error` on protocol violations or if the buffer is too small.

| Parameter | Type | Description |
|-----------|------|-------------|
| `frame` | `[BitReader](base.md#bitreader) &` |  |
| `payload` | `char *&` |  |

---

{#handshakecomplete}

#### handshakeComplete

`const`

```cpp
bool handshakeComplete() const
```

Returns true if the [WebSocket](#websocket) handshake has completed successfully.

---

{#acceptserverrequest}

#### acceptServerRequest

```cpp
void acceptServerRequest(http::Request & request, http::Response & response)
```

[Server](#server) side.

Validates the client upgrade request and writes a 101 Switching Protocols response. Sets the internal state to mark the handshake as complete. 
#### Parameters
* `request` Incoming HTTP upgrade request. 

* `response` HTTP response to populate with the handshake reply. 

#### Exceptions
* `std::runtime_error` if the request is not a valid [WebSocket](#websocket) upgrade.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |
| `response` | `[http::Response](#response-1) &` |  |

---

{#createclienthandshakerequest}

#### createClientHandshakeRequest

```cpp
void createClientHandshakeRequest(http::Request & request)
```

[Client](#client) side.

Populates `request` with the [WebSocket](#websocket) upgrade headers ([Connection](#connection-1), Upgrade, Sec-WebSocket-Key, Sec-WebSocket-Version) to initiate the handshake. 
#### Parameters
* `request` HTTP request to add upgrade headers to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `[http::Request](#request-4) &` |  |

---

{#checkclienthandshakeresponse}

#### checkClientHandshakeResponse

```cpp
bool checkClientHandshakeResponse(http::Response & response)
```

Validates the server's 101 Switching Protocols response. 
#### Parameters
* `response` The HTTP response received from the server. 

#### Returns
true if the handshake succeeded and data can flow. 

#### Exceptions
* `std::runtime_error` if the server rejected or mishandled the upgrade.

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `[http::Response](#response-1) &` |  |

---

{#completeclienthandshake}

#### completeClientHandshake

```cpp
void completeClientHandshake(http::Response & response)
```

Completes the client-side handshake by verifying [Connection](#connection-1), Upgrade and Sec-WebSocket-Accept headers. Advances internal state to "complete". 
#### Parameters
* `response` The 101 Switching Protocols response from the server. 

#### Exceptions
* `std::runtime_error` if any required header is missing or incorrect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `[http::Response](#response-1) &` |  |

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`frameFlags`](#frameflags-1) `const` | Returns the frame flags of the most recently received frame. Set by [readFrame()](#readframe) |
| `bool` | [`mustMaskPayload`](#mustmaskpayload) `const` | Returns true if the payload must be masched. Used by [writeFrame()](#writeframe) |
| `ws::Mode` | [`mode`](#mode-3) `const` |  |

---

{#frameflags-1}

#### frameFlags

`const`

```cpp
int frameFlags() const
```

Returns the frame flags of the most recently received frame. Set by [readFrame()](#readframe)

---

{#mustmaskpayload}

#### mustMaskPayload

`const`

```cpp
bool mustMaskPayload() const
```

Returns true if the payload must be masched. Used by [writeFrame()](#writeframe)

---

{#mode-3}

#### mode

`const`

```cpp
ws::Mode mode() const
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ws::Mode` | [`_mode`](#_mode-3)  |  |
| `int` | [`_frameFlags`](#_frameflags)  |  |
| `int` | [`_headerState`](#_headerstate)  |  |
| `bool` | [`_maskPayload`](#_maskpayload)  |  |
| `Random` | [`_rnd`](#_rnd)  |  |
| `std::string` | [`_key`](#_key)  |  |
| `bool` | [`_fragmented`](#_fragmented)  | Currently receiving a fragmented message. |
| `int` | [`_fragmentOpcode`](#_fragmentopcode)  | Opcode of the first frame in the fragment sequence. |
| `Buffer` | [`_fragmentBuffer`](#_fragmentbuffer)  | Accumulated payload from continuation frames. |
| `Buffer` | [`_incompleteFrame`](#_incompleteframe)  | Buffer for incomplete frame data across TCP segments. |

---

{#_mode-3}

#### _mode

```cpp
ws::Mode _mode
```

---

{#_frameflags}

#### _frameFlags

```cpp
int _frameFlags
```

---

{#_headerstate}

#### _headerState

```cpp
int _headerState
```

---

{#_maskpayload}

#### _maskPayload

```cpp
bool _maskPayload
```

---

{#_rnd}

#### _rnd

```cpp
Random _rnd
```

---

{#_key}

#### _key

```cpp
std::string _key
```

---

{#_fragmented}

#### _fragmented

```cpp
bool _fragmented {false}
```

Currently receiving a fragmented message.

---

{#_fragmentopcode}

#### _fragmentOpcode

```cpp
int _fragmentOpcode {0}
```

Opcode of the first frame in the fragment sequence.

---

{#_fragmentbuffer}

#### _fragmentBuffer

```cpp
Buffer _fragmentBuffer
```

Accumulated payload from continuation frames.

---

{#_incompleteframe}

#### _incompleteFrame

```cpp
Buffer _incompleteFrame
```

Buffer for incomplete frame data across TCP segments.

