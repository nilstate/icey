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
    CodecId id;
    CodecMediaType mediaType;
    const char* rtpName;
    const char* ffmpegEncoder;
    uint32_t clockRate;
    int defaultPT;  // 0 = dynamic (caller assigns)
    const char* fmtp;  // SDP format parameters (nullable)
};


// Ordered by preference within each category.
// Video: H264 preferred for browser compatibility.
// Audio: Opus preferred for quality at low bitrate.
constexpr std::array<CodecMapping, 10> kVideoCodecs = {{
    {CodecId::H264, CodecMediaType::Video, "H264",  "libx264",     90000, 96,  "profile-level-id=42e01f;packetization-mode=1"},
    {CodecId::H264, CodecMediaType::Video, "H264",  "h264_nvenc",  90000, 96,  "profile-level-id=42e01f;packetization-mode=1"},
    {CodecId::H264, CodecMediaType::Video, "H264",  "h264_vaapi",  90000, 96,  "profile-level-id=42e01f;packetization-mode=1"},
    {CodecId::VP8,  CodecMediaType::Video, "VP8",   "libvpx",      90000, 97,  nullptr},
    {CodecId::VP9,  CodecMediaType::Video, "VP9",   "libvpx-vp9",  90000, 98,  nullptr},
    {CodecId::AV1,  CodecMediaType::Video, "AV1",   "libaom-av1",  90000, 99,  nullptr},
    {CodecId::AV1,  CodecMediaType::Video, "AV1",   "libsvtav1",   90000, 99,  nullptr},
    {CodecId::AV1,  CodecMediaType::Video, "AV1",   "av1_nvenc",   90000, 99,  nullptr},
    {CodecId::H265, CodecMediaType::Video, "H265",  "libx265",     90000, 100, nullptr},
    {CodecId::H265, CodecMediaType::Video, "H265",  "hevc_nvenc",  90000, 100, nullptr},
}};


constexpr std::array<CodecMapping, 5> kAudioCodecs = {{
    {CodecId::Opus, CodecMediaType::Audio, "opus",  "libopus",     48000, 111, "minptime=10;useinbandfec=1"},
    {CodecId::PCMU, CodecMediaType::Audio, "PCMU",  "pcm_mulaw",   8000,  0,   nullptr},
    {CodecId::PCMA, CodecMediaType::Audio, "PCMA",  "pcm_alaw",    8000,  8,   nullptr},
    {CodecId::G722, CodecMediaType::Audio, "G722",  "g722",        8000,  9,   nullptr},
    {CodecId::AAC,  CodecMediaType::Audio, "AAC",   "aac",         48000, 0,   nullptr},
}};


/// Case-insensitive string comparison.
bool iequals(const std::string& a, const std::string& b)
{
    if (a.size() != b.size()) return false;
    return std::equal(a.begin(), a.end(), b.begin(),
        [](char ca, char cb) { return std::tolower(ca) == std::tolower(cb); });
}


bool icontains(std::string_view haystack, std::string_view needle)
{
    if (needle.empty() || haystack.size() < needle.size())
        return false;

    for (size_t i = 0; i <= haystack.size() - needle.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < needle.size(); ++j) {
            if (std::tolower(static_cast<unsigned char>(haystack[i + j])) !=
                std::tolower(static_cast<unsigned char>(needle[j]))) {
                match = false;
                break;
            }
        }
        if (match)
            return true;
    }
    return false;
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


CodecSpec makeSpec(const CodecMapping& entry)
{
    CodecSpec spec;
    spec.id = entry.id;
    spec.mediaType = entry.mediaType;
    spec.rtpName = entry.rtpName;
    spec.ffmpegName = entry.ffmpegEncoder;
    spec.clockRate = entry.clockRate;
    spec.payloadType = entry.defaultPT;
    if (entry.fmtp)
        spec.fmtp = entry.fmtp;
    return spec;
}


template <size_t N>
std::optional<CodecSpec>
findByRtp(const std::string& rtpName, const std::array<CodecMapping, N>& table)
{
    for (const auto& entry : table) {
        if (iequals(rtpName, entry.rtpName))
            return makeSpec(entry);
    }
    return std::nullopt;
}


template <size_t N>
std::optional<CodecSpec>
findByFfmpeg(const std::string& ffmpegName, const std::array<CodecMapping, N>& table)
{
    for (const auto& entry : table) {
        if (ffmpegName == entry.ffmpegEncoder)
            return makeSpec(entry);
    }
    return std::nullopt;
}


template <size_t N>
std::optional<CodecSpec>
detectInSdp(std::string_view sdp, const std::array<CodecMapping, N>& table)
{
    for (const auto& entry : table) {
        if (icontains(sdp, entry.rtpName))
            return makeSpec(entry);
    }
    return std::nullopt;
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
                if (entry.fmtp)
                    result.fmtp = entry.fmtp;
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
    if (auto spec = specFromRtp(rtpName))
        return spec->clockRate;
    return 0;
}


int CodecNegotiator::defaultPayloadType(const std::string& rtpName)
{
    if (auto spec = specFromRtp(rtpName))
        return spec->payloadType;
    return 0;
}


std::optional<CodecSpec>
CodecNegotiator::specFromRtp(const std::string& rtpName)
{
    if (auto spec = findByRtp(rtpName, kVideoCodecs))
        return spec;
    return findByRtp(rtpName, kAudioCodecs);
}


std::optional<CodecSpec>
CodecNegotiator::specFromFfmpeg(const std::string& ffmpegName)
{
    if (auto spec = findByFfmpeg(ffmpegName, kVideoCodecs))
        return spec;
    return findByFfmpeg(ffmpegName, kAudioCodecs);
}


std::optional<CodecSpec>
CodecNegotiator::specFromVideoCodec(const av::VideoCodec& codec)
{
    if (!codec.specified())
        return std::nullopt;

    if (!codec.encoder.empty())
        return specFromFfmpeg(codec.encoder);

    return specFromRtp(codec.name);
}


std::optional<CodecSpec>
CodecNegotiator::specFromAudioCodec(const av::AudioCodec& codec)
{
    if (!codec.specified())
        return std::nullopt;

    if (!codec.encoder.empty())
        return specFromFfmpeg(codec.encoder);

    return specFromRtp(codec.name);
}


std::optional<CodecSpec>
CodecNegotiator::detectCodec(std::string_view sdp, CodecMediaType mediaType)
{
    if (mediaType == CodecMediaType::Video)
        return detectInSdp(sdp, kVideoCodecs);
    return detectInSdp(sdp, kAudioCodecs);
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


av::VideoCodec NegotiatedCodec::toWebRtcVideoCodec(int width, int height, double fps) const
{
    auto codec = toVideoCodec(width, height, fps);

    if (rtpName == "H264") {
        codec.options["preset"] = "ultrafast";
        codec.options["tune"] = "zerolatency";
        codec.options["profile"] = "baseline";
    }
    else if (rtpName == "VP8") {
        codec.options["deadline"] = "realtime";
        codec.options["cpu-used"] = "8";
    }
    else if (rtpName == "VP9") {
        codec.options["deadline"] = "realtime";
        codec.options["cpu-used"] = "8";
        codec.options["row-mt"] = "1";
    }
    else if (rtpName == "AV1") {
        codec.options["preset"] = "12";
        codec.options["usage"] = "realtime";
    }
    else if (rtpName == "H265") {
        codec.options["preset"] = "ultrafast";
        codec.options["tune"] = "zerolatency";
    }

    return codec;
}


av::AudioCodec NegotiatedCodec::toWebRtcAudioCodec(int channels) const
{
    // Opus requires 48000 Hz; other codecs use their native clock rate.
    int rate = (rtpName == "opus") ? 48000 : static_cast<int>(clockRate);
    auto codec = av::AudioCodec(rtpName, ffmpegName, channels, rate);

    if (rtpName == "opus") {
        codec.sampleFmt = "flt";
        codec.options["application"] = "lowdelay";
    }

    return codec;
}


} // namespace wrtc
} // namespace icy
