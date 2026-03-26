///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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

#include <atomic>
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
/// Accepts only the packet type that matches the bound track.
/// Non-matching packets are passed through unchanged so mixed
/// audio/video PacketStream chains can share one source cleanly.
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

    /// Send an encoded media frame to the bound WebRTC track.
    /// Converts the FFmpeg microsecond timestamp to an RTP timestamp using
    /// the track's clock rate, then calls rtc::Track::sendFrame(). Only
    /// forwards the packet downstream on a successful send.
    /// @param packet  An av::VideoPacket or av::AudioPacket carrying the
    ///                encoded frame data and a microsecond timestamp.
    void process(IPacket& packet) override;

    /// Return true only for the packet type that matches the bound track.
    /// @param packet  Packet to test. May be nullptr.
    /// @return        True if the packet can be processed by this sender.
    bool accepts(IPacket* packet) override;

    /// Called by the PacketStream when stream state changes.
    /// Logs when the stream is stopping; no other action is taken.
    /// @param state  New PacketStream state.
    void onStreamStateChange(const PacketStreamState& state) override;

    /// True if this sender is bound to a video track.
    [[nodiscard]] bool isVideo() const;

    /// True if bound to any track.
    [[nodiscard]] bool bound() const;

    PacketSignal emitter;

private:
    enum class TrackKind
    {
        Unbound,
        Video,
        Audio
    };

    std::shared_ptr<rtc::Track> _track;
    std::shared_ptr<rtc::RtpPacketizationConfig> _rtpConfig;
    std::atomic<TrackKind> _kind{TrackKind::Unbound};
    mutable std::mutex _mutex;
};


} // namespace wrtc
} // namespace icy


/// @}
