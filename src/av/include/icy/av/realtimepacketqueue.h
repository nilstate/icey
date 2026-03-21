///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/av/packet.h"
#include "icy/base.h"
#include "icy/packetqueue.h"


namespace icy {
namespace av {


/// Queue that emits media packets in presentation-timestamp order relative to a realtime clock.
///
/// Packets are sorted by their `time` field on insertion. On each pop attempt the queue checks
/// whether the wall-clock time since stream activation has reached the next packet's timestamp;
/// if not, the packet is held back. This provides a soft realtime playback scheduler.
template <class PacketT>
class RealtimePacketQueue : public AsyncPacketQueue<PacketT>
{
public:
    using BaseQueue = AsyncPacketQueue<PacketT>;

    /// Construct the queue with a maximum capacity.
    /// @param maxSize  The maximum number of packets the queue will hold.
    RealtimePacketQueue(int maxSize = 1024)
        : BaseQueue(maxSize)
    {
    }

    virtual ~RealtimePacketQueue() noexcept
    {
    }

    /// Insert a packet into the queue and re-sort by presentation timestamp.
    /// @param item  The packet to enqueue; ownership is transferred.
    virtual void push(PacketT* item) override
    {
        BaseQueue::push(item);
        BaseQueue::template sort<MediaPacketTimeCompare>();
    }

    /// Return the elapsed time since stream activation in microseconds.
    int64_t realTime()
    {
        return (time::hrtime() - _startTime) / 1000;
    }

protected:
    /// Return the next packet whose timestamp is <= realTime(), or nullptr if none is ready.
    virtual PacketT* popNext() override
    {
        if (BaseQueue::empty())
            return nullptr;

        auto next = BaseQueue::front();
        if (next->time > realTime())
            return nullptr;
        BaseQueue::pop();

        STrace << "Pop next: " << BaseQueue::size() << ": "
               << realTime() << " > " << next->time;
        return next;
    }

    /// Record the stream start time when the stream becomes active.
    virtual void onStreamStateChange(const PacketStreamState& state) override
    {
        LTrace("Stream state changed: ", state);

        if (state.equals(PacketStreamState::Active)) {
            _startTime = time::hrtime();
        }

        BaseQueue::onStreamStateChange(state);
    }

    /// Comparator for ordering media packets by presentation timestamp
    struct MediaPacketTimeCompare
    {
        bool operator()(const MediaPacket* a, const MediaPacket* b)
        {
            return a->time < b->time;
        }
    };

    int64_t _startTime;
};


} // namespace av
} // namespace icy


/// @\}
