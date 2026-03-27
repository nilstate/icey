///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup vision
/// @{


#pragma once


#include "icy/av/packet.h"
#include "icy/packetstream.h"
#include "icy/vision/vision.h"

#include <atomic>
#include <cstdint>
#include <limits>


namespace icy {
namespace vision {


struct Vision_API FrameSamplerConfig
{
    uint32_t everyNthFrame = 1;
    int64_t minIntervalUsec = 0;
};


struct Vision_API FrameSamplerStats
{
    uint64_t seen = 0;
    uint64_t forwarded = 0;
    uint64_t dropped = 0;
};


/// PacketProcessor that forwards a sampled subset of decoded video frames.
///
/// The sampler is intentionally zero-copy on its synchronous path: sampled
/// frames are forwarded by reference and dropped frames are discarded before any
/// queue hop. Pair it with DetectionQueue when a worker-thread boundary is
/// required.
class Vision_API FrameSampler : public PacketProcessor
{
public:
    PacketSignal emitter;

    explicit FrameSampler(FrameSamplerConfig config = {})
        : PacketProcessor(emitter)
        , _config(sanitize(config))
    {
    }

    void setConfig(FrameSamplerConfig config)
    {
        _config = sanitize(config);
        reset();
    }

    [[nodiscard]] FrameSamplerConfig config() const
    {
        return _config;
    }

    [[nodiscard]] FrameSamplerStats stats() const
    {
        return {
            .seen = _seen.load(),
            .forwarded = _forwarded.load(),
            .dropped = _dropped.load(),
        };
    }

    void reset()
    {
        _seen = 0;
        _forwarded = 0;
        _dropped = 0;
        _lastForwardedTimeUsec = kUnsetTimeUsec;
    }

    void process(IPacket& packet) override
    {
        auto* frame = dynamic_cast<av::PlanarVideoPacket*>(&packet);
        if (!frame) {
            ++_dropped;
            return;
        }

        const uint64_t seen = ++_seen;
        if (!shouldForward(*frame, seen)) {
            ++_dropped;
            return;
        }

        ++_forwarded;
        _lastForwardedTimeUsec = frame->time;
        PacketProcessor::emit(packet);
    }

    bool accepts(IPacket*) override
    {
        // Accept every packet so non-video input is swallowed by this branch
        // instead of falling through into later detection stages.
        return true;
    }

private:
    static constexpr int64_t kUnsetTimeUsec = std::numeric_limits<int64_t>::min();

    static FrameSamplerConfig sanitize(FrameSamplerConfig config)
    {
        if (config.everyNthFrame == 0)
            config.everyNthFrame = 1;
        return config;
    }

    bool shouldForward(const av::PlanarVideoPacket& frame, uint64_t seen) const
    {
        if (_config.everyNthFrame > 1 &&
            ((seen - 1) % static_cast<uint64_t>(_config.everyNthFrame)) != 0)
            return false;

        if (_config.minIntervalUsec > 0) {
            const int64_t last = _lastForwardedTimeUsec.load();
            if (last != kUnsetTimeUsec &&
                frame.time > last &&
                (frame.time - last) < _config.minIntervalUsec)
                return false;
        }

        return true;
    }

    FrameSamplerConfig _config;
    std::atomic<uint64_t> _seen{0};
    std::atomic<uint64_t> _forwarded{0};
    std::atomic<uint64_t> _dropped{0};
    std::atomic<int64_t> _lastForwardedTimeUsec{kUnsetTimeUsec};
};


} // namespace vision
} // namespace icy


/// @}
