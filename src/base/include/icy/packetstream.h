///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/error.h"
#include "icy/interface.h"
#include "icy/logger.h"
#include "icy/loop.h"
#include "icy/packetsignal.h"
#include "icy/stateful.h"
#include <cassert>
#include <cstdint>
#include <memory>


namespace icy {


struct PacketStreamState;
enum class PacketRetention;


//
// Packet Stream Adapter
//


/// This class is a wrapper for integrating external
/// classes with the a PacketStream's data flow and
/// state machine.
class Base_API PacketStreamAdapter
{
public:
    /// Construct the adapter, binding it to the given packet signal.
    /// @param emitter The outgoing packet signal owned by the subclass.
    PacketStreamAdapter(PacketSignal& emitter);
    virtual ~PacketStreamAdapter() {};

    /// Emit a mutable raw buffer as a packet.
    /// @param data  Pointer to the buffer (not copied; caller retains ownership).
    /// @param len   Number of bytes in the buffer.
    /// @param flags Optional packet flags (see PacketFlags).
    virtual void emit(char* data, size_t len, unsigned flags = 0);

    /// Emit a read-only raw buffer as a packet (data is copied internally).
    /// @param data  Pointer to the buffer.
    /// @param len   Number of bytes in the buffer.
    /// @param flags Optional packet flags (see PacketFlags).
    virtual void emit(const char* data, size_t len, unsigned flags = 0);

    /// Emit a string as a packet (data is copied internally).
    /// @param str   String payload.
    /// @param flags Optional packet flags (see PacketFlags).
    virtual void emit(const std::string& str, unsigned flags = 0);

    /// Emit a flag-only packet carrying no payload data.
    /// @param flags Packet flags to embed in the emitted FlagPacket.
    virtual void emit(unsigned flags = 0);

    /// Emit an existing packet directly onto the outgoing signal.
    /// @param packet The packet to forward; must remain valid for the duration of the call.
    virtual void emit(IPacket& packet);

    /// Returns a reference to the outgoing packet signal.
    PacketSignal& getEmitter();

    /// Returns how this adapter treats incoming packet lifetime.
    /// Most adapters are synchronous and therefore only borrow the packet for
    /// the current call chain. Queue-style adapters override this to advertise
    /// that they clone before deferred use. Callers may treat the first adapter
    /// reporting Cloned or Retained as the explicit ownership boundary in the
    /// stream graph.
    [[nodiscard]] virtual PacketRetention retention() const;

    /// Called by the PacketStream to notify when the internal
    /// Stream state changes.
    /// On receiving the Stopped state, it is the responsibility
    /// of the adapter to have ceased all outgoing packet transmission,
    /// especially in multi-thread scenarios.
    virtual void onStreamStateChange(const PacketStreamState&) {};

protected:
    /// NonCopyable and NonMovable
    PacketStreamAdapter(const PacketStreamAdapter&) = delete;
    PacketStreamAdapter& operator=(const PacketStreamAdapter&) = delete;
    PacketStreamAdapter(PacketStreamAdapter&&) = delete;
    PacketStreamAdapter& operator=(PacketStreamAdapter&&) = delete;

    PacketSignal& _emitter;
};


using PacketSource = PacketStreamAdapter; ///< For 0.8.x compatibility


//
// PacketProcessor
//

/// This class is a virtual interface for creating
/// PacketStreamAdapters which process that and emit
/// the IPacket type.
class Base_API PacketProcessor : public PacketStreamAdapter
{
public:
    PacketProcessor(PacketSignal& emitter)
        : PacketStreamAdapter(emitter)
    {
    }

    /// This method performs processing on the given
    /// packet and emits the result.
    ///
    /// Processors that defer work asynchronously must either clone the packet
    /// or retain an owned equivalent. See retention().
    virtual void process(IPacket& packet) = 0;

    /// This method ensures compatibility with the given
    /// packet type. Return false to reject the packet.
    virtual bool accepts(IPacket*) { return true; };

    /// Stream operator alias for process()
    virtual void operator<<(IPacket& packet) { process(packet); };
};


/// Compatibility alias for a packet processor that packetizes stream output.
using IPacketizer = PacketProcessor;
using IDepacketizer = PacketProcessor; ///< For 0.8.x compatibility


//
// Packet Adapter Reference
//

/// Provides a reference to a PacketStreamAdapter with optional ownership.
struct PacketAdapterReference
{
    using Ptr = std::shared_ptr<PacketAdapterReference>;

    PacketStreamAdapter* ptr;                // non-owning view
    std::shared_ptr<void> _prevent_deletion; // prevent premature deletion via type erasure
    int order;
    bool syncState;

    /// Construct with raw pointer (non-owning).
    PacketAdapterReference(PacketStreamAdapter* ptr = nullptr,
                           int order = 0, bool syncState = false)
        : ptr(ptr)
        , order(order)
        , syncState(syncState)
    {
    }

    /// Construct with shared_ptr ownership.
    template <class C>
    PacketAdapterReference(std::shared_ptr<C> owned, int order = 0,
                           bool syncState = false)
        : ptr(static_cast<PacketStreamAdapter*>(owned.get()))
        , _prevent_deletion(std::move(owned))
        , order(order)
        , syncState(syncState)
    {
    }

    /// Comparator for sorting references by ascending order value.
    /// @param l Left-hand reference.
    /// @param r Right-hand reference.
    /// @return true if @p l should appear before @p r in the processor chain.
    static bool compareOrder(const PacketAdapterReference::Ptr& l,
                             const PacketAdapterReference::Ptr& r)
    {
        return l->order < r->order;
    }
};


/// Ordered list of packet adapter references used for sources and processors.
using PacketAdapterVec = std::vector<PacketAdapterReference::Ptr>;


/// Describes how an adapter treats incoming packet lifetime beyond the
/// current synchronous call chain.
///
/// Borrowed adapters must finish using the packet before emit()/process()
/// returns. Cloned and Retained adapters are explicit ownership boundaries:
/// callers may mutate or free borrowed input only after such a boundary, or
/// after the whole synchronous write/emit call has returned.
enum class PacketRetention
{
    Borrowed,  ///< Packet is only used synchronously during the current emit/process call.
    Cloned,    ///< Adapter makes its own copy before deferred or asynchronous use.
    Retained   ///< Adapter retains the original packet object beyond the current call chain.
};


/// Flags which determine how the packet is handled by the PacketStream
enum class PacketFlags : unsigned
{
    NoModify = 0x01, ///< The packet should not be modified by processors.
    Final = 0x02     ///< The final packet in the stream.
};

constexpr unsigned operator|(PacketFlags lhs, PacketFlags rhs)
{
    return static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs);
}


//
// Packet Stream State
//


/// State machine states for PacketStream
struct PacketStreamState : public State
{
    enum class Type : unsigned int
    {
        None = 0,
        Locked,
        Active,
        Paused,
        // Resetting,
        Stopping,
        Stopped,
        Closed,
        Error,
    };

    // Re-export enum values into the struct scope for backward compatibility.
    // These are unsigned int constants so they work with State::ID (uint32_t).
    static constexpr unsigned int None = static_cast<unsigned int>(Type::None);
    static constexpr unsigned int Locked = static_cast<unsigned int>(Type::Locked);
    static constexpr unsigned int Active = static_cast<unsigned int>(Type::Active);
    static constexpr unsigned int Paused = static_cast<unsigned int>(Type::Paused);
    static constexpr unsigned int Stopping = static_cast<unsigned int>(Type::Stopping);
    static constexpr unsigned int Stopped = static_cast<unsigned int>(Type::Stopped);
    static constexpr unsigned int Closed = static_cast<unsigned int>(Type::Closed);
    static constexpr unsigned int Error = static_cast<unsigned int>(Type::Error);

    std::string str(unsigned int id) const override
    {
        switch (static_cast<Type>(id)) {
            case Type::None:
                return "None";
            case Type::Locked:
                return "Locked";
            case Type::Active:
                return "Active";
            // case Type::Resetting:
            //    return "Resetting";
            case Type::Paused:
                return "Paused";
            case Type::Stopping:
                return "Stopping";
            case Type::Stopped:
                return "Stopped";
            case Type::Closed:
                return "Closed";
            case Type::Error:
                return "Error";
        }
        return "undefined";
    }
};


//
// Packet Stream
//


/// Processes and broadcasts IPackets through a configurable adapter graph.
///
/// A PacketStream consists of one or many PacketSources, one or many
/// PacketProcessors, and one or many delegate receivers.
///
/// This class enables the developer to setup a processor chain in order
/// to perform arbitrary processing on data packets using interchangeable
/// packet adapters, and pump the output to any delegate function or even
/// another PacketStream.
///
/// Note that PacketStream itself inherits from PacketStreamAdapter, so a
/// PacketStream can be the source of another PacketStream.
///
/// All PacketStream methods are thread-safe, but once the stream is
/// running you will not be able to attach or detach stream adapters.
///
/// In order to synchronize output packets with the application event
/// loop take a look at the SyncPacketQueue class.
/// For lengthy operations you can add an AsyncPacketQueue to the start
/// of the stream to defer processing from the PacketSource thread.
class Base_API PacketStream : public Stateful<PacketStreamState>
{
public:
    using Ptr = std::shared_ptr<PacketStream>;

    /// Construct a named packet stream.
    /// @param name Optional human-readable name used in log output.
    PacketStream(const std::string& name = "");

    /// Destroy the stream; calls close() then reset() to release all adapters.
    virtual ~PacketStream();

    PacketStream(const PacketStream&) = delete;
    PacketStream& operator=(const PacketStream&) = delete;
    PacketStream(PacketStream&&) = delete;
    PacketStream& operator=(PacketStream&&) = delete;

    /// Start the stream and synchronized sources.
    virtual void start();

    /// Stop the stream and synchronized sources.
    virtual void stop();

    /// Pause the stream.
    virtual void pause();

    /// Resume the stream.
    virtual void resume();

    /// Close the stream and transition the internal state to Closed.
    virtual void close();

    /// Cleanup all managed stream adapters and reset the stream state.
    virtual void reset();

    /// Returns true when the stream is in the Active state.
    virtual bool active() const;

    /// Returns true when the stream is in the Stopping or Stopped state.
    virtual bool stopped() const;

    /// Returns true when the stream is in the Closed or Error state.
    virtual bool closed() const;

    /// Sets the stream to locked state.
    /// In a locked state no new adapters can be added or removed
    /// from the stream until the stream is stopped.
    virtual bool lock();

    /// Returns true is the stream is currently locked.
    virtual bool locked() const;

    /// Write a mutable buffer into the stream without copying.
    /// The caller must keep the buffer alive until processing crosses a
    /// Cloned/Retained boundary or, if the graph is fully synchronous,
    /// until write() returns.
    /// @param data Pointer to the raw data buffer.
    /// @param len  Number of bytes to process.
    virtual void write(char* data, size_t len);

    /// Write a read-only buffer into the stream; data is copied immediately
    /// into an owning RawPacket before any adapter sees it.
    /// @param data Pointer to the raw data buffer.
    /// @param len  Number of bytes to process.
    virtual void write(const char* data, size_t len);

    /// Write a packet directly into the processing chain.
    /// @param packet Packet to process; moved into the stream.
    virtual void write(IPacket&& packet);

    /// Attach a bare packet signal as a stream source.
    /// The signal is wrapped in an unowned PacketStreamAdapter internally.
    /// Useful when the source is another PacketStream::emitter.
    /// @param source The packet signal to attach; must outlive the stream.
    virtual void attachSource(PacketSignal& source);

    /// Attach a PacketStreamAdapter as a source.
    /// Source adapters default to Borrowed retention unless overridden; they
    /// must not retain inbound packet storage asynchronously without reporting
    /// Cloned or Retained.
    /// @param source    The adapter to attach; must not be null.
    /// @param owned     If true the stream takes ownership and deletes the pointer on teardown.
    /// @param syncState If true and @p source implements basic::Startable, its
    ///                  start()/stop() will be called by startSources()/stopSources().
    virtual void attachSource(PacketStreamAdapter* source, bool owned = true, bool syncState = false);

    /// Attach a shared_ptr-managed source adapter to the stream.
    /// The stream shares ownership; the adapter is kept alive at least until teardown.
    /// Throws std::runtime_error if @p ptr does not derive from PacketStreamAdapter.
    /// @tparam C        Adapter type; must derive from PacketStreamAdapter.
    /// @param ptr       Shared pointer to the adapter instance.
    /// @param syncState If true and @p ptr implements basic::Startable, its
    ///                  start()/stop() will be called by startSources()/stopSources().
    template <class C>
    void attachSource(std::shared_ptr<C> ptr, bool syncState = false)
    {
        auto source = dynamic_cast<PacketStreamAdapter*>(ptr.get());
        if (!source) {
            throw std::runtime_error("Cannot attach incompatible packet source.");
        }

        attachSource(std::make_shared<PacketAdapterReference>(
            std::move(ptr), 0, syncState));
    }

    /// Detach a source by its packet signal.
    /// Disconnects the signal from the stream's process slot and removes the adapter entry.
    /// @param source The packet signal previously attached via attachSource(PacketSignal&).
    /// @return true if the source was found and removed, false otherwise.
    virtual bool detachSource(PacketSignal& source);

    /// Detach a source by its adapter pointer.
    /// Disconnects the adapter's emitter from the stream's process slot and removes the entry.
    /// @param source Pointer to the adapter previously attached.
    /// @return true if the source was found and removed, false otherwise.
    virtual bool detachSource(PacketStreamAdapter* source);

    /// Attach a packet processor to the stream.
    /// Processors are executed in ascending order of their @p order value.
    /// Pass order = -1 to append at the end of the current processor list.
    /// Valid range is -1 to 101; values outside this range throw std::invalid_argument.
    /// Borrowed processors must finish with the packet before process() returns.
    /// Queue/processors that defer work must report Cloned or Retained via
    /// retention() so upstream code has an explicit ownership boundary.
    /// @param proc  The processor to attach; must not be null.
    /// @param order Position in the processing chain (lower runs first).
    /// @param owned If true the stream takes ownership and deletes the pointer on teardown.
    virtual void attach(PacketProcessor* proc, int order = 0, bool owned = true);

    /// Attach a shared_ptr-managed processor to the stream.
    /// The stream shares ownership; the processor is kept alive at least until teardown.
    /// Throws std::runtime_error if @p ptr does not derive from PacketProcessor.
    /// @tparam C        Processor type; must derive from PacketProcessor.
    /// @param ptr       Shared pointer to the processor instance.
    /// @param order     Position in the processing chain (lower runs first).
    /// @param syncState Reserved for future use; currently unused.
    template <class C>
    void attach(std::shared_ptr<C> ptr, int order = 0, bool syncState = false)
    {
        auto proc = dynamic_cast<PacketProcessor*>(ptr.get());
        if (!proc) {
            throw std::runtime_error(
                "Cannot attach incompatible packet processor.");
        }

        attach(std::make_shared<PacketAdapterReference>(
            std::move(ptr), order, syncState));
    }

    /// Detach a packet processor from the stream.
    /// The processor's delegate connections are removed; ownership is released if held.
    /// @param proc Pointer to the processor to remove.
    /// @return true if the processor was found and removed, false otherwise.
    virtual bool detach(PacketProcessor* proc);

    /// Synchronize stream output packets with a libuv event loop.
    /// Internally attaches a SyncPacketQueue at order 101 so that all packets
    /// emitted by the processor chain are dispatched from the loop thread rather
    /// than the source thread. Must be called before start().
    /// @param loop The event loop to synchronize output onto; must not be null.
    virtual void synchronizeOutput(uv::Loop* loop);

    /// Enable or disable auto-start behaviour (default: false).
    /// When enabled, the stream automatically transitions to Active state
    /// upon receiving the first packet while in the None or Locked state.
    /// Must be called before start().
    /// @param flag true to enable auto-start, false to disable.
    virtual void autoStart(bool flag);

    /// Enable or disable close-on-error behaviour (default: true).
    /// When enabled, an unhandled processor exception causes the stream to
    /// transition from Error to Closed state automatically.
    /// @param flag true to close the stream on error, false to remain in Error state.
    virtual void closeOnError(bool flag);

    /// Accessors for the unmanaged client data pointer.
    // virtual void setClientData(void* data);
    // virtual void* clientData() const;

    /// Return the last captured exception, if the stream is in Error state.
    /// The pointer is null when no error has occurred.
    /// @return A reference to the stored exception_ptr; empty if no error.
    const std::exception_ptr& error();

    /// Return the name assigned to this stream at construction.
    /// @return The stream name; empty string if none was provided.
    std::string name() const;

    /// Signals to delegates on outgoing packets.
    PacketSignal emitter;

    /// Signals that the PacketStream is in Error state.
    /// If stream output is synchronized then the Error signal will be
    /// sent from the synchronization context, otherwise it will be sent from
    /// the async processor context. See synchronizeOutput()
    ThreadSignal<void(PacketStream&, const std::exception_ptr&)> Error;

    /// Signals that the PacketStream is in Close state.
    /// This signal is sent immediately via the close() method,
    /// and as such will be sent from the calling thread context.
    ThreadSignal<void(PacketStream&)> Close;

    /// Returns a combined list of all stream sources and processors.
    PacketAdapterVec adapters() const;

    /// Returns a list of all stream sources.
    PacketAdapterVec sources() const;

    /// Returns a list of all stream processors.
    PacketAdapterVec processors() const;

    /// Return the number of source adapters currently registered.
    /// @return Source count; thread-safe.
    int numSources() const;

    /// Return the number of processor adapters currently registered.
    /// @return Processor count; thread-safe.
    int numProcessors() const;

    /// Return the total number of adapters (sources + processors).
    /// @return Combined adapter count; thread-safe.
    int numAdapters() const;

    /// Return the nth source of type AdapterT, or nullptr if not found.
    /// Sources are searched in their registered order; only adapters that
    /// dynamic_cast successfully to AdapterT are counted.
    /// @tparam AdapterT Target type; must derive from PacketStreamAdapter.
    /// @param index     Zero-based index among matching sources (default 0).
    /// @return Pointer to the matching adapter, or nullptr.
    template <class AdapterT>
    AdapterT* getSource(int index = 0)
    {
        int x = 0;
        std::lock_guard<std::mutex> guard(_mutex);
        for (unsigned i = 0; i < _sources.size(); i++) {
            AdapterT* source = dynamic_cast<AdapterT*>(_sources[i]->ptr);
            if (source) {
                if (index == x)
                    return source;
                else
                    x++;
            }
        }
        return nullptr;
    }

    /// Return the nth processor of type AdapterT, or nullptr if not found.
    /// Processors are searched in their registered order; only adapters that
    /// dynamic_cast successfully to AdapterT are counted.
    /// @tparam AdapterT Target type; must derive from PacketProcessor.
    /// @param index     Zero-based index among matching processors (default 0).
    /// @return Pointer to the matching processor, or nullptr.
    template <class AdapterT>
    AdapterT* getProcessor(int index = 0)
    {
        int x = 0;
        std::lock_guard<std::mutex> guard(_mutex);
        for (unsigned i = 0; i < _processors.size(); i++) {
            AdapterT* processor = dynamic_cast<AdapterT*>(_processors[i]->ptr);
            if (processor) {
                if (index == x)
                    return processor;
                else
                    x++;
            }
        }
        return nullptr;
    }

    /// Return the processor registered at a specific order value.
    /// Unlike the template overload, this searches by order rather than by type and index.
    /// @param order The order value to match (default 0).
    /// @return Pointer to the matching processor, or nullptr if none registered at that order.
    PacketProcessor* getProcessor(int order = 0)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        for (unsigned i = 0; i < _processors.size(); i++) {
            PacketProcessor* processor =
                dynamic_cast<PacketProcessor*>(_processors[i]->ptr);
            if (processor && _processors[i]->order == order) {
                return processor;
            }
        }
        return nullptr;
    }

protected:
    /// Attach the source and processor delegate chain.
    void setup();

    /// Detach the source and processor delegate chain.
    void teardown();

    void attachSource(PacketAdapterReference::Ptr ref);
    void attach(PacketAdapterReference::Ptr ref);

    /// Start synchronized sources.
    void startSources();

    /// Stop synchronized sources.
    void stopSources();

    /// Process incoming packets.
    virtual void process(IPacket& packet);

    /// Emit the final packet to listeners.
    ///
    /// Synchronized signals such as Close and Error are sent
    /// from this method. See synchronizeOutput()
    void emit(IPacket& packet);

    /// Synchronize queued states with adapters.
    void synchronizeStates();

    /// Override the Stateful::onStateChange method
    virtual void onStateChange(PacketStreamState& state, const PacketStreamState& oldState) override;

    /// Returns true if the given state ID is queued.
    // bool hasQueuedState(PacketStreamState::ID state) const;

    /// Asserts that the stream can be modified, ie is not in the Locked,
    /// Stopping or Active states.
    void assertCanModify();

    /// Handle an internal exception.
    void handleException(std::exception& exc);

    mutable std::mutex _mutex;
    mutable std::mutex _procMutex;
    std::string _name;
    PacketAdapterVec _sources;
    PacketAdapterVec _processors;
    std::deque<PacketStreamState> _states;
    std::exception_ptr _error;
    bool _autoStart;
    bool _closeOnError;
    bool _wired;
};


/// Non-owning list of packet stream pointers used for graph traversal helpers.
using PacketStreamVec = std::vector<PacketStream*>;

/// Owning list of packet stream handles retained across stream graphs.
using PacketStreamPtrVec = std::vector<PacketStream::Ptr>;


} // namespace icy

/// @}
