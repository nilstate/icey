///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "icy/webrtc/track.h"
#include "icy/webrtc/webrtc.h"

#include "icy/av/packet.h"
#include "icy/packetsignal.h"
#include "icy/packetstream.h"

#include <rtc/rtc.hpp>

#include <memory>
#include <mutex>


namespace icy {
namespace wrtc {


/// PacketProcessor that sends encoded media to a single
/// libdatachannel Track via sendFrame().
///
/// Bind to one track (video or audio). Accepts the corresponding
/// packet type from the PacketStream and converts timestamps from
/// FFmpeg microseconds to the track's RTP clock rate.
///
/// Usage:
///   auto vh = createVideoTrack(pc, codec);
///   WebRtcTrackSender videoSender(vh);
///
///   PacketStream stream;
///   stream.attachSource(capture);
///   stream.attach(encoder, 1, true);
///   stream.attach(&videoSender, 5, false);
///   stream.start();
///
/// Only emits the packet downstream on successful send, so a
/// chained recorder won't record frames that failed to transmit.
///
/// Accepts both VideoPacket and AudioPacket. It determines which
/// to handle based on the track type (video or audio) set at
/// construction.
class WEBRTC_API WebRtcTrackSender : public PacketProcessor
{
public:
    /// Construct an unbound sender. Call bind() before use.
    WebRtcTrackSender();

    /// Construct bound to a track handle from createVideoTrack()
    /// or createAudioTrack().
    explicit WebRtcTrackSender(const TrackHandle& handle);

    /// Bind to a track. Can be called to rebind to a different track.
    void bind(const TrackHandle& handle);

    /// Unbind from the current track.
    void unbind();

    /// PacketProcessor interface.
    void process(IPacket& packet) override;
    bool accepts(IPacket* packet) override;
    void onStreamStateChange(const PacketStreamState& state) override;

    /// True if this sender is bound to a video track.
    [[nodiscard]] bool isVideo() const;

    /// True if bound to any track.
    [[nodiscard]] bool bound() const;

    PacketSignal emitter;

private:
    std::shared_ptr<rtc::Track> _track;
    std::shared_ptr<rtc::RtpPacketizationConfig> _rtpConfig;
    bool _isVideo = true;
    mutable std::mutex _mutex;
};


} // namespace wrtc
} // namespace icy


/// @\}
