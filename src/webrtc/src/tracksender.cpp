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

    // Determine track type from the description.
    if (_track)
        _isVideo = (_track->description().type() == "video");
}


void WebRtcTrackSender::unbind()
{
    std::lock_guard lock(_mutex);
    _track.reset();
    _rtpConfig.reset();
}


void WebRtcTrackSender::process(IPacket& packet)
{
    std::lock_guard lock(_mutex);

    if (!_track || !_track->isOpen() || !_rtpConfig)
        return;

    av::MediaPacket* mp = nullptr;
    if (_isVideo)
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
        (mp->time * _rtpConfig->clockRate) / 1000000);

    try {
        _track->sendFrame(
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
    if (_isVideo)
        return dynamic_cast<av::VideoPacket*>(packet) != nullptr;
    return dynamic_cast<av::AudioPacket*>(packet) != nullptr;
}


void WebRtcTrackSender::onStreamStateChange(const PacketStreamState& state)
{
    if (state.id() == PacketStreamState::Stopping) {
        LDebug("WebRtcTrackSender: stream stopping");
    }
}


bool WebRtcTrackSender::isVideo() const
{
    std::lock_guard lock(_mutex);
    return _isVideo;
}


bool WebRtcTrackSender::bound() const
{
    std::lock_guard lock(_mutex);
    return _track != nullptr;
}


} // namespace wrtc
} // namespace icy
