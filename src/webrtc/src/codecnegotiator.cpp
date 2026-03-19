///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"

#ifdef HAVE_FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
}
#endif

#include <algorithm>
#include <array>
#include <cctype>


namespace icy {
namespace wrtc {


namespace {


struct CodecMapping
{
    const char* rtpName;
    const char* ffmpegEncoder;
    uint32_t clockRate;
    int defaultPT;  // 0 = dynamic (caller assigns)
};


// Ordered by preference within each category.
// Video: H264 preferred for browser compatibility.
// Audio: Opus preferred for quality at low bitrate.
constexpr std::array<CodecMapping, 10> kVideoCodecs = {{
    {"H264",  "libx264",     90000, 96},
    {"H264",  "h264_nvenc",  90000, 96},
    {"H264",  "h264_vaapi",  90000, 96},
    {"VP8",   "libvpx",      90000, 97},
    {"VP9",   "libvpx-vp9",  90000, 98},
    {"AV1",   "libaom-av1",  90000, 99},
    {"AV1",   "libsvtav1",   90000, 99},
    {"AV1",   "av1_nvenc",   90000, 99},
    {"H265",  "libx265",     90000, 100},
    {"H265",  "hevc_nvenc",  90000, 100},
}};


constexpr std::array<CodecMapping, 5> kAudioCodecs = {{
    {"opus",  "libopus",     48000, 111},
    {"PCMU",  "pcm_mulaw",   8000,  0},
    {"PCMA",  "pcm_alaw",    8000,  8},
    {"G722",  "g722",        8000,  9},
    {"AAC",   "aac",         48000, 0},
}};


/// Case-insensitive string comparison.
bool iequals(const std::string& a, const std::string& b)
{
    if (a.size() != b.size()) return false;
    return std::equal(a.begin(), a.end(), b.begin(),
        [](char ca, char cb) { return std::tolower(ca) == std::tolower(cb); });
}


/// Check if FFmpeg has an encoder by name.
bool ffmpegHasEncoder(const std::string& name)
{
#ifdef HAVE_FFMPEG
    const AVCodec* codec = avcodec_find_encoder_by_name(name.c_str());
    return codec != nullptr;
#else
    return false;
#endif
}


/// Search a codec table for the best match.
template <size_t N>
std::optional<NegotiatedCodec>
negotiate(const std::vector<std::string>& offered,
          const std::array<CodecMapping, N>& table)
{
    // For each offered codec (in caller's preference order),
    // find the first encoder FFmpeg supports.
    for (const auto& offer : offered) {
        for (const auto& entry : table) {
            if (iequals(offer, entry.rtpName) &&
                ffmpegHasEncoder(entry.ffmpegEncoder)) {
                NegotiatedCodec result;
                result.rtpName = entry.rtpName;
                result.ffmpegName = entry.ffmpegEncoder;
                result.payloadType = entry.defaultPT;
                result.clockRate = entry.clockRate;
                LDebug("Negotiated codec: ", entry.rtpName,
                       " -> ", entry.ffmpegEncoder);
                return result;
            }
        }
    }
    return std::nullopt;
}


} // anonymous namespace


std::optional<NegotiatedCodec>
CodecNegotiator::negotiateVideo(const std::vector<std::string>& offeredCodecs)
{
    return negotiate(offeredCodecs, kVideoCodecs);
}


std::optional<NegotiatedCodec>
CodecNegotiator::negotiateAudio(const std::vector<std::string>& offeredCodecs)
{
    return negotiate(offeredCodecs, kAudioCodecs);
}


bool CodecNegotiator::hasEncoder(const std::string& name)
{
    // Try direct FFmpeg name first
    if (ffmpegHasEncoder(name))
        return true;

    // Try mapping from RTP name
    auto ffmpeg = rtpToFfmpeg(name);
    if (!ffmpeg.empty())
        return ffmpegHasEncoder(ffmpeg);

    return false;
}


std::string CodecNegotiator::rtpToFfmpeg(const std::string& rtpName)
{
    // Return the first (highest-preference) encoder for this RTP name
    // that FFmpeg actually has.
    for (const auto& entry : kVideoCodecs) {
        if (iequals(rtpName, entry.rtpName) &&
            ffmpegHasEncoder(entry.ffmpegEncoder))
            return entry.ffmpegEncoder;
    }
    for (const auto& entry : kAudioCodecs) {
        if (iequals(rtpName, entry.rtpName) &&
            ffmpegHasEncoder(entry.ffmpegEncoder))
            return entry.ffmpegEncoder;
    }

    // Fallback: return first mapping even if encoder isn't available
    for (const auto& entry : kVideoCodecs) {
        if (iequals(rtpName, entry.rtpName))
            return entry.ffmpegEncoder;
    }
    for (const auto& entry : kAudioCodecs) {
        if (iequals(rtpName, entry.rtpName))
            return entry.ffmpegEncoder;
    }

    return {};
}


std::string CodecNegotiator::ffmpegToRtp(const std::string& ffmpegName)
{
    for (const auto& entry : kVideoCodecs) {
        if (ffmpegName == entry.ffmpegEncoder)
            return entry.rtpName;
    }
    for (const auto& entry : kAudioCodecs) {
        if (ffmpegName == entry.ffmpegEncoder)
            return entry.rtpName;
    }
    return {};
}


uint32_t CodecNegotiator::clockRate(const std::string& rtpName)
{
    for (const auto& entry : kVideoCodecs) {
        if (iequals(rtpName, entry.rtpName))
            return entry.clockRate;
    }
    for (const auto& entry : kAudioCodecs) {
        if (iequals(rtpName, entry.rtpName))
            return entry.clockRate;
    }
    return 0;
}


int CodecNegotiator::defaultPayloadType(const std::string& rtpName)
{
    for (const auto& entry : kVideoCodecs) {
        if (iequals(rtpName, entry.rtpName))
            return entry.defaultPT;
    }
    for (const auto& entry : kAudioCodecs) {
        if (iequals(rtpName, entry.rtpName))
            return entry.defaultPT;
    }
    return 0;
}


// ---------------------------------------------------------------------------
// NegotiatedCodec conversion to AV types
// ---------------------------------------------------------------------------

av::VideoCodec NegotiatedCodec::toVideoCodec(int width, int height, double fps) const
{
    return av::VideoCodec(rtpName, ffmpegName, width, height, fps);
}


av::AudioCodec NegotiatedCodec::toAudioCodec(int channels, int sampleRate) const
{
    int rate = sampleRate > 0 ? sampleRate : static_cast<int>(clockRate);
    return av::AudioCodec(rtpName, ffmpegName, channels, rate);
}


} // namespace wrtc
} // namespace icy
