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


/// This class emits media packets based on their realtime pts value.
template <class PacketT>
class RealtimePacketQueue : public AsyncPacketQueue<PacketT>
{
public:
    using BaseQueue = AsyncPacketQueue<PacketT>;

    RealtimePacketQueue(int maxSize = 1024)
        : BaseQueue(maxSize)
    {
    }

    virtual ~RealtimePacketQueue() noexcept
    {
    }

    // Add an item to the queue
    virtual void push(PacketT* item) override
    {
        BaseQueue::push(item);
        BaseQueue::template sort<MediaPacketTimeCompare>();
    }

    // Return the current duration from stream start in microseconds
    int64_t realTime()
    {
        return (time::hrtime() - _startTime) / 1000;
    }

protected:
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

    virtual void onStreamStateChange(const PacketStreamState& state) override
    {
        LTrace("Stream state changed: ", state);

        if (state.equals(PacketStreamState::Active)) {
            _startTime = time::hrtime();
        }

        BaseQueue::onStreamStateChange(state);
    }

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
