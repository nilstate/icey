///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/trackreceiver.h"
#include "icy/time.h"
#include "icy/webrtc/detail/receiverjitterbuffer.h"
#include "codecregistry.h"


namespace icy {
namespace wrtc {


// Out-of-line so the unique_ptr<ReceiverJitterBuffer> member can be
// declared against the forward declaration in the header.
WebRtcTrackReceiver::DispatchState::DispatchState() = default;
WebRtcTrackReceiver::DispatchState::~DispatchState() = default;


WebRtcTrackReceiver::WebRtcTrackReceiver()
    : PacketStreamAdapter(emitter)
    , _dispatch([this]() { flushPending(); })
    , _timer(uv::defaultLoop())
{
    _timer.Timeout += [this]() { flushPending(); };
    _state->jitterBuffer = std::make_unique<detail::ReceiverJitterBuffer>();
    _state->jitterBuffer->configure(_state->jitterConfig);
    _state->timerTickMs =
        std::max<int64_t>(1, _state->jitterConfig.tickIntervalMs);
    _timer.setTimeout(_state->timerTickMs);
    _timer.setInterval(_state->timerTickMs);
    {
        std::lock_guard lock(_state->mutex);
        _state->dispatch = &_dispatch;
    }
}


WebRtcTrackReceiver::~WebRtcTrackReceiver()
{
    _state->alive.store(false, std::memory_order_release);
    {
        // After this block an in-flight track callback only touches the
        // shared state: it sees a null dispatcher and never dereferences
        // this (partially destroyed) receiver.
        std::lock_guard lock(_state->mutex);
        _state->dispatch = nullptr;
        _state->pending.clear();
        if (_state->jitterBuffer)
            _state->jitterBuffer->reset();
    }
    if (_timer.active())
        _timer.stop();
    _dispatch.close();
}


void WebRtcTrackReceiver::bind(std::shared_ptr<rtc::Track> track)
{
    auto desc = track->description();
    bool isVideo = (desc.type() == "video");
    uint32_t clockRate = isVideo ? 90000u : 48000u;
    if (auto spec = codec_registry::detectInMedia(
            desc, isVideo ? CodecMediaType::Video : CodecMediaType::Audio)) {
        clockRate = spec->clockRate;
    }
    const auto generation = ++_generation;
    _state->generation.store(generation, std::memory_order_release);
    {
        std::lock_guard lock(_state->mutex);
        _state->pending.clear();
        _state->jitterBuffer->reset();
    }
    _dispatch.post();
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
    track->onFrame([state, generation, isVideo, clockRate](rtc::binary frame, rtc::FrameInfo info) {
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
            enqueue(state, std::move(pkt));
        }
        else {
            auto pkt = std::make_unique<av::AudioPacket>();
            pkt->copyData(frame.data(), frame.size());
            pkt->time = timeUs;
            enqueue(state, std::move(pkt));
        }
    });
}


void WebRtcTrackReceiver::configureJitterBuffer(const JitterBufferConfig& config)
{
    {
        std::lock_guard lock(_state->mutex);
        _state->jitterConfig = config;
        _state->jitterBuffer->configure(config);
        _state->pending.clear();
        _state->timerTickMs = std::max<int64_t>(1, config.tickIntervalMs);
        _state->timerNeedsUpdate = true;
    }
    _dispatch.post();
}


JitterBufferConfig WebRtcTrackReceiver::jitterBufferConfig() const
{
    std::lock_guard lock(_state->mutex);
    return _state->jitterConfig;
}


bool WebRtcTrackReceiver::jitterBufferEnabled() const
{
    std::lock_guard lock(_state->mutex);
    return _state->jitterBuffer && _state->jitterBuffer->enabled();
}


void WebRtcTrackReceiver::enqueue(const std::shared_ptr<DispatchState>& state,
                                  std::unique_ptr<IPacket> packet)
{
    if (!packet)
        return;

    std::lock_guard lock(state->mutex);
    if (!state->alive.load(std::memory_order_acquire))
        return;
    if (state->jitterBuffer && state->jitterBuffer->enabled()) {
        state->jitterBuffer->push(std::move(packet), time::hrtime() / 1000);
    }
    else {
        state->pending.emplace_back(std::move(packet));
    }
    // Post under the state mutex: the destructor nulls `dispatch` under the
    // same mutex before the Synchronizer is destroyed.
    if (state->dispatch)
        state->dispatch->post();
}


void WebRtcTrackReceiver::flushPending()
{
    std::deque<std::unique_ptr<IPacket>> pending;
    bool shouldRunTimer = false;
    {
        std::lock_guard lock(_state->mutex);
        if (_state->timerNeedsUpdate) {
            if (_timer.active())
                _timer.stop();
            _timer.setTimeout(_state->timerTickMs);
            _timer.setInterval(_state->timerTickMs);
            _state->timerNeedsUpdate = false;
        }

        pending.swap(_state->pending);
        if (_state->jitterBuffer && _state->jitterBuffer->enabled()) {
            _state->jitterBuffer->drainReady(time::hrtime() / 1000, pending);
            shouldRunTimer = _state->jitterBuffer->hasPending();
        }
    }

    if (shouldRunTimer) {
        if (!_timer.active())
            _timer.start();
    }
    else if (_timer.active()) {
        _timer.stop();
    }

    for (auto& packet : pending)
        emit(*packet);
}


} // namespace wrtc
} // namespace icy
