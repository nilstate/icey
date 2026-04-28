#include "codecregistry.h"

#include <rtc/h264rtpdepacketizer.hpp>
#include <rtc/h264rtppacketizer.hpp>
#include <rtc/h265rtpdepacketizer.hpp>
#include <rtc/h265rtppacketizer.hpp>
#include <rtc/rtp.hpp>
#include <rtc/rtpdepacketizer.hpp>
#include <rtc/rtppacketizer.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <mutex>
#include <stdexcept>
#include <unordered_map>


namespace icy {
namespace wrtc::codec_registry {

// Forward declaration so the anonymous-namespace helpers below can call
// hasEncoder, which is defined further down in this same translation unit
// at codec_registry scope (it's part of the public API).
bool hasEncoder(const std::string& ffmpegName);

namespace {


class Vp8RtpDepacketizer final : public rtc::VideoRtpDepacketizer
{
protected:
    rtc::message_ptr reassemble(message_buffer& buffer) override
    {
        if (buffer.empty())
            return nullptr;

        auto first = *buffer.begin();
        auto firstHeader = reinterpret_cast<const rtc::RtpHeader*>(first->data());
        const auto payloadType = firstHeader->payloadType();
        const auto timestamp = firstHeader->timestamp();

        rtc::binary frame;
        for (const auto& packet : buffer) {
            auto rtpHeader = reinterpret_cast<const rtc::RtpHeader*>(packet->data());
            auto rtpHeaderSize = rtpHeader->getSize() + rtpHeader->getExtensionHeaderSize();
            size_t paddingSize = rtpHeader->padding()
                ? std::to_integer<uint8_t>(packet->back())
                : 0;

            if (packet->size() <= rtpHeaderSize + paddingSize)
                continue;

            size_t payloadOffset = rtpHeaderSize;
            const size_t payloadEnd = packet->size() - paddingSize;
            if (!skipPayloadDescriptor(*packet, payloadOffset, payloadEnd))
                continue;

            frame.insert(frame.end(),
                         packet->begin() + payloadOffset,
                         packet->begin() + payloadEnd);
        }

        if (frame.empty())
            return nullptr;

        return rtc::make_message(std::move(frame), createFrameInfo(timestamp, payloadType));
    }

private:
    static bool skipPayloadDescriptor(const rtc::Message& packet,
                                      size_t& offset,
                                      size_t payloadEnd)
    {
        if (offset >= payloadEnd)
            return false;

        const auto descriptor = std::to_integer<uint8_t>(packet.at(offset++));
        if (!(descriptor & 0x80))
            return offset <= payloadEnd;

        if (offset >= payloadEnd)
            return false;

        const auto extension = std::to_integer<uint8_t>(packet.at(offset++));
        const bool hasPictureId = extension & 0x80;
        const bool hasTl0PicIdx = extension & 0x40;
        const bool hasTid = extension & 0x20;
        const bool hasKeyIdx = extension & 0x10;

        if (hasPictureId) {
            if (offset >= payloadEnd)
                return false;
            const auto pictureId = std::to_integer<uint8_t>(packet.at(offset++));
            if ((pictureId & 0x80) != 0) {
                if (offset >= payloadEnd)
                    return false;
                ++offset;
            }
        }

        if (hasTl0PicIdx) {
            if (offset >= payloadEnd)
                return false;
            ++offset;
        }

        if (hasTid || hasKeyIdx) {
            if (offset >= payloadEnd)
                return false;
            ++offset;
        }

        return offset <= payloadEnd;
    }
};


bool iequals(std::string_view a, std::string_view b)
{
    if (a.size() != b.size())
        return false;
    return std::equal(a.begin(), a.end(), b.begin(),
        [](char ca, char cb) {
            return std::tolower(static_cast<unsigned char>(ca)) ==
                   std::tolower(static_cast<unsigned char>(cb));
        });
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


// Order matters. Entries are walked in sequence; the first one whose ffmpeg
// encoder is linked (hasEncoder() == true) wins for both SDP negotiation and
// the auto-pick path used by callers that pass an empty encoder name. Place
// hardware encoders ahead of libx264 so a build that includes
// VideoToolbox / NVENC / VA-API / QSV / AMF will use the OS hardware path
// by default; libx264 stays as the universal software fallback. On builds
// that don't include a given hardware encoder, hasEncoder() returns false
// and the walk falls through cleanly.
const std::array<Entry, 13> kVideoEntries = {{
    {CodecSpec{CodecId::H264, CodecMediaType::Video, "H264", "h264_videotoolbox", 90000, 96,
               "profile-level-id=42e01f;packetization-mode=1"}, AV_CODEC_ID_H264},
    {CodecSpec{CodecId::H264, CodecMediaType::Video, "H264", "h264_nvenc", 90000, 96,
               "profile-level-id=42e01f;packetization-mode=1"}, AV_CODEC_ID_H264},
    {CodecSpec{CodecId::H264, CodecMediaType::Video, "H264", "h264_vaapi", 90000, 96,
               "profile-level-id=42e01f;packetization-mode=1"}, AV_CODEC_ID_H264},
    {CodecSpec{CodecId::H264, CodecMediaType::Video, "H264", "h264_qsv", 90000, 96,
               "profile-level-id=42e01f;packetization-mode=1"}, AV_CODEC_ID_H264},
    {CodecSpec{CodecId::H264, CodecMediaType::Video, "H264", "h264_amf", 90000, 96,
               "profile-level-id=42e01f;packetization-mode=1"}, AV_CODEC_ID_H264},
    {CodecSpec{CodecId::H264, CodecMediaType::Video, "H264", "libx264", 90000, 96,
               "profile-level-id=42e01f;packetization-mode=1"}, AV_CODEC_ID_H264},
    {CodecSpec{CodecId::VP8, CodecMediaType::Video, "VP8", "libvpx", 90000, 97, {}}, AV_CODEC_ID_VP8},
    {CodecSpec{CodecId::VP9, CodecMediaType::Video, "VP9", "libvpx-vp9", 90000, 98, {}}, AV_CODEC_ID_VP9},
    {CodecSpec{CodecId::AV1, CodecMediaType::Video, "AV1", "av1_nvenc", 90000, 99, {}}, AV_CODEC_ID_AV1},
    {CodecSpec{CodecId::AV1, CodecMediaType::Video, "AV1", "libsvtav1", 90000, 99, {}}, AV_CODEC_ID_AV1},
    {CodecSpec{CodecId::AV1, CodecMediaType::Video, "AV1", "libaom-av1", 90000, 99, {}}, AV_CODEC_ID_AV1},
    {CodecSpec{CodecId::H265, CodecMediaType::Video, "H265", "hevc_nvenc", 90000, 100, {}}, AV_CODEC_ID_HEVC},
    {CodecSpec{CodecId::H265, CodecMediaType::Video, "H265", "libx265", 90000, 100, {}}, AV_CODEC_ID_HEVC},
}};


const std::array<Entry, 5> kAudioEntries = {{
    {CodecSpec{CodecId::Opus, CodecMediaType::Audio, "opus", "libopus", 48000, 111,
               "minptime=10;useinbandfec=1"}, AV_CODEC_ID_OPUS},
    {CodecSpec{CodecId::PCMU, CodecMediaType::Audio, "PCMU", "pcm_mulaw", 8000, 0, {}}, AV_CODEC_ID_PCM_MULAW},
    {CodecSpec{CodecId::PCMA, CodecMediaType::Audio, "PCMA", "pcm_alaw", 8000, 8, {}}, AV_CODEC_ID_PCM_ALAW},
    {CodecSpec{CodecId::G722, CodecMediaType::Audio, "G722", "g722", 8000, 9, {}}, AV_CODEC_ID_ADPCM_G722},
    {CodecSpec{CodecId::AAC, CodecMediaType::Audio, "AAC", "aac", 48000, 0, {}}, AV_CODEC_ID_AAC},
}};


template <size_t N>
const Entry* findByRtpInTable(std::string_view rtpName, const std::array<Entry, N>& table)
{
    // Walk all entries with the given RTP name and return the first one
    // whose ffmpeg encoder is linked at runtime. Combined with the table's
    // hardware-first ordering, this picks the platform-best encoder
    // automatically. A name with no usable encoder returns nullptr instead
    // of an unusable entry.
    for (const auto& entry : table) {
        if (iequals(rtpName, entry.spec.rtpName) && hasEncoder(entry.spec.ffmpegName))
            return &entry;
    }
    return nullptr;
}


template <size_t N>
const Entry* findByFfmpegInTable(std::string_view ffmpegName, const std::array<Entry, N>& table)
{
    for (const auto& entry : table) {
        if (ffmpegName == entry.spec.ffmpegName)
            return &entry;
    }
    return nullptr;
}


template <size_t N>
std::optional<CodecSpec> detectInTable(std::string_view sdp, const std::array<Entry, N>& table)
{
    for (const auto& entry : table) {
        if (icontains(sdp, entry.spec.rtpName))
            return entry.spec;
    }
    return std::nullopt;
}


template <size_t N>
std::optional<CodecSpec> detectInMediaTable(const rtc::Description::Media& media,
                                            const std::array<Entry, N>& table)
{
    for (int payloadType : media.payloadTypes()) {
        const auto* map = media.rtpMap(payloadType);
        if (!map)
            continue;

        for (const auto& entry : table) {
            if (iequals(map->format, entry.spec.rtpName))
                return entry.spec;
        }
    }

    return std::nullopt;
}


template <size_t N>
std::optional<NegotiatedCodec> negotiateInTable(const std::vector<std::string>& offered,
                                                const std::array<Entry, N>& table)
{
    for (const auto& offer : offered) {
        for (const auto& entry : table) {
            if (iequals(offer, entry.spec.rtpName) && hasEncoder(entry.spec.ffmpegName)) {
                NegotiatedCodec result;
                result.rtpName = entry.spec.rtpName;
                result.ffmpegName = entry.spec.ffmpegName;
                result.payloadType = entry.spec.payloadType;
                result.clockRate = entry.spec.clockRate;
                result.fmtp = entry.spec.fmtp;
                return result;
            }
        }
    }
    return std::nullopt;
}


} // namespace


const Entry* findByRtp(std::string_view rtpName)
{
    if (auto* entry = findByRtpInTable(rtpName, kVideoEntries))
        return entry;
    return findByRtpInTable(rtpName, kAudioEntries);
}


const Entry* findByFfmpeg(std::string_view ffmpegName)
{
    if (auto* entry = findByFfmpegInTable(ffmpegName, kVideoEntries))
        return entry;
    return findByFfmpegInTable(ffmpegName, kAudioEntries);
}


std::optional<CodecSpec> detectInSdp(std::string_view sdp, CodecMediaType mediaType)
{
    if (mediaType == CodecMediaType::Video)
        return detectInTable(sdp, kVideoEntries);
    return detectInTable(sdp, kAudioEntries);
}


std::optional<CodecSpec> detectInMedia(const rtc::Description::Media& media,
                                       CodecMediaType mediaType)
{
    if (mediaType == CodecMediaType::Video)
        return detectInMediaTable(media, kVideoEntries);
    return detectInMediaTable(media, kAudioEntries);
}


std::optional<NegotiatedCodec> negotiate(const std::vector<std::string>& offered,
                                         CodecMediaType mediaType)
{
    if (mediaType == CodecMediaType::Video)
        return negotiateInTable(offered, kVideoEntries);
    return negotiateInTable(offered, kAudioEntries);
}


bool hasEncoder(const std::string& ffmpegName)
{
    // Result is cached: probing involves alloc + open + free of an
    // AVCodecContext, which is cheap once but adds up over repeated
    // SDP negotiations.
    static std::mutex cacheMutex;
    static std::unordered_map<std::string, bool> cache;

    {
        std::lock_guard lock(cacheMutex);
        if (auto it = cache.find(ffmpegName); it != cache.end())
            return it->second;
    }

    bool result = false;
    const AVCodec* codec = avcodec_find_encoder_by_name(ffmpegName.c_str());
    if (codec) {
        // Build-time presence is necessary but not sufficient: hardware
        // encoders like h264_nvenc, h264_vaapi, h264_videotoolbox link
        // against runtime resources (CUDA, VA-API drivers, VideoToolbox
        // session) that may be missing on the host. Probe by allocating
        // a context and opening it with a minimal config; treat any
        // failure as "not available" so codec selection can fall back.
        AVCodecContext* ctx = avcodec_alloc_context3(codec);
        if (ctx) {
            if (codec->type == AVMEDIA_TYPE_VIDEO) {
                ctx->width = 64;
                ctx->height = 64;
                ctx->pix_fmt = (codec->pix_fmts && codec->pix_fmts[0] != AV_PIX_FMT_NONE)
                    ? codec->pix_fmts[0]
                    : AV_PIX_FMT_YUV420P;
                ctx->time_base = AVRational{1, 30};
                ctx->framerate = AVRational{30, 1};
            } else if (codec->type == AVMEDIA_TYPE_AUDIO) {
                ctx->sample_rate = (codec->supported_samplerates &&
                                    codec->supported_samplerates[0] != 0)
                    ? codec->supported_samplerates[0]
                    : 48000;
                ctx->sample_fmt = (codec->sample_fmts &&
                                   codec->sample_fmts[0] != AV_SAMPLE_FMT_NONE)
                    ? codec->sample_fmts[0]
                    : AV_SAMPLE_FMT_FLT;
                av_channel_layout_default(&ctx->ch_layout, 2);
            }
            result = avcodec_open2(ctx, codec, nullptr) == 0;
            avcodec_free_context(&ctx);
        }
    }

    {
        std::lock_guard lock(cacheMutex);
        cache.emplace(ffmpegName, result);
    }
    return result;
}


std::string ffmpegToRtp(std::string_view ffmpegName)
{
    if (auto* entry = findByFfmpeg(ffmpegName))
        return entry->spec.rtpName;
    return {};
}


const Entry* resolveVideo(const av::VideoCodec& codec)
{
    if (!codec.specified())
        return nullptr;
    if (!codec.encoder.empty()) {
        // Try the explicit ffmpeg encoder name first ("libx264",
        // "h264_videotoolbox", ...). If it doesn't match, treat the string
        // as a codec alias ("h264", "vp8", ...) and resolve via the RTP
        // name lookup, which returns the platform-best available encoder.
        if (auto* entry = findByFfmpeg(codec.encoder))
            return entry;
        return findByRtp(codec.encoder);
    }
    return findByRtp(codec.name);
}


const Entry* resolveAudio(const av::AudioCodec& codec)
{
    if (!codec.specified())
        return nullptr;
    if (!codec.encoder.empty()) {
        if (auto* entry = findByFfmpeg(codec.encoder))
            return entry;
        return findByRtp(codec.encoder);
    }
    return findByRtp(codec.name);
}


void addCodec(rtc::Description::Video& media, const CodecSpec& spec)
{
    switch (spec.id) {
    case CodecId::H264:
        media.addH264Codec(spec.payloadType);
        break;
    case CodecId::VP8:
        media.addVP8Codec(spec.payloadType);
        break;
    case CodecId::VP9:
        media.addVP9Codec(spec.payloadType);
        break;
    case CodecId::H265:
        media.addH265Codec(spec.payloadType);
        break;
    default:
        throw std::invalid_argument("Unsupported WebRTC video send codec: " + spec.rtpName);
    }
}


void addCodec(rtc::Description::Audio& media, const CodecSpec& spec)
{
    switch (spec.id) {
    case CodecId::Opus:
        media.addOpusCodec(spec.payloadType);
        break;
    case CodecId::PCMU:
        media.addPCMUCodec(spec.payloadType);
        break;
    case CodecId::PCMA:
        media.addPCMACodec(spec.payloadType);
        break;
    default:
        throw std::invalid_argument("Unsupported WebRTC audio send codec: " + spec.rtpName);
    }
}


std::shared_ptr<rtc::MediaHandler>
makePacketizer(const CodecSpec& spec,
               const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig)
{
    switch (spec.id) {
    case CodecId::H264:
        return std::make_shared<rtc::H264RtpPacketizer>(
            rtc::NalUnit::Separator::StartSequence, rtpConfig);
    case CodecId::H265:
        return std::make_shared<rtc::H265RtpPacketizer>(
            rtc::NalUnit::Separator::StartSequence, rtpConfig);
    case CodecId::Opus:
        return std::make_shared<rtc::OpusRtpPacketizer>(rtpConfig);
    default:
        return std::make_shared<rtc::RtpPacketizer>(rtpConfig);
    }
}


std::shared_ptr<rtc::MediaHandler> makeDepacketizer(const CodecSpec& spec)
{
    switch (spec.id) {
    case CodecId::H264:
        return std::make_shared<rtc::H264RtpDepacketizer>();
    case CodecId::H265:
        return std::make_shared<rtc::H265RtpDepacketizer>();
    case CodecId::VP8:
        return std::make_shared<Vp8RtpDepacketizer>();
    case CodecId::PCMU:
        return std::make_shared<rtc::PCMURtpDepacketizer>();
    case CodecId::PCMA:
        return std::make_shared<rtc::PCMARtpDepacketizer>();
    case CodecId::Opus:
        return std::make_shared<rtc::OpusRtpDepacketizer>();
    default:
        return std::make_shared<rtc::RtpDepacketizer>(
            spec.clockRate ? spec.clockRate
                           : (spec.mediaType == CodecMediaType::Video ? 90000 : 48000));
    }
}


// Encoder-specific low-latency option presets for H.264. Each ffmpeg encoder
// has its own option vocabulary; setting "preset=ultrafast" on a hardware
// encoder gets ignored by ffmpeg's av_opt_set with a warning, which is the
// same behaviour as the previous code, but here we set the right knobs so
// the hardware path actually runs in low-latency mode.
void applyH264EncoderDefaults(av::VideoCodec& codec)
{
    const auto& enc = codec.encoder;
    if (enc == "h264_videotoolbox") {
        codec.options["realtime"] = "1";
        codec.options["allow_sw"] = "0";
        codec.options["profile"] = "baseline";
    } else if (enc == "h264_nvenc") {
        codec.options["preset"] = "p1";
        codec.options["tune"] = "ll";
        codec.options["zerolatency"] = "1";
        codec.options["profile"] = "baseline";
    } else if (enc == "h264_vaapi") {
        codec.options["low_power"] = "1";
        codec.options["idr_interval"] = "60";
        codec.options["profile"] = "constrained_baseline";
    } else if (enc == "h264_qsv") {
        codec.options["preset"] = "veryfast";
        codec.options["look_ahead"] = "0";
        codec.options["profile"] = "baseline";
    } else if (enc == "h264_amf") {
        codec.options["usage"] = "ultralowlatency";
        codec.options["quality"] = "speed";
        codec.options["profile"] = "constrained_baseline";
    } else {
        // libx264 and any other software fallback.
        codec.options["preset"] = "ultrafast";
        codec.options["tune"] = "zerolatency";
        codec.options["profile"] = "baseline";
    }
}


void applyWebRtcDefaults(av::VideoCodec& codec, const CodecSpec& spec)
{
    switch (spec.id) {
    case CodecId::H264:
        applyH264EncoderDefaults(codec);
        break;
    case CodecId::VP8:
        codec.options["deadline"] = "realtime";
        codec.options["cpu-used"] = "8";
        break;
    case CodecId::VP9:
        codec.options["deadline"] = "realtime";
        codec.options["cpu-used"] = "8";
        codec.options["row-mt"] = "1";
        break;
    case CodecId::AV1:
        codec.options["preset"] = "12";
        codec.options["usage"] = "realtime";
        break;
    case CodecId::H265:
        codec.options["preset"] = "ultrafast";
        codec.options["tune"] = "zerolatency";
        break;
    default:
        break;
    }
}


void applyWebRtcDefaults(av::AudioCodec& codec, const CodecSpec& spec)
{
    if (spec.id == CodecId::Opus) {
        codec.sampleFmt = "flt";
        codec.options["application"] = "lowdelay";
    }
}


AVCodecID decoderCodecId(const CodecSpec& spec)
{
    if (auto* entry = findByRtp(spec.rtpName))
        return entry->decoderCodecId;
    return AV_CODEC_ID_NONE;
}


} // namespace wrtc::codec_registry
} // namespace icy
