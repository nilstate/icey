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


#include "icy/webrtc/webrtc.h"
#include "icy/av/codec.h"

#include <rtc/rtc.hpp>

#include <cstdint>
#include <memory>
#include <string>


namespace icy {
namespace wrtc {


/// Result of creating a track: the track itself plus its RTP config.
/// Keep the config around - you need it for WebRtcTrackSender.
struct WEBRTC_API TrackHandle
{
    std::shared_ptr<rtc::Track> track;
    std::shared_ptr<rtc::RtpPacketizationConfig> rtpConfig;
};


/// Create a video send track on a PeerConnection.
///
/// Sets up the full outgoing media handler chain:
///   Packetizer → SrReporter → NackResponder → PliHandler → RembHandler
///
/// The packetizer is selected based on the codec:
///   H264 → H264RtpPacketizer (Annex-B long start sequence)
///   H265 → H265RtpPacketizer
///   VP8/VP9/other → generic RtpPacketizer
///
/// @param pc           PeerConnection to add the track to.
/// @param codec        Video codec. Must name a supported RTP codec or FFmpeg
///                     encoder (e.g. "H264" or "libx264").
/// @param ssrc         RTP SSRC. 0 = auto-generate.
/// @param cname        RTCP CNAME. Empty = "icey".
/// @param nackBuffer   Max packets stored for NACK retransmission.
/// @param onPli        Callback when remote peer requests a keyframe.
///                     Connect to your encoder to force IDR.
/// @param onRemb       Callback when remote peer reports bandwidth estimate.
///                     Bitrate in bits/sec.
/// @return             TrackHandle with the track and its RTP config.
[[nodiscard]] WEBRTC_API TrackHandle createVideoTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::VideoCodec& codec,
    uint32_t ssrc = 0,
    const std::string& cname = {},
    unsigned nackBuffer = 512,
    std::function<void()> onPli = nullptr,
    std::function<void(unsigned int)> onRemb = nullptr);


/// Create an audio send track on a PeerConnection.
///
/// Sets up the outgoing media handler chain:
///   AudioRtpPacketizer → SrReporter
///
/// The packetizer clock rate is selected based on codec:
///   opus → 48kHz, PCMU/PCMA → 8kHz, etc.
///
/// @param pc           PeerConnection to add the track to.
/// @param codec        Audio codec. Must name a supported RTP codec or FFmpeg
///                     encoder (e.g. "opus" or "libopus").
/// @param ssrc         RTP SSRC. 0 = auto-generate.
/// @param cname        RTCP CNAME. Empty = "icey".
/// @return             TrackHandle with the track and its RTP config.
[[nodiscard]] WEBRTC_API TrackHandle createAudioTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::AudioCodec& codec,
    uint32_t ssrc = 0,
    const std::string& cname = {});


/// Set up the receive-side media handler chain on a remote track.
///
/// Selects the correct depacketizer based on the track's SDP codec:
///   Video: H264RtpDepacketizer, H265RtpDepacketizer, or generic
///   Audio: OpusRtpDepacketizer, PCMURtpDepacketizer, etc.
///
/// Also chains an RtcpReceivingSession for receiver reports.
///
/// Call this from PeerConnection::onTrack() before binding a
/// WebRtcTrackReceiver.
///
/// @param track        The remote track from onTrack callback.
/// @return             True when a supported depacketizer was installed.
[[nodiscard]] WEBRTC_API bool setupReceiveTrack(std::shared_ptr<rtc::Track> track);


/// Generate a random SSRC.
[[nodiscard]] WEBRTC_API uint32_t generateSsrc();


} // namespace wrtc
} // namespace icy


/// @\}
