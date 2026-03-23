///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/trackreceiver.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"


namespace icy {
namespace wrtc {


WebRtcTrackReceiver::WebRtcTrackReceiver()
    : PacketStreamAdapter(emitter)
{
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

    track->onFrame([this, isVideo, clockRate](rtc::binary frame, rtc::FrameInfo info) {
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
            // Construct owning VideoPacket via copyData() so downstream
            // processors can safely queue it asynchronously.
            av::VideoPacket pkt;
            pkt.copyData(frame.data(), frame.size());
            pkt.time = timeUs;
            emit(pkt);
        }
        else {
            av::AudioPacket pkt;
            pkt.copyData(frame.data(), frame.size());
            pkt.time = timeUs;
            emit(pkt);
        }
    });

    LDebug("Bound to ", isVideo ? "video" : "audio", " track");
}


} // namespace wrtc
} // namespace icy
