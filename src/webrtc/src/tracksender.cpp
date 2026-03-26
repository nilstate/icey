///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/tracksender.h"
#include "icy/logger.h"


namespace icy {
namespace wrtc {


WebRtcTrackSender::WebRtcTrackSender()
    : PacketProcessor(emitter)
{
}


WebRtcTrackSender::WebRtcTrackSender(const TrackHandle& handle)
    : PacketProcessor(emitter)
{
    bind(handle);
}


void WebRtcTrackSender::bind(const TrackHandle& handle)
{
    std::lock_guard lock(_mutex);
    _track = handle.track;
    _rtpConfig = handle.rtpConfig;

    if (_track) {
        _kind.store(
            _track->description().type() == "video" ? TrackKind::Video : TrackKind::Audio,
            std::memory_order_release);
    }
    else {
        _kind.store(TrackKind::Unbound, std::memory_order_release);
    }
}


void WebRtcTrackSender::unbind()
{
    std::lock_guard lock(_mutex);
    _track.reset();
    _rtpConfig.reset();
    _kind.store(TrackKind::Unbound, std::memory_order_release);
}


void WebRtcTrackSender::process(IPacket& packet)
{
    std::shared_ptr<rtc::Track> track;
    std::shared_ptr<rtc::RtpPacketizationConfig> rtpConfig;
    {
        std::lock_guard lock(_mutex);
        track = _track;
        rtpConfig = _rtpConfig;
    }

    const auto kind = _kind.load(std::memory_order_acquire);
    if (!track || !track->isOpen() || !rtpConfig || kind == TrackKind::Unbound)
        return;

    av::MediaPacket* mp = nullptr;
    if (kind == TrackKind::Video)
        mp = dynamic_cast<av::VideoPacket*>(&packet);
    else
        mp = dynamic_cast<av::AudioPacket*>(&packet);

    if (!mp) {
        emit(packet);
        return;
    }

    // Convert FFmpeg timestamp (microseconds) to RTP timestamp
    // using the clock rate from the RTP config.
    auto rtpTs = static_cast<uint32_t>(
        (mp->time * rtpConfig->clockRate) / 1000000);

    try {
        track->sendFrame(
            reinterpret_cast<const rtc::byte*>(mp->data()), mp->size(),
            rtc::FrameInfo(rtpTs));

        // Only pass downstream on success.
        emit(packet);
    }
    catch (const std::exception& e) {
        LWarn("sendFrame failed: ", e.what());
    }
}


bool WebRtcTrackSender::accepts(IPacket* packet)
{
    if (!packet)
        return false;

    switch (_kind.load(std::memory_order_acquire)) {
    case TrackKind::Video:
        return dynamic_cast<av::VideoPacket*>(packet) != nullptr;
    case TrackKind::Audio:
        return dynamic_cast<av::AudioPacket*>(packet) != nullptr;
    case TrackKind::Unbound:
        return dynamic_cast<av::VideoPacket*>(packet) != nullptr ||
               dynamic_cast<av::AudioPacket*>(packet) != nullptr;
    }

    return false;
}


void WebRtcTrackSender::onStreamStateChange(const PacketStreamState& state)
{
    if (state.id() == PacketStreamState::Stopping) {
        LDebug("WebRtcTrackSender: stream stopping");
    }
}


bool WebRtcTrackSender::isVideo() const
{
    return _kind.load(std::memory_order_acquire) == TrackKind::Video;
}


bool WebRtcTrackSender::bound() const
{
    std::lock_guard lock(_mutex);
    return _track != nullptr;
}


} // namespace wrtc
} // namespace icy
