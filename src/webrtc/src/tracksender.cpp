///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "scy/webrtc/tracksender.h"
#include "scy/logger.h"


namespace scy {
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

    auto* mp = dynamic_cast<av::MediaPacket*>(&packet);
    if (!mp)
        return;

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
    // Accept any MediaPacket derivative (VideoPacket, AudioPacket, etc.)
    return dynamic_cast<av::MediaPacket*>(packet) != nullptr;
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
} // namespace scy
