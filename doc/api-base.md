# base 

The `base` module contains reusable cross platform tools and utilities.

### Classes

| Name | Description |
|------|-------------|
| [`Thread`](#classicy_1_1Thread) | This class implements a platform-independent wrapper around an operating system thread. |
| [`Application`](#classicy_1_1Application) | Main Icey application class. |
| [`MutableBuffer`](#classicy_1_1MutableBuffer) | The [MutableBuffer](#classicy_1_1MutableBuffer) class provides a safe representation of a buffer that can be modified. It does not own the underlying data, and so is cheap to copy or assign. |
| [`ConstBuffer`](#classicy_1_1ConstBuffer) | The [ConstBuffer](#classicy_1_1ConstBuffer) class provides a safe representation of a buffer that cannot be modified. It does not own the underlying data, and so is cheap to copy or assign. |
| [`BitReader`](#classicy_1_1BitReader) | Class for reading binary streams. |
| [`BitWriter`](#classicy_1_1BitWriter) | Class for reading/writing binary streams. |
| [`DynamicBitWriter`](#classicy_1_1DynamicBitWriter) | Class for reading/writing dynamically resizable binary streams. |
| [`AbstractCollection`](#classicy_1_1AbstractCollection) | [AbstractCollection](#classicy_1_1AbstractCollection) is an abstract interface for managing a key-value store of indexed pointers. |
| [`PointerCollection`](#classicy_1_1PointerCollection) | This collection is used to maintain a map of unique_ptr values indexed by key in a thread-safe way. |
| [`LiveCollection`](#classicy_1_1LiveCollection) |  |
| [`KVCollection`](#classicy_1_1KVCollection) | Reusable stack based unique key-value store for DRY coding. |
| [`NVCollection`](#classicy_1_1NVCollection) | A storage container for a name value collections. This collection can store multiple entries for each name, and it's getters are case-insensitive. |
| [`Configuration`](#classicy_1_1Configuration) | [Configuration](#classicy_1_1Configuration) is an abstract base class for managing different kinds of configuration storage back ends such as JSON, XML, or database. |
| [`ScopedConfiguration`](#classicy_1_1ScopedConfiguration) | [ScopedConfiguration](#classicy_1_1ScopedConfiguration) provides multiple levels of configuration for a module. Multiple levels means that there is a module level scope, and a default scope. When a property is accessed, the module scope value will be used if available, otherwise the default scope value will be used. |
| [`Timestamp`](#classicy_1_1Timestamp) | A [Timestamp](#classicy_1_1Timestamp) stores a monotonic* time value with (theoretical) microseconds resolution. Timestamps can be compared with each other and simple arithmetics are supported. |
| [`Timespan`](#classicy_1_1Timespan) | A class that represents time spans up to microsecond resolution. |
| [`DateTime`](#classicy_1_1DateTime) | This class represents an instant in time, expressed in years, months, days, hours, minutes, seconds and milliseconds based on the Gregorian calendar. The class is mainly useful for conversions between UTC, Julian day and Gregorian calendar dates. |
| [`Timezone`](#classicy_1_1Timezone) | This class provides information about the current timezone. |
| [`LocalDateTime`](#classicy_1_1LocalDateTime) | This class represents an instant in local time (as opposed to UTC), expressed in years, months, days, hours, minutes, seconds and milliseconds based on the Gregorian calendar. |
| [`DateTimeFormat`](#classicy_1_1DateTimeFormat) | Definition of date/time formats and various constants used by [DateTimeFormatter](#classicy_1_1DateTimeFormatter) and [DateTimeParser](#classicy_1_1DateTimeParser). |
| [`DateTimeFormatter`](#classicy_1_1DateTimeFormatter) | This class converts dates and times into strings, supporting a variety of standard and custom formats. |
| [`DateTimeParser`](#classicy_1_1DateTimeParser) | This class provides a method for parsing dates and times from strings. All parsing methods do their best to parse a meaningful result, even from malformed input strings. |
| [`Stopwatch`](#classicy_1_1Stopwatch) | A simple facility to measure time intervals with microsecond resolution. |
| [`Idler`](#classicy_1_1Idler) | Asynchronous type that triggers callbacks when the event loop is idle. |
| [`Decoder`](#classicy_1_1basic_1_1Decoder) |  |
| [`Encoder`](#classicy_1_1basic_1_1Encoder) |  |
| [`Runnable`](#classicy_1_1basic_1_1Runnable) | Abstract interface for classes that can be run and cancelled. |
| [`Startable`](#classicy_1_1basic_1_1Startable) | Abstract interface for a classes that can be started and stopped. |
| [`Sendable`](#classicy_1_1basic_1_1Sendable) | Abstract interface for classes that can be sent and cancelled. |
| [`Queue`](#classicy_1_1ipc_1_1Queue) | IPC queue is for safely passing templated actions between threads and processes. |
| [`SyncQueue`](#classicy_1_1ipc_1_1SyncQueue) | IPC synchronization queue is for passing templated actions between threads and the event loop we are synchronizing with. |
| [`LogWriter`](#classicy_1_1LogWriter) | Log output stream writer. |
| [`AsyncLogWriter`](#classicy_1_1AsyncLogWriter) | [Thread](#classicy_1_1Thread) based log output stream writer. |
| [`Logger`](#classicy_1_1Logger) | [Logger](#classicy_1_1Logger) class. |
| [`LogChannel`](#classicy_1_1LogChannel) |  |
| [`ConsoleChannel`](#classicy_1_1ConsoleChannel) |  |
| [`FileChannel`](#classicy_1_1FileChannel) |  |
| [`RotatingFileChannel`](#classicy_1_1RotatingFileChannel) |  |
| [`IPacket`](#classicy_1_1IPacket) | The basic packet type which is passed around the Icey system. [IPacket](#classicy_1_1IPacket) can be extended for each protocol to enable polymorphic processing and callbacks using [PacketStream](#classicy_1_1PacketStream) and friends. |
| [`FlagPacket`](#classicy_1_1FlagPacket) | Packet for sending bitwise flags along the packet stream. |
| [`RawPacket`](#classicy_1_1RawPacket) | [RawPacket](#classicy_1_1RawPacket) is the default data packet type which consists of an optionally managed char pointer and a size value. |
| [`IPacketCreationStrategy`](#classicy_1_1IPacketCreationStrategy) |  |
| [`PacketFactory`](#classicy_1_1PacketFactory) |  |
| [`ThreadedStreamReader`](#classicy_1_1ThreadedStreamReader) | Threaded stream reader class. |
| [`StreamWriter`](#classicy_1_1StreamWriter) | Packet stream writer class. |
| [`SyncPacketQueue`](#classicy_1_1SyncPacketQueue) |  |
| [`AsyncPacketQueue`](#classicy_1_1AsyncPacketQueue) |  |
| [`PacketStreamAdapter`](#classicy_1_1PacketStreamAdapter) | This class is a wrapper for integrating external classes with the a [PacketStream](#classicy_1_1PacketStream)'s data flow and state machine. |
| [`PacketProcessor`](#classicy_1_1PacketProcessor) | This class is a virtual interface for creating PacketStreamAdapters which process that and emit the [IPacket](#classicy_1_1IPacket) type. |
| [`PacketStream`](#classicy_1_1PacketStream) | This class is used for processing and boradcasting IPackets in a flexible way. A [PacketStream](#classicy_1_1PacketStream) consists of one or many PacketSources, one or many PacketProcessors, and one or many delegate receivers. |
| [`PacketTransaction`](#classicy_1_1PacketTransaction) | This class provides request/response functionality for [IPacket](#classicy_1_1IPacket) types. |
| [`Pipe`](#classicy_1_1Pipe) | [Pipe](#classicy_1_1Pipe) implementation for process stdio. |
| [`Process`](#classicy_1_1Process) |  |
| [`Queue`](#classicy_1_1Queue) | Thread-safe queue container. |
| [`RunnableQueue`](#classicy_1_1RunnableQueue) |  |
| [`SyncQueue`](#classicy_1_1SyncQueue) | [SyncQueue](#classicy_1_1SyncQueue) extends [Synchronizer](#classicy_1_1Synchronizer) to implement a synchronized FIFO queue which receives T objects from any thread and synchronizes them for safe consumption by the associated event loop. |
| [`AsyncQueue`](#classicy_1_1AsyncQueue) | [AsyncQueue](#classicy_1_1AsyncQueue) is a thread-based queue which receives packets from any thread source and dispatches them asynchronously. |
| [`Random`](#classicy_1_1Random) | [Random](#classicy_1_1Random) implements a pseudo random number generator (PRNG). The PRNG is a nonlinear additive feedback random number generator using 256 bytes of state information and a period of up to 2^69. |
| [`Runner`](#classicy_1_1Runner) | [Runner](#classicy_1_1Runner) is a virtual interface for implementing asynchronous objects such as threads and futures. |
| [`StopPropagation`](#classicy_1_1StopPropagation) | Exception to break out of the current [Signal](#classicy_1_1Signal) callback scope. |
| [`Signal`](#classicy_1_1Signal) | [Signal](#classicy_1_1Signal) and slots implementation. |
| [`Signal< RT(Args...)>`](#classicy_1_1Signal_3_01RT_07Args_8_8_8_08_4) |  |
| [`Singleton`](#classicy_1_1Singleton) | Helper template class for managing singleton objects allocated on the heap. |
| [`State`](#classicy_1_1State) | [State](#classicy_1_1State) class for state machines. |
| [`Stateful`](#classicy_1_1Stateful) | [State](#classicy_1_1State) machine implementation. |
| [`Stream`](#classicy_1_1Stream) | Basic stream type for sockets and pipes. |
| [`Synchronizer`](#classicy_1_1Synchronizer) | [Synchronizer](#classicy_1_1Synchronizer) enables any thread to communicate with the associated event loop via synchronized callbacks. |
| [`Task`](#classicy_1_1Task) | Abstract class is for implementing any kind asyncronous task. |
| [`TaskRunner`](#classicy_1_1TaskRunner) | [Runner](#classicy_1_1Runner) for tasks that inherit the `[Task](#classicy_1_1Task)` interface. |
| [`Test`](#classicy_1_1test_1_1Test) | [Test](#classicy_1_1test_1_1Test) wrapper class. |
| [`FunctionTest`](#classicy_1_1test_1_1FunctionTest) |  |
| [`TestRunner`](#classicy_1_1test_1_1TestRunner) | [Test](#classicy_1_1test_1_1Test) manager queue. |
| [`Timer`](#classicy_1_1Timer) | Asynchronous event based timer. |
| [`TZInfo`](#classicy_1_1TZInfo) |  |
| [`Timeout`](#classicy_1_1Timeout) | [Timeout](#classicy_1_1Timeout) counter which expires after a given delay. |
| [`TimedToken`](#classicy_1_1TimedToken) | Token that expires after the specified duration. |
| [`OptionParser`](#structicy_1_1OptionParser) |  |
| [`ShutdownCmd`](#structicy_1_1ShutdownCmd) |  |
| [`Encoder`](#structicy_1_1base64_1_1Encoder) | Base64 encoder. |
| [`Decoder`](#structicy_1_1base64_1_1Decoder) | Base64 decoder. |
| [`Bitwise`](#structicy_1_1Bitwise) | Container for smart management of bitwise integer flags. |
| [`ILT`](#structicy_1_1NVCollection_1_1ILT) |  |
| [`AbstractDelegate`](#structicy_1_1AbstractDelegate) | Abstract delegate interface. |
| [`FunctionDelegate`](#structicy_1_1FunctionDelegate) | The `[FunctionDelegate](#structicy_1_1FunctionDelegate)` contains a `std::function`. |
| [`ClassDelegate`](#structicy_1_1ClassDelegate) | The `[ClassDelegate](#structicy_1_1ClassDelegate)` contains a pointer to a class member. |
| [`ConstClassDelegate`](#structicy_1_1ConstClassDelegate) | The `[ConstClassDelegate](#structicy_1_1ConstClassDelegate)` contains a pointer to a `const` class member. |
| [`PolymorphicDelegate`](#structicy_1_1PolymorphicDelegate) | Polymorphic function delegate. |
| [`Error`](#structicy_1_1Error) | Basic error type. |
| [`Encoder`](#structicy_1_1hex_1_1Encoder) | Hex encoder. |
| [`Decoder`](#structicy_1_1hex_1_1Decoder) | Hex decoder. |
| [`Action`](#structicy_1_1ipc_1_1Action) | Default action type for executing synchronized callbacks. |
| [`LogStream`](#structicy_1_1LogStream) |  |
| [`Dispose`](#structicy_1_1deleter_1_1Dispose) | Deleter functor that calls dispose() on the pointer. |
| [`Array`](#structicy_1_1deleter_1_1Array) | Deleter functor for array pointers. |
| [`IPacketInfo`](#structicy_1_1IPacketInfo) | An abstract interface for packet sources to provide extra information about packets. |
| [`PacketCreationStrategy`](#structicy_1_1PacketCreationStrategy) | This template class implements an adapter that sits between an SignalBase and an object receiving notifications from it. |
| [`PacketAdapterReference`](#structicy_1_1PacketAdapterReference) | Provides a reference to a [PacketStreamAdapter](#classicy_1_1PacketStreamAdapter) with optional ownership. |
| [`PacketStreamState`](#structicy_1_1PacketStreamState) |  |
| [`TransactionState`](#structicy_1_1TransactionState) |  |
| [`Context`](#structicy_1_1Runner_1_1Context) | [Context](#structicy_1_1Runner_1_1Context) object which we send to the thread context. |
| [`SharedLibrary`](#structicy_1_1SharedLibrary) |  |
| [`Deleter`](#structicy_1_1Singleton_1_1Deleter) | Custom deleter that can access private destructors via friendship. |
| [`Version`](#structicy_1_1util_1_1Version) |  |

### Members

| Name | Description |
|------|-------------|
| [`__CLASS_FUNCTION__`](#group__base_1ga59a5ffd42b19d28fe2cdd703647ea267) |  |
| [`STrace`](#group__base_1ga8fa36ad681be913ccc80568b5e1b707e) |  |
| [`SDebug`](#group__base_1ga994543ef28923f7e79a42280b974e57a) |  |
| [`SInfo`](#group__base_1ga428be2b940a54979e08fe97c9511f70f) |  |
| [`SWarn`](#group__base_1ga4e70a349e149eaf8d719346d98ccc3e8) |  |
| [`SError`](#group__base_1ga3d49c78b4f5f794c1fb59e44fc3cdefc) |  |
| [`LTrace`](#group__base_1gad99562e79a8f5d76e2a364dee4b0ac9c) |  |
| [`LDebug`](#group__base_1ga8f15acfec2db055b15d9dab7b6975210) |  |
| [`LInfo`](#group__base_1gaaec3c38855627fda219c81a8cd509a68) |  |
| [`LWarn`](#group__base_1ga487f424982fc03d4a52e0b03efedc855) |  |
| [`LError`](#group__base_1ga10b47e965290e0f238a4e6ce711b1b3e) |  |
| [`expect`](#group__base_1ga92645105a4c87ac01db7587df58caca6) |  |
| [`TYPE_0`](#group__base_1ga7684fdb8b359fbbbcaa5e149704024b3) |  |
| [`BREAK_0`](#group__base_1ga3c1b328e567007cf8b2a488f7da20cd2) |  |
| [`DEG_0`](#group__base_1ga4b02509c5d048cb2525e0066d7c2f191) |  |
| [`SEP_0`](#group__base_1gae66d150b37bdc4fb534c6cf4fd6927f5) |  |
| [`TYPE_1`](#group__base_1gadf00e147e54d287fe67232fc3c03881f) |  |
| [`BREAK_1`](#group__base_1ga2f8ab1a0f7425aaa202c501115ac97ac) |  |
| [`DEG_1`](#group__base_1ga0be2834c56b0aaa7a6d34986f7921200) |  |
| [`SEP_1`](#group__base_1ga706a88d02086f617d9a4112903e47e41) |  |
| [`TYPE_2`](#group__base_1ga8fde5bf4966bf8bead39337acde60635) |  |
| [`BREAK_2`](#group__base_1ga05b3315f5cfe0296dcd69a52f1655c6d) |  |
| [`DEG_2`](#group__base_1ga3ac87cdf960466ff1225acc151e1b990) |  |
| [`SEP_2`](#group__base_1ga7af722c75c13c22d73bbcca1088b8dfa) |  |
| [`TYPE_3`](#group__base_1ga625df8982f188845257d31ea0e6c59e3) |  |
| [`BREAK_3`](#group__base_1gae301c8961d39d5bb4f771779e7f892a5) |  |
| [`DEG_3`](#group__base_1ga551433e25d99861799734a5d64404a75) |  |
| [`SEP_3`](#group__base_1ga2edba143f3e114a7eb2e12e87cd22168) |  |
| [`TYPE_4`](#group__base_1ga1eea18b6fa0e026859776264e7b70d06) |  |
| [`BREAK_4`](#group__base_1ga5b20d74b1c51063cf48aac12871c508b) |  |
| [`DEG_4`](#group__base_1gaa5908dcf53ecbe2f7e9ba5c3ffca01c1) |  |
| [`SEP_4`](#group__base_1ga579e3bd1ae8bb600f9eab886a82bf075) |  |

---

#### __CLASS_FUNCTION__ 

```cpp
__CLASS_FUNCTION__()
```

---

#### STrace 

```cpp
STrace()
```

---

#### SDebug 

```cpp
SDebug()
```

---

#### SInfo 

```cpp
SInfo()
```

---

#### SWarn 

```cpp
SWarn()
```

---

#### SError 

```cpp
SError()
```

---

#### LTrace 

```cpp
LTrace()
```

---

#### LDebug 

```cpp
LDebug()
```

---

#### LInfo 

```cpp
LInfo()
```

---

#### LWarn 

```cpp
LWarn()
```

---

#### LError 

```cpp
LError()
```

---

#### expect 

```cpp
expect()
```

---

#### TYPE_0 

```cpp
TYPE_0()
```

---

#### BREAK_0 

```cpp
BREAK_0()
```

---

#### DEG_0 

```cpp
DEG_0()
```

---

#### SEP_0 

```cpp
SEP_0()
```

---

#### TYPE_1 

```cpp
TYPE_1()
```

---

#### BREAK_1 

```cpp
BREAK_1()
```

---

#### DEG_1 

```cpp
DEG_1()
```

---

#### SEP_1 

```cpp
SEP_1()
```

---

#### TYPE_2 

```cpp
TYPE_2()
```

---

#### BREAK_2 

```cpp
BREAK_2()
```

---

#### DEG_2 

```cpp
DEG_2()
```

---

#### SEP_2 

```cpp
SEP_2()
```

---

#### TYPE_3 

```cpp
TYPE_3()
```

---

#### BREAK_3 

```cpp
BREAK_3()
```

---

#### DEG_3 

```cpp
DEG_3()
```

---

#### SEP_3 

```cpp
SEP_3()
```

---

#### TYPE_4 

```cpp
TYPE_4()
```

---

#### BREAK_4 

```cpp
BREAK_4()
```

---

#### DEG_4 

```cpp
DEG_4()
```

---

#### SEP_4 

```cpp
SEP_4()
```

## Thread 

> **Extends:** `icy::Runner`, `icy::Runner`
> **Defined in:** `thread.h`

This class implements a platform-independent wrapper around an operating system thread.

Platform-independent wrapper around an operating system thread.

This class inherits the `[Runner](#classicy_1_1Runner)` interface and may be used with any implementation that's powered by an asynchronous `[Runner](#classicy_1_1Runner)`.

### Members

| Name | Description |
|------|-------------|
| [`Thread`](#group__base_1ga54fd807dcf46e13b7740713f24487816) |  |
| [`Thread`](#group__base_1gabed246ada7fd8383ba003ddfaff44673) |  |
| [`Thread`](#group__base_1gab5b31ddaa4aac8464c14ff69a30ff8e0) |  |
| [`Thread`](#group__base_1gae77a1fdf6ad50532ade119cfad1fff28) |  |
| [`~Thread`](#group__base_1gaf8d1a393d2f0130b0e8d3e634f50c125) |  |
| [`join`](#group__base_1gad5b13af2dd59884809566c8f95a99f16) | Waits until the thread exits. |
| [`waitForExit`](#group__base_1ga60b2ef4860d9cb68ec601d95a35b38b5) | Waits until the thread exits. |
| [`id`](#group__base_1gad3ac929d47e0dcadc5f106473d11a148) | Returns the native thread handle. |
| [`Thread`](#group__base_1ga03f23cddda2cf51b3306eb94f24492bc) | Templated constructor. |
| [`start`](#group__base_1gac61d598778274d3c6b8342cad1d869c7) | Start a function with veradic arguments. |
| [`start`](#group__base_1ga5f7fb6da8dc9da113fdc20de6f336c3b) | Start the asynchronous context with the given void function. |
| [`currentID`](#group__base_1ga5c4c221bb4c25036320d22087f72a355) | Returns the native thread ID of the current thread. |
| [`Thread`](#group__base_1ga2db47dac5c43119f6254de56f3c1c7ad) | NonCopyable and NonMovable. |
| [`operator=`](#group__base_1ga5eb15190777fefe4728f8bc2eaf1817e) |  |
| [`async`](#group__base_1gac9ec059f9ffbd29d9790a7f95c5c5d21) | Returns true if the implementation is thread-based, or false if it belongs to an event loop. |
| [`startAsync`](#group__base_1gaf0bc2b6aec6123dc914966507326c61d) |  |
| [`mainID`](#group__base_1ga61fbd13a4564ad111609b13ec008ef91) | Accessor for the main thread ID. |
| [`_handle`](#group__base_1ga4f306e5416331b9b4131adfc0f45c908) |  |
| [`_thread`](#group__base_1gac600f9a3aefe15106420b37be988b062) |  |
| [`Thread`](#classicy_1_1Thread_1ga54fd807dcf46e13b7740713f24487816) | Default constructor. |
| [`~Thread`](#classicy_1_1Thread_1gaf8d1a393d2f0130b0e8d3e634f50c125) | Destructor. |
| [`join`](#classicy_1_1Thread_1gad5b13af2dd59884809566c8f95a99f16) | Wait until the thread exits. |
| [`id`](#classicy_1_1Thread_1gad3ac929d47e0dcadc5f106473d11a148) | Return the native thread handle. |
| [`currentID`](#classicy_1_1Thread_1ga5c4c221bb4c25036320d22087f72a355) | Return the native thread ID of the current thread. |
| [`Thread`](#classicy_1_1Thread_1ga2db47dac5c43119f6254de56f3c1c7ad) | NonCopyable and NonMovable. |
| [`operator=`](#classicy_1_1Thread_1ga5eb15190777fefe4728f8bc2eaf1817e) |  |
| [`async`](#classicy_1_1Thread_1gac9ec059f9ffbd29d9790a7f95c5c5d21) | Returns true if the implementation is thread-based, or false if it belongs to an event loop. |

---

#### Thread 

```cpp
Thread()
```

---

#### Thread 

```cpp
Thread(basic::Runnable & target)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `basic::Runnable &` |  |

---

#### Thread 

```cpp
Thread(std::function< void()> target)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `std::function< void()>` |  |

---

#### Thread 

```cpp
Thread(std::function< void(void *)> target, void * arg)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `std::function< void(void *)>` |  |
| `arg` | `void *` |  |

---

#### ~Thread 

```cpp
virtual ~Thread()
```

---

#### join 

```cpp
void join()
```

Waits until the thread exits.

---

#### waitForExit 

```cpp
bool waitForExit(int timeout)
```

Waits until the thread exits.

The thread should be cancelled beore calling this method. This method must be called from outside the current thread context or deadlock will ensue.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `int` |  |

---

#### id 

```cpp
uv_thread_t id() const
```

Returns the native thread handle.

---

#### Thread 

```cpp
template<typename Function, typename... Args> inline explicit Thread(Function && func, Args &&... args)
```

Templated constructor.

This constructor starts the thread with the given function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |

---

#### start 

```cpp
template<typename Function, typename... Args> inline void start(Function && func, Args &&... args)
```

Start a function with veradic arguments.

This method starts the thread with the given function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |

---

#### start 

```cpp
virtual void start(std::function< void()> func)
```

Start the asynchronous context with the given void function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `std::function< void()>` |  |

---

#### currentID 

```cpp
static uv_thread_t currentID()
```

Returns the native thread ID of the current thread.

---

#### Thread 

```cpp
Thread(const Thread &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
Thread & operator=(const Thread &) = delete
```

---

#### async 

```cpp
virtual bool async() const
```

Returns true if the implementation is thread-based, or false if it belongs to an event loop.

---

#### startAsync 

```cpp
virtual void startAsync()
```

---

#### mainID 

```cpp
const std::thread::id mainID = uv_thread_self()
```

Accessor for the main thread ID.

---

#### _handle 

```cpp
uv_thread_t _handle
```

---

#### _thread 

```cpp
std::thread _thread
```

---

#### Thread 

```cpp
Thread()
```

Default constructor.

---

#### ~Thread 

```cpp
virtual ~Thread()
```

Destructor.

---

#### join 

```cpp
void join()
```

Wait until the thread exits.

---

#### id 

```cpp
std::thread::id id() const
```

Return the native thread handle.

---

#### currentID 

```cpp
static std::thread::id currentID()
```

Return the native thread ID of the current thread.

---

#### Thread 

```cpp
Thread(const [Thread](#classicy_1_1Thread) &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
[Thread](#classicy_1_1Thread) & operator=(const [Thread](#classicy_1_1Thread) &) = delete
```

---

#### async 

```cpp
virtual bool async() const
```

Returns true if the implementation is thread-based, or false if it belongs to an event loop.

## Application 

> **Defined in:** `application.h`

Main Icey application class.

This class exposes basic features required by most applications:

* Running the event loop

* Command line option parsing **See also**: [OptionParser](#structicy_1_1OptionParser)

* Shutdown signal (Ctrl-C) handling

* Garbage collection

### Members

| Name | Description |
|------|-------------|
| [`Application`](#group__base_1gadb808e26528e18856d2896a3ce7947ca) | Constructor. |
| [`~Application`](#group__base_1ga0aafa241a2debe7409ae096420068d1e) | Destructor. |
| [`run`](#group__base_1gacf3f29800455e5d360a19b9c8fe66241) | Run the application event loop. |
| [`stop`](#group__base_1ga830f616291457503049f9fc9d1a3f8ba) | Stop the application event loop. |
| [`finalize`](#group__base_1gae94cf187f38a9accf0abdffc868e700a) | Finalize and free any remaining pointers still held by the application event loop. |
| [`waitForShutdown`](#group__base_1ga648ccd97eb0acd73cb4dc0b2b4ae030a) | Bind the shutdown signal and run the main event loop. |
| [`bindShutdownSignal`](#group__base_1ga80ca3367db09fde58ffec5b3b17ce960) | Bind the shutdown signal. |
| [`getDefault`](#group__base_1ga5db9db61083ba33fdfdbd5c8082aba19) | Returns the default [Application](#classicy_1_1Application) singleton, although [Application](#classicy_1_1Application) instances may be initialized individually. |
| [`Application`](#group__base_1gad2a86916a222902d29c639cc5daa9d5b) |  |
| [`operator=`](#group__base_1ga18e7eaa27695f7846fb43289b47b6ee1) |  |
| [`onShutdownSignal`](#group__base_1gabec6f73a14b517119f8e29b749d783f6) |  |
| [`onPrintHandle`](#group__base_1ga53e5c35202a822d72f44b7582ff07d5f) |  |
| [`loop`](#group__base_1ga0cf54187707ea8b8f88324f045fbc797) | Active event loop. |

---

#### Application 

```cpp
Application(uv::Loop * loop)
```

Constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~Application 

```cpp
~Application()
```

Destructor.

---

#### run 

```cpp
void run()
```

Run the application event loop.

---

#### stop 

```cpp
void stop()
```

Stop the application event loop.

---

#### finalize 

```cpp
void finalize()
```

Finalize and free any remaining pointers still held by the application event loop.

---

#### waitForShutdown 

```cpp
void waitForShutdown(std::function< void(void *)> callback, void * opaque)
```

Bind the shutdown signal and run the main event loop.

| Parameter | Type | Description |
|-----------|------|-------------|
| `callback` | `std::function< void(void *)>` |  |
| `opaque` | `void *` |  |

---

#### bindShutdownSignal 

```cpp
void bindShutdownSignal(std::function< void(void *)> callback, void * opaque)
```

Bind the shutdown signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `callback` | `std::function< void(void *)>` |  |
| `opaque` | `void *` |  |

---

#### getDefault 

```cpp
static Application & getDefault()
```

Returns the default [Application](#classicy_1_1Application) singleton, although [Application](#classicy_1_1Application) instances may be initialized individually.

---

#### Application 

```cpp
Application(const Application &) = delete
```

---

#### operator= 

```cpp
Application & operator=(const Application &) = delete
```

---

#### onShutdownSignal 

```cpp
static void onShutdownSignal(uv_signal_t * req, int signum)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `req` | `uv_signal_t *` |  |
| `signum` | `int` |  |

---

#### onPrintHandle 

```cpp
static void onPrintHandle(uv_handle_t * handle, void * arg)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_handle_t *` |  |
| `arg` | `void *` |  |

---

#### loop 

```cpp
uv::Loop * loop
```

Active event loop.

The event loop may be assigned on construction, otherwise the default event loop will be used.

## MutableBuffer 

> **Defined in:** `buffer.h`

The [MutableBuffer](#classicy_1_1MutableBuffer) class provides a safe representation of a buffer that can be modified. It does not own the underlying data, and so is cheap to copy or assign.

### Members

| Name | Description |
|------|-------------|
| [`MutableBuffer`](#group__base_1ga6305583f6d3eb711d61bdfbb0d3ccbea) | Construct an empty buffer. |
| [`MutableBuffer`](#group__base_1gaa7382ec91da7e891ae4579b4092dcb89) | Construct a buffer to represent the given memory range. |
| [`data`](#group__base_1ga2a8d6e86c032207fee491787fbee3fa6) |  |
| [`size`](#group__base_1ga6d8abadd0bc97709ff7bb5e3fa13a6a7) |  |
| [`cstr`](#group__base_1ga63b3387bb3327e2cd5a01f886aa16e4d) | Cast the buffer as a char pointer. |
| [`str`](#group__base_1ga5ed8318956c72abfcd85d9f36643e94d) | Returns the buffer as a string. |
| [`_data`](#group__base_1ga40dd0ba9bacac375ac14b439bc5f230f) |  |
| [`_size`](#group__base_1gacac6cdcfc5b21505e1159c8750cb96e8) |  |

---

#### MutableBuffer 

```cpp
inline MutableBuffer()
```

Construct an empty buffer.

---

#### MutableBuffer 

```cpp
inline MutableBuffer(void * data, size_t size)
```

Construct a buffer to represent the given memory range.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `void *` |  |
| `size` | `size_t` |  |

---

#### data 

```cpp
inline void * data() const
```

---

#### size 

```cpp
inline size_t size() const
```

---

#### cstr 

```cpp
inline char * cstr() const
```

Cast the buffer as a char pointer.

---

#### str 

```cpp
inline std::string str() const
```

Returns the buffer as a string.

---

#### _data 

```cpp
void * _data
```

---

#### _size 

```cpp
size_t _size
```

## ConstBuffer 

> **Defined in:** `buffer.h`

The [ConstBuffer](#classicy_1_1ConstBuffer) class provides a safe representation of a buffer that cannot be modified. It does not own the underlying data, and so is cheap to copy or assign.

### Members

| Name | Description |
|------|-------------|
| [`ConstBuffer`](#group__base_1ga9ee27b72f04bbf0a7012a2f56542f8ef) | Construct an empty buffer. |
| [`ConstBuffer`](#group__base_1gab0397dd96489a351a71fdc0213deac11) | Construct a buffer to represent the given memory range. |
| [`ConstBuffer`](#group__base_1ga217c5af0a20a4a11cfa451a389997ad9) | Construct a non-modifiable buffer from a modifiable one. |
| [`data`](#group__base_1gae7face6730ab3f32a4b204c03529b865) |  |
| [`size`](#group__base_1ga20f090f293422b51279b9de74f3d8dea) |  |
| [`cstr`](#group__base_1gab363d3fa459eb61194c5489530884552) | Cast the buffer as a const char pointer. |
| [`str`](#group__base_1gab94b7a54497431189aeae02fe254c2c8) | Returns the buffer as a string. |
| [`_data`](#group__base_1gacade139b3e2d38ca09b382ae15b47cc5) |  |
| [`_size`](#group__base_1gae20401d38018eb75ae74cd23c94a0c45) |  |

---

#### ConstBuffer 

```cpp
inline ConstBuffer()
```

Construct an empty buffer.

---

#### ConstBuffer 

```cpp
inline ConstBuffer(const void * data, size_t size)
```

Construct a buffer to represent the given memory range.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const void *` |  |
| `size` | `size_t` |  |

---

#### ConstBuffer 

```cpp
inline ConstBuffer(const MutableBuffer & b)
```

Construct a non-modifiable buffer from a modifiable one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `b` | `const MutableBuffer &` |  |

---

#### data 

```cpp
inline const void * data() const
```

---

#### size 

```cpp
inline size_t size() const
```

---

#### cstr 

```cpp
inline const char * cstr() const
```

Cast the buffer as a const char pointer.

---

#### str 

```cpp
inline std::string str() const
```

Returns the buffer as a string.

---

#### _data 

```cpp
const void * _data
```

---

#### _size 

```cpp
size_t _size
```

## BitReader 

> **Defined in:** `buffer.h`

Class for reading binary streams.

### Members

| Name | Description |
|------|-------------|
| [`BitReader`](#group__base_1ga72905d506783acd42a98dcee7f959e57) |  |
| [`BitReader`](#group__base_1gabf0db9bf938b95829322f00935eb6092) |  |
| [`BitReader`](#group__base_1ga4d25c7558ec919c38a2aa2bb2d3d549e) |  |
| [`~BitReader`](#group__base_1ga2b0a8e18dd144d85baaf44b4711ef57f) |  |
| [`get`](#group__base_1gaeb43583e35589cbf38d5ce90ee77c37d) | Reads a value from the [BitReader](#classicy_1_1BitReader). Returns false if there isn't enough data left for the specified type. Throws a std::out_of_range exception if reading past the limit. |
| [`get`](#group__base_1ga8c868edd08c6881b78fb7b455fbfd4c3) |  |
| [`getU8`](#group__base_1ga071979552c567a13fe0723ae659cfcbc) |  |
| [`getU16`](#group__base_1ga7235dad48ba744d85a2ba57b794fe456) |  |
| [`getU24`](#group__base_1gac9f87a05f5faca590b71cd5562794e65) |  |
| [`getU32`](#group__base_1ga393e2366848a14541c2084e17230f529) |  |
| [`getU64`](#group__base_1ga9b0440dcaf68c5b17e02cc00290c467e) |  |
| [`peek`](#group__base_1gae207340c2735276d75374184da016fe2) |  |
| [`peekU8`](#group__base_1gab561d29c0ddd0224fbd26f07fecc0903) |  |
| [`peekU16`](#group__base_1ga48fd6095876041c6835c1b9c8827e2fe) |  |
| [`peekU24`](#group__base_1ga163dd7b966ff9cfdbb60e9cab30a1eb6) |  |
| [`peekU32`](#group__base_1gae1f697546db37d27ea8c1f1862af4e3c) |  |
| [`peekU64`](#group__base_1gad7e4e58bf5874f3ef4c02f639401ce45) | Peeks data from the [BitReader](#classicy_1_1BitReader). -1 is returned if reading past boundary. |
| [`skipToChar`](#group__base_1ga04c8225dd818b9b318695305516e99c0) |  |
| [`skipWhitespace`](#group__base_1ga4796e1bd18cae8e4ae8c62398557c74c) |  |
| [`skipToNextLine`](#group__base_1ga31f4b884ce29c3dcbc6898fde862a5e2) |  |
| [`skipNextWord`](#group__base_1ga18d93cf6404f0e76811f670ccc596290) |  |
| [`readNextWord`](#group__base_1ga33e3ee1da159e557e66cf554b7e7111e) |  |
| [`readNextNumber`](#group__base_1ga7c0ecb36592167964d0283f9cc2a8b6a) |  |
| [`readLine`](#group__base_1gaf844ec2228b8a663f8573b5f1c4f1608) |  |
| [`readToNext`](#group__base_1ga5aab9beb7804826ae778577507b3a10e) |  |
| [`seek`](#group__base_1gabeacae46cca18859f2d5ff07374f46c9) | Set position pointer to absolute position. Throws a std::out_of_range exception if the value exceeds the limit. |
| [`skip`](#group__base_1gacc13c5b3a4389c9db9e5d05ffc717a86) | Set position pointer to relative position. Throws a std::out_of_range exception if the value exceeds the limit. |
| [`limit`](#group__base_1ga33617f1db650063c3601b39baa3d8380) | Returns the read limit. |
| [`position`](#group__base_1gac3bd978ba8558d4d120bd47376834e43) | Returns the current read position. |
| [`available`](#group__base_1ga4e553e8007d836fab027def3ab087288) | Returns the number of elements between the current position and the limit. |
| [`begin`](#group__base_1gae951c0fa5f76bba0e7468dcb7a2f7344) |  |
| [`current`](#group__base_1gadaf5501e315046c2ae8e988fa596ab70) |  |
| [`order`](#group__base_1ga82d4b93cc1c3c8a9bbfd0937b7f59aeb) |  |
| [`toString`](#group__base_1gac5342f8fbc0fa5e1c49615e530a9c5fe) |  |
| [`init`](#group__base_1gaeb106bc617ebfa66b53bd40197e91af3) |  |
| [`_position`](#group__base_1gad7302306c40922de5ec9fd8c94e616bc) |  |
| [`_limit`](#group__base_1ga6188ac62b1e66762fdd2473335205262) |  |
| [`_order`](#group__base_1ga4ee4b2b3556e5937a1a24d61d53d0513) |  |
| [`_bytes`](#group__base_1gae3aca9ee141c803928c047813d3f4a8c) |  |

---

#### BitReader 

```cpp
BitReader(const char * bytes, size_t size, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const char *` |  |
| `size` | `size_t` |  |
| `order` | `ByteOrder` |  |

---

#### BitReader 

```cpp
BitReader(const Buffer & buf, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const Buffer &` |  |
| `order` | `ByteOrder` |  |

---

#### BitReader 

```cpp
BitReader(const ConstBuffer & pod, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pod` | `const ConstBuffer &` |  |
| `order` | `ByteOrder` |  |

---

#### ~BitReader 

```cpp
~BitReader()
```

---

#### get 

```cpp
void get(char * val, size_t len)
```

Reads a value from the [BitReader](#classicy_1_1BitReader). Returns false if there isn't enough data left for the specified type. Throws a std::out_of_range exception if reading past the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `char *` |  |
| `len` | `size_t` |  |

---

#### get 

```cpp
void get(std::string & val, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `std::string &` |  |
| `len` | `size_t` |  |

---

#### getU8 

```cpp
void getU8(uint8_t & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint8_t &` |  |

---

#### getU16 

```cpp
void getU16(uint16_t & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint16_t &` |  |

---

#### getU24 

```cpp
void getU24(uint32_t & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint32_t &` |  |

---

#### getU32 

```cpp
void getU32(uint32_t & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint32_t &` |  |

---

#### getU64 

```cpp
void getU64(uint64_t & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint64_t &` |  |

---

#### peek 

```cpp
const char peek()
```

---

#### peekU8 

```cpp
const uint8_t peekU8()
```

---

#### peekU16 

```cpp
const uint16_t peekU16()
```

---

#### peekU24 

```cpp
const uint32_t peekU24()
```

---

#### peekU32 

```cpp
const uint32_t peekU32()
```

---

#### peekU64 

```cpp
const uint64_t peekU64()
```

Peeks data from the [BitReader](#classicy_1_1BitReader). -1 is returned if reading past boundary.

---

#### skipToChar 

```cpp
size_t skipToChar(char c)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `c` | `char` |  |

---

#### skipWhitespace 

```cpp
size_t skipWhitespace()
```

---

#### skipToNextLine 

```cpp
size_t skipToNextLine()
```

---

#### skipNextWord 

```cpp
size_t skipNextWord()
```

---

#### readNextWord 

```cpp
size_t readNextWord(std::string & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `std::string &` |  |

---

#### readNextNumber 

```cpp
size_t readNextNumber(unsigned int & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `unsigned int &` |  |

---

#### readLine 

```cpp
size_t readLine(std::string & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `std::string &` |  |

---

#### readToNext 

```cpp
size_t readToNext(std::string & val, char c)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `std::string &` |  |
| `c` | `char` |  |

---

#### seek 

```cpp
void seek(size_t val)
```

Set position pointer to absolute position. Throws a std::out_of_range exception if the value exceeds the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `size_t` |  |

---

#### skip 

```cpp
void skip(size_t size)
```

Set position pointer to relative position. Throws a std::out_of_range exception if the value exceeds the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `size_t` |  |

---

#### limit 

```cpp
size_t limit() const
```

Returns the read limit.

---

#### position 

```cpp
inline size_t position() const
```

Returns the current read position.

---

#### available 

```cpp
size_t available() const
```

Returns the number of elements between the current position and the limit.

---

#### begin 

```cpp
inline const char * begin() const
```

---

#### current 

```cpp
inline const char * current() const
```

---

#### order 

```cpp
inline ByteOrder order() const
```

---

#### toString 

```cpp
std::string toString()
```

---

#### init 

```cpp
void init(const char * bytes, size_t size, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const char *` |  |
| `size` | `size_t` |  |
| `order` | `ByteOrder` |  |

---

#### _position 

```cpp
size_t _position
```

---

#### _limit 

```cpp
size_t _limit
```

---

#### _order 

```cpp
ByteOrder _order
```

---

#### _bytes 

```cpp
const char * _bytes
```

## BitWriter 

> **Subclasses:** `icy::DynamicBitWriter`
> **Defined in:** `buffer.h`

Class for reading/writing binary streams.

Note that when using the constructor with the Buffer reference as an argument, the writer will dynamically expand the given buffer when writing passed the buffer capacity. All other cases will throw a std::out_of_range error when writing past the buffer capacity.

### Members

| Name | Description |
|------|-------------|
| [`BitWriter`](#group__base_1gab2dd71d855d182099b857a741b5d304a) |  |
| [`BitWriter`](#group__base_1ga9197ef38d9bcd9c796da1ef8ae70bb8c) |  |
| [`BitWriter`](#group__base_1ga0d30b3f7a40f306e24b5f905c692356e) |  |
| [`~BitWriter`](#group__base_1ga18cd74cab24e3aafc649ec3fa700611c) |  |
| [`put`](#group__base_1ga6f9a39d105c71a454ff1654881f760cb) | Append bytes to the buffer. Throws a `std::out_of_range` exception if reading past the limit. |
| [`put`](#group__base_1ga74ae0af5b9c1456fe21e5d7d8dc8c0b0) |  |
| [`putU8`](#group__base_1ga46dce3e3b73e40e84efb0e3764e66255) |  |
| [`putU16`](#group__base_1gace97db79776cfed401191e62154dbdc3) |  |
| [`putU24`](#group__base_1ga1bbad9337122aba95fcd1c7c136c1f34) |  |
| [`putU32`](#group__base_1ga07067c29ece34bd0e74497c841c66ebd) |  |
| [`putU64`](#group__base_1gae52010b9de5b44800d09d2ba6a4d4a13) |  |
| [`update`](#group__base_1gaf078f080a4b1844c55af4ef5f2c6492c) | Update a byte range. Throws a `std::out_of_range` exception if reading past the limit. |
| [`update`](#group__base_1ga29f24c588d65384b16ca1b290c925f2d) |  |
| [`updateU8`](#group__base_1gadb08f97afe9929995bab4e0050370ec0) |  |
| [`updateU16`](#group__base_1gaea07f0367babbfdeb63a3974e522457e) |  |
| [`updateU24`](#group__base_1ga543249a93a5b97f95c0ee23fc4c76ae1) |  |
| [`updateU32`](#group__base_1gac4ad85bdffd63789f9a55f4ea5043fc7) |  |
| [`updateU64`](#group__base_1gaf3f88c70e2da140a92662033ebbf32b6) |  |
| [`seek`](#group__base_1ga89948f0917a17ae6685542fcf6cd89a0) | Set position pointer to absolute position. Throws a `std::out_of_range` exception if the value exceeds the limit. |
| [`skip`](#group__base_1ga1ba542529f0e43e8ebfb0c49686ad63e) | Set position pointer to relative position. Throws a `std::out_of_range` exception if the value exceeds the limit. |
| [`limit`](#group__base_1gabee4a577412bb95082447e33b0e85225) | Returns the write limit. |
| [`position`](#group__base_1ga20daa94564eb9c24623e284275881baf) | Returns the current write position. |
| [`available`](#group__base_1ga221694bbd90c92d62f083413868448f0) | Returns the number of elements between the current write position and the limit. |
| [`begin`](#group__base_1gae9e198a59900c23e454b0df968ded299) |  |
| [`current`](#group__base_1gaf71f7b8e2e5778e3fe3457b40c4e3cea) |  |
| [`begin`](#group__base_1gaad1f7fe521048b1805076c34b435a57c) |  |
| [`current`](#group__base_1ga6e07f702615163b589ea0d840ed5da94) |  |
| [`order`](#group__base_1gaf577c3165d6bea6a49e49f33e056b56a) |  |
| [`toString`](#group__base_1ga4d4c2a107262e4dc586917646c69ed31) | Returns written bytes as a string. |
| [`init`](#group__base_1gac7dfaab4eee54c93c128af5f1bacc63b) |  |
| [`_position`](#group__base_1gae603a7273b54ba11f2a3c8a0651b6a4f) |  |
| [`_limit`](#group__base_1ga2855d1965aeb98fa431ce444c5cf850d) |  |
| [`_order`](#group__base_1gae89b790e3d037ddbed2683e37316ff74) |  |
| [`_bytes`](#group__base_1gac290f14abeca0508bd045a35e79cb4f6) |  |

---

#### BitWriter 

```cpp
BitWriter(char * bytes, size_t size, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `char *` |  |
| `size` | `size_t` |  |
| `order` | `ByteOrder` |  |

---

#### BitWriter 

```cpp
BitWriter(Buffer & buf, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `Buffer &` |  |
| `order` | `ByteOrder` |  |

---

#### BitWriter 

```cpp
BitWriter(MutableBuffer & pod, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pod` | `MutableBuffer &` |  |
| `order` | `ByteOrder` |  |

---

#### ~BitWriter 

```cpp
virtual ~BitWriter()
```

---

#### put 

```cpp
virtual void put(const char * val, size_t len)
```

Append bytes to the buffer. Throws a `std::out_of_range` exception if reading past the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `const char *` |  |
| `len` | `size_t` |  |

---

#### put 

```cpp
void put(const std::string & val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `const std::string &` |  |

---

#### putU8 

```cpp
void putU8(uint8_t val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint8_t` |  |

---

#### putU16 

```cpp
void putU16(uint16_t val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint16_t` |  |

---

#### putU24 

```cpp
void putU24(uint32_t val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint32_t` |  |

---

#### putU32 

```cpp
void putU32(uint32_t val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint32_t` |  |

---

#### putU64 

```cpp
void putU64(uint64_t val)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint64_t` |  |

---

#### update 

```cpp
virtual bool update(const char * val, size_t len, size_t pos)
```

Update a byte range. Throws a `std::out_of_range` exception if reading past the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `const char *` |  |
| `len` | `size_t` |  |
| `pos` | `size_t` |  |

---

#### update 

```cpp
bool update(const std::string & val, size_t pos)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `const std::string &` |  |
| `pos` | `size_t` |  |

---

#### updateU8 

```cpp
bool updateU8(uint8_t val, size_t pos)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint8_t` |  |
| `pos` | `size_t` |  |

---

#### updateU16 

```cpp
bool updateU16(uint16_t val, size_t pos)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint16_t` |  |
| `pos` | `size_t` |  |

---

#### updateU24 

```cpp
bool updateU24(uint32_t val, size_t pos)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint32_t` |  |
| `pos` | `size_t` |  |

---

#### updateU32 

```cpp
bool updateU32(uint32_t val, size_t pos)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint32_t` |  |
| `pos` | `size_t` |  |

---

#### updateU64 

```cpp
bool updateU64(uint64_t val, size_t pos)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `uint64_t` |  |
| `pos` | `size_t` |  |

---

#### seek 

```cpp
void seek(size_t val)
```

Set position pointer to absolute position. Throws a `std::out_of_range` exception if the value exceeds the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `size_t` |  |

---

#### skip 

```cpp
void skip(size_t size)
```

Set position pointer to relative position. Throws a `std::out_of_range` exception if the value exceeds the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `size_t` |  |

---

#### limit 

```cpp
size_t limit() const
```

Returns the write limit.

---

#### position 

```cpp
inline size_t position() const
```

Returns the current write position.

---

#### available 

```cpp
size_t available() const
```

Returns the number of elements between the current write position and the limit.

---

#### begin 

```cpp
inline char * begin()
```

---

#### current 

```cpp
inline char * current()
```

---

#### begin 

```cpp
inline const char * begin() const
```

---

#### current 

```cpp
inline const char * current() const
```

---

#### order 

```cpp
inline ByteOrder order() const
```

---

#### toString 

```cpp
std::string toString()
```

Returns written bytes as a string.

---

#### init 

```cpp
virtual void init(char * bytes, size_t size, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `char *` |  |
| `size` | `size_t` |  |
| `order` | `ByteOrder` |  |

---

#### _position 

```cpp
size_t _position
```

---

#### _limit 

```cpp
size_t _limit
```

---

#### _order 

```cpp
ByteOrder _order
```

---

#### _bytes 

```cpp
char * _bytes
```

## DynamicBitWriter 

> **Extends:** `icy::BitWriter`
> **Defined in:** `buffer.h`

Class for reading/writing dynamically resizable binary streams.

Note that when using the constructor with the Buffer reference as an argument, the writer will dynamically expand the given buffer when writing passed the buffer capacity. All other cases will throw a std::out_of_range error when writing past the buffer capacity.

### Members

| Name | Description |
|------|-------------|
| [`DynamicBitWriter`](#group__base_1ga49a2d410780afa3e2147f7860e774533) |  |
| [`DynamicBitWriter`](#group__base_1gab8bd9f094a9a88adc83305428d6414cd) |  |
| [`~DynamicBitWriter`](#group__base_1ga3d894b26b22905a62cb71cabf747db47) |  |
| [`put`](#group__base_1ga24ba1cf5b5f9ce0b0299946b7b673df2) | Append bytes to the buffer. Throws a `std::out_of_range` exception if reading past the limit. |
| [`update`](#group__base_1ga6e09d70eb7d0892345b0d669645fa1fa) | Update a byte range. Throws a `std::out_of_range` exception if reading past the limit. |
| [`_buffer`](#group__base_1ga5198320e1d2b20c6bed0a38f9168695a) |  |
| [`_offset`](#group__base_1gaef9c3c368088675470dd483e92987c70) |  |

---

#### DynamicBitWriter 

```cpp
DynamicBitWriter(Buffer & buf, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `Buffer &` |  |
| `order` | `ByteOrder` |  |

---

#### DynamicBitWriter 

```cpp
DynamicBitWriter(Buffer & buf, Buffer::iterator offset, ByteOrder order)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `Buffer &` |  |
| `offset` | `Buffer::iterator` |  |
| `order` | `ByteOrder` |  |

---

#### ~DynamicBitWriter 

```cpp
virtual ~DynamicBitWriter()
```

---

#### put 

```cpp
virtual void put(const char * val, size_t len)
```

Append bytes to the buffer. Throws a `std::out_of_range` exception if reading past the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `const char *` |  |
| `len` | `size_t` |  |

---

#### update 

```cpp
virtual bool update(const char * val, size_t len, size_t pos)
```

Update a byte range. Throws a `std::out_of_range` exception if reading past the limit.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `const char *` |  |
| `len` | `size_t` |  |
| `pos` | `size_t` |  |

---

#### _buffer 

```cpp
Buffer & _buffer
```

---

#### _offset 

```cpp
size_t _offset
```

## AbstractCollection 

> **Subclasses:** `icy::PointerCollection< std::string, IUser >`, `icy::PointerCollection< std::string, Peer >`, `icy::PointerCollection< std::string, LocalPackage >`, `icy::PointerCollection< std::string, RemotePackage >`, `icy::PointerCollection< std::string, Message >`, `icy::PointerCollection< uint32_t, TCPConnectionPair >`, `icy::PointerCollection< TKey, TValue >`
> **Defined in:** `collection.h`

[AbstractCollection](#classicy_1_1AbstractCollection) is an abstract interface for managing a key-value store of indexed pointers.

### Members

| Name | Description |
|------|-------------|
| [`AbstractCollection`](#group__base_1ga890f80336ad1fc7036ef91c7c4006723) |  |
| [`~AbstractCollection`](#group__base_1gadb44ca86f52d6229148fab898575ba5f) |  |
| [`add`](#group__base_1gaaee10ff4d5a246d2b62d3e1f4deb799d) |  |
| [`remove`](#group__base_1ga7688d5f1a92a829835e39144fa703383) |  |
| [`remove`](#group__base_1ga950a0e97583716495ccf83e68563eda3) |  |
| [`exists`](#group__base_1ga862bf994fa6f95c49563528a1dc7f320) |  |
| [`exists`](#group__base_1gaadc51ac08f83c7fd416067d31488fc4e) |  |
| [`free`](#group__base_1gad1fb3be3451f07f6a6c2841a71105657) |  |
| [`empty`](#group__base_1gaa145cb6f2956777b6da95c31750645b8) |  |
| [`size`](#group__base_1ga82ead0d2085900b857d6e25b729eac8e) |  |
| [`get`](#group__base_1ga798a0d970cc984132442f082d6e80484) |  |
| [`clear`](#group__base_1gaff3c5cd97f75e7f56401aa18d13b39c3) |  |

---

#### AbstractCollection 

```cpp
inline AbstractCollection()
```

---

#### ~AbstractCollection 

```cpp
virtual inline ~AbstractCollection()
```

---

#### add 

```cpp
bool add(const TKey & key, TValue * item, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `TValue *` |  |
| `whiny` | `bool` |  |

---

#### remove 

```cpp
bool remove(const TValue * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `const TValue *` |  |

---

#### remove 

```cpp
TValue * remove(const TKey & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### exists 

```cpp
bool exists(const TKey & key) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### exists 

```cpp
bool exists(const TValue * item) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `const TValue *` |  |

---

#### free 

```cpp
bool free(const TKey & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### empty 

```cpp
bool empty() const
```

---

#### size 

```cpp
size_t size() const
```

---

#### get 

```cpp
TValue * get(const TKey & key, bool whiny) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `whiny` | `bool` |  |

---

#### clear 

```cpp
void clear()
```

## PointerCollection 

> **Extends:** `icy::AbstractCollection< TKey, TValue >`
> **Subclasses:** `icy::LiveCollection< std::string, IUser >`, `icy::LiveCollection< std::string, Peer >`, `icy::LiveCollection< std::string, LocalPackage >`, `icy::LiveCollection< std::string, RemotePackage >`, `icy::TimedManager< std::string, Message >`, `icy::DiagnosticManager`, `icy::LiveCollection< TKey, TValue >`, `icy::TimedManager< TKey, TValue >`
> **Defined in:** `collection.h`

This collection is used to maintain a map of unique_ptr values indexed by key in a thread-safe way.

### Members

| Name | Description |
|------|-------------|
| [`PointerCollection`](#group__base_1ga4eb0b556b5bba9468b6bbc2930e264e6) |  |
| [`~PointerCollection`](#group__base_1ga04848af4625edc5ccbbbf719e6c17443) |  |
| [`add`](#group__base_1gaec876210d229bc9b9bf90bfe2926fb5f) |  |
| [`add`](#group__base_1ga62ff0911c59aa1a9aeac9d7698f1ab17) |  |
| [`update`](#group__base_1ga01b529a06faff43645cb5c67368d8e99) |  |
| [`get`](#group__base_1ga329a6bf84cb48cfab9250a608e35f691) |  |
| [`free`](#group__base_1ga67ee7cebf96b422b4584fe123b800f41) |  |
| [`remove`](#group__base_1ga5efa219cb476f3a59a592386f6a16081) |  |
| [`remove`](#group__base_1gaa37d26de497e938147192a5d668b34ae) |  |
| [`exists`](#group__base_1gaa950f6b1d1e040da88873be453eda1b5) |  |
| [`exists`](#group__base_1ga8fe0e3813f6a30ecd11702bba3e1514d) |  |
| [`empty`](#group__base_1gace7b19f0e9bad5510467a46596e13d37) |  |
| [`size`](#group__base_1ga3ba7fedd8251e52569ff8e6baaf93b30) |  |
| [`clear`](#group__base_1ga8b8ccb87af37bfe3a6053b804cbe24c8) |  |
| [`map`](#group__base_1ga14adc685db1957fe0bed158aa837c4c5) |  |
| [`map`](#group__base_1gab1e7530c91ffd91d4c06bbc3038f0a62) |  |
| [`onAdd`](#group__base_1gac3a8305d5d3342ed8e4b9771458eac36) |  |
| [`onRemove`](#group__base_1ga95c4d1eb5fdb1b79c6fbe5fa1ee65f37) |  |
| [`_mutex`](#group__base_1ga8fe93e84c7b64c29650da99cb6be530e) |  |
| [`_map`](#group__base_1ga46a0cda25005bad73a3b8e8751cb21da) |  |

---

#### PointerCollection 

```cpp
inline PointerCollection()
```

---

#### ~PointerCollection 

```cpp
virtual inline ~PointerCollection()
```

---

#### add 

```cpp
virtual inline bool add(const TKey & key, TValue * item, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `TValue *` |  |
| `whiny` | `bool` |  |

---

#### add 

```cpp
virtual inline bool add(const TKey & key, std::unique_ptr< TValue > item, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `std::unique_ptr< TValue >` |  |
| `whiny` | `bool` |  |

---

#### update 

```cpp
virtual inline void update(const TKey & key, std::unique_ptr< TValue > item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `std::unique_ptr< TValue >` |  |

---

#### get 

```cpp
virtual inline TValue * get(const TKey & key, bool whiny) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `whiny` | `bool` |  |

---

#### free 

```cpp
virtual inline bool free(const TKey & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### remove 

```cpp
virtual inline TValue * remove(const TKey & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### remove 

```cpp
virtual inline bool remove(const TValue * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `const TValue *` |  |

---

#### exists 

```cpp
virtual inline bool exists(const TKey & key) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### exists 

```cpp
virtual inline bool exists(const TValue * item) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `const TValue *` |  |

---

#### empty 

```cpp
virtual inline bool empty() const
```

---

#### size 

```cpp
virtual inline size_t size() const
```

---

#### clear 

```cpp
virtual inline void clear()
```

---

#### map 

```cpp
virtual inline Map & map()
```

---

#### map 

```cpp
virtual inline const Map & map() const
```

---

#### onAdd 

```cpp
virtual inline void onAdd(const TKey &, TValue *)
```

---

#### onRemove 

```cpp
virtual inline void onRemove(const TKey &, TValue *)
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _map 

```cpp
Map _map
```

## LiveCollection 

> **Extends:** `icy::PointerCollection< TKey, TValue >`
> **Subclasses:** `icy::StreamManager`
> **Defined in:** `collection.h`

### Members

| Name | Description |
|------|-------------|
| [`onAdd`](#group__base_1gaae1485c677aab1d47b09ff809c7fc9dd) |  |
| [`onRemove`](#group__base_1ga27214e644373169b247aa87e8603b761) |  |
| [`ItemAdded`](#group__base_1gae557422bf75d0ad6c0a2c2910b8833ec) |  |
| [`ItemRemoved`](#group__base_1ga37d011126f82ca545769f0fd8d3859d5) |  |

---

#### onAdd 

```cpp
virtual inline void onAdd(const TKey &, TValue * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `TValue *` |  |

---

#### onRemove 

```cpp
virtual inline void onRemove(const TKey &, TValue * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `TValue *` |  |

---

#### ItemAdded 

```cpp
Signal< void(TValue &)> ItemAdded
```

---

#### ItemRemoved 

```cpp
Signal< void(const TValue &)> ItemRemoved
```

## KVCollection 

> **Defined in:** `collection.h`

Reusable stack based unique key-value store for DRY coding.

### Members

| Name | Description |
|------|-------------|
| [`KVCollection`](#group__base_1ga91a2efa7efe06f01164d4cf805381851) |  |
| [`~KVCollection`](#group__base_1gac9ac9c331dc55310741b1f3b87b5269a) |  |
| [`add`](#group__base_1gaee5cb65f2a4800e23984ac012fc50c73) |  |
| [`get`](#group__base_1ga4b943a0998cb16a67d6a0b525b9dc536) |  |
| [`get`](#group__base_1ga7979bc09410a279c804927f41d16dae5) |  |
| [`remove`](#group__base_1ga3fdb425c32ea35e45a5577955ec3bdd4) |  |
| [`has`](#group__base_1ga8b8c96f803ae6b0c701fd02e29104f66) |  |
| [`empty`](#group__base_1ga6f9493de942d040964118ce00feb208a) |  |
| [`size`](#group__base_1gac38f3417c1421b8e889a283ee657261c) |  |
| [`clear`](#group__base_1gaeb411422a7704fd35374f0d931095e69) |  |
| [`map`](#group__base_1ga29c7802b4df2c44c170ccd5e66b0ac0d) |  |
| [`_map`](#group__base_1gaacf1b4202998f2ba32b06e6e340c1643) |  |

---

#### KVCollection 

```cpp
inline KVCollection()
```

---

#### ~KVCollection 

```cpp
virtual inline ~KVCollection()
```

---

#### add 

```cpp
virtual inline bool add(const TKey & key, const TValue & item, bool update, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `const TValue &` |  |
| `update` | `bool` |  |
| `whiny` | `bool` |  |

---

#### get 

```cpp
virtual inline TValue & get(const TKey & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### get 

```cpp
virtual inline const TValue & get(const TKey & key, const TValue & defaultValue) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `defaultValue` | `const TValue &` |  |

---

#### remove 

```cpp
virtual inline bool remove(const TKey & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### has 

```cpp
virtual inline bool has(const TKey & key) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |

---

#### empty 

```cpp
virtual inline bool empty() const
```

---

#### size 

```cpp
virtual inline size_t size() const
```

---

#### clear 

```cpp
virtual inline void clear()
```

---

#### map 

```cpp
virtual inline Map & map()
```

---

#### _map 

```cpp
Map _map
```

## NVCollection 

> **Subclasses:** `icy::http::FormWriter`, `icy::http::Message`
> **Defined in:** `collection.h`

A storage container for a name value collections. This collection can store multiple entries for each name, and it's getters are case-insensitive.

### Members

| Name | Description |
|------|-------------|
| [`NVCollection`](#group__base_1ga41eb71d1735c8238b24dcda700c23d21) |  |
| [`NVCollection`](#group__base_1gaca0d4928654fd5b79fd06612222080b9) |  |
| [`~NVCollection`](#group__base_1gaed8abfccd111eea9b5cabe59ef8aac46) |  |
| [`operator=`](#group__base_1ga6e9938e6b926c62d4ccbea0c7a623dec) | Assigns the name-value pairs of another [NVCollection](#classicy_1_1NVCollection) to this one. |
| [`operator[]`](#group__base_1gab76fc12abf8a9f13ac81e32d9788cea3) | Returns the value of the (first) name-value pair with the given name. |
| [`set`](#group__base_1ga36860b3fb325b24ed969668192d2d5ad) | Sets the value of the (first) name-value pair with the given name. |
| [`add`](#group__base_1gae0cbccd7da15dd37eed7631c853c2565) | Adds a new name-value pair with the given name and value. |
| [`get`](#group__base_1ga02db5c5230e78a2f8272b0380ac8fe2b) | Returns the value of the first name-value pair with the given name. |
| [`get`](#group__base_1ga5b609c360028a9ec67aee8fb1c6e543d) | Returns the value of the first name-value pair with the given name. If no value with the given name has been found, the defaultValue is returned. |
| [`has`](#group__base_1ga01251074fc906db8ec5abaaaf084fdad) | Returns true if there is at least one name-value pair with the given name. |
| [`find`](#group__base_1ga07ca18d979f83a19bcf526357e45d2e8) | Returns an iterator pointing to the first name-value pair with the given name. |
| [`begin`](#group__base_1gaef153806a4bf20b97530fcb8c0cb0c5f) | Returns an iterator pointing to the begin of the name-value pair collection. |
| [`end`](#group__base_1ga5de16e354315a6c291156deef03b3229) | Returns an iterator pointing to the end of the name-value pair collection. |
| [`empty`](#group__base_1gaf563c36905a2529054785a1734d46f67) | Returns true iff the header does not have any content. |
| [`size`](#group__base_1gae1a0d58fc01bd230cf5497d8e5ae398f) | Returns the number of name-value pairs in the collection. |
| [`erase`](#group__base_1gab295478be7ae8a444d0071c96f4108ff) | Removes all name-value pairs with the given name. |
| [`clear`](#group__base_1ga73d773429898c1c3ba526155b6cc0b90) | Removes all name-value pairs and their values. |
| [`_map`](#group__base_1ga7d5555328212afe80f3421437edc5dd5) |  |

---

#### NVCollection 

```cpp
inline NVCollection()
```

---

#### NVCollection 

```cpp
inline NVCollection(const NVCollection & nvc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `nvc` | `const NVCollection &` |  |

---

#### ~NVCollection 

```cpp
virtual inline ~NVCollection()
```

---

#### operator= 

```cpp
NVCollection & operator=(const NVCollection & nvc)
```

Assigns the name-value pairs of another [NVCollection](#classicy_1_1NVCollection) to this one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `nvc` | `const NVCollection &` |  |

---

#### operator[] 

```cpp
const std::string & operator[](const std::string & name) const
```

Returns the value of the (first) name-value pair with the given name.

Throws a NotFoundException if the name-value pair does not exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### set 

```cpp
void set(const std::string & name, const std::string & value)
```

Sets the value of the (first) name-value pair with the given name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### add 

```cpp
void add(const std::string & name, const std::string & value)
```

Adds a new name-value pair with the given name and value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### get 

```cpp
const std::string & get(const std::string & name) const
```

Returns the value of the first name-value pair with the given name.

Throws a NotFoundException if the name-value pair does not exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### get 

```cpp
const std::string & get(const std::string & name, const std::string & defaultValue) const
```

Returns the value of the first name-value pair with the given name. If no value with the given name has been found, the defaultValue is returned.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `defaultValue` | `const std::string &` |  |

---

#### has 

```cpp
bool has(const std::string & name) const
```

Returns true if there is at least one name-value pair with the given name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### find 

```cpp
ConstIterator find(const std::string & name) const
```

Returns an iterator pointing to the first name-value pair with the given name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### begin 

```cpp
ConstIterator begin() const
```

Returns an iterator pointing to the begin of the name-value pair collection.

---

#### end 

```cpp
ConstIterator end() const
```

Returns an iterator pointing to the end of the name-value pair collection.

---

#### empty 

```cpp
bool empty() const
```

Returns true iff the header does not have any content.

---

#### size 

```cpp
int size() const
```

Returns the number of name-value pairs in the collection.

---

#### erase 

```cpp
void erase(const std::string & name)
```

Removes all name-value pairs with the given name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### clear 

```cpp
void clear()
```

Removes all name-value pairs and their values.

---

#### _map 

```cpp
Map _map
```

## ILT 

> **Defined in:** `collection.h`

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__base_1ga966ae0464dd2d1011bcd55702729b5eb) |  |

---

#### operator() 

```cpp
inline bool operator()(const std::string & s1, const std::string & s2) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `s1` | `const std::string &` |  |
| `s2` | `const std::string &` |  |

## Configuration 

> **Subclasses:** `icy::json::Configuration`
> **Defined in:** `configuration.h`

[Configuration](#classicy_1_1Configuration) is an abstract base class for managing different kinds of configuration storage back ends such as JSON, XML, or database.

Subclasses must override the [getRaw()](#group__base_1ga1246d954823215df6d932e78a08bc23b) and [setRaw()](#group__base_1gac6a4023c271c2357334275f15329310d) and methods.

This class is safe for multithreaded use.

### Members

| Name | Description |
|------|-------------|
| [`Configuration`](#group__base_1ga17ab598f09508375af5c81bcca2f7adf) | Creates the [Configuration](#classicy_1_1Configuration). |
| [`~Configuration`](#group__base_1ga92fedd22f2c662f1fd5ba513dedaad45) | Destroys the [Configuration](#classicy_1_1Configuration). |
| [`exists`](#group__base_1ga46b7b29d78f0cf2939e9ffb3e561652c) | Returns true if the property with the given key exists. |
| [`getString`](#group__base_1ga05bb2078ab85e6d01e07bd0339e7f051) | Returns the string value of the property with the given name. Throws a NotFoundException if the key does not exist. |
| [`getString`](#group__base_1ga759bde263c84572e2dca86974b79abe0) | If a property with the given key exists, returns the property's string value, otherwise returns the given default value. |
| [`getRawString`](#group__base_1ga32c5e46d389028da0c63b71059b261f1) | Returns the raw string value of the property with the given name. Throws a NotFoundException if the key does not exist. References to other properties are not expanded. |
| [`getRawString`](#group__base_1ga39dea64c1f5a83216ed7f1de59fbc1ce) | If a property with the given key exists, returns the property's raw string value, otherwise returns the given default value. References to other properties are not expanded. |
| [`getInt`](#group__base_1gae4002dbb61ede2e81e518474ebc49057) | Returns the int value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal. |
| [`getInt`](#group__base_1ga8b3ab025bfee6fdd2b9e83a75f892ca4) | If a property with the given key exists, returns the property's int value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal. |
| [`getLargeInt`](#group__base_1gad069788603d90de791b983999d127f88) | Returns the int value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal. |
| [`getLargeInt`](#group__base_1ga4048a586cef122d39c1553228c407524) | If a property with the given key exists, returns the property's int value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal. |
| [`getDouble`](#group__base_1gabb0a768d637e028d16bd14ddb3c6eeeb) | Returns the double value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to a double. |
| [`getDouble`](#group__base_1gaa570bb19020ae6f6d1afa8b5e099da84) | If a property with the given key exists, returns the property's double value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to an double. |
| [`getBool`](#group__base_1ga18f5f14dbe585841a2c97e3e6fabe923) | Returns the double value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to a double. |
| [`getBool`](#group__base_1gacf0956fc36e268cbc074e11ac8975b54) | If a property with the given key exists, returns the property's bool value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to a boolean. The following string values can be converted into a boolean: |
| [`setString`](#group__base_1gaa6619e41320857455ea04e4cf5dc5ada) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`setInt`](#group__base_1ga9668ea52c509d5ee335ca194402b0a8c) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`setLargeInt`](#group__base_1ga82ff1ce7b02ab4989669913175970cd2) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`setDouble`](#group__base_1ga0c39abf4df23106c7b47221fe9cc3252) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`setBool`](#group__base_1ga6130c066e06d2f3c8554160d216058bf) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`getRaw`](#group__base_1ga1246d954823215df6d932e78a08bc23b) | If the property with the given key exists, stores the property's value in value and returns true. Otherwise, returns false. |
| [`setRaw`](#group__base_1gac6a4023c271c2357334275f15329310d) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`Configuration`](#group__base_1ga138e9fc74cc2ade87e6786cb0423be9a) |  |
| [`operator=`](#group__base_1ga917d8e07fffa63872970f751ea20eab6) |  |
| [`parseInt`](#group__base_1ga3953767cc1696a6d52453c3c5ed9fc29) |  |
| [`parseLargeInt`](#group__base_1gaefe4edd971f9b5156ba539a93d0a4bae) |  |
| [`parseBool`](#group__base_1gac311aca82c3ab375ae07e099f7889761) |  |
| [`PropertyChanged`](#group__base_1ga885286a3a5e03556d0aab503afa8952d) | The Key and Value of the changed configuration property. |
| [`_mutex`](#group__base_1gaad886cabbbd16422e1cf70c146d2cae1) |  |

---

#### Configuration 

```cpp
Configuration()
```

Creates the [Configuration](#classicy_1_1Configuration).

---

#### ~Configuration 

```cpp
virtual ~Configuration()
```

Destroys the [Configuration](#classicy_1_1Configuration).

---

#### exists 

```cpp
bool exists(const std::string & key) const
```

Returns true if the property with the given key exists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getString 

```cpp
std::string getString(const std::string & key) const
```

Returns the string value of the property with the given name. Throws a NotFoundException if the key does not exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getString 

```cpp
std::string getString(const std::string & key, const std::string & defaultValue) const
```

If a property with the given key exists, returns the property's string value, otherwise returns the given default value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `const std::string &` |  |

---

#### getRawString 

```cpp
std::string getRawString(const std::string & key) const
```

Returns the raw string value of the property with the given name. Throws a NotFoundException if the key does not exist. References to other properties are not expanded.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getRawString 

```cpp
std::string getRawString(const std::string & key, const std::string & defaultValue) const
```

If a property with the given key exists, returns the property's raw string value, otherwise returns the given default value. References to other properties are not expanded.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `const std::string &` |  |

---

#### getInt 

```cpp
int getInt(const std::string & key) const
```

Returns the int value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getInt 

```cpp
int getInt(const std::string & key, int defaultValue) const
```

If a property with the given key exists, returns the property's int value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `int` |  |

---

#### getLargeInt 

```cpp
std::int64_t getLargeInt(const std::string & key) const
```

Returns the int value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getLargeInt 

```cpp
std::int64_t getLargeInt(const std::string & key, std::int64_t defaultValue) const
```

If a property with the given key exists, returns the property's int value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to an int. Numbers starting with 0x are treated as hexadecimal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `std::int64_t` |  |

---

#### getDouble 

```cpp
double getDouble(const std::string & key) const
```

Returns the double value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to a double.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getDouble 

```cpp
double getDouble(const std::string & key, double defaultValue) const
```

If a property with the given key exists, returns the property's double value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to an double.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `double` |  |

---

#### getBool 

```cpp
bool getBool(const std::string & key) const
```

Returns the double value of the property with the given name. Throws a NotFoundException if the key does not exist. Throws a SyntaxException if the property can not be converted to a double.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getBool 

```cpp
bool getBool(const std::string & key, bool defaultValue) const
```

If a property with the given key exists, returns the property's bool value, otherwise returns the given default value. Throws a SyntaxException if the property can not be converted to a boolean. The following string values can be converted into a boolean:

* numerical values: non zero becomes true, zero becomes false

* strings: true, yes, on become true, false, no, off become false Case does not matter.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `bool` |  |

---

#### setString 

```cpp
void setString(const std::string & key, const std::string & value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### setInt 

```cpp
void setInt(const std::string & key, int value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `int` |  |

---

#### setLargeInt 

```cpp
void setLargeInt(const std::string & key, std::int64_t value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `std::int64_t` |  |

---

#### setDouble 

```cpp
void setDouble(const std::string & key, double value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `double` |  |

---

#### setBool 

```cpp
void setBool(const std::string & key, bool value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `bool` |  |

---

#### getRaw 

```cpp
bool getRaw(const std::string & key, std::string & value) const
```

If the property with the given key exists, stores the property's value in value and returns true. Otherwise, returns false.

Must be overridden by subclasses.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `std::string &` |  |

---

#### setRaw 

```cpp
void setRaw(const std::string & key, const std::string & value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

The implementation is responsible for emitting the PropertyChanged signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### Configuration 

```cpp
Configuration(const Configuration &) = delete
```

---

#### operator= 

```cpp
Configuration & operator=(const Configuration &) = delete
```

---

#### parseInt 

```cpp
static int parseInt(const std::string & value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### parseLargeInt 

```cpp
static std::int64_t parseLargeInt(const std::string & value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### parseBool 

```cpp
static bool parseBool(const std::string & value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const std::string &` |  |

---

#### PropertyChanged 

```cpp
Signal< void(const std::string &, const std::string &)> PropertyChanged
```

The Key and Value of the changed configuration property.

---

#### _mutex 

```cpp
std::mutex _mutex
```

## ScopedConfiguration 

> **Defined in:** `configuration.h`

[ScopedConfiguration](#classicy_1_1ScopedConfiguration) provides multiple levels of configuration for a module. Multiple levels means that there is a module level scope, and a default scope. When a property is accessed, the module scope value will be used if available, otherwise the default scope value will be used.

Example scoping: Module: channels.[name].modes.[name].[value] Default: modes.[name].[value]

### Members

| Name | Description |
|------|-------------|
| [`ScopedConfiguration`](#group__base_1gab5e9573cc6a94c6b942aeb8fa75982bf) |  |
| [`ScopedConfiguration`](#group__base_1ga2d9a45d573b97ec8d2fa4c5496bdedfa) |  |
| [`getString`](#group__base_1ga815efb31af29d03a283bd511d73865e1) |  |
| [`getInt`](#group__base_1ga054a3db8ef6052a2c244dfbb13309b9f) |  |
| [`getDouble`](#group__base_1ga2b3857cbbca439b3b825ce7f521fa061) |  |
| [`getBool`](#group__base_1ga7fe7def68b732c44e7c3835b89cc53dd) |  |
| [`setString`](#group__base_1ga4d0ed2a990ce5ab5b28c2ba2e06b13ad) |  |
| [`setInt`](#group__base_1ga94aac2d52d9eb5689797986c40edfabf) |  |
| [`setDouble`](#group__base_1ga72aa7eab9ef3496e461b5593b5edb8bf) |  |
| [`setBool`](#group__base_1ga018ae27a4b9d4d2344df5723b1b01635) |  |
| [`getCurrentScope`](#group__base_1ga36f148de5018eff36e52c0c990b039dc) |  |
| [`getDafaultKey`](#group__base_1gac6effdc628b5f325c4585d0a06322894) |  |
| [`getScopedKey`](#group__base_1ga1b6e232dfd5b13c1eaff4408adc03b77) |  |
| [`operator=`](#group__base_1gac4d499e6d6ffa9b911bddebb027dcc38) |  |
| [`config`](#group__base_1gab789db5bbe6952f38128f6a477b6cb45) |  |
| [`currentScope`](#group__base_1gae55030c57f6402acba43d7d3ea38a415) |  |
| [`defaultScope`](#group__base_1ga77d05f9ce766ee3f0a84a313ae83426b) |  |

---

#### ScopedConfiguration 

```cpp
ScopedConfiguration(Configuration & config, const std::string & currentScope, const std::string & defaultScope)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `config` | `Configuration &` |  |
| `currentScope` | `const std::string &` |  |
| `defaultScope` | `const std::string &` |  |

---

#### ScopedConfiguration 

```cpp
ScopedConfiguration(const ScopedConfiguration & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const ScopedConfiguration &` |  |

---

#### getString 

```cpp
std::string getString(const std::string & key, const std::string & defaultValue, bool forceDefaultScope) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `const std::string &` |  |
| `forceDefaultScope` | `bool` |  |

---

#### getInt 

```cpp
int getInt(const std::string & key, int defaultValue, bool forceDefaultScope) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `int` |  |
| `forceDefaultScope` | `bool` |  |

---

#### getDouble 

```cpp
double getDouble(const std::string & key, double defaultValue, bool forceDefaultScope) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `double` |  |
| `forceDefaultScope` | `bool` |  |

---

#### getBool 

```cpp
bool getBool(const std::string & key, bool defaultValue, bool forceDefaultScope) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultValue` | `bool` |  |
| `forceDefaultScope` | `bool` |  |

---

#### setString 

```cpp
void setString(const std::string & key, const std::string & value, bool defaultScope)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `const std::string &` |  |
| `defaultScope` | `bool` |  |

---

#### setInt 

```cpp
void setInt(const std::string & key, int value, bool defaultScope)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `int` |  |
| `defaultScope` | `bool` |  |

---

#### setDouble 

```cpp
void setDouble(const std::string & key, double value, bool defaultScope)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `double` |  |
| `defaultScope` | `bool` |  |

---

#### setBool 

```cpp
void setBool(const std::string & key, bool value, bool defaultScope)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `bool` |  |
| `defaultScope` | `bool` |  |

---

#### getCurrentScope 

```cpp
std::string getCurrentScope(const std::string & key) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getDafaultKey 

```cpp
std::string getDafaultKey(const std::string & key) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### getScopedKey 

```cpp
std::string getScopedKey(const std::string & key, bool defaultScope) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `defaultScope` | `bool` |  |

---

#### operator= 

```cpp
ScopedConfiguration & operator=(const ScopedConfiguration &)
```

---

#### config 

```cpp
Configuration & config
```

---

#### currentScope 

```cpp
std::string currentScope
```

---

#### defaultScope 

```cpp
std::string defaultScope
```

## Timestamp 

> **Defined in:** `datetime.h`

A [Timestamp](#classicy_1_1Timestamp) stores a monotonic* time value with (theoretical) microseconds resolution. Timestamps can be compared with each other and simple arithmetics are supported.

[*] Note that [Timestamp](#classicy_1_1Timestamp) values are only monotonic as long as the systems's clock is monotonic as well (and not, e.g. set back).

Timestamps are UTC (Coordinated Universal Time) based and thus independent of the timezone in effect on the system.

### Members

| Name | Description |
|------|-------------|
| [`Timestamp`](#group__base_1ga59eb0caf11a4b42805be30b575c64685) | Creates a timestamp with the current time. |
| [`Timestamp`](#group__base_1ga634322242b4d434f27d0b549e6a2d02a) | Creates a timestamp from the given time value. |
| [`Timestamp`](#group__base_1gaee8a093ba4e7d09969d6e9d66d39c98c) | Copy constructor. |
| [`~Timestamp`](#group__base_1ga9454ae9b51eef4ddfd6a5fc517d5fb89) | Destroys the timestamp. |
| [`operator=`](#group__base_1gaa6f065d2bafe45160d5830ec7f3fea03) |  |
| [`operator=`](#group__base_1gad270942977ff386b8295e92d32a4247a) |  |
| [`swap`](#group__base_1ga051b185aa50b85119422c8069b31f9b1) | Swaps the [Timestamp](#classicy_1_1Timestamp) with another one. |
| [`update`](#group__base_1gae851ba7fcabd33ddf4b69029b93e7da1) | Updates the [Timestamp](#classicy_1_1Timestamp) with the current time. |
| [`operator==`](#group__base_1gaaff90e64401ed08703d4bab0b09bc5bd) |  |
| [`operator!=`](#group__base_1ga70b120698ef0ca3429d5e6035e03ab51) |  |
| [`operator>`](#group__base_1gacdc27b968a6f92a38dfa29f4ce0639ca) |  |
| [`operator>=`](#group__base_1gae4ddc243de02e4d233f681d9df760111) |  |
| [`operator<`](#group__base_1gae1ae154f8dc65bb9966411801d514f88) |  |
| [`operator<=`](#group__base_1ga18146b0b77bdc63ad3eaee69a460e38b) |  |
| [`operator+`](#group__base_1gaae2bf2fef867c4856c03637c4eaa6916) |  |
| [`operator-`](#group__base_1gafb9644c6db3560126efd053dfca80f7f) |  |
| [`operator-`](#group__base_1gaf904d00bddad7f6dd1ad05c7fd8592ee) |  |
| [`operator+=`](#group__base_1gab575836d90b63b74515022efa909b261) |  |
| [`operator-=`](#group__base_1ga4a845176e12d2116809c4c7b9b3fd940) |  |
| [`epochTime`](#group__base_1ga018ebc0e980ee7e467da6238010a4313) | Returns the timestamp expressed in time_t. time_t base time is midnight, January 1, 1970. Resolution is one second. |
| [`utcTime`](#group__base_1gac3ec372fc5bc417e7d38a7849d3bd173) | Returns the timestamp expressed in UTC-based time. UTC base time is midnight, October 15, 1582. Resolution is 100 nanoseconds. |
| [`epochMicroseconds`](#group__base_1ga9ed38faa45afc53468343bc1c89c6000) | Returns the timestamp expressed in microseconds since the Unix epoch, midnight, January 1, 1970. |
| [`elapsed`](#group__base_1gaee70f8a34b9712e25310f2bb642305b4) | Returns the time elapsed since the time denoted by the timestamp. Equivalent to [Timestamp()](#group__base_1ga59eb0caf11a4b42805be30b575c64685) - *this. |
| [`isElapsed`](#group__base_1gadf63758d952b2ee7aa4e00238e89e950) | Returns true iff the given interval has passed since the time denoted by the timestamp. |
| [`fromEpochTime`](#group__base_1gab8199d2f7572ad52e05e74512f402307) | Creates a timestamp from a std::time_t. |
| [`fromUtcTime`](#group__base_1gacef58d2de9fa06c922c0a8e2853d93c7) | Creates a timestamp from a UTC time value. |
| [`resolution`](#group__base_1ga66994444da248ff6b2dfd3f72768768d) | Returns the resolution in units per second. Since the timestamp has microsecond resolution, the returned value is always 1000000. |
| [`_ts`](#group__base_1gac3b48ed8bc4dbfb2de54ea9d3a4c2d04) |  |

---

#### Timestamp 

```cpp
Timestamp()
```

Creates a timestamp with the current time.

---

#### Timestamp 

```cpp
Timestamp(TimeVal tv)
```

Creates a timestamp from the given time value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tv` | `TimeVal` |  |

---

#### Timestamp 

```cpp
Timestamp(const Timestamp & other)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `const Timestamp &` |  |

---

#### ~Timestamp 

```cpp
~Timestamp()
```

Destroys the timestamp.

---

#### operator= 

```cpp
Timestamp & operator=(const Timestamp & other)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `const Timestamp &` |  |

---

#### operator= 

```cpp
Timestamp & operator=(TimeVal tv)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `tv` | `TimeVal` |  |

---

#### swap 

```cpp
void swap(Timestamp & timestamp)
```

Swaps the [Timestamp](#classicy_1_1Timestamp) with another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timestamp` | `Timestamp &` |  |

---

#### update 

```cpp
void update()
```

Updates the [Timestamp](#classicy_1_1Timestamp) with the current time.

---

#### operator== 

```cpp
inline bool operator==(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator!= 

```cpp
inline bool operator!=(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator> 

```cpp
inline bool operator>(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator>= 

```cpp
inline bool operator>=(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator< 

```cpp
inline bool operator<(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator<= 

```cpp
inline bool operator<=(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator+ 

```cpp
inline Timestamp operator+(TimeDiff d) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `TimeDiff` |  |

---

#### operator- 

```cpp
inline Timestamp operator-(TimeDiff d) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `TimeDiff` |  |

---

#### operator- 

```cpp
inline TimeDiff operator-(const Timestamp & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timestamp &` |  |

---

#### operator+= 

```cpp
inline Timestamp & operator+=(TimeDiff d)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `TimeDiff` |  |

---

#### operator-= 

```cpp
inline Timestamp & operator-=(TimeDiff d)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `TimeDiff` |  |

---

#### epochTime 

```cpp
inline std::time_t epochTime() const
```

Returns the timestamp expressed in time_t. time_t base time is midnight, January 1, 1970. Resolution is one second.

---

#### utcTime 

```cpp
inline UtcTimeVal utcTime() const
```

Returns the timestamp expressed in UTC-based time. UTC base time is midnight, October 15, 1582. Resolution is 100 nanoseconds.

---

#### epochMicroseconds 

```cpp
inline TimeVal epochMicroseconds() const
```

Returns the timestamp expressed in microseconds since the Unix epoch, midnight, January 1, 1970.

---

#### elapsed 

```cpp
inline TimeDiff elapsed() const
```

Returns the time elapsed since the time denoted by the timestamp. Equivalent to [Timestamp()](#group__base_1ga59eb0caf11a4b42805be30b575c64685) - *this.

---

#### isElapsed 

```cpp
inline bool isElapsed(TimeDiff interval) const
```

Returns true iff the given interval has passed since the time denoted by the timestamp.

| Parameter | Type | Description |
|-----------|------|-------------|
| `interval` | `TimeDiff` |  |

---

#### fromEpochTime 

```cpp
static Timestamp fromEpochTime(std::time_t t)
```

Creates a timestamp from a std::time_t.

| Parameter | Type | Description |
|-----------|------|-------------|
| `t` | `std::time_t` |  |

---

#### fromUtcTime 

```cpp
static Timestamp fromUtcTime(UtcTimeVal val)
```

Creates a timestamp from a UTC time value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `val` | `UtcTimeVal` |  |

---

#### resolution 

```cpp
static inline TimeVal resolution()
```

Returns the resolution in units per second. Since the timestamp has microsecond resolution, the returned value is always 1000000.

---

#### _ts 

```cpp
TimeVal _ts
```

## Timespan 

> **Defined in:** `datetime.h`

A class that represents time spans up to microsecond resolution.

### Members

| Name | Description |
|------|-------------|
| [`Timespan`](#group__base_1ga183ab023a03182a6b62287f31ba4667c) | Creates a zero [Timespan](#classicy_1_1Timespan). |
| [`Timespan`](#group__base_1ga36bd62906099ad73b6411bbee9df4687) | Creates a [Timespan](#classicy_1_1Timespan). |
| [`Timespan`](#group__base_1ga9281640ae0cfdf1af90bf219e3ab2502) | Creates a [Timespan](#classicy_1_1Timespan). Useful for creating a [Timespan](#classicy_1_1Timespan) from a struct timeval. |
| [`Timespan`](#group__base_1gad7c52f4c3f97829b1a2184819c6fdb78) | Creates a [Timespan](#classicy_1_1Timespan). |
| [`Timespan`](#group__base_1gad644dad5d9fcc72c753fb207aa0f7799) | Creates a [Timespan](#classicy_1_1Timespan) from another one. |
| [`~Timespan`](#group__base_1ga58f2185a33f9adf2043381a216e7e3d0) | Destroys the [Timespan](#classicy_1_1Timespan). |
| [`operator=`](#group__base_1gacb09eb0c421d3ee1567221bbcbfa1686) | Assignment operator. |
| [`operator=`](#group__base_1ga53a89a4c0d074d07a69651f9b1390ca2) | Assignment operator. |
| [`assign`](#group__base_1ga8aeb84fe300f38a39ec0ee7cd4a2c463) | Assigns a new span. |
| [`assign`](#group__base_1ga326e06380b6c271143dd53261061e0a7) | Assigns a new span. Useful for assigning from a struct timeval. |
| [`swap`](#group__base_1ga3289402362d3f09413c7e27c8a5f84a7) | Swaps the [Timespan](#classicy_1_1Timespan) with another one. |
| [`operator==`](#group__base_1ga6901eaae07320f3bbf4cbcbdcba58788) |  |
| [`operator!=`](#group__base_1gac44da14b266ce4dbf80779330cb7090d) |  |
| [`operator>`](#group__base_1ga7add4d0610dc0b197739f9b3d68dc39f) |  |
| [`operator>=`](#group__base_1gad19debbff7b0f7cad21fab77e589e982) |  |
| [`operator<`](#group__base_1ga11f56482aeb54a4a1e787750d8e1bd5d) |  |
| [`operator<=`](#group__base_1gaf862e58fa751b510a38233b11aa2c658) |  |
| [`operator==`](#group__base_1ga337882f65d1c794d461573d928877ad9) |  |
| [`operator!=`](#group__base_1gab46f76d0e8a8ed7a150a20ffeb02d337) |  |
| [`operator>`](#group__base_1ga3c356ed1db34e4a04abfdf8dae5d0a7f) |  |
| [`operator>=`](#group__base_1ga6a10617e981175a0672fc9532edf5fd6) |  |
| [`operator<`](#group__base_1ga57fa8c22b3b06fa8884b1dae53b2974d) |  |
| [`operator<=`](#group__base_1gaa9c1dfaca2cac4fe2da24ca6e835e808) |  |
| [`operator+`](#group__base_1gacdafb69117cce505b5c3290eff82e231) |  |
| [`operator-`](#group__base_1ga036be956f49e4ee5b3c7974c49c84689) |  |
| [`operator+=`](#group__base_1gac6abe30a87d689470bce966fe2bca4f1) |  |
| [`operator-=`](#group__base_1gab466c189ee0189435fda807fce532333) |  |
| [`operator+`](#group__base_1gae336ef6426d9886f13f62e3e0326a92b) |  |
| [`operator-`](#group__base_1ga96a28c1241b0c065e3c213ab4cc44cf4) |  |
| [`operator+=`](#group__base_1gadfd1a60c223de14e723fbb03e15f157f) |  |
| [`operator-=`](#group__base_1ga476bf78a0dea511c9dbe6c99ec86ca6c) |  |
| [`days`](#group__base_1gab39bf74d96f285b9502b0411321c2c40) | Returns the number of days. |
| [`hours`](#group__base_1ga6887a573816f2fdc9019e1e42b15a1fa) | Returns the number of hours (0 to 23). |
| [`totalHours`](#group__base_1ga4c78c96a19b68218bc1ec9056870dd48) | Returns the total number of hours. |
| [`minutes`](#group__base_1ga9b4fa5de6b469773c139d3992c368580) | Returns the number of minutes (0 to 59). |
| [`totalMinutes`](#group__base_1ga8dc72898fece5e76f4e89de4a60483ca) | Returns the total number of minutes. |
| [`seconds`](#group__base_1ga6a1f8b3aaadca7e4092b76e860e11473) | Returns the number of seconds (0 to 59). |
| [`totalSeconds`](#group__base_1ga2d936ac7dd1d8c6e3b445992c08fb0da) | Returns the total number of seconds. |
| [`milliseconds`](#group__base_1ga2d17d492ef454811b4487c210f6f4fa4) | Returns the number of milliseconds (0 to 999). |
| [`totalMilliseconds`](#group__base_1ga5301028400cc25b3ff5f1f34e85e3612) | Returns the total number of milliseconds. |
| [`microseconds`](#group__base_1ga038a3be32de4ebe87b3fe3676a327a04) | Returns the fractions of a millisecond in microseconds (0 to 999). |
| [`useconds`](#group__base_1ga4dbdae0aa5912a70c8f3497560fce318) | Returns the fractions of a second in microseconds (0 to 999999). |
| [`totalMicroseconds`](#group__base_1ga9fe3ab13665735f41ab6fd8ed8ae477b) | Returns the total number of microseconds. |
| [`MILLISECONDS`](#group__base_1gaf9dfc80ff279a8e33a4590fc66103084) | The number of microseconds in a millisecond. |
| [`SECONDS`](#group__base_1ga17ba35f40e7272d4491d98da4d79a31c) | The number of microseconds in a second. |
| [`MINUTES`](#group__base_1gacecea48ab9088a739906f31ad1cbcb1c) | The number of microseconds in a minute. |
| [`HOURS`](#group__base_1ga1414c0cb94c9e31c48cea196aa6bfef2) | The number of microseconds in a hour. |
| [`DAYS`](#group__base_1ga71586bb12510718ac92b58daf6b26795) | The number of microseconds in a day. |
| [`_span`](#group__base_1gaa2e7e7aebeb68bd2ccdf5431fc3a6bb3) |  |

---

#### Timespan 

```cpp
Timespan()
```

Creates a zero [Timespan](#classicy_1_1Timespan).

---

#### Timespan 

```cpp
Timespan(TimeDiff microseconds)
```

Creates a [Timespan](#classicy_1_1Timespan).

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### Timespan 

```cpp
Timespan(long seconds, long microseconds)
```

Creates a [Timespan](#classicy_1_1Timespan). Useful for creating a [Timespan](#classicy_1_1Timespan) from a struct timeval.

| Parameter | Type | Description |
|-----------|------|-------------|
| `seconds` | `long` |  |
| `microseconds` | `long` |  |

---

#### Timespan 

```cpp
Timespan(int days, int hours, int minutes, int seconds, int microseconds)
```

Creates a [Timespan](#classicy_1_1Timespan).

| Parameter | Type | Description |
|-----------|------|-------------|
| `days` | `int` |  |
| `hours` | `int` |  |
| `minutes` | `int` |  |
| `seconds` | `int` |  |
| `microseconds` | `int` |  |

---

#### Timespan 

```cpp
Timespan(const Timespan & timespan)
```

Creates a [Timespan](#classicy_1_1Timespan) from another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timespan` | `const Timespan &` |  |

---

#### ~Timespan 

```cpp
~Timespan()
```

Destroys the [Timespan](#classicy_1_1Timespan).

---

#### operator= 

```cpp
Timespan & operator=(const Timespan & timespan)
```

Assignment operator.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timespan` | `const Timespan &` |  |

---

#### operator= 

```cpp
Timespan & operator=(TimeDiff microseconds)
```

Assignment operator.

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### assign 

```cpp
Timespan & assign(int days, int hours, int minutes, int seconds, int microseconds)
```

Assigns a new span.

| Parameter | Type | Description |
|-----------|------|-------------|
| `days` | `int` |  |
| `hours` | `int` |  |
| `minutes` | `int` |  |
| `seconds` | `int` |  |
| `microseconds` | `int` |  |

---

#### assign 

```cpp
Timespan & assign(long seconds, long microseconds)
```

Assigns a new span. Useful for assigning from a struct timeval.

| Parameter | Type | Description |
|-----------|------|-------------|
| `seconds` | `long` |  |
| `microseconds` | `long` |  |

---

#### swap 

```cpp
void swap(Timespan & timespan)
```

Swaps the [Timespan](#classicy_1_1Timespan) with another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timespan` | `Timespan &` |  |

---

#### operator== 

```cpp
inline bool operator==(const Timespan & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timespan &` |  |

---

#### operator!= 

```cpp
inline bool operator!=(const Timespan & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timespan &` |  |

---

#### operator> 

```cpp
inline bool operator>(const Timespan & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timespan &` |  |

---

#### operator>= 

```cpp
inline bool operator>=(const Timespan & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timespan &` |  |

---

#### operator< 

```cpp
inline bool operator<(const Timespan & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timespan &` |  |

---

#### operator<= 

```cpp
inline bool operator<=(const Timespan & ts) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ts` | `const Timespan &` |  |

---

#### operator== 

```cpp
inline bool operator==(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator!= 

```cpp
inline bool operator!=(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator> 

```cpp
inline bool operator>(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator>= 

```cpp
inline bool operator>=(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator< 

```cpp
inline bool operator<(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator<= 

```cpp
inline bool operator<=(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator+ 

```cpp
Timespan operator+(const Timespan & d) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `const Timespan &` |  |

---

#### operator- 

```cpp
Timespan operator-(const Timespan & d) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `const Timespan &` |  |

---

#### operator+= 

```cpp
Timespan & operator+=(const Timespan & d)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `const Timespan &` |  |

---

#### operator-= 

```cpp
Timespan & operator-=(const Timespan & d)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `d` | `const Timespan &` |  |

---

#### operator+ 

```cpp
Timespan operator+(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator- 

```cpp
Timespan operator-(TimeDiff microseconds) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator+= 

```cpp
Timespan & operator+=(TimeDiff microseconds)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### operator-= 

```cpp
Timespan & operator-=(TimeDiff microseconds)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `microseconds` | `TimeDiff` |  |

---

#### days 

```cpp
inline int days() const
```

Returns the number of days.

---

#### hours 

```cpp
inline int hours() const
```

Returns the number of hours (0 to 23).

---

#### totalHours 

```cpp
inline int totalHours() const
```

Returns the total number of hours.

---

#### minutes 

```cpp
inline int minutes() const
```

Returns the number of minutes (0 to 59).

---

#### totalMinutes 

```cpp
inline int totalMinutes() const
```

Returns the total number of minutes.

---

#### seconds 

```cpp
inline int seconds() const
```

Returns the number of seconds (0 to 59).

---

#### totalSeconds 

```cpp
inline int totalSeconds() const
```

Returns the total number of seconds.

---

#### milliseconds 

```cpp
inline int milliseconds() const
```

Returns the number of milliseconds (0 to 999).

---

#### totalMilliseconds 

```cpp
inline TimeDiff totalMilliseconds() const
```

Returns the total number of milliseconds.

---

#### microseconds 

```cpp
inline int microseconds() const
```

Returns the fractions of a millisecond in microseconds (0 to 999).

---

#### useconds 

```cpp
inline int useconds() const
```

Returns the fractions of a second in microseconds (0 to 999999).

---

#### totalMicroseconds 

```cpp
inline TimeDiff totalMicroseconds() const
```

Returns the total number of microseconds.

---

#### MILLISECONDS 

```cpp
const TimeDiff MILLISECONDS = 1000
```

The number of microseconds in a millisecond.

---

#### SECONDS 

```cpp
const TimeDiff SECONDS = 1000 * Timespan::MILLISECONDS
```

The number of microseconds in a second.

---

#### MINUTES 

```cpp
const TimeDiff MINUTES = 60 * Timespan::SECONDS
```

The number of microseconds in a minute.

---

#### HOURS 

```cpp
const TimeDiff HOURS = 60 * Timespan::MINUTES
```

The number of microseconds in a hour.

---

#### DAYS 

```cpp
const TimeDiff DAYS = 24 * Timespan::HOURS
```

The number of microseconds in a day.

---

#### _span 

```cpp
TimeDiff _span
```

## DateTime 

> **Defined in:** `datetime.h`

This class represents an instant in time, expressed in years, months, days, hours, minutes, seconds and milliseconds based on the Gregorian calendar. The class is mainly useful for conversions between UTC, Julian day and Gregorian calendar dates.

The date and time stored in a [DateTime](#classicy_1_1DateTime) is always in UTC (Coordinated Universal Time) and thus independent of the timezone in effect on the system.

Conversion calculations are based on algorithms collected and described by Peter Baum at [http://vsg.cape.com/~pbaum/date/date0.htm](http://vsg.cape.com/~pbaum/date/date0.htm)

Internally, this class stores a date/time in two forms (UTC and broken down) for performance reasons. Only use this class for conversions between date/time representations. Use the [Timestamp](#classicy_1_1Timestamp) class for everything else.

Notes:

* Zero is a valid year (in accordance with ISO 8601 and astronomical year numbering)

* Year zero (0) is a leap year

* Negative years (years preceding 1 BC) are not supported

For more information, please see:

* [http://en.wikipedia.org/wiki/Gregorian_Calendar](http://en.wikipedia.org/wiki/Gregorian_Calendar)

* [http://en.wikipedia.org/wiki/Julian_day](http://en.wikipedia.org/wiki/Julian_day)

* [http://en.wikipedia.org/wiki/UTC](http://en.wikipedia.org/wiki/UTC)

* [http://en.wikipedia.org/wiki/ISO_8601](http://en.wikipedia.org/wiki/ISO_8601)

### Members

| Name | Description |
|------|-------------|
| [`Months`](#group__base_1ga9e6c37f4daad98cbb93b264905eaf498) | Symbolic names for month numbers (1 to 12). |
| [`DaysOfWeek`](#group__base_1ga2113d4a106804ff86e3e9e4c144542e7) | Symbolic names for week day numbers (0 to 6). |
| [`DateTime`](#group__base_1gafc8db97ad1127e27ea8cc055d64bacdc) | Creates a [DateTime](#classicy_1_1DateTime) for the current date and time. |
| [`DateTime`](#group__base_1ga8757497c5f7df732cc6f84d2c31e997d) | Creates a [DateTime](#classicy_1_1DateTime) for the date and time given in a [Timestamp](#classicy_1_1Timestamp). |
| [`DateTime`](#group__base_1ga0723f5d0fb69a9f9f2231723b4712c2b) | Creates a [DateTime](#classicy_1_1DateTime) for the given Gregorian date and time. |
| [`DateTime`](#group__base_1gaa8d974aeec2eaea35d48ac2860c6cfa6) | Creates a [DateTime](#classicy_1_1DateTime) for the given Julian day. |
| [`DateTime`](#group__base_1gae8ca8fe73a37b81ea207f21115a3218a) | Creates a [DateTime](#classicy_1_1DateTime) from an UtcTimeVal and a TimeDiff. |
| [`DateTime`](#group__base_1gaa8c6129e0263af535cd47eace907e8da) | Copy constructor. Creates the [DateTime](#classicy_1_1DateTime) from another one. |
| [`~DateTime`](#group__base_1gaab8ba2987dd09faf606927a8c5c8d57b) | Destroys the [DateTime](#classicy_1_1DateTime). |
| [`operator=`](#group__base_1gaacf8c5cbf1e8b62928a397989844ca51) | Assigns another [DateTime](#classicy_1_1DateTime). |
| [`operator=`](#group__base_1gae5bfb492a3b5469b672ab3c8c60ca2b4) | Assigns a [Timestamp](#classicy_1_1Timestamp). |
| [`operator=`](#group__base_1ga40a6a5696a62415686d4ba0f8a36d52c) | Assigns a Julian day. |
| [`assign`](#group__base_1ga60d82a180ce48c06261f324e8aa32615) | Assigns a Gregorian date and time. |
| [`swap`](#group__base_1ga2d52b10410733fc42b1d222b78b546f3) | Swaps the [DateTime](#classicy_1_1DateTime) with another one. |
| [`year`](#group__base_1ga8ee24f2380b3a6f0728f1451cec5b846) | Returns the year. |
| [`month`](#group__base_1ga36843329b8b306cb82a733b46f4d49c7) | Returns the month (1 to 12). |
| [`week`](#group__base_1gaf5ad099452341c3b5682a8f7352f8422) | Returns the week number within the year. FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1). The returned week number will be from 0 to 53. Week number 1 is the week containing January 4. This is in accordance to ISO 8601. |
| [`day`](#group__base_1ga78029b135e06449cf535f2ae8c7ab497) | Returns the day witin the month (1 to 31). |
| [`dayOfWeek`](#group__base_1ga2ea7a5f2314c97c367867fc550d61a5d) | Returns the weekday (0 to 6, where 0 = Sunday, 1 = Monday, ..., 6 = Saturday). |
| [`dayOfYear`](#group__base_1gad9924403dc634f1e1ba699498d5c9c61) | Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc. |
| [`hour`](#group__base_1ga885ca6176737aff2dfa2443a10512c12) | Returns the hour (0 to 23). |
| [`hourAMPM`](#group__base_1gad73305d05775a09b0fbd4c8db3c96923) | Returns the hour (0 to 12). |
| [`isAM`](#group__base_1gab9661f01a53099540cb71b23c0c1871d) | Returns true if hour < 12;. |
| [`isPM`](#group__base_1ga12d69cfbf155b6f294785aa0b9c49fe9) | Returns true if hour >= 12. |
| [`minute`](#group__base_1gac124db7fe3f0fb9980087d09ed5abde6) | Returns the minute (0 to 59). |
| [`second`](#group__base_1gab5062725edc5fb0dea2d9ea62dbdaa85) | Returns the second (0 to 59). |
| [`millisecond`](#group__base_1gaa7bd963176ae54f61301fca30e02592a) | Returns the millisecond (0 to 999) |
| [`microsecond`](#group__base_1gad4dbddbc0255cc26c9fd9dafaed353b9) | Returns the microsecond (0 to 999) |
| [`julianDay`](#group__base_1ga48a04a535ae9290933994bfe0084b176) | Returns the julian day for the date and time. |
| [`timestamp`](#group__base_1ga591c0712a8e1250820532510a54a49b3) | Returns the date and time expressed as a [Timestamp](#classicy_1_1Timestamp). |
| [`utcTime`](#group__base_1gaa67be192392537cf1674d41f56704ab9) | Returns the date and time expressed in UTC-based time. UTC base time is midnight, October 15, 1582. Resolution is 100 nanoseconds. |
| [`operator==`](#group__base_1ga4262a88ba878383b2b4e0a1712b11649) |  |
| [`operator!=`](#group__base_1ga82a285c08ba0e9221127a4421211ba75) |  |
| [`operator<`](#group__base_1ga4968d25e134d2f9992866d797e27b7e3) |  |
| [`operator<=`](#group__base_1ga4bea2b52f94d03908e2b0e82c8f3bd29) |  |
| [`operator>`](#group__base_1ga35693ffc981a1608ed24246f1162ffa3) |  |
| [`operator>=`](#group__base_1ga2d2264fc51381c3f29b95e51433a96c1) |  |
| [`operator+`](#group__base_1gaed9c5e50f2543472847ca0675f6f87ca) |  |
| [`operator-`](#group__base_1ga46b10a0779e2e09a0be2630832412a47) |  |
| [`operator-`](#group__base_1gaae40c16a2b2f6f2c63e93efe904d38dd) |  |
| [`operator+=`](#group__base_1gaa56672df917ae0e0e5a02f73a3f1e94a) |  |
| [`operator-=`](#group__base_1gadb7d7dc23a9eaa6e054f2383a21bb388) |  |
| [`makeUTC`](#group__base_1gaef85751cde47a0e909aeddae1775b805) | Converts a local time into UTC, by applying the given time zone differential. |
| [`makeLocal`](#group__base_1ga82ce62343d007567544418aed404dcd9) | Converts a UTC time into a local time, by applying the given time zone differential. |
| [`isLeapYear`](#group__base_1gab7ceaf8d08c284eb209ad6e370e6961f) | Returns true if the given year is a leap year; false otherwise. |
| [`daysOfMonth`](#group__base_1ga544d4bee671c5c6680867452263fcc6c) | Returns the number of days in the given month and year. Month is from 1 to 12. |
| [`isValid`](#group__base_1ga463d516bd237f7c943e25c3f69d33e95) | Checks if the given date and time is valid (all arguments are within a proper range). |
| [`toJulianDay`](#group__base_1gae96017b4e44c759d2fd5dc8dd919641a) | Computes the Julian day for an UTC time. |
| [`toJulianDay`](#group__base_1ga36713db7db011080de64cdc28ec6fae7) | Computes the Julian day for a gregorian calendar date and time. See [http://vsg.cape.com/~pbaum/date/jdimp.htm](http://vsg.cape.com/~pbaum/date/jdimp.htm), section 2.3.1 for the algorithm. |
| [`toUtcTime`](#group__base_1ga39589bb35dd881e229d1e82474b59fec) | Computes the UTC time for a Julian day. |
| [`computeGregorian`](#group__base_1ga6f59bebcde64cc028cdbcde61b7eb619) | Computes the Gregorian date for the given Julian day. See [http://vsg.cape.com/~pbaum/date/injdimp.htm](http://vsg.cape.com/~pbaum/date/injdimp.htm), section 3.3.1 for the algorithm. |
| [`computeDaytime`](#group__base_1ga57cdba4dec7b53c448c56545d0c0a8fb) | Extracts the daytime (hours, minutes, seconds, etc.) from the stored utcTime. |
| [`checkLimit`](#group__base_1ga10a738833c77e5ad0d146bd61bca9337) | Utility functions used to correct the overflow in computeGregorian. |
| [`normalize`](#group__base_1ga4d52d557202e5c4bdce44f45db8035f4) |  |
| [`_utcTime`](#group__base_1ga14e6532e1a7944987d9adfb1c7852512) |  |
| [`_year`](#group__base_1ga7f8ffc73810bbfbcf2d53c019b6c44c8) |  |
| [`_month`](#group__base_1ga0a721136b7c47d19f61f6734a49a3800) |  |
| [`_day`](#group__base_1gacd8e4b76482ac75bdeda1592ac01f73a) |  |
| [`_hour`](#group__base_1ga91bcdb7cf4accb4367f2602465067b19) |  |
| [`_minute`](#group__base_1gaebc63e2d4d9be134dd59521eaf884b3a) |  |
| [`_second`](#group__base_1ga85225b668528291f7903fe8e2ade16ec) |  |
| [`_millisecond`](#group__base_1ga09a330dab1c5465b0ae79e26bd3561ab) |  |
| [`_microsecond`](#group__base_1gaf24f7557bd0c588a709ececbc818e3ce) |  |

---

#### Months 

```cpp
enum Months
```

Symbolic names for month numbers (1 to 12).

| Value | Description |
|-------|-------------|
| `JANUARY` |  |
| `FEBRUARY` |  |
| `MARCH` |  |
| `APRIL` |  |
| `MAY` |  |
| `JUNE` |  |
| `JULY` |  |
| `AUGUST` |  |
| `SEPTEMBER` |  |
| `OCTOBER` |  |
| `NOVEMBER` |  |
| `DECEMBER` |  |

---

#### DaysOfWeek 

```cpp
enum DaysOfWeek
```

Symbolic names for week day numbers (0 to 6).

| Value | Description |
|-------|-------------|
| `SUNDAY` |  |
| `MONDAY` |  |
| `TUESDAY` |  |
| `WEDNESDAY` |  |
| `THURSDAY` |  |
| `FRIDAY` |  |
| `SATURDAY` |  |

---

#### DateTime 

```cpp
DateTime()
```

Creates a [DateTime](#classicy_1_1DateTime) for the current date and time.

---

#### DateTime 

```cpp
DateTime(const Timestamp & timestamp)
```

Creates a [DateTime](#classicy_1_1DateTime) for the date and time given in a [Timestamp](#classicy_1_1Timestamp).

| Parameter | Type | Description |
|-----------|------|-------------|
| `timestamp` | `const Timestamp &` |  |

---

#### DateTime 

```cpp
DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
```

Creates a [DateTime](#classicy_1_1DateTime) for the given Gregorian date and time.

* year is from 0 to 9999.

* month is from 1 to 12.

* day is from 1 to 31.

* hour is from 0 to 23.

* minute is from 0 to 59.

* second is from 0 to 59.

* millisecond is from 0 to 999.

* microsecond is from 0 to 999.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microsecond` | `int` |  |

---

#### DateTime 

```cpp
DateTime(double julianDay)
```

Creates a [DateTime](#classicy_1_1DateTime) for the given Julian day.

| Parameter | Type | Description |
|-----------|------|-------------|
| `julianDay` | `double` |  |

---

#### DateTime 

```cpp
DateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff)
```

Creates a [DateTime](#classicy_1_1DateTime) from an UtcTimeVal and a TimeDiff.

Mainly used internally by [DateTime](#classicy_1_1DateTime) and friends.

| Parameter | Type | Description |
|-----------|------|-------------|
| `utcTime` | `Timestamp::UtcTimeVal` |  |
| `diff` | `Timestamp::TimeDiff` |  |

---

#### DateTime 

```cpp
DateTime(const DateTime & dateTime)
```

Copy constructor. Creates the [DateTime](#classicy_1_1DateTime) from another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### ~DateTime 

```cpp
~DateTime()
```

Destroys the [DateTime](#classicy_1_1DateTime).

---

#### operator= 

```cpp
DateTime & operator=(const DateTime & dateTime)
```

Assigns another [DateTime](#classicy_1_1DateTime).

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator= 

```cpp
DateTime & operator=(const Timestamp & timestamp)
```

Assigns a [Timestamp](#classicy_1_1Timestamp).

| Parameter | Type | Description |
|-----------|------|-------------|
| `timestamp` | `const Timestamp &` |  |

---

#### operator= 

```cpp
DateTime & operator=(double julianDay)
```

Assigns a Julian day.

| Parameter | Type | Description |
|-----------|------|-------------|
| `julianDay` | `double` |  |

---

#### assign 

```cpp
DateTime & assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microseconds)
```

Assigns a Gregorian date and time.

* year is from 0 to 9999.

* month is from 1 to 12.

* day is from 1 to 31.

* hour is from 0 to 23.

* minute is from 0 to 59.

* second is from 0 to 59.

* millisecond is from 0 to 999.

* microsecond is from 0 to 999.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microseconds` | `int` |  |

---

#### swap 

```cpp
void swap(DateTime & dateTime)
```

Swaps the [DateTime](#classicy_1_1DateTime) with another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `DateTime &` |  |

---

#### year 

```cpp
inline int year() const
```

Returns the year.

---

#### month 

```cpp
inline int month() const
```

Returns the month (1 to 12).

---

#### week 

```cpp
int week(int firstDayOfWeek) const
```

Returns the week number within the year. FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1). The returned week number will be from 0 to 53. Week number 1 is the week containing January 4. This is in accordance to ISO 8601.

The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started on a Saturday, week 1 will be the week starting on Monday, January 3. January 1 and 2 will fall within week 0 (or the last week of the previous year).

For 2007, which starts on a Monday, week 1 will be the week startung on Monday, January 1. There will be no week 0 in 2007.

| Parameter | Type | Description |
|-----------|------|-------------|
| `firstDayOfWeek` | `int` |  |

---

#### day 

```cpp
inline int day() const
```

Returns the day witin the month (1 to 31).

---

#### dayOfWeek 

```cpp
int dayOfWeek() const
```

Returns the weekday (0 to 6, where 0 = Sunday, 1 = Monday, ..., 6 = Saturday).

---

#### dayOfYear 

```cpp
int dayOfYear() const
```

Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc.

---

#### hour 

```cpp
inline int hour() const
```

Returns the hour (0 to 23).

---

#### hourAMPM 

```cpp
inline int hourAMPM() const
```

Returns the hour (0 to 12).

---

#### isAM 

```cpp
inline bool isAM() const
```

Returns true if hour < 12;.

---

#### isPM 

```cpp
inline bool isPM() const
```

Returns true if hour >= 12.

---

#### minute 

```cpp
inline int minute() const
```

Returns the minute (0 to 59).

---

#### second 

```cpp
inline int second() const
```

Returns the second (0 to 59).

---

#### millisecond 

```cpp
inline int millisecond() const
```

Returns the millisecond (0 to 999)

---

#### microsecond 

```cpp
inline int microsecond() const
```

Returns the microsecond (0 to 999)

---

#### julianDay 

```cpp
double julianDay() const
```

Returns the julian day for the date and time.

---

#### timestamp 

```cpp
inline Timestamp timestamp() const
```

Returns the date and time expressed as a [Timestamp](#classicy_1_1Timestamp).

---

#### utcTime 

```cpp
inline Timestamp::UtcTimeVal utcTime() const
```

Returns the date and time expressed in UTC-based time. UTC base time is midnight, October 15, 1582. Resolution is 100 nanoseconds.

---

#### operator== 

```cpp
inline bool operator==(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator!= 

```cpp
inline bool operator!=(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator< 

```cpp
inline bool operator<(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator<= 

```cpp
inline bool operator<=(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator> 

```cpp
inline bool operator>(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator>= 

```cpp
inline bool operator>=(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator+ 

```cpp
DateTime operator+(const Timespan & span) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### operator- 

```cpp
DateTime operator-(const Timespan & span) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### operator- 

```cpp
Timespan operator-(const DateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### operator+= 

```cpp
DateTime & operator+=(const Timespan & span)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### operator-= 

```cpp
DateTime & operator-=(const Timespan & span)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### makeUTC 

```cpp
void makeUTC(int tzd)
```

Converts a local time into UTC, by applying the given time zone differential.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |

---

#### makeLocal 

```cpp
void makeLocal(int tzd)
```

Converts a UTC time into a local time, by applying the given time zone differential.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |

---

#### isLeapYear 

```cpp
static inline bool isLeapYear(int year)
```

Returns true if the given year is a leap year; false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |

---

#### daysOfMonth 

```cpp
static int daysOfMonth(int year, int month)
```

Returns the number of days in the given month and year. Month is from 1 to 12.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |

---

#### isValid 

```cpp
static bool isValid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
```

Checks if the given date and time is valid (all arguments are within a proper range).

Returns true if all arguments are valid, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microsecond` | `int` |  |

---

#### toJulianDay 

```cpp
static inline double toJulianDay(Timestamp::UtcTimeVal utcTime)
```

Computes the Julian day for an UTC time.

| Parameter | Type | Description |
|-----------|------|-------------|
| `utcTime` | `Timestamp::UtcTimeVal` |  |

---

#### toJulianDay 

```cpp
static double toJulianDay(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
```

Computes the Julian day for a gregorian calendar date and time. See [http://vsg.cape.com/~pbaum/date/jdimp.htm](http://vsg.cape.com/~pbaum/date/jdimp.htm), section 2.3.1 for the algorithm.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microsecond` | `int` |  |

---

#### toUtcTime 

```cpp
static inline Timestamp::UtcTimeVal toUtcTime(double julianDay)
```

Computes the UTC time for a Julian day.

| Parameter | Type | Description |
|-----------|------|-------------|
| `julianDay` | `double` |  |

---

#### computeGregorian 

```cpp
void computeGregorian(double julianDay)
```

Computes the Gregorian date for the given Julian day. See [http://vsg.cape.com/~pbaum/date/injdimp.htm](http://vsg.cape.com/~pbaum/date/injdimp.htm), section 3.3.1 for the algorithm.

| Parameter | Type | Description |
|-----------|------|-------------|
| `julianDay` | `double` |  |

---

#### computeDaytime 

```cpp
void computeDaytime()
```

Extracts the daytime (hours, minutes, seconds, etc.) from the stored utcTime.

---

#### checkLimit 

```cpp
void checkLimit(short & lower, short & higher, short limit)
```

Utility functions used to correct the overflow in computeGregorian.

| Parameter | Type | Description |
|-----------|------|-------------|
| `lower` | `short &` |  |
| `higher` | `short &` |  |
| `limit` | `short` |  |

---

#### normalize 

```cpp
void normalize()
```

---

#### _utcTime 

```cpp
Timestamp::UtcTimeVal _utcTime
```

---

#### _year 

```cpp
short _year
```

---

#### _month 

```cpp
short _month
```

---

#### _day 

```cpp
short _day
```

---

#### _hour 

```cpp
short _hour
```

---

#### _minute 

```cpp
short _minute
```

---

#### _second 

```cpp
short _second
```

---

#### _millisecond 

```cpp
short _millisecond
```

---

#### _microsecond 

```cpp
short _microsecond
```

## Timezone 

> **Defined in:** `datetime.h`

This class provides information about the current timezone.

### Members

| Name | Description |
|------|-------------|
| [`utcOffset`](#group__base_1gad3222adfbb30111556179a35f05aab7f) | Returns the offset of local time to UTC, in seconds. local time = UTC + [utcOffset()](#group__base_1gad3222adfbb30111556179a35f05aab7f) + [dst()](#group__base_1ga06df217abf17778cace33570327b859b). |
| [`dst`](#group__base_1ga06df217abf17778cace33570327b859b) | Returns the daylight saving time offset in seconds if daylight saving time is in use. local time = UTC + [utcOffset()](#group__base_1gad3222adfbb30111556179a35f05aab7f) + [dst()](#group__base_1ga06df217abf17778cace33570327b859b). |
| [`isDst`](#group__base_1ga71aaa73c0b0a54c8f42d95efeed7e9b4) | Returns true if daylight saving time is in effect for the given time. Depending on the operating system platform this might only work reliably for certain date ranges, as the C library's localtime() function is used. |
| [`tzd`](#group__base_1gac940583482d1cd15b3abb37acbf224d4) | Returns the time zone differential for the current timezone. The timezone differential is computed as [utcOffset()](#group__base_1gad3222adfbb30111556179a35f05aab7f) + [dst()](#group__base_1ga06df217abf17778cace33570327b859b) /// and is expressed in seconds. |
| [`name`](#group__base_1ga56a345cbd893fd081c9d0e838e551a12) | Returns the timezone name currently in effect. |
| [`standardName`](#group__base_1ga810a32bd323905cc8a42cc4fb39e1e80) | Returns the timezone name if not daylight saving time is in effect. |
| [`dstName`](#group__base_1ga09d7845c9aae79917a9d510288590d2d) | Returns the timezone name if daylight saving time is in effect. |

---

#### utcOffset 

```cpp
static int utcOffset()
```

Returns the offset of local time to UTC, in seconds. local time = UTC + [utcOffset()](#group__base_1gad3222adfbb30111556179a35f05aab7f) + [dst()](#group__base_1ga06df217abf17778cace33570327b859b).

---

#### dst 

```cpp
static int dst()
```

Returns the daylight saving time offset in seconds if daylight saving time is in use. local time = UTC + [utcOffset()](#group__base_1gad3222adfbb30111556179a35f05aab7f) + [dst()](#group__base_1ga06df217abf17778cace33570327b859b).

---

#### isDst 

```cpp
static bool isDst(const Timestamp & timestamp)
```

Returns true if daylight saving time is in effect for the given time. Depending on the operating system platform this might only work reliably for certain date ranges, as the C library's localtime() function is used.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timestamp` | `const Timestamp &` |  |

---

#### tzd 

```cpp
static int tzd()
```

Returns the time zone differential for the current timezone. The timezone differential is computed as [utcOffset()](#group__base_1gad3222adfbb30111556179a35f05aab7f) + [dst()](#group__base_1ga06df217abf17778cace33570327b859b) /// and is expressed in seconds.

---

#### name 

```cpp
static std::string name()
```

Returns the timezone name currently in effect.

---

#### standardName 

```cpp
static std::string standardName()
```

Returns the timezone name if not daylight saving time is in effect.

---

#### dstName 

```cpp
static std::string dstName()
```

Returns the timezone name if daylight saving time is in effect.

## LocalDateTime 

> **Defined in:** `datetime.h`

This class represents an instant in local time (as opposed to UTC), expressed in years, months, days, hours, minutes, seconds and milliseconds based on the Gregorian calendar.

In addition to the date and time, the class also maintains a time zone differential, which denotes the difference in seconds from UTC to local time, i.e. UTC = local time - time zone differential.

Although [LocalDateTime](#classicy_1_1LocalDateTime) supports relational and arithmetic operators, all date/time comparisons and date/time arithmetics should be done in UTC, using the [DateTime](#classicy_1_1DateTime) or [Timestamp](#classicy_1_1Timestamp) class for better performance. The relational operators normalize the dates/times involved to UTC before carrying out the comparison.

The time zone differential is based on the input date and time and current time zone. A number of constructors accept an explicit time zone differential parameter. These should not be used since daylight savings time processing is impossible since the time zone is unknown. Each of the constructors accepting a tzd parameter have been marked as deprecated and may be removed in a future revision.

### Members

| Name | Description |
|------|-------------|
| [`LocalDateTime`](#group__base_1gaf8f507cd5b10d8089d263b12e34e14fc) | Creates a [LocalDateTime](#classicy_1_1LocalDateTime) with the current date/time for the current time zone. |
| [`LocalDateTime`](#group__base_1gaa6b69743391798f50ba00facb3b344b8) | Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Gregorian local date and time. |
| [`LocalDateTime`](#group__base_1gac25cdf7cf37094a68feccb09da251a63) | @ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Gregorian date and time in the time zone denoted by the time zone differential in tzd. |
| [`LocalDateTime`](#group__base_1ga8541afcbe851e21828977693c1130b5b) | Creates a [LocalDateTime](#classicy_1_1LocalDateTime) from the UTC time given in dateTime, using the time zone differential of the current time zone. |
| [`LocalDateTime`](#group__base_1gab8d6a8ba8d4ce3c43821dc4d8561aa0d) | @ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) from the UTC time given in dateTime, using the given time zone differential. Adjusts dateTime for the given time zone differential. |
| [`LocalDateTime`](#group__base_1ga8aaa4e3e37cbac08e70d41e9eff399d0) | @ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) from the UTC time given in dateTime, using the given time zone differential. If adjust is true, adjusts dateTime for the given time zone differential. |
| [`LocalDateTime`](#group__base_1gabe29c1922e6895d2d3b7aa1a01c5a40a) | Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Julian day in the local time zone. |
| [`LocalDateTime`](#group__base_1ga20abd6e936deee4ce9d14af620c008d7) | @ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Julian day in the time zone denoted by the time zone differential in tzd. |
| [`LocalDateTime`](#group__base_1gae63da0ed0e40399b8986a776a3043053) | Copy constructor. Creates the [LocalDateTime](#classicy_1_1LocalDateTime) from another one. |
| [`~LocalDateTime`](#group__base_1ga173cd6cde153c287ed6c83691329be4e) | Destroys the [LocalDateTime](#classicy_1_1LocalDateTime). |
| [`operator=`](#group__base_1ga6e18acbc0937ed82dc958f9372504e33) | Assigns another [LocalDateTime](#classicy_1_1LocalDateTime). |
| [`operator=`](#group__base_1gafa3356c42a95457524c27c2be9a717fd) | Assigns a timestamp. |
| [`operator=`](#group__base_1ga344e356fccab4631853d9f5cd37de8c6) | Assigns a Julian day in the local time zone. |
| [`assign`](#group__base_1gaac91b6bebab01e9b755e5b72e9137107) | Assigns a Gregorian local date and time. |
| [`assign`](#group__base_1ga5a05f078a0a5b372ad43aad8f8505a77) | @ deprecated Assigns a Gregorian local date and time in the time zone denoted by the time zone differential in tzd. |
| [`assign`](#group__base_1ga1ad03bada11f55c902dbec05d2fc20ba) | @ deprecated Assigns a Julian day in the time zone denoted by the time zone differential in tzd. |
| [`swap`](#group__base_1ga6bc1c879e4d7b3b04a976c1161ecd5fc) | Swaps the [LocalDateTime](#classicy_1_1LocalDateTime) with another one. |
| [`year`](#group__base_1ga8b67a920a92e22ea2b6d4e8a306566c9) | Returns the year. |
| [`month`](#group__base_1ga8bb9094aac43c17269669b6422448932) | Returns the month (1 to 12). |
| [`week`](#group__base_1gaad9976490f29168f69b842eda06653cc) | Returns the week number within the year. FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1). The returned week number will be from 0 to 53. Week number 1 is the week containing January 4. This is in accordance to ISO 8601. |
| [`day`](#group__base_1gadced33febfae85988662db9b42e56b02) | Returns the day witin the month (1 to 31). |
| [`dayOfWeek`](#group__base_1gaf0b1cfda4f34b38a3f030a43cb935499) | Returns the weekday (0 to 6, where 0 = Sunday, 1 = Monday, ..., 6 = Saturday). |
| [`dayOfYear`](#group__base_1ga4591a15d833c10a3a6ba391721bba9b6) | Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc. |
| [`hour`](#group__base_1ga03a3ac243fec29a048966cb9d3f2c84e) | Returns the hour (0 to 23). |
| [`hourAMPM`](#group__base_1ga715ce88ba0720784ffb6d029be23929f) | Returns the hour (0 to 12). |
| [`isAM`](#group__base_1ga3dcd5082e83dfdf1fb448ca809abe23a) | Returns true if hour < 12;. |
| [`isPM`](#group__base_1gae32597d36b509e031d2c85e0f585cf33) | Returns true if hour >= 12. |
| [`minute`](#group__base_1ga1dbfbad9d0753b0ff904994b486a34b3) | Returns the minute (0 to 59). |
| [`second`](#group__base_1ga5767718a9fb2343d45d3b3c199a2a19e) | Returns the second (0 to 59). |
| [`millisecond`](#group__base_1gaad844e44442901f267a5de9c79992374) | Returns the millisecond (0 to 999) |
| [`microsecond`](#group__base_1ga23ed596d25ee1cd738810c7053e41ce9) | Returns the microsecond (0 to 999) |
| [`julianDay`](#group__base_1gabe283094aa6c16646c54b712f299c040) | Returns the julian day for the date. |
| [`tzd`](#group__base_1gaf4c12ad2150d60db15fae7b272eab06d) | Returns the time zone differential. |
| [`utc`](#group__base_1ga416f298027e9fbb02fbbbb59280bce06) | Returns the UTC equivalent for the local date and time. |
| [`timestamp`](#group__base_1gab031a9c8c582592f94857d03e9732c08) | Returns the date and time expressed as a [Timestamp](#classicy_1_1Timestamp). |
| [`utcTime`](#group__base_1ga59f8e28c14dce3c650e5127f5aa9b304) | Returns the UTC equivalent for the local date and time. |
| [`operator==`](#group__base_1ga33d47d51a7701e0b275500adf395df1a) |  |
| [`operator!=`](#group__base_1gad10f335463ae82abbc089fc6194e90f7) |  |
| [`operator<`](#group__base_1gaf569d4c7514648e90ebd3837ba4dde95) |  |
| [`operator<=`](#group__base_1gad2528f970817d96c39323bad3dcdb259) |  |
| [`operator>`](#group__base_1gae0861580c425aee3e4bba0c269e32d17) |  |
| [`operator>=`](#group__base_1gac7cd53d72b4af340dae23afb3d727925) |  |
| [`operator+`](#group__base_1gacd5f92bb4a0cd7ded64e7fb3ffa6396e) |  |
| [`operator-`](#group__base_1gad7a1bbae9e178124fa09a1e2a5bc0571) |  |
| [`operator-`](#group__base_1ga216a8affb2ff0334d51f57ee0ca81dd7) |  |
| [`operator+=`](#group__base_1ga8a0e1e554f15d276630225f97fd59961) |  |
| [`operator-=`](#group__base_1ga11a3cfaef5f0d1ffb66f802646eb29a6) |  |
| [`LocalDateTime`](#group__base_1ga1ab5b8d9c17a7ca56e8949c890d45004) |  |
| [`determineTzd`](#group__base_1ga290ea5b1aee903968e51eed129b287f7) | Recalculate the tzd based on the _dateTime member based on the current timezone using the Standard C runtime functions. If adjust is true, then [adjustForTzd()](#group__base_1gad0d104ecf1aa081650b681f2afe5718d) is called after the differential is calculated. |
| [`adjustForTzd`](#group__base_1gad0d104ecf1aa081650b681f2afe5718d) | Adjust the _dateTime member based on the _tzd member. |
| [`dstOffset`](#group__base_1ga764a663d49040c03389b398732cec64b) | Determine the DST offset for the current date/time. |
| [`_dateTime`](#group__base_1ga8925e6e312f69cf44a74d1984214f72c) |  |
| [`_tzd`](#group__base_1ga3a1982a7b47ea6217612c1ebe42bc4da) |  |

---

#### LocalDateTime 

```cpp
LocalDateTime()
```

Creates a [LocalDateTime](#classicy_1_1LocalDateTime) with the current date/time for the current time zone.

---

#### LocalDateTime 

```cpp
LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
```

Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Gregorian local date and time.

* year is from 0 to 9999.

* month is from 1 to 12.

* day is from 1 to 31.

* hour is from 0 to 23.

* minute is from 0 to 59.

* second is from 0 to 59.

* millisecond is from 0 to 999.

* microsecond is from 0 to 999.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microsecond` | `int` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(int tzd, int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
```

@ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Gregorian date and time in the time zone denoted by the time zone differential in tzd.

* tzd is in seconds.

* year is from 0 to 9999.

* month is from 1 to 12.

* day is from 1 to 31.

* hour is from 0 to 23.

* minute is from 0 to 59.

* second is from 0 to 59.

* millisecond is from 0 to 999.

* microsecond is from 0 to 999.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microsecond` | `int` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(const DateTime & dateTime)
```

Creates a [LocalDateTime](#classicy_1_1LocalDateTime) from the UTC time given in dateTime, using the time zone differential of the current time zone.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(int tzd, const DateTime & dateTime)
```

@ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) from the UTC time given in dateTime, using the given time zone differential. Adjusts dateTime for the given time zone differential.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |
| `dateTime` | `const DateTime &` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(int tzd, const DateTime & dateTime, bool adjust)
```

@ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) from the UTC time given in dateTime, using the given time zone differential. If adjust is true, adjusts dateTime for the given time zone differential.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |
| `dateTime` | `const DateTime &` |  |
| `adjust` | `bool` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(double julianDay)
```

Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Julian day in the local time zone.

| Parameter | Type | Description |
|-----------|------|-------------|
| `julianDay` | `double` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(int tzd, double julianDay)
```

@ deprecated Creates a [LocalDateTime](#classicy_1_1LocalDateTime) for the given Julian day in the time zone denoted by the time zone differential in tzd.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |
| `julianDay` | `double` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(const LocalDateTime & dateTime)
```

Copy constructor. Creates the [LocalDateTime](#classicy_1_1LocalDateTime) from another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### ~LocalDateTime 

```cpp
~LocalDateTime()
```

Destroys the [LocalDateTime](#classicy_1_1LocalDateTime).

---

#### operator= 

```cpp
LocalDateTime & operator=(const LocalDateTime & dateTime)
```

Assigns another [LocalDateTime](#classicy_1_1LocalDateTime).

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator= 

```cpp
LocalDateTime & operator=(const Timestamp & timestamp)
```

Assigns a timestamp.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timestamp` | `const Timestamp &` |  |

---

#### operator= 

```cpp
LocalDateTime & operator=(double julianDay)
```

Assigns a Julian day in the local time zone.

| Parameter | Type | Description |
|-----------|------|-------------|
| `julianDay` | `double` |  |

---

#### assign 

```cpp
LocalDateTime & assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microseconds)
```

Assigns a Gregorian local date and time.

* year is from 0 to 9999.

* month is from 1 to 12.

* day is from 1 to 31.

* hour is from 0 to 23.

* minute is from 0 to 59.

* second is from 0 to 59.

* millisecond is from 0 to 999.

* microsecond is from 0 to 999.

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microseconds` | `int` |  |

---

#### assign 

```cpp
LocalDateTime & assign(int tzd, int year, int month, int day, int hour, int minute, int second, int millisecond, int microseconds)
```

@ deprecated Assigns a Gregorian local date and time in the time zone denoted by the time zone differential in tzd.

* tzd is in seconds.

* year is from 0 to 9999.

* month is from 1 to 12.

* day is from 1 to 31.

* hour is from 0 to 23.

* minute is from 0 to 59.

* second is from 0 to 59.

* millisecond is from 0 to 999.

* microsecond is from 0 to 999.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |
| `year` | `int` |  |
| `month` | `int` |  |
| `day` | `int` |  |
| `hour` | `int` |  |
| `minute` | `int` |  |
| `second` | `int` |  |
| `millisecond` | `int` |  |
| `microseconds` | `int` |  |

---

#### assign 

```cpp
LocalDateTime & assign(int tzd, double julianDay)
```

@ deprecated Assigns a Julian day in the time zone denoted by the time zone differential in tzd.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tzd` | `int` |  |
| `julianDay` | `double` |  |

---

#### swap 

```cpp
void swap(LocalDateTime & dateTime)
```

Swaps the [LocalDateTime](#classicy_1_1LocalDateTime) with another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `LocalDateTime &` |  |

---

#### year 

```cpp
inline int year() const
```

Returns the year.

---

#### month 

```cpp
inline int month() const
```

Returns the month (1 to 12).

---

#### week 

```cpp
inline int week(int firstDayOfWeek) const
```

Returns the week number within the year. FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1). The returned week number will be from 0 to 53. Week number 1 is the week containing January 4. This is in accordance to ISO 8601.

The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started on a Saturday, week 1 will be the week starting on Monday, January 3. January 1 and 2 will fall within week 0 (or the last week of the previous year).

For 2007, which starts on a Monday, week 1 will be the week startung on Monday, January 1. There will be no week 0 in 2007.

| Parameter | Type | Description |
|-----------|------|-------------|
| `firstDayOfWeek` | `int` |  |

---

#### day 

```cpp
inline int day() const
```

Returns the day witin the month (1 to 31).

---

#### dayOfWeek 

```cpp
inline int dayOfWeek() const
```

Returns the weekday (0 to 6, where 0 = Sunday, 1 = Monday, ..., 6 = Saturday).

---

#### dayOfYear 

```cpp
inline int dayOfYear() const
```

Returns the number of the day in the year. January 1 is 1, February 1 is 32, etc.

---

#### hour 

```cpp
inline int hour() const
```

Returns the hour (0 to 23).

---

#### hourAMPM 

```cpp
inline int hourAMPM() const
```

Returns the hour (0 to 12).

---

#### isAM 

```cpp
inline bool isAM() const
```

Returns true if hour < 12;.

---

#### isPM 

```cpp
inline bool isPM() const
```

Returns true if hour >= 12.

---

#### minute 

```cpp
inline int minute() const
```

Returns the minute (0 to 59).

---

#### second 

```cpp
inline int second() const
```

Returns the second (0 to 59).

---

#### millisecond 

```cpp
inline int millisecond() const
```

Returns the millisecond (0 to 999)

---

#### microsecond 

```cpp
inline int microsecond() const
```

Returns the microsecond (0 to 999)

---

#### julianDay 

```cpp
inline double julianDay() const
```

Returns the julian day for the date.

---

#### tzd 

```cpp
inline int tzd() const
```

Returns the time zone differential.

---

#### utc 

```cpp
DateTime utc() const
```

Returns the UTC equivalent for the local date and time.

---

#### timestamp 

```cpp
inline Timestamp timestamp() const
```

Returns the date and time expressed as a [Timestamp](#classicy_1_1Timestamp).

---

#### utcTime 

```cpp
inline Timestamp::UtcTimeVal utcTime() const
```

Returns the UTC equivalent for the local date and time.

---

#### operator== 

```cpp
bool operator==(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator!= 

```cpp
bool operator!=(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator< 

```cpp
bool operator<(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator<= 

```cpp
bool operator<=(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator> 

```cpp
bool operator>(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator>= 

```cpp
bool operator>=(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator+ 

```cpp
LocalDateTime operator+(const Timespan & span) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### operator- 

```cpp
LocalDateTime operator-(const Timespan & span) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### operator- 

```cpp
Timespan operator-(const LocalDateTime & dateTime) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |

---

#### operator+= 

```cpp
LocalDateTime & operator+=(const Timespan & span)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### operator-= 

```cpp
LocalDateTime & operator-=(const Timespan & span)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `span` | `const Timespan &` |  |

---

#### LocalDateTime 

```cpp
LocalDateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff, int tzd)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `utcTime` | `Timestamp::UtcTimeVal` |  |
| `diff` | `Timestamp::TimeDiff` |  |
| `tzd` | `int` |  |

---

#### determineTzd 

```cpp
void determineTzd(bool adjust)
```

Recalculate the tzd based on the _dateTime member based on the current timezone using the Standard C runtime functions. If adjust is true, then [adjustForTzd()](#group__base_1gad0d104ecf1aa081650b681f2afe5718d) is called after the differential is calculated.

| Parameter | Type | Description |
|-----------|------|-------------|
| `adjust` | `bool` |  |

---

#### adjustForTzd 

```cpp
inline void adjustForTzd()
```

Adjust the _dateTime member based on the _tzd member.

---

#### dstOffset 

```cpp
std::time_t dstOffset(int & dstOffset) const
```

Determine the DST offset for the current date/time.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dstOffset` | `int &` |  |

---

#### _dateTime 

```cpp
DateTime _dateTime
```

---

#### _tzd 

```cpp
int _tzd
```

## DateTimeFormat 

> **Defined in:** `datetime.h`

Definition of date/time formats and various constants used by [DateTimeFormatter](#classicy_1_1DateTimeFormatter) and [DateTimeParser](#classicy_1_1DateTimeParser).

### Members

| Name | Description |
|------|-------------|
| [`ISO8601_FORMAT`](#group__base_1ga109ea0bf73c13b30bf07e646dde1a202) | predefined date formats The date/time format defined in the ISO 8601 standard. |
| [`ISO8601_FRAC_FORMAT`](#group__base_1gafc5610125c46f81c8b37a33a8275e930) | The date/time format defined in the ISO 8601 standard, with fractional seconds. |
| [`RFC822_FORMAT`](#group__base_1ga3a5787e700549b3a93a70dcaa02056ae) | The date/time format defined in RFC 822 (obsoleted by RFC 1123). |
| [`RFC1123_FORMAT`](#group__base_1ga68d1b1aa6350fc40f88249069113fd4b) | The date/time format defined in RFC 1123 (obsoletes RFC 822). |
| [`HTTP_FORMAT`](#group__base_1ga359715c473f222e8f0dab4a2ebc7d863) | The date/time format defined in the HTTP specification (RFC 2616), which is basically a variant of RFC 1036 with a zero-padded day field. |
| [`RFC850_FORMAT`](#group__base_1ga8fa14f9a2948c74f48b694783b866f7c) | The date/time format defined in RFC 850 (obsoleted by RFC 1036). |
| [`RFC1036_FORMAT`](#group__base_1gae7c168c83eccc9fa22db1160794815d8) | The date/time format defined in RFC 1036 (obsoletes RFC 850). |
| [`ASCTIME_FORMAT`](#group__base_1ga8812f0ba04cfeb783b657aa24235243f) | The date/time format produced by the ANSI C asctime() function. |
| [`SORTABLE_FORMAT`](#group__base_1ga58bb4a2c89da2c6ea330b5db58ef81e0) | A simple, sortable date/time format. |
| [`WEEKDAY_NAMES`](#group__base_1gaa7b8ce2185d3976007545a5bc01c3806) | names used by formatter and parser English names of week days (Sunday, Monday, Tuesday, ...). |
| [`MONTH_NAMES`](#group__base_1ga5ab8b86c3b61513e25ad206a6d3e61f9) | English names of months (January, February, ...). |

---

#### ISO8601_FORMAT 

```cpp
const std::string ISO8601_FORMAT
```

predefined date formats The date/time format defined in the ISO 8601 standard.

Examples: 2005-01-01T12:00:00+01:00 2005-01-01T11:00:00Z

---

#### ISO8601_FRAC_FORMAT 

```cpp
const std::string ISO8601_FRAC_FORMAT
```

The date/time format defined in the ISO 8601 standard, with fractional seconds.

Examples: 2005-01-01T12:00:00.000000+01:00 2005-01-01T11:00:00.000000Z

---

#### RFC822_FORMAT 

```cpp
const std::string RFC822_FORMAT
```

The date/time format defined in RFC 822 (obsoleted by RFC 1123).

Examples: Sat, 1 Jan 05 12:00:00 +0100 Sat, 1 Jan 05 11:00:00 GMT

---

#### RFC1123_FORMAT 

```cpp
const std::string RFC1123_FORMAT
```

The date/time format defined in RFC 1123 (obsoletes RFC 822).

Examples: Sat, 1 Jan 2005 12:00:00 +0100 Sat, 1 Jan 2005 11:00:00 GMT

---

#### HTTP_FORMAT 

```cpp
const std::string HTTP_FORMAT
```

The date/time format defined in the HTTP specification (RFC 2616), which is basically a variant of RFC 1036 with a zero-padded day field.

Examples: Sat, 01 Jan 2005 12:00:00 +0100 Sat, 01 Jan 2005 11:00:00 GMT

---

#### RFC850_FORMAT 

```cpp
const std::string RFC850_FORMAT
```

The date/time format defined in RFC 850 (obsoleted by RFC 1036).

Examples: Saturday, 1-Jan-05 12:00:00 +0100 Saturday, 1-Jan-05 11:00:00 GMT

---

#### RFC1036_FORMAT 

```cpp
const std::string RFC1036_FORMAT
```

The date/time format defined in RFC 1036 (obsoletes RFC 850).

Examples: Saturday, 1 Jan 05 12:00:00 +0100 Saturday, 1 Jan 05 11:00:00 GMT

---

#### ASCTIME_FORMAT 

```cpp
const std::string ASCTIME_FORMAT
```

The date/time format produced by the ANSI C asctime() function.

Example: Sat Jan 1 12:00:00 2005

---

#### SORTABLE_FORMAT 

```cpp
const std::string SORTABLE_FORMAT
```

A simple, sortable date/time format.

Example: 2005-01-01 12:00:00

---

#### WEEKDAY_NAMES 

```cpp
const std::string WEEKDAY_NAMES = {
    "Sunday",   "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"}
```

names used by formatter and parser English names of week days (Sunday, Monday, Tuesday, ...).

---

#### MONTH_NAMES 

```cpp
const std::string MONTH_NAMES = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"}
```

English names of months (January, February, ...).

## DateTimeFormatter 

> **Defined in:** `datetime.h`

This class converts dates and times into strings, supporting a variety of standard and custom formats.

There are two kind of static member functions:

* format* functions return a std::string containin the formatted value.

* append* functions append the formatted value to an existing string.

### Members

| Name | Description |
|------|-------------|
| [``](#group__base_1ga762f4ef4f609c78cca375e743c5b47fc) |  |
| [`format`](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) | Formats the given timestamp according to the given format. The format string is used as a template to format the date and is copied character by character except for the following special characters, which are replaced by the corresponding value. |
| [`format`](#group__base_1ga3b9d02f1929b6213d8e05653dd790338) | Formats the given date and time according to the given format. See [format(const Timestamp&, const std::string&, int)](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for more information. |
| [`format`](#group__base_1gaaf0ee3821ccd348d4a3300e5084f35db) | Formats the given local date and time according to the given format. See [format(const Timestamp&, const std::string&, int)](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for more information. |
| [`format`](#group__base_1ga14b073e3e8e082e5d85bd2e55a47481c) | Formats the given timespan according to the given format. The format string is used as a template to format the date and is copied character by character except for the following special characters, which are replaced by the corresponding value. |
| [`append`](#group__base_1ga6cdc1d2ab5c778931ba07201f233ca12) | Formats the given timestamp according to the given format and appends it to str. |
| [`append`](#group__base_1gafa1da4e0959d36bed21fcb1e98206bd3) | Formats the given date and time according to the given format and appends it to str. |
| [`append`](#group__base_1ga566b4b57f0c04d33b75482fd9c7ed0c8) | Formats the given local date and time according to the given format and appends it to str. |
| [`append`](#group__base_1ga6155681cf2b57eaad1260053a3b07def) | Formats the given timespan according to the given format and appends it to str. |
| [`tzdISO`](#group__base_1ga709ac3bdd21971e1fd2a86363b1caba8) | Formats the given timezone differential in ISO format. If timeZoneDifferential is UTC, "Z" is returned, otherwise, +HH.MM (or -HH.MM) is returned. |
| [`tzdRFC`](#group__base_1ga56de331398bdfad483c2870ff4ece483) | Formats the given timezone differential in RFC format. If timeZoneDifferential is UTC, "GMT" is returned, otherwise ++HHMM (or -HHMM) is returned. |
| [`tzdISO`](#group__base_1ga6feb30c9395ec3415dd62beb4403d7e9) | Formats the given timezone differential in ISO format and appends it to the given string. If timeZoneDifferential is UTC, "Z" is returned, otherwise, +HH.MM (or -HH.MM) is returned. |
| [`tzdRFC`](#group__base_1gad71eec1009915e80215e20a6528f7c43) | Formats the given timezone differential in RFC format and appends it to the given string. If timeZoneDifferential is UTC, "GMT" is returned, otherwise ++HHMM (or -HHMM) is returned. |

---

####  

```cpp
enum 
```

| Value | Description |
|-------|-------------|
| `UTC` | Special value for timeZoneDifferential denoting UTC. |

---

#### format 

```cpp
static inline std::string format(const Timestamp & timestamp, const std::string & fmt, int timeZoneDifferential)
```

Formats the given timestamp according to the given format. The format string is used as a template to format the date and is copied character by character except for the following special characters, which are replaced by the corresponding value.

* w - abbreviated weekday (Mon, Tue, ...)

* W - full weekday (Monday, Tuesday, ...)

* b - abbreviated month (Jan, Feb, ...)

* B - full month (January, February, ...)

* d - zero-padded day of month (01 .. 31)

* e - day of month (1 .. 31)

* f - space-padded day of month ( 1 .. 31)

* m - zero-padded month (01 .. 12)

* n - month (1 .. 12)

* o - space-padded month ( 1 .. 12)

* y - year without century (70)

* Y - year with century (1970)

* H - hour (00 .. 23)

* h - hour (00 .. 12)

* a - am/pm

* A - AM/PM

* M - minute (00 .. 59)

* S - second (00 .. 59)

* s - seconds and microseconds (equivalent to S.F)

* i - millisecond (000 .. 999)

* c - centisecond (0 .. 9)

* F - fractional seconds/microseconds (000000 - 999999)

* z - time zone differential in ISO 8601 format (Z or +NN.NN)

* Z - time zone differential in RFC format (GMT or +NNNN)

* %% - percent sign

Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timestamp` | `const Timestamp &` |  |
| `fmt` | `const std::string &` |  |
| `timeZoneDifferential` | `int` |  |

---

#### format 

```cpp
static inline std::string format(const DateTime & dateTime, const std::string & fmt, int timeZoneDifferential)
```

Formats the given date and time according to the given format. See [format(const Timestamp&, const std::string&, int)](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for more information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const DateTime &` |  |
| `fmt` | `const std::string &` |  |
| `timeZoneDifferential` | `int` |  |

---

#### format 

```cpp
static inline std::string format(const LocalDateTime & dateTime, const std::string & fmt)
```

Formats the given local date and time according to the given format. See [format(const Timestamp&, const std::string&, int)](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for more information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dateTime` | `const LocalDateTime &` |  |
| `fmt` | `const std::string &` |  |

---

#### format 

```cpp
static inline std::string format(const Timespan & timespan, const std::string & fmt)
```

Formats the given timespan according to the given format. The format string is used as a template to format the date and is copied character by character except for the following special characters, which are replaced by the corresponding value.

* d - days

* H - hours (00 .. 23)

* h - total hours (0 .. n)

* M - minutes (00 .. 59)

* m - total minutes (0 .. n)

* S - seconds (00 .. 59)

* s - total seconds (0 .. n)

* i - milliseconds (000 .. 999)

* c - centisecond (0 .. 9)

* F - fractional seconds/microseconds (000000 - 999999)

* %% - percent sign

| Parameter | Type | Description |
|-----------|------|-------------|
| `timespan` | `const Timespan &` |  |
| `fmt` | `const std::string &` |  |

---

#### append 

```cpp
static inline void append(std::string & str, const Timestamp & timestamp, const std::string & fmt, int timeZoneDifferential)
```

Formats the given timestamp according to the given format and appends it to str.

See [format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for documentation of the formatting string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `timestamp` | `const Timestamp &` |  |
| `fmt` | `const std::string &` |  |
| `timeZoneDifferential` | `int` |  |

---

#### append 

```cpp
static void append(std::string & str, const DateTime & dateTime, const std::string & fmt, int timeZoneDifferential)
```

Formats the given date and time according to the given format and appends it to str.

See [format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for documentation of the formatting string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `dateTime` | `const DateTime &` |  |
| `fmt` | `const std::string &` |  |
| `timeZoneDifferential` | `int` |  |

---

#### append 

```cpp
static void append(std::string & str, const LocalDateTime & dateTime, const std::string & fmt)
```

Formats the given local date and time according to the given format and appends it to str.

See [format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for documentation of the formatting string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `dateTime` | `const LocalDateTime &` |  |
| `fmt` | `const std::string &` |  |

---

#### append 

```cpp
static void append(std::string & str, const Timespan & timespan, const std::string & fmt)
```

Formats the given timespan according to the given format and appends it to str.

See [format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for documentation of the formatting string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `timespan` | `const Timespan &` |  |
| `fmt` | `const std::string &` |  |

---

#### tzdISO 

```cpp
static inline std::string tzdISO(int timeZoneDifferential)
```

Formats the given timezone differential in ISO format. If timeZoneDifferential is UTC, "Z" is returned, otherwise, +HH.MM (or -HH.MM) is returned.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeZoneDifferential` | `int` |  |

---

#### tzdRFC 

```cpp
static inline std::string tzdRFC(int timeZoneDifferential)
```

Formats the given timezone differential in RFC format. If timeZoneDifferential is UTC, "GMT" is returned, otherwise ++HHMM (or -HHMM) is returned.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeZoneDifferential` | `int` |  |

---

#### tzdISO 

```cpp
static void tzdISO(std::string & str, int timeZoneDifferential)
```

Formats the given timezone differential in ISO format and appends it to the given string. If timeZoneDifferential is UTC, "Z" is returned, otherwise, +HH.MM (or -HH.MM) is returned.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `timeZoneDifferential` | `int` |  |

---

#### tzdRFC 

```cpp
static void tzdRFC(std::string & str, int timeZoneDifferential)
```

Formats the given timezone differential in RFC format and appends it to the given string. If timeZoneDifferential is UTC, "GMT" is returned, otherwise ++HHMM (or -HHMM) is returned.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `std::string &` |  |
| `timeZoneDifferential` | `int` |  |

## DateTimeParser 

> **Defined in:** `datetime.h`

This class provides a method for parsing dates and times from strings. All parsing methods do their best to parse a meaningful result, even from malformed input strings.

The returned [DateTime](#classicy_1_1DateTime) will always contain a time in the same timezone as the time in the string. Call [DateTime::makeUTC()](#group__base_1gaef85751cde47a0e909aeddae1775b805) with the timeZoneDifferential returned by [parse()](#group__base_1ga7249fe6e774b64ab401a6f3f97311f26) to convert the [DateTime](#classicy_1_1DateTime) to UTC.

Note: When parsing a time in 12-hour (AM/PM) format, the hour (h) must be parsed before the AM/PM designator (a, A), otherwise the AM/PM designator will be ignored.

See the [DateTimeFormatter](#classicy_1_1DateTimeFormatter) class for a list of supported format specifiers. In addition to the format specifiers supported by [DateTimeFormatter](#classicy_1_1DateTimeFormatter), an additional specifier is supported: r will parse a year given by either two or four digits. Years 69-00 are interpreted in the 20th century (1969-2000), years 01-68 in the 21th century (2001-2068).

### Members

| Name | Description |
|------|-------------|
| [`parse`](#group__base_1ga7249fe6e774b64ab401a6f3f97311f26) | Parses a date and time in the given format from the given string. Throws a SyntaxException if the string cannot be successfully parsed. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats. |
| [`parse`](#group__base_1ga7f26e4baca235538426f02338c3342fc) | Parses a date and time in the given format from the given string. Throws a SyntaxException if the string cannot be successfully parsed. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats. |
| [`tryParse`](#group__base_1ga56574df84989361f4a604e46b46d85b7) | Parses a date and time in the given format from the given string. Returns true if the string has been successfully parsed, false otherwise. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats. |
| [`parse`](#group__base_1ga3724f5eae88f6d685735d51732eeef1a) | Parses a date and time from the given dateTime string. Before parsing, the method examines the dateTime string for a known date/time format. Throws a SyntaxException if the string cannot be successfully parsed. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats. |
| [`parse`](#group__base_1ga61d6cfa09678ba2987fe32ba4b54a032) | Parses a date and time from the given dateTime string. Before parsing, the method examines the dateTime string for a known date/time format. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats. |
| [`tryParse`](#group__base_1ga1fa3ddbd5eac453273f03100e14b4b95) | Parses a date and time from the given dateTime string. Before parsing, the method examines the dateTime string for a known date/time format. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats. |
| [`parseMonth`](#group__base_1ga4901cfc35835848ebd0026032039fcaf) | Tries to interpret the given range as a month name. The range must be at least three characters long. Returns the month number (1 .. 12) if the month name is valid. Otherwise throws a SyntaxException. |
| [`parseDayOfWeek`](#group__base_1gad11bed124776792fd833d6347cbc8e0f) | Tries to interpret the given range as a weekday name. The range must be at least three characters long. Returns the weekday number (0 .. 6, where 0 = Synday, 1 = Monday, etc.) if the weekday name is valid. Otherwise throws a SyntaxException. |
| [`parseTZD`](#group__base_1gab1f9c2e9e62a2704bf88762ab4706b48) |  |
| [`parseAMPM`](#group__base_1ga0f80915827b85ccdb6fd92ffff25b728) |  |

---

#### parse 

```cpp
static void parse(const std::string & fmt, const std::string & str, DateTime & dateTime, int & timeZoneDifferential)
```

Parses a date and time in the given format from the given string. Throws a SyntaxException if the string cannot be successfully parsed. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fmt` | `const std::string &` |  |
| `str` | `const std::string &` |  |
| `dateTime` | `DateTime &` |  |
| `timeZoneDifferential` | `int &` |  |

---

#### parse 

```cpp
static DateTime parse(const std::string & fmt, const std::string & str, int & timeZoneDifferential)
```

Parses a date and time in the given format from the given string. Throws a SyntaxException if the string cannot be successfully parsed. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fmt` | `const std::string &` |  |
| `str` | `const std::string &` |  |
| `timeZoneDifferential` | `int &` |  |

---

#### tryParse 

```cpp
static bool tryParse(const std::string & fmt, const std::string & str, DateTime & dateTime, int & timeZoneDifferential)
```

Parses a date and time in the given format from the given string. Returns true if the string has been successfully parsed, false otherwise. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fmt` | `const std::string &` |  |
| `str` | `const std::string &` |  |
| `dateTime` | `DateTime &` |  |
| `timeZoneDifferential` | `int &` |  |

---

#### parse 

```cpp
static void parse(const std::string & str, DateTime & dateTime, int & timeZoneDifferential)
```

Parses a date and time from the given dateTime string. Before parsing, the method examines the dateTime string for a known date/time format. Throws a SyntaxException if the string cannot be successfully parsed. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |
| `dateTime` | `DateTime &` |  |
| `timeZoneDifferential` | `int &` |  |

---

#### parse 

```cpp
static DateTime parse(const std::string & str, int & timeZoneDifferential)
```

Parses a date and time from the given dateTime string. Before parsing, the method examines the dateTime string for a known date/time format. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |
| `timeZoneDifferential` | `int &` |  |

---

#### tryParse 

```cpp
static bool tryParse(const std::string & str, DateTime & dateTime, int & timeZoneDifferential)
```

Parses a date and time from the given dateTime string. Before parsing, the method examines the dateTime string for a known date/time format. Please see [DateTimeFormatter::format()](#group__base_1ga321b8e51f548b4e27bbdd2f54168228a) for a description of the format string. Class [DateTimeFormat](#classicy_1_1DateTimeFormat) defines format strings for various standard date/time formats.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |
| `dateTime` | `DateTime &` |  |
| `timeZoneDifferential` | `int &` |  |

---

#### parseMonth 

```cpp
static int parseMonth(std::string::const_iterator & it, const std::string::const_iterator & end)
```

Tries to interpret the given range as a month name. The range must be at least three characters long. Returns the month number (1 .. 12) if the month name is valid. Otherwise throws a SyntaxException.

| Parameter | Type | Description |
|-----------|------|-------------|
| `it` | `std::string::const_iterator &` |  |
| `end` | `const std::string::const_iterator &` |  |

---

#### parseDayOfWeek 

```cpp
static int parseDayOfWeek(std::string::const_iterator & it, const std::string::const_iterator & end)
```

Tries to interpret the given range as a weekday name. The range must be at least three characters long. Returns the weekday number (0 .. 6, where 0 = Synday, 1 = Monday, etc.) if the weekday name is valid. Otherwise throws a SyntaxException.

| Parameter | Type | Description |
|-----------|------|-------------|
| `it` | `std::string::const_iterator &` |  |
| `end` | `const std::string::const_iterator &` |  |

---

#### parseTZD 

```cpp
static int parseTZD(std::string::const_iterator & it, const std::string::const_iterator & end)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `it` | `std::string::const_iterator &` |  |
| `end` | `const std::string::const_iterator &` |  |

---

#### parseAMPM 

```cpp
static int parseAMPM(std::string::const_iterator & it, const std::string::const_iterator & end, int hour)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `it` | `std::string::const_iterator &` |  |
| `end` | `const std::string::const_iterator &` |  |
| `hour` | `int` |  |

## Stopwatch 

> **Defined in:** `datetime.h`

A simple facility to measure time intervals with microsecond resolution.

The [Stopwatch](#classicy_1_1Stopwatch) uses the current system time, so if the system time changes the measured time will be incorrect.

### Members

| Name | Description |
|------|-------------|
| [`Stopwatch`](#group__base_1gac4e0a026e16191af82dd820b98d10dfb) |  |
| [`~Stopwatch`](#group__base_1ga8c4cf8243072ce31339f003a06a7f2e6) |  |
| [`start`](#group__base_1ga668b373cd0428065e574eb0f8bf591a5) | Starts (or restarts) the stopwatch. |
| [`stop`](#group__base_1ga4843077bf122dca1078f9fa84c5a9b84) | Stops or pauses the stopwatch. |
| [`reset`](#group__base_1ga8d1261800ad12511d76821ea78c76611) | Resets the stopwatch. |
| [`restart`](#group__base_1ga67dc5bd64beea0d85f8a691b0bb2f7fb) | Resets and starts the stopwatch. |
| [`elapsed`](#group__base_1ga2faaf087859f8cd8443a9b0dc61e2fbb) | Returns the elapsed time in microseconds since the stopwatch started. |
| [`elapsedSeconds`](#group__base_1ga004d96123d0dd9f3ddb14e1c6d5ea9a3) | Returns the number of seconds elapsed since the stopwatch started. |
| [`elapsedMilliseconds`](#group__base_1ga1a1d93858e58200b50061d20d3f8f0e6) | Returns the number of milliseconds elapsed since the stopwatch started. |
| [`resolution`](#group__base_1ga2a0ddf3cd805c968a72dabab9faebdd9) | Returns the resolution of the stopwatch. |
| [`Stopwatch`](#group__base_1ga2835ead121dae5fe2ec03d178e34d4f8) |  |
| [`operator=`](#group__base_1ga8235704317a8ecaf2af18d8cfb5b78e3) |  |
| [`_start`](#group__base_1ga282c91a1ad8ab7cc457fd3787a16e232) |  |
| [`_elapsed`](#group__base_1ga96484d48f351cf9862574351323f7833) |  |
| [`_running`](#group__base_1ga099c4db05c71b0c9f7c10c8a2147496f) |  |

---

#### Stopwatch 

```cpp
Stopwatch()
```

---

#### ~Stopwatch 

```cpp
~Stopwatch()
```

---

#### start 

```cpp
void start()
```

Starts (or restarts) the stopwatch.

---

#### stop 

```cpp
void stop()
```

Stops or pauses the stopwatch.

---

#### reset 

```cpp
void reset()
```

Resets the stopwatch.

---

#### restart 

```cpp
void restart()
```

Resets and starts the stopwatch.

---

#### elapsed 

```cpp
Timestamp::TimeDiff elapsed() const
```

Returns the elapsed time in microseconds since the stopwatch started.

---

#### elapsedSeconds 

```cpp
int elapsedSeconds() const
```

Returns the number of seconds elapsed since the stopwatch started.

---

#### elapsedMilliseconds 

```cpp
int elapsedMilliseconds() const
```

Returns the number of milliseconds elapsed since the stopwatch started.

---

#### resolution 

```cpp
static Timestamp::TimeVal resolution()
```

Returns the resolution of the stopwatch.

---

#### Stopwatch 

```cpp
Stopwatch(const Stopwatch &) = delete
```

---

#### operator= 

```cpp
Stopwatch & operator=(const Stopwatch &) = delete
```

---

#### _start 

```cpp
Timestamp _start
```

---

#### _elapsed 

```cpp
Timestamp::TimeDiff _elapsed
```

---

#### _running 

```cpp
bool _running
```

## Idler 

> **Extends:** `icy::Runner`
> **Defined in:** `idler.h`

Asynchronous type that triggers callbacks when the event loop is idle.

This class inherits the `[Runner](#classicy_1_1Runner)` interface and may be used with any implementation that's powered by an asynchronous `[Runner](#classicy_1_1Runner)`.

### Members

| Name | Description |
|------|-------------|
| [`Idler`](#group__base_1ga739bc010da58a894a3645ff96a4d952b) | Create the idler with the given event loop. |
| [`Idler`](#group__base_1ga5668cd56a3f7b9e94dd156ed16c81daf) | Create and start the idler with the given callback. |
| [`Idler`](#group__base_1ga436dab9be6d7df1fcbd7648231f2b7b5) | Create and start the idler with the given callback and event loop. |
| [`start`](#group__base_1ga08c0528ddb37409dbc34a605c66a0e32) | Start the idler with the given callback function. |
| [`start`](#group__base_1gae20b7dc946cc87b954d55ee98b13e42d) | Start the idler with the given callback function. |
| [`~Idler`](#group__base_1ga11188502419543dad55d458c911517f3) |  |
| [`handle`](#group__base_1ga93b2d76ea0682c2cf152393d3e36c34f) |  |
| [`init`](#group__base_1ga331ca6dc143e20e5520d09d6db3e6f46) |  |
| [`async`](#group__base_1ga234255f1e2a998e67d9749ef9451ec8c) | Returns true if the implementation is thread-based, or false if it belongs to an event loop. |
| [`_handle`](#group__base_1gaffe75a6686ac64a54381ce771fc72846) |  |

---

#### Idler 

```cpp
Idler(uv::Loop * loop)
```

Create the idler with the given event loop.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### Idler 

```cpp
template<typename Function, typename... Args> inline explicit Idler(Function && func, Args &&... args)
```

Create and start the idler with the given callback.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |

---

#### Idler 

```cpp
template<typename Function, typename... Args> inline explicit Idler(uv::Loop * loop, Function && func, Args &&... args)
```

Create and start the idler with the given callback and event loop.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |

---

#### start 

```cpp
template<typename Function, typename... Args> inline void start(Function && func, Args &&... args)
```

Start the idler with the given callback function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |

---

#### start 

```cpp
virtual void start(std::function< void()> func)
```

Start the idler with the given callback function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `std::function< void()>` |  |

---

#### ~Idler 

```cpp
virtual ~Idler() = default
```

---

#### handle 

```cpp
uv::Handle< uv_idle_t > & handle()
```

---

#### init 

```cpp
virtual void init()
```

---

#### async 

```cpp
virtual bool async() const
```

Returns true if the implementation is thread-based, or false if it belongs to an event loop.

---

#### _handle 

```cpp
uv::Handle< uv_idle_t > _handle
```

## Decoder 

> **Subclasses:** `icy::base64::Decoder`, `icy::hex::Decoder`
> **Defined in:** `interface.h`

### Members

| Name | Description |
|------|-------------|
| [`Decoder`](#group__base_1gade9cc166b3b0ef9ff1f0b50b77e7274e) |  |
| [`~Decoder`](#group__base_1ga50e48ec8e55949e65a6982499bfc10a8) |  |
| [`decode`](#group__base_1gac65b84911c78dd1a08a1a6350598574f) |  |
| [`finalize`](#group__base_1ga47f23ea47df219cfdcd17139e0d699d8) |  |

---

#### Decoder 

```cpp
Decoder() = default
```

---

#### ~Decoder 

```cpp
virtual ~Decoder() = default
```

---

#### decode 

```cpp
ssize_t decode(const char * inbuf, size_t nread, char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

#### finalize 

```cpp
virtual inline ssize_t finalize(char *)
```

## Encoder 

> **Subclasses:** `icy::base64::Encoder`, `icy::hex::Encoder`
> **Defined in:** `interface.h`

### Members

| Name | Description |
|------|-------------|
| [`Encoder`](#group__base_1gaf17b2ede23093905c0053f9026ed6d27) |  |
| [`~Encoder`](#group__base_1ga66430a3e3d8cf10fb45792b86c11b1da) |  |
| [`encode`](#group__base_1ga0568bf005c544b0ba6a51e2aec7b27f7) |  |
| [`finalize`](#group__base_1gae834fb2c3fc0400200e8668967487271) |  |

---

#### Encoder 

```cpp
Encoder() = default
```

---

#### ~Encoder 

```cpp
virtual ~Encoder() = default
```

---

#### encode 

```cpp
ssize_t encode(const char * inbuf, size_t nread, char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

#### finalize 

```cpp
virtual inline ssize_t finalize(char *)
```

## Runnable 

> **Subclasses:** `icy::RunnableQueue< IPacket >`, `icy::RunnableQueue< PacketT >`, `icy::AsyncDiagnostic`, `icy::AsyncLogWriter`, `icy::RunnableQueue< T >`, `icy::Task`, `icy::TaskRunner`, `icy::av::MediaCapture`, `icy::pacm::InstallTask`
> **Defined in:** `interface.h`

Abstract interface for classes that can be run and cancelled.

### Members

| Name | Description |
|------|-------------|
| [`Runnable`](#group__base_1ga24989581d30677599fd7049641aad045) |  |
| [`~Runnable`](#group__base_1gaf7dbd42fb236f2bb1a9148a295ee9e15) |  |
| [`run`](#group__base_1ga58cbd03f96d0fcbac2441437f5325e5e) | The run method will be called by the asynchronous context. |
| [`cancel`](#group__base_1ga0443ed87de7952461d0948c88c8b003f) | Cancel the current task. The [run()](#group__base_1ga58cbd03f96d0fcbac2441437f5325e5e) method should return ASAP. |
| [`cancelled`](#group__base_1gab3795d080eae2e58a57f2c63524c8ef3) | Returns true when the task has been cancelled. |
| [`exit`](#group__base_1gaccdc67bba64aeb38d3c7a6c4f07d9964) |  |

---

#### Runnable 

```cpp
inline Runnable()
```

---

#### ~Runnable 

```cpp
virtual ~Runnable() = default
```

---

#### run 

```cpp
void run()
```

The run method will be called by the asynchronous context.

---

#### cancel 

```cpp
virtual inline void cancel(bool flag)
```

Cancel the current task. The [run()](#group__base_1ga58cbd03f96d0fcbac2441437f5325e5e) method should return ASAP.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### cancelled 

```cpp
virtual inline bool cancelled() const
```

Returns true when the task has been cancelled.

---

#### exit 

```cpp
std::atomic< bool > exit
```

## Startable 

> **Subclasses:** `icy::ThreadedStreamReader`, `icy::av::ICapture`, `icy::http::FormWriter`
> **Defined in:** `interface.h`

Abstract interface for a classes that can be started and stopped.

### Members

| Name | Description |
|------|-------------|
| [`start`](#group__base_1gaccc9e2a0d97f8a69d75f3970e1ccbd78) |  |
| [`stop`](#group__base_1ga8c7cffc58e6830dade1d8434f7b5822d) |  |

---

#### start 

```cpp
void start()
```

---

#### stop 

```cpp
void stop()
```

## Sendable 

> **Subclasses:** `icy::PacketTransaction< sockio::Packet >`, `icy::PacketTransaction< Message >`, `icy::PacketTransaction< PacketT >`
> **Defined in:** `interface.h`

Abstract interface for classes that can be sent and cancelled.

### Members

| Name | Description |
|------|-------------|
| [`send`](#group__base_1ga2bd0718ede40baace8487cd06b87fcff) |  |
| [`cancel`](#group__base_1gaf2764466a92113997b48d40baf2403e0) |  |

---

#### send 

```cpp
bool send()
```

---

#### cancel 

```cpp
void cancel()
```

## Queue 

> **Subclasses:** `icy::ipc::SyncQueue< TAction >`
> **Defined in:** `ipc.h`

IPC queue is for safely passing templated actions between threads and processes.

### Members

| Name | Description |
|------|-------------|
| [`Queue`](#group__base_1gac93ef0838e9baed8333908c8d65e7b8d) |  |
| [`~Queue`](#group__base_1ga9546d7f27045e6717afa2125dfafd053) |  |
| [`push`](#group__base_1ga9c786634a41240b2c27b2bf3de496f9c) |  |
| [`pop`](#group__base_1ga02cdcc43483f08820b6eea3866d01c9d) |  |
| [`runSync`](#group__base_1ga1e0208d28978fee4cb039205a0fbc020) |  |
| [`close`](#group__base_1ga0efb103e4e82dc199cba8c3908183ea4) |  |
| [`post`](#group__base_1ga3bb872c395705ae78b72cfafc188e1fd) |  |
| [`waitForSync`](#group__base_1ga394493f310e54d4bb33265cb840fefdf) |  |
| [`_mutex`](#group__base_1gabe0c2735e26749941052787051bb44bd) |  |
| [`_actions`](#group__base_1ga238c51fb2dd7bf29152e4863d13fa1e5) |  |

---

#### Queue 

```cpp
inline Queue()
```

---

#### ~Queue 

```cpp
virtual inline ~Queue()
```

---

#### push 

```cpp
virtual inline void push(TAction * action)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `action` | `TAction *` |  |

---

#### pop 

```cpp
virtual inline TAction * pop()
```

---

#### runSync 

```cpp
virtual inline void runSync()
```

---

#### close 

```cpp
virtual inline void close()
```

---

#### post 

```cpp
virtual inline void post()
```

---

#### waitForSync 

```cpp
inline void waitForSync()
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _actions 

```cpp
std::deque< TAction * > _actions
```

## SyncQueue 

> **Extends:** `icy::ipc::Queue< ipc::Action >`
> **Defined in:** `ipc.h`

IPC synchronization queue is for passing templated actions between threads and the event loop we are synchronizing with.

### Members

| Name | Description |
|------|-------------|
| [`SyncQueue`](#group__base_1ga5d75b88fd48fd9b14a1300775471fcbb) |  |
| [`~SyncQueue`](#group__base_1gae3f23ea3eb42ed63fbb0850d43805178) |  |
| [`close`](#group__base_1ga8546519d8077f5c05a52d9f2b926b233) |  |
| [`post`](#group__base_1gabbb1f498c837051e29c0553ed4127db8) |  |
| [`sync`](#group__base_1ga88b2c8e4905f07f99f5e36573eb9ac4c) |  |
| [`_sync`](#group__base_1gad6b153706fc24a42c70b32def9f2f6aa) |  |

---

#### SyncQueue 

```cpp
inline SyncQueue(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~SyncQueue 

```cpp
virtual inline ~SyncQueue()
```

---

#### close 

```cpp
virtual inline void close()
```

---

#### post 

```cpp
virtual inline void post()
```

---

#### sync 

```cpp
virtual inline Synchronizer & sync()
```

---

#### _sync 

```cpp
Synchronizer _sync
```

## LogWriter 

> **Subclasses:** `icy::AsyncLogWriter`
> **Defined in:** `logger.h`

Log output stream writer.

### Members

| Name | Description |
|------|-------------|
| [`LogWriter`](#group__base_1gad880791856a3ee0c440304b8ad6aa0fd) |  |
| [`~LogWriter`](#group__base_1ga7059e18a3a0c69fe8333f2bbf9a9e6d0) |  |
| [`write`](#group__base_1gafd98afb4e7030c196f9e167460336c1f) | Writes the given log message stream. |

---

#### LogWriter 

```cpp
LogWriter()
```

---

#### ~LogWriter 

```cpp
virtual ~LogWriter()
```

---

#### write 

```cpp
virtual void write(std::unique_ptr< LogStream > stream)
```

Writes the given log message stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `std::unique_ptr< LogStream >` |  |

## AsyncLogWriter 

> **Extends:** `icy::LogWriter`, `icy::basic::Runnable`
> **Defined in:** `logger.h`

[Thread](#classicy_1_1Thread) based log output stream writer.

### Members

| Name | Description |
|------|-------------|
| [`AsyncLogWriter`](#group__base_1ga0188ef05599b6dda991e15a125926a93) |  |
| [`~AsyncLogWriter`](#group__base_1ga94107b494835580b0e435f9c7ef870fe) |  |
| [`write`](#group__base_1gaeaa026442c5927b21b599ded87601908) | Queues the given log message stream. |
| [`flush`](#group__base_1ga6af94a2ef68c7b0114f6d91684a2aca7) | Flushes queued messages. |
| [`run`](#group__base_1gaa38a45dab3b692894a6d1753dadb7164) | Writes queued messages asynchronously. |
| [`clear`](#group__base_1gad918ab56f2652c0df2a0b4c74823fc18) | Clears all queued messages. |
| [`writeNext`](#group__base_1ga05d6ccb870b82f130810f3c9d002d9bb) |  |
| [`_thread`](#group__base_1ga75dcded0c2370591afb489f4892b7b54) |  |
| [`_pending`](#group__base_1ga60cdd0b73b13f1d27e06a9e4fedea209) |  |
| [`_mutex`](#group__base_1ga6a888007ca6e0e1bddd6a6378b9f36e0) |  |

---

#### AsyncLogWriter 

```cpp
AsyncLogWriter()
```

---

#### ~AsyncLogWriter 

```cpp
virtual ~AsyncLogWriter()
```

---

#### write 

```cpp
virtual void write(std::unique_ptr< LogStream > stream)
```

Queues the given log message stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `std::unique_ptr< LogStream >` |  |

---

#### flush 

```cpp
void flush()
```

Flushes queued messages.

---

#### run 

```cpp
virtual void run()
```

Writes queued messages asynchronously.

---

#### clear 

```cpp
void clear()
```

Clears all queued messages.

---

#### writeNext 

```cpp
bool writeNext()
```

---

#### _thread 

```cpp
Thread _thread
```

---

#### _pending 

```cpp
std::deque< std::unique_ptr< LogStream > > _pending
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## Logger 

> **Defined in:** `logger.h`

[Logger](#classicy_1_1Logger) class.

### Members

| Name | Description |
|------|-------------|
| [`Logger`](#group__base_1gaa58d26b288c4baf5595bbbcb71653b95) |  |
| [`~Logger`](#group__base_1ga84c1d5e6cd7053bd476eca1eca9e8030) |  |
| [`add`](#group__base_1ga05923e337fd59d892c096ad31afa3801) | Adds the given log channel. Takes ownership. |
| [`remove`](#group__base_1ga72971c94e65ab1660ec9bb00a1bcd54e) | Removes the given log channel by name. |
| [`get`](#group__base_1ga91e6c5fd7fa7340056fe8ba8935c2641) | Returns the specified log channel. Throws an exception if the channel doesn't exist. |
| [`setDefault`](#group__base_1ga7ec2b37dc4de80b4790a522f8c08f0a8) | Sets the default log to the specified log channel. |
| [`setWriter`](#group__base_1ga4f547043311cd4aa045d67b156d6575d) | Sets the log writer instance. Takes ownership. |
| [`getDefault`](#group__base_1ga5a48a75f4ee17d6bc3fa81b92331d64d) | Returns the default log channel, or the nullptr channel if no default channel has been set. |
| [`write`](#group__base_1gadea484eefd15977b355c26107cd325df) | Writes the given message to the default log channel. The message will be copied. |
| [`write`](#group__base_1ga267674b14b4556d9acd86e16ef8b331c) | Writes the given message to the default log channel. |
| [`instance`](#group__base_1ga803744d1eb81ca3ba367c5e0081c1e98) | Returns the default logger singleton. [Logger](#classicy_1_1Logger) instances may be created separately as needed. |
| [`setInstance`](#group__base_1ga89d453d2d992f8dde3b57732f3c204c5) | Sets the default logger singleton instance. |
| [`destroy`](#group__base_1ga5972c9db1f70e5a85b85e48401e9168a) | Destroys the default logger singleton instance. |
| [`Logger`](#group__base_1gae25ba15fd8c92541a296f5be97f0ece8) | NonCopyable and NonMovable. |
| [`operator=`](#group__base_1gacb35ed074bb58c0d6f69109ee331d95d) |  |
| [`_mutex`](#group__base_1ga01d897e3d08b06665c99e2a4fe27fc12) |  |
| [`_channels`](#group__base_1ga9477bfaa930adfd93c981708c72fff5f) |  |
| [`_defaultChannel`](#group__base_1ga0bc50bd2da990173ed15e251b7df2e9c) |  |
| [`_writer`](#group__base_1gacf4bad357409e6cee0c4d0c0594c7bb8) |  |

---

#### Logger 

```cpp
Logger()
```

---

#### ~Logger 

```cpp
~Logger()
```

---

#### add 

```cpp
void add(std::unique_ptr< LogChannel > channel)
```

Adds the given log channel. Takes ownership.

| Parameter | Type | Description |
|-----------|------|-------------|
| `channel` | `std::unique_ptr< LogChannel >` |  |

---

#### remove 

```cpp
void remove(const std::string & name)
```

Removes the given log channel by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### get 

```cpp
LogChannel * get(const std::string & name, bool whiny) const
```

Returns the specified log channel. Throws an exception if the channel doesn't exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### setDefault 

```cpp
void setDefault(const std::string & name)
```

Sets the default log to the specified log channel.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### setWriter 

```cpp
void setWriter(std::unique_ptr< LogWriter > writer)
```

Sets the log writer instance. Takes ownership.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `std::unique_ptr< LogWriter >` |  |

---

#### getDefault 

```cpp
LogChannel * getDefault() const
```

Returns the default log channel, or the nullptr channel if no default channel has been set.

---

#### write 

```cpp
void write(const LogStream & stream)
```

Writes the given message to the default log channel. The message will be copied.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `const LogStream &` |  |

---

#### write 

```cpp
void write(std::unique_ptr< LogStream > stream)
```

Writes the given message to the default log channel.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `std::unique_ptr< LogStream >` |  |

---

#### instance 

```cpp
static Logger & instance()
```

Returns the default logger singleton. [Logger](#classicy_1_1Logger) instances may be created separately as needed.

---

#### setInstance 

```cpp
static void setInstance(Logger * logger, bool freeExisting)
```

Sets the default logger singleton instance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `logger` | `Logger *` |  |
| `freeExisting` | `bool` |  |

---

#### destroy 

```cpp
static void destroy()
```

Destroys the default logger singleton instance.

---

#### Logger 

```cpp
Logger(const Logger &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
Logger & operator=(const Logger &) = delete
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _channels 

```cpp
LogChannelMap _channels
```

---

#### _defaultChannel 

```cpp
LogChannel * _defaultChannel
```

---

#### _writer 

```cpp
std::unique_ptr< LogWriter > _writer
```

## LogChannel 

> **Subclasses:** `icy::ConsoleChannel`, `icy::FileChannel`, `icy::RotatingFileChannel`
> **Defined in:** `logger.h`

### Members

| Name | Description |
|------|-------------|
| [`LogChannel`](#group__base_1ga6eb210d878e572311225b015a9cb0bf1) |  |
| [`~LogChannel`](#group__base_1gadd94d58dca0580ff476c68e3526f3aff) |  |
| [`write`](#group__base_1gac8341cc041aa699d3e7ee7f41d37f568) |  |
| [`write`](#group__base_1gaf330bc738bee8077756d40cb366c3aee) |  |
| [`format`](#group__base_1gafff619cedc1fa6a263b64f198b100307) |  |
| [`name`](#group__base_1gafa0ee6ecc108c2f153760696afc335b8) |  |
| [`level`](#group__base_1gaf151e272e61eb39e9d29bb973eac88f0) |  |
| [`timeFormat`](#group__base_1gad20ec0d479792546f08423b0631cff9d) |  |
| [`setLevel`](#group__base_1ga6d3ccc136614e8aee1b811b42ac1013c) |  |
| [`setTimeFormat`](#group__base_1ga8a169d04776fc4c28bd7e1b22e22895a) |  |
| [`setFilter`](#group__base_1ga897e0402cc8b441489deff693575a444) |  |
| [`_name`](#group__base_1ga36f066b53b2c1afe8928bfd882011fbb) |  |
| [`_level`](#group__base_1gad78b4c8b505efe909158ee7e52828a15) |  |
| [`_timeFormat`](#group__base_1gaf59b0b2da184acb4d50f9bba96221d70) |  |
| [`_filter`](#group__base_1ga40e25f529270dfc4ec4a40b14e514a02) |  |

---

#### LogChannel 

```cpp
LogChannel(std::string name, Level level, std::string timeFormat)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` |  |
| `level` | `Level` |  |
| `timeFormat` | `std::string` |  |

---

#### ~LogChannel 

```cpp
virtual ~LogChannel() = default
```

---

#### write 

```cpp
virtual void write(const LogStream & stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `const LogStream &` |  |

---

#### write 

```cpp
virtual void write(std::string message, Level level, std::string realm)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `std::string` |  |
| `level` | `Level` |  |
| `realm` | `std::string` |  |

---

#### format 

```cpp
virtual void format(const LogStream & stream, std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `const LogStream &` |  |
| `ost` | `std::ostream &` |  |

---

#### name 

```cpp
inline std::string name() const
```

---

#### level 

```cpp
inline Level level() const
```

---

#### timeFormat 

```cpp
inline std::string timeFormat() const
```

---

#### setLevel 

```cpp
inline void setLevel(Level level)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `level` | `Level` |  |

---

#### setTimeFormat 

```cpp
inline void setTimeFormat(std::string format)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `format` | `std::string` |  |

---

#### setFilter 

```cpp
inline void setFilter(std::string filter)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `filter` | `std::string` |  |

---

#### _name 

```cpp
std::string _name
```

---

#### _level 

```cpp
Level _level
```

---

#### _timeFormat 

```cpp
std::string _timeFormat
```

---

#### _filter 

```cpp
std::string _filter
```

## ConsoleChannel 

> **Extends:** `icy::LogChannel`
> **Defined in:** `logger.h`

### Members

| Name | Description |
|------|-------------|
| [`ConsoleChannel`](#group__base_1ga53bca70103bf1cc5384c49d5e8f2ccdc) |  |
| [`~ConsoleChannel`](#group__base_1gaadb6451f2e487ad09f037f89bf964d46) |  |
| [`write`](#group__base_1ga0929915fd0556254dcdaa1d49cc34758) |  |

---

#### ConsoleChannel 

```cpp
ConsoleChannel(std::string name, Level level, std::string timeFormat)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` |  |
| `level` | `Level` |  |
| `timeFormat` | `std::string` |  |

---

#### ~ConsoleChannel 

```cpp
virtual ~ConsoleChannel() = default
```

---

#### write 

```cpp
virtual void write(const LogStream & stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `const LogStream &` |  |

## FileChannel 

> **Extends:** `icy::LogChannel`
> **Defined in:** `logger.h`

### Members

| Name | Description |
|------|-------------|
| [`FileChannel`](#group__base_1gab4f92ce254f7bf2252fba993ea320429) |  |
| [`~FileChannel`](#group__base_1gaeb8131af118653a2b09bca46660fc2b1) |  |
| [`write`](#group__base_1gabf845ee5a13c4e4662566764d7cc2d0f) |  |
| [`setPath`](#group__base_1ga2ae18223e682fc74ced9f079b9473f64) |  |
| [`path`](#group__base_1ga4399a5666e22bab99beba14926b6db56) |  |
| [`open`](#group__base_1gac96f91229ba9e1d0566f6402aae8c294) |  |
| [`close`](#group__base_1ga69982cecf5699940fd17ef9852d88ea8) |  |
| [`_fstream`](#group__base_1ga4f6cafce9cad0d449da791f4a8ff6416) |  |
| [`_path`](#group__base_1gaee5340f4b20b5361e05e84cb3eca9fd3) |  |

---

#### FileChannel 

```cpp
FileChannel(std::string name, std::string path, Level level, std::string timeFormat)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` |  |
| `path` | `std::string` |  |
| `level` | `Level` |  |
| `timeFormat` | `std::string` |  |

---

#### ~FileChannel 

```cpp
virtual ~FileChannel()
```

---

#### write 

```cpp
virtual void write(const LogStream & stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `const LogStream &` |  |

---

#### setPath 

```cpp
void setPath(const std::string & path)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### path 

```cpp
std::string path() const
```

---

#### open 

```cpp
virtual void open()
```

---

#### close 

```cpp
virtual void close()
```

---

#### _fstream 

```cpp
std::ofstream _fstream
```

---

#### _path 

```cpp
std::string _path
```

## RotatingFileChannel 

> **Extends:** `icy::LogChannel`
> **Defined in:** `logger.h`

### Members

| Name | Description |
|------|-------------|
| [`RotatingFileChannel`](#group__base_1ga84f26cb25b2c0891d75871b6716168a3) |  |
| [`~RotatingFileChannel`](#group__base_1gaac06718485f99a815fea1794918add66) |  |
| [`write`](#group__base_1gad7ae8aa634cf62b9218c1db4a9e94036) |  |
| [`rotate`](#group__base_1ga6987d2d74c5bf0d806b910262dafc18b) |  |
| [`dir`](#group__base_1gad734c262397421ebe099d6e721cc1825) |  |
| [`filename`](#group__base_1gadd1ee0bd5e6a7b8e3c81dcb7d599e0a7) |  |
| [`rotationInterval`](#group__base_1ga9507632f6b9471837ebcf1220ce9bee6) |  |
| [`setDir`](#group__base_1ga2cfa616f69cd79ace910e9b67658e70e) |  |
| [`setExtension`](#group__base_1ga394e6a3fee41f0629b5101d185a4197c) |  |
| [`setRotationInterval`](#group__base_1gadeb5c1ec6bed59029009039ebe623ef0) |  |
| [`_fstream`](#group__base_1ga4f6cc715458e53fae26d2d7562e3e8b5) |  |
| [`_dir`](#group__base_1gaccead79b2337cea1e03927f09cc6eb2f) |  |
| [`_filename`](#group__base_1ga997dcf71593d8450e25f5f33b1e41dd4) |  |
| [`_extension`](#group__base_1gacbb348e159c2da3ce607710957a39bfb) |  |
| [`_rotationInterval`](#group__base_1ga6fcee513ef9e7648ca8b83691fcf8fe6) | The log rotation interval in seconds. |
| [`_rotatedAt`](#group__base_1gad6355b7ebfeeb0605c24c4934d45e912) | The time the log was last rotated. |

---

#### RotatingFileChannel 

```cpp
RotatingFileChannel(std::string name, std::string dir, Level level, std::string extension, int rotationInterval, std::string timeFormat)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `std::string` |  |
| `dir` | `std::string` |  |
| `level` | `Level` |  |
| `extension` | `std::string` |  |
| `rotationInterval` | `int` |  |
| `timeFormat` | `std::string` |  |

---

#### ~RotatingFileChannel 

```cpp
virtual ~RotatingFileChannel()
```

---

#### write 

```cpp
virtual void write(const LogStream & stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `const LogStream &` |  |

---

#### rotate 

```cpp
virtual void rotate()
```

---

#### dir 

```cpp
inline std::string dir() const
```

---

#### filename 

```cpp
inline std::string filename() const
```

---

#### rotationInterval 

```cpp
inline int rotationInterval() const
```

---

#### setDir 

```cpp
inline void setDir(std::string dir)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dir` | `std::string` |  |

---

#### setExtension 

```cpp
inline void setExtension(std::string ext)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ext` | `std::string` |  |

---

#### setRotationInterval 

```cpp
inline void setRotationInterval(int interval)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `interval` | `int` |  |

---

#### _fstream 

```cpp
std::unique_ptr< std::ofstream > _fstream
```

---

#### _dir 

```cpp
std::string _dir
```

---

#### _filename 

```cpp
std::string _filename
```

---

#### _extension 

```cpp
std::string _extension
```

---

#### _rotationInterval 

```cpp
int _rotationInterval
```

The log rotation interval in seconds.

---

#### _rotatedAt 

```cpp
time_t _rotatedAt
```

The time the log was last rotated.

## IPacket 

> **Subclasses:** `icy::FlagPacket`, `icy::RawPacket`, `icy::smpl::Message`, `icy::sockio::Packet`, `icy::stun::Message`
> **Defined in:** `packet.h`

The basic packet type which is passed around the Icey system. [IPacket](#classicy_1_1IPacket) can be extended for each protocol to enable polymorphic processing and callbacks using [PacketStream](#classicy_1_1PacketStream) and friends.

### Members

| Name | Description |
|------|-------------|
| [`IPacket`](#group__base_1ga18264ec6de7582fc121a467e3ea7183c) |  |
| [`IPacket`](#group__base_1gaadf363596a491bcfa114040d3e5f1ce5) |  |
| [`operator=`](#group__base_1gae6bef3dd34fec6b60b3f399d7646fee6) |  |
| [`clone`](#group__base_1ga55fb6c9a605afc8f788d517db17f5c94) |  |
| [`~IPacket`](#group__base_1gafc24256c3b16380ba4556a82ea721971) |  |
| [`read`](#group__base_1ga17dec4307d132f37add42674fbfc18f1) | Read/parse to the packet from the given input buffer. The number of bytes read is returned. |
| [`write`](#group__base_1gabdcb765c58d74be25e190680bf7483d6) | Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer. |
| [`size`](#group__base_1ga2fe3e4a722ab42218066bd6ae91bf336) | The size of the packet in bytes. |
| [`hasData`](#group__base_1ga4fc1159e6f71b7231f8de2220f6f0a83) |  |
| [`data`](#group__base_1gaf62b3af8177804474ca7270c44a08809) | The packet data pointer for buffered packets. |
| [`constData`](#group__base_1ga459a655ebf7651ddf0f7c5ebf8d3d6dc) | The const packet data pointer for buffered packets. |
| [`className`](#group__base_1ga7fbf025db89783335cb7296e35531ccb) |  |
| [`print`](#group__base_1gad23b82ac26af7d46ca3828f1116ee9d7) |  |
| [`source`](#group__base_1gad3f9d5b27d476a5e693dd752036e2aa9) | Packet source pointer reference which enables processors along the signal chain can determine the packet origin. Often a subclass of PacketStreamSource. |
| [`opaque`](#group__base_1gaf6b50b2656e3dfc60907b366b35fb1c0) | Optional client data pointer. This pointer is not managed by the packet. |
| [`info`](#group__base_1gaa3bc87e58c6c9b2dce8a4e8a661236d0) | Optional extra information about the packet. |
| [`flags`](#group__base_1ga9eb92d88fc9705ff304ddd382c7588fa) | Provides basic information about the packet. |

---

#### IPacket 

```cpp
inline IPacket(void * source, void * opaque, std::unique_ptr< IPacketInfo > info, unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `void *` |  |
| `opaque` | `void *` |  |
| `info` | `std::unique_ptr< IPacketInfo >` |  |
| `flags` | `unsigned` |  |

---

#### IPacket 

```cpp
inline IPacket(const IPacket & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const IPacket &` |  |

---

#### operator= 

```cpp
inline IPacket & operator=(const IPacket & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const IPacket &` |  |

---

#### clone 

```cpp
IPacket * clone() const
```

---

#### ~IPacket 

```cpp
virtual ~IPacket() = default
```

---

#### read 

```cpp
ssize_t read(const ConstBuffer &)
```

Read/parse to the packet from the given input buffer. The number of bytes read is returned.

---

#### write 

```cpp
void write(Buffer &) const
```

Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer.

Todo: It may be prefferable to use our pod types here instead of buffer input, but the current codebase requires that the buffer be dynamically resizable for some protocols...

---

#### size 

```cpp
virtual inline size_t size() const
```

The size of the packet in bytes.

This is the nember of bytes that will be written on a call to [write()](#group__base_1gabdcb765c58d74be25e190680bf7483d6), but may not be the number of bytes that will be consumed by [read()](#group__base_1ga17dec4307d132f37add42674fbfc18f1).

---

#### hasData 

```cpp
virtual inline bool hasData() const
```

---

#### data 

```cpp
virtual inline char * data() const
```

The packet data pointer for buffered packets.

---

#### constData 

```cpp
virtual inline const void * constData() const
```

The const packet data pointer for buffered packets.

---

#### className 

```cpp
const char * className() const
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

#### source 

```cpp
void * source
```

Packet source pointer reference which enables processors along the signal chain can determine the packet origin. Often a subclass of PacketStreamSource.

---

#### opaque 

```cpp
void * opaque
```

Optional client data pointer. This pointer is not managed by the packet.

---

#### info 

```cpp
std::unique_ptr< IPacketInfo > info
```

Optional extra information about the packet.

---

#### flags 

```cpp
Bitwise flags
```

Provides basic information about the packet.

## FlagPacket 

> **Extends:** `icy::IPacket`
> **Defined in:** `packet.h`

Packet for sending bitwise flags along the packet stream.

### Members

| Name | Description |
|------|-------------|
| [`FlagPacket`](#group__base_1ga653e36ffe31957974caf6d74eac86ada) |  |
| [`clone`](#group__base_1ga9787ee63af30cbd45b90120954dea731) |  |
| [`FlagPacket`](#group__base_1ga2fe2b6ce02c8780ccb958867d7b40f68) |  |
| [`~FlagPacket`](#group__base_1ga2533d0666de1de52036a90928753baab) |  |
| [`read`](#group__base_1gae9244ad486c645bf78967acf967567e7) | Read/parse to the packet from the given input buffer. The number of bytes read is returned. |
| [`write`](#group__base_1ga47591bd1b58f3f6b996dc968144bc855) | Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer. |
| [`className`](#group__base_1ga38299df2b1e45317582db01f127a7d33) |  |

---

#### FlagPacket 

```cpp
inline FlagPacket(unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flags` | `unsigned` |  |

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### FlagPacket 

```cpp
inline FlagPacket(const FlagPacket & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const FlagPacket &` |  |

---

#### ~FlagPacket 

```cpp
virtual ~FlagPacket() = default
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

## RawPacket 

> **Extends:** `icy::IPacket`
> **Subclasses:** `icy::av::MediaPacket`, `icy::net::SocketPacket`
> **Defined in:** `packet.h`

[RawPacket](#classicy_1_1RawPacket) is the default data packet type which consists of an optionally managed char pointer and a size value.

### Members

| Name | Description |
|------|-------------|
| [`RawPacket`](#group__base_1ga8bbe5592ab4c6990427a9f5d0aaacd56) | Construct with borrowed (non-owning) buffer. |
| [`RawPacket`](#group__base_1ga29a1c6212a4b355ea9f0af2fe2df5540) | Construct with const data (copied, owning). |
| [`RawPacket`](#group__base_1ga8e345ba779f5ac06df86e2a6c7ec2ead) | Copy constructor (always copies data). |
| [`~RawPacket`](#group__base_1ga7323a9ff445c569f2fdbab4c3eca348d) |  |
| [`clone`](#group__base_1ga4df70dab414133565b2ca3491ba7bfa4) |  |
| [`copyData`](#group__base_1ga45af5d87430a9c3f899dbbfedd5e6997) |  |
| [`read`](#group__base_1ga27d4b98e4dabc04b7fe1986a9e1c03bb) | Read/parse to the packet from the given input buffer. The number of bytes read is returned. |
| [`write`](#group__base_1ga8d7ce1ab0cac231c74c0daaa0c01fcaf) | Copy/generate to the packet given output buffer. The number of bytes written can be obtained from the buffer. |
| [`data`](#group__base_1ga699d6504e49924674c57d4f028aa162c) | The packet data pointer for buffered packets. |
| [`size`](#group__base_1ga052fc05cf75fe7162a9fe9c4923d11a5) | The size of the packet in bytes. |
| [`className`](#group__base_1ga84508aba2ea50772f03fc76bbb97fa3d) |  |
| [`ownsBuffer`](#group__base_1gad11ad552cf59a9bf88d54644bd1c0259) |  |
| [`_data`](#group__base_1ga846efb5bb786079552a8d91617ddf1d2) |  |
| [`_size`](#group__base_1ga236b72d805511afae7400d6543df19fb) |  |
| [`_owned`](#group__base_1gaf735f77b6cce15dbf19a3dd4ca9b7470) |  |

---

#### RawPacket 

```cpp
inline RawPacket(char * data, size_t size, unsigned flags, void * source, void * opaque, std::unique_ptr< IPacketInfo > info)
```

Construct with borrowed (non-owning) buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `char *` |  |
| `size` | `size_t` |  |
| `flags` | `unsigned` |  |
| `source` | `void *` |  |
| `opaque` | `void *` |  |
| `info` | `std::unique_ptr< IPacketInfo >` |  |

---

#### RawPacket 

```cpp
inline RawPacket(const char * data, size_t size, unsigned flags, void * source, void * opaque, std::unique_ptr< IPacketInfo > info)
```

Construct with const data (copied, owning).

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `size` | `size_t` |  |
| `flags` | `unsigned` |  |
| `source` | `void *` |  |
| `opaque` | `void *` |  |
| `info` | `std::unique_ptr< IPacketInfo >` |  |

---

#### RawPacket 

```cpp
inline RawPacket(const RawPacket & that)
```

Copy constructor (always copies data).

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const RawPacket &` |  |

---

#### ~RawPacket 

```cpp
virtual ~RawPacket() = default
```

---

#### clone 

```cpp
virtual inline IPacket * clone() const
```

---

#### copyData 

```cpp
virtual inline void copyData(const void * data, size_t size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const void *` |  |
| `size` | `size_t` |  |

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

#### data 

```cpp
virtual inline char * data() const
```

The packet data pointer for buffered packets.

---

#### size 

```cpp
virtual inline size_t size() const
```

The size of the packet in bytes.

This is the nember of bytes that will be written on a call to [write()](#group__base_1ga8d7ce1ab0cac231c74c0daaa0c01fcaf), but may not be the number of bytes that will be consumed by [read()](#group__base_1ga27d4b98e4dabc04b7fe1986a9e1c03bb).

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### ownsBuffer 

```cpp
inline bool ownsBuffer() const
```

---

#### _data 

```cpp
char * _data
```

---

#### _size 

```cpp
size_t _size
```

---

#### _owned 

```cpp
std::unique_ptr< char[]> _owned
```

## IPacketCreationStrategy 

> **Subclasses:** `icy::PacketCreationStrategy< PacketT >`
> **Defined in:** `packetfactory.h`

### Members

| Name | Description |
|------|-------------|
| [`IPacketCreationStrategy`](#group__base_1ga50f56327263cb8d8ee572496ea9a7a3b) |  |
| [`~IPacketCreationStrategy`](#group__base_1gaaefe166ced07d6decb2b9f8072e12d1b) |  |
| [`create`](#group__base_1gae4853a278a11b59b96d02789a78a3ba7) |  |
| [`priority`](#group__base_1gaba808d34edb6272253418c30210f25ec) |  |
| [`compareProiroty`](#group__base_1ga66ee4fd6b354dd7f97e954a03af23c13) |  |

---

#### IPacketCreationStrategy 

```cpp
inline IPacketCreationStrategy()
```

---

#### ~IPacketCreationStrategy 

```cpp
virtual ~IPacketCreationStrategy() = default
```

---

#### create 

```cpp
IPacket * create(const ConstBuffer & buffer, size_t & nread) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const ConstBuffer &` |  |
| `nread` | `size_t &` |  |

---

#### priority 

```cpp
int priority() const
```

---

#### compareProiroty 

```cpp
static inline bool compareProiroty(const IPacketCreationStrategy * l, const IPacketCreationStrategy * r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `l` | `const IPacketCreationStrategy *` |  |
| `r` | `const IPacketCreationStrategy *` |  |

## PacketFactory 

> **Defined in:** `packetfactory.h`

### Members

| Name | Description |
|------|-------------|
| [`PacketFactory`](#group__base_1gac108d58054dccbf3736c7ecb2a968d58) |  |
| [`~PacketFactory`](#group__base_1ga803341e3fb9567e3017ad1774acda0fd) |  |
| [`registerPacketType`](#group__base_1ga362ade3e31ecca2015cdb85cec6c9e4f) |  |
| [`unregisterPacketType`](#group__base_1ga6e1ee51199e2b5241afb8f872120f48a) |  |
| [`registerStrategy`](#group__base_1gaec4ee2b41f920d44c5e3276992e16a48) |  |
| [`unregisterStrategy`](#group__base_1gabdbc79c8ee73048b8cff7ece2f465b30) |  |
| [`types`](#group__base_1gac5f739f1472bd97d68629ceadd27c9c7) |  |
| [`types`](#group__base_1ga78324767a673d825a34e6b6e02a29036) |  |
| [`onPacketCreated`](#group__base_1ga86ec412fa2a46187ddf8154cab0afd5c) |  |
| [`createPacket`](#group__base_1ga63cd252d598c9a379bb9de996d450f4d) |  |
| [`_types`](#group__base_1gac2bbd50d6da749659cc25265ba3b13e6) |  |

---

#### PacketFactory 

```cpp
inline PacketFactory()
```

---

#### ~PacketFactory 

```cpp
virtual inline ~PacketFactory()
```

---

#### registerPacketType 

```cpp
template<class PacketT> inline void registerPacketType(int priority)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `priority` | `int` |  |

---

#### unregisterPacketType 

```cpp
template<class PacketT> inline void unregisterPacketType()
```

---

#### registerStrategy 

```cpp
template<class StrategyT> inline void registerStrategy(int priority)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `priority` | `int` |  |

---

#### unregisterStrategy 

```cpp
template<class StrategyT> inline void unregisterStrategy()
```

---

#### types 

```cpp
inline PacketCreationStrategyList & types()
```

---

#### types 

```cpp
inline PacketCreationStrategyList types() const
```

---

#### onPacketCreated 

```cpp
virtual inline bool onPacketCreated(IPacket *)
```

---

#### createPacket 

```cpp
virtual inline IPacket * createPacket(const ConstBuffer & buffer, size_t & nread)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const ConstBuffer &` |  |
| `nread` | `size_t &` |  |

---

#### _types 

```cpp
PacketCreationStrategyList _types
```

## ThreadedStreamReader 

> **Extends:** `icy::PacketStreamAdapter`, `icy::basic::Startable`
> **Defined in:** `packetio.h`

Threaded stream reader class.

This class can be connected to a `[PacketStream](#classicy_1_1PacketStream)` to read input from any class that derives from `std::istream`. It's most regularly used for reading input files.

### Members

| Name | Description |
|------|-------------|
| [`ThreadedStreamReader`](#group__base_1ga145262d7cc0e8082313771c78d719a93) |  |
| [`~ThreadedStreamReader`](#group__base_1ga39fe2eb7ff89764e1bdea0dd6c391b09) |  |
| [`start`](#group__base_1ga199b9b2a213b92b4e81a735d457f82ac) |  |
| [`stop`](#group__base_1ga447294977952a774ce6abafed560c3d7) |  |
| [`stream`](#group__base_1ga8887679c8111b21d63686450bb62eead) |  |
| [`stream`](#group__base_1gaaa188157fe727154da089d04ff924aa0) |  |
| [`emitter`](#group__base_1ga567cb288d73e8913f61ba6d76008b22f) |  |
| [`_runner`](#group__base_1ga209b1d5e46545ea7ed88bc0f0492f22d) |  |
| [`_istream`](#group__base_1gaf859c1452a438a01f7faf1771f4f8463) |  |

---

#### ThreadedStreamReader 

```cpp
inline ThreadedStreamReader(std::istream * is)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `is` | `std::istream *` |  |

---

#### ~ThreadedStreamReader 

```cpp
inline ~ThreadedStreamReader()
```

---

#### start 

```cpp
virtual inline void start()
```

---

#### stop 

```cpp
virtual inline void stop()
```

---

#### stream 

```cpp
template<class StreamT> inline StreamT & stream()
```

---

#### stream 

```cpp
inline std::istream & stream()
```

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### _runner 

```cpp
Thread _runner
```

---

#### _istream 

```cpp
std::istream * _istream
```

## StreamWriter 

> **Extends:** `icy::PacketProcessor`
> **Defined in:** `packetio.h`

Packet stream writer class.

This class can be connected to a `[PacketStream](#classicy_1_1PacketStream)` to write output to any class that derives from `std::ostream`. It's most regularly used for writing output files.

### Members

| Name | Description |
|------|-------------|
| [`StreamWriter`](#group__base_1gad15868ece5ffa0a63ae71ab555b0d43f) |  |
| [`~StreamWriter`](#group__base_1ga0bdd60e6b4e56dcde24d2b7f372d55e2) |  |
| [`process`](#group__base_1gae8a1107b09b3644ef7c69ac70bb88459) | This method performs processing on the given packet and emits the result. |
| [`stream`](#group__base_1gac2501915ffeb2643615ae43936d8aedc) |  |
| [`onStreamStateChange`](#group__base_1ga22f372f88ca99f812be3e888a40fba77) | Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`stream`](#group__base_1gaa16cb5b70ea5470206bec6e56ae43b23) |  |
| [`emitter`](#group__base_1ga1996908eb8e6301955ff4df97c5a22a7) |  |
| [`_ostream`](#group__base_1ga2e9fe16cc8bec4c2ab33673eda06f4ff) |  |

---

#### StreamWriter 

```cpp
inline StreamWriter(std::ostream * stream)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `std::ostream *` |  |

---

#### ~StreamWriter 

```cpp
virtual inline ~StreamWriter()
```

---

#### process 

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### stream 

```cpp
template<class StreamT> inline StreamT & stream()
```

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### stream 

```cpp
inline std::ostream & stream()
```

---

#### emitter 

```cpp
PacketSignal emitter
```

---

#### _ostream 

```cpp
std::ostream * _ostream
```

## SyncPacketQueue 

> **Extends:** `icy::SyncQueue< IPacket >`, `icy::PacketProcessor`
> **Defined in:** `packetqueue.h`

### Members

| Name | Description |
|------|-------------|
| [`SyncPacketQueue`](#group__base_1gae6d88a8cc455a3fcc463e302a4fa1b1f) |  |
| [`SyncPacketQueue`](#group__base_1ga10b57a26379bac19ff0f4c48b367ab18) |  |
| [`~SyncPacketQueue`](#group__base_1gaaddebcff191f315fa84dd988bb9f9f64) |  |
| [`process`](#group__base_1ga75df139f954f41a0d315009a2ae5e000) | This method performs processing on the given packet and emits the result. |
| [`accepts`](#group__base_1ga00793d12f0252f6f4b2e439ecb725829) | This method ensures compatibility with the given packet type. Return false to reject the packet. |
| [`dispatch`](#group__base_1ga11c2c12c52dd842cd37d83ea8f05147c) | Dispatch a single item to listeners. |
| [`onStreamStateChange`](#group__base_1ga37f27e601fe6cd6e73c5b3da83b7634d) | Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`emitter`](#group__base_1ga9cb2e51e85e32907283c98c62fc5967d) |  |

---

#### SyncPacketQueue 

```cpp
inline SyncPacketQueue(uv::Loop * loop, int maxSize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |
| `maxSize` | `int` |  |

---

#### SyncPacketQueue 

```cpp
inline SyncPacketQueue(int maxSize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `maxSize` | `int` |  |

---

#### ~SyncPacketQueue 

```cpp
virtual inline ~SyncPacketQueue()
```

---

#### process 

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### accepts 

```cpp
virtual inline bool accepts(IPacket *)
```

This method ensures compatibility with the given packet type. Return false to reject the packet.

---

#### dispatch 

```cpp
virtual inline void dispatch(T & item)
```

Dispatch a single item to listeners.

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `T &` |  |

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### emitter 

```cpp
PacketSignal emitter
```

## AsyncPacketQueue 

> **Extends:** `icy::AsyncQueue< IPacket >`, `icy::PacketProcessor`
> **Defined in:** `packetqueue.h`

### Members

| Name | Description |
|------|-------------|
| [`AsyncPacketQueue`](#group__base_1gab12334be9597666d9ccdbd8c1dc2b8b6) |  |
| [`~AsyncPacketQueue`](#group__base_1ga1d52cb92da8849a198e4c47aea73876c) |  |
| [`close`](#group__base_1gab25027fe2c2a2cb00edf378a4e314ea0) |  |
| [`process`](#group__base_1ga7566b9cd568c49a13b63816ced79121f) | This method performs processing on the given packet and emits the result. |
| [`accepts`](#group__base_1ga5076d28d49bf343e866c5014e538e8af) | This method ensures compatibility with the given packet type. Return false to reject the packet. |
| [`dispatch`](#group__base_1gaf6e78ca0279615319d33d7e6aa624e55) | Dispatch a single item to listeners. |
| [`onStreamStateChange`](#group__base_1gae72126b2b0f118975423119a3ddd6978) | Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`emitter`](#group__base_1gaf462c1a658ea10a25f0159c3adf22054) |  |

---

#### AsyncPacketQueue 

```cpp
inline AsyncPacketQueue(int maxSize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `maxSize` | `int` |  |

---

#### ~AsyncPacketQueue 

```cpp
virtual inline ~AsyncPacketQueue()
```

---

#### close 

```cpp
virtual inline void close()
```

---

#### process 

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### accepts 

```cpp
virtual inline bool accepts(IPacket *)
```

This method ensures compatibility with the given packet type. Return false to reject the packet.

---

#### dispatch 

```cpp
virtual inline void dispatch(T & item)
```

Dispatch a single item to listeners.

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `T &` |  |

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### emitter 

```cpp
PacketSignal emitter
```

## PacketStreamAdapter 

> **Subclasses:** `icy::PacketProcessor`, `icy::ThreadedStreamReader`, `icy::av::ICapture`, `icy::http::FormWriter`
> **Defined in:** `packetstream.h`

This class is a wrapper for integrating external classes with the a [PacketStream](#classicy_1_1PacketStream)'s data flow and state machine.

### Members

| Name | Description |
|------|-------------|
| [`PacketStreamAdapter`](#group__base_1ga33639caa7975d01430b01643da0f3ddb) |  |
| [`~PacketStreamAdapter`](#group__base_1ga6223fd3eac5802d6efa67b37515c7c3f) |  |
| [`emit`](#group__base_1ga995248310998c29df87051389f52b58c) |  |
| [`emit`](#group__base_1ga298c0975355b11dfc09669d4663a0260) |  |
| [`emit`](#group__base_1gaefade7945aa72cd6b1fee74746fda713) |  |
| [`emit`](#group__base_1ga4f329880c10ea2492479ddd90df48b09) |  |
| [`emit`](#group__base_1ga3b302bbd1b8c8eb21d549eda77ab2e58) |  |
| [`getEmitter`](#group__base_1ga3c56e12afc6c4863d9f70258dfe6c2a5) | Returns a reference to the outgoing packet signal. |
| [`onStreamStateChange`](#group__base_1gacaab737ce646207471c5f7bb3b6b794e) | Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios. |
| [`PacketStreamAdapter`](#group__base_1ga2e52c68e66f76569dacf29a41cf8a73b) | NonCopyable and NonMovable. |
| [`operator=`](#group__base_1gab56d24063e6015ea655f22ba9d5723f1) |  |
| [`_emitter`](#group__base_1ga080cfd0707ca633ed93b77aa066c6e8b) |  |

---

#### PacketStreamAdapter 

```cpp
PacketStreamAdapter(PacketSignal & emitter)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `emitter` | `PacketSignal &` |  |

---

#### ~PacketStreamAdapter 

```cpp
virtual inline ~PacketStreamAdapter()
```

---

#### emit 

```cpp
virtual void emit(char * data, size_t len, unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `char *` |  |
| `len` | `size_t` |  |
| `flags` | `unsigned` |  |

---

#### emit 

```cpp
virtual void emit(const char * data, size_t len, unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `flags` | `unsigned` |  |

---

#### emit 

```cpp
virtual void emit(const std::string & str, unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |
| `flags` | `unsigned` |  |

---

#### emit 

```cpp
virtual void emit(unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flags` | `unsigned` |  |

---

#### emit 

```cpp
virtual void emit(IPacket & packet)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### getEmitter 

```cpp
PacketSignal & getEmitter()
```

Returns a reference to the outgoing packet signal.

---

#### onStreamStateChange 

```cpp
virtual inline void onStreamStateChange(const PacketStreamState &)
```

Called by the [PacketStream](#classicy_1_1PacketStream) to notify when the internal [Stream](#classicy_1_1Stream) state changes. On receiving the Stopped state, it is the responsibility of the adapter to have ceased all outgoing packet transmission, especially in multi-thread scenarios.

---

#### PacketStreamAdapter 

```cpp
PacketStreamAdapter(const PacketStreamAdapter &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
PacketStreamAdapter & operator=(const PacketStreamAdapter &) = delete
```

---

#### _emitter 

```cpp
PacketSignal & _emitter
```

## PacketProcessor 

> **Extends:** `icy::PacketStreamAdapter`
> **Subclasses:** `icy::AsyncPacketQueue< PacketT >`, `icy::AsyncPacketQueue< T >`, `icy::Base64PacketEncoder`, `icy::StreamWriter`, `icy::SyncPacketQueue< T >`, `icy::av::FLVMetadataInjector`, `icy::av::FPSLimiter`, `icy::av::MultiplexPacketEncoder`, `icy::http::ChunkedAdapter`, `icy::http::MultipartAdapter`
> **Defined in:** `packetstream.h`

This class is a virtual interface for creating PacketStreamAdapters which process that and emit the [IPacket](#classicy_1_1IPacket) type.

### Members

| Name | Description |
|------|-------------|
| [`PacketProcessor`](#group__base_1gaf3ab7df2c2416fdb3bd4e2b313c8b772) |  |
| [`process`](#group__base_1gad7adc3fd78dce41f0f96744dfe6d1d50) | This method performs processing on the given packet and emits the result. |
| [`accepts`](#group__base_1gacb581741dd22cc5c0191397b32419ddc) | This method ensures compatibility with the given packet type. Return false to reject the packet. |
| [`operator<<`](#group__base_1ga0b09928130bd267d58273ea85be1891a) | [Stream](#classicy_1_1Stream) operator alias for [process()](#group__base_1gad7adc3fd78dce41f0f96744dfe6d1d50) |

---

#### PacketProcessor 

```cpp
inline PacketProcessor(PacketSignal & emitter)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `emitter` | `PacketSignal &` |  |

---

#### process 

```cpp
void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### accepts 

```cpp
virtual inline bool accepts(IPacket *)
```

This method ensures compatibility with the given packet type. Return false to reject the packet.

---

#### operator<< 

```cpp
virtual inline void operator<<(IPacket & packet)
```

[Stream](#classicy_1_1Stream) operator alias for [process()](#group__base_1gad7adc3fd78dce41f0f96744dfe6d1d50)

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

## PacketStream 

> **Extends:** `icy::Stateful< PacketStreamState >`
> **Defined in:** `packetstream.h`

This class is used for processing and boradcasting IPackets in a flexible way. A [PacketStream](#classicy_1_1PacketStream) consists of one or many PacketSources, one or many PacketProcessors, and one or many delegate receivers.

This class enables the developer to setup a processor chain in order to perform arbitrary processing on data packets using interchangeable packet adapters, and pump the output to any delegate function,/// or even another [PacketStream](#classicy_1_1PacketStream).

Note that [PacketStream](#classicy_1_1PacketStream) itself inherits from [PacketStreamAdapter](#classicy_1_1PacketStreamAdapter),/// so a [PacketStream](#classicy_1_1PacketStream) be the source of another [PacketStream](#classicy_1_1PacketStream).

All [PacketStream](#classicy_1_1PacketStream) methods are thread-safe, but once the stream is running you will not be able to attach or detach stream adapters.

In order to synchronize output packets with the application event loop take a look at the [SyncPacketQueue](#classicy_1_1SyncPacketQueue) class. For lengthy operations you can add an [AsyncPacketQueue](#classicy_1_1AsyncPacketQueue) to the start of the stream to defer processing from the PacketSource thread.

### Members

| Name | Description |
|------|-------------|
| [`PacketStream`](#group__base_1ga7313ef8bb9b19b1b8cf88698e77cbb92) |  |
| [`~PacketStream`](#group__base_1ga2b6dffb019c4da42da8fea435623568e) |  |
| [`start`](#group__base_1gaa07075299f2271e58f78de3fd7e62b10) | Start the stream and synchronized sources. |
| [`stop`](#group__base_1ga5ace99af3f0ff1e93483a7f5dd5dca69) | Stop the stream and synchronized sources. |
| [`pause`](#group__base_1ga866ba120ec05be813d82a6e05827292e) | Pause the stream. |
| [`resume`](#group__base_1ga2e27223e238aa0174180abd379e3149c) | Resume the stream. |
| [`close`](#group__base_1gac0eec4eda5ff0571b46b4a39af9f7dcf) | Close the stream and transition the internal state to Closed. |
| [`reset`](#group__base_1gabf0b20944de791715e6891520fd6784b) | Cleanup all managed stream adapters and reset the stream state. |
| [`active`](#group__base_1ga6f53641239172c34777c6fae3c96fc98) | Returns true when the stream is in the Active state. |
| [`stopped`](#group__base_1gaa1485ea7e14fd0d929886604930a4092) | Returns true when the stream is in the Stopping or Stopped state. |
| [`closed`](#group__base_1ga45d9514c5da12522e7e672f46f433d27) | Returns true when the stream is in the Closed or [Error](#structicy_1_1Error) state. |
| [`lock`](#group__base_1gaa49be3aec494af1412f80a0613d5dc77) | Sets the stream to locked state. In a locked state no new adapters can be added or removed from the stream until the stream is stopped. |
| [`locked`](#group__base_1gad0ff7534821aebeda8731d5840cef8bb) | Returns true is the stream is currently locked. |
| [`write`](#group__base_1ga74422f101e021e1eea6ebab7cd36098d) | Writes data to the stream (nocopy). |
| [`write`](#group__base_1gad7b06d811db22b5c08aa4fce2d86eb19) | Writes data to the stream (copied). |
| [`write`](#group__base_1ga4bd4ad751e112c62f93db2920677e75b) | Writes an incoming packet onto the stream. |
| [`attachSource`](#group__base_1ga16adec5b00a33070556dd8531e1825c0) | Attaches a source packet emitter to the stream. The source packet adapter can be another [PacketStream::emitter](#group__base_1ga3d4fa2d39446cd24ce5d8281834b70db). |
| [`attachSource`](#group__base_1ga42e243c7a6bf9cb7a53725e7ca268e64) | Attaches a source packet emitter to the stream. If owned is true, the stream takes ownership and will delete the pointer. If syncState is true and the source is a [basic::Startable](#classicy_1_1basic_1_1Startable), then the source's [start()](#group__base_1gaa07075299f2271e58f78de3fd7e62b10)/stop() methods will be synchronized when calling [startSources()](#group__base_1gaeb9605f5d35a6aaa92bf427e0989827c)/stopSources(). |
| [`attachSource`](#group__base_1gae6f1bb5accd371bd151cfbabc141b331) | Attaches a source packet emitter to the stream. This method enables compatibility with shared_ptr managed adapter instances. |
| [`detachSource`](#group__base_1ga06e1aa0b843f233d6dc71ca293a24945) | Detaches the given source packet signal from the stream. |
| [`detachSource`](#group__base_1ga4f77bef24858fbb7bf31b43899c6fd27) | Detaches the given source packet adapter from the stream. |
| [`attach`](#group__base_1ga7f94960094b20d385f932c915b24c508) | Attaches a packet processor to the stream. Order determines the position of the processor in the stream queue. If owned is true, the stream takes ownership and will delete the pointer. |
| [`attach`](#group__base_1ga9ddf204e9fa1db2aca165f4d4bd04132) | Attaches a packet processor to the stream. This method enables compatibility with shared_ptr managed adapter instances. |
| [`detach`](#group__base_1ga879c4077767fcbd5def56f4398202d63) | Detaches a packet processor from the stream. |
| [`synchronizeOutput`](#group__base_1ga27a797846a098946cf73d80726302a9e) | Synchronize stream output packets with the given event loop. |
| [`autoStart`](#group__base_1ga2042eee4cb00925265abeaef5cbb2588) | Set the stream to auto start if inactive (default: false). |
| [`closeOnError`](#group__base_1ga58995f68cf550280612199862a7dba84) | Set the stream to be closed on error (default: true). |
| [`error`](#group__base_1ga894d3a083f545aa5d4eb001819ce79f7) | Accessors for the unmanaged client data pointer. |
| [`name`](#group__base_1gacf6a0e6b50acb9575a6cda69863c43f3) | Returns the name of the packet stream. |
| [`adapters`](#group__base_1ga847517ada8ae4492b1fa54eb75fe7d0b) | Returns a combined list of all stream sources and processors. |
| [`sources`](#group__base_1ga95efa9dc101b1ddd2a37f587931a45bc) | Returns a list of all stream sources. |
| [`processors`](#group__base_1ga7c3ddf305de8bff9d02e00338f6a6792) | Returns a list of all stream processors. |
| [`numSources`](#group__base_1ga846914577fbf526e0389ee36820865d8) |  |
| [`numProcessors`](#group__base_1ga7e1d39ba8a8aca30b86c693937dde258) |  |
| [`numAdapters`](#group__base_1gacd6972c47d4945735771841b16163a46) |  |
| [`getSource`](#group__base_1ga6921d9eb5ad3fcd1541093db094b47eb) |  |
| [`getProcessor`](#group__base_1ga54f24499988940ed9dd77f9409cdcf8f) |  |
| [`getProcessor`](#group__base_1ga74ea091cd48ca43f5a04c8cdf1124c77) | Returns the [PacketProcessor](#classicy_1_1PacketProcessor) at the given position. |
| [`setup`](#group__base_1ga2e71c28fac4871d262138e58e048c641) | Attach the source and processor delegate chain. |
| [`teardown`](#group__base_1ga7529aa1efb6d052d08b1aa9d64988a03) | Detach the source and processor delegate chain. |
| [`attachSource`](#group__base_1ga4187fed0c3f68c220a9bb2779481e2db) |  |
| [`attach`](#group__base_1ga6ab3b5c164ac0bab8137ae0e293b7f99) |  |
| [`startSources`](#group__base_1gaeb9605f5d35a6aaa92bf427e0989827c) | Start synchronized sources. |
| [`stopSources`](#group__base_1ga73955535a0312cd3371c3317c1bfa1f0) | Stop synchronized sources. |
| [`process`](#group__base_1ga379f518fe7c1a50af177b738e0b28f17) | [Process](#classicy_1_1Process) incoming packets. |
| [`emit`](#group__base_1ga2ec0154d38011167b59aa54a2cdddd98) | Emit the final packet to listeners. |
| [`synchronizeStates`](#group__base_1ga4abe768801f8b39876834e45ead0ac1e) | Synchronize queued states with adapters. |
| [`onStateChange`](#group__base_1gae5e38cf390b7ac651342801d09a3e0bb) | Override the [Stateful::onStateChange](#group__base_1ga3c88b4aa8e9343f9bdc1a8c2ae2a5452) method. |
| [`assertCanModify`](#group__base_1ga8222585fd3f575c67b6b28f1e7cb5217) | Returns true if the given state ID is queued. |
| [`handleException`](#group__base_1ga40d1f91b9b8cb5216ce132998eaae458) | Handle an internal exception. |
| [`emitter`](#group__base_1ga3d4fa2d39446cd24ce5d8281834b70db) | Signals to delegates on outgoing packets. |
| [`Error`](#group__base_1ga91c28709d238b0cfa21ba0929296d981) | Signals that the [PacketStream](#classicy_1_1PacketStream) is in [Error](#structicy_1_1Error) state. If stream output is synchronized then the [Error](#structicy_1_1Error) signal will be sent from the synchronization context, otherwise it will be sent from the async processor context. See [synchronizeOutput()](#group__base_1ga27a797846a098946cf73d80726302a9e) |
| [`Close`](#group__base_1ga1a7296d92dbedd388ea99ff6436e5037) | Signals that the [PacketStream](#classicy_1_1PacketStream) is in Close state. This signal is sent immediately via the [close()](#group__base_1gac0eec4eda5ff0571b46b4a39af9f7dcf) method, and as such will be sent from the calling thread context. |
| [`_mutex`](#group__base_1ga07b9e3677e5d7811efeee61017ea91f4) |  |
| [`_procMutex`](#group__base_1gacb611f82818fdbd6cbd316240c2e13eb) |  |
| [`_name`](#group__base_1ga928ac73b404916c524349f82b837a251) |  |
| [`_sources`](#group__base_1ga884fa29fc53edcca06ff84997b48cfd5) |  |
| [`_processors`](#group__base_1ga06fe5e28961a3352db8f80138eb30564) |  |
| [`_states`](#group__base_1ga6cdb9717a975c66542a88ce34586265b) |  |
| [`_error`](#group__base_1gaabf4120335505da2f373b7afb64cf156) |  |
| [`_autoStart`](#group__base_1gabc12f096a88953028521259790bae93f) |  |
| [`_closeOnError`](#group__base_1ga255d1421e03dfac4aafbe763c759f85a) |  |

---

#### PacketStream 

```cpp
PacketStream(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### ~PacketStream 

```cpp
virtual ~PacketStream()
```

---

#### start 

```cpp
virtual void start()
```

Start the stream and synchronized sources.

---

#### stop 

```cpp
virtual void stop()
```

Stop the stream and synchronized sources.

---

#### pause 

```cpp
virtual void pause()
```

Pause the stream.

---

#### resume 

```cpp
virtual void resume()
```

Resume the stream.

---

#### close 

```cpp
virtual void close()
```

Close the stream and transition the internal state to Closed.

---

#### reset 

```cpp
virtual void reset()
```

Cleanup all managed stream adapters and reset the stream state.

---

#### active 

```cpp
virtual bool active() const
```

Returns true when the stream is in the Active state.

---

#### stopped 

```cpp
virtual bool stopped() const
```

Returns true when the stream is in the Stopping or Stopped state.

---

#### closed 

```cpp
virtual bool closed() const
```

Returns true when the stream is in the Closed or [Error](#structicy_1_1Error) state.

---

#### lock 

```cpp
virtual bool lock()
```

Sets the stream to locked state. In a locked state no new adapters can be added or removed from the stream until the stream is stopped.

---

#### locked 

```cpp
virtual bool locked() const
```

Returns true is the stream is currently locked.

---

#### write 

```cpp
virtual void write(char * data, size_t len)
```

Writes data to the stream (nocopy).

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `char *` |  |
| `len` | `size_t` |  |

---

#### write 

```cpp
virtual void write(const char * data, size_t len)
```

Writes data to the stream (copied).

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### write 

```cpp
virtual void write(IPacket && packet)
```

Writes an incoming packet onto the stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &&` |  |

---

#### attachSource 

```cpp
virtual void attachSource(PacketSignal & source)
```

Attaches a source packet emitter to the stream. The source packet adapter can be another [PacketStream::emitter](#group__base_1ga3d4fa2d39446cd24ce5d8281834b70db).

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `PacketSignal &` |  |

---

#### attachSource 

```cpp
virtual void attachSource(PacketStreamAdapter * source, bool owned, bool syncState)
```

Attaches a source packet emitter to the stream. If owned is true, the stream takes ownership and will delete the pointer. If syncState is true and the source is a [basic::Startable](#classicy_1_1basic_1_1Startable), then the source's [start()](#group__base_1gaa07075299f2271e58f78de3fd7e62b10)/stop() methods will be synchronized when calling [startSources()](#group__base_1gaeb9605f5d35a6aaa92bf427e0989827c)/stopSources().

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `PacketStreamAdapter *` |  |
| `owned` | `bool` |  |
| `syncState` | `bool` |  |

---

#### attachSource 

```cpp
template<class C> inline void attachSource(std::shared_ptr< C > ptr, bool syncState)
```

Attaches a source packet emitter to the stream. This method enables compatibility with shared_ptr managed adapter instances.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `std::shared_ptr< C >` |  |
| `syncState` | `bool` |  |

---

#### detachSource 

```cpp
virtual bool detachSource(PacketSignal & source)
```

Detaches the given source packet signal from the stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `PacketSignal &` |  |

---

#### detachSource 

```cpp
virtual bool detachSource(PacketStreamAdapter * source)
```

Detaches the given source packet adapter from the stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `PacketStreamAdapter *` |  |

---

#### attach 

```cpp
virtual void attach(PacketProcessor * proc, int order, bool owned)
```

Attaches a packet processor to the stream. Order determines the position of the processor in the stream queue. If owned is true, the stream takes ownership and will delete the pointer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `proc` | `PacketProcessor *` |  |
| `order` | `int` |  |
| `owned` | `bool` |  |

---

#### attach 

```cpp
template<class C> inline void attach(std::shared_ptr< C > ptr, int order, bool syncState)
```

Attaches a packet processor to the stream. This method enables compatibility with shared_ptr managed adapter instances.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `std::shared_ptr< C >` |  |
| `order` | `int` |  |
| `syncState` | `bool` |  |

---

#### detach 

```cpp
virtual bool detach(PacketProcessor * proc)
```

Detaches a packet processor from the stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `proc` | `PacketProcessor *` |  |

---

#### synchronizeOutput 

```cpp
virtual void synchronizeOutput(uv::Loop * loop)
```

Synchronize stream output packets with the given event loop.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### autoStart 

```cpp
virtual void autoStart(bool flag)
```

Set the stream to auto start if inactive (default: false).

With this flag set the stream will automatically transition to Active state if the in either the None or Locaked state.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### closeOnError 

```cpp
virtual void closeOnError(bool flag)
```

Set the stream to be closed on error (default: true).

With this flag set the stream will be automatically transitioned to Closed state from [Error](#structicy_1_1Error) state.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### error 

```cpp
const std::exception_ptr & error()
```

Accessors for the unmanaged client data pointer.

Returns the stream error (if any).

---

#### name 

```cpp
std::string name() const
```

Returns the name of the packet stream.

---

#### adapters 

```cpp
PacketAdapterVec adapters() const
```

Returns a combined list of all stream sources and processors.

---

#### sources 

```cpp
PacketAdapterVec sources() const
```

Returns a list of all stream sources.

---

#### processors 

```cpp
PacketAdapterVec processors() const
```

Returns a list of all stream processors.

---

#### numSources 

```cpp
int numSources() const
```

---

#### numProcessors 

```cpp
int numProcessors() const
```

---

#### numAdapters 

```cpp
int numAdapters() const
```

---

#### getSource 

```cpp
template<class AdapterT> inline AdapterT * getSource(int index)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### getProcessor 

```cpp
template<class AdapterT> inline AdapterT * getProcessor(int index)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### getProcessor 

```cpp
inline PacketProcessor * getProcessor(int order)
```

Returns the [PacketProcessor](#classicy_1_1PacketProcessor) at the given position.

| Parameter | Type | Description |
|-----------|------|-------------|
| `order` | `int` |  |

---

#### setup 

```cpp
void setup()
```

Attach the source and processor delegate chain.

---

#### teardown 

```cpp
void teardown()
```

Detach the source and processor delegate chain.

---

#### attachSource 

```cpp
void attachSource(PacketAdapterReference::Ptr ref)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ref` | `PacketAdapterReference::Ptr` |  |

---

#### attach 

```cpp
void attach(PacketAdapterReference::Ptr ref)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ref` | `PacketAdapterReference::Ptr` |  |

---

#### startSources 

```cpp
void startSources()
```

Start synchronized sources.

---

#### stopSources 

```cpp
void stopSources()
```

Stop synchronized sources.

---

#### process 

```cpp
virtual void process(IPacket & packet)
```

[Process](#classicy_1_1Process) incoming packets.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### emit 

```cpp
void emit(IPacket & packet)
```

Emit the final packet to listeners.

Synchronized signals such as Close and [Error](#structicy_1_1Error) are sent from this method. See [synchronizeOutput()](#group__base_1ga27a797846a098946cf73d80726302a9e)

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### synchronizeStates 

```cpp
void synchronizeStates()
```

Synchronize queued states with adapters.

---

#### onStateChange 

```cpp
virtual void onStateChange(PacketStreamState & state, const PacketStreamState & oldState)
```

Override the [Stateful::onStateChange](#group__base_1ga3c88b4aa8e9343f9bdc1a8c2ae2a5452) method.

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `PacketStreamState &` |  |
| `oldState` | `const PacketStreamState &` |  |

---

#### assertCanModify 

```cpp
void assertCanModify()
```

Returns true if the given state ID is queued.

Asserts that the stream can be modified, ie is not in the Locked, Stopping or Active states.

---

#### handleException 

```cpp
void handleException(std::exception & exc)
```

Handle an internal exception.

| Parameter | Type | Description |
|-----------|------|-------------|
| `exc` | `std::exception &` |  |

---

#### emitter 

```cpp
PacketSignal emitter
```

Signals to delegates on outgoing packets.

---

#### Error 

```cpp
Signal< void(PacketStream &, const std::exception_ptr &)> Error
```

Signals that the [PacketStream](#classicy_1_1PacketStream) is in [Error](#structicy_1_1Error) state. If stream output is synchronized then the [Error](#structicy_1_1Error) signal will be sent from the synchronization context, otherwise it will be sent from the async processor context. See [synchronizeOutput()](#group__base_1ga27a797846a098946cf73d80726302a9e)

---

#### Close 

```cpp
Signal< void(PacketStream &)> Close
```

Signals that the [PacketStream](#classicy_1_1PacketStream) is in Close state. This signal is sent immediately via the [close()](#group__base_1gac0eec4eda5ff0571b46b4a39af9f7dcf) method, and as such will be sent from the calling thread context.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _procMutex 

```cpp
std::mutex _procMutex
```

---

#### _name 

```cpp
std::string _name
```

---

#### _sources 

```cpp
PacketAdapterVec _sources
```

---

#### _processors 

```cpp
PacketAdapterVec _processors
```

---

#### _states 

```cpp
std::deque< PacketStreamState > _states
```

---

#### _error 

```cpp
std::exception_ptr _error
```

---

#### _autoStart 

```cpp
bool _autoStart
```

---

#### _closeOnError 

```cpp
bool _closeOnError
```

## PacketTransaction 

> **Extends:** `icy::basic::Sendable`, `icy::Stateful< TransactionState >`
> **Subclasses:** `icy::net::Transaction< Message >`, `icy::net::Transaction< PacketT >`
> **Defined in:** `packettransaction.h`

This class provides request/response functionality for [IPacket](#classicy_1_1IPacket) types.

PacketTransactions are fire and forget. The object will be deleted after a successful response or a timeout.

### Members

| Name | Description |
|------|-------------|
| [`PacketTransaction`](#group__base_1gadb8e26b65b3a60f24db3a0e8183a66a3) |  |
| [`PacketTransaction`](#group__base_1gaf6a92e72b87b249648ccd46fc433f09c) |  |
| [`send`](#group__base_1gad94b9acb3f91d3a32499b86340b72d2e) | Starts the transaction timer and sends the request. Overriding classes should implement send logic here. |
| [`cancel`](#group__base_1ga49c75e9316f50f660872dd4b249c28ca) | Cancellation means that the agent will not retransmit the request, will not treat the lack of response to be a failure, but will wait the duration of the transaction timeout for a response. |
| [`cancelled`](#group__base_1gaa54ffbb88648bcc1bba5c768bf86b74f) |  |
| [`dispose`](#group__base_1ga620cd3aada050a2cd54a9ea34400c847) | Schedules the transaction for deferred deletion. |
| [`canResend`](#group__base_1ga08ff3ee8b0be3e16a84699c44ea6a2a6) |  |
| [`attempts`](#group__base_1ga4f6506c8e89b3d125fd7c08464c63688) |  |
| [`retries`](#group__base_1ga34cb6bd2af610a3d8c8639a47d0713ef) |  |
| [`request`](#group__base_1gaa5c37750d66412b497c9130b5ad58cc2) |  |
| [`request`](#group__base_1ga44b16598078348c741c8903dfda2b848) |  |
| [`response`](#group__base_1gaa086ddf7d95b1866e89721c0c3ef0e5c) |  |
| [`response`](#group__base_1gaafd5b5bae5829fffac6d6f2bf81204bb) |  |
| [`~PacketTransaction`](#group__base_1gac52c12fac214bb76b7b61c510672ed55) |  |
| [`onStateChange`](#group__base_1ga3f025f9c6286905e0ef499ff3b6c5607) | Override to handle post state change logic. |
| [`handlePotentialResponse`](#group__base_1ga5111866a3ab35fdf98dab7b21a8cbb3d) | Processes a potential response candidate and updates the state accordingly. |
| [`checkResponse`](#group__base_1ga2c0fb5cd9cd4f64f9600d04f2fc5cb34) | Checks a potential response candidate and returns true on successful match. |
| [`onResponse`](#group__base_1ga738b44f24205411c26bcb0ffe8ce6a80) | Called when a successful response is received. |
| [`onTimeout`](#group__base_1ga6c44b73210c0d856a72380a00403c486) |  |
| [`_request`](#group__base_1ga062276bf6ecdca9a64f8b89b6ba8e268) |  |
| [`_response`](#group__base_1ga4c36f743181e84dedf6b71cd6acb5c04) |  |
| [`_timer`](#group__base_1ga56cf3ef1cde1ed234a81adbc534a4d0c) | The request timeout callback. |
| [`_retries`](#group__base_1ga3c72d7ce16d3b6defff73f7fb572ed2b) | The maximum number of attempts before the transaction is considered failed. |
| [`_attempts`](#group__base_1ga15a0177855becd97f06df6f4b24d59a9) | The number of times the transaction has been sent. |
| [`_destroyed`](#group__base_1gae22c00abf59f34d7274f98bbd8e140f0) |  |

---

#### PacketTransaction 

```cpp
inline PacketTransaction(long timeout, int retries, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `long` |  |
| `retries` | `int` |  |
| `loop` | `uv::Loop *` |  |

---

#### PacketTransaction 

```cpp
inline PacketTransaction(const PacketT & request, long timeout, int retries, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `request` | `const PacketT &` |  |
| `timeout` | `long` |  |
| `retries` | `int` |  |
| `loop` | `uv::Loop *` |  |

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

#### cancelled 

```cpp
inline bool cancelled() const
```

---

#### dispose 

```cpp
virtual inline void dispose()
```

Schedules the transaction for deferred deletion.

It is safe to call this function while the transaction is still active, providing the call is made from the same loop thread which the timer is running on.

Protected by the base implementation as this is called by the internal state machine.

---

#### canResend 

```cpp
virtual inline bool canResend()
```

---

#### attempts 

```cpp
inline int attempts() const
```

---

#### retries 

```cpp
inline int retries() const
```

---

#### request 

```cpp
inline PacketT & request()
```

---

#### request 

```cpp
inline PacketT request() const
```

---

#### response 

```cpp
inline PacketT & response()
```

---

#### response 

```cpp
inline PacketT response() const
```

---

#### ~PacketTransaction 

```cpp
virtual inline ~PacketTransaction()
```

---

#### onStateChange 

```cpp
virtual inline void onStateChange(TransactionState & state, const TransactionState &)
```

Override to handle post state change logic.

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `TransactionState &` |  |

---

#### handlePotentialResponse 

```cpp
virtual inline bool handlePotentialResponse(const PacketT & packet)
```

Processes a potential response candidate and updates the state accordingly.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `const PacketT &` |  |

---

#### checkResponse 

```cpp
bool checkResponse(const PacketT & packet)
```

Checks a potential response candidate and returns true on successful match.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `const PacketT &` |  |

---

#### onResponse 

```cpp
virtual inline void onResponse()
```

Called when a successful response is received.

---

#### onTimeout 

```cpp
virtual inline void onTimeout()
```

---

#### _request 

```cpp
PacketT _request
```

---

#### _response 

```cpp
PacketT _response
```

---

#### _timer 

```cpp
Timer _timer
```

The request timeout callback.

---

#### _retries 

```cpp
int _retries
```

The maximum number of attempts before the transaction is considered failed.

---

#### _attempts 

```cpp
int _attempts
```

The number of times the transaction has been sent.

---

#### _destroyed 

```cpp
bool _destroyed
```

## Pipe 

> **Extends:** `icy::Stream< uv_pipe_t >`
> **Defined in:** `pipe.h`

[Pipe](#classicy_1_1Pipe) implementation for process stdio.

### Members

| Name | Description |
|------|-------------|
| [`Pipe`](#group__base_1ga0bda328195d87f2aafedad6a5edb5891) |  |
| [`~Pipe`](#group__base_1gab94d527ad3579a1e7c7681ea8f8292aa) |  |
| [`init`](#group__base_1ga92c41af5c2e2cc2c7bf99235efdb743f) |  |
| [`readStart`](#group__base_1ga12f86b98a2d7ffc42ae666e8e5a8f8b9) |  |

---

#### Pipe 

```cpp
Pipe(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~Pipe 

```cpp
virtual ~Pipe()
```

---

#### init 

```cpp
virtual void init(bool ipc)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ipc` | `bool` |  |

---

#### readStart 

```cpp
virtual bool readStart()
```

## Process 

> **Defined in:** `process.h`

### Members

| Name | Description |
|------|-------------|
| [`Process`](#group__base_1ga21555aabd6288aeb0e2b7ef13dbc40ce) | Default constructor. |
| [`Process`](#group__base_1ga826344e39c8f9b525f590bb68e5923f8) | Constructor with command line arguments. |
| [`~Process`](#group__base_1ga2a922cfd98eae176afd280f9c645588d) | Destructor. |
| [`spawn`](#group__base_1ga2b8587f848160ac9f84743a27cc28151) | Spawns the process. Options must be properly set. Throws and exception on error. |
| [`kill`](#group__base_1gaebe1f79e3d47b3863573f8ad5a550d50) | Kills the process. |
| [`pid`](#group__base_1gab6b488740e07c049371ccdbac53ab50c) | Returns the process PID. |
| [`in`](#group__base_1ga704930d325b7403884a1e2e7e0748feb) | Returns the stdin pipe. |
| [`out`](#group__base_1gaf6f1b01781af6acfa1f31b904978bf5d) | Returns the stdout pipe. |
| [`init`](#group__base_1ga304d9596e4cdb66929e937691a15a225) |  |
| [`file`](#group__base_1ga3a1ca1c79dedddcf650a2a75d495cac8) | Path to the program to execute. Cenvenience proxy for options.file. Must be set before `[spawn()](#group__base_1ga2b8587f848160ac9f84743a27cc28151)` |
| [`cwd`](#group__base_1gac8c553f362862e81791ad95d81811834) | Set the current working directory. Cenvenience proxy for options.cwd. Must be set before `[spawn()](#group__base_1ga2b8587f848160ac9f84743a27cc28151)` |
| [`args`](#group__base_1gab717c10f12861cb06e94570f6e3cc955) | Command line agruments to pass to the process. Cenvenience proxy for options.args. Must be set before `[spawn()](#group__base_1ga2b8587f848160ac9f84743a27cc28151)` |
| [`onstdout`](#group__base_1ga31ab2cc3c0bd7ac0904536df48a7d579) | Stdout signal. Signals when a line has been output from the process. |
| [`onexit`](#group__base_1gacc4d49ab33dc77b331248ada30fd576e) | Exit stgnals. Signals process exit status code. |
| [`options`](#group__base_1ga8fa7a36e401ff6b282c6a65cad8461ef) | LibUV C options. Available for advanced use cases. |
| [`_handle`](#group__base_1ga46a6bb5aa18927d34008eaf77e83c335) |  |
| [`_stdin`](#group__base_1ga8d3b4922f4c4afc6fa1a4df3b4b7354c) |  |
| [`_stdout`](#group__base_1gaa986e17a53f2201fa837a0b678543c31) |  |
| [`_stdio`](#group__base_1ga4230cffe8750a9e74e8314fd53ef2658) |  |
| [`_cargs`](#group__base_1ga34b82a517a54bfcf781679804c7db705) |  |

---

#### Process 

```cpp
Process(uv::Loop * loop)
```

Default constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### Process 

```cpp
Process(std::initializer_list< std::string > args, uv::Loop * loop)
```

Constructor with command line arguments.

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `std::initializer_list< std::string >` |  |
| `loop` | `uv::Loop *` |  |

---

#### ~Process 

```cpp
~Process()
```

Destructor.

---

#### spawn 

```cpp
void spawn()
```

Spawns the process. Options must be properly set. Throws and exception on error.

---

#### kill 

```cpp
bool kill(int signum)
```

Kills the process.

| Parameter | Type | Description |
|-----------|------|-------------|
| `signum` | `int` |  |

---

#### pid 

```cpp
int pid() const
```

Returns the process PID.

---

#### in 

```cpp
Pipe & in()
```

Returns the stdin pipe.

---

#### out 

```cpp
Pipe & out()
```

Returns the stdout pipe.

---

#### init 

```cpp
void init()
```

---

#### file 

```cpp
std::string file
```

Path to the program to execute. Cenvenience proxy for options.file. Must be set before `[spawn()](#group__base_1ga2b8587f848160ac9f84743a27cc28151)`

---

#### cwd 

```cpp
std::string cwd
```

Set the current working directory. Cenvenience proxy for options.cwd. Must be set before `[spawn()](#group__base_1ga2b8587f848160ac9f84743a27cc28151)`

---

#### args 

```cpp
std::vector< std::string > args
```

Command line agruments to pass to the process. Cenvenience proxy for options.args. Must be set before `[spawn()](#group__base_1ga2b8587f848160ac9f84743a27cc28151)`

---

#### onstdout 

```cpp
std::function< void(std::string)> onstdout
```

Stdout signal. Signals when a line has been output from the process.

---

#### onexit 

```cpp
std::function< void(std::int64_t)> onexit
```

Exit stgnals. Signals process exit status code.

---

#### options 

```cpp
ProcessOptions options
```

LibUV C options. Available for advanced use cases.

---

#### _handle 

```cpp
uv::Handle< uv_process_t > _handle
```

---

#### _stdin 

```cpp
Pipe _stdin
```

---

#### _stdout 

```cpp
Pipe _stdout
```

---

#### _stdio 

```cpp
uv_stdio_container_t _stdio
```

---

#### _cargs 

```cpp
std::vector< char * > _cargs
```

## Queue 

> **Subclasses:** `icy::RunnableQueue< IPacket >`, `icy::RunnableQueue< PacketT >`
> **Defined in:** `queue.h`

Thread-safe queue container.

### Members

| Name | Description |
|------|-------------|
| [`push`](#group__base_1ga80ab35a22c083c80a6a7c717c2174c6a) |  |
| [`empty`](#group__base_1ga1bad28bab5f78677918cc73ceff03312) |  |
| [`front`](#group__base_1ga5d8c71ff5fcddff9a376aa8d3b74a1d1) |  |
| [`front`](#group__base_1gad7727ff38467b28de1f68dc32187bbac) |  |
| [`back`](#group__base_1ga9b4d0f0de452cb1f729f492f292635d8) |  |
| [`back`](#group__base_1gae59ccc30d8974fa4229ce743255c9470) |  |
| [`pop`](#group__base_1gaca35447c62090eb344640f15d75fd0b3) |  |
| [`sort`](#group__base_1ga3e9d6104c9eed0124e2a38da6de52fbc) |  |
| [`size`](#group__base_1ga246b30f749f103846c41b4a80e06b646) |  |
| [`queue`](#group__base_1ga6f087595e41336ffb5353d7ab03c4954) |  |
| [`_queue`](#group__base_1gac8bf28a8573f46c65292f5dfedfad510) |  |
| [`_mutex`](#group__base_1gaa1d5ef5979b813608de21b6c4e88d452) |  |

---

#### push 

```cpp
inline void push(const T & data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const T &` |  |

---

#### empty 

```cpp
inline bool empty() const
```

---

#### front 

```cpp
inline T & front()
```

---

#### front 

```cpp
inline T const & front() const
```

---

#### back 

```cpp
inline T & back()
```

---

#### back 

```cpp
inline T const & back() const
```

---

#### pop 

```cpp
inline void pop()
```

---

#### sort 

```cpp
template<typename Compare> inline void sort()
```

---

#### size 

```cpp
inline size_t size()
```

---

#### queue 

```cpp
inline std::deque< T > & queue()
```

---

#### _queue 

```cpp
std::deque< T > _queue
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## RunnableQueue 

> **Extends:** `icy::Queue< T * >`, `icy::basic::Runnable`
> **Subclasses:** `icy::AsyncQueue< IPacket >`, `icy::AsyncQueue< PacketT >`, `icy::SyncQueue< IPacket >`, `icy::AsyncQueue< T >`, `icy::SyncQueue< T >`
> **Defined in:** `queue.h`

### Members

| Name | Description |
|------|-------------|
| [`RunnableQueue`](#group__base_1ga04ba6cebdaf72214752e3cb1ce867461) |  |
| [`~RunnableQueue`](#group__base_1ga52f804f6571e308e1dcba1c948969c23) |  |
| [`push`](#group__base_1gac870c5b8456c3509e144517d055757dd) | Push an item onto the queue. The queue takes ownership of the item pointer. |
| [`flush`](#group__base_1gaacc8fe709fdfef37c09b0a4b4223435e) | Flush all outgoing items. |
| [`clear`](#group__base_1ga5a36945ff1d15379b4ae6de4046fb05c) |  |
| [`run`](#group__base_1ga372c52aa2d9b4c33f9b620dfb2c2af23) | Called asynchronously to dispatch queued items. If not timeout is set this method blocks until [cancel()](#group__base_1ga0443ed87de7952461d0948c88c8b003f) is called, otherwise [runTimeout()](#group__base_1ga834803a488030d079458bc67563ce927) will be called. Pseudo protected for std::bind compatability. |
| [`runTimeout`](#group__base_1ga834803a488030d079458bc67563ce927) | Called asynchronously to dispatch queued items until the queue is empty or the timeout expires. Pseudo protected for std::bind compatability. |
| [`dispatch`](#group__base_1gac8b4857c7505c81466387b3dc29c39e0) | Dispatch a single item to listeners. |
| [`timeout`](#group__base_1ga77f03651312f779c34fe066e1625f7b5) |  |
| [`setTimeout`](#group__base_1gad0dfa024d9a58c4d98191c2fa52b8778) |  |
| [`RunnableQueue`](#group__base_1ga002019b63e3d4e0c2f978ab5fa27682d) |  |
| [`operator=`](#group__base_1ga00552f5279e435b0beb97e8d98c70898) |  |
| [`popNext`](#group__base_1ga49f0c25f9709b4be2b934265244cade9) | Pops the next waiting item. |
| [`dispatchNext`](#group__base_1ga94ba51c70d311ed6ac2e8310de616e85) | Pops and dispatches the next waiting item. |
| [`ondispatch`](#group__base_1gaffe98a4f91bf2bc8131b59cb94006c9b) | The default dispatch function. Must be set before the queue is running. |
| [`_limit`](#group__base_1gac4231be4ccc33b05adef9052c6b70938) |  |
| [`_timeout`](#group__base_1ga21cf39b5892920945baa2b2fff5c14e4) |  |
| [`_mutex`](#group__base_1ga81f6b512f77af4e07e110c45db23f0b8) |  |

---

#### RunnableQueue 

```cpp
inline RunnableQueue(int limit, int timeout)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `limit` | `int` |  |
| `timeout` | `int` |  |

---

#### ~RunnableQueue 

```cpp
virtual inline ~RunnableQueue()
```

---

#### push 

```cpp
virtual inline void push(T * item)
```

Push an item onto the queue. The queue takes ownership of the item pointer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `T *` |  |

---

#### flush 

```cpp
virtual inline void flush()
```

Flush all outgoing items.

---

#### clear 

```cpp
inline void clear()
```

---

#### run 

```cpp
virtual inline void run()
```

Called asynchronously to dispatch queued items. If not timeout is set this method blocks until [cancel()](#group__base_1ga0443ed87de7952461d0948c88c8b003f) is called, otherwise [runTimeout()](#group__base_1ga834803a488030d079458bc67563ce927) will be called. Pseudo protected for std::bind compatability.

---

#### runTimeout 

```cpp
virtual inline void runTimeout()
```

Called asynchronously to dispatch queued items until the queue is empty or the timeout expires. Pseudo protected for std::bind compatability.

---

#### dispatch 

```cpp
virtual inline void dispatch(T & item)
```

Dispatch a single item to listeners.

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `T &` |  |

---

#### timeout 

```cpp
inline int timeout()
```

---

#### setTimeout 

```cpp
inline void setTimeout(int milliseconds)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `milliseconds` | `int` |  |

---

#### RunnableQueue 

```cpp
RunnableQueue(const RunnableQueue &) = delete
```

---

#### operator= 

```cpp
RunnableQueue & operator=(const RunnableQueue &) = delete
```

---

#### popNext 

```cpp
virtual inline T * popNext()
```

Pops the next waiting item.

---

#### dispatchNext 

```cpp
virtual inline bool dispatchNext()
```

Pops and dispatches the next waiting item.

---

#### ondispatch 

```cpp
std::function< void(T &)> ondispatch
```

The default dispatch function. Must be set before the queue is running.

---

#### _limit 

```cpp
int _limit
```

---

#### _timeout 

```cpp
int _timeout
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## SyncQueue 

> **Extends:** `icy::RunnableQueue< T >`
> **Subclasses:** `icy::SyncPacketQueue< T >`
> **Defined in:** `queue.h`

[SyncQueue](#classicy_1_1SyncQueue) extends [Synchronizer](#classicy_1_1Synchronizer) to implement a synchronized FIFO queue which receives T objects from any thread and synchronizes them for safe consumption by the associated event loop.

### Members

| Name | Description |
|------|-------------|
| [`SyncQueue`](#group__base_1gae1cf6a31580baa5e4455d193ba3c0b54) |  |
| [`~SyncQueue`](#group__base_1ga9539ee4bbac1c32eb8c315a15ee0c9b3) | Destruction is deferred to allow enough time for all callbacks to return. |
| [`push`](#group__base_1ga807c4a3d7d9adf4c15fd9b78121ca830) | Pushes an item onto the queue. Item pointers are now managed by the [SyncQueue](#classicy_1_1SyncQueue). |
| [`cancel`](#group__base_1gafd7fe7eff78132084172f272e76c37b7) |  |
| [`sync`](#group__base_1ga1e3c839536b14db6be1846e40cd16fd4) |  |
| [`_sync`](#group__base_1gacf0e3e74f572918437858efc1f769a26) |  |

---

#### SyncQueue 

```cpp
inline SyncQueue(uv::Loop * loop, int limit, int timeout)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |
| `limit` | `int` |  |
| `timeout` | `int` |  |

---

#### ~SyncQueue 

```cpp
virtual inline ~SyncQueue()
```

Destruction is deferred to allow enough time for all callbacks to return.

---

#### push 

```cpp
virtual inline void push(T * item)
```

Pushes an item onto the queue. Item pointers are now managed by the [SyncQueue](#classicy_1_1SyncQueue).

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `T *` |  |

---

#### cancel 

```cpp
virtual inline void cancel()
```

---

#### sync 

```cpp
inline Synchronizer & sync()
```

---

#### _sync 

```cpp
Synchronizer _sync
```

## AsyncQueue 

> **Extends:** `icy::RunnableQueue< T >`
> **Subclasses:** `icy::AsyncPacketQueue< PacketT >`, `icy::AsyncPacketQueue< T >`
> **Defined in:** `queue.h`

[AsyncQueue](#classicy_1_1AsyncQueue) is a thread-based queue which receives packets from any thread source and dispatches them asynchronously.

This queue is useful for deferring load from operation critical system devices before performing long running tasks.

The thread will call the [RunnableQueue](#classicy_1_1RunnableQueue)'s [run()](#group__base_1ga372c52aa2d9b4c33f9b620dfb2c2af23) method to constantly flush outgoing packets until [cancel()](#group__base_1ga0096428c7d684eb39f378784ee868fa1) is called.

### Members

| Name | Description |
|------|-------------|
| [`AsyncQueue`](#group__base_1ga1e59a5d7850526b3bb9226e707552a87) |  |
| [`cancel`](#group__base_1ga0096428c7d684eb39f378784ee868fa1) |  |
| [`~AsyncQueue`](#group__base_1gad02ee332589a016e4864aba8e75b3194) |  |
| [`_thread`](#group__base_1ga43d70d35f82a4c1286108cc14b6b8066) |  |

---

#### AsyncQueue 

```cpp
inline AsyncQueue(int limit)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `limit` | `int` |  |

---

#### cancel 

```cpp
virtual inline void cancel()
```

---

#### ~AsyncQueue 

```cpp
virtual inline ~AsyncQueue()
```

---

#### _thread 

```cpp
Thread _thread
```

## Random 

> **Defined in:** `random.h`

[Random](#classicy_1_1Random) implements a pseudo random number generator (PRNG). The PRNG is a nonlinear additive feedback random number generator using 256 bytes of state information and a period of up to 2^69.

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__base_1gafda9e80241e992379938503a8d5bb9ae) |  |
| [``](#group__base_1ga87e817337573c060b8dd31a319069ef1) |  |
| [`Random`](#group__base_1ga658f3a6766a2f0207f378aef3e6a5f79) | Creates and initializes the PRNG. Specify either a state buffer size (8 to 256 bytes) or one of the Type values. |
| [`~Random`](#group__base_1gae5781de4f82f83eb1d90b6a44a3d6a8f) | Destroys the PRNG. |
| [`seed`](#group__base_1ga7ed905c8f70572e16fc55f5afaaccf98) | Seeds the pseudo random generator with the given seed. |
| [`seed`](#group__base_1ga088b5bbefdc56704ad059271355d0087) | Seeds the pseudo random generator with a random seed obtained from a RandomInputStream. |
| [`next`](#group__base_1ga7251a36332b0ed1e2dcc0cde2d484f29) | Returns the next 31-bit pseudo random number. |
| [`next`](#group__base_1ga326e0f261aa051c98cf495f0b1f52da7) | Returns the next 31-bit pseudo random number modulo n. |
| [`nextChar`](#group__base_1ga539b6e8002927588265e941ff166d6ae) | Returns the next pseudo random character. |
| [`nextBool`](#group__base_1gadc7c49f9a915ca4c8af9527bceefbffb) | Returns the next boolean pseudo random value. |
| [`nextFloat`](#group__base_1gac6ac62ca9dfb0e6566c3b8078a661032) | Returns the next float pseudo random number between 0.0 and 1.0. |
| [`nextDouble`](#group__base_1gadba7f18dddf46b7b34deeddf57fa5f74) | Returns the next double pseudo random number between 0.0 and 1.0. |
| [`getSeed`](#group__base_1ga1c11a090bf29cb6f69e8c389d1eee114) | Generates a random seed using native OS functions. |
| [`initState`](#group__base_1ga4eb708ad8fbe9515b56501dcaa1c7940) |  |
| [`goodRand`](#group__base_1ga087d152820fd220b26bfcfe6d3863132) |  |
| [`_fptr`](#group__base_1ga4934f0731699b432415c424c1925d957) |  |
| [`_rptr`](#group__base_1ga962a0cedac6a414b67edb313e289c073) |  |
| [`_state`](#group__base_1ga4b2bad7a096c8871073fa372491ca88c) |  |
| [`_randType`](#group__base_1ga0308d48427d49885271c4e0b98e9ba63) |  |
| [`_randDeg`](#group__base_1ga09d0b983160aa081c30c26d140ccf0d7) |  |
| [`_randSep`](#group__base_1gadad904ab8da1d3dbdc91721b2c593825) |  |
| [`_endPtr`](#group__base_1gaae39b7248a27747ba740f49eca8e6821) |  |
| [`_buffer`](#group__base_1ga76382c414bf0465627c388c287c4873a) |  |

---

#### Type 

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `RND_STATE_0` |  |
| `RND_STATE_32` | linear congruential |
| `RND_STATE_64` | x**7 + x**3 + 1 |
| `RND_STATE_128` | x**15 + x + 1 |
| `RND_STATE_256` | x**31 + x**3 + 1 |

---

####  

```cpp
enum 
```

| Value | Description |
|-------|-------------|
| `MAX_TYPES` |  |
| `NSHUFF` |  |

---

#### Random 

```cpp
Random(int stateSize)
```

Creates and initializes the PRNG. Specify either a state buffer size (8 to 256 bytes) or one of the Type values.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stateSize` | `int` |  |

---

#### ~Random 

```cpp
~Random()
```

Destroys the PRNG.

---

#### seed 

```cpp
void seed(uint32_t seed)
```

Seeds the pseudo random generator with the given seed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `seed` | `uint32_t` |  |

---

#### seed 

```cpp
void seed()
```

Seeds the pseudo random generator with a random seed obtained from a RandomInputStream.

---

#### next 

```cpp
uint32_t next()
```

Returns the next 31-bit pseudo random number.

---

#### next 

```cpp
uint32_t next(uint32_t n)
```

Returns the next 31-bit pseudo random number modulo n.

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `uint32_t` |  |

---

#### nextChar 

```cpp
char nextChar()
```

Returns the next pseudo random character.

---

#### nextBool 

```cpp
bool nextBool()
```

Returns the next boolean pseudo random value.

---

#### nextFloat 

```cpp
float nextFloat()
```

Returns the next float pseudo random number between 0.0 and 1.0.

---

#### nextDouble 

```cpp
double nextDouble()
```

Returns the next double pseudo random number between 0.0 and 1.0.

---

#### getSeed 

```cpp
static void getSeed(char * seed, unsigned length)
```

Generates a random seed using native OS functions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `seed` | `char *` |  |
| `length` | `unsigned` |  |

---

#### initState 

```cpp
void initState(uint32_t seed, char * arg_state, std::int32_t n)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `seed` | `uint32_t` |  |
| `arg_state` | `char *` |  |
| `n` | `std::int32_t` |  |

---

#### goodRand 

```cpp
static uint32_t goodRand(std::int32_t x)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `std::int32_t` |  |

---

#### _fptr 

```cpp
uint32_t * _fptr
```

---

#### _rptr 

```cpp
uint32_t * _rptr
```

---

#### _state 

```cpp
uint32_t * _state
```

---

#### _randType 

```cpp
int _randType
```

---

#### _randDeg 

```cpp
int _randDeg
```

---

#### _randSep 

```cpp
int _randSep
```

---

#### _endPtr 

```cpp
uint32_t * _endPtr
```

---

#### _buffer 

```cpp
std::unique_ptr< char[]> _buffer
```

## Runner 

> **Subclasses:** `icy::Idler`, `icy::Synchronizer`, `icy::Thread`, `icy::Thread`, `icy::Timer`
> **Defined in:** `runner.h`

[Runner](#classicy_1_1Runner) is a virtual interface for implementing asynchronous objects such as threads and futures.

### Members

| Name | Description |
|------|-------------|
| [`Runner`](#group__base_1ga4c9c5568422630b80d7bcec186d7f9ae) |  |
| [`~Runner`](#group__base_1gabfc107d7db7140257b4dfa2a4e2cd647) |  |
| [`start`](#group__base_1gabe54278718ed31b12d3f97e2e57c56ed) | Start the asynchronous context with the given invokeback. |
| [`running`](#group__base_1ga82d2c9fecae021a80e3568fc440ba446) | Returns true if the async context is currently running. |
| [`cancel`](#group__base_1ga679d52263009759d6c7fd77c53972a0a) | Cancels the async context. |
| [`cancelled`](#group__base_1ga5374eaf1369a6623367db28f9472e956) | True when the task has been cancelled. It is up to the implementation to return at the earliest possible time. |
| [`repeating`](#group__base_1gaebdf1e6504e226fc910cd839b4bc1e51) | Returns true if the [Runner](#classicy_1_1Runner) is operating in repeating mode. |
| [`setRepeating`](#group__base_1ga12c1000762cde5b41c2412ba9172d436) | This setting means the implementation should invoke the target function repeatedly until cancelled. The importance of this method to normalize the functionality of threadded and event loop driven [Runner](#classicy_1_1Runner) models. |
| [`async`](#group__base_1ga61903ea9f01179265132953f56ca8a3b) | Returns true if the implementation is thread-based, or false if it belongs to an event loop. |
| [`tid`](#group__base_1ga8c0368bae16c191ce1f4effd1003fccd) | Return the native thread ID. |
| [`waitForExit`](#group__base_1ga871cd630b94c2bafee226866b52b0c9c) | Wait until the thread exits. |
| [`Runner`](#group__base_1ga6f3ded568733fb97244b4f51ffd0ea2c) | NonCopyable and NonMovable. |
| [`operator=`](#group__base_1gaf76d61099580638ce024f452decf76f4) |  |
| [`_context`](#group__base_1ga2dea91abc5566c9a613c6cdf253f6aa0) | Shared pointer to the internal [Context](#structicy_1_1Runner_1_1Context). |

---

#### Runner 

```cpp
Runner()
```

---

#### ~Runner 

```cpp
virtual ~Runner()
```

---

#### start 

```cpp
void start(std::function< void()> target)
```

Start the asynchronous context with the given invokeback.

The target `Runnable` instance must outlive the `[Runner](#classicy_1_1Runner)`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `std::function< void()>` |  |

---

#### running 

```cpp
bool running() const
```

Returns true if the async context is currently running.

---

#### cancel 

```cpp
void cancel()
```

Cancels the async context.

---

#### cancelled 

```cpp
bool cancelled() const
```

True when the task has been cancelled. It is up to the implementation to return at the earliest possible time.

---

#### repeating 

```cpp
bool repeating() const
```

Returns true if the [Runner](#classicy_1_1Runner) is operating in repeating mode.

---

#### setRepeating 

```cpp
void setRepeating(bool flag)
```

This setting means the implementation should invoke the target function repeatedly until cancelled. The importance of this method to normalize the functionality of threadded and event loop driven [Runner](#classicy_1_1Runner) models.

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### async 

```cpp
bool async() const
```

Returns true if the implementation is thread-based, or false if it belongs to an event loop.

---

#### tid 

```cpp
std::thread::id tid() const
```

Return the native thread ID.

---

#### waitForExit 

```cpp
bool waitForExit(int timeout)
```

Wait until the thread exits.

The thread should be cancelled beore invokeing this method. This method must be invokeed from outside the current thread context or deadlock will ensue.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `int` |  |

---

#### Runner 

```cpp
Runner(const Runner &) = delete
```

NonCopyable and NonMovable.

---

#### operator= 

```cpp
Runner & operator=(const Runner &) = delete
```

---

#### _context 

```cpp
std::shared_ptr< Context > _context
```

Shared pointer to the internal [Context](#structicy_1_1Runner_1_1Context).

## Context 

> **Defined in:** `runner.h`

[Context](#structicy_1_1Runner_1_1Context) object which we send to the thread context.

This intermediate object allows us to garecefully handle late invokebacks and so avoid the need for deferred destruction of `[Runner](#classicy_1_1Runner)` objects.

### Members

| Name | Description |
|------|-------------|
| [`reset`](#group__base_1ga85d58cf01bb21e0f0280ee99f5053dfb) |  |
| [`Context`](#group__base_1ga8b65b067b317daa79442b0235f22df77) |  |
| [`tid`](#group__base_1gaa43dd023235e42c86f9aa2b5d2d3211b) |  |
| [`running`](#group__base_1ga26e1db2bf00f40be908b6facce006e4f) |  |
| [`cancelled`](#group__base_1ga66278b215dabb449abe3f1f5a5afd8cb) |  |
| [`repeating`](#group__base_1ga2a3f06bbc8956c474960af89e3d6a0b1) |  |

---

#### reset 

```cpp
inline void reset()
```

---

#### Context 

```cpp
inline Context()
```

---

#### tid 

```cpp
std::thread::id tid
```

---

#### running 

```cpp
std::atomic< bool > running
```

---

#### cancelled 

```cpp
std::atomic< bool > cancelled
```

---

#### repeating 

```cpp
bool repeating = false
```

## StopPropagation 

> **Extends:** `std::exception`
> **Defined in:** `signal.h`

Exception to break out of the current [Signal](#classicy_1_1Signal) callback scope.

### Members

| Name | Description |
|------|-------------|
| [`~StopPropagation`](#group__base_1ga62918948c39bc8a834fda3faaa498d7b) |  |

---

#### ~StopPropagation 

```cpp
virtual ~StopPropagation() = default
```

## Signal 

[Signal](#classicy_1_1Signal) and slots implementation.

To create a signal, declare member variables of type `[Signal](#classicy_1_1Signal)<...>` in your class. The template parameter is the argument types that will be passed to the callback functions.

Here's a simple example with a class `MyClass` that has a single signal `my_signal` which takes a single `int` argument: class MyClass
{
public:
    Signal<int> my_signal;
    ...
};
 To connect to a signal, call its `attach()` member function, passing a function (in the `std::function` sense) as the argument. The function can be a function pointer, a functor object, or an anonymous lambda function.

Here's an example connecting to the above signal to a lambda function: MyClass my_class;
my_class.my_signal.attach([](int x) {
    cout << "value: " << x << endl;
});
 The `attach()` function returns a unique ID of type `int` which can be used later to disconnect the callback function.

Here's an example using the above code to then immediately disconnect the signal connection with the `detach()` member function: MyClass my_class;
auto id = my_class.my_signal.attach([](int x) {
    cout << "value: " << x << endl;
});
my_class.my_signal.detach(id);
 The `detach()` function is passed the callback ID and will return `true` if a callback was disconnected or `false` if the ID wasn't found. Note that `detach()` can be safely called from within the callback scope.

In the case of class members there is a `[slot()](#group__base_1gac8a25d21946945c9244ec49f540177c1)` helper that can be used to bind the signal like so: class TargetClass
{
public:
    Signal<int> my_signal;

    int print(int x)
    {
        cout << "say: " << x << endl;
    }
};

MyClass my_class;

TargetClass target_class;
my_class.my_signal += slot(&target_class, &TargetClass::print)
 The `[slot()](#group__base_1gac8a25d21946945c9244ec49f540177c1)` helper can also be used to disconnect class member callbacks like so: my_class.my_signal -= slot(&target_class, &TargetClass::print)
 To emit the signal, call its `emit()` member function passing arguments matching the types of those in the signal variable declaration.

Using the above example code, here's an example showing how to emit `my_signal`: my_class.my_signal.emit(42);
 Since the signal was declared to take a single `int` argument, this will cause any callbacks connected to the signal to be called, passing the integer `42` as the only argument.

## Signal< RT(Args...)> 

> **Defined in:** `signal.h`

### Members

| Name | Description |
|------|-------------|
| [`attach`](#group__base_1gac2a3756f75e8e9a1bccb3303ec491dff) | Connects a `lambda` or `std::function` to the `[Signal](#classicy_1_1Signal)`. The returned value can be used to detach the slot. |
| [`attach`](#group__base_1ga77af5fd67dc1c8929ce0041fa1b573d0) | Connects a `SlotPtr` instance to the `[Signal](#classicy_1_1Signal)`. The returned value can be used to detach the slot. |
| [`detach`](#group__base_1gaea38b0b03fb3e5a21d5d9514964e7608) | Detaches a previously attached slot. |
| [`detach`](#group__base_1ga80430b9e160632b7227a1eabfca56213) | Detaches all slots for the given instance. |
| [`detach`](#group__base_1ga2450e3597bcba5e05d70d99d4e8da569) | Detaches all attached functions for the given instance. |
| [`detachAll`](#group__base_1ga587e054f0a2a0c5c478d418490d53972) | Detaches all previously attached functions. |
| [`emit`](#group__base_1ga16ee016345c12463e640b889a618da48) | Emits the signal to all attached functions. |
| [`slots`](#group__base_1gaff2ff00aa6f20acf8a2fbeef57948f5f) | Returns the managed slot list. |
| [`nslots`](#group__base_1ga662662b9450d80b7699bbe2f7c4ae83c) | Returns the number of active slots. |
| [`operator+=`](#group__base_1ga7711cffea5254d8c5ef372f0343e40af) | Convenience operators. |
| [`operator+=`](#group__base_1ga4eb920126905b5f6d5f2bb7fcf8c4477) |  |
| [`operator-=`](#group__base_1gab4fd251624562e056ef80c3d15dd5688) |  |
| [`operator-=`](#group__base_1ga710960eba97189f8a7981ede5d8b96a2) |  |
| [`operator-=`](#group__base_1ga1379909a60fe721c20b601b9f1cd5f62) |  |
| [`Signal`](#group__base_1ga5892a3865b373642deff22ebb05b105d) | Default constructor. |
| [`Signal`](#group__base_1gaedc57c0c91f9383f924b9e77239572d6) | Copy constructor. |
| [`operator=`](#group__base_1ga2d05970fba425e5b71a179a7fa7b21c5) | Assignment operator. |
| [`_mutex`](#group__base_1ga8990e916c761cfdd551c9408b8f76100) |  |
| [`_slots`](#group__base_1ga73b22c1ee61122fc06510242c04a253f) |  |
| [`_lastId`](#group__base_1ga965406210a208b6eb6fa4fe9ad2fb44b) |  |

---

#### attach 

```cpp
inline int attach(Function const & func, void * instance, int id, int priority) const
```

Connects a `lambda` or `std::function` to the `[Signal](#classicy_1_1Signal)`. The returned value can be used to detach the slot.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function const &` |  |
| `instance` | `void *` |  |
| `id` | `int` |  |
| `priority` | `int` |  |

---

#### attach 

```cpp
inline int attach(SlotPtr slot) const
```

Connects a `SlotPtr` instance to the `[Signal](#classicy_1_1Signal)`. The returned value can be used to detach the slot.

| Parameter | Type | Description |
|-----------|------|-------------|
| `slot` | `SlotPtr` |  |

---

#### detach 

```cpp
inline bool detach(int id) const
```

Detaches a previously attached slot.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `int` |  |

---

#### detach 

```cpp
inline bool detach(const void * instance) const
```

Detaches all slots for the given instance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `instance` | `const void *` |  |

---

#### detach 

```cpp
inline bool detach(SlotPtr other) const
```

Detaches all attached functions for the given instance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `SlotPtr` |  |

---

#### detachAll 

```cpp
inline void detachAll() const
```

Detaches all previously attached functions.

---

#### emit 

```cpp
virtual inline void emit(Args... args)
```

Emits the signal to all attached functions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` |  |

---

#### slots 

```cpp
inline std::vector< SlotPtr > slots() const
```

Returns the managed slot list.

---

#### nslots 

```cpp
inline size_t nslots() const
```

Returns the number of active slots.

---

#### operator+= 

```cpp
inline int operator+=(Function const & func)
```

Convenience operators.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function const &` |  |

---

#### operator+= 

```cpp
inline int operator+=(SlotPtr slot)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `slot` | `SlotPtr` |  |

---

#### operator-= 

```cpp
inline bool operator-=(int id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `int` |  |

---

#### operator-= 

```cpp
inline bool operator-=(const void * instance)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `instance` | `const void *` |  |

---

#### operator-= 

```cpp
inline bool operator-=(SlotPtr slot)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `slot` | `SlotPtr` |  |

---

#### Signal 

```cpp
inline Signal()
```

Default constructor.

---

#### Signal 

```cpp
inline Signal(const Signal & r)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Signal &` |  |

---

#### operator= 

```cpp
inline Signal & operator=(const Signal & r)
```

Assignment operator.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Signal &` |  |

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _slots 

```cpp
std::vector< SlotPtr > _slots
```

---

#### _lastId 

```cpp
int _lastId = 0
```

## Singleton 

> **Defined in:** `singleton.h`

Helper template class for managing singleton objects allocated on the heap.

### Members

| Name | Description |
|------|-------------|
| [`Singleton`](#group__base_1ga4041a0d028f806012cfe48040f903e76) |  |
| [`~Singleton`](#group__base_1gac1e0a1036a9f0eb917e31a4a43a33f8b) |  |
| [`get`](#group__base_1ga116fecf0951357c605749cd1ecfcbdf0) | Returns a pointer to the singleton object held by the [Singleton](#classicy_1_1Singleton). The first call to get will instantiate the singleton. |
| [`swap`](#group__base_1ga68ed207f0d38a3a8ba424977b06af3c4) | Swaps the old pointer with the new one and returns the old instance. |
| [`destroy`](#group__base_1ga30845630ba9bebeb153d5519f46e9aef) | Destroys the managed singleton instance. |
| [`_ptr`](#group__base_1ga5de7a3516a5f2f911e6b7e5b29448a65) |  |
| [`_m`](#group__base_1gad94e69e7905e096a586d3b5118ce4e8d) |  |

---

#### Singleton 

```cpp
Singleton() = default
```

---

#### ~Singleton 

```cpp
~Singleton() = default
```

---

#### get 

```cpp
inline S * get()
```

Returns a pointer to the singleton object held by the [Singleton](#classicy_1_1Singleton). The first call to get will instantiate the singleton.

---

#### swap 

```cpp
inline S * swap(S * newPtr)
```

Swaps the old pointer with the new one and returns the old instance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `newPtr` | `S *` |  |

---

#### destroy 

```cpp
inline void destroy()
```

Destroys the managed singleton instance.

---

#### _ptr 

```cpp
std::unique_ptr< S, Deleter > _ptr
```

---

#### _m 

```cpp
std::mutex _m
```

## Deleter 

Custom deleter that can access private destructors via friendship.

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__base_1gafef1cfec1d364ba3d428ffb409aed1e8) |  |

---

#### operator() 

```cpp
inline void operator()(S * p) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `p` | `S *` |  |

## State 

> **Subclasses:** `icy::DiagnosticState`, `icy::PacketStreamState`, `icy::TransactionState`, `icy::av::EncoderState`, `icy::pacm::InstallationState`, `icy::sockio::ClientState`, `icy::turn::ClientState`
> **Defined in:** `stateful.h`

[State](#classicy_1_1State) class for state machines.

This class defined the state for a state machine, and should be extended and passed to implementations of the `[Stateful](#classicy_1_1Stateful)` template.

For an example **See also**: [PacketStreamState](#structicy_1_1PacketStreamState)

### Members

| Name | Description |
|------|-------------|
| [`State`](#group__base_1ga62afd904ca2b19a794832408b1952d40) |  |
| [`State`](#group__base_1ga7db01e0c12d75a6aa8b14e63ed81ebb6) |  |
| [`operator=`](#group__base_1ga62a59f3d8aa97fd4361f7f55b3fa445b) |  |
| [`~State`](#group__base_1gaa7be0fe7a375c27073cc5c9dc063269f) |  |
| [`id`](#group__base_1gad5b4dd779ec160d101eb3f780c24d04f) |  |
| [`set`](#group__base_1gac7b685fc2715f9cac21d14310fbd08c6) |  |
| [`str`](#group__base_1gaf08ad3e3c5e32fddda195ff37115278b) |  |
| [`toString`](#group__base_1ga328e6a3b702375b0ebcf56dafceadda0) |  |
| [`equals`](#group__base_1gabe8d32bfca4f473ca60b5d41f0d03bda) |  |
| [`between`](#group__base_1ga27f326095da9f4eca2584bda0e3179d4) |  |
| [`operator==`](#group__base_1gae2bc129ba4fa926d9b782863d8bf3813) |  |
| [`operator==`](#group__base_1ga0f17f91af6980a8c4446ff8a556b07fc) |  |
| [`_id`](#group__base_1ga7dc89cb74e42f44177e294a105f93f15) |  |

---

#### State 

```cpp
State(ID id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `ID` |  |

---

#### State 

```cpp
State(const State & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const State &` |  |

---

#### operator= 

```cpp
State & operator=(const State & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const State &` |  |

---

#### ~State 

```cpp
virtual ~State() = default
```

---

#### id 

```cpp
ID id() const
```

---

#### set 

```cpp
void set(ID id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `ID` |  |

---

#### str 

```cpp
virtual std::string str(ID id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `ID` |  |

---

#### toString 

```cpp
virtual std::string toString() const
```

---

#### equals 

```cpp
bool equals(ID id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `ID` |  |

---

#### between 

```cpp
bool between(ID lid, ID rid) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `lid` | `ID` |  |
| `rid` | `ID` |  |

---

#### operator== 

```cpp
inline bool operator==(const State & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const State &` |  |

---

#### operator== 

```cpp
inline bool operator==(const State::ID & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const State::ID &` |  |

---

#### _id 

```cpp
std::atomic< ID > _id
```

## Stateful 

> **Defined in:** `stateful.h`

[State](#classicy_1_1State) machine implementation.

For an example **See also**: [PacketStream](#classicy_1_1PacketStream)

#### Parameters
* `The` derived `[State](#classicy_1_1State)` type.

### Members

| Name | Description |
|------|-------------|
| [`Stateful`](#group__base_1ga17f56a2225eb139ebf730715844ae093) |  |
| [`~Stateful`](#group__base_1ga88e8a5a4d0be0a7c8b4bcf8283e8118a) |  |
| [`stateEquals`](#group__base_1ga46c34eb0102b150761777244d09a3105) |  |
| [`stateBetween`](#group__base_1gab9afafcff1b0f56dc4908ffefffa0951) |  |
| [`state`](#group__base_1ga25a3dd6324c6a4937e3be09a5db5c6e9) |  |
| [`state`](#group__base_1gaf74b1907ca291f0fe0288e7fed013a33) |  |
| [`beforeStateChange`](#group__base_1gaa8d284c63cb789cd53c36b06ff3bcc6f) | Override to handle pre state change logic. Return false to prevent state change. |
| [`onStateChange`](#group__base_1ga3c88b4aa8e9343f9bdc1a8c2ae2a5452) | Override to handle post state change logic. |
| [`setState`](#group__base_1ga391c2db9587462e05885b9bb4203825b) | Sets the state and sends the state signal if the state change was successful. |
| [`setState`](#group__base_1gab22136dc42f0b6352249274fd13d4ef0) | Sets the state and sends the state signal if the state change was successful. |
| [`StateChange`](#group__base_1ga822472b26b7dcdce4e5d1b2095983ca9) | Signals when the state changes. |
| [`_state`](#group__base_1gaa772242e3f30125cb5006f757f6d9021) |  |

---

#### Stateful 

```cpp
inline Stateful()
```

---

#### ~Stateful 

```cpp
virtual inline ~Stateful()
```

---

#### stateEquals 

```cpp
virtual inline bool stateEquals(typename T::ID id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `typename T::ID` |  |

---

#### stateBetween 

```cpp
virtual inline bool stateBetween(typename T::ID lid, typename T::ID rid) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `lid` | `typename T::ID` |  |
| `rid` | `typename T::ID` |  |

---

#### state 

```cpp
virtual inline T & state()
```

---

#### state 

```cpp
virtual inline const T state() const
```

---

#### beforeStateChange 

```cpp
virtual inline bool beforeStateChange(const T & state)
```

Override to handle pre state change logic. Return false to prevent state change.

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `const T &` |  |

---

#### onStateChange 

```cpp
virtual inline void onStateChange(T &, const T &)
```

Override to handle post state change logic.

---

#### setState 

```cpp
virtual inline bool setState(void * sender, typename T::ID id)
```

Sets the state and sends the state signal if the state change was successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `id` | `typename T::ID` |  |

---

#### setState 

```cpp
virtual inline bool setState(void * sender, const T & state)
```

Sets the state and sends the state signal if the state change was successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `state` | `const T &` |  |

---

#### StateChange 

```cpp
Signal< void(void *, T &, const T &)> StateChange
```

Signals when the state changes.

---

#### _state 

```cpp
T _state
```

## Stream 

> **Extends:** `icy::uv::Handle< T >`
> **Defined in:** `stream.h`

Basic stream type for sockets and pipes.

### Members

| Name | Description |
|------|-------------|
| [`Stream`](#group__base_1ga573ab3d7b0726931ef779f1f3316d729) |  |
| [`~Stream`](#group__base_1ga139967e399937a37bac0659df012050d) |  |
| [`close`](#group__base_1ga2daec2bd06cc428828b644b836833c6f) | Closes and resets the stream handle. This will close the active socket/pipe and destroy the handle. |
| [`shutdown`](#group__base_1ga8198a3529459f9628609369beed52be7) | Sends a shutdown packet to the connected peer. Return true if the shutdown packet was sent. |
| [`write`](#group__base_1gaca313829afc5741744191f1d72a081fe) | Writes data to the stream. |
| [`write`](#group__base_1ga8ed161ca9c9a5eb606af66db61d4462d) | Write data to the target stream. |
| [`stream`](#group__base_1ga4af2e803567d28d0da339fe09fd34a24) | Return the uv_stream_t pointer. |
| [`readStart`](#group__base_1ga0d44541ca0615c3f7905354f635a87f3) |  |
| [`readStop`](#group__base_1ga5ce90e446eb2588dbec48749c8ab47de) |  |
| [`onRead`](#group__base_1ga0dca15710b5b129077bf165933fc9fd5) |  |
| [`handleRead`](#group__base_1ga5a41a97f6002d8f5a2b5a455a748e754) | UV callbacks. |
| [`allocReadBuffer`](#group__base_1gae046b67a5834f8ba7a1c7356e7720e9d) |  |
| [`Read`](#group__base_1gaa4b7ae83d45685339e7ac7f8308e8555) | [Signal](#classicy_1_1Signal) the notifies when data is available for read. |
| [`_buffer`](#group__base_1gabc5b1ed474e8c4e04bedef0c05df5e13) |  |
| [`_started`](#group__base_1ga3d0d3973d5f49cd52a42642b46ff576e) |  |

---

#### Stream 

```cpp
inline Stream(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~Stream 

```cpp
virtual inline ~Stream()
```

---

#### close 

```cpp
virtual inline void close()
```

Closes and resets the stream handle. This will close the active socket/pipe and destroy the handle.

If the stream is already closed this call will have no side-effects.

---

#### shutdown 

```cpp
inline bool shutdown()
```

Sends a shutdown packet to the connected peer. Return true if the shutdown packet was sent.

---

#### write 

```cpp
inline bool write(const char * data, size_t len)
```

Writes data to the stream.

Return false if the underlying socket is closed. This method does not throw an exception.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### write 

```cpp
inline bool write(const char * data, size_t len, uv_stream_t * send)
```

Write data to the target stream.

This method is only valid for IPC streams.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |
| `send` | `uv_stream_t *` |  |

---

#### stream 

```cpp
inline uv_stream_t * stream()
```

Return the uv_stream_t pointer.

---

#### readStart 

```cpp
virtual inline bool readStart()
```

---

#### readStop 

```cpp
virtual inline bool readStop()
```

---

#### onRead 

```cpp
virtual inline void onRead(const char * data, size_t len)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `len` | `size_t` |  |

---

#### handleRead 

```cpp
static inline void handleRead(uv_stream_t * handle, ssize_t nread, const uv_buf_t * buf)
```

UV callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_stream_t *` |  |
| `nread` | `ssize_t` |  |
| `buf` | `const uv_buf_t *` |  |

---

#### allocReadBuffer 

```cpp
static inline void allocReadBuffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `handle` | `uv_handle_t *` |  |
| `suggested_size` | `size_t` |  |
| `buf` | `uv_buf_t *` |  |

---

#### Read 

```cpp
Signal< void(const char *, const int &)> Read
```

[Signal](#classicy_1_1Signal) the notifies when data is available for read.

---

#### _buffer 

```cpp
Buffer _buffer
```

---

#### _started 

```cpp
bool _started {false}
```

## Synchronizer 

> **Extends:** `icy::Runner`
> **Defined in:** `synchronizer.h`

[Synchronizer](#classicy_1_1Synchronizer) enables any thread to communicate with the associated event loop via synchronized callbacks.

This class inherits the `[Runner](#classicy_1_1Runner)` interface and may be used with any implementation that's powered by an asynchronous `[Runner](#classicy_1_1Runner)`.

### Members

| Name | Description |
|------|-------------|
| [`Synchronizer`](#group__base_1ga444b58b7cd15aa6177aab2550551e9d1) | Create the synchronization context the given event loop. |
| [`Synchronizer`](#group__base_1ga3b654ca58a890cc899b528e7792457d6) | Create the synchronization context the given event loop and method. The target method will be called from the event loop context. |
| [`Synchronizer`](#group__base_1ga83229baca61a3bb8d619049434d7ef63) | Create the synchronization context the given event loop and method. |
| [`~Synchronizer`](#group__base_1ga78f28955aaa877686204db051ff098a3) | Destructor. |
| [`post`](#group__base_1ga3ad0d439617049c785f89a03dd8064cf) | Send a synchronization request to the event loop. Call this each time you want the target method called synchronously. The synchronous method will be called on next iteration. This is not atomic, so do not expect a callback for every request. |
| [`start`](#group__base_1ga30af5bd71957b34eaf6127c2a6eae8ea) | Start the synchronizer with the given callback. |
| [`start`](#group__base_1gaebac12640f586042e138e25a24b25c72) | Start the synchronizer with the given callback function. |
| [`cancel`](#group__base_1ga9305feda357ecdbc54c55fc493cada4b) |  |
| [`close`](#group__base_1ga0580bcfe1a059a5a1b75b235065ba2b1) |  |
| [`handle`](#group__base_1gabd4d17feef12768ead54b96df39f18cd) |  |
| [`async`](#group__base_1gaa598621d521e3e8757f678a4ba025936) | Returns true if the implementation is thread-based, or false if it belongs to an event loop. |
| [`_handle`](#group__base_1ga7e421f1a449033592d904e89650c8ddd) |  |

---

#### Synchronizer 

```cpp
Synchronizer(uv::Loop * loop)
```

Create the synchronization context the given event loop.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### Synchronizer 

```cpp
Synchronizer(std::function< void()> target, uv::Loop * loop)
```

Create the synchronization context the given event loop and method. The target method will be called from the event loop context.

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `std::function< void()>` |  |
| `loop` | `uv::Loop *` |  |

---

#### Synchronizer 

```cpp
template<typename Function, typename... Args> inline explicit Synchronizer(Function && func, Args &&... args, uv::Loop * loop)
```

Create the synchronization context the given event loop and method.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |
| `loop` | `uv::Loop *` |  |

---

#### ~Synchronizer 

```cpp
virtual ~Synchronizer()
```

Destructor.

---

#### post 

```cpp
void post()
```

Send a synchronization request to the event loop. Call this each time you want the target method called synchronously. The synchronous method will be called on next iteration. This is not atomic, so do not expect a callback for every request.

---

#### start 

```cpp
template<typename Function, typename... Args> inline void start(Function && func, Args &&... args)
```

Start the synchronizer with the given callback.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `Function &&` |  |
| `args` | `Args &&...` |  |

---

#### start 

```cpp
virtual void start(std::function< void()> func)
```

Start the synchronizer with the given callback function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `std::function< void()>` |  |

---

#### cancel 

```cpp
virtual void cancel()
```

---

#### close 

```cpp
virtual void close()
```

---

#### handle 

```cpp
uv::Handle< uv_async_t > & handle()
```

---

#### async 

```cpp
virtual bool async() const
```

Returns true if the implementation is thread-based, or false if it belongs to an event loop.

---

#### _handle 

```cpp
uv::Handle< uv_async_t > _handle
```

## Task 

> **Extends:** `icy::basic::Runnable`
> **Subclasses:** `icy::sched::Task`
> **Defined in:** `task.h`

Abstract class is for implementing any kind asyncronous task.

Tasks are designed to be run by a [TaskRunner](#classicy_1_1TaskRunner).

### Members

| Name | Description |
|------|-------------|
| [`Task`](#group__base_1ga785465536aec43f179f47c7439e44aa8) | Constructor. |
| [`destroy`](#group__base_1gaecd8a85018f8f75ae802dcbf3749bdef) | Sets the task to destroyed state. |
| [`destroyed`](#group__base_1ga7a28372139954b0ff0c96f99be0418f2) | Signals that the task should be disposed of. |
| [`repeating`](#group__base_1gaed5e6c02f8118ab86c7ff620ee2ddf48) | Signals that the task's should be called repeatedly by the [TaskRunner](#classicy_1_1TaskRunner). If this returns false the task will be [cancelled()](#group__base_1gab3795d080eae2e58a57f2c63524c8ef3) |
| [`id`](#group__base_1ga4ffbff2b4bfdc71d9a278f9693297bc1) | Unique task ID. |
| [`Task`](#group__base_1gac513ae573567b9f7708c278c614c7e44) |  |
| [`operator=`](#group__base_1ga7bdbb1d628d2e2f2865cc2aad59480f2) |  |
| [`~Task`](#group__base_1gac05de8a5d56fb224bee75277714de893) | Destroctor. Should remain protected. |
| [`run`](#group__base_1ga0bfb09779b8f6aa9c5e3ce7dd7e4cfe3) | Called by the [TaskRunner](#classicy_1_1TaskRunner) to run the task. Override this method to implement task action. Returning true means the true should be called again, and false will cause the task to be destroyed. The task will similarly be destroyed id [destroy()](#group__base_1gaecd8a85018f8f75ae802dcbf3749bdef) was called during the current task iteration. |
| [`_id`](#group__base_1ga1d20300119a652f0ce185bc1510b041f) |  |
| [`_repeating`](#group__base_1gae5fcfd4789448c078bbc04a24b00849a) |  |
| [`_destroyed`](#group__base_1ga930e56a65dfd577c40fe7d30f3018e68) |  |

---

#### Task 

```cpp
Task(bool repeat)
```

Constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `repeat` | `bool` |  |

---

#### destroy 

```cpp
virtual void destroy()
```

Sets the task to destroyed state.

---

#### destroyed 

```cpp
virtual bool destroyed() const
```

Signals that the task should be disposed of.

---

#### repeating 

```cpp
virtual bool repeating() const
```

Signals that the task's should be called repeatedly by the [TaskRunner](#classicy_1_1TaskRunner). If this returns false the task will be [cancelled()](#group__base_1gab3795d080eae2e58a57f2c63524c8ef3)

---

#### id 

```cpp
virtual uint32_t id() const
```

Unique task ID.

---

#### Task 

```cpp
Task(const Task & task) = delete
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `const Task &` |  |

---

#### operator= 

```cpp
Task & operator=(Task const &) = delete
```

---

#### ~Task 

```cpp
virtual ~Task()
```

Destroctor. Should remain protected.

---

#### run 

```cpp
void run()
```

Called by the [TaskRunner](#classicy_1_1TaskRunner) to run the task. Override this method to implement task action. Returning true means the true should be called again, and false will cause the task to be destroyed. The task will similarly be destroyed id [destroy()](#group__base_1gaecd8a85018f8f75ae802dcbf3749bdef) was called during the current task iteration.

---

#### _id 

```cpp
uint32_t _id
```

---

#### _repeating 

```cpp
bool _repeating
```

---

#### _destroyed 

```cpp
bool _destroyed
```

## TaskRunner 

> **Extends:** `icy::basic::Runnable`
> **Subclasses:** `icy::sched::Scheduler`
> **Defined in:** `task.h`

[Runner](#classicy_1_1Runner) for tasks that inherit the `[Task](#classicy_1_1Task)` interface.

The `[TaskRunner](#classicy_1_1TaskRunner)` continually loops through each task in the task list calling the task's `[run()](#group__base_1ga07a481d667f1a9c67e881eedcffc10a5)` method.

The `[TaskRunner](#classicy_1_1TaskRunner)` is powered by an abstract `[Runner](#classicy_1_1Runner)` instance, which means that tasks can be executed in a thread or event loop context.

### Members

| Name | Description |
|------|-------------|
| [`TaskRunner`](#group__base_1ga68fea71b30b2732252bf31bf34f34b13) |  |
| [`~TaskRunner`](#group__base_1ga7f694f4681bee437fffd58ad6e21f09e) |  |
| [`start`](#group__base_1ga3f30dc3d3e1f68ada87091a2f1ca73d1) | Starts a task, adding it if it doesn't exist. |
| [`cancel`](#group__base_1ga429b24f441d2f86b7dcddc6764359d49) | Cancels a task. |
| [`destroy`](#group__base_1ga44547fd3858f512ea07c71676059ca7e) | Queues a task for destruction. |
| [`exists`](#group__base_1ga6f2cdcb41929fb914cdf288e1a00358e) | Returns weather or not a task exists. |
| [`get`](#group__base_1ga0c78d8e9196bc74fe1a5120974f4c8b9) | Returns the task pointer matching the given ID, or nullptr if no task exists. |
| [`setRunner`](#group__base_1ga9fe00258dfd9bef1fc91c5a944c6b754) | Set the asynchronous context for packet processing. This may be a [Thread](#classicy_1_1Thread) or another derivative of Async. Must be set before the stream is activated. |
| [`className`](#group__base_1ga710f2dcbc11de260c76c16b8c8600ca3) |  |
| [`getDefault`](#group__base_1ga216b6da5a3e0ae72bebf7371ef0e4725) | Returns the default `[TaskRunner](#classicy_1_1TaskRunner)` singleton, although [TaskRunner](#classicy_1_1TaskRunner) instances may be initialized individually. The default runner should be kept for short running tasks such as timers in order to maintain performance. |
| [`run`](#group__base_1ga07a481d667f1a9c67e881eedcffc10a5) | Called by the async context to run the next task. |
| [`add`](#group__base_1ga3cf9ff9159b170aa7e413bf0b0d0703e) | Adds a task to the runner. |
| [`remove`](#group__base_1gaca2341833adec925d50db8a5e128f679) | Removes a task from the runner. |
| [`next`](#group__base_1ga5c318e730939bf4b63072ebcdac5e9ee) | Returns the next task to be run. |
| [`clear`](#group__base_1gaa92a25f1d62e16bc40642688f569d5bd) | Destroys and clears all manages tasks. |
| [`onAdd`](#group__base_1gad6fc274ed81e757dba96db8f24fb2420) | Called after a task is added. |
| [`onStart`](#group__base_1ga2178d212993fd101fbe839afaaa0f79f) | Called after a task is started. |
| [`onCancel`](#group__base_1gab28a8ded043ae47ecace8c79512ec31a) | Called after a task is cancelled. |
| [`onRemove`](#group__base_1ga3c19020b93c4abaf5006c67a8b1ae474) | Called after a task is removed. |
| [`onRun`](#group__base_1ga0851ff27ae76aeb33d8d4f289844aa61) | Called after a task has run. |
| [`Idle`](#group__base_1gabf058e5fccd90bd4c65e22a5bc542ddf) | Fires after completing an iteration of all tasks. |
| [`Shutdown`](#group__base_1gaf3afae0333fb5b248c320f07a876c682) | Signals when the `[TaskRunner](#classicy_1_1TaskRunner)` is shutting down. |
| [`_mutex`](#group__base_1ga0f479eb77e6349c2d7db94bd1776b578) |  |
| [`_runner`](#group__base_1gaac415837586c91449e3ef85c33a2e6f3) |  |
| [`_tasks`](#group__base_1ga0d19424a7b9506240e72da3a45a4b08c) |  |

---

#### TaskRunner 

```cpp
TaskRunner(std::shared_ptr< Runner > runner)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `runner` | `std::shared_ptr< Runner >` |  |

---

#### ~TaskRunner 

```cpp
virtual ~TaskRunner()
```

---

#### start 

```cpp
virtual bool start(Task * task)
```

Starts a task, adding it if it doesn't exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### cancel 

```cpp
virtual bool cancel(Task * task)
```

Cancels a task.

The task reference will be managed the [TaskRunner](#classicy_1_1TaskRunner) until the task is destroyed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### destroy 

```cpp
virtual bool destroy(Task * task)
```

Queues a task for destruction.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### exists 

```cpp
virtual bool exists(Task * task) const
```

Returns weather or not a task exists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### get 

```cpp
virtual Task * get(uint32_t id) const
```

Returns the task pointer matching the given ID, or nullptr if no task exists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `uint32_t` |  |

---

#### setRunner 

```cpp
virtual void setRunner(std::shared_ptr< Runner > runner)
```

Set the asynchronous context for packet processing. This may be a [Thread](#classicy_1_1Thread) or another derivative of Async. Must be set before the stream is activated.

| Parameter | Type | Description |
|-----------|------|-------------|
| `runner` | `std::shared_ptr< Runner >` |  |

---

#### className 

```cpp
virtual inline const char * className() const
```

---

#### getDefault 

```cpp
static TaskRunner & getDefault()
```

Returns the default `[TaskRunner](#classicy_1_1TaskRunner)` singleton, although [TaskRunner](#classicy_1_1TaskRunner) instances may be initialized individually. The default runner should be kept for short running tasks such as timers in order to maintain performance.

---

#### run 

```cpp
virtual void run()
```

Called by the async context to run the next task.

---

#### add 

```cpp
virtual bool add(Task * task)
```

Adds a task to the runner.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### remove 

```cpp
virtual bool remove(Task * task)
```

Removes a task from the runner.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### next 

```cpp
virtual Task * next() const
```

Returns the next task to be run.

---

#### clear 

```cpp
virtual void clear()
```

Destroys and clears all manages tasks.

---

#### onAdd 

```cpp
virtual void onAdd(Task * task)
```

Called after a task is added.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### onStart 

```cpp
virtual void onStart(Task * task)
```

Called after a task is started.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### onCancel 

```cpp
virtual void onCancel(Task * task)
```

Called after a task is cancelled.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### onRemove 

```cpp
virtual void onRemove(Task * task)
```

Called after a task is removed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### onRun 

```cpp
virtual void onRun(Task * task)
```

Called after a task has run.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `Task *` |  |

---

#### Idle 

```cpp
NullSignal Idle
```

Fires after completing an iteration of all tasks.

---

#### Shutdown 

```cpp
NullSignal Shutdown
```

Signals when the `[TaskRunner](#classicy_1_1TaskRunner)` is shutting down.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _runner 

```cpp
std::shared_ptr< Runner > _runner
```

---

#### _tasks 

```cpp
TaskList _tasks
```

## Test 

> **Subclasses:** `icy::test::FunctionTest`
> **Defined in:** `test.h`

[Test](#classicy_1_1test_1_1Test) wrapper class.

This class is for implementing any kind of unit test that can be executed by a `[TestRunner](#classicy_1_1test_1_1TestRunner)`.

### Members

| Name | Description |
|------|-------------|
| [`Test`](#group__base_1ga77ec963c31149717e725fb25a0d9c3e2) |  |
| [`~Test`](#group__base_1ga190692773ad0ccef364dbfcd5a10b2a8) | Should remain protected. |
| [`run`](#group__base_1ga4b2e88e8a32def45844c9fdcf1b4bc5e) | Called by the [TestRunner](#classicy_1_1test_1_1TestRunner) to run the test. |
| [`passed`](#group__base_1ga7136d60a9c295927a26d1eee662a9112) | Return true when the test passed without errors. |
| [`Test`](#group__base_1ga53e1fc6fb63e2228193a074b44f2ae8c) |  |
| [`operator=`](#group__base_1ga80fa2a2d52c286fd031b8620e2395a42) |  |
| [`name`](#group__base_1ga05c0192e474a337da0822200d2ad8084) | The name of the test. |
| [`errors`](#group__base_1ga97c97aa995efb443d470f235a08275a9) | A list of test errors. |
| [`duration`](#group__base_1gac56aeb8b24a0ad4d6ac4d5c8f8f2a260) | The test run duration for benchmarking. |

---

#### Test 

```cpp
Test(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### ~Test 

```cpp
virtual ~Test()
```

Should remain protected.

---

#### run 

```cpp
void run()
```

Called by the [TestRunner](#classicy_1_1test_1_1TestRunner) to run the test.

---

#### passed 

```cpp
bool passed()
```

Return true when the test passed without errors.

---

#### Test 

```cpp
Test(const Test & test) = delete
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `test` | `const Test &` |  |

---

#### operator= 

```cpp
Test & operator=(Test const &) = delete
```

---

#### name 

```cpp
std::string name
```

The name of the test.

---

#### errors 

```cpp
SErrorist errors
```

A list of test errors.

---

#### duration 

```cpp
double duration
```

The test run duration for benchmarking.

## FunctionTest 

> **Extends:** `icy::test::Test`
> **Defined in:** `test.h`

### Members

| Name | Description |
|------|-------------|
| [`FunctionTest`](#group__base_1gac840656d94f9442624c1b15f4d72bf78) |  |
| [`~FunctionTest`](#group__base_1gacc32a5204646832c258c30892449531e) |  |
| [`run`](#group__base_1ga5d3c89ca30abab0fe4020db97ed6e513) | Called by the [TestRunner](#classicy_1_1test_1_1TestRunner) to run the test. |
| [`target`](#group__base_1gabc68dd3cfbefe5af9544fb34b9edb6d7) |  |

---

#### FunctionTest 

```cpp
inline FunctionTest(std::function< void()> target, const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `std::function< void()>` |  |
| `name` | `const std::string &` |  |

---

#### ~FunctionTest 

```cpp
virtual inline ~FunctionTest()
```

---

#### run 

```cpp
virtual inline void run()
```

Called by the [TestRunner](#classicy_1_1test_1_1TestRunner) to run the test.

---

#### target 

```cpp
std::function< void()> target
```

## TestRunner 

> **Defined in:** `test.h`

[Test](#classicy_1_1test_1_1Test) manager queue.

The `[TestRunner](#classicy_1_1test_1_1TestRunner)` is a queue in charge of running one or many tests.

When `[run()](#group__base_1gae237ec747f0a785fb2514154a46022b3)` the `[TestRunner](#classicy_1_1test_1_1TestRunner)` loops through each test in the list calling the test's `[run()](#group__base_1gae237ec747f0a785fb2514154a46022b3)` method.

### Members

| Name | Description |
|------|-------------|
| [`TestRunner`](#group__base_1gaa969f22aa934715fccb037572de9d51b) |  |
| [`~TestRunner`](#group__base_1ga816353a412bed8f169ecfaf82b34cd8b) |  |
| [`add`](#group__base_1ga576996da07db6bf6de177586530c352c) | Add a test to the runner. |
| [`get`](#group__base_1gae6eae9609b3a4b78e8508e930e924220) | Return a pointer to the test matching the given name, or nullptr if no matching test exists. |
| [`run`](#group__base_1gae237ec747f0a785fb2514154a46022b3) | Called by the async context to run the next test. |
| [`clear`](#group__base_1ga6dd3a9bc14a7c4223d8811bc11651f82) | Destroy and clears all managed tests. |
| [`current`](#group__base_1gad5960442871ffed66bf6d14a159ec29f) | Return the currently active [Test](#classicy_1_1test_1_1Test) or nullptr. |
| [`tests`](#group__base_1ga63d7bb169e8987b0ef42e032a5c15dec) | Return the list of tests. |
| [`errors`](#group__base_1ga3f6c0d4bd8239546c2ffb09baf3c6d17) | Return a map of tests and errors. |
| [`passed`](#group__base_1gaa899250db70a6f7d3855a58ac46b9385) | Return true if all tests passed. |
| [`getDefault`](#group__base_1gac6a1b7a3bb9090c9426d5f785f0979e3) | Return the default `[TestRunner](#classicy_1_1test_1_1TestRunner)` singleton, although `[TestRunner](#classicy_1_1test_1_1TestRunner)` instances may also be initialized individually. |
| [`_mutex`](#group__base_1ga55aaec3e17a75e9df9a1640a4830d674) |  |
| [`_tests`](#group__base_1ga07642f477365332784394b083e0878ec) |  |
| [`_current`](#group__base_1ga3e5f349d8600497eca5acacd6e853ad7) |  |

---

#### TestRunner 

```cpp
TestRunner()
```

---

#### ~TestRunner 

```cpp
virtual ~TestRunner()
```

---

#### add 

```cpp
void add(Test * test)
```

Add a test to the runner.

| Parameter | Type | Description |
|-----------|------|-------------|
| `test` | `Test *` |  |

---

#### get 

```cpp
Test * get(const std::string & name) const
```

Return a pointer to the test matching the given name, or nullptr if no matching test exists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### run 

```cpp
void run()
```

Called by the async context to run the next test.

---

#### clear 

```cpp
void clear()
```

Destroy and clears all managed tests.

---

#### current 

```cpp
Test * current() const
```

Return the currently active [Test](#classicy_1_1test_1_1Test) or nullptr.

---

#### tests 

```cpp
TestList tests() const
```

Return the list of tests.

---

#### errors 

```cpp
ErrorMap errors() const
```

Return a map of tests and errors.

---

#### passed 

```cpp
bool passed() const
```

Return true if all tests passed.

---

#### getDefault 

```cpp
static TestRunner & getDefault()
```

Return the default `[TestRunner](#classicy_1_1test_1_1TestRunner)` singleton, although `[TestRunner](#classicy_1_1test_1_1TestRunner)` instances may also be initialized individually.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _tests 

```cpp
TestList _tests
```

---

#### _current 

```cpp
Test * _current
```

## Timer 

> **Extends:** `icy::Runner`
> **Defined in:** `timer.h`

Asynchronous event based timer.

### Members

| Name | Description |
|------|-------------|
| [`Timer`](#group__base_1ga1fae10ff8c5f7967f91cde97229af7f9) | Create a timer. |
| [`Timer`](#group__base_1ga2371f483389963e2b6634835452c5f5f) | Create a timeout timer. |
| [`Timer`](#group__base_1gac5f14edef0a9c3a7e5109db6a56133dc) | Create a repeating interval timer. |
| [`~Timer`](#group__base_1ga64a91b7e8c9c4145058cf2088b3221a4) | Destructor. |
| [`start`](#group__base_1gabf583d65fae824bb07ed66885491f916) | Start the timer. |
| [`start`](#group__base_1gaa7be3558a210ba5a963f8b4ae05cfbf8) | Start the timer with the given callback function. |
| [`stop`](#group__base_1ga13edfe3f0b4efd439cf640377624b5be) | Stop the timer. |
| [`restart`](#group__base_1ga0e9afbcacb03261ee441b0631e2dccf1) | Restart the timer. |
| [`again`](#group__base_1gab91426f1cb86e09423775e6ee59a2b8e) | Trigger the timer again. |
| [`setTimeout`](#group__base_1gabbfdeaba8dbf67e79d2635660fc10551) | Set the timeout value. |
| [`setInterval`](#group__base_1gab2ba3bfa336276158867c017b39f24e4) | Set the repeat value. |
| [`active`](#group__base_1ga3bc8f6a117d6982ea125f7d1f26ad67f) |  |
| [`timeout`](#group__base_1ga731fc5f29c10ef7237494b5fef7063d5) |  |
| [`interval`](#group__base_1gae80ec21a3b77a186e8781179e478a623) |  |
| [`count`](#group__base_1ga3e4978ac6eb64b151c670a68980ff11d) |  |
| [`handle`](#group__base_1gac14b1d3b7ddd6fc5e30146cd4f40ed28) |  |
| [`async`](#group__base_1ga4fd229d3c1db2fd45362cf0861b5e359) | Returns true if the implementation is thread-based, or false if it belongs to an event loop. |
| [`Timer`](#group__base_1ga7757e24daa73bdf095e93d94050adf57) |  |
| [`operator=`](#group__base_1gab242b0701daeebee901b2b8b1ceaea9d) |  |
| [`init`](#group__base_1ga4d7d19830efea92f0944f972caba2fdb) |  |
| [`Timeout`](#group__base_1ga70ab283148e5fb200fe84f75ea9ea138) | [Signal](#classicy_1_1Signal) that gets triggered on timeout. |
| [`_handle`](#group__base_1gaf4b35f98a8978d847315e8f3415cc639) |  |
| [`_timeout`](#group__base_1ga146dcd71f59664feead6e84c971c6b9a) |  |
| [`_interval`](#group__base_1ga0a84a02412fc3e3fe9fed3ce4ff8b7fa) |  |
| [`_count`](#group__base_1gad7d0505734bb3683560456c3a5fea546) |  |

---

#### Timer 

```cpp
Timer(uv::Loop * loop)
```

Create a timer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### Timer 

```cpp
Timer(std::int64_t timeout, uv::Loop * loop, std::function< void()> func)
```

Create a timeout timer.

The timeout timer will trigger once after `timeout` milliseconds.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `std::int64_t` |  |
| `loop` | `uv::Loop *` |  |
| `func` | `std::function< void()>` |  |

---

#### Timer 

```cpp
Timer(std::int64_t timeout, std::int64_t interval, uv::Loop * loop, std::function< void()> func)
```

Create a repeating interval timer.

The interval timer will trigger once after `timeout` milliseconds, and continue to trigger after the `interval` milliseconds.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `std::int64_t` |  |
| `interval` | `std::int64_t` |  |
| `loop` | `uv::Loop *` |  |
| `func` | `std::function< void()>` |  |

---

#### ~Timer 

```cpp
virtual ~Timer()
```

Destructor.

---

#### start 

```cpp
void start()
```

Start the timer.

---

#### start 

```cpp
virtual void start(std::function< void()> func)
```

Start the timer with the given callback function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `std::function< void()>` |  |

---

#### stop 

```cpp
void stop()
```

Stop the timer.

---

#### restart 

```cpp
void restart()
```

Restart the timer.

This method works even if it hasn't been started yet. An interval or interval must be set or an exception will be thrown.

---

#### again 

```cpp
void again()
```

Trigger the timer again.

If the timer is repeating restart it using the repeat value as the `timeout`. If the timer has never been started before it returns -1 and sets the error to `UV_EINVAL`.

---

#### setTimeout 

```cpp
void setTimeout(std::int64_t timeout)
```

Set the timeout value.

The timer must not be active when this value is set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `timeout` | `std::int64_t` |  |

---

#### setInterval 

```cpp
void setInterval(std::int64_t interval)
```

Set the repeat value.

Note that if the repeat value is set from a timer callback it does not immediately take effect. If the timer was non-repeating before, it will have been stopped. If it was repeating, then the old repeat value will have been used to schedule the next timeout.

| Parameter | Type | Description |
|-----------|------|-------------|
| `interval` | `std::int64_t` |  |

---

#### active 

```cpp
bool active() const
```

---

#### timeout 

```cpp
std::int64_t timeout() const
```

---

#### interval 

```cpp
std::int64_t interval() const
```

---

#### count 

```cpp
std::int64_t count()
```

---

#### handle 

```cpp
uv::Handle< uv_timer_t > & handle()
```

---

#### async 

```cpp
virtual bool async() const
```

Returns true if the implementation is thread-based, or false if it belongs to an event loop.

---

#### Timer 

```cpp
Timer(const Timer &) = delete
```

---

#### operator= 

```cpp
Timer & operator=(const Timer &) = delete
```

---

#### init 

```cpp
void init()
```

---

#### Timeout 

```cpp
NullSignal Timeout
```

[Signal](#classicy_1_1Signal) that gets triggered on timeout.

---

#### _handle 

```cpp
uv::Handle< uv_timer_t > _handle
```

---

#### _timeout 

```cpp
std::int64_t _timeout
```

---

#### _interval 

```cpp
std::int64_t _interval
```

---

#### _count 

```cpp
std::int64_t _count
```

## TZInfo 

### Members

| Name | Description |
|------|-------------|
| [`TZInfo`](#group__base_1ga6491a626255da59420f4e79f135bec79) |  |
| [`timeZone`](#group__base_1ga2a6e305f50e4018b7a4f8cd97a4f579e) |  |
| [`name`](#group__base_1ga4f6e3fa6eb45afcecab5a279e0e98dcb) |  |

---

#### TZInfo 

```cpp
inline TZInfo()
```

---

#### timeZone 

```cpp
inline int timeZone()
```

---

#### name 

```cpp
inline const char * name(bool dst)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `dst` | `bool` |  |

## Timeout 

> **Subclasses:** `icy::TimedToken`
> **Defined in:** `timeout.h`

[Timeout](#classicy_1_1Timeout) counter which expires after a given delay.

### Members

| Name | Description |
|------|-------------|
| [`Timeout`](#group__base_1ga7a0c9d99bcece690d28b3568a532c8bb) |  |
| [`Timeout`](#group__base_1gafd9ba1a97e117625f6c87121c5b1bb53) |  |
| [`~Timeout`](#group__base_1ga96b82e3575fe7a5d18d46f9e409e3883) |  |
| [`running`](#group__base_1ga451634c2144d1adda5a9301dcef6c118) |  |
| [`start`](#group__base_1ga4de922b575fa908ce89b978f26ef4059) |  |
| [`stop`](#group__base_1gafb82ee057268b469694dc2aa8c72bc36) |  |
| [`reset`](#group__base_1gaddc18501ce642ff8724d15fc41939ac8) |  |
| [`remaining`](#group__base_1gae7eafa18334cdd1def62b1cd9504a5ed) |  |
| [`expired`](#group__base_1ga434bc816acdc562bd4434b853d2252f9) |  |
| [`setDelay`](#group__base_1ga29f93b01c146d755a6f46bb1f6ec8cee) |  |
| [`startAt`](#group__base_1ga82eac31b0527400086bbe0c2414ad9b5) |  |
| [`delay`](#group__base_1ga5d32e6ba4780efe685496b6d6ef95c62) |  |
| [`operator=`](#group__base_1ga3f4396dde53c6f38869c40b1a1ee1074) |  |
| [`_startAt`](#group__base_1gac8610fab1717accf992e597d12a904c1) |  |
| [`_delay`](#group__base_1ga94fe2854e36942ecf3abff5a1f19769e) |  |

---

#### Timeout 

```cpp
Timeout(long delay, bool autoStart)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `delay` | `long` |  |
| `autoStart` | `bool` |  |

---

#### Timeout 

```cpp
Timeout(const Timeout & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `const Timeout &` |  |

---

#### ~Timeout 

```cpp
~Timeout()
```

---

#### running 

```cpp
bool running() const
```

---

#### start 

```cpp
void start()
```

---

#### stop 

```cpp
void stop()
```

---

#### reset 

```cpp
void reset()
```

---

#### remaining 

```cpp
long remaining() const
```

---

#### expired 

```cpp
bool expired() const
```

---

#### setDelay 

```cpp
inline void setDelay(long delay)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `delay` | `long` |  |

---

#### startAt 

```cpp
inline clock_t startAt() const
```

---

#### delay 

```cpp
inline long delay() const
```

---

#### operator= 

```cpp
Timeout & operator=(const Timeout & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `const Timeout &` |  |

---

#### _startAt 

```cpp
clock_t _startAt
```

---

#### _delay 

```cpp
long _delay
```

## TimedToken 

> **Extends:** `icy::Timeout`
> **Defined in:** `timeout.h`

Token that expires after the specified duration.

### Members

| Name | Description |
|------|-------------|
| [`TimedToken`](#group__base_1ga3172961c6e661451b64bc60c935c2e30) |  |
| [`TimedToken`](#group__base_1ga00d0a09459c0d5b2de18de73ccf1eda7) |  |
| [`id`](#group__base_1ga468f28c2d15d97b85ed6687bec10ce37) |  |
| [`operator==`](#group__base_1ga36b8cadb6d6ccc57b725b530d8f92f47) |  |
| [`operator==`](#group__base_1ga44476c978974979e4cb079b8562ce724) |  |
| [`_id`](#group__base_1ga7d90971c34ef4d63f00a5153c74cc1c3) |  |

---

#### TimedToken 

```cpp
TimedToken(long duration)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `duration` | `long` |  |

---

#### TimedToken 

```cpp
TimedToken(const std::string & id, long duration)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `duration` | `long` |  |

---

#### id 

```cpp
inline std::string id() const
```

---

#### operator== 

```cpp
inline bool operator==(const TimedToken & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const TimedToken &` |  |

---

#### operator== 

```cpp
inline bool operator==(const std::string & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const std::string &` |  |

---

#### _id 

```cpp
std::string _id
```

## OptionParser 

> **Defined in:** `application.h`

### Members

| Name | Description |
|------|-------------|
| [`OptionParser`](#group__base_1gacfaf92525d5f46c8c4165b4e9687a01e) |  |
| [`has`](#group__base_1gadce3a04bb7896063238940156056e18e) |  |
| [`get`](#group__base_1ga975b86adb62cff3250a769a9e63d2d7c) |  |
| [`get`](#group__base_1gadc8dc1da69c345f00ee501b8b163da3b) |  |
| [`exepath`](#group__base_1gacf94798c87c7e8a3d9c0fee9f0b9b021) |  |
| [`args`](#group__base_1ga3f72252b7f830d90de66dcefa2372585) |  |

---

#### OptionParser 

```cpp
OptionParser(int argc, char * argv, const char * delim)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `argc` | `int` |  |
| `argv` | `char *` |  |
| `delim` | `const char *` |  |

---

#### has 

```cpp
inline bool has(const char * key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const char *` |  |

---

#### get 

```cpp
inline std::string get(const char * key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const char *` |  |

---

#### get 

```cpp
template<typename NumericType> inline NumericType get(const char * key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const char *` |  |

---

#### exepath 

```cpp
std::string exepath
```

---

#### args 

```cpp
OptionMap args
```

## ShutdownCmd 

> **Defined in:** `application.h`

### Members

| Name | Description |
|------|-------------|
| [`opaque`](#group__base_1gaf060b2f1e83d40f1648fed0eae5a3083) |  |
| [`callback`](#group__base_1gad5fcf4a4368d6d70bafa19fe339edfcc) |  |

---

#### opaque 

```cpp
void * opaque
```

---

#### callback 

```cpp
std::function< void(void *)> callback
```

## Encoder 

> **Extends:** `icy::basic::Encoder`
> **Defined in:** `base64.h`

Base64 encoder.

### Members

| Name | Description |
|------|-------------|
| [`Encoder`](#group__base_1ga0b4bbf5fe617990a4044d61a5aa4d9b5) |  |
| [`encode`](#group__base_1ga8cd9623054dd2d6eebee7561feeeaec5) |  |
| [`encode`](#group__base_1ga169108c69ec1b416d1bdc1a21eb2114d) |  |
| [`encode`](#group__base_1ga7466c9aec36a67c1f41a20841f09192c) |  |
| [`finalize`](#group__base_1ga203b9c249c5a45dc281d85b74588f8ee) |  |
| [`setLineLength`](#group__base_1gaa95988d443bebd458b3c2e421a46d799) |  |
| [`_state`](#group__base_1gad85db3c5637d5b339034826bdbc0e786) |  |
| [`_buffersize`](#group__base_1ga759e0716429cb47740e4a65ab28acc6b) |  |

---

#### Encoder 

```cpp
inline Encoder(int buffersize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffersize` | `int` |  |

---

#### encode 

```cpp
inline void encode(std::istream & istrm, std::ostream & ostrm)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `istrm` | `std::istream &` |  |
| `ostrm` | `std::ostream &` |  |

---

#### encode 

```cpp
inline void encode(const std::string & in, std::string & out)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `in` | `const std::string &` |  |
| `out` | `std::string &` |  |

---

#### encode 

```cpp
virtual inline ssize_t encode(const char * inbuf, size_t nread, char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

#### finalize 

```cpp
virtual inline ssize_t finalize(char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `outbuf` | `char *` |  |

---

#### setLineLength 

```cpp
inline void setLineLength(int lineLength)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `lineLength` | `int` |  |

---

#### _state 

```cpp
internal::encodestate _state
```

---

#### _buffersize 

```cpp
int _buffersize
```

## Decoder 

> **Extends:** `icy::basic::Decoder`
> **Defined in:** `base64.h`

Base64 decoder.

### Members

| Name | Description |
|------|-------------|
| [`Decoder`](#group__base_1ga3a55fbfacbfe7fb8ea87b7788d02f7b7) |  |
| [`decode`](#group__base_1ga6ebfa454300eb5794cee27ce14d2210a) |  |
| [`decode`](#group__base_1ga8156e5a9480a2c87dadc098e7827ac73) |  |
| [`decode`](#group__base_1gaee095eb1f717a873862271efdf8b78f5) |  |
| [`_state`](#group__base_1ga852ccfc6b149bdbf6158bd830a00fb8f) |  |
| [`_buffersize`](#group__base_1gae44073b77993e4a6a1461d75960aaa61) |  |

---

#### Decoder 

```cpp
inline Decoder(int buffersize)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffersize` | `int` |  |

---

#### decode 

```cpp
inline ssize_t decode(char value_in)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value_in` | `char` |  |

---

#### decode 

```cpp
virtual inline ssize_t decode(const char * inbuf, size_t nread, char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

#### decode 

```cpp
inline void decode(std::istream & istrm, std::ostream & ostrm)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `istrm` | `std::istream &` |  |
| `ostrm` | `std::ostream &` |  |

---

#### _state 

```cpp
internal::decodestate _state
```

---

#### _buffersize 

```cpp
int _buffersize
```

## Bitwise 

> **Defined in:** `bitwise.h`

Container for smart management of bitwise integer flags.

### Members

| Name | Description |
|------|-------------|
| [`Bitwise`](#group__base_1gab17fe0dc48af357d997ba8bcce089def) |  |
| [`reset`](#group__base_1ga61f01af4d27436a26ef0c336f9d0dd23) |  |
| [`set`](#group__base_1ga73020121e07e2201909bf45030ef28c9) |  |
| [`add`](#group__base_1ga3ff03ce689fc75d83c0fd12e25f4e489) |  |
| [`remove`](#group__base_1ga8fbd08d9008b713faa32830f3f164075) |  |
| [`toggle`](#group__base_1ga34695eee0b63a6118ef4af65f9dbf081) |  |
| [`has`](#group__base_1ga84fb5bee9780f9ed4f6417adf378a039) |  |
| [`data`](#group__base_1ga561f756550e5240f615043453f77912b) |  |

---

#### Bitwise 

```cpp
inline Bitwise(unsigned flags)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flags` | `unsigned` |  |

---

#### reset 

```cpp
virtual inline void reset()
```

---

#### set 

```cpp
virtual inline void set(unsigned flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `unsigned` |  |

---

#### add 

```cpp
virtual inline void add(unsigned flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `unsigned` |  |

---

#### remove 

```cpp
virtual inline void remove(unsigned flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `unsigned` |  |

---

#### toggle 

```cpp
virtual inline void toggle(unsigned flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `unsigned` |  |

---

#### has 

```cpp
virtual inline bool has(unsigned flag) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `unsigned` |  |

---

#### data 

```cpp
unsigned data
```

## ILT 

> **Defined in:** `collection.h`

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__base_1ga966ae0464dd2d1011bcd55702729b5eb) |  |

---

#### operator() 

```cpp
inline bool operator()(const std::string & s1, const std::string & s2) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `s1` | `const std::string &` |  |
| `s2` | `const std::string &` |  |

## AbstractDelegate 

> **Defined in:** `delegate.h`

Abstract delegate interface.

The `Delegate` class contains a pointer to a function. This wrapper class is used instead of `std::function` since it is interchangable with fast delegates and also provides an equality operator for comparing the underlying function where supported.

### Members

| Name | Description |
|------|-------------|
| [`~AbstractDelegate`](#group__base_1ga1af7274ae4df82deae9da6ee4ae98fe5) |  |
| [`operator()`](#group__base_1ga3b6d651d76d2b08df99ea910dcc102d2) |  |
| [`operator==`](#group__base_1ga1680a6dd4d3a54f5e52189e3d7d043bd) |  |

---

#### ~AbstractDelegate 

```cpp
virtual inline ~AbstractDelegate()
```

---

#### operator() 

```cpp
RT operator()(Args... args) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` |  |

---

#### operator== 

```cpp
bool operator==(const AbstractDelegate< RT, Args... > & that) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const AbstractDelegate< RT, Args... > &` |  |

## FunctionDelegate 

> **Extends:** `icy::AbstractDelegate< RT, Args... >`
> **Defined in:** `delegate.h`

The `[FunctionDelegate](#structicy_1_1FunctionDelegate)` contains a `std::function`.

### Members

| Name | Description |
|------|-------------|
| [`FunctionDelegate`](#group__base_1gafcbadf834f7a4d0a40ca4e5db7e34f14) |  |
| [`operator()`](#group__base_1gad7c59caa9745234db4724fc1ea3d9501) |  |
| [`operator==`](#group__base_1ga09fee8d97f33cb5b1ae140177a689810) |  |
| [`func`](#group__base_1gaca57bf731778089faea714a0a8fcf078) |  |

---

#### FunctionDelegate 

```cpp
inline FunctionDelegate(std::function< RT(Args...)> func)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `func` | `std::function< RT(Args...)>` |  |

---

#### operator() 

```cpp
virtual inline RT operator()(Args... args) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` |  |

---

#### operator== 

```cpp
virtual inline bool operator==(const AbstractDelegate< RT, Args... > &) const
```

---

#### func 

```cpp
std::function< RT(Args...)> func
```

## ClassDelegate 

> **Extends:** `icy::AbstractDelegate< RT, Args... >`
> **Defined in:** `delegate.h`

The `[ClassDelegate](#structicy_1_1ClassDelegate)` contains a pointer to a class member.

This class implements fast delegates and function comparison.

### Members

| Name | Description |
|------|-------------|
| [`ClassDelegate`](#group__base_1ga1e015d5de17fee08ea68dd5718984608) |  |
| [`operator()`](#group__base_1gaca4d987f9321f3916c722bf583d75c06) |  |
| [`operator==`](#group__base_1ga7d0d05b24bab289f378cc0cebd3a47d0) |  |
| [`instance`](#group__base_1gaa13b69a5686d1236aa09f96ed2fe35e2) |  |
| [`method`](#group__base_1ga48f7cfa04f8bce9a3a4361015f5572a8) |  |

---

#### ClassDelegate 

```cpp
inline ClassDelegate(Class * instance, RT(Class::*)(Args...) method)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `instance` | `Class *` |  |
| `method` | `RT(Class::*)(Args...)` |  |

---

#### operator() 

```cpp
virtual inline RT operator()(Args... args) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` |  |

---

#### operator== 

```cpp
virtual inline bool operator==(const AbstractDelegate< RT, Args... > & that) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const AbstractDelegate< RT, Args... > &` |  |

---

#### instance 

```cpp
Class * instance
```

---

#### method 

```cpp
RT(Class::* method
```

## ConstClassDelegate 

> **Extends:** `icy::AbstractDelegate< RT, Args... >`
> **Defined in:** `delegate.h`

The `[ConstClassDelegate](#structicy_1_1ConstClassDelegate)` contains a pointer to a `const` class member.

This class implements fast delegates and function comparison.

### Members

| Name | Description |
|------|-------------|
| [`ConstClassDelegate`](#group__base_1ga5fac63979562f15b65f25a483ab0f169) |  |
| [`operator()`](#group__base_1ga762fd100320822342b42ed13377c1654) |  |
| [`operator==`](#group__base_1ga3e792075d3ee46ae33399d3be3ff7474) |  |
| [`instance`](#group__base_1ga4bee6cef9aa2f40ca5e2135acb7792a4) |  |
| [`method`](#group__base_1ga6a3575991488c6157ecb7b8136259bbe) |  |

---

#### ConstClassDelegate 

```cpp
inline ConstClassDelegate(Class * instance, RT(Class::*)(Args...) const method)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `instance` | `Class *` |  |
| `method` | `RT(Class::*)(Args...) const` |  |

---

#### operator() 

```cpp
virtual inline RT operator()(Args... args) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` |  |

---

#### operator== 

```cpp
virtual inline bool operator==(const AbstractDelegate< RT, Args... > & that) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const AbstractDelegate< RT, Args... > &` |  |

---

#### instance 

```cpp
Class * instance
```

---

#### method 

```cpp
RT(Class::* method
```

## PolymorphicDelegate 

> **Extends:** `icy::AbstractDelegate< RT, IT & >`
> **Defined in:** `delegate.h`

Polymorphic function delegate.

Theis class contains a pointer to a class member that receices a derived subclass (`PT`) of the base type specified by the `IT` param.

### Members

| Name | Description |
|------|-------------|
| [`PolymorphicDelegate`](#group__base_1ga053ed6d6ff8aaaf7a3d5e5f890470877) |  |
| [`operator()`](#group__base_1ga180cb44cb8588e1cb8ed2d8532944903) |  |
| [`operator==`](#group__base_1ga5d574d1759ddc6b92fb3252e445896d2) |  |
| [`instance`](#group__base_1gad74b599904e1a50cc05d2a4ab2f20a03) |  |
| [`method`](#group__base_1ga8efb24a32f5f596d1aeefba40af69b3d) |  |

---

#### PolymorphicDelegate 

```cpp
inline PolymorphicDelegate(Class * instance, RT(Class::*)(PT &) method)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `instance` | `Class *` |  |
| `method` | `RT(Class::*)(PT &)` |  |

---

#### operator() 

```cpp
virtual inline RT operator()(IT & object) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `IT &` |  |

---

#### operator== 

```cpp
virtual inline bool operator==(const AbstractDelegate< RT, IT & > & that) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const AbstractDelegate< RT, IT & > &` |  |

---

#### instance 

```cpp
Class * instance
```

---

#### method 

```cpp
RT(Class::* method
```

## Error 

> **Defined in:** `error.h`

Basic error type.

Errors contain an error code, message, and exception pointer.

### Members

| Name | Description |
|------|-------------|
| [`Error`](#group__base_1gaab8c612dc5997bbf6b57397ec1a0e6d9) |  |
| [`Error`](#group__base_1ga75e04b5142b2bcec58bf418dd15253c7) |  |
| [`Error`](#group__base_1gab773389bc3179a4587bb57fba06addbe) |  |
| [`any`](#group__base_1ga6acffca8b048905bbc39b40a9f6a589c) |  |
| [`reset`](#group__base_1ga55e9f6b538f9526c20ccafe8df691f7d) |  |
| [`rethrow`](#group__base_1gabe0a98baa3810d0e0ed301c4ce44b3b1) |  |
| [`err`](#group__base_1ga87426c8d146710ad14ed03a6d6dd5ff7) |  |
| [`message`](#group__base_1ga2c262d00d0cfd95c2d7326a6907269da) |  |
| [`exception`](#group__base_1gafa6e4615e88233c1ec0d1f0cb56b05f7) |  |

---

#### Error 

```cpp
inline Error()
```

---

#### Error 

```cpp
inline Error(const std::string & msg)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `msg` | `const std::string &` |  |

---

#### Error 

```cpp
inline Error(const char * msg)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `msg` | `const char *` |  |

---

#### any 

```cpp
inline bool any() const
```

---

#### reset 

```cpp
inline void reset()
```

---

#### rethrow 

```cpp
inline void rethrow()
```

---

#### err 

```cpp
int err
```

---

#### message 

```cpp
std::string message
```

---

#### exception 

```cpp
std::exception_ptr exception
```

## Encoder 

> **Extends:** `icy::basic::Encoder`
> **Defined in:** `hex.h`

Hex encoder.

### Members

| Name | Description |
|------|-------------|
| [`Encoder`](#group__base_1gaa0343ef0f07c144b4a50748750004aee) |  |
| [`encode`](#group__base_1ga013a8ad2abd63a127a87106d2b115a77) |  |
| [`finalize`](#group__base_1gad6a22c7c309534c6f8f484ce86e1445c) |  |
| [`setUppercase`](#group__base_1gaf3135b7edfac6b752a05686c193e2495) |  |
| [`setLineLength`](#group__base_1ga9c23084ff0fc0b186777ca003e30deda) |  |
| [`_linePos`](#group__base_1ga288899d48739dcf0378d6b9f11dfe58f) |  |
| [`_lineLength`](#group__base_1ga064c41e567fdbeb17a86c98f2bd278ff) |  |
| [`_uppercase`](#group__base_1ga4c6c6ece9aee62f7edbe81a4462b6f9f) |  |

---

#### Encoder 

```cpp
inline Encoder()
```

---

#### encode 

```cpp
virtual inline ssize_t encode(const char * inbuf, size_t nread, char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

#### finalize 

```cpp
virtual inline ssize_t finalize(char *)
```

---

#### setUppercase 

```cpp
inline void setUppercase(bool flag)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `flag` | `bool` |  |

---

#### setLineLength 

```cpp
inline void setLineLength(int lineLength)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `lineLength` | `int` |  |

---

#### _linePos 

```cpp
int _linePos
```

---

#### _lineLength 

```cpp
int _lineLength
```

---

#### _uppercase 

```cpp
int _uppercase
```

## Decoder 

> **Extends:** `icy::basic::Decoder`
> **Defined in:** `hex.h`

Hex decoder.

### Members

| Name | Description |
|------|-------------|
| [`Decoder`](#group__base_1ga5998a95cdfedea8b8ab0a02065f0a624) |  |
| [`~Decoder`](#group__base_1ga9d6d431cf4d0739cf304afcbc2962b2d) |  |
| [`decode`](#group__base_1gacf9b20b34b833cc8cb7d0db55de85b2e) |  |
| [`finalize`](#group__base_1ga9ed7367746cf1fa8291da4aecdf3cf79) |  |
| [`readnext`](#group__base_1ga2bdf4e0671eae619854ec7788e23afa1) |  |
| [`nybble`](#group__base_1gad58c78f2177b39304b7e910d56eac83d) |  |
| [`iswspace`](#group__base_1ga19c548e3b173984aba506a7f8a837d6f) |  |
| [`lastbyte`](#group__base_1gacf494fcbc21eb2ff254940ad66e3da68) |  |

---

#### Decoder 

```cpp
inline Decoder()
```

---

#### ~Decoder 

```cpp
virtual inline ~Decoder()
```

---

#### decode 

```cpp
virtual inline ssize_t decode(const char * inbuf, size_t nread, char * outbuf)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `outbuf` | `char *` |  |

---

#### finalize 

```cpp
virtual inline ssize_t finalize(char *)
```

---

#### readnext 

```cpp
inline bool readnext(const char * inbuf, size_t nread, size_t & rpos, char & c)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const char *` |  |
| `nread` | `size_t` |  |
| `rpos` | `size_t &` |  |
| `c` | `char &` |  |

---

#### nybble 

```cpp
inline int nybble(const int n)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `n` | `const int` |  |

---

#### iswspace 

```cpp
inline bool iswspace(const char c)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `c` | `const char` |  |

---

#### lastbyte 

```cpp
char lastbyte
```

## Action 

> **Defined in:** `ipc.h`

Default action type for executing synchronized callbacks.

### Members

| Name | Description |
|------|-------------|
| [`Action`](#group__base_1gad514b31a48c3cbdbbe22285058efc830) |  |
| [`target`](#group__base_1ga2f7b67072f4348a827abba8403d02ace) |  |
| [`arg`](#group__base_1ga7bfadcacf4497ab86260402b2f4129e7) |  |
| [`data`](#group__base_1gaaf3ce54c7827ef8b2958a6caa8f9fbab) |  |

---

#### Action 

```cpp
inline Action(Callback target, void * arg, const std::string & data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `target` | `Callback` |  |
| `arg` | `void *` |  |
| `data` | `const std::string &` |  |

---

#### target 

```cpp
Callback target
```

---

#### arg 

```cpp
void * arg
```

---

#### data 

```cpp
std::string data
```

## LogStream 

> **Defined in:** `logger.h`

### Members

| Name | Description |
|------|-------------|
| [`LogStream`](#group__base_1ga0b017d90cac27462656e4fed47a7a914) |  |
| [`LogStream`](#group__base_1ga234f0a55d2faddf5b97dca2ae579c23b) |  |
| [`write`](#group__base_1ga6fa8f529dcd3bad150c2f55115842917) |  |
| [`operator<<`](#group__base_1ga1cba951b82630ad2f0e5f5fe6e19c901) |  |
| [`operator<<`](#group__base_1gaf934f45be9fbd983294e2ba9e4d50ece) |  |

---

#### LogStream 

```cpp
inline LogStream(Level level, std::string realm, int line, const char * channel)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `level` | `Level` |  |
| `realm` | `std::string` |  |
| `line` | `int` |  |
| `channel` | `const char *` |  |

---

#### LogStream 

```cpp
inline LogStream(const LogStream & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const LogStream &` |  |

---

#### write 

```cpp
template<typename... Args> inline void write(Args... args)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `args` | `Args...` |  |

---

#### operator<< 

```cpp
template<typename T> inline LogStream & operator<<(const T &)
```

---

#### operator<< 

```cpp
inline LogStream & operator<<(std::ostream &(*)(std::ostream &) f)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `f` | `std::ostream &(*)(std::ostream &)` |  |

## Dispose 

> **Defined in:** `memory.h`

Deleter functor that calls dispose() on the pointer.

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__base_1ga83955b9220bdccdbaac1ca7923c3ad65) |  |

---

#### operator() 

```cpp
inline void operator()(T * ptr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `T *` |  |

## Array 

> **Defined in:** `memory.h`

Deleter functor for array pointers.

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__base_1ga2b9a2ee44aca9d8171c8188d1caa5e53) |  |

---

#### operator() 

```cpp
inline void operator()(T * ptr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `T *` |  |

## IPacketInfo 

> **Subclasses:** `icy::net::PacketInfo`
> **Defined in:** `packet.h`

An abstract interface for packet sources to provide extra information about packets.

### Members

| Name | Description |
|------|-------------|
| [`IPacketInfo`](#group__base_1ga992e069cee48907d6df5da25f484e868) |  |
| [`~IPacketInfo`](#group__base_1ga2a4f88eda361491c5ab8b2ca3f8b8199) |  |
| [`clone`](#group__base_1gaa21cf32fa208d94f1848dd747de03500) |  |

---

#### IPacketInfo 

```cpp
IPacketInfo() = default
```

---

#### ~IPacketInfo 

```cpp
virtual ~IPacketInfo() = default
```

---

#### clone 

```cpp
IPacketInfo * clone() const
```

## PacketCreationStrategy 

> **Extends:** `icy::IPacketCreationStrategy`
> **Defined in:** `packetfactory.h`

This template class implements an adapter that sits between an SignalBase and an object receiving notifications from it.

### Members

| Name | Description |
|------|-------------|
| [`PacketCreationStrategy`](#group__base_1gaf5895e8457d4d23df0a521026546ca73) |  |
| [`~PacketCreationStrategy`](#group__base_1gafad0e19b9e69c85139949c859da8ac1f) |  |
| [`create`](#group__base_1ga894eabd43c26cb9ff8930f3de6acd06d) |  |
| [`priority`](#group__base_1ga6fdf4baa67285b1c4cecea31598edd42) |  |
| [`_priority`](#group__base_1ga192649e00c46e3d9ce641e448f207ea9) |  |

---

#### PacketCreationStrategy 

```cpp
inline PacketCreationStrategy(int priority)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `priority` | `int` |  |

---

#### ~PacketCreationStrategy 

```cpp
virtual inline ~PacketCreationStrategy()
```

---

#### create 

```cpp
virtual inline IPacket * create(const ConstBuffer & buffer, size_t & nread) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `const ConstBuffer &` |  |
| `nread` | `size_t &` |  |

---

#### priority 

```cpp
virtual inline int priority() const
```

---

#### _priority 

```cpp
int _priority
```

## PacketAdapterReference 

> **Defined in:** `packetstream.h`

Provides a reference to a [PacketStreamAdapter](#classicy_1_1PacketStreamAdapter) with optional ownership.

### Members

| Name | Description |
|------|-------------|
| [`PacketAdapterReference`](#group__base_1ga91bc371499cd7aa33df43e0d0e7092c4) | Construct with raw pointer (non-owning). |
| [`PacketAdapterReference`](#group__base_1ga1e47fed55e3ab3d6a75c05a22f66205d) | Construct with shared_ptr ownership. |
| [`compareOrder`](#group__base_1gab82d5838221a0af7d6722446e3f9e49c) |  |
| [`ptr`](#group__base_1ga91ef0542de1409ffa2a4a55d735abc99) |  |
| [`_prevent_deletion`](#group__base_1ga32fa567738d15daed55ad4e7c66fd485) |  |
| [`order`](#group__base_1ga1ede9883ce7a84245fd2ed7b6428ff1d) |  |
| [`syncState`](#group__base_1ga67b82c55538ee173a9305b0dad953f3f) |  |

---

#### PacketAdapterReference 

```cpp
inline PacketAdapterReference(PacketStreamAdapter * ptr, int order, bool syncState)
```

Construct with raw pointer (non-owning).

| Parameter | Type | Description |
|-----------|------|-------------|
| `ptr` | `PacketStreamAdapter *` |  |
| `order` | `int` |  |
| `syncState` | `bool` |  |

---

#### PacketAdapterReference 

```cpp
template<class C> inline PacketAdapterReference(std::shared_ptr< C > owned, int order, bool syncState)
```

Construct with shared_ptr ownership.

| Parameter | Type | Description |
|-----------|------|-------------|
| `owned` | `std::shared_ptr< C >` |  |
| `order` | `int` |  |
| `syncState` | `bool` |  |

---

#### compareOrder 

```cpp
static inline bool compareOrder(const PacketAdapterReference::Ptr & l, const PacketAdapterReference::Ptr & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `l` | `const PacketAdapterReference::Ptr &` |  |
| `r` | `const PacketAdapterReference::Ptr &` |  |

---

#### ptr 

```cpp
PacketStreamAdapter * ptr
```

---

#### _prevent_deletion 

```cpp
std::shared_ptr< void > _prevent_deletion
```

---

#### order 

```cpp
int order
```

---

#### syncState 

```cpp
bool syncState
```

## PacketStreamState 

> **Extends:** `icy::State`
> **Defined in:** `packetstream.h`

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__base_1ga1d08d6ddfd64ec3fcb406a75a059d3a9) |  |
| [`str`](#group__base_1gaaee1b46215fbe13176a0de4c4b926cc4) |  |

---

#### Type 

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` |  |
| `Locked` |  |
| `Active` |  |
| `Paused` |  |
| `Stopping` |  |
| `Stopped` |  |
| `Closed` |  |
| `Error` |  |

---

#### str 

```cpp
inline std::string str(unsigned int id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `unsigned int` |  |

## TransactionState 

> **Extends:** `icy::State`
> **Defined in:** `packettransaction.h`

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__base_1gaf0133ba73a52562d30e404efa5935058) |  |
| [`str`](#group__base_1ga3bc0a991c145f88c468ef75cf3be51be) |  |

---

#### Type 

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `Waiting` |  |
| `Running` |  |
| `Success` |  |
| `Cancelled` |  |
| `Failed` |  |

---

#### str 

```cpp
inline std::string str(unsigned int id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `unsigned int` |  |

## Context 

> **Defined in:** `runner.h`

[Context](#structicy_1_1Runner_1_1Context) object which we send to the thread context.

This intermediate object allows us to garecefully handle late invokebacks and so avoid the need for deferred destruction of `[Runner](#classicy_1_1Runner)` objects.

### Members

| Name | Description |
|------|-------------|
| [`reset`](#group__base_1ga85d58cf01bb21e0f0280ee99f5053dfb) |  |
| [`Context`](#group__base_1ga8b65b067b317daa79442b0235f22df77) |  |
| [`tid`](#group__base_1gaa43dd023235e42c86f9aa2b5d2d3211b) |  |
| [`running`](#group__base_1ga26e1db2bf00f40be908b6facce006e4f) |  |
| [`cancelled`](#group__base_1ga66278b215dabb449abe3f1f5a5afd8cb) |  |
| [`repeating`](#group__base_1ga2a3f06bbc8956c474960af89e3d6a0b1) |  |

---

#### reset 

```cpp
inline void reset()
```

---

#### Context 

```cpp
inline Context()
```

---

#### tid 

```cpp
std::thread::id tid
```

---

#### running 

```cpp
std::atomic< bool > running
```

---

#### cancelled 

```cpp
std::atomic< bool > cancelled
```

---

#### repeating 

```cpp
bool repeating = false
```

## SharedLibrary 

> **Defined in:** `sharedlibrary.h`

### Members

| Name | Description |
|------|-------------|
| [`open`](#group__base_1ga21dfcbd8f0adda4d0bebd1a317570783) | Opens a shared library. The filename is in utf-8. Returns true on success and false on error. Call `[SharedLibrary::error()](#group__base_1gad238435837ffd636d54e7b18973c64a9)` to get the error message. |
| [`close`](#group__base_1gac139b2afe33befeb2079d60327729f26) | Closes the shared library. |
| [`sym`](#group__base_1ga8028a3c607dfd1df3f5cb3614bd00a8b) | Retrieves a data pointer from a dynamic library. It is legal for a symbol to map to nullptr. Returns 0 on success and -1 if the symbol was not found. |
| [`setError`](#group__base_1gaa72dbffb419561d3330b00086846d43b) |  |
| [`error`](#group__base_1gad238435837ffd636d54e7b18973c64a9) |  |
| [`_lib`](#group__base_1ga69563904f0fb50a897c217c372110244) |  |
| [`_error`](#group__base_1gaa920397bfdf4e812eef6fcd8d03c4455) |  |

---

#### open 

```cpp
inline bool open(const std::string & path)
```

Opens a shared library. The filename is in utf-8. Returns true on success and false on error. Call `[SharedLibrary::error()](#group__base_1gad238435837ffd636d54e7b18973c64a9)` to get the error message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### close 

```cpp
inline void close()
```

Closes the shared library.

---

#### sym 

```cpp
inline bool sym(const char * name, void ** ptr)
```

Retrieves a data pointer from a dynamic library. It is legal for a symbol to map to nullptr. Returns 0 on success and -1 if the symbol was not found.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const char *` |  |
| `ptr` | `void **` |  |

---

#### setError 

```cpp
inline void setError(const std::string & prefix)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `prefix` | `const std::string &` |  |

---

#### error 

```cpp
inline std::string error() const
```

---

#### _lib 

```cpp
uv_lib_t _lib
```

---

#### _error 

```cpp
std::string _error
```

## Deleter 

Custom deleter that can access private destructors via friendship.

### Members

| Name | Description |
|------|-------------|
| [`operator()`](#group__base_1gafef1cfec1d364ba3d428ffb409aed1e8) |  |

---

#### operator() 

```cpp
inline void operator()(S * p) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `p` | `S *` |  |

## Version 

> **Defined in:** `util.h`

### Members

| Name | Description |
|------|-------------|
| [`Version`](#group__base_1gabefe33fadbd31c287b84c33a34c0cdb4) |  |
| [`operator<`](#group__base_1gac7dbfa8df99679fc2fc161523dc89d50) |  |
| [`operator==`](#group__base_1ga11b1e27149c742ffb0b9c6b3fe2405a9) |  |
| [`major`](#group__base_1ga05e06561c82da53755f84c1a0e52282b) |  |
| [`minor`](#group__base_1ga51722ff9365a52c346ee5a2b500ef719) |  |
| [`revision`](#group__base_1ga9fd57e2361545ed40ea4c7a19240d10b) |  |
| [`build`](#group__base_1gad20634ae741953afa2129e82bfdbdd8a) |  |

---

#### Version 

```cpp
inline Version(const std::string & version)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### operator< 

```cpp
inline bool operator<(const Version & other)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `const Version &` |  |

---

#### operator== 

```cpp
inline bool operator==(const Version & other)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `other` | `const Version &` |  |

---

#### major 

```cpp
int major
```

---

#### minor 

```cpp
int minor
```

---

#### revision 

```cpp
int revision
```

---

#### build 

```cpp
int build
```

