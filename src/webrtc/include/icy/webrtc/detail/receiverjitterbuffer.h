///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#pragma once


#include "icy/av/packet.h"
#include "icy/webrtc/jitterbuffer.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <limits>
#include <map>
#include <memory>


namespace icy {
namespace wrtc {
namespace detail {


class ReceiverJitterBuffer
{
public:
    ReceiverJitterBuffer() = default;

    void configure(const JitterBufferConfig& config)
    {
        _config = normalize(config);
        reset();
    }

    [[nodiscard]] JitterBufferConfig config() const
    {
        return _config;
    }

    [[nodiscard]] bool enabled() const
    {
        return _config.enabled && _config.maxDelayMs > 0 &&
               _config.maxDelayMs >= _config.minDelayMs;
    }

    void reset()
    {
        _buffer.clear();
        _anchored = false;
        _havePrevSample = false;
        _anchorMediaTimeUs = 0;
        _anchorArrivalUs = 0;
        _prevMediaTimeUs = 0;
        _prevArrivalUs = 0;
        _jitterUs = 0;
        _currentDelayUs = std::max<int64_t>(0, _config.minDelayMs) * 1000;
        _lastReleasedMediaTimeUs = std::numeric_limits<int64_t>::min();
        _serial = 0;
    }

    void push(std::unique_ptr<IPacket> packet, std::uint64_t arrivalUs)
    {
        if (!packet || !enabled())
            return;

        auto* media = dynamic_cast<av::MediaPacket*>(packet.get());
        if (!media)
            return;

        const int64_t mediaTimeUs = media->time;
        if (mediaTimeUs <= _lastReleasedMediaTimeUs || hasQueued(mediaTimeUs))
            return;

        if (!_anchored) {
            _anchored = true;
            _anchorMediaTimeUs = mediaTimeUs;
            _anchorArrivalUs = arrivalUs;
            _currentDelayUs = std::max<int64_t>(0, _config.minDelayMs) * 1000;
        }

        updateDelayEstimate(mediaTimeUs, arrivalUs);

        Entry entry;
        entry.mediaTimeUs = mediaTimeUs;
        entry.releaseAtUs = computeReleaseTimeUs(mediaTimeUs);
        entry.packet = std::move(packet);
        _buffer.emplace(Key{mediaTimeUs, _serial++}, std::move(entry));

        _prevMediaTimeUs = mediaTimeUs;
        _prevArrivalUs = arrivalUs;
        _havePrevSample = true;
    }

    void drainReady(std::uint64_t nowUs, std::deque<std::unique_ptr<IPacket>>& out)
    {
        while (!_buffer.empty()) {
            auto it = _buffer.begin();
            if (it->second.releaseAtUs > nowUs)
                break;

            _lastReleasedMediaTimeUs = it->second.mediaTimeUs;
            out.emplace_back(std::move(it->second.packet));
            _buffer.erase(it);
        }
    }

    [[nodiscard]] bool hasPending() const
    {
        return !_buffer.empty();
    }

private:
    struct Key
    {
        int64_t mediaTimeUs = 0;
        std::uint64_t serial = 0;

        [[nodiscard]] bool operator<(const Key& other) const
        {
            if (mediaTimeUs != other.mediaTimeUs)
                return mediaTimeUs < other.mediaTimeUs;
            return serial < other.serial;
        }
    };

    struct Entry
    {
        int64_t mediaTimeUs = 0;
        std::uint64_t releaseAtUs = 0;
        std::unique_ptr<IPacket> packet;
    };

    [[nodiscard]] static JitterBufferConfig normalize(const JitterBufferConfig& config)
    {
        JitterBufferConfig normalized = config;
        normalized.minDelayMs = std::max<int64_t>(0, normalized.minDelayMs);
        normalized.maxDelayMs = std::max(normalized.maxDelayMs, normalized.minDelayMs);
        normalized.adaptiveFactor = std::max(0.0, normalized.adaptiveFactor);
        normalized.tickIntervalMs = std::max<int64_t>(1, normalized.tickIntervalMs);
        return normalized;
    }

    [[nodiscard]] bool hasQueued(int64_t mediaTimeUs) const
    {
        auto it = _buffer.lower_bound(Key{mediaTimeUs, 0});
        return it != _buffer.end() && it->first.mediaTimeUs == mediaTimeUs;
    }

    void updateDelayEstimate(int64_t mediaTimeUs, std::uint64_t arrivalUs)
    {
        if (!_havePrevSample)
            return;

        const int64_t mediaDeltaUs = mediaTimeUs - _prevMediaTimeUs;
        if (mediaDeltaUs <= 0)
            return;

        const int64_t arrivalDeltaUs = static_cast<int64_t>(arrivalUs - _prevArrivalUs);
        const int64_t variationUs = std::llabs(arrivalDeltaUs - mediaDeltaUs);
        if (variationUs > _jitterUs)
            _jitterUs = variationUs;
        else
            _jitterUs = (_jitterUs * 7 + variationUs) / 8;

        const int64_t minDelayUs = _config.minDelayMs * 1000;
        const int64_t maxDelayUs = _config.maxDelayMs * 1000;
        const int64_t adaptiveUs = static_cast<int64_t>(
            std::llround(_config.adaptiveFactor * static_cast<double>(_jitterUs)));
        _currentDelayUs = std::clamp(minDelayUs + adaptiveUs, minDelayUs, maxDelayUs);
    }

    [[nodiscard]] std::uint64_t computeReleaseTimeUs(int64_t mediaTimeUs) const
    {
        const int64_t offsetUs = mediaTimeUs - _anchorMediaTimeUs;
        const int64_t releaseAtUs = static_cast<int64_t>(_anchorArrivalUs) +
                                    offsetUs + _currentDelayUs;
        return releaseAtUs > 0 ? static_cast<std::uint64_t>(releaseAtUs) : 0u;
    }

    JitterBufferConfig _config;
    std::map<Key, Entry> _buffer;
    bool _anchored = false;
    bool _havePrevSample = false;
    int64_t _anchorMediaTimeUs = 0;
    std::uint64_t _anchorArrivalUs = 0;
    int64_t _prevMediaTimeUs = 0;
    std::uint64_t _prevArrivalUs = 0;
    int64_t _jitterUs = 0;
    int64_t _currentDelayUs = 0;
    int64_t _lastReleasedMediaTimeUs = std::numeric_limits<int64_t>::min();
    std::uint64_t _serial = 0;
};


} // namespace detail
} // namespace wrtc
} // namespace icy
