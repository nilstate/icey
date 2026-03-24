{#basemodule}

# base

The `base` module contains reusable cross platform tools and utilities.

### Namespaces

| Name | Description |
|------|-------------|
| [`hex`](#hex) |  |
| [`ipc`](#ipc) | Classes for inter-process communication. |
| [`test`](#test) | Modern unit testing framework. |
| [`time`](#time-3) | Classes and functions for handling time. |
| [`base64`](#base64) |  |
| [`deleter`](#deleter-1) |  |
| [`numeric`](#numeric) |  |
| [`basic`](#basic) | Interface classes. |
| [`fs`](#fs) |  |
| [`util`](#util) |  |

{#hex}

# hex

### Classes

| Name | Description |
|------|-------------|
| [`Decoder`](#decoder) | Hex decoder. |
| [`Encoder`](#encoder-1) | Hex encoder. |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`encode`](#encode-14) `inline` | Converts the STL container to Hex. |

---

{#encode-14}

#### encode

`inline`

```cpp
template<typename T> inline std::string encode(const T & bytes)
```

Converts the STL container to Hex.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const T &` |  |

{#decoder}

## Decoder

```cpp
#include <hex.h>
```

> **Inherits:** [`Decoder`](#decoder-4)

Hex decoder.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `char` | [`lastbyte`](#lastbyte)  |  |

---

{#lastbyte}

#### lastbyte

```cpp
char lastbyte
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Decoder`](#decoder-1) `inline` |  |
| `ssize_t` | [`decode`](#decode-4) `virtual` `inline` | Decodes hex-encoded input to binary. Whitespace in the input is ignored. A trailing unpaired nibble is buffered and prepended on the next call.  |
| `ssize_t` | [`finalize`](#finalize) `virtual` `inline` | No-op finalizer; hex decoding has no pending output state.  |
| `bool` | [`readnext`](#readnext) `inline` | Reads the next non-whitespace character from inbuf, prepending any buffered lastbyte before consuming from the stream.  |
| `int` | [`nybble`](#nybble) `inline` | Converts an ASCII hex character to its 4-bit integer value.  |
| `bool` | [`iswspace`](#iswspace) `inline` | Returns true if c is an ASCII whitespace character (space, CR, tab, LF).  |

---

{#decoder-1}

#### Decoder

`inline`

```cpp
inline Decoder()
```

---

{#decode-4}

#### decode

`virtual` `inline`

```cpp
virtual inline ssize_t decode(const char * inbuf, size_t nread, char * outbuf)
```

Decodes hex-encoded input to binary. Whitespace in the input is ignored. A trailing unpaired nibble is buffered and prepended on the next call. 
#### Parameters
* `inbuf` Hex-encoded input buffer. 

* `nread` Number of bytes to read from inbuf. 

* `outbuf` Destination buffer; must have capacity >= nread / 2. 

#### Returns
Number of decoded bytes written to outbuf.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

{#finalize}

#### finalize

`virtual` `inline`

```cpp
virtual inline ssize_t finalize(char *)
```

No-op finalizer; hex decoding has no pending output state. 
#### Returns
Always 0.

---

{#readnext}

#### readnext

`inline`

```cpp
inline bool readnext(const char * inbuf, size_t nread, size_t & rpos, char & c)
```

Reads the next non-whitespace character from inbuf, prepending any buffered lastbyte before consuming from the stream. 
#### Parameters
* `inbuf` Input buffer. 

* `nread` Total bytes in inbuf. 

* `rpos` Current read position; advanced on each consumed byte. 

* `c` Output: the next non-whitespace character. 

#### Returns
true if more input remains after c was read, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `rpos` | `size_t &` |  |
| `c` | `char &` |  |

---

{#nybble}

#### nybble

`inline`

```cpp
inline int nybble(const int n)
```

Converts an ASCII hex character to its 4-bit integer value. 
#### Parameters
* `n` ASCII character ('0'-'9', 'a'-'f', 'A'-'F'). 

#### Returns
Integer value in the range [0, 15]. 

#### Exceptions
* `std::runtime_error` if n is not a valid hex character.

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `const int` |  |

---

{#iswspace}

#### iswspace

`inline`

```cpp
inline bool iswspace(const char c)
```

Returns true if c is an ASCII whitespace character (space, CR, tab, LF). 
#### Parameters
* `c` Character to test. 

#### Returns
true if c is whitespace.

| Parameter | Type | Description |
|-----------|------|-------------|
| `c` | `const char` |  |

{#encoder-1}

## Encoder

```cpp
#include <hex.h>
```

> **Inherits:** [`Encoder`](#encoder-5)

Hex encoder.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`_linePos`](#_linepos)  |  |
| `int` | [`_lineLength`](#_linelength)  |  |
| `int` | [`_uppercase`](#_uppercase)  |  |

---

{#_linepos}

#### _linePos

```cpp
int _linePos
```

---

{#_linelength}

#### _lineLength

```cpp
int _lineLength
```

---

{#_uppercase}

#### _uppercase

```cpp
int _uppercase
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Encoder`](#encoder-2) `inline` |  |
| `ssize_t` | [`encode`](#encode-15) `virtual` `inline` | Encodes binary input as lowercase hex characters, optionally inserting newlines every `_lineLength` output characters.  |
| `ssize_t` | [`finalize`](#finalize-1) `virtual` `inline` | No-op finalizer; hex encoding has no pending state.  |
| `void` | [`setUppercase`](#setuppercase) `inline` | Controls whether encoded output uses uppercase hex digits (A-F) or lowercase (a-f).  |
| `void` | [`setLineLength`](#setlinelength) `inline` | Sets the maximum number of output characters per line before a newline is inserted. Set to 0 to disable line wrapping.  |

---

{#encoder-2}

#### Encoder

`inline`

```cpp
inline Encoder()
```

---

{#encode-15}

#### encode

`virtual` `inline`

```cpp
virtual inline ssize_t encode(const char * inbuf, size_t nread, char * outbuf)
```

Encodes binary input as lowercase hex characters, optionally inserting newlines every `_lineLength` output characters. 
#### Parameters
* `inbuf` Input buffer to encode. 

* `nread` Number of bytes to read from inbuf. 

* `outbuf` Destination buffer; must have capacity >= nread * 2 + nread/_lineLength + 1. 

#### Returns
Number of bytes written to outbuf.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

{#finalize-1}

#### finalize

`virtual` `inline`

```cpp
virtual inline ssize_t finalize(char *)
```

No-op finalizer; hex encoding has no pending state. 
#### Returns
Always 0.

---

{#setuppercase}

#### setUppercase

`inline`

```cpp
inline void setUppercase(bool flag)
```

Controls whether encoded output uses uppercase hex digits (A-F) or lowercase (a-f). 
#### Parameters
* `flag` true for uppercase, false for lowercase.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

{#setlinelength}

#### setLineLength

`inline`

```cpp
inline void setLineLength(int lineLength)
```

Sets the maximum number of output characters per line before a newline is inserted. Set to 0 to disable line wrapping. 
#### Parameters
* `lineLength` Characters per line.

| Parameter | Type | Description |
|-----------|------|-------------|
| `lineLength` | `int` |  |

{#ipc}

# ipc

Classes for inter-process communication.

### Classes

| Name | Description |
|------|-------------|
| [`Queue`](#queue) | IPC queue is for safely passing templated actions between threads and processes. |
| [`SyncQueue`](#syncqueue) | IPC synchronization queue is for passing templated actions between threads and the event loop we are synchronizing with. |
| [`Action`](#action) | Default action type for executing synchronized callbacks. |

{#queue}

## Queue

```cpp
#include <ipc.h>
```

> **Subclassed by:** [`SyncQueue< TAction >`](#syncqueue)

IPC queue is for safely passing templated actions between threads and processes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Queue`](#queue-1) `inline` |  |
| `void` | [`push`](#push-1) `virtual` `inline` | Pushes an action onto the queue and triggers a post notification. Takes ownership of action; the queue deletes it after execution. Thread-safe.  |
| `TAction *` | [`pop`](#pop) `virtual` `inline` | Removes and returns the next action from the front of the queue. The caller takes ownership of the returned pointer. Thread-safe.  |
| `void` | [`runSync`](#runsync) `virtual` `inline` | Drains the queue by invoking and deleting every pending action in order. Must be called from the thread that owns the event loop. |
| `void` | [`close`](#close-17) `virtual` `inline` | Closes the underlying notification handle. No-op in the base implementation. |
| `void` | [`post`](#post) `virtual` `inline` | Signals the event loop that new actions are available. No-op in the base implementation. |
| `void` | [`waitForSync`](#waitforsync) `inline` | Blocks the calling thread until the queue is empty or the timeout elapses. Polls every 10 ms. Logs a warning if the timeout is reached.  |

---

{#queue-1}

#### Queue

`inline`

```cpp
inline Queue()
```

---

{#push-1}

#### push

`virtual` `inline`

```cpp
virtual inline void push(TAction * action)
```

Pushes an action onto the queue and triggers a post notification. Takes ownership of action; the queue deletes it after execution. Thread-safe. 
#### Parameters
* `action` Heap-allocated action to enqueue.

| Parameter | Type | Description |
|-----------|------|-------------|
| `action` | `TAction *` |  |

---

{#pop}

#### pop

`virtual` `inline`

```cpp
virtual inline TAction * pop()
```

Removes and returns the next action from the front of the queue. The caller takes ownership of the returned pointer. Thread-safe. 
#### Returns
Pointer to the next action, or nullptr if the queue is empty.

---

{#runsync}

#### runSync

`virtual` `inline`

```cpp
virtual inline void runSync()
```

Drains the queue by invoking and deleting every pending action in order. Must be called from the thread that owns the event loop.

---

{#close-17}

#### close

`virtual` `inline`

```cpp
virtual inline void close()
```

Closes the underlying notification handle. No-op in the base implementation.

---

{#post}

#### post

`virtual` `inline`

```cpp
virtual inline void post()
```

Signals the event loop that new actions are available. No-op in the base implementation.

---

{#waitforsync}

#### waitForSync

`inline`

```cpp
inline void waitForSync(std::chrono::milliseconds timeout)
```

Blocks the calling thread until the queue is empty or the timeout elapses. Polls every 10 ms. Logs a warning if the timeout is reached. 
#### Parameters
* `timeout` Maximum time to wait (default: 5000 ms).

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `std::chrono::milliseconds` |  |

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-7)  |  |
| `std::deque< TAction * >` | [`_actions`](#_actions)  |  |

---

{#_mutex-7}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_actions}

#### _actions

```cpp
std::deque< TAction * > _actions
```

{#syncqueue}

## SyncQueue

```cpp
#include <ipc.h>
```

> **Inherits:** [`Action >`](#queue)

IPC synchronization queue is for passing templated actions between threads and the event loop we are synchronizing with.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`SyncQueue`](#syncqueue-1) `inline` | Constructs a [SyncQueue](#syncqueue) bound to the given libuv event loop.  |
| `void` | [`close`](#close-18) `virtual` `inline` | Closes the underlying [Synchronizer](#classicy_1_1Synchronizer) handle and stops loop wakeups. |
| `void` | [`post`](#post-1) `virtual` `inline` | Wakes up the event loop so pending actions are dispatched via [runSync()](#runsync). |
| `Synchronizer &` | [`sync`](#sync) `virtual` `inline` | Returns a reference to the internal [Synchronizer](#classicy_1_1Synchronizer).  |

---

{#syncqueue-1}

#### SyncQueue

`inline`

```cpp
inline SyncQueue(uv::Loop * loop)
```

Constructs a [SyncQueue](#syncqueue) bound to the given libuv event loop. 
#### Parameters
* `loop` Event loop to synchronize with (default: the process-wide default loop).

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `[uv::Loop](#namespaceicy_1_1uv_1a8bfd153231f95de982e16db911389619) *` |  |

---

{#close-18}

#### close

`virtual` `inline`

```cpp
virtual inline void close()
```

Closes the underlying [Synchronizer](#classicy_1_1Synchronizer) handle and stops loop wakeups.

---

{#post-1}

#### post

`virtual` `inline`

```cpp
virtual inline void post()
```

Wakes up the event loop so pending actions are dispatched via [runSync()](#runsync).

---

{#sync}

#### sync

`virtual` `inline`

```cpp
virtual inline Synchronizer & sync()
```

Returns a reference to the internal [Synchronizer](#classicy_1_1Synchronizer). 
#### Returns
Reference to the [Synchronizer](#classicy_1_1Synchronizer) used for loop wakeup.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Synchronizer` | [`_sync`](#_sync)  |  |

---

{#_sync}

#### _sync

```cpp
Synchronizer _sync
```

{#action}

## Action

```cpp
#include <ipc.h>
```

Default action type for executing synchronized callbacks.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Callback` | [`target`](#target)  | The callable to invoke when the action is dispatched. |
| `void *` | [`arg`](#arg)  | Optional opaque pointer passed to the callback. |
| `std::string` | [`data`](#data)  | Optional string payload passed to the callback. |

---

{#target}

#### target

```cpp
Callback target
```

The callable to invoke when the action is dispatched.

---

{#arg}

#### arg

```cpp
void * arg
```

Optional opaque pointer passed to the callback.

---

{#data}

#### data

```cpp
std::string data
```

Optional string payload passed to the callback.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Action`](#action-1) `inline` | Constructs an [Action](#action) with the given callback, optional argument, and optional data.  |

---

{#action-1}

#### Action

`inline`

```cpp
inline Action(Callback target, void * arg, const std::string & data)
```

Constructs an [Action](#action) with the given callback, optional argument, and optional data. 
#### Parameters
* `target` Callback to invoke on dispatch. 

* `arg` Opaque pointer passed to the callback (default: nullptr). 

* `data` String payload passed to the callback (default: empty).

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `[Callback](#callback-1)` |  |
| `arg` | `void *` |  |
| `data` | `const std::string &` |  |

### Public Types

| Name | Description |
|------|-------------|
| [`Callback`](#callback-1)  |  |

---

{#callback-1}

#### Callback

```cpp
std::function< void(const Action &)> Callback()
```

{#test}

# test

Modern unit testing framework.

### Classes

| Name | Description |
|------|-------------|
| [`FunctionTest`](#functiontest) | [Test](#test-1) wrapper for standalone test functions. |
| [`Test`](#test-1) | [Test](#test-1) wrapper class. |
| [`TestRunner`](#testrunner) | [Test](#test-1) manager queue. |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`init`](#init-7)  | Initialize the test environment. |
| `int` | [`finalize`](#finalize-2)  | Finalize the test environment. |
| `void` | [`runAll`](#runall)  | Run all tests. |
| `void` | [`describe`](#describe)  | Describe a test environment implemented by the given lambda function. |
| `void` | [`describe`](#describe-1)  | Describe a test environment implemented by the given test instance. |
| `void` | [`expectImpl`](#expectimpl)  | Expect asserts that a condition is true (use [expect()](#test_8h_1a92645105a4c87ac01db7587df58caca6) as defined below). |
| `bool` | [`waitFor`](#waitfor)  | Run the event loop until a condition is met or timeout expires. Returns true if the condition was satisfied, false on timeout. Useful for testing async operations that complete via libuv callbacks. |

---

{#init-7}

#### init

```cpp
void init()
```

Initialize the test environment.

---

{#finalize-2}

#### finalize

```cpp
int finalize()
```

Finalize the test environment.

Destroy the [TestRunner](#testrunner) singleton instance and return the exit code.

---

{#runall}

#### runAll

```cpp
void runAll()
```

Run all tests.

---

{#describe}

#### describe

```cpp
void describe(const std::string & name, std::function< void()> target)
```

Describe a test environment implemented by the given lambda function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `target` | `std::function< void()>` |  |

---

{#describe-1}

#### describe

```cpp
void describe(const std::string & name, Test * test)
```

Describe a test environment implemented by the given test instance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `test` | `[Test](#test-1) *` |  |

---

{#expectimpl}

#### expectImpl

```cpp
void expectImpl(bool passed, const char * assert, const char * file, long line)
```

Expect asserts that a condition is true (use [expect()](#test_8h_1a92645105a4c87ac01db7587df58caca6) as defined below).

| Parameter | Type | Description |
|-----------|------|-------------|
| `passed` | `bool` |  |
| `assert` | `const char *` |  |
| `file` | `const char *` |  |
| `line` | `long` |  |

---

{#waitfor}

#### waitFor

```cpp
bool waitFor(std::function< bool()> condition, int timeoutMs)
```

Run the event loop until a condition is met or timeout expires. Returns true if the condition was satisfied, false on timeout. Useful for testing async operations that complete via libuv callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `condition` | `std::function< bool()>` |  |
| `timeoutMs` | `int` |  |

{#functiontest}

## FunctionTest

```cpp
#include <test.h>
```

> **Inherits:** [`Test`](#test-1)

[Test](#test-1) wrapper for standalone test functions.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::function< void()>` | [`target`](#target-1)  |  |

---

{#target-1}

#### target

```cpp
std::function< void()> target
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FunctionTest`](#functiontest-1) `inline` | #### Parameters |

---

{#functiontest-1}

#### FunctionTest

`inline`

```cpp
inline FunctionTest(std::function< void()> target, const std::string & name)
```

#### Parameters
* `target` Lambda or function to execute as the test body. 

* `name` Human-readable test name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `std::function< void()>` |  |
| `name` | `const std::string &` |  |

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`run`](#run-1) `virtual` `inline` | Called by the [TestRunner](#testrunner) to run the test. |

---

{#run-1}

#### run

`virtual` `inline`

```cpp
virtual inline void run()
```

Called by the [TestRunner](#testrunner) to run the test.

{#test-1}

## Test

```cpp
#include <test.h>
```

> **Subclassed by:** [`FunctionTest`](#functiontest)

[Test](#test-1) wrapper class.

This class is for implementing any kind of unit test that can be executed by a `[TestRunner](#testrunner)`.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`name`](#name-4)  | The name of the test. |
| `SErrorist` | [`errors`](#errors)  | A list of test errors. |
| `double` | [`duration`](#duration-1)  | The test run duration for benchmarking. |

---

{#name-4}

#### name

```cpp
std::string name
```

The name of the test.

---

{#errors}

#### errors

```cpp
SErrorist errors
```

A list of test errors.

---

{#duration-1}

#### duration

```cpp
double duration
```

The test run duration for benchmarking.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Test`](#test-2)  | #### Parameters |
|  | [`~Test`](#test-3) `virtual` | Should remain protected. |
| `void` | [`run`](#run-2)  | Called by the [TestRunner](#testrunner) to run the test. |
| `bool` | [`passed`](#passed)  | Return true when the test passed without errors. |

---

{#test-2}

#### Test

```cpp
Test(const std::string & name)
```

#### Parameters
* `name` Human-readable name displayed in test output.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

{#test-3}

#### ~Test

`virtual`

```cpp
virtual ~Test()
```

Should remain protected.

---

{#run-2}

#### run

```cpp
void run()
```

Called by the [TestRunner](#testrunner) to run the test.

---

{#passed}

#### passed

```cpp
bool passed()
```

Return true when the test passed without errors.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Test`](#test-4)  |  |

---

{#test-4}

#### Test

```cpp
Test(const Test & test) = delete
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `test` | `const [Test](#test-1) &` |  |

{#testrunner}

## TestRunner

```cpp
#include <test.h>
```

[Test](#test-1) manager queue.

The `[TestRunner](#testrunner)` is a queue in charge of running one or many tests.

When `[run()](#run-3)` the `[TestRunner](#testrunner)` loops through each test in the list calling the test's `[run()](#run-3)` method.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`TestRunner`](#testrunner-1)  |  |
| `void` | [`add`](#add)  | Adds a test to the runner and prints its name to stdout.  |
| `Test *` | [`get`](#get-3) `const` | Return a pointer to the test matching the given name, or nullptr if no matching test exists.  |
| `void` | [`run`](#run-3)  | Runs all registered tests sequentially, printing results to stdout. |
| `void` | [`clear`](#clear-1)  | Destroy and clears all managed tests. |
| `Test *` | [`current`](#current) `const` | Return the currently active [Test](#test-1) or nullptr. |
| `TestList` | [`tests`](#tests) `const` | Return the list of tests. |
| `ErrorMap` | [`errors`](#errors-1) `const` | Return a map of tests and errors. |
| `bool` | [`passed`](#passed-1) `const` | Return true if all tests passed. |

---

{#testrunner-1}

#### TestRunner

```cpp
TestRunner()
```

---

{#add}

#### add

```cpp
void add(Test * test)
```

Adds a test to the runner and prints its name to stdout. 
#### Parameters
* `test` Non-null pointer to the test; the runner takes ownership.

| Parameter | Type | Description |
|-----------|------|-------------|
| `test` | `[Test](#test-1) *` |  |

---

{#get-3}

#### get

`const`

```cpp
Test * get(std::string_view name) const
```

Return a pointer to the test matching the given name, or nullptr if no matching test exists. 
#### Parameters
* `name` [Test](#test-1) name to search for. 

#### Returns
Matching test pointer or nullptr.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string_view` |  |

---

{#run-3}

#### run

```cpp
void run()
```

Runs all registered tests sequentially, printing results to stdout.

---

{#clear-1}

#### clear

```cpp
void clear()
```

Destroy and clears all managed tests.

---

{#current}

#### current

`const`

```cpp
Test * current() const
```

Return the currently active [Test](#test-1) or nullptr.

---

{#tests}

#### tests

`const`

```cpp
TestList tests() const
```

Return the list of tests.

---

{#errors-1}

#### errors

`const`

```cpp
ErrorMap errors() const
```

Return a map of tests and errors.

---

{#passed-1}

#### passed

`const`

```cpp
bool passed() const
```

Return true if all tests passed.

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `TestRunner &` | [`getDefault`](#getdefault-1) `static` | Return the default `[TestRunner](#testrunner)` singleton, although `[TestRunner](#testrunner)` instances may also be initialized individually. |

---

{#getdefault-1}

#### getDefault

`static`

```cpp
static TestRunner & getDefault()
```

Return the default `[TestRunner](#testrunner)` singleton, although `[TestRunner](#testrunner)` instances may also be initialized individually.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-8)  |  |
| `TestList` | [`_tests`](#_tests)  |  |
| `Test *` | [`_current`](#_current)  |  |

---

{#_mutex-8}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_tests}

#### _tests

```cpp
TestList _tests
```

---

{#_current}

#### _current

```cpp
Test * _current
```

{#time-3}

# time

Classes and functions for handling time.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::time_t` | [`now`](#now)  | Returns the current wall-clock time as a UTC time_t (seconds since epoch).  |
| `double` | [`clockSecs`](#clocksecs)  | Returns the elapsed process time in decimal seconds using a monotonic clock.  |
| `std::string` | [`print`](#print-7)  | Formats a broken-down time value using the given strftime format string.  |
| `std::string` | [`printLocal`](#printlocal)  | Formats the current local time using the given strftime format string.  |
| `std::string` | [`printUTC`](#printutc)  | Formats the current UTC time using the given strftime format string.  |
| `std::tm` | [`toLocal`](#tolocal)  | Converts a time_t value to a broken-down local time structure. Uses thread-safe native functions (localtime_r / localtime_s).  |
| `std::tm` | [`toUTC`](#toutc)  | Converts a time_t value to a broken-down UTC time structure. Uses thread-safe native functions (gmtime_r / gmtime_s).  |
| `std::string` | [`getLocal`](#getlocal)  | Returns the current local time as an ISO8601 formatted string.  |
| `std::string` | [`getUTC`](#getutc)  | Returns the current UTC time as an ISO8601 formatted string.  |
| `uint64_t` | [`hrtime`](#hrtime)  | Returns the current high-resolution monotonic time in nanoseconds.  |

---

{#now}

#### now

```cpp
std::time_t now()
```

Returns the current wall-clock time as a UTC time_t (seconds since epoch). 
#### Returns
Current UTC time in seconds since the Unix epoch.

---

{#clocksecs}

#### clockSecs

```cpp
double clockSecs()
```

Returns the elapsed process time in decimal seconds using a monotonic clock. 
#### Returns
[Process](#classicy_1_1Process) time in seconds.

---

{#print-7}

#### print

```cpp
std::string print(const std::tm & dt, const char * fmt)
```

Formats a broken-down time value using the given strftime format string. 
#### Parameters
* `dt` Broken-down time to format. 

* `fmt` strftime format string (default: ISO8601Format). 

#### Returns
Formatted time string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dt` | `const std::tm &` |  |
| `fmt` | `const char *` |  |

---

{#printlocal}

#### printLocal

```cpp
std::string printLocal(const char * fmt)
```

Formats the current local time using the given strftime format string. 
#### Parameters
* `fmt` strftime format string (default: ISO8601Format). 

#### Returns
Formatted local time string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fmt` | `const char *` |  |

---

{#printutc}

#### printUTC

```cpp
std::string printUTC(const char * fmt)
```

Formats the current UTC time using the given strftime format string. 
#### Parameters
* `fmt` strftime format string (default: ISO8601Format). 

#### Returns
Formatted UTC time string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fmt` | `const char *` |  |

---

{#tolocal}

#### toLocal

```cpp
std::tm toLocal(const std::time_t & time)
```

Converts a time_t value to a broken-down local time structure. Uses thread-safe native functions (localtime_r / localtime_s). 
#### Parameters
* `time` UTC time value to convert. 

#### Returns
Broken-down local time.

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | `const std::time_t &` |  |

---

{#toutc}

#### toUTC

```cpp
std::tm toUTC(const std::time_t & time)
```

Converts a time_t value to a broken-down UTC time structure. Uses thread-safe native functions (gmtime_r / gmtime_s). 
#### Parameters
* `time` UTC time value to convert. 

#### Returns
Broken-down UTC time.

| Parameter | Type | Description |
|-----------|------|-------------|
| `time` | `const std::time_t &` |  |

---

{#getlocal}

#### getLocal

```cpp
std::string getLocal()
```

Returns the current local time as an ISO8601 formatted string. 
#### Returns
ISO8601 local time string.

---

{#getutc}

#### getUTC

```cpp
std::string getUTC()
```

Returns the current UTC time as an ISO8601 formatted string. 
#### Returns
ISO8601 UTC time string.

---

{#hrtime}

#### hrtime

```cpp
uint64_t hrtime()
```

Returns the current high-resolution monotonic time in nanoseconds. 
#### Returns
Current time in nanoseconds (suitable for measuring intervals).

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `const int64_t` | [`kNumMillisecsPerSec`](#knummillisecspersec) `static` | Constants for calculating time. |
| `const int64_t` | [`kNumMicrosecsPerSec`](#knummicrosecspersec) `static` |  |
| `const int64_t` | [`kNumNanosecsPerSec`](#knumnanosecspersec) `static` |  |
| `const int64_t` | [`kNumMicrosecsPerMillisec`](#knummicrosecspermillisec) `static` |  |
| `const int64_t` | [`kNumNanosecsPerMillisec`](#knumnanosecspermillisec) `static` |  |
| `const int64_t` | [`kNumNanosecsPerMicrosec`](#knumnanosecspermicrosec) `static` |  |
| `const char *` | [`ISO8601Format`](#iso8601format) `static` | The date/time format defined in the ISO 8601 standard. This is the default format used throughout the library for consistency. |

---

{#knummillisecspersec}

#### kNumMillisecsPerSec

`static`

```cpp
const int64_t kNumMillisecsPerSec = (1000)
```

Constants for calculating time.

---

{#knummicrosecspersec}

#### kNumMicrosecsPerSec

`static`

```cpp
const int64_t kNumMicrosecsPerSec = (1000000)
```

---

{#knumnanosecspersec}

#### kNumNanosecsPerSec

`static`

```cpp
const int64_t kNumNanosecsPerSec = (1000000000)
```

---

{#knummicrosecspermillisec}

#### kNumMicrosecsPerMillisec

`static`

```cpp
const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec
```

---

{#knumnanosecspermillisec}

#### kNumNanosecsPerMillisec

`static`

```cpp
const int64_t kNumNanosecsPerMillisec = kNumNanosecsPerSec / kNumMillisecsPerSec
```

---

{#knumnanosecspermicrosec}

#### kNumNanosecsPerMicrosec

`static`

```cpp
const int64_t kNumNanosecsPerMicrosec = kNumNanosecsPerSec / kNumMicrosecsPerSec
```

---

{#iso8601format}

#### ISO8601Format

`static`

```cpp
const char * ISO8601Format = "%Y-%m-%dT%H:%M:%SZ"
```

The date/time format defined in the ISO 8601 standard. This is the default format used throughout the library for consistency.

Examples: 2005-01-01T12:00:00+01:00 2005-01-01T11:00:00Z

{#base64}

# base64

### Classes

| Name | Description |
|------|-------------|
| [`Decoder`](#decoder-2) | Base64 decoder. |
| [`Encoder`](#encoder-3) | Base64 encoder. |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `size_t` | [`encodedBufferCapacity`](#encodedbuffercapacity) `inline` | Returns a safe temporary buffer size for encoding up to `inputSize` bytes. Includes padding/newline slack so callers can reuse the same buffer for finalize(). |
| `std::string` | [`encode`](#encode-16) `inline` | Encodes an STL byte container to a Base64 string.  |
| `std::string` | [`decode`](#decode-5) `inline` | Decodes a Base64-encoded STL container to a binary string.  |

---

{#encodedbuffercapacity}

#### encodedBufferCapacity

`inline`

```cpp
inline size_t encodedBufferCapacity(size_t inputSize, int lineLength)
```

Returns a safe temporary buffer size for encoding up to `inputSize` bytes. Includes padding/newline slack so callers can reuse the same buffer for finalize().

| Parameter | Type | Description |
|-----------|------|-------------|
| `inputSize` | `size_t` |  |
| `lineLength` | `int` |  |

---

{#encode-16}

#### encode

`inline`

```cpp
template<typename T> inline std::string encode(const T & bytes, int lineLength)
```

Encodes an STL byte container to a Base64 string. 
#### Parameters
* `T` Container type with a `size()` method and contiguous `operator[]`. 

#### Parameters
* `bytes` Input data container. 

* `lineLength` Characters per line in the output (0 disables wrapping). 

#### Returns
Base64-encoded string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const T &` |  |
| `lineLength` | `int` |  |

---

{#decode-5}

#### decode

`inline`

```cpp
template<typename T> inline std::string decode(const T & bytes)
```

Decodes a Base64-encoded STL container to a binary string. 
#### Parameters
* `T` Container type with a `size()` method and contiguous `operator[]`. 

#### Parameters
* `bytes` Input Base64 data container. 

#### Returns
Decoded binary string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const T &` |  |

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `constexpr int` | [`BUFFER_SIZE`](#buffer_size)  |  |
| `constexpr int` | [`LINE_LENGTH`](#line_length)  |  |

---

{#buffer_size}

#### BUFFER_SIZE

```cpp
constexpr int BUFFER_SIZE = 16384
```

---

{#line_length}

#### LINE_LENGTH

```cpp
constexpr int LINE_LENGTH = 72
```

{#decoder-2}

## Decoder

```cpp
#include <base64.h>
```

> **Inherits:** [`Decoder`](#decoder-4)

Base64 decoder.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `internal::decodestate` | [`_state`](#_state)  |  |
| `int` | [`_buffersize`](#_buffersize)  |  |

---

{#_state}

#### _state

```cpp
internal::decodestate _state
```

---

{#_buffersize}

#### _buffersize

```cpp
int _buffersize
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Decoder`](#decoder-3) `inline` | #### Parameters |
| `ssize_t` | [`decode`](#decode-6) `inline` | Decodes a single Base64 character to its 6-bit value.  |
| `ssize_t` | [`decode`](#decode-7) `virtual` `inline` | Decodes a raw Base64 buffer into binary data.  |
| `void` | [`decode`](#decode-8) `inline` | Decodes the entire input stream and writes binary output to `ostrm`. Resets the decoder state after completion.  |

---

{#decoder-3}

#### Decoder

`inline`

```cpp
inline Decoder(int buffersize)
```

#### Parameters
* `buffersize` Internal read buffer size in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffersize` | `int` |  |

---

{#decode-6}

#### decode

`inline`

```cpp
inline ssize_t decode(char value_in)
```

Decodes a single Base64 character to its 6-bit value. 
#### Parameters
* `value_in` Base64 character. 

#### Returns
Decoded 6-bit value, or a negative sentinel on invalid input.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value_in` | `char` |  |

---

{#decode-7}

#### decode

`virtual` `inline`

```cpp
virtual inline ssize_t decode(const char * inbuf, size_t nread, char * outbuf)
```

Decodes a raw Base64 buffer into binary data. 
#### Parameters
* `inbuf` Input Base64 characters. 

* `nread` Number of characters to decode. 

* `outbuf` Output buffer; must be at least `nread * 3 / 4` bytes. 

#### Returns
Number of binary bytes written.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

{#decode-8}

#### decode

`inline`

```cpp
inline void decode(std::istream & istrm, std::ostream & ostrm)
```

Decodes the entire input stream and writes binary output to `ostrm`. Resets the decoder state after completion. 
#### Parameters
* `istrm` Source stream of Base64 data. 

* `ostrm` Destination stream for decoded binary output.

| Parameter | Type | Description |
|-----------|------|-------------|
| `istrm` | `std::istream &` |  |
| `ostrm` | `std::ostream &` |  |

{#encoder-3}

## Encoder

```cpp
#include <base64.h>
```

> **Inherits:** [`Encoder`](#encoder-5)

Base64 encoder.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `internal::encodestate` | [`_state`](#_state-1)  |  |
| `int` | [`_buffersize`](#_buffersize-1)  |  |

---

{#_state-1}

#### _state

```cpp
internal::encodestate _state
```

---

{#_buffersize-1}

#### _buffersize

```cpp
int _buffersize
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Encoder`](#encoder-4) `inline` | #### Parameters |
| `void` | [`encode`](#encode-17) `inline` | Encodes the entire input stream and writes Base64 output to `ostrm`. Resets the encoder state after completion.  |
| `void` | [`encode`](#encode-18) `inline` | Encodes a string to Base64 and appends the result to `out`. Resets the encoder state after completion.  |
| `ssize_t` | [`encode`](#encode-19) `virtual` `inline` | Encodes a raw buffer, writing Base64 characters to `outbuf`. May be called multiple times before calling `[finalize()](#finalize-3)`.  |
| `ssize_t` | [`finalize`](#finalize-3) `virtual` `inline` | Writes any pending padding and resets the encoder state. Must be called once after all `[encode()](#encode-17)` calls to flush the final block.  |
| `void` | [`setLineLength`](#setlinelength-1) `inline` | Sets the line wrap length for encoded output (0 disables line wrapping).  |

---

{#encoder-4}

#### Encoder

`inline`

```cpp
inline Encoder(int buffersize)
```

#### Parameters
* `buffersize` Internal read buffer size in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffersize` | `int` |  |

---

{#encode-17}

#### encode

`inline`

```cpp
inline void encode(std::istream & istrm, std::ostream & ostrm)
```

Encodes the entire input stream and writes Base64 output to `ostrm`. Resets the encoder state after completion. 
#### Parameters
* `istrm` Source stream to encode. 

* `ostrm` Destination stream for Base64 output.

| Parameter | Type | Description |
|-----------|------|-------------|
| `istrm` | `std::istream &` |  |
| `ostrm` | `std::ostream &` |  |

---

{#encode-18}

#### encode

`inline`

```cpp
inline void encode(const std::string & in, std::string & out)
```

Encodes a string to Base64 and appends the result to `out`. Resets the encoder state after completion. 
#### Parameters
* `in` Input string. 

* `out` Output string to which Base64 characters are appended.

| Parameter | Type | Description |
|-----------|------|-------------|
| `in` | `const std::string &` |  |
| `out` | `std::string &` |  |

---

{#encode-19}

#### encode

`virtual` `inline`

```cpp
virtual inline ssize_t encode(const char * inbuf, size_t nread, char * outbuf)
```

Encodes a raw buffer, writing Base64 characters to `outbuf`. May be called multiple times before calling `[finalize()](#finalize-3)`. 
#### Parameters
* `inbuf` Input binary data. 

* `nread` Number of bytes to encode. 

* `outbuf` Output buffer; must be at least `nread * 4 / 3 + 4` bytes. 

#### Returns
Number of Base64 characters written.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

{#finalize-3}

#### finalize

`virtual` `inline`

```cpp
virtual inline ssize_t finalize(char * outbuf)
```

Writes any pending padding and resets the encoder state. Must be called once after all `[encode()](#encode-17)` calls to flush the final block. 
#### Parameters
* `outbuf` Output buffer; must be at least 5 bytes. 

#### Returns
Number of characters written.

| Parameter | Type | Description |
|-----------|------|-------------|
| `outbuf` | `char *` |  |

---

{#setlinelength-1}

#### setLineLength

`inline`

```cpp
inline void setLineLength(int lineLength)
```

Sets the line wrap length for encoded output (0 disables line wrapping). 
#### Parameters
* `lineLength` Characters per line; use 0 to disable.

| Parameter | Type | Description |
|-----------|------|-------------|
| `lineLength` | `int` |  |

{#deleter-1}

# deleter

### Classes

| Name | Description |
|------|-------------|
| [`Array`](#array) | Deleter functor for array pointers. Calls delete[] on the pointer. |
| [`Dispose`](#dispose-1) | Deleter functor that calls dispose() on the pointer. Useful with std::unique_ptr for objects that require a dispose() call rather than direct deletion. |

{#array}

## Array

Deleter functor for array pointers. Calls delete[] on the pointer.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-9) `inline` | Calls delete[] on ptr if non-null.  |

---

{#operator-9}

#### operator()

`inline`

```cpp
inline void operator()(T * ptr)
```

Calls delete[] on ptr if non-null. 
#### Parameters
* `ptr` Array pointer to delete; may be nullptr.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `T *` |  |

{#dispose-1}

## Dispose

Deleter functor that calls dispose() on the pointer. Useful with std::unique_ptr for objects that require a dispose() call rather than direct deletion.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator()`](#operator-10) `inline` | Calls ptr->dispose() if ptr is non-null.  |

---

{#operator-10}

#### operator()

`inline`

```cpp
inline void operator()(T * ptr)
```

Calls ptr->dispose() if ptr is non-null. 
#### Parameters
* `ptr` Pointer to dispose; may be nullptr.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `T *` |  |

{#numeric}

# numeric

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`format`](#format-8)  | Formats an integer value in decimal notation.  |
| `void` | [`format`](#format-9)  | Formats an integer value in decimal notation, right justified in a field having at least the specified width.  |
| `void` | [`format0`](#format0)  | Formats an integer value in decimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`formatHex`](#formathex)  | Formats an int value in hexadecimal notation. The value is treated as unsigned.  |
| `void` | [`formatHex`](#formathex-1)  | Formats an int value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. The value is treated as unsigned.  |
| `void` | [`format`](#format-10)  | Formats an unsigned int value in decimal notation.  |
| `void` | [`format`](#format-11)  | Formats an unsigned int value in decimal notation, right justified in a field having at least the specified width.  |
| `void` | [`format0`](#format0-1)  | Formats an unsigned int value in decimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`formatHex`](#formathex-2)  | Formats an unsigned int value in hexadecimal notation.  |
| `void` | [`formatHex`](#formathex-3)  | Formats an unsigned int value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`format`](#format-12)  | Formats a long value in decimal notation.  |
| `void` | [`format`](#format-13)  | Formats a long value in decimal notation, right justified in a field having at least the specified width.  |
| `void` | [`format0`](#format0-2)  | Formats a long value in decimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`formatHex`](#formathex-4)  | Formats a long value in hexadecimal notation. The value is treated as unsigned.  |
| `void` | [`formatHex`](#formathex-5)  | Formats a long value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. The value is treated as unsigned.  |
| `void` | [`format`](#format-14)  | Formats an unsigned long value in decimal notation.  |
| `void` | [`format`](#format-15)  | Formats an unsigned long value in decimal notation, right justified in a field having at least the specified width.  |
| `void` | [`format0`](#format0-3)  | Formats an unsigned long value in decimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`formatHex`](#formathex-6)  | Formats an unsigned long value in hexadecimal notation.  |
| `void` | [`formatHex`](#formathex-7)  | Formats an unsigned long value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`format`](#format-16)  | Formats a 64-bit integer value in decimal notation.  |
| `void` | [`format`](#format-17)  | Formats a 64-bit integer value in decimal notation, right justified in a field having at least the specified width.  |
| `void` | [`format0`](#format0-4)  | Formats a 64-bit integer value in decimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`formatHex`](#formathex-8)  | Formats a 64-bit integer value in hexadecimal notation. The value is treated as unsigned.  |
| `void` | [`formatHex`](#formathex-9)  | Formats a 64-bit integer value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. The value is treated as unsigned.  |
| `void` | [`format`](#format-18)  | Formats an unsigned 64-bit integer value in decimal notation.  |
| `void` | [`format`](#format-19)  | Formats an unsigned 64-bit integer value in decimal notation, right justified in a field having at least the specified width.  |
| `void` | [`format0`](#format0-5)  | Formats an unsigned 64-bit integer value in decimal notation, right justified and zero-padded in a field having at least the specified width.  |
| `void` | [`formatHex`](#formathex-10)  | Formats an unsigned 64-bit integer value in hexadecimal notation.  |
| `void` | [`formatHex`](#formathex-11)  | Formats an unsigned 64-bit integer value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width.  |

---

{#format-8}

#### format

```cpp
void format(std::string & str, int value)
```

Formats an integer value in decimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `int` |  |

---

{#format-9}

#### format

```cpp
void format(std::string & str, int value, int width)
```

Formats an integer value in decimal notation, right justified in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with spaces on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `int` |  |
| `width` | `int` |  |

---

{#format0}

#### format0

```cpp
void format0(std::string & str, int value, int width)
```

Formats an integer value in decimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `int` |  |
| `width` | `int` |  |

---

{#formathex}

#### formatHex

```cpp
void formatHex(std::string & str, int value)
```

Formats an int value in hexadecimal notation. The value is treated as unsigned. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `int` |  |

---

{#formathex-1}

#### formatHex

```cpp
void formatHex(std::string & str, int value, int width)
```

Formats an int value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. The value is treated as unsigned. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `int` |  |
| `width` | `int` |  |

---

{#format-10}

#### format

```cpp
void format(std::string & str, unsigned value)
```

Formats an unsigned int value in decimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned` |  |

---

{#format-11}

#### format

```cpp
void format(std::string & str, unsigned value, int width)
```

Formats an unsigned int value in decimal notation, right justified in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with spaces on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned` |  |
| `width` | `int` |  |

---

{#format0-1}

#### format0

```cpp
void format0(std::string & str, unsigned int value, int width)
```

Formats an unsigned int value in decimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned int` |  |
| `width` | `int` |  |

---

{#formathex-2}

#### formatHex

```cpp
void formatHex(std::string & str, unsigned value)
```

Formats an unsigned int value in hexadecimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned` |  |

---

{#formathex-3}

#### formatHex

```cpp
void formatHex(std::string & str, unsigned value, int width)
```

Formats an unsigned int value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned` |  |
| `width` | `int` |  |

---

{#format-12}

#### format

```cpp
void format(std::string & str, long value)
```

Formats a long value in decimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `long` |  |

---

{#format-13}

#### format

```cpp
void format(std::string & str, long value, int width)
```

Formats a long value in decimal notation, right justified in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with spaces on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `long` |  |
| `width` | `int` |  |

---

{#format0-2}

#### format0

```cpp
void format0(std::string & str, long value, int width)
```

Formats a long value in decimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `long` |  |
| `width` | `int` |  |

---

{#formathex-4}

#### formatHex

```cpp
void formatHex(std::string & str, long value)
```

Formats a long value in hexadecimal notation. The value is treated as unsigned. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `long` |  |

---

{#formathex-5}

#### formatHex

```cpp
void formatHex(std::string & str, long value, int width)
```

Formats a long value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. The value is treated as unsigned. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `long` |  |
| `width` | `int` |  |

---

{#format-14}

#### format

```cpp
void format(std::string & str, unsigned long value)
```

Formats an unsigned long value in decimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned long` |  |

---

{#format-15}

#### format

```cpp
void format(std::string & str, unsigned long value, int width)
```

Formats an unsigned long value in decimal notation, right justified in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with spaces on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned long` |  |
| `width` | `int` |  |

---

{#format0-3}

#### format0

```cpp
void format0(std::string & str, unsigned long value, int width)
```

Formats an unsigned long value in decimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned long` |  |
| `width` | `int` |  |

---

{#formathex-6}

#### formatHex

```cpp
void formatHex(std::string & str, unsigned long value)
```

Formats an unsigned long value in hexadecimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned long` |  |

---

{#formathex-7}

#### formatHex

```cpp
void formatHex(std::string & str, unsigned long value, int width)
```

Formats an unsigned long value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `unsigned long` |  |
| `width` | `int` |  |

---

{#format-16}

#### format

```cpp
void format(std::string & str, std::int64_t value)
```

Formats a 64-bit integer value in decimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `std::int64_t` |  |

---

{#format-17}

#### format

```cpp
void format(std::string & str, std::int64_t value, int width)
```

Formats a 64-bit integer value in decimal notation, right justified in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with spaces on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `std::int64_t` |  |
| `width` | `int` |  |

---

{#format0-4}

#### format0

```cpp
void format0(std::string & str, std::int64_t value, int width)
```

Formats a 64-bit integer value in decimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `std::int64_t` |  |
| `width` | `int` |  |

---

{#formathex-8}

#### formatHex

```cpp
void formatHex(std::string & str, std::int64_t value)
```

Formats a 64-bit integer value in hexadecimal notation. The value is treated as unsigned. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `std::int64_t` |  |

---

{#formathex-9}

#### formatHex

```cpp
void formatHex(std::string & str, std::int64_t value, int width)
```

Formats a 64-bit integer value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. The value is treated as unsigned. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `std::int64_t` |  |
| `width` | `int` |  |

---

{#format-18}

#### format

```cpp
void format(std::string & str, uint64_t value)
```

Formats an unsigned 64-bit integer value in decimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `uint64_t` |  |

---

{#format-19}

#### format

```cpp
void format(std::string & str, uint64_t value, int width)
```

Formats an unsigned 64-bit integer value in decimal notation, right justified in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with spaces on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `uint64_t` |  |
| `width` | `int` |  |

---

{#format0-5}

#### format0

```cpp
void format0(std::string & str, uint64_t value, int width)
```

Formats an unsigned 64-bit integer value in decimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `uint64_t` |  |
| `width` | `int` |  |

---

{#formathex-10}

#### formatHex

```cpp
void formatHex(std::string & str, uint64_t value)
```

Formats an unsigned 64-bit integer value in hexadecimal notation. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `uint64_t` |  |

---

{#formathex-11}

#### formatHex

```cpp
void formatHex(std::string & str, uint64_t value, int width)
```

Formats an unsigned 64-bit integer value in hexadecimal notation, right justified and zero-padded in a field having at least the specified width. 
#### Parameters
* `str` Output string to append to. 

* `value` Value to format. 

* `width` Minimum field width; padded with zeros on the left.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `value` | `uint64_t` |  |
| `width` | `int` |  |

{#basic}

# basic

Interface classes.

### Classes

| Name | Description |
|------|-------------|
| [`Decoder`](#decoder-4) | Abstract interface for stream decoders. |
| [`Encoder`](#encoder-5) | Abstract interface for stream encoders. |
| [`Runnable`](#runnable) | Abstract interface for classes that can be run and cancelled. |
| [`Sendable`](#sendable) | Abstract interface for classes that can be sent and cancelled. |
| [`Startable`](#startable) | Abstract interface for a classes that can be started and stopped. |

{#decoder-4}

## Decoder

```cpp
#include <interface.h>
```

> **Subclassed by:** [`Decoder`](#decoder-2), [`Decoder`](#decoder)

Abstract interface for stream decoders.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Decoder`](#decoder-5)  |  |
| `ssize_t` | [`decode`](#decode-9)  | Decodes nread bytes from inbuf and writes decoded output to outbuf.  |
| `ssize_t` | [`finalize`](#finalize-4) `virtual` `inline` | Flushes any buffered state and writes final output to outbuf.  |

---

{#decoder-5}

#### Decoder

```cpp
Decoder() = default
```

---

{#decode-9}

#### decode

```cpp
ssize_t decode(const char * inbuf, size_t nread, char * outbuf)
```

Decodes nread bytes from inbuf and writes decoded output to outbuf. 
#### Parameters
* `inbuf` Encoded input buffer. 

* `nread` Number of bytes to decode from inbuf. 

* `outbuf` Destination buffer for decoded output. 

#### Returns
Number of bytes written to outbuf, or -1 on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

{#finalize-4}

#### finalize

`virtual` `inline`

```cpp
virtual inline ssize_t finalize(char * outbuf)
```

Flushes any buffered state and writes final output to outbuf. 
#### Parameters
* `outbuf` Destination buffer for any remaining output. 

#### Returns
Number of bytes written, or 0 if nothing to flush.

| Parameter | Type | Description |
|-----------|------|-------------|
| `outbuf` | `char *` |  |

{#encoder-5}

## Encoder

```cpp
#include <interface.h>
```

> **Subclassed by:** [`Encoder`](#encoder-3), [`Encoder`](#encoder-1)

Abstract interface for stream encoders.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Encoder`](#encoder-6)  |  |
| `ssize_t` | [`encode`](#encode-20)  | Encodes nread bytes from inbuf and writes encoded output to outbuf.  |
| `ssize_t` | [`finalize`](#finalize-5) `virtual` `inline` | Flushes any buffered state and writes final output to outbuf.  |

---

{#encoder-6}

#### Encoder

```cpp
Encoder() = default
```

---

{#encode-20}

#### encode

```cpp
ssize_t encode(const char * inbuf, size_t nread, char * outbuf)
```

Encodes nread bytes from inbuf and writes encoded output to outbuf. 
#### Parameters
* `inbuf` Raw input buffer to encode. 

* `nread` Number of bytes to encode from inbuf. 

* `outbuf` Destination buffer for encoded output. 

#### Returns
Number of bytes written to outbuf, or -1 on error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

{#finalize-5}

#### finalize

`virtual` `inline`

```cpp
virtual inline ssize_t finalize(char * outbuf)
```

Flushes any buffered state and writes final output to outbuf. 
#### Parameters
* `outbuf` Destination buffer for any remaining output. 

#### Returns
Number of bytes written, or 0 if nothing to flush.

| Parameter | Type | Description |
|-----------|------|-------------|
| `outbuf` | `char *` |  |

{#runnable}

## Runnable

```cpp
#include <interface.h>
```

> **Subclassed by:** [`RunnableQueue< IPacket >`](#classicy_1_1RunnableQueue), [`RunnableQueue< PacketT >`](#classicy_1_1RunnableQueue), [`AsyncDiagnostic`](#classicy_1_1AsyncDiagnostic), [`AsyncLogWriter`](#classicy_1_1AsyncLogWriter), [`RunnableQueue< T >`](#classicy_1_1RunnableQueue), [`Task`](#classicy_1_1Task), [`TaskRunner`](#classicy_1_1TaskRunner), [`MediaCapture`](#mediacapture), [`InstallTask`](#installtask)

Abstract interface for classes that can be run and cancelled.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Runnable`](#runnable-1) `inline` |  |
| `void` | [`run`](#run-4)  | The run method will be called by the asynchronous context. |
| `void` | [`cancel`](#cancel-1) `virtual` `inline` | Cancel the current task. The [run()](#run-4) method should return ASAP. |
| `bool` | [`cancelled`](#cancelled) `virtual` `const` `inline` | Returns true when the task has been cancelled. |

---

{#runnable-1}

#### Runnable

`inline`

```cpp
inline Runnable()
```

---

{#run-4}

#### run

```cpp
void run()
```

The run method will be called by the asynchronous context.

---

{#cancel-1}

#### cancel

`virtual` `inline`

```cpp
virtual inline void cancel(bool flag)
```

Cancel the current task. The [run()](#run-4) method should return ASAP.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

{#cancelled}

#### cancelled

`virtual` `const` `inline`

```cpp
virtual inline bool cancelled() const
```

Returns true when the task has been cancelled.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::atomic< bool >` | [`exit`](#exit)  |  |

---

{#exit}

#### exit

```cpp
std::atomic< bool > exit
```

{#sendable}

## Sendable

```cpp
#include <interface.h>
```

> **Subclassed by:** [`PacketTransaction< Message >`](#classicy_1_1PacketTransaction), [`PacketTransaction< PacketT >`](#classicy_1_1PacketTransaction)

Abstract interface for classes that can be sent and cancelled.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`send`](#send-9)  | Initiates the send operation.  |
| `void` | [`cancel`](#cancel-2)  | Cancels a pending send operation. |

---

{#send-9}

#### send

```cpp
bool send()
```

Initiates the send operation. 
#### Returns
true if the send was dispatched successfully, false otherwise.

---

{#cancel-2}

#### cancel

```cpp
void cancel()
```

Cancels a pending send operation.

{#startable}

## Startable

```cpp
#include <interface.h>
```

> **Subclassed by:** [`ThreadedStreamReader`](#classicy_1_1ThreadedStreamReader), [`ICapture`](#icapture), [`FormWriter`](#formwriter)

Abstract interface for a classes that can be started and stopped.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`start`](#start-7)  | Starts the object (e.g. begins processing or listening). |
| `void` | [`stop`](#stop-6)  | Stops the object (e.g. halts processing or closes resources). |

---

{#start-7}

#### start

```cpp
void start()
```

Starts the object (e.g. begins processing or listening).

---

{#stop-6}

#### stop

```cpp
void stop()
```

Stops the object (e.g. halts processing or closes resources).

{#fs}

# fs

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`filename`](#filename)  | Returns the file name and extension part of the given path.  |
| `std::string` | [`basename`](#basename)  | Returns the file name without its extension.  |
| `std::string` | [`dirname`](#dirname)  | Returns the directory part of the path.  |
| `std::string` | [`extname`](#extname)  | Returns the file extension part of the path.  |
| `bool` | [`exists`](#exists-1)  | Returns true if the file or directory exists.  |
| `bool` | [`isdir`](#isdir)  | Returns true if the path refers to a directory.  |
| `std::int64_t` | [`filesize`](#filesize)  | Returns the size in bytes of the given file.  |
| `void` | [`readdir`](#readdir)  | Populates `res` with the names of all entries in the given directory.  |
| `void` | [`mkdir`](#mkdir)  | Creates a single directory.  |
| `void` | [`mkdirr`](#mkdirr)  | Creates a directory and all missing parent directories.  |
| `void` | [`rmdir`](#rmdir)  | Removes an empty directory.  |
| `void` | [`unlink`](#unlink)  | Deletes a file.  |
| `void` | [`rename`](#rename)  | Renames or moves the given file to the target path.  |
| `void` | [`addsep`](#addsep)  | Appends the platform-specific path separator to `path` if not already present.  |
| `void` | [`addnode`](#addnode)  | Appends a path node to `path`, inserting a separator if necessary.  |
| `std::string` | [`makePath`](#makepath)  | Joins a base path and a node component into a single path string.  |
| `std::string` | [`normalize`](#normalize)  | Normalizes a path by resolving `.` and `..` segments and converting separators to the native platform style.  |
| `std::string` | [`transcode`](#transcode)  | Transcodes a path to the native platform format. On Windows with `ICY_UNICODE` defined, converts to the Windows-native wide-to-narrow format. On other platforms, returns the path unchanged.  |
| `bool` | [`savefile`](#savefile)  | Writes `size` bytes from `data` to the file at `path`, creating or overwriting it.  |

---

{#filename}

#### filename

```cpp
std::string filename(std::string_view path)
```

Returns the file name and extension part of the given path. 
#### Parameters
* `path` Filesystem path to parse. 

#### Returns
Filename component including extension (e.g. "file.txt").

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#basename}

#### basename

```cpp
std::string basename(std::string_view path)
```

Returns the file name without its extension. 
#### Parameters
* `path` Filesystem path to parse. 

#### Returns
Filename without the extension (e.g. "file").

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#dirname}

#### dirname

```cpp
std::string dirname(std::string_view path)
```

Returns the directory part of the path. 
#### Parameters
* `path` Filesystem path to parse. 

#### Returns
Directory component including trailing separator (e.g. "/usr/local/").

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#extname}

#### extname

```cpp
std::string extname(std::string_view path, bool includeDot)
```

Returns the file extension part of the path. 
#### Parameters
* `path` Filesystem path to parse. 

* `includeDot` If true (default), includes the leading dot (e.g. ".txt"); if false, the dot is stripped. 

#### Returns
Extension string, or empty if the path has no extension.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |
| `includeDot` | `bool` |  |

---

{#exists-1}

#### exists

```cpp
bool exists(std::string_view path)
```

Returns true if the file or directory exists. 
#### Parameters
* `path` Path to check. 

#### Returns
True if the path exists on the filesystem.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#isdir}

#### isdir

```cpp
bool isdir(std::string_view path)
```

Returns true if the path refers to a directory. 
#### Parameters
* `path` Path to check. 

#### Returns
True if the path exists and is a directory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#filesize}

#### filesize

```cpp
std::int64_t filesize(std::string_view path)
```

Returns the size in bytes of the given file. 
#### Parameters
* `path` Path to the file. 

#### Returns
File size in bytes, or -1 if the file does not exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#readdir}

#### readdir

```cpp
void readdir(std::string_view path, std::vector< std::string > & res)
```

Populates `res` with the names of all entries in the given directory. 
#### Parameters
* `path` Path to the directory to read. 

* `res` Vector to receive the list of entry names.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |
| `res` | `std::vector< std::string > &` |  |

---

{#mkdir}

#### mkdir

```cpp
void mkdir(std::string_view path, int mode)
```

Creates a single directory. 
#### Parameters
* `path` Path of the directory to create. 

* `mode` Permission bits (default: 0755). Ignored on Windows.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |
| `mode` | `int` |  |

---

{#mkdirr}

#### mkdirr

```cpp
void mkdirr(std::string_view path, int mode)
```

Creates a directory and all missing parent directories. 
#### Parameters
* `path` Path of the directory hierarchy to create. 

* `mode` Permission bits (default: 0755). Ignored on Windows.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |
| `mode` | `int` |  |

---

{#rmdir}

#### rmdir

```cpp
void rmdir(std::string_view path)
```

Removes an empty directory. 
#### Parameters
* `path` Path of the directory to remove.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#unlink}

#### unlink

```cpp
void unlink(std::string_view path)
```

Deletes a file. 
#### Parameters
* `path` Path of the file to delete.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#rename}

#### rename

```cpp
void rename(std::string_view path, std::string_view target)
```

Renames or moves the given file to the target path. 
#### Parameters
* `path` Source file path. 

* `target` Destination file path.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |
| `target` | `std::string_view` |  |

---

{#addsep}

#### addsep

```cpp
void addsep(std::string & path)
```

Appends the platform-specific path separator to `path` if not already present. 
#### Parameters
* `path` Path string to modify in place.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string &` |  |

---

{#addnode}

#### addnode

```cpp
void addnode(std::string & path, std::string_view node)
```

Appends a path node to `path`, inserting a separator if necessary. 
#### Parameters
* `path` Base path string to modify in place. 

* `node` Directory or file name component to append.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string &` |  |
| `node` | `std::string_view` |  |

---

{#makepath}

#### makePath

```cpp
std::string makePath(std::string_view base, std::string_view node)
```

Joins a base path and a node component into a single path string. 
#### Parameters
* `base` Base directory path. 

* `node` Directory or file name component to append. 

#### Returns
Joined path string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `base` | `std::string_view` |  |
| `node` | `std::string_view` |  |

---

{#normalize}

#### normalize

```cpp
std::string normalize(std::string_view path)
```

Normalizes a path by resolving `.` and `..` segments and converting separators to the native platform style. 
#### Parameters
* `path` Path string to normalize. 

#### Returns
Normalized path string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#transcode}

#### transcode

```cpp
std::string transcode(std::string_view path)
```

Transcodes a path to the native platform format. On Windows with `ICY_UNICODE` defined, converts to the Windows-native wide-to-narrow format. On other platforms, returns the path unchanged. 
#### Parameters
* `path` Path string to transcode. 

#### Returns
Transcoded path string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |

---

{#savefile}

#### savefile

```cpp
bool savefile(std::string_view path, const char * data, size_t size, bool whiny)
```

Writes `size` bytes from `data` to the file at `path`, creating or overwriting it. 
#### Parameters
* `path` Destination file path. Parent directories must already exist. 

* `data` Pointer to the data to write. 

* `size` Number of bytes to write. 

* `whiny` If true, throws a `std::runtime_error` on failure instead of returning false. 

#### Returns
True on success, false on failure (when `whiny` is false).

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string_view` |  |
| `data` | `const char *` |  |
| `size` | `size_t` |  |
| `whiny` | `bool` |  |

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`separator`](#separator)  | The platform specific path separator string: "/" on unix and "\\" on windows. |
| `const char` | [`delimiter`](#delimiter)  | The platform specific path separator character: '/' on unix and '\' on windows. |

---

{#separator}

#### separator

```cpp
const char * separator
```

The platform specific path separator string: "/" on unix and "\\" on windows.

---

{#delimiter}

#### delimiter

```cpp
const char delimiter
```

The platform specific path separator character: '/' on unix and '\' on windows.

{#util}

# util

### Classes

| Name | Description |
|------|-------------|
| [`Version`](#version) | Semantic version number with major, minor, and patch fields. |

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`format`](#format-20)  | Printf-style string formatting for POD types.  |
| `void` | [`toUnderscore`](#tounderscore)  | Replaces all non-alphanumeric characters in str with underscores and converts to lowercase.  |
| `bool` | [`isNumber`](#isnumber)  | Returns true if str consists entirely of digit characters.  |
| `bool` | [`endsWith`](#endswith)  | Returns true if str ends with the given suffix.  |
| `void` | [`removeSpecialCharacters`](#removespecialcharacters)  | Replaces non-alphanumeric characters. Removes all non-alphanumeric characters from str in place.  |
| `void` | [`replaceSpecialCharacters`](#replacespecialcharacters)  | Replaces all non-alphanumeric characters in str with `with` in place.  |
| `bool` | [`tryParseHex`](#tryparsehex)  | Attempts to parse a hex string into an unsigned integer.  |
| `unsigned` | [`parseHex`](#parsehex)  | Parses a hex string into an unsigned integer.  |
| `std::string` | [`dumpbin`](#dumpbin)  | Formats the binary contents of data as a hex+ASCII dump string.  |
| `bool` | [`compareVersion`](#compareversion)  | Compares two dot-separated version strings.  |
| `bool` | [`matchNodes`](#matchnodes)  | Checks whether node matches xnode by splitting both on delim and comparing element-wise.  |
| `bool` | [`matchNodes`](#matchnodes-1)  | Checks whether params matches xparams element-wise.  |
| `std::string` | [`memAddress`](#memaddress)  | Returns the memory address of ptr as a hex string (e.g. "0x7f3a2b10c0").  |
| `std::string` | [`itostr`](#itostr)  | Converts an integer (or any stream-insertable type) to its string representation.  |
| `T` | [`strtoi`](#strtoi)  | Parses a string into integer type T using std::istringstream. Returns 0 if parsing fails. Ensure T has sufficient range for the value.  |
| `uint32_t` | [`randomNumber`](#randomnumber)  | Generates a 31-bit pseudo random number using the internal [Random](#classicy_1_1Random) instance.  |
| `std::string` | [`randomString`](#randomstring)  | Generates a random alphanumeric string of the given length.  |
| `std::string` | [`randomBinaryString`](#randombinarystring)  | Generates a random binary string of the given byte length.  |
| `void` | [`split`](#split)  | Splits str on the delimiter string and appends tokens to elems.  |
| `std::vector< std::string >` | [`split`](#split-1)  | Splits str on the delimiter string and returns the tokens as a vector.  |
| `void` | [`split`](#split-2)  | Splits str on the delimiter character and appends tokens to elems.  |
| `std::vector< std::string >` | [`split`](#split-3)  | Splits str on the delimiter character and returns the tokens as a vector.  |
| `S &` | [`replaceInPlace`](#replaceinplace)  | Replace all occurrences of `from` in `str` with `to`, starting at position `start`. Modifies and returns `str` in place. `from` must not be empty. |
| `S &` | [`replaceInPlace`](#replaceinplace-1)  | Replace all occurrences of `from` in `str` with `to`, starting at position `start`. C-string overload. Modifies and returns `str` in place. |
| `S` | [`replace`](#replace)  | Replace all occurences of from (which must not be the empty string) in str with to, starting at position start. |
| `S` | [`replace`](#replace-1)  | Returns a copy of str with all occurrences of from replaced by to (C-string overload).  |
| `S` | [`trimLeft`](#trimleft)  | Returns a copy of str with all leading whitespace removed. |
| `S &` | [`trimLeftInPlace`](#trimleftinplace)  | Removes all leading whitespace in str. |
| `S` | [`trimRight`](#trimright)  | Returns a copy of str with all trailing whitespace removed. |
| `S &` | [`trimRightInPlace`](#trimrightinplace)  | Removes all trailing whitespace in str. |
| `S` | [`trim`](#trim)  | Returns a copy of str with all leading and trailing whitespace removed. |
| `S &` | [`trimInPlace`](#triminplace)  | Removes all leading and trailing whitespace in str. |
| `S` | [`toUpper`](#toupper)  | Returns a copy of str containing all upper-case characters. |
| `S &` | [`toUpperInPlace`](#toupperinplace)  | Replaces all characters in str with their upper-case counterparts. |
| `S` | [`toLower`](#tolower)  | Returns a copy of str containing all lower-case characters. |
| `S &` | [`toLowerInPlace`](#tolowerinplace)  | Replaces all characters in str with their lower-case counterparts. |
| `int` | [`icompare`](#icompare) `inline` | Case-insensitive string comparison (locale-independent, ASCII only).  |
| `std::streamsize` | [`copyStreamUnbuffered`](#copystreamunbuffered)  | Copies all bytes from istr to ostr one byte at a time (no internal buffer).  |
| `std::streamsize` | [`copyStream`](#copystream)  | Copies all bytes from istr to ostr using an internal buffer.  |
| `std::streamsize` | [`copyToString`](#copytostring)  | Reads all bytes from istr and appends them to str.  |
| `void` | [`clearList`](#clearlist) `inline` | Delete all elements from a list of pointers. |
| `void` | [`clearDeque`](#cleardeque) `inline` | Delete all elements from a deque of pointers. |
| `void` | [`clearVector`](#clearvector) `inline` | Delete all elements from a vector of pointers. |
| `void` | [`clearMap`](#clearmap) `inline` | Delete all associated values from a map (not the key elements). |

---

{#format-20}

#### format

```cpp
std::string format(const char * fmt, ...)
```

Printf-style string formatting for POD types. 
#### Parameters
* `fmt` printf format string. 

* `...` Format arguments. 

#### Returns
Formatted string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fmt` | `const char *` |  |

---

{#tounderscore}

#### toUnderscore

```cpp
void toUnderscore(std::string & str)
```

Replaces all non-alphanumeric characters in str with underscores and converts to lowercase. 
#### Parameters
* `str` String to transform in place.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |

---

{#isnumber}

#### isNumber

```cpp
bool isNumber(std::string_view str)
```

Returns true if str consists entirely of digit characters. 
#### Parameters
* `str` String to test. 

#### Returns
true if every character in str is a decimal digit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |

---

{#endswith}

#### endsWith

```cpp
bool endsWith(std::string_view str, std::string_view suffix)
```

Returns true if str ends with the given suffix. 
#### Parameters
* `str` String to test. 

* `suffix` Suffix to look for. 

#### Returns
true if str ends with suffix.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |
| `suffix` | `std::string_view` |  |

---

{#removespecialcharacters}

#### removeSpecialCharacters

```cpp
void removeSpecialCharacters(std::string & str, bool allowSpaces)
```

Replaces non-alphanumeric characters. Removes all non-alphanumeric characters from str in place. 
#### Parameters
* `str` String to modify. 

* `allowSpaces` If true, ASCII spaces are preserved.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `allowSpaces` | `bool` |  |

---

{#replacespecialcharacters}

#### replaceSpecialCharacters

```cpp
void replaceSpecialCharacters(std::string & str, char with, bool allowSpaces)
```

Replaces all non-alphanumeric characters in str with `with` in place. 
#### Parameters
* `str` String to modify. 

* `with` Replacement character (default: '_'). 

* `allowSpaces` If true, ASCII spaces are preserved rather than replaced.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `with` | `char` |  |
| `allowSpaces` | `bool` |  |

---

{#tryparsehex}

#### tryParseHex

```cpp
bool tryParseHex(std::string_view s, unsigned & value)
```

Attempts to parse a hex string into an unsigned integer. 
#### Parameters
* `s` Hex string (with or without 0x prefix). 

* `value` Output: parsed value on success. 

#### Returns
true if parsing succeeded, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `std::string_view` |  |
| `value` | `unsigned &` |  |

---

{#parsehex}

#### parseHex

```cpp
unsigned parseHex(std::string_view s)
```

Parses a hex string into an unsigned integer. 
#### Parameters
* `s` Hex string (with or without 0x prefix). 

#### Returns
Parsed value. 

#### Exceptions
* `std::invalid_argument` if the string is not valid hex.

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `std::string_view` |  |

---

{#dumpbin}

#### dumpbin

```cpp
std::string dumpbin(const char * data, size_t len)
```

Formats the binary contents of data as a hex+ASCII dump string. 
#### Parameters
* `data` Pointer to the buffer to dump. 

* `len` Number of bytes to dump. 

#### Returns
Multi-line hex dump string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

{#compareversion}

#### compareVersion

```cpp
bool compareVersion(std::string_view l, std::string_view r)
```

Compares two dot-separated version strings. 
#### Parameters
* `l` Left (local) version string. 

* `r` Right (remote) version string. 

#### Returns
true if l is strictly greater than r, false if l is equal or less.

| Parameter | Type | Description |
|-----------|------|-------------|
| `l` | `std::string_view` |  |
| `r` | `std::string_view` |  |

---

{#matchnodes}

#### matchNodes

```cpp
bool matchNodes(std::string_view node, std::string_view xnode, std::string_view delim)
```

Checks whether node matches xnode by splitting both on delim and comparing element-wise. 
#### Parameters
* `node` Node list string to test. 

* `xnode` Expected node list pattern. 

* `delim` Delimiter used to split both strings (default: "\r\n"). 

#### Returns
true if all elements of node match the corresponding elements of xnode.

| Parameter | Type | Description |
|-----------|------|-------------|
| `node` | `std::string_view` |  |
| `xnode` | `std::string_view` |  |
| `delim` | `std::string_view` |  |

---

{#matchnodes-1}

#### matchNodes

```cpp
bool matchNodes(const std::vector< std::string > & params, const std::vector< std::string > & xparams)
```

Checks whether params matches xparams element-wise. 
#### Parameters
* `params` Parameter list to test. 

* `xparams` Expected parameter list. 

#### Returns
true if every element of params matches the corresponding element of xparams.

| Parameter | Type | Description |
|-----------|------|-------------|
| `params` | `const std::vector< std::string > &` |  |
| `xparams` | `const std::vector< std::string > &` |  |

---

{#memaddress}

#### memAddress

```cpp
std::string memAddress(const void * ptr)
```

Returns the memory address of ptr as a hex string (e.g. "0x7f3a2b10c0"). 
#### Parameters
* `ptr` Pointer whose address to format. 

#### Returns
Hex string representation of the pointer value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `const void *` |  |

---

{#itostr}

#### itostr

```cpp
template<typename T> std::string itostr(const T & t)
```

Converts an integer (or any stream-insertable type) to its string representation. 
#### Parameters
* `T` Type to convert; must support `operator<<` on std::ostream. 

#### Parameters
* `t` Value to convert. 

#### Returns
String representation of t.

| Parameter | Type | Description |
|-----------|------|-------------|
| `t` | `const T &` |  |

---

{#strtoi}

#### strtoi

```cpp
template<typename T> T strtoi(std::string_view s)
```

Parses a string into integer type T using std::istringstream. Returns 0 if parsing fails. Ensure T has sufficient range for the value. 
#### Parameters
* `T` Target integer type. 

#### Parameters
* `s` String to parse. 

#### Returns
Parsed value, or 0 on failure.

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `std::string_view` |  |

---

{#randomnumber}

#### randomNumber

```cpp
uint32_t randomNumber()
```

Generates a 31-bit pseudo random number using the internal [Random](#classicy_1_1Random) instance. 
#### Returns
Pseudo random uint32_t value.

---

{#randomstring}

#### randomString

```cpp
std::string randomString(int size)
```

Generates a random alphanumeric string of the given length. 
#### Parameters
* `size` Number of characters to generate. 

#### Returns
[Random](#classicy_1_1Random) string of length size.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `int` |  |

---

{#randombinarystring}

#### randomBinaryString

```cpp
std::string randomBinaryString(int size, bool doBase64)
```

Generates a random binary string of the given byte length. 
#### Parameters
* `size` Number of random bytes to generate. 

* `doBase64` If true, returns the bytes as a base64-encoded string. 

#### Returns
[Random](#classicy_1_1Random) binary string, optionally base64-encoded.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `int` |  |
| `doBase64` | `bool` |  |

---

{#split}

#### split

```cpp
void split(std::string_view str, std::string_view delim, std::vector< std::string > & elems, int limit)
```

Splits str on the delimiter string and appends tokens to elems. 
#### Parameters
* `str` String to split. 

* `delim` Delimiter string. 

* `elems` Output vector; tokens are appended to it. 

* `limit` Maximum number of splits (-1 for unlimited).

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |
| `delim` | `std::string_view` |  |
| `elems` | `std::vector< std::string > &` |  |
| `limit` | `int` |  |

---

{#split-1}

#### split

```cpp
std::vector< std::string > split(std::string_view str, std::string_view delim, int limit)
```

Splits str on the delimiter string and returns the tokens as a vector. 
#### Parameters
* `str` String to split. 

* `delim` Delimiter string. 

* `limit` Maximum number of splits (-1 for unlimited). 

#### Returns
Vector of token strings.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |
| `delim` | `std::string_view` |  |
| `limit` | `int` |  |

---

{#split-2}

#### split

```cpp
void split(std::string_view str, char delim, std::vector< std::string > & elems, int limit)
```

Splits str on the delimiter character and appends tokens to elems. 
#### Parameters
* `str` String to split. 

* `delim` Delimiter character. 

* `elems` Output vector; tokens are appended to it. 

* `limit` Maximum number of splits (-1 for unlimited).

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |
| `delim` | `char` |  |
| `elems` | `std::vector< std::string > &` |  |
| `limit` | `int` |  |

---

{#split-3}

#### split

```cpp
std::vector< std::string > split(std::string_view str, char delim, int limit)
```

Splits str on the delimiter character and returns the tokens as a vector. 
#### Parameters
* `str` String to split. 

* `delim` Delimiter character. 

* `limit` Maximum number of splits (-1 for unlimited). 

#### Returns
Vector of token strings.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string_view` |  |
| `delim` | `char` |  |
| `limit` | `int` |  |

---

{#replaceinplace}

#### replaceInPlace

```cpp
template<class S> S & replaceInPlace(S & str, const S & from, const S & to, typename S::size_type start)
```

Replace all occurrences of `from` in `str` with `to`, starting at position `start`. Modifies and returns `str` in place. `from` must not be empty.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |
| `from` | `const S &` |  |
| `to` | `const S &` |  |
| `start` | `typename S::size_type` |  |

---

{#replaceinplace-1}

#### replaceInPlace

```cpp
template<class S> S & replaceInPlace(S & str, const typename S::value_type * from, const typename S::value_type * to, typename S::size_type start)
```

Replace all occurrences of `from` in `str` with `to`, starting at position `start`. C-string overload. Modifies and returns `str` in place.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |
| `from` | `const typename S::value_type *` |  |
| `to` | `const typename S::value_type *` |  |
| `start` | `typename S::size_type` |  |

---

{#replace}

#### replace

```cpp
template<class S> S replace(const S & str, const S & from, const S & to, typename S::size_type start)
```

Replace all occurences of from (which must not be the empty string) in str with to, starting at position start.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |
| `from` | `const S &` |  |
| `to` | `const S &` |  |
| `start` | `typename S::size_type` |  |

---

{#replace-1}

#### replace

```cpp
template<class S> S replace(const S & str, const typename S::value_type * from, const typename S::value_type * to, typename S::size_type start)
```

Returns a copy of str with all occurrences of from replaced by to (C-string overload). 
#### Parameters
* `str` Source string. 

* `from` Substring to search for; must not be empty. 

* `to` Replacement string. 

* `start` Position in str at which to begin searching (default: 0). 

#### Returns
New string with all replacements applied.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |
| `from` | `const typename S::value_type *` |  |
| `to` | `const typename S::value_type *` |  |
| `start` | `typename S::size_type` |  |

---

{#trimleft}

#### trimLeft

```cpp
template<class S> S trimLeft(const S & str)
```

Returns a copy of str with all leading whitespace removed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |

---

{#trimleftinplace}

#### trimLeftInPlace

```cpp
template<class S> S & trimLeftInPlace(S & str)
```

Removes all leading whitespace in str.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |

---

{#trimright}

#### trimRight

```cpp
template<class S> S trimRight(const S & str)
```

Returns a copy of str with all trailing whitespace removed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |

---

{#trimrightinplace}

#### trimRightInPlace

```cpp
template<class S> S & trimRightInPlace(S & str)
```

Removes all trailing whitespace in str.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |

---

{#trim}

#### trim

```cpp
template<class S> S trim(const S & str)
```

Returns a copy of str with all leading and trailing whitespace removed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |

---

{#triminplace}

#### trimInPlace

```cpp
template<class S> S & trimInPlace(S & str)
```

Removes all leading and trailing whitespace in str.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |

---

{#toupper}

#### toUpper

```cpp
template<class S> S toUpper(const S & str)
```

Returns a copy of str containing all upper-case characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |

---

{#toupperinplace}

#### toUpperInPlace

```cpp
template<class S> S & toUpperInPlace(S & str)
```

Replaces all characters in str with their upper-case counterparts.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |

---

{#tolower}

#### toLower

```cpp
template<class S> S toLower(const S & str)
```

Returns a copy of str containing all lower-case characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const S &` |  |

---

{#tolowerinplace}

#### toLowerInPlace

```cpp
template<class S> S & toLowerInPlace(S & str)
```

Replaces all characters in str with their lower-case counterparts.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `S &` |  |

---

{#icompare}

#### icompare

`inline`

```cpp
inline int icompare(std::string_view a, std::string_view b)
```

Case-insensitive string comparison (locale-independent, ASCII only). 
#### Parameters
* `a` First string. 

* `b` Second string. 

#### Returns
Negative if a < b, zero if a == b, positive if a > b.

| Parameter | Type | Description |
|-----------|------|-------------|
| `a` | `std::string_view` |  |
| `b` | `std::string_view` |  |

---

{#copystreamunbuffered}

#### copyStreamUnbuffered

```cpp
std::streamsize copyStreamUnbuffered(std::istream & istr, std::ostream & ostr)
```

Copies all bytes from istr to ostr one byte at a time (no internal buffer). 
#### Parameters
* `istr` Source stream. 

* `ostr` Destination stream. 

#### Returns
Total number of bytes copied.

| Parameter | Type | Description |
|-----------|------|-------------|
| `istr` | `std::istream &` |  |
| `ostr` | `std::ostream &` |  |

---

{#copystream}

#### copyStream

```cpp
std::streamsize copyStream(std::istream & istr, std::ostream & ostr, size_t bufferSize)
```

Copies all bytes from istr to ostr using an internal buffer. 
#### Parameters
* `istr` Source stream. 

* `ostr` Destination stream. 

* `bufferSize` Internal buffer size in bytes (default: 8192). 

#### Returns
Total number of bytes copied.

| Parameter | Type | Description |
|-----------|------|-------------|
| `istr` | `std::istream &` |  |
| `ostr` | `std::ostream &` |  |
| `bufferSize` | `size_t` |  |

---

{#copytostring}

#### copyToString

```cpp
std::streamsize copyToString(std::istream & istr, std::string & str, size_t bufferSize)
```

Reads all bytes from istr and appends them to str. 
#### Parameters
* `istr` Source stream. 

* `str` Output string to append data to. 

* `bufferSize` Internal buffer size in bytes (default: 8192). 

#### Returns
Total number of bytes read.

| Parameter | Type | Description |
|-----------|------|-------------|
| `istr` | `std::istream &` |  |
| `str` | `std::string &` |  |
| `bufferSize` | `size_t` |  |

---

{#clearlist}

#### clearList

`inline`

```cpp
template<typename Val> inline void clearList(std::list< Val * > & L)
```

Delete all elements from a list of pointers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `L` | `std::list< Val * > &` |  |

---

{#cleardeque}

#### clearDeque

`inline`

```cpp
template<typename Val> inline void clearDeque(std::deque< Val * > & D)
```

Delete all elements from a deque of pointers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `D` | `std::deque< Val * > &` |  |

---

{#clearvector}

#### clearVector

`inline`

```cpp
template<typename Val> inline void clearVector(std::vector< Val * > & V)
```

Delete all elements from a vector of pointers.

| Parameter | Type | Description |
|-----------|------|-------------|
| `V` | `std::vector< Val * > &` |  |

---

{#clearmap}

#### clearMap

`inline`

```cpp
template<typename Key, typename Val> inline void clearMap(std::map< Key, Val * > & M)
```

Delete all associated values from a map (not the key elements).

| Parameter | Type | Description |
|-----------|------|-------------|
| `M` | `std::map< Key, Val * > &` |  |

{#version}

## Version

```cpp
#include <util.h>
```

Semantic version number with major, minor, and patch fields.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`major`](#major)  |  |
| `int` | [`minor`](#minor)  |  |
| `int` | [`revision`](#revision)  |  |
| `int` | [`build`](#build)  |  |

---

{#major}

#### major

```cpp
int major
```

---

{#minor}

#### minor

```cpp
int minor
```

---

{#revision}

#### revision

```cpp
int revision
```

---

{#build}

#### build

```cpp
int build
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Version`](#version-1) `inline` | Parses a dot-separated version string into up to four numeric fields. Unspecified fields default to 0. Examples: "1.2.3", "2.0", "3.7.8.0".  |
| `bool` | [`operator<`](#operator-11) `inline` | Returns true if this version is strictly less than other. Compares fields in major, minor, revision, build order.  |
| `bool` | [`operator==`](#operator-12) `const` `inline` | Returns true if all four version fields are equal.  |

---

{#version-1}

#### Version

`inline`

```cpp
inline Version(std::string_view version)
```

Parses a dot-separated version string into up to four numeric fields. Unspecified fields default to 0. Examples: "1.2.3", "2.0", "3.7.8.0". 
#### Parameters
* `version` Dot-separated version string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `std::string_view` |  |

---

{#operator-11}

#### operator<

`inline`

```cpp
inline bool operator<(const Version & other)
```

Returns true if this version is strictly less than other. Compares fields in major, minor, revision, build order. 
#### Parameters
* `other` [Version](#version) to compare against. 

#### Returns
true if this < other.

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `const [Version](#version) &` |  |

---

{#operator-12}

#### operator==

`const` `inline`

```cpp
inline bool operator==(const Version & other) const
```

Returns true if all four version fields are equal. 
#### Parameters
* `other` [Version](#version) to compare against. 

#### Returns
true if this == other.

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `const [Version](#version) &` |  |

