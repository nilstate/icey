///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "scy/webrtc/webrtc.h"

#include "scy/av/codec.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>


namespace scy {
namespace wrtc {


/// Result of codec negotiation between a remote SDP offer
/// and the local FFmpeg codec inventory.
struct WEBRTC_API NegotiatedCodec
{
    std::string rtpName;     ///< RTP codec name (e.g. "H264", "VP8", "opus")
    std::string ffmpegName;  ///< FFmpeg encoder name (e.g. "libx264", "libvpx", "libopus")
    int payloadType = 0;     ///< RTP payload type from SDP
    uint32_t clockRate = 0;  ///< RTP clock rate (90000 for video, 48000 for opus)
    std::string fmtp;        ///< Format parameters (e.g. "profile-level-id=42e01f")

    /// Create an av::VideoCodec from this negotiation result.
    /// Width, height, fps default to 0 (caller should set these).
    [[nodiscard]] av::VideoCodec toVideoCodec(int width = 0, int height = 0,
                                               double fps = 0.0) const;

    /// Create an av::AudioCodec from this negotiation result.
    /// Channels default to 2, sampleRate to the RTP clock rate.
    [[nodiscard]] av::AudioCodec toAudioCodec(int channels = 2,
                                               int sampleRate = 0) const;
};


/// Maps RTP codec names to FFmpeg encoders and queries FFmpeg
/// at runtime to determine what codecs are available.
///
/// This is stateless - all methods are static. Call negotiate*()
/// with a list of RTP codec names offered by the remote peer,
/// and it returns the best match that FFmpeg can encode.
class WEBRTC_API CodecNegotiator
{
public:
    /// Negotiate the best video codec from a list of offered RTP codec names.
    /// Returns the first match that FFmpeg can encode, in preference order:
    /// H264 > VP8 > VP9 > AV1 > H265
    [[nodiscard]] static std::optional<NegotiatedCodec>
    negotiateVideo(const std::vector<std::string>& offeredCodecs);

    /// Negotiate the best audio codec from a list of offered RTP codec names.
    /// Returns the first match that FFmpeg can encode, in preference order:
    /// opus > PCMU > PCMA
    [[nodiscard]] static std::optional<NegotiatedCodec>
    negotiateAudio(const std::vector<std::string>& offeredCodecs);

    /// Check if FFmpeg has an encoder for the given codec name.
    /// Accepts both FFmpeg names ("libx264") and RTP names ("H264").
    [[nodiscard]] static bool hasEncoder(const std::string& name);

    /// Map an RTP codec name to the preferred FFmpeg encoder name.
    /// Returns empty string if no mapping exists.
    [[nodiscard]] static std::string rtpToFfmpeg(const std::string& rtpName);

    /// Map an FFmpeg encoder name to the RTP codec name.
    /// Returns empty string if no mapping exists.
    [[nodiscard]] static std::string ffmpegToRtp(const std::string& ffmpegName);

    /// Get the standard RTP clock rate for a codec.
    [[nodiscard]] static uint32_t clockRate(const std::string& rtpName);

    /// Get the default RTP payload type for a codec.
    /// Returns 0 for dynamic payload types (caller should assign).
    [[nodiscard]] static int defaultPayloadType(const std::string& rtpName);
};


} // namespace wrtc
} // namespace scy


/// @\}
