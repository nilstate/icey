# Base

> The foundational module: event loop, signals, packet pipelines, logging, and system utilities.

**[API Reference →](../api/base.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/base/)**

## Overview

The `base` module is the substrate every other icey module builds on. It provides the libuv event loop wrappers, the signal/slot system that wires components together, the `PacketStream` pipeline that moves data through the library, handle and stream abstractions for I/O, a structured logging system, a generic state machine, binary I/O primitives, and a broad set of system utilities covering threads, the filesystem, process management, timers, and collections.

All public types live in the `icy` namespace. libuv-specific types (loops, handles) are in `icy::uv`. Filesystem utilities are in `icy::fs`. Base64 codec is in `icy::base64`. The module ships as the `icey::base` CMake target, which is a dependency of every other module in the library.

```cmake
target_link_libraries(myapp PRIVATE icey::base)
```

The key headers are pulled in transitively through each other, but the top-level entry points are:

| Subsystem | Header |
| --- | --- |
| Event loop | `icy/loop.h`, `icy/application.h` |
| Signals and slots | `icy/signal.h`, `icy/packetsignal.h` |
| Packet pipeline | `icy/packetstream.h`, `icy/packet.h`, `icy/packetqueue.h` |
| Handles and streams | `icy/handle.h`, `icy/stream.h` |
| Timers | `icy/timer.h`, `icy/idler.h`, `icy/synchronizer.h` |
| Logging | `icy/logger.h` |
| State machine | `icy/stateful.h` |
| Binary I/O | `icy/buffer.h`, `icy/byteorder.h` |
| Filesystem | `icy/filesystem.h` |
| Process management | `icy/process.h`, `icy/pipe.h` |
| Collections | `icy/collection.h` |
| Configuration | `icy/configuration.h` |
| Utilities | `icy/util.h`, `icy/numeric.h`, `icy/random.h`, `icy/hex.h` |

The `base` module's only required external dependency is libuv, which is fetched automatically by CMake. Everything else in this module is pure C++20 with standard library.

## Architecture

The module is organized around a few load-bearing abstractions that everything else references.

```
┌─────────────────────────────────────────────────────────────────┐
│                         icy::uv::Loop                           │
│               (libuv event loop; one per thread)                │
│                                                                 │
│  ┌──────────────┐  ┌──────────────┐  ┌────────────────────────┐ │
│  │ Handle<T>    │  │    Timer     │  │     Synchronizer       │ │
│  │ Stream<T>    │  │    Idler     │  │  (cross-thread wakeup) │ │
│  │ (TCP, pipe)  │  │              │  │                        │ │
│  └──────────────┘  └──────────────┘  └────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘

Signal<RT(Args...)>         — typed, priority-ordered, thread-safe pub/sub
PacketSignal                — Signal<void(IPacket&)>; the pipeline wire

┌──────────────────────────────────────────────────────┐
│                     PacketStream                     │
│                                                      │
│  [Source]──►[Processor @ order 1]──►[Processor @ 5] │
│                                           │          │
│                                    emitter (PacketSignal)       │
│                                           │          │
│                                      [Sink slots]    │
└──────────────────────────────────────────────────────┘

Stateful<T>  — state machine with transition guards and StateChange signal
Runner       — unified cancel/waitForExit interface (Thread, Timer, Idler, Synchronizer)
```

**Signal** is the connective tissue. Every event in the library; network data arriving, a stream changing state, a timer firing, a packet ready for processing; travels through a `Signal<T>`. Components attach slots, emit events, and detach when they go away.

**PacketStream** is the data plane. A source emits `IPacket` objects, an ordered chain of processors transforms them, and the final `emitter` signal delivers them to sinks. The `av` module's capture and encoder classes are sources and processors; the `webrtc` module's track sender is a sink; the `net` and `http` modules use streams internally for framing.

**Handle\<T\>** wraps every libuv handle type with thread-affinity enforcement, RAII close semantics, and a safe async destruction mechanism that prevents use-after-free in pending callbacks.

**Runner** provides a unified interface for stopping async contexts. `Thread`, `Timer`, `Idler`, and `Synchronizer` all implement `Runner`, so the `cancel()` / `waitForExit()` pattern works the same way regardless of what is running underneath.

## Usage

### Event loop

The process-wide default loop is available immediately. For most programs, this is the only loop you will ever need.

```cpp
#include "icy/loop.h"

// The process-wide default loop.
uv::Loop* loop = uv::defaultLoop();

// Run the loop until all handles are done or uv_stop is called.
uv::runLoop();               // UV_RUN_DEFAULT (blocks)
uv::runLoop(loop, UV_RUN_ONCE);   // poll once, return
uv::runLoop(loop, UV_RUN_NOWAIT); // non-blocking poll

// Stop from inside a callback.
uv::stopLoop();
```

For tests or embedded contexts that need an isolated loop, use `ScopedLoop`. It initializes and tears down the loop automatically:

```cpp
#include "icy/loop.h"

uv::ScopedLoop sl;
// sl.loop is a fresh uv_loop_t*
// Implicit conversion to Loop* makes it usable directly:
uv::runLoop(sl);
```

For longer-lived private loops, create and close them manually:

```cpp
uv::Loop* private_loop = uv::createLoop();
// ... set up handles ...
uv::closeLoop(private_loop);
delete private_loop;
```

### Application entry point

`Application` wraps loop startup, SIGINT handling, and optional command-line parsing. It is the recommended starting point for standalone programs.

```cpp
#include "icy/application.h"

int main(int argc, char* argv[])
{
    Application app;

    // Parse command-line options.
    OptionParser opts(argc, argv, "--");
    int port = opts.get<int>("port");
    if (!port) port = 8080;

    // Set up servers, connections, timers here.
    // ...

    // Block until Ctrl-C, then run the shutdown callback.
    app.waitForShutdown([](void*) {
        LInfo("Shutting down cleanly");
        uv::stopLoop();
    });

    return 0;
}
```

For code that does not need the `Application` class, the `waitForShutdown` free function installs a SIGINT handler and runs the loop:

```cpp
waitForShutdown([](void*) {
    myServer.stop();
    uv::stopLoop();
});
```

### Signals and slots

Signals are typed, priority-ordered, and thread-safe pub/sub connections. Declare them as public members of any class.

```cpp
#include "icy/signal.h"

class Server {
public:
    Signal<void(int port)>           Started;
    Signal<void(const std::string&)> Error;
    Signal<bool(const Request&)>     Request; // bool return stops propagation
};
```

**Attaching slots:**

```cpp
Server srv;

// Lambda; attach() returns the slot ID.
int id = srv.Started.attach([](int port) {
    LInfo("Listening on port ", port);
});

// Member function.
srv.Error += slot(&myObj, &MyClass::onError);

// Const member function.
srv.Error += slot(&myObj, &MyClass::onErrorConst);

// Free function.
srv.Error += slot(&onErrorFn);

// operator+= returns the ID but discards it here.
srv.Started += [](int port) { /* ... */ };
```

**Detaching slots:**

```cpp
// By slot ID.
srv.Started.detach(id);
srv.Started -= id;

// By instance pointer (removes all slots from that object).
srv.Error.detach(&myObj);
srv.Error -= &myObj;

// By slot object (exact binding match).
auto s = slot(&myObj, &MyClass::onError);
srv.Error -= s;

// Remove everything.
srv.Error.detachAll();
```

**Emitting:**

```cpp
srv.Started.emit(8080);

// bool signals stop propagation as soon as any slot returns true.
bool handled = srv.Request.emit(req);
```

**Thread safety:** `Signal<T>` uses `std::shared_mutex` internally. On `emit`, a snapshot copy of the slot list is taken under a shared lock before iteration, so slots can be added or removed from other threads mid-emission without data races.

For signals that fire exclusively on the event loop thread, use `LocalSignal<T>`. It substitutes a no-op mutex and eliminates the locking overhead entirely:

```cpp
// Only used on the loop thread; locking overhead is unnecessary.
LocalSignal<void(const char*, const int&)> Read;
```

**Priorities:** Higher priority values execute first. Pass the priority as the fourth argument to `attach()`:

```cpp
// Runs before the default-priority slot.
srv.Started.attach([](int) { /* high priority */ }, nullptr, -1, 10);
srv.Started.attach([](int) { /* default priority */ });
```

### PacketStream pipeline

`PacketStream` connects one or more sources, an ordered chain of processors, and any number of downstream sinks. Every data-producing subsystem in icey plugs into this abstraction.

```
Source ──► [Processor @ order 1] ──► [Processor @ order 5] ──► emitter ──► sinks
```

**Packet types:**

All packets implement `IPacket`. The two concrete types in `base` are:

- `RawPacket` — a byte buffer with optional ownership. Constructing with `char*` borrows the pointer (zero-copy); constructing with `const char*` copies into owned storage.
- `FlagPacket` — carries only bitwise flags, no payload. Used for control signals such as end-of-stream.

Subtype your own packets for protocol-specific data:

```cpp
#include "icy/packet.h"

struct VideoFrame : public IPacket {
    int width, height;
    std::vector<uint8_t> planes[3];
    int64_t pts;

    std::unique_ptr<IPacket> clone() const override {
        return std::make_unique<VideoFrame>(*this);
    }

    ssize_t read(const ConstBuffer&) override { return 0; }
    void write(Buffer&) const override {}
    const char* className() const override { return "VideoFrame"; }
};
```

The `rawPacket()` factory functions create `RawPacket` values from buffers and raw pointers without heap allocation:

```cpp
char buf[1024];
size_t n = fillBuffer(buf, sizeof(buf));

// Borrowed (zero-copy): caller keeps buf alive until the stream crosses a
// Cloned/Retained boundary, or until write()/emit() returns in a fully
// synchronous graph.
auto pkt = rawPacket(buf, n);

// Owned copy: safe to discard buf after this line.
auto owned = rawPacket(static_cast<const char*>(buf), n);
```

**Retention boundary rule:**

- `PacketRetention::Borrowed` means the adapter only touches the packet during the current synchronous call chain.
- `PacketRetention::Cloned` and `PacketRetention::Retained` are explicit ownership boundaries.
- `SyncPacketQueue` and `AsyncPacketQueue` are clone boundaries: upstream code may reuse or free borrowed packet storage after the packet enters one of those queues.

**Writing sources:**

A source is a `PacketStreamAdapter` that holds a `PacketSignal` and calls `emit()` when data is ready. The stream listens to that signal.

```cpp
#include "icy/packetstream.h"

class CameraCapture : public PacketStreamAdapter {
public:
    PacketSignal _emitter;

    CameraCapture() : PacketStreamAdapter(_emitter) {}

    // Called by your capture loop when a frame arrives.
    void onFrame(char* data, size_t len) {
        emit(data, len); // delegates to _emitter
    }

    // Called by the stream when its state changes.
    void onStreamStateChange(const PacketStreamState& s) override {
        if (s.id() == PacketStreamState::Stopped)
            stopCapture();
        else if (s.id() == PacketStreamState::Active)
            startCapture();
    }
};
```

`syncState = true` causes the stream to call `start()` and `stop()` on the source when its own state changes, provided the source implements `basic::Startable`.

**Writing processors:**

Processors implement `PacketProcessor` and override `process()`. They run in ascending order value.

```cpp
class Transcoder : public PacketProcessor {
public:
    PacketSignal _emitter;

    Transcoder() : PacketProcessor(_emitter) {}

    // Return false to reject packets of the wrong type.
    bool accepts(IPacket* pkt) override {
        return dynamic_cast<VideoFrame*>(pkt) != nullptr;
    }

    void process(IPacket& packet) override {
        auto& frame = static_cast<VideoFrame&>(packet);
        Buffer encoded = encode(frame);
        emit(encoded.data(), encoded.size()); // push downstream
    }
};
```

**Assembling a pipeline:**

```cpp
#include "icy/packetstream.h"

PacketStream stream("video");

// Source: owns the adapter, syncs start/stop.
stream.attachSource(new CameraCapture, /*owned=*/true, /*syncState=*/true);

// Processor at order 5: encoder.
stream.attach(new Transcoder, 5);

// Processor at order 10: packetizer.
stream.attach(new RTPPacketizer, 10);

// Sink: a slot on the emitter signal.
stream.emitter += packetSlot(&trackSender, &WebRtcTrackSender::sendPacket);

// Error and close callbacks.
stream.Error += [](PacketStream& s, const std::exception_ptr& ep) {
    try { std::rethrow_exception(ep); }
    catch (std::exception& e) { LError("Stream error: ", e.what()); }
};
stream.Close += [](PacketStream&) {
    LInfo("Stream closed");
};

stream.start();
// ...
stream.stop();
stream.close();
```

**Using a shared_ptr source:**

```cpp
auto capture = std::make_shared<CameraCapture>();
stream.attachSource(capture, /*syncState=*/true);
// stream shares ownership; capture stays alive until teardown
```

**Chaining streams:**

The `emitter` of one stream can feed directly into another:

```cpp
PacketStream upstream, downstream;
downstream.attachSource(upstream.emitter);
```

**Synchronizing output to the event loop:**

When a source runs on a background thread but sinks must run on the libuv event loop (the common case for network sends), call `synchronizeOutput()` before `start()`. This inserts a `SyncPacketQueue` at order 101. That queue is both the thread hop and the explicit clone boundary for borrowed packets:

```cpp
stream.synchronizeOutput(uv::defaultLoop());
stream.start();
// stream.emitter now always fires on the loop thread
```

Use `AsyncPacketQueue` the same way when you need a worker-thread boundary before expensive processing. The queue clones first, then defers.

**PacketStream lifecycle states:**

```
None ──[start()]──► Active ──[pause()]──► Paused ──[resume()]──► Active
                         └──[stop()]──► Stopping ──► Stopped
                         └──[error]──► Error ──[closeOnError=true]──► Closed
```

```cpp
stream.autoStart(true);     // transition to Active on first incoming packet
stream.closeOnError(false); // stay in Error state instead of auto-closing
bool ok = stream.active();
bool done = stream.stopped();
bool gone = stream.closed();
```

**Type-filtered packet slots:**

`packetSlot()` creates a slot that `dynamic_cast`s each incoming `IPacket` before invoking the callback. Non-matching packets are silently ignored:

```cpp
#include "icy/packetsignal.h"

// Only fires when the IPacket is actually a VideoFrame.
stream.emitter += packetSlot(this, &MyClass::onVideoFrame);

void MyClass::onVideoFrame(VideoFrame& frame) {
    // called only for VideoFrame packets
}
```

**Packet queues:**

For more explicit threading control, two queue adapters are available from `icy/packetqueue.h`:

- `SyncPacketQueue<T>` — clones incoming packets and dispatches them on a libuv event loop. Used internally by `synchronizeOutput()`.
- `AsyncPacketQueue<T>` — clones incoming packets and dispatches them on a dedicated background thread. Attach at order 0 to move all downstream processing off the source thread.

```cpp
#include "icy/packetqueue.h"

// Offload processing to a worker thread.
stream.attach(new AsyncPacketQueue<RawPacket>, 0);
stream.attach(new HeavyProcessor, 5);
```

### Handles and streams

`uv::Handle<T>` is the base for every libuv handle type. It enforces thread affinity (every method asserts the calling thread matches the construction thread), manages the async close lifecycle, and provides `init()`, `invoke()`, and `invokeOrThrow()` for calling libuv functions safely.

```cpp
#include "icy/handle.h"

// Bare timer handle example (prefer the Timer class for real use).
uv::Handle<uv_timer_t> h;
h.init(&uv_timer_init);
h.invoke(&uv_timer_start, h.get(), myCallback, 1000, 0);

// Ref/unref control loop exit.
h.ref();   // prevent loop exit while this handle is alive
h.unref(); // allow loop to exit even with this handle alive

h.close(); // schedules async uv_close; onClose() fires after completion
```

The internal `Context<T>` object owns the raw `uv_handle_t*` and clears its `data` pointer before calling `uv_close`, preventing use-after-free in any pending write-completion callbacks.

`Stream<T>` extends `Handle<T>` for stream-oriented handles (TCP sockets, pipes). It maintains a 64 KiB read buffer, a freelist for `uv_write_t` allocation, and a 16 MB write-queue high-water mark:

```cpp
#include "icy/stream.h"

// The net module's TCPSocket is a Stream<uv_tcp_t>.
// Use it like:
mySocket.Read += [](const char* data, const int& len) {
    // consume len bytes from data; called on the loop thread
};

// Non-blocking write. Returns false if the high-water mark is reached
// or if reads have not started yet.
bool queued = mySocket.write(buf, len);

// Half-close (TCP FIN); remote reads will get EOF.
mySocket.shutdown();

// Adjust the backpressure threshold (default 16 MB).
mySocket.setHighWaterMark(4 * 1024 * 1024);
```

The `net` module's `TCPSocket` and `SSLSocket` subclass `Stream<T>`, while `UDPSocket` extends `Handle<uv_udp_t>` directly.

### Timer

`Timer` wraps `uv_timer_t`. It implements `Runner`, so it participates in the standard `cancel()` / `waitForExit()` lifecycle.

```cpp
#include "icy/timer.h"

// One-shot: fires once after 500 ms.
Timer oneshot(500, uv::defaultLoop(), []{ LInfo("fired once"); });

// Repeating: fires every 1000 ms with no initial delay.
Timer tick(0, 1000);
tick.Timeout += []{ LInfo("tick"); };
tick.start();

// Change the interval while running (takes effect on the next fire).
tick.setInterval(2000);

// Reset the countdown without stopping.
tick.again();

// Query the timer.
LInfo("count=", tick.count(), " interval=", tick.interval(), "ms");

tick.stop();
```

For a one-shot timer with a callback, the constructor that takes a `std::function` starts immediately:

```cpp
Timer t(500, uv::defaultLoop(), []{ LInfo("500ms elapsed"); });
// t is running; no explicit start() needed
```

### Idler

`Idler` fires a callback on every event loop idle iteration. Use it for background work that must not block I/O. The handle is unreffed by default so it does not prevent the loop from exiting.

```cpp
#include "icy/idler.h"

Idler idler([]{
    if (hasPendingWork())
        doNextWorkItem();
});

// Stop it:
idler.cancel();
```

### Synchronizer

`Synchronizer` allows any thread to wake the event loop and invoke a callback on the loop thread. It wraps `uv_async_t`. Multiple `post()` calls may coalesce into a single callback invocation.

```cpp
#include "icy/synchronizer.h"

Synchronizer sync([]{
    // Always runs on the event loop thread.
    drainIncomingQueue();
}, uv::defaultLoop());

// From any background thread:
sync.post(); // wakeup; next loop iteration calls the callback
```

### Runner

`Runner` is the unified abstract interface for cancellable async contexts. `Thread`, `Timer`, `Idler`, and `Synchronizer` all implement it.

```cpp
#include "icy/runner.h"

// Thread usage:
Thread worker([]{
    while (!uv::defaultLoop()) // just an example
        processWork();
});

// Universal control:
worker.cancel();
bool clean = worker.waitForExit(3000); // wait up to 3s
```

`Runner` exposes:

| Method | Description |
| --- | --- |
| `start(func)` | Start the async context with `func` as the target |
| `cancel()` | Signal the context to stop at the earliest opportunity |
| `cancelled()` | Returns true if `cancel()` has been called |
| `running()` | Returns true if the context is currently active |
| `repeating()` | Returns true if the target runs in a loop |
| `setRepeating(flag)` | Enable/disable repeat mode; must be called before `start()` |
| `waitForExit(ms)` | Block until the context exits or the timeout elapses |
| `async()` | Returns true for thread-backed runners, false for event-loop runners |

### Logging

The logging system is built around named channels and a global `Logger` singleton. All log output is gated by the `ICY_ENABLE_LOGGING` compile definition; when it is absent every log call compiles to nothing with zero runtime cost.

**Setting up channels:**

```cpp
#include "icy/logger.h"

auto& logger = Logger::instance();

// Console at Debug level (default timestamp format HH:MM:SS).
logger.add(std::make_unique<ConsoleChannel>("console", Level::Debug));
logger.setDefault("console");

// File channel at Info level.
logger.add(std::make_unique<FileChannel>(
    "file", "/var/log/myapp.log", Level::Info));

// Time-rotating channel: new file every 6 hours.
logger.add(std::make_unique<RotatingFileChannel>(
    "rotating", "/var/log/myapp/", Level::Warn,
    "log",        // file extension
    6 * 3600));   // rotation interval in seconds
```

**Log levels** (ascending severity): `Trace`, `Debug`, `Info`, `Warn`, `Error`, `Fatal`. Messages below the channel's configured level are dropped without formatting.

**Logging macros:**

Two families of macros are provided. The `L*` macros accept variadic arguments and flush immediately. The `S*` macros return a `LogStream` that accepts `<<` operators:

```cpp
LTrace("entering: ", __func__);
LDebug("packet size: ", pkt.size(), " bytes");
LInfo("server started on port ", port);
LWarn("write queue full (", qsize, " bytes)");
LError("connection failed: ", err.message);

// Stream style for complex formatting:
SInfo << "processed " << count << " frames in " << elapsed << "ms";
SDebug << "packet flags: 0x" << std::hex << pkt.flags;
```

**Async log writer:**

For applications where log-formatting latency matters, swap in the async writer:

```cpp
logger.setWriter(std::make_unique<AsyncLogWriter>());
```

**Realm filter:**

Each channel can filter messages by realm (the source filename by default). Only messages whose realm contains the filter string are written:

```cpp
auto ch = std::make_unique<ConsoleChannel>("net-debug", Level::Trace);
ch->setFilter("tcp"); // only messages from files with "tcp" in the path
logger.add(std::move(ch));
```

**Converting between level strings and enums:**

```cpp
Level lvl = getLevelFromString("warn"); // Level::Warn
const char* name = getStringFromLevel(Level::Error); // "error"
```

### State machine

`Stateful<T>` is a template that adds a typed state object, transition guards, and a `StateChange` signal to any class. `PacketStream` is the primary consumer, but it applies to any object with discrete lifecycle states.

Define a state type that inherits `State`:

```cpp
#include "icy/stateful.h"

struct ConnectionState : public State {
    static constexpr unsigned int Disconnected = 0;
    static constexpr unsigned int Connecting   = 1;
    static constexpr unsigned int Connected    = 2;
    static constexpr unsigned int Closing      = 3;

    std::string str(unsigned int id) const override {
        switch (id) {
            case Disconnected: return "Disconnected";
            case Connecting:   return "Connecting";
            case Connected:    return "Connected";
            case Closing:      return "Closing";
        }
        return "undefined";
    }
};

class Connection : public Stateful<ConnectionState> {
public:
    void connect(const std::string& host) {
        setState(this, ConnectionState::Connecting);
        // ... initiate connection ...
    }

    void onConnected() {
        setState(this, ConnectionState::Connected);
    }

protected:
    // Return false to veto a transition.
    bool beforeStateChange(const ConnectionState& next) override {
        if (state().id() == ConnectionState::Closing &&
            next.id() == ConnectionState::Connected)
            return false; // can't reconnect while closing
        return true;
    }

    void onStateChange(ConnectionState& s, const ConnectionState& old) override {
        LDebug("Connection: ", old, " -> ", s);
    }
};
```

Observe state changes from outside the class:

```cpp
Connection conn;
conn.StateChange += [](void* sender, ConnectionState& s, const ConnectionState& old) {
    LInfo("Connection state changed: ", old.toString(), " -> ", s.toString());
};

// Query current state.
conn.stateEquals(ConnectionState::Connected);
conn.stateBetween(ConnectionState::Connecting, ConnectionState::Connected);
conn.state().toString(); // "Connected"
```

### Buffers and binary I/O

`MutableBuffer` and `ConstBuffer` are non-owning view types analogous to `std::span`. They reference existing memory without copying it.

```cpp
#include "icy/buffer.h"

std::string data = "hello world";
MutableBuffer mb = mutableBuffer(data);
ConstBuffer   cb = constBuffer(data);

// Cast to typed pointer.
char*       p  = bufferCast<char*>(mb);
const char* cp = bufferCast<const char*>(cb);

// Get as string (copies).
std::string s = cb.str();

// Buffer is just std::vector<char>.
Buffer owned;
owned.resize(1024);
```

`mutableBuffer()` and `constBuffer()` are overloaded for `std::string`, `std::vector<T>`, `Buffer`, and raw pointer+size pairs.

**BitReader** — sequential binary reader with byte-order conversion:

```cpp
Buffer raw = receiveNetworkPacket();
BitReader r(raw, ByteOrder::Network);

uint8_t  version;
uint16_t length;
uint32_t timestamp;

r.getU8(version);
r.getU16(length);
r.getU32(timestamp);

std::string payload;
r.get(payload, length);

// Peek without advancing.
uint8_t nextByte = r.peekU8();

// String-oriented helpers.
std::string word, line;
r.readNextWord(word);
r.readLine(line);

// Navigation.
r.skip(4);
r.seek(0);
size_t remaining = r.available();
```

**BitWriter** — sequential binary writer. The fixed-capacity form throws `std::out_of_range` when exhausted. Use `DynamicBitWriter` with a `Buffer&` for auto-resizing:

```cpp
Buffer out;
DynamicBitWriter w(out, ByteOrder::Network);

w.putU8(0x01);

// Write a length prefix we will fill in later.
size_t lengthPos = w.position();
w.putU16(0); // placeholder

w.putU32(timestamp);
w.put(payload.data(), payload.size());

// Patch the length field now that we know it.
uint16_t bodyLen = static_cast<uint16_t>(w.position() - lengthPos - 2);
w.updateU16(bodyLen, lengthPos);
```

### Filesystem utilities

```cpp
#include "icy/filesystem.h"

// Path components.
fs::filename("/usr/local/bin/icey"); // "icey"
fs::basename("/usr/local/bin/icey"); // "icey"
fs::dirname("/usr/local/bin/icey");  // "/usr/local/bin/"
fs::extname("archive.tar.gz");       // ".gz"
fs::extname("archive.tar.gz", false); // "gz"

// Existence checks.
bool present = fs::exists("/tmp/output.log");
bool isDir   = fs::isdir("/var/log");
int64_t sz   = fs::filesize("/etc/hosts");

// Directory operations.
fs::mkdirr("/var/log/myapp/2026/03"); // creates all missing parents
std::vector<std::string> entries;
fs::readdir("/var/log", entries);

// Path construction.
std::string path = fs::makePath("/var/log", "myapp.log");
fs::addnode(path, "extra");           // appends with separator
std::string norm = fs::normalize("./foo/../bar");

// Write to file (creates or overwrites).
fs::savefile("/tmp/out.bin", data.data(), data.size(), /*whiny=*/true);

// Rename or move.
fs::rename("/tmp/a.tmp", "/tmp/a.dat");

// Delete.
fs::unlink("/tmp/a.dat");
fs::rmdir("/tmp/emptydir");

// Platform separator.
std::string joined = std::string("/var/log") + fs::separator + "myapp.log";
```

### Process management

```cpp
#include "icy/process.h"

Process ffmpeg;
ffmpeg.file = "/usr/bin/ffmpeg";
ffmpeg.args = {"-i", "input.mp4", "-c:v", "libx264",
               "-preset", "fast", "output.mkv"};
ffmpeg.cwd  = "/tmp/scratch";

// Stdout and stderr arrive line-by-line.
ffmpeg.onstdout = [](std::string line) { LDebug("ffmpeg: ", line); };
ffmpeg.onstderr = [](std::string line) { LWarn("ffmpeg stderr: ", line); };
ffmpeg.onexit   = [](std::int64_t code) {
    LInfo("ffmpeg exited with code ", code);
};

ffmpeg.spawn();

// Write to stdin.
std::string cmd = "q\n";
ffmpeg.in().write(cmd.data(), cmd.size());

// Send a signal.
ffmpeg.kill(SIGTERM);
```

`Process::in()`, `out()`, and `err()` return `Pipe` handles, which are `Stream<uv_pipe_t>` instances. You can attach slots to their `Read` signals and call `write()` on them directly.

### Collections

Three collection templates cover the common patterns. All live in `icy/collection.h`.

**`PointerCollection<TKey, TValue>`** — thread-safe `unique_ptr` map. Operations use `std::shared_mutex` (shared for reads, exclusive for writes):

```cpp
#include "icy/collection.h"

PointerCollection<std::string, Session> sessions;

sessions.add("abc123", new Session("abc123"));
Session* s = sessions.get("abc123");
sessions.free("abc123");     // destroys the session
Session* raw = sessions.remove("xyz"); // transfers ownership to caller

bool present = sessions.exists("abc123");
size_t count = sessions.size();
sessions.clear();
```

**`LiveCollection<TKey, TValue>`** — extends `PointerCollection` with observable `ItemAdded` and `ItemRemoved` signals:

```cpp
LiveCollection<std::string, Session> sessions;

sessions.ItemAdded   += [](Session& s) { LInfo("Session added: ", s.id()); };
sessions.ItemRemoved += [](const Session& s) { LInfo("Session removed"); };

sessions.add("abc123", new Session("abc123")); // fires ItemAdded
sessions.free("abc123");                        // fires ItemRemoved
```

**`KVCollection<TKey, TValue>`** — value-type map (not pointer-based), no locking. Suitable for string-to-string config stores and similar small maps:

```cpp
KVCollection<std::string, std::string> opts;
opts.add("host", "0.0.0.0");
opts.add("port", "8080");

std::string host = opts.get("host");
bool has = opts.has("port");
opts.remove("port");
```

**`NVCollection`** — case-insensitive, multi-valued name-value store backed by a `vector<pair<string,string>>`. The `http` module uses it for HTTP headers:

```cpp
NVCollection headers;
headers.set("Content-Type", "application/json");
headers.add("X-Custom-Header", "value1");
headers.add("X-Custom-Header", "value2"); // multiple values for same name

const std::string& ct = headers["content-type"]; // case-insensitive lookup
bool has = headers.has("X-Custom-Header");
```

**`TimedManager<TKey, TValue>`** — extends `PointerCollection` with automatic expiry. Items are deleted after a configurable timeout:

```cpp
#include "icy/timedmanager.h"

TimedManager<std::string, Token> tokens;

// Add a token that expires after 30 seconds.
tokens.add("tok_abc", new Token("tok_abc"), 30000);

// Reset the expiry for an existing item.
tokens.expires("tok_abc", 60000);

// Override onTimeout to customize expiry behaviour.
```

### Configuration

`Configuration` is an abstract key-value store with typed accessors and change signals. Subclass it and implement `getRaw()` / `setRaw()` to back it with any storage:

```cpp
#include "icy/configuration.h"

class JsonConfiguration : public Configuration {
protected:
    bool getRaw(const std::string& key, std::string& value) const override {
        // look up key in _json
    }
    void setRaw(const std::string& key, const std::string& value) override {
        // store in _json; emit PropertyChanged
        PropertyChanged.emit(key, value);
    }
};

JsonConfiguration config;
config.PropertyChanged += [](const std::string& key, const std::string& val) {
    LDebug("Config changed: ", key, " = ", val);
};

config.setString("server.host", "0.0.0.0");
config.setInt("server.port", 8080);
config.setBool("server.tls", true);

std::string host = config.getString("server.host");
int port         = config.getInt("server.port", 80); // with default
```

`ScopedConfiguration` provides a two-level fallback — a module-specific scope tried first, a global default scope as a fallback — useful for per-channel configuration with global defaults:

```cpp
ScopedConfiguration scoped(config,
    "channels.video",  // current scope
    "defaults");       // fallback scope

int bitrate = scoped.getInt("bitrate", 2000000);
// checks "channels.video.bitrate" first, then "defaults.bitrate"
```

### Rate limiting

`RateLimiter` is a token bucket that throttles event frequency. Useful for preventing signal storms over noisy network connections:

```cpp
#include "icy/ratelimiter.h"

// Allow at most 5 messages per 6 seconds.
RateLimiter limiter(5.0, 6.0);

void onIncomingMessage(const Message& msg) {
    if (!limiter.canSend()) {
        LWarn("Rate limit exceeded, dropping message");
        return;
    }
    processMessage(msg);
}
```

### Base64 codec

```cpp
#include "icy/base64.h"

// Encode a vector of bytes.
std::vector<uint8_t> raw = {0x01, 0x02, 0x03};
std::string encoded = base64::encode(raw); // no line wrapping
std::string wrapped = base64::encode(raw, 76); // wrap at 76 chars

// Decode back.
std::string decoded = base64::decode(encoded);

// Streaming encoder.
base64::Encoder enc;
std::istringstream in("hello world");
std::ostringstream out;
enc.encode(in, out);
std::string b64 = out.str();
```

### Shared library loading

`SharedLibrary` wraps libuv's `uv_lib_t` for runtime shared library loading and symbol resolution. The `graft` module builds its runtime loader on top of this:

```cpp
#include "icy/sharedlibrary.h"

SharedLibrary lib;
if (!lib.open("/usr/local/lib/myplugin.so")) {
    LError("Cannot load plugin: ", lib.error());
    return;
}

using FactoryFn = void*(*)();
FactoryFn factory = nullptr;
if (!lib.sym("createPlugin", reinterpret_cast<void**>(&factory))) {
    LError("Symbol not found: ", lib.error());
    return;
}

auto plugin = factory();
// ...
lib.close();
```

### Interfaces

`icy/interface.h` defines four abstract interfaces that appear throughout the codebase:

| Interface | Methods | Use |
| --- | --- | --- |
| `basic::Startable` | `start()`, `stop()` | Anything the stream can sync-start |
| `basic::Runnable` | `run()`, `cancel()`, `cancelled()` | Thread-runnable objects |
| `basic::Encoder` | `encode()`, `finalize()` | Stream encoder (base64, codec wrappers) |
| `basic::Decoder` | `decode()`, `finalize()` | Stream decoder |

### Error handling

`Error` is a simple struct carrying an error code, message string, and optional `std::exception_ptr`:

```cpp
#include "icy/error.h"

Error err;
err.err = UV_ECONNREFUSED;
err.message = uv::formatError("Connection failed", UV_ECONNREFUSED);

if (err.any()) {
    LError(err.message);
    err.rethrow(); // re-throws stored exception_ptr if set
}

// Handle-level errors (set automatically by UV callbacks):
myHandle.setUVError(UV_ENOENT, "File open failed");
myHandle.throwLastError("File open failed");
```

### Timeout and TimedToken

`Timeout` is a simple wall-clock countdown used internally by `TimedManager`:

```cpp
#include "icy/timeout.h"

Timeout t(5000); // 5 second countdown
t.start();

// ... later ...
if (t.expired()) {
    LInfo("timed out; ", t.remaining(), "ms remaining was 0");
}
t.reset(); // restart the countdown
```

`TimedToken` pairs a `Timeout` with a randomly generated ID string. Used for short-lived authentication or session tokens:

```cpp
TimedToken token(30000); // 30s lifetime, auto-generated ID
LInfo("token: ", token.id());

if (token.expired())
    LWarn("token expired");
```

## Configuration

The `base` module has no required external dependencies beyond libuv, which CMake fetches automatically. The optional compile-time flags that affect `base` behaviour are:

| CMake flag / Definition | Effect |
| --- | --- |
| `ICY_ENABLE_LOGGING` | Enables all logging macros. Without this, every `LDebug`, `LInfo`, etc. compiles to nothing. Defined automatically when `-DENABLE_LOGGING=ON` (the default). |
| `ICY_EXCEPTION_RECOVERY` | Wraps `Runner::runAsync` and `Stream::handleRead` in try/catch. Prevents uncaught exceptions from crashing the process at the cost of hiding bugs. Off by default. |
| `ICY_SHARED_LIBRARY` | Activates `__declspec(dllexport/dllimport)` on Windows when building `base` as a DLL. |
| `ICY_WIN` | Detected automatically; enables Windows-specific code paths in filesystem, process, and socket handling. |
| `ICY_UNICODE` | Enables `fs::transcode()` on Windows for wide-to-narrow path conversion. |

To link `icey::base` from a parent CMake project using FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(icey
  GIT_REPOSITORY https://github.com/nilstate/icey.git
  GIT_TAG 2.4.3
)
FetchContent_MakeAvailable(icey)
target_link_libraries(myapp PRIVATE icey::base)
```

After installing with `cmake --install build`:

```cmake
find_package(icey REQUIRED)
target_link_libraries(myapp PRIVATE icey::base)
```

## See Also

- [Net](net.md) — TCP, SSL/TLS, and UDP sockets; `TCPSocket` and `SSLSocket` subclass `Stream<T>`, `UDPSocket` extends `Handle<T>`.
- [HTTP](http.md) — HTTP server and client; uses `PacketStream` internally for request/response framing and body streaming.
- [AV](av.md) — FFmpeg capture and codec classes; implement `PacketStreamAdapter` and `PacketProcessor` to plug into any `PacketStream`.
- [WebRTC](webrtc.md) — `WebRtcTrackSender` and `WebRtcTrackReceiver` are sinks and sources that connect directly to `PacketStream`.
- [Symple](symple.md) — Real-time messaging; uses the signal/slot system throughout for event dispatch.
- [Sched](sched.md) — Task scheduler; builds on `Thread` and `Runner` for deferred and periodic job execution.
- [Graft](graft.md) — Native plugin ABI and runtime loading; uses `SharedLibrary` for shared-library opening and symbol lookup.

## Best Next Reads

- [Runtime Contracts](../concepts/runtime-contracts.md) for the loop, signal, and shutdown rules that sit underneath `base`
- [PacketStream](../concepts/packetstream.md) for the pipeline and ownership model built on top of `Signal` and `IPacket`
- [Net](net.md) if you want to see those contracts applied directly to sockets and transport buffers
