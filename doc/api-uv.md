# uv 

The `uv` module contains C++ wrappers for `libuv`.

### Classes

| Name | Description |
|------|-------------|
| [`Handle`](#classscy_1_1uv_1_1Handle) | Wrapper class for managing `uv_handle_t` variants. |
| [`Context`](#structscy_1_1uv_1_1Context) | Shared `libuv` handle context. |
| [`BasicEvent`](#structscy_1_1uv_1_1BasicEvent) | Default request callback event. |
| [`Request`](#structscy_1_1uv_1_1Request) | Wrapper class for managing `uv_req_t` variants. |
| [`ConnectReq`](#structscy_1_1uv_1_1ConnectReq) | [Stream](./doc/api-base.md#classscy_1_1Stream) connection request for sockets and pipes. |
| [`GetAddrInfoEvent`](#structscy_1_1uv_1_1GetAddrInfoEvent) | Get address info request callback event. |
| [`GetAddrInfoReq`](#structscy_1_1uv_1_1GetAddrInfoReq) | DNS resolver request to get the IP address of a hostname. |

## Handle 

> **Subclasses:** `scy::Stream< uv_pipe_t >`, `scy::Stream< uv_tcp_t >`, `scy::Stream< T >`
> **Defined in:** `handle.h`

Wrapper class for managing `uv_handle_t` variants.

This class manages the handle during it's lifecycle and safely handles the asynchronous destruction mechanism.

### Members

| Name | Description |
|------|-------------|
| [`Handle`](#group__uv_1gaf854eceb8518bda916774bc3a7564f2e) |  |
| [`~Handle`](#group__uv_1ga6e83df44c5a7312440ef4c553249d1e3) |  |
| [`init`](#group__uv_1gad883331655d4ca532a9364525f92ffec) | Initialize the handle. |
| [`invoke`](#group__uv_1gad6d9e20a3b932067608a21d513ee1dc2) | Invoke a method on the handle. |
| [`invokeOrThrow`](#group__uv_1gab361483fb3d274d6cd9b57def3201a6a) | Invoke a method on the handle. |
| [`close`](#group__uv_1ga30f5777b4b06de06c7867d5aa0c5fba2) | Close and destroy the handle. |
| [`ref`](#group__uv_1ga66960719f77ad6e4337135ba57aaacc6) | Reference main loop again, once unref'd. |
| [`unref`](#group__uv_1gad08501641ba20a3b51590fdabf708b53) | Unreference the main loop after initialized. |
| [`initialized`](#group__uv_1ga9f00c0db0e29656e77591f93cb78abf8) | Return true if the handle has been initialized. |
| [`active`](#group__uv_1gab0f0149cf3366468c1a4a8939670d70e) | Return true when the handle is active. |
| [`closing`](#group__uv_1ga1702f25d65a416ee7c63b03c594c3efd) | Return true if the handle is closing. |
| [`closed`](#group__uv_1gaaf63956b52958374cf9605351de1613c) | Return true is the handle has been closed. |
| [`error`](#group__uv_1ga7a417c609609e95f6640616c2feca656) | Return the error context if any. |
| [`setError`](#group__uv_1ga28954ab3e3c9cab2d406ee382fd8c9db) | Set the error and triggers callbacks. |
| [`setUVError`](#group__uv_1ga0b13beb047d3b4f1fa1095014305c065) | Set the error and trigger relevant callbacks. This method can be called inside `libuv` callbacks. |
| [`setAndThrowError`](#group__uv_1gae8ed57be72aa225686f67214ca21222c) | Set the error and throw an exception. Should never be called inside `libuv` callbacks. |
| [`throwLastError`](#group__uv_1gaf017bab932c1738f9c0bdf3364f9c5d3) | Throw an exception if the handle is in error state. The error message prefix will be updated if provided. |
| [`loop`](#group__uv_1gaa39643311153b62440c46d9f98d06093) | Return a cast pointer to the managed `libuv` handle. |
| [`self`](#group__uv_1gade08aa52cd372aea4ad24d2f54458886) | Return a pointer to the current or derived instance. |
| [`reset`](#group__uv_1ga2299bfef477dd7edd7e9f4595b39efb5) | Reset the internal handle pointer and container state. |
| [`get`](#group__uv_1gacbb9d1703eabdd2962ae99470a992d67) | Return a typecasted pointer to the managed handle. |
| [`tid`](#group__uv_1ga2528dc53f1af808d3fa0dc05563deb4c) | Return the parent thread ID. |
| [`context`](#group__uv_1gac4b3ca8752769915f33c76924d3bbbd3) | Return the shared handle pointer context. |
| [`assertThread`](#group__uv_1gaa46a6be75e3a975a003a938631eb6ae2) | Assert the call is from the parent event loop thread. |
| [`onError`](#group__uv_1gaffc115aedb14ff27a16613570738753c) | [Error](./doc/api-base.md#structscy_1_1Error) callback. Override to handle errors. The error may be a UV error, or a custom error. |
| [`onClose`](#group__uv_1gad6564419640315d2933cb9d08f948c53) | Close callback. Override to handle closure. |
| [`Handle`](#group__uv_1ga0b418ca2765a4f2b6295ab4226fe0e0e) | NonCopyable and NonMovable. |
| [`operator=`](#group__uv_1gad883c6796c902047357383f64ee0f164) |  |
| [`_loop`](#group__uv_1ga158c033f5c162bfd2d3c7e7c4d1d63ba) |  |
| [`_context`](#group__uv_1ga052d3d8c37fba9cbb926c019ae0d5c8a) |  |
| [`_tid`](#group__uv_1ga4f9cb8a011b402320694bc2cf20970b6) |  |
| [`_error`](#group__uv_1ga6c3b4a8509c1e7434ad63f7840faa085) |  |

---

#### Handle 

```cpp
inline Handle(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~Handle 

```cpp
virtual inline ~Handle()
```

---

#### init 

```cpp
template<typename F, typename... Args> inline bool init(F && f, Args &&... args)
```

Initialize the handle.

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `F &&` |  |
| `args` | `Args &&...` |  |

---

#### invoke 

```cpp
template<typename F, typename... Args> inline bool invoke(F && f, Args &&... args)
```

Invoke a method on the handle.

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `F &&` |  |
| `args` | `Args &&...` |  |

---

#### invokeOrThrow 

```cpp
template<typename F, typename... Args> inline void invokeOrThrow(const std::string & message, F && f, Args &&... args)
```

Invoke a method on the handle.

An exception will be thrown if the invoked method returns an error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `const std::string &` |  |
| `f` | `F &&` |  |
| `args` | `Args &&...` |  |

---

#### close 

```cpp
virtual inline void close()
```

Close and destroy the handle.

---

#### ref 

```cpp
inline void ref()
```

Reference main loop again, once unref'd.

---

#### unref 

```cpp
inline void unref()
```

Unreference the main loop after initialized.

---

#### initialized 

```cpp
inline bool initialized() const
```

Return true if the handle has been initialized.

---

#### active 

```cpp
virtual inline bool active() const
```

Return true when the handle is active.

---

#### closing 

```cpp
virtual inline bool closing() const
```

Return true if the handle is closing.

---

#### closed 

```cpp
virtual inline bool closed() const
```

Return true is the handle has been closed.

---

#### error 

```cpp
inline const [scy::Error](./doc/api-base.md#structscy_1_1Error) & error() const
```

Return the error context if any.

---

#### setError 

```cpp
virtual inline void setError(const Error & error)
```

Set the error and triggers callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const Error &` |  |

---

#### setUVError 

```cpp
inline void setUVError(int err, std::string prefix)
```

Set the error and trigger relevant callbacks. This method can be called inside `libuv` callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `int` |  |
| `prefix` | `std::string` |  |

---

#### setAndThrowError 

```cpp
inline void setAndThrowError(int err, std::string prefix)
```

Set the error and throw an exception. Should never be called inside `libuv` callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `err` | `int` |  |
| `prefix` | `std::string` |  |

---

#### throwLastError 

```cpp
inline void throwLastError(std::string prefix)
```

Throw an exception if the handle is in error state. The error message prefix will be updated if provided.

| Parameter | Type | Description |
|-----------|------|-------------|
| `prefix` | `std::string` |  |

---

#### loop 

```cpp
inline uv::Loop * loop() const
```

Return a cast pointer to the managed `libuv` handle.

---

#### self 

```cpp
virtual inline void * self()
```

Return a pointer to the current or derived instance.

---

#### reset 

```cpp
inline void reset()
```

Reset the internal handle pointer and container state.

---

#### get 

```cpp
template<typename [Handle](#classscy_1_1uv_1_1Handle)> inline Handle * get() const
```

Return a typecasted pointer to the managed handle.

---

#### tid 

```cpp
inline std::thread::id tid() const
```

Return the parent thread ID.

---

#### context 

```cpp
inline std::shared_ptr< Context< T > > context() const
```

Return the shared handle pointer context.

---

#### assertThread 

```cpp
inline void assertThread() const
```

Assert the call is from the parent event loop thread.

---

#### onError 

```cpp
virtual inline void onError(const Error & error)
```

[Error](./doc/api-base.md#structscy_1_1Error) callback. Override to handle errors. The error may be a UV error, or a custom error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `error` | `const Error &` |  |

---

#### onClose 

```cpp
virtual inline void onClose()
```

Close callback. Override to handle closure.

---

#### Handle 

```cpp
Handle(const Handle &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
Handle & operator=(const Handle &) = delete
```

---

#### _loop 

```cpp
uv::Loop * _loop
```

---

#### _context 

```cpp
std::shared_ptr< Context< T > > _context
```

---

#### _tid 

```cpp
std::thread::id _tid = std::this_thread::get_id()
```

---

#### _error 

```cpp
Error _error
```

## Context 

> **Defined in:** `handle.h`

Shared `libuv` handle context.

### Members

| Name | Description |
|------|-------------|
| [`Context`](#group__uv_1ga4e2cc68f5af9da3664ed1d678c7ed152) |  |
| [`~Context`](#group__uv_1ga8c43209d5ec23f42726c090d8d3875f6) |  |
| [`handle`](#group__uv_1gadb09e2ca5aa8003d12d602ee9e92e5e2) |  |
| [`ptr`](#group__uv_1ga5b56a1767069f00d52535b5a12253011) |  |
| [`initialized`](#group__uv_1ga7bd98d15327713a202724137f0429196) |  |
| [`deleted`](#group__uv_1ga33997488e52b3073a4a0ffbcf97d7900) |  |

---

#### Context 

```cpp
inline Context(Handle< T > * h)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `h` | `Handle< T > *` |  |

---

#### ~Context 

```cpp
inline ~Context()
```

---

#### handle 

```cpp
Handle< T > * handle = nullptr
```

---

#### ptr 

```cpp
T * ptr = new T
```

---

#### initialized 

```cpp
bool initialized = false
```

---

#### deleted 

```cpp
bool deleted = false
```

## BasicEvent 

> **Defined in:** `request.h`

Default request callback event.

### Members

| Name | Description |
|------|-------------|
| [`status`](#group__uv_1gaaf2af795b60beb6649fe7f26f15d374a) |  |

---

#### status 

```cpp
int status
```

## Request 

> **Defined in:** `request.h`

Wrapper class for managing `uv_req_t` variants.

This class povides safe access to the parent handle incase the handle gets destroyed before the request callback returns, and should be used whenever the handle pointer is accessed via the callback.

### Members

| Name | Description |
|------|-------------|
| [`Request`](#group__uv_1gad027fbdf614b379deadaf4681c8f3da2) |  |
| [`invoke`](#group__uv_1ga182893e356c9cbfd3702b52afef6850b) |  |
| [`defaultCallback`](#group__uv_1ga9b5acaa79eb6c49487c376ef6dd897e4) |  |
| [`req`](#group__uv_1ga1c2d4993bdd5fa576901b59c677811b3) |  |
| [`callback`](#group__uv_1ga93497dface68fec7c3073c4ae1fbb621) |  |
| [`invoke`](#structscy_1_1uv_1_1Request_1ga182893e356c9cbfd3702b52afef6850b) |  |

---

#### Request 

```cpp
inline Request()
```

---

#### invoke 

```cpp
template<typename F, typename... Args> inline auto invoke(F && f, Args &&... args)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `F &&` |  |
| `args` | `Args &&...` |  |

---

#### defaultCallback 

```cpp
static inline void defaultCallback(T * req, int status)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `req` | `T *` |  |
| `status` | `int` |  |

---

#### req 

```cpp
T req
```

---

#### callback 

```cpp
std::function< void(const E &)> callback
```

---

#### invoke 

```cpp
template<typename F, typename... Args> inline auto invoke(F && f, Args &&... args)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `F &&` |  |
| `args` | `Args &&...` |  |

## ConnectReq 

> **Extends:** `scy::uv::Request< uv_connect_t >`
> **Defined in:** `request.h`

[Stream](./doc/api-base.md#classscy_1_1Stream) connection request for sockets and pipes.

### Members

| Name | Description |
|------|-------------|
| [`ConnectReq`](#group__uv_1gaf3c68ef1149aaf2fbc902776afcaa563) |  |
| [`connect`](#group__uv_1ga02fc3c248e3346f95b201bb39455aa21) |  |
| [`connect`](#group__uv_1gad07534f80d5b321995ad74f3b5a5abbf) |  |

---

#### ConnectReq 

```cpp
inline ConnectReq()
```

---

#### connect 

```cpp
inline auto connect(uv_tcp_t * handle, const struct sockaddr * addr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_tcp_t *` |  |
| `addr` | `const struct sockaddr *` |  |

---

#### connect 

```cpp
inline auto connect(uv_pipe_t * handle, const char * name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_pipe_t *` |  |
| `name` | `const char *` |  |

## GetAddrInfoEvent 

> **Defined in:** `request.h`

Get address info request callback event.

### Members

| Name | Description |
|------|-------------|
| [`status`](#group__uv_1gaa3c385f2d6fad57b50d74db35770ea99) |  |
| [`addr`](#group__uv_1ga4f414acbeba1d1b61d3c829b5ca1fac5) |  |

---

#### status 

```cpp
int status
```

---

#### addr 

```cpp
struct addrinfo * addr
```

## GetAddrInfoReq 

> **Extends:** `scy::uv::Request< uv_getaddrinfo_t, GetAddrInfoEvent >`
> **Defined in:** `request.h`

DNS resolver request to get the IP address of a hostname.

### Members

| Name | Description |
|------|-------------|
| [`GetAddrInfoReq`](#group__uv_1ga317acd338a40f9d913ce58f10c9ad8d7) |  |
| [`resolve`](#group__uv_1ga28197714fd84c1bfbb96cb0ae2377b0a) |  |
| [`getAddrInfoCallback`](#group__uv_1gaf879113f50493b9cb303b72e1cdc7c3f) |  |

---

#### GetAddrInfoReq 

```cpp
inline GetAddrInfoReq()
```

---

#### resolve 

```cpp
inline auto resolve(const std::string & host, int port, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `host` | `const std::string &` |  |
| `port` | `int` |  |
| `loop` | `uv::Loop *` |  |

---

#### getAddrInfoCallback 

```cpp
static inline void getAddrInfoCallback(Request::Type * req, int status, struct addrinfo * res)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `req` | `Request::Type *` |  |
| `status` | `int` |  |
| `res` | `struct addrinfo *` |  |

