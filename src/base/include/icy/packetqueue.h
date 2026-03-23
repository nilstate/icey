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
#include "icy/packetstream.h"
#include "icy/queue.h"
#include "icy/synchronizer.h"


namespace icy {


//
// Synchronized Packet Queue
//


/// Synchronized packet queue for event loop integration
template <class T = IPacket>
class SyncPacketQueue : public SyncQueue<T>
    , public PacketProcessor
{
public:
    using Queue = SyncQueue<T>;
    using Processor = PacketProcessor;

    /// @param loop Event loop to synchronize dispatch onto.
    /// @param maxSize Maximum number of queued packets before oldest are dropped.
    SyncPacketQueue(uv::Loop* loop, int maxSize = 1024)
        : Queue(loop, maxSize)
        , Processor(this->emitter)
    {
    }

    /// Uses the default libuv event loop.
    /// @param maxSize Maximum number of queued packets before oldest are dropped.
    SyncPacketQueue(int maxSize = 1024)
        : Queue(uv::defaultLoop(), maxSize)
        , Processor(this->emitter)
    {
    }

    virtual ~SyncPacketQueue() {}

    /// Clones the incoming packet and pushes it onto the queue for synchronized dispatch.
    /// Drops the packet with a warning if the queue has been cancelled.
    /// @param packet Incoming packet to enqueue.
    virtual void process(IPacket& packet) override;

    /// Returns true if the packet can be cast to type `T`.
    /// @param packet Packet to test.
    /// @return True if `dynamic_cast<T*>(packet)` succeeds.
    virtual bool accepts(IPacket* packet) override;

    [[nodiscard]] PacketRetention retention() const override
    {
        return PacketRetention::Cloned;
    }

    PacketSignal emitter;

protected:
    /// Emits the packet to downstream processors from the event loop thread.
    /// @param packet Packet to dispatch.
    void dispatch(T& packet) override;

    /// Cancels the queue on `Closed` or `Error` stream state transitions.
    /// @param state New stream state.
    void onStreamStateChange(const PacketStreamState&) override;
};


template <class T>
inline void SyncPacketQueue<T>::process(IPacket& packet)
{
    if (Queue::cancelled()) {
        LWarn("Process late packet");
        return;
    }

    Queue::push(static_cast<T*>(packet.clone().release()));
}


template <class T>
inline void SyncPacketQueue<T>::dispatch(T& packet)
{
    if (Queue::cancelled()) {
        LWarn("Dispatch late packet");
        return;
    }

    // Note: Emit should never be called after closure.
    // Any late packets should have been dealt with
    // and dropped by the run() function.
    Processor::emit(packet);
}


template <class T>
inline bool SyncPacketQueue<T>::accepts(IPacket* packet)
{
    return dynamic_cast<T*>(packet) != nullptr;
}


template <class T>
inline void SyncPacketQueue<T>::onStreamStateChange(const PacketStreamState& state)
{
    LTrace("Stream state: ", state);

    switch (state.id()) {
        // case PacketStreamState::None:
        // case PacketStreamState::Active:
        // case PacketStreamState::Resetting:
        // case PacketStreamState::Stopping:
        // case PacketStreamState::Stopped:
        case PacketStreamState::Closed:
        case PacketStreamState::Error:
            Queue::cancel();
            break;
    }
}


//
// Asynchronous Packet Queue
//


/// Thread-based asynchronous packet dispatch queue
template <class T = IPacket>
class AsyncPacketQueue : public AsyncQueue<T>
    , public PacketProcessor
{
public:
    using Queue = AsyncQueue<T>;
    using Processor = PacketProcessor;

    /// @param maxSize Maximum number of queued packets before oldest are dropped.
    AsyncPacketQueue(int maxSize = 1024)
        : Queue(maxSize)
        , Processor(this->emitter)
    {
    }

    virtual ~AsyncPacketQueue() {}

    /// Flushes remaining packets, cancels the queue, and joins the dispatch thread.
    virtual void close();

    /// Clones the incoming packet and pushes it onto the async queue.
    /// Drops the packet with a warning if the queue has been cancelled.
    /// @param packet Incoming packet to enqueue.
    void process(IPacket& packet) override;

    /// Returns true if the packet can be cast to type `T`.
    /// @param packet Packet to test.
    /// @return True if `dynamic_cast<T*>(packet)` succeeds.
    bool accepts(IPacket* packet) override;

    [[nodiscard]] PacketRetention retention() const override
    {
        return PacketRetention::Cloned;
    }

    PacketSignal emitter;

protected:
    /// Emits the packet to downstream processors from the async thread.
    /// @param packet Packet to dispatch.
    void dispatch(T& packet) override;

    /// Closes the queue on `Error` or `Closed` stream state transitions.
    /// @param state New stream state.
    void onStreamStateChange(const PacketStreamState&) override;
};


template <class T>
inline void AsyncPacketQueue<T>::close()
{
    // Flush queued items, some protocols can't afford dropped packets
    Queue::flush();
    if (!Queue::empty())
        LWarn("Queue not empty after flush during close");
    Queue::cancel();
    Queue::_thread.join();
}


template <class T>
inline void
AsyncPacketQueue<T>::dispatch(T& packet)
{
    if (Queue::cancelled()) {
        LWarn("Dispatch late packet");
        return;
    }

    Processor::emit(packet);
}


template <class T>
inline void AsyncPacketQueue<T>::process(IPacket& packet)
{
    if (Queue::cancelled()) {
        LWarn("Process late packet");
        return;
    }

    this->push(static_cast<T*>(packet.clone().release()));
}


template <class T>
inline bool AsyncPacketQueue<T>::accepts(IPacket* packet)
{
    return dynamic_cast<T*>(packet) != nullptr;
}


template <class T>
inline void
AsyncPacketQueue<T>::onStreamStateChange(const PacketStreamState& state)
{
    LTrace("Stream state: ", state);

    switch (state.id()) {
        case PacketStreamState::Active:
            break;

        case PacketStreamState::Stopped:
            break;

        case PacketStreamState::Error:
        case PacketStreamState::Closed:
            close();
            break;

            // case PacketStreamState::Resetting:
            // case PacketStreamState::None:
            // case PacketStreamState::Stopping:
    }
}

} // namespace icy


/// @\}
