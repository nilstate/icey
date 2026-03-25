///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/trackreceiver.h"
#include "icy/webrtc/codecnegotiator.h"


namespace icy {
namespace wrtc {


WebRtcTrackReceiver::WebRtcTrackReceiver()
    : PacketStreamAdapter(emitter)
    , _dispatch([this]() { flushPending(); })
{
}


WebRtcTrackReceiver::~WebRtcTrackReceiver()
{
    _state->alive.store(false, std::memory_order_release);
    _dispatch.close();
    std::lock_guard lock(_mutex);
    _pending.clear();
}


void WebRtcTrackReceiver::bind(std::shared_ptr<rtc::Track> track)
{
    auto desc = track->description();
    bool isVideo = (desc.type() == "video");
    auto sdp = std::string(desc.generateSdp("\r\n", ""));
    uint32_t clockRate = isVideo ? 90000u : 48000u;
    if (auto spec = CodecNegotiator::detectCodec(
            sdp, isVideo ? CodecMediaType::Video : CodecMediaType::Audio)) {
        clockRate = spec->clockRate;
    }
    const auto generation = ++_generation;
    _state->generation.store(generation, std::memory_order_release);
    auto state = _state;
    track->onOpen([state, generation]() {
        if (!state->alive.load(std::memory_order_acquire) ||
            state->generation.load(std::memory_order_acquire) != generation) {
            return;
        }
    });
    track->onMessage([state, generation](rtc::binary) {
        if (!state->alive.load(std::memory_order_acquire) ||
            state->generation.load(std::memory_order_acquire) != generation) {
            return;
        }
    }, nullptr);
    track->onFrame([this, state, generation, isVideo, clockRate](rtc::binary frame, rtc::FrameInfo info) {
        if (!state->alive.load(std::memory_order_acquire) ||
            state->generation.load(std::memory_order_acquire) != generation) {
            return;
        }
        if (frame.empty())
            return;

        // Convert RTP timestamp to microseconds.
        // Use timestampSeconds if the depacketizer provides it (v0.24+),
        // otherwise convert from the raw RTP timestamp.
        int64_t timeUs = 0;
        if (info.timestampSeconds.has_value()) {
            timeUs = static_cast<int64_t>(
                info.timestampSeconds->count() * 1000000.0);
        }
        else {
            timeUs = static_cast<int64_t>(info.timestamp) * 1000000 / clockRate;
        }

        if (isVideo) {
            auto pkt = std::make_unique<av::VideoPacket>();
            pkt->copyData(frame.data(), frame.size());
            pkt->time = timeUs;
            enqueue(std::move(pkt));
        }
        else {
            auto pkt = std::make_unique<av::AudioPacket>();
            pkt->copyData(frame.data(), frame.size());
            pkt->time = timeUs;
            enqueue(std::move(pkt));
        }
    });
}


void WebRtcTrackReceiver::enqueue(std::unique_ptr<IPacket> packet)
{
    {
        std::lock_guard lock(_mutex);
        _pending.emplace_back(std::move(packet));
    }
    _dispatch.post();
}


void WebRtcTrackReceiver::flushPending()
{
    std::deque<std::unique_ptr<IPacket>> pending;
    {
        std::lock_guard lock(_mutex);
        pending.swap(_pending);
    }

    for (auto& packet : pending)
        emit(*packet);
}


} // namespace wrtc
} // namespace icy
