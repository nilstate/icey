{#uvmodule}

# uv

The `uv` module contains C++ wrappers for `libuv`.

### Classes

| Name | Description |
|------|-------------|
| [`Handle`](#handle-2) | Wrapper class for managing `uv_handle_t` variants. |
| [`ScopedLoop`](#scopedloop) | RAII wrapper for a libuv event loop. Automatically closes and deletes the loop on destruction. |
| [`HandleStorage`](#handlestorage-1) | Extra storage placed around a raw `libuv` handle for close-time cleanup hooks. |
| [`Context`](#context-1) | Shared `libuv` handle context. |
| [`BasicEvent`](#basicevent) | Default request callback event carrying a libuv status code. |
| [`Request`](#request) | Wrapper class for managing `uv_req_t` variants. |
| [`ConnectReq`](#connectreq) | Asynchronous connection request for TCP sockets and named pipes. |
| [`GetAddrInfoEvent`](#getaddrinfoevent) | Callback event delivered when a `[GetAddrInfoReq](#getaddrinforeq)` resolves. |
| [`GetAddrInfoReq`](#getaddrinforeq) | DNS resolver request to get the IP address of a hostname. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `uv_loop_t` | [`Loop`](#loop)  | Alias for a `libuv` event loop instance. |

---

{#loop}

#### Loop

```cpp
uv_loop_t Loop()
```

Alias for a `libuv` event loop instance.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `Loop *` | [`defaultLoop`](#defaultloop) `inline` | Returns the process-wide default libuv event loop. |
| `void` | [`runLoop`](#runloop) `inline` | Runs the given event loop using the specified run mode. Blocks until the loop exits (when using `UV_RUN_DEFAULT`). |
| `void` | [`stopLoop`](#stoploop) `inline` | Stops the given event loop, causing `uv_run` to return after the current iteration. |
| `Loop *` | [`createLoop`](#createloop) `inline` | Allocates and initializes a new libuv event loop. The caller is responsible for closing and deleting the returned loop. |
| `bool` | [`closeLoop`](#closeloop) `inline` | Closes the given event loop, releasing internal resources. All handles must be closed before calling this. |
| `HandleStorage< T > *` | [`handleStorage`](#handlestorage) `inline` | Returns the extended storage wrapper that owns `handle`. |
| `void` | [`setHandleCloseCleanup`](#sethandleclosecleanup) `inline` | Registers a cleanup callback that runs when `handle` finally closes. |
| `void` | [`clearHandleCloseCleanup`](#clearhandleclosecleanup) `inline` | Clears any pending close-time cleanup callback registered on `handle`. |
| `auto` | [`withHandleContext`](#withhandlecontext) `inline` | Wraps `callback` so it only runs while the owning handle is still alive. Captures the intrusive `[Context](#context-1)` token, rehydrates the typed owner on entry, and suppresses invocation if the handle has already been deleted. |
| `T &` | [`createRequest`](#createrequest) `inline` | Allocate a heap-owned `[Request](#request)` of type `T` and attach `callback` to it. |
| `T &` | [`createRetainedRequest`](#createretainedrequest) `inline` | Allocate a heap-owned `[Request](#request)` of type `T` whose callback retains additional state until completion. |

---

{#defaultloop}

#### defaultLoop

`inline`

```cpp
inline Loop * defaultLoop()
```

Returns the process-wide default libuv event loop. 
#### Returns
Pointer to the default `uv_loop_t`.

---

{#runloop}

#### runLoop

`inline`

```cpp
inline void runLoop(Loop * loop, uv_run_mode mode) = default
```

Runs the given event loop using the specified run mode. Blocks until the loop exits (when using `UV_RUN_DEFAULT`). 
#### Parameters
* `loop` Event loop to run. Defaults to the default loop. 

* `mode` libuv run mode: `UV_RUN_DEFAULT`, `UV_RUN_ONCE`, or `UV_RUN_NOWAIT`.

---

{#stoploop}

#### stopLoop

`inline`

```cpp
inline void stopLoop(Loop * loop) = default
```

Stops the given event loop, causing `uv_run` to return after the current iteration. 
#### Parameters
* `loop` Event loop to stop. Defaults to the default loop.

---

{#createloop}

#### createLoop

`inline`

```cpp
inline Loop * createLoop()
```

Allocates and initializes a new libuv event loop. The caller is responsible for closing and deleting the returned loop. 
#### Returns
Pointer to a newly initialized `uv_loop_t`.

---

{#closeloop}

#### closeLoop

`inline`

```cpp
inline bool closeLoop(Loop * loop)
```

Closes the given event loop, releasing internal resources. All handles must be closed before calling this. 
#### Parameters
* `loop` Event loop to close. 

#### Returns
True on success, false if the loop still has active handles.

---

{#handlestorage}

#### handleStorage

`inline`

```cpp
template<typename T> inline HandleStorage< T > * handleStorage(T * handle)
```

Returns the extended storage wrapper that owns `handle`. 
#### Parameters
* `handle` Raw `libuv` handle pointer previously allocated by `[Context](#context-1)<T>`.

---

{#sethandleclosecleanup}

#### setHandleCloseCleanup

`inline`

```cpp
template<typename T> inline void setHandleCloseCleanup(T * handle, void * data, void(*)(void *) cleanup)
```

Registers a cleanup callback that runs when `handle` finally closes. 
#### Parameters
* `handle` Raw `libuv` handle pointer. 

* `data` User data passed back to `cleanup`. 

* `cleanup` Function invoked exactly once when the handle storage is released.

---

{#clearhandleclosecleanup}

#### clearHandleCloseCleanup

`inline`

```cpp
template<typename T> inline void clearHandleCloseCleanup(T * handle)
```

Clears any pending close-time cleanup callback registered on `handle`. 
#### Parameters
* `handle` Raw `libuv` handle pointer.

---

{#withhandlecontext}

#### withHandleContext

`inline`

```cpp
template<typename Owner, typename Callback> inline auto withHandleContext(Owner & owner, Callback && callback)
```

Wraps `callback` so it only runs while the owning handle is still alive. Captures the intrusive `[Context](#context-1)` token, rehydrates the typed owner on entry, and suppresses invocation if the handle has already been deleted. 
#### Parameters
* `owner` Owning handle instance. 

* `callback` Callable that receives `Owner&` followed by the libuv callback args.

---

{#createrequest}

#### createRequest

`inline`

```cpp
template<typename T> inline T & createRequest(std::function< void(const typename T::Event &)> callback)
```

Allocate a heap-owned `[Request](#request)` of type `T` and attach `callback` to it.

The returned reference is valid until the request's `defaultCallback` fires and deletes the object.

#### Parameters
* `T` A specialization of `[Request](#request)`. 

#### Parameters
* `callback` Completion handler; receives a `T::Event` on completion. 

#### Returns
Reference to the newly allocated request.

---

{#createretainedrequest}

#### createRetainedRequest

`inline`

```cpp
template<typename T, typename Retained, typename Callback> inline T & createRetainedRequest(Retained && retained, Callback && callback)
```

Allocate a heap-owned `[Request](#request)` of type `T` whose callback retains additional state until completion.

This is the standard way to bind request completion to handle lifetime or other retained context without hand-rolling per-call capture logic.

#### Parameters
* `T` A specialization of `[Request](#request)`. 

* `Retained` Retained object type copied or moved into the callback. 

* `Callback` Callable invoked as `callback(retained, event)`. 

#### Parameters
* `retained` Extra state to keep alive until the request completes. 

* `callback` Completion handler receiving the retained state and event. 

#### Returns
Reference to the newly allocated request.

{#handle-2}

## Handle

```cpp
#include <icy/handle.h>
```

> **Subclassed by:** [`Stream< uv_pipe_t >`](base.md#stream), [`Stream< uv_tcp_t >`](base.md#stream), [`Stream< T >`](base.md#stream)

Wrapper class for managing `uv_handle_t` variants.

This class manages the handle during its lifecycle and safely handles the asynchronous destruction mechanism.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Handle`](#handle-3) `inline` | Construct the handle bound to the given event loop. |
| `bool` | [`init`](#init-2) `inline` | Initialize the underlying libuv handle by calling `f` with the loop, the raw handle pointer, and any additional `args`. |
| `bool` | [`invoke`](#invoke) `inline` | Invoke a libuv function `f` with `args` on the initialized handle. |
| `void` | [`invokeOrThrow`](#invokeorthrow) `inline` | Invoke a libuv function `f` with `args`, throwing on failure. |
| `void` | [`close`](#close-11) `virtual` `inline` | Close and destroy the handle. |
| `void` | [`ref`](#ref) `inline` | Re-reference the handle with the event loop after a previous `[unref()](#unref)`. |
| `void` | [`unref`](#unref) `inline` | Unreference the handle from the event loop. |
| `bool` | [`initialized`](#initialized-1) `const` `inline` | Return `true` if the handle has been successfully initialized via `[init()](#init-2)`. |
| `bool` | [`active`](#active) `virtual` `const` `inline` | Return `true` when the handle is active (libuv `uv_is_active`). |
| `bool` | [`closing`](#closing-1) `virtual` `const` `inline` | Return `true` if `uv_close` has been called and the handle is awaiting its close callback (libuv `uv_is_closing`). |
| `bool` | [`closed`](#closed) `virtual` `const` `inline` | Return `true` if the handle has been fully closed (context released). |
| `const icy::Error &` | [`error`](#error-4) `const` `inline` | Return the last error set on this handle, or a default-constructed `[Error](base.md#error)` if no error has occurred. |
| `void` | [`setError`](#seterror) `virtual` `inline` | Set the error state and invoke `[onError()](#onerror)`. |
| `void` | [`setUVError`](#setuverror) `inline` | Translate a libuv error code into an `[Error](base.md#error)` and call `[setError()](#seterror)`. |
| `void` | [`setAndThrowError`](#setandthrowerror) `inline` | Set the error state from a libuv error code and throw a `std::runtime_error`. |
| `void` | [`throwLastError`](#throwlasterror) `inline` | Throw a `std::runtime_error` if the handle currently holds an error. |
| `uv::Loop *` | [`loop`](#loop-2) `const` `inline` | Return the event loop this handle is bound to. |
| `void` | [`reset`](#reset-3) `inline` | Close the current handle (if open) and allocate a fresh `[Context](#context-1)`, leaving the handle ready to be re-initialized via `[init()](#init-2)`. |
| `Handle *` | [`get`](#get-2) `const` `inline` | Return the raw libuv handle pointer cast to `[Handle](#handle-2)`. |
| `std::thread::id` | [`tid`](#tid) `const` `inline` | Return the ID of the thread that constructed this handle. |
| `IntrusivePtr< Context< T > >` | [`context`](#context-3) `const` `inline` | Return the raw `[Context](#context-1)` that owns the libuv handle memory. |
| `void` | [`setCloseCleanup`](#setclosecleanup) `inline` |  |
| `void` | [`clearCloseCleanup`](#clearclosecleanup) `inline` |  |
| `void` | [`assertThread`](#assertthread) `const` `inline` | Throw `std::logic_error` if called from any thread other than the thread that constructed this handle. |

---

{#handle-3}

#### Handle

`inline`

```cpp
inline Handle(uv::Loop * loop)
```

Construct the handle bound to the given event loop.

#### Parameters
* `loop` Event loop to associate this handle with. Defaults to the process-wide default loop.

---

{#init-2}

#### init

`inline`

```cpp
template<typename F, typename... Args> inline bool init(F && f, Args &&... args)
```

Initialize the underlying libuv handle by calling `f` with the loop, the raw handle pointer, and any additional `args`.

Must be called exactly once before any other operations. Throws `std::logic_error` if the handle is already initialized or the context is invalid.

#### Parameters
* `f` libuv init function (e.g. `uv_tcp_init`). 

* `args` Additional arguments forwarded after the loop and handle pointer. 

#### Returns
`true` on success; `false` and sets the error state on failure.

---

{#invoke}

#### invoke

`inline`

```cpp
template<typename F, typename... Args> inline bool invoke(F && f, Args &&... args)
```

Invoke a libuv function `f` with `args` on the initialized handle.

Throws `std::logic_error` if the handle is not yet initialized. Sets the error state and returns `false` if `f` returns a libuv error code.

#### Parameters
* `f` libuv function to call. 

* `args` Arguments forwarded to `f`. 

#### Returns
`true` on success; `false` on libuv error.

---

{#invokeorthrow}

#### invokeOrThrow

`inline`

```cpp
template<typename F, typename... Args> inline void invokeOrThrow(const std::string & message, F && f, Args &&... args)
```

Invoke a libuv function `f` with `args`, throwing on failure.

Identical to `[invoke()](#invoke)` but throws a `std::runtime_error` with `message` prepended if `f` returns a libuv error code. Must not be called from inside a libuv callback.

#### Parameters
* `message` [Error](base.md#error) message prefix used in the thrown exception. 

* `f` libuv function to call. 

* `args` Arguments forwarded to `f`.

---

{#close-11}

#### close

`virtual` `inline`

```cpp
virtual inline void close()
```

Close and destroy the handle.

Releases the `[Context](#context-1)` (which schedules the async `uv_close`) and then fires `[onClose()](#onclose)`. Safe to call more than once; subsequent calls are no-ops.

---

{#ref}

#### ref

`inline`

```cpp
inline void ref()
```

Re-reference the handle with the event loop after a previous `[unref()](#unref)`.

When all handles are unref'd the loop exits automatically. This call reverses that. Has no effect if the handle is not initialized.

---

{#unref}

#### unref

`inline`

```cpp
inline void unref()
```

Unreference the handle from the event loop.

The loop will exit when all active handles are unref'd, even if this handle is still alive. Has no effect if the handle is not initialized.

---

{#initialized-1}

#### initialized

`const` `inline`

```cpp
inline bool initialized() const
```

Return `true` if the handle has been successfully initialized via `[init()](#init-2)`.

---

{#active}

#### active

`virtual` `const` `inline`

```cpp
virtual inline bool active() const
```

Return `true` when the handle is active (libuv `uv_is_active`).

"Active" has type-specific meaning: a timer is active while counting, a stream is active while connected, etc.

---

{#closing-1}

#### closing

`virtual` `const` `inline`

```cpp
virtual inline bool closing() const
```

Return `true` if `uv_close` has been called and the handle is awaiting its close callback (libuv `uv_is_closing`).

---

{#closed}

#### closed

`virtual` `const` `inline`

```cpp
virtual inline bool closed() const
```

Return `true` if the handle has been fully closed (context released).

---

{#error-4}

#### error

`const` `inline`

```cpp
inline const icy::Error & error() const
```

Return the last error set on this handle, or a default-constructed `[Error](base.md#error)` if no error has occurred.

---

{#seterror}

#### setError

`virtual` `inline`

```cpp
virtual inline void setError(const Error & error)
```

Set the error state and invoke `[onError()](#onerror)`.

#### Parameters
* `error` [Error](base.md#error) value to store and propagate.

---

{#setuverror}

#### setUVError

`inline`

```cpp
inline void setUVError(int err, std::string prefix)
```

Translate a libuv error code into an `[Error](base.md#error)` and call `[setError()](#seterror)`.

Safe to call from inside libuv callbacks.

#### Parameters
* `err` libuv error code (negative integer). 

* `prefix` Human-readable prefix prepended to the formatted message.

---

{#setandthrowerror}

#### setAndThrowError

`inline`

```cpp
inline void setAndThrowError(int err, std::string prefix)
```

Set the error state from a libuv error code and throw a `std::runtime_error`.

Must not be called from inside libuv callbacks; use `[setUVError()](#setuverror)` there.

#### Parameters
* `err` libuv error code (negative integer). 

* `prefix` Human-readable prefix prepended to the thrown message.

---

{#throwlasterror}

#### throwLastError

`inline`

```cpp
inline void throwLastError(std::string prefix)
```

Throw a `std::runtime_error` if the handle currently holds an error.

The stored error's message is re-formatted with `prefix` before throwing. No-op if the handle is not in an error state.

#### Parameters
* `prefix` Human-readable prefix used when re-formatting the message.

---

{#loop-2}

#### loop

`const` `inline`

```cpp
inline uv::Loop * loop() const
```

Return the event loop this handle is bound to.

Asserts that the caller is on the owning thread.

#### Returns
Pointer to the associated `[uv::Loop](#loop)`.

---

{#reset-3}

#### reset

`inline`

```cpp
inline void reset()
```

Close the current handle (if open) and allocate a fresh `[Context](#context-1)`, leaving the handle ready to be re-initialized via `[init()](#init-2)`.

---

{#get-2}

#### get

`const` `inline`

```cpp
template<typename Handle> inline Handle * get() const
```

Return the raw libuv handle pointer cast to `[Handle](#handle-2)`.

Returns `nullptr` if the context has been released (handle closed). Asserts that the caller is on the owning thread.

#### Parameters
* `[Handle](#handle-2)` Target type; defaults to the native handle type `T`. 

#### Returns
Pointer to the underlying libuv handle, or `nullptr`.

---

{#tid}

#### tid

`const` `inline`

```cpp
inline std::thread::id tid() const
```

Return the ID of the thread that constructed this handle.

All handle operations must be performed on this thread.

#### Returns
`std::thread::id` of the owning thread.

---

{#context-3}

#### context

`const` `inline`

```cpp
inline IntrusivePtr< Context< T > > context() const
```

Return the raw `[Context](#context-1)` that owns the libuv handle memory.

Primarily for use by subclasses and libuv callbacks that need to access the underlying libuv handle memory.

#### Returns
A retained reference to the `[Context](#context-1)`, or an empty reference if closed.

---

{#setclosecleanup}

#### setCloseCleanup

`inline`

```cpp
template<typename U> inline void setCloseCleanup(U * data)
```

---

{#clearclosecleanup}

#### clearCloseCleanup

`inline`

```cpp
inline void clearCloseCleanup()
```

---

{#assertthread}

#### assertThread

`const` `inline`

```cpp
inline void assertThread() const
```

Throw `std::logic_error` if called from any thread other than the thread that constructed this handle.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uv::Loop *` | [`_loop`](#_loop)  |  |
| `IntrusivePtr< Context< T > >` | [`_context`](#_context)  |  |
| `std::thread::id` | [`_tid`](#_tid)  |  |
| `Error` | [`_error`](#_error-1)  |  |

---

{#_loop}

#### _loop

```cpp
uv::Loop * _loop
```

---

{#_context}

#### _context

```cpp
IntrusivePtr< Context< T > > _context
```

---

{#_tid}

#### _tid

```cpp
std::thread::id _tid = std::this_thread::get_id()
```

---

{#_error-1}

#### _error

```cpp
Error _error
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onError`](#onerror) `virtual` `inline` | Called by `[setError()](#seterror)` after the error state has been updated. |
| `void` | [`onClose`](#onclose) `virtual` `inline` | Called by `[close()](#close-11)` after the context has been released. |
|  | [`Handle`](#handle-4)  | NonCopyable and NonMovable. |
|  | [`Handle`](#handle-5)  | Deleted constructor. |

---

{#onerror}

#### onError

`virtual` `inline`

```cpp
virtual inline void onError(const Error & error)
```

Called by `[setError()](#seterror)` after the error state has been updated.

Override to react to errors. The default implementation is a no-op.

#### Parameters
* `error` The error that was set.

---

{#onclose}

#### onClose

`virtual` `inline`

```cpp
virtual inline void onClose()
```

Called by `[close()](#close-11)` after the context has been released.

Override to perform cleanup on handle closure. The default implementation is a no-op.

---

{#handle-4}

#### Handle

```cpp
Handle(const Handle &) = delete
```

NonCopyable and NonMovable.

---

{#handle-5}

#### Handle

```cpp
Handle(Handle &&) = delete
```

Deleted constructor.

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-7)  | Define the native handle type. |

---

{#type-7}

#### Type

```cpp
T Type()
```

Define the native handle type.

{#scopedloop}

## ScopedLoop

```cpp
#include <icy/loop.h>
```

RAII wrapper for a libuv event loop. Automatically closes and deletes the loop on destruction.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Loop *` | [`loop`](#loop-1)  |  |

---

{#loop-1}

#### loop

```cpp
Loop * loop
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ScopedLoop`](#scopedloop-1) `inline` |  |
|  | [`operator Loop *`](#operatorloop) `const` `inline` | Implicit conversion to `Loop*` for use with libuv APIs. |
| `Loop *` | [`get`](#get-1) `const` `inline` | Returns the raw event loop pointer. |
|  | [`ScopedLoop`](#scopedloop-2)  | Deleted constructor. |
|  | [`ScopedLoop`](#scopedloop-3)  | Deleted constructor. |

---

{#scopedloop-1}

#### ScopedLoop

`inline`

```cpp
inline ScopedLoop()
```

---

{#operatorloop}

#### operator Loop *

`const` `inline`

```cpp
inline operator Loop *() const
```

Implicit conversion to `Loop*` for use with libuv APIs.

---

{#get-1}

#### get

`const` `inline`

```cpp
inline Loop * get() const
```

Returns the raw event loop pointer. 
#### Returns
Pointer to the underlying `uv_loop_t`.

---

{#scopedloop-2}

#### ScopedLoop

```cpp
ScopedLoop(const ScopedLoop &) = delete
```

Deleted constructor.

---

{#scopedloop-3}

#### ScopedLoop

```cpp
ScopedLoop(ScopedLoop &&) = delete
```

Deleted constructor.

{#handlestorage-1}

## HandleStorage

```cpp
#include <icy/handle.h>
```

Extra storage placed around a raw `libuv` handle for close-time cleanup hooks.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `T` | [`handle`](#handle)  | Embedded raw `libuv` handle object. |
| `void *` | [`closeData`](#closedata)  | Opaque cleanup payload invoked on close. |
| `void(*` | [`closeCleanup`](#closecleanup)  | Cleanup function for `closeData`. |

---

{#handle}

#### handle

```cpp
T handle {}
```

Embedded raw `libuv` handle object.

---

{#closedata}

#### closeData

```cpp
void * closeData = nullptr
```

Opaque cleanup payload invoked on close.

---

{#closecleanup}

#### closeCleanup

```cpp
void(* closeCleanup = nullptr
```

Cleanup function for `closeData`.

{#context-1}

## Context

```cpp
#include <icy/handle.h>
```

> **Inherits:** [`RefCounted< Context< T > >`](base.md#refcounted)

Shared `libuv` handle context.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Handle< T > *` | [`handle`](#handle-1)  |  |
| `HandleStorage< T > *` | [`storage`](#storage)  |  |
| `T *` | [`ptr`](#ptr-3)  |  |
| `bool` | [`initialized`](#initialized)  |  |
| `bool` | [`deleted`](#deleted)  |  |

---

{#handle-1}

#### handle

```cpp
Handle< T > * handle = nullptr
```

---

{#storage}

#### storage

```cpp
HandleStorage< T > * storage = new <T>
```

---

{#ptr-3}

#### ptr

```cpp
T * ptr = &->
```

---

{#initialized}

#### initialized

```cpp
bool initialized = false
```

---

{#deleted}

#### deleted

```cpp
bool deleted = false
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Context`](#context-2) `inline` |  |
| `Owner *` | [`owner`](#owner) `const` `inline` |  |

---

{#context-2}

#### Context

`inline`

```cpp
inline Context(Handle< T > * h)
```

---

{#owner}

#### owner

`const` `inline`

```cpp
template<typename Owner> inline Owner * owner() const
```

{#basicevent}

## BasicEvent

```cpp
#include <icy/request.h>
```

Default request callback event carrying a libuv status code.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`status`](#status)  | libuv result: 0 on success, negative on error. |

---

{#status}

#### status

```cpp
int status
```

libuv result: 0 on success, negative on error.

{#request}

## Request

```cpp
#include <icy/request.h>
```

Wrapper class for managing `uv_req_t` variants.

This class provides safe access to the parent handle in case the handle gets destroyed before the request callback returns, and should be used whenever the handle pointer is accessed via the callback.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `T` | [`req`](#req)  | The underlying libuv request object. |
| `std::function< void(const E &)>` | [`callback`](#callback)  | Called when the request completes. |

---

{#req}

#### req

```cpp
T req
```

The underlying libuv request object.

---

{#callback}

#### callback

```cpp
std::function< void(const E &)> callback
```

Called when the request completes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Request`](#request-1) `inline` | Construct the request and set `req.data` to `this` so callbacks can recover the wrapper pointer. |
| `auto` | [`invoke`](#invoke-1) `inline` | Call `f` with `args`. If `f` returns a non-zero libuv error code, the `callback` is invoked immediately with that status. |
| `auto` | [`invoke`](#invoke-2) `inline` | Call `f` with `args`. Overload for void-returning functions; no error checking is performed. |

---

{#request-1}

#### Request

`inline`

```cpp
inline Request()
```

Construct the request and set `req.data` to `this` so callbacks can recover the wrapper pointer.

---

{#invoke-1}

#### invoke

`inline`

```cpp
template<typename F, typename... Args> inline auto invoke(F && f, Args &&... args)
```

Call `f` with `args`. If `f` returns a non-zero libuv error code, the `callback` is invoked immediately with that status.

Enabled only when `f` returns a non-void type (i.e. an error code).

#### Parameters
* `f` libuv function to call. 

* `args` Arguments forwarded to `f`. 

#### Returns
`true` (non-zero = success) if `f` returned 0; `false` on error.

---

{#invoke-2}

#### invoke

`inline`

```cpp
template<typename F, typename... Args> inline auto invoke(F && f, Args &&... args)
```

Call `f` with `args`. Overload for void-returning functions; no error checking is performed.

#### Parameters
* `f` Function to call. 

* `args` Arguments forwarded to `f`.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`defaultCallback`](#defaultcallback) `static` `inline` | Standard libuv completion callback. Invokes `callback` with the status event and then deletes the `[Request](#request)` wrapper. |

---

{#defaultcallback}

#### defaultCallback

`static` `inline`

```cpp
static inline void defaultCallback(T * req, int status)
```

Standard libuv completion callback. Invokes `callback` with the status event and then deletes the `[Request](#request)` wrapper.

#### Parameters
* `req` The completed libuv request. 

* `status` libuv status code (0 on success, negative on error).

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-8)  |  |
| [`Event`](#event)  |  |

---

{#type-8}

#### Type

```cpp
T Type()
```

---

{#event}

#### Event

```cpp
E Event()
```

{#connectreq}

## ConnectReq

```cpp
#include <icy/request.h>
```

> **Inherits:** [`Request< uv_connect_t >`](#request)

Asynchronous connection request for TCP sockets and named pipes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ConnectReq`](#connectreq-1) `inline` | Construct and set `req.data` to `this`. |
| `auto` | [`connect`](#connect) `inline` | Initiate a TCP connection to `addr` on `handle`. |
| `auto` | [`connect`](#connect-1) `inline` | Initiate a named-pipe connection to `name` on `handle`. |

---

{#connectreq-1}

#### ConnectReq

`inline`

```cpp
inline ConnectReq()
```

Construct and set `req.data` to `this`.

---

{#connect}

#### connect

`inline`

```cpp
inline auto connect(uv_tcp_t * handle, const struct sockaddr * addr)
```

Initiate a TCP connection to `addr` on `handle`.

#### Parameters
* `handle` Initialized `uv_tcp_t` to connect. 

* `addr` Target address (IPv4 or IPv6 `sockaddr`). 

#### Returns
`true` if the connect request was submitted successfully.

---

{#connect-1}

#### connect

`inline`

```cpp
inline auto connect(uv_pipe_t * handle, const char * name)
```

Initiate a named-pipe connection to `name` on `handle`.

#### Parameters
* `handle` Initialized `uv_pipe_t` to connect. 

* `name` Filesystem path (Unix) or named-pipe name (Windows). 

#### Returns
`true` if the connect request was submitted successfully.

{#getaddrinfoevent}

## GetAddrInfoEvent

```cpp
#include <icy/request.h>
```

Callback event delivered when a `[GetAddrInfoReq](#getaddrinforeq)` resolves.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`status`](#status-1)  | libuv status: 0 on success, negative on error. |
| `struct addrinfo *` | [`addr`](#addr)  | Resolved address list; freed after the callback returns. |

---

{#status-1}

#### status

```cpp
int status
```

libuv status: 0 on success, negative on error.

---

{#addr}

#### addr

```cpp
struct addrinfo * addr = nullptr
```

Resolved address list; freed after the callback returns.

{#getaddrinforeq}

## GetAddrInfoReq

```cpp
#include <icy/request.h>
```

> **Inherits:** [`Request< uv_getaddrinfo_t, GetAddrInfoEvent >`](#request)

DNS resolver request to get the IP address of a hostname.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`GetAddrInfoReq`](#getaddrinforeq-1) `inline` | Construct and set `req.data` to `this`. |
| `auto` | [`resolve`](#resolve) `inline` | Begin asynchronous DNS resolution of `host` at `port`. |

---

{#getaddrinforeq-1}

#### GetAddrInfoReq

`inline`

```cpp
inline GetAddrInfoReq()
```

Construct and set `req.data` to `this`.

---

{#resolve}

#### resolve

`inline`

```cpp
inline auto resolve(const std::string & host, int port, uv::Loop * loop)
```

Begin asynchronous DNS resolution of `host` at `port`.

The result is delivered to `callback` as a `[GetAddrInfoEvent](#getaddrinfoevent)`. The `addrinfo` pointer in the event is freed immediately after the callback returns; do not retain it.

#### Parameters
* `host` Hostname or numeric IP address string to resolve. 

* `port` Port number; converted to a service string for `getaddrinfo`. 

* `loop` Event loop on which to run the resolution. 

#### Returns
`true` if the request was submitted successfully.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`getAddrInfoCallback`](#getaddrinfocallback) `static` `inline` | libuv completion callback for `uv_getaddrinfo`. |

---

{#getaddrinfocallback}

#### getAddrInfoCallback

`static` `inline`

```cpp
static inline void getAddrInfoCallback(Request::Type * req, int status, struct addrinfo * res)
```

libuv completion callback for `uv_getaddrinfo`.

Invokes the stored callback with the resolved address list, then frees the `addrinfo` chain and deletes the wrapper.

#### Parameters
* `req` The completed `uv_getaddrinfo_t` request. 

* `status` libuv status code. 

* `res` Resolved address list (freed after callback returns).

### Public Types

| Name | Description |
|------|-------------|
| [`Request`](#request-2)  |  |

---

{#request-2}

#### Request

```cpp
uv::Request< uv_getaddrinfo_t, GetAddrInfoEvent > Request()
```

