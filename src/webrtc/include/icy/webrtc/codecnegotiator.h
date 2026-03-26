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


#include "icy/webrtc/webrtc.h"

#include "icy/av/codec.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


namespace icy {
/// WebRTC codec negotiation and track setup helpers.
namespace wrtc {


/// Whether a codec carries audio or video media.
enum class CodecMediaType
{
    Video, ///< Video RTP payload.
    Audio  ///< Audio RTP payload.
};


/// Stable codec identifiers used across negotiation and track setup.
enum class CodecId
{
    Unknown, ///< Unrecognized or unsupported codec.
    H264,    ///< H.264 / AVC.
    H265,    ///< H.265 / HEVC.
    VP8,     ///< Google VP8.
    VP9,     ///< Google VP9.
    AV1,     ///< AOMedia AV1.
    Opus,    ///< Opus audio.
    PCMU,    ///< G.711 mu-law audio.
    PCMA,    ///< G.711 A-law audio.
    G722,    ///< G.722 wideband audio.
    AAC      ///< AAC audio.
};


/// Canonical description of a codec supported by icey's WebRTC helpers.
struct WEBRTC_API CodecSpec
{
    CodecId id = CodecId::Unknown;             ///< Stable codec identifier.
    CodecMediaType mediaType = CodecMediaType::Video; ///< Audio or video media kind.
    std::string rtpName;                       ///< Canonical RTP codec name.
    std::string ffmpegName;                    ///< Preferred FFmpeg encoder name.
    uint32_t clockRate = 0;                    ///< RTP clock rate in Hz.
    int payloadType = 0;                       ///< Default static or preferred dynamic payload type.
    std::string fmtp;                          ///< Canonical fmtp line for SDP generation.

    [[nodiscard]] bool valid() const { return id != CodecId::Unknown; }
};


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

    /// Create an av::VideoCodec configured for WebRTC browser playback.
    /// Sets low-latency options: ultrafast preset, zerolatency tune,
    /// constrained baseline profile for H.264, and appropriate defaults
    /// for VP8/VP9/AV1.
    [[nodiscard]] av::VideoCodec toWebRtcVideoCodec(int width = 0, int height = 0,
                                                     double fps = 0.0) const;

    /// Create an av::AudioCodec from this negotiation result.
    /// Channels default to 2, sampleRate to the RTP clock rate.
    [[nodiscard]] av::AudioCodec toAudioCodec(int channels = 2,
                                               int sampleRate = 0) const;

    /// Create an av::AudioCodec configured for WebRTC browser playback.
    /// Forces 48000 Hz for Opus, sets appropriate options.
    [[nodiscard]] av::AudioCodec toWebRtcAudioCodec(int channels = 2) const;
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

    /// Return the canonical codec spec for an RTP name, if known.
    [[nodiscard]] static std::optional<CodecSpec>
    specFromRtp(const std::string& rtpName);

    /// Return the canonical codec spec for an FFmpeg encoder name, if known.
    [[nodiscard]] static std::optional<CodecSpec>
    specFromFfmpeg(const std::string& ffmpegName);

    /// Resolve the canonical codec spec from an explicit video codec config.
    /// Prefers the FFmpeg encoder when present, otherwise falls back to RTP name.
    [[nodiscard]] static std::optional<CodecSpec>
    specFromVideoCodec(const av::VideoCodec& codec);

    /// Resolve the canonical codec spec from an explicit audio codec config.
    /// Prefers the FFmpeg encoder when present, otherwise falls back to RTP name.
    [[nodiscard]] static std::optional<CodecSpec>
    specFromAudioCodec(const av::AudioCodec& codec);

    /// Resolve a strict canonical video codec spec or throw.
    [[nodiscard]] static CodecSpec requireVideoSpec(const av::VideoCodec& codec);

    /// Resolve a strict canonical audio codec spec or throw.
    [[nodiscard]] static CodecSpec requireAudioSpec(const av::AudioCodec& codec);

    /// Resolve a browser-safe WebRTC video codec config from an explicit codec.
    [[nodiscard]] static av::VideoCodec
    resolveWebRtcVideoCodec(const av::VideoCodec& codec);

    /// Resolve a browser-safe WebRTC audio codec config from an explicit codec.
    [[nodiscard]] static av::AudioCodec
    resolveWebRtcAudioCodec(const av::AudioCodec& codec);

    /// Detect the first known codec present in an SDP snippet for the given
    /// media type.
    [[nodiscard]] static std::optional<CodecSpec>
    detectCodec(std::string_view sdp, CodecMediaType mediaType);
};


} // namespace wrtc
} // namespace icy


/// @}
