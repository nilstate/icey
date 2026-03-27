///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/codecnegotiator.h"
#include "codecregistry.h"
#include "icy/logger.h"

#include <stdexcept>


namespace icy {
namespace wrtc {


std::optional<NegotiatedCodec>
CodecNegotiator::negotiateVideo(const std::vector<std::string>& offeredCodecs)
{
    auto negotiated = codec_registry::negotiate(offeredCodecs, CodecMediaType::Video);
    if (negotiated)
        LDebug("Negotiated codec: ", negotiated->rtpName, " -> ", negotiated->ffmpegName);
    return negotiated;
}


std::optional<NegotiatedCodec>
CodecNegotiator::negotiateAudio(const std::vector<std::string>& offeredCodecs)
{
    auto negotiated = codec_registry::negotiate(offeredCodecs, CodecMediaType::Audio);
    if (negotiated)
        LDebug("Negotiated codec: ", negotiated->rtpName, " -> ", negotiated->ffmpegName);
    return negotiated;
}


bool CodecNegotiator::hasEncoder(const std::string& name)
{
    if (codec_registry::hasEncoder(name))
        return true;

    auto ffmpeg = rtpToFfmpeg(name);
    if (!ffmpeg.empty())
        return codec_registry::hasEncoder(ffmpeg);

    return false;
}


std::string CodecNegotiator::rtpToFfmpeg(const std::string& rtpName)
{
    if (auto* entry = codec_registry::findByRtp(rtpName);
        entry && codec_registry::hasEncoder(entry->spec.ffmpegName)) {
        return entry->spec.ffmpegName;
    }
    return {};
}


std::string CodecNegotiator::ffmpegToRtp(const std::string& ffmpegName)
{
    return codec_registry::ffmpegToRtp(ffmpegName);
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
    if (auto* entry = codec_registry::findByRtp(rtpName))
        return entry->spec;
    return std::nullopt;
}


std::optional<CodecSpec>
CodecNegotiator::specFromFfmpeg(const std::string& ffmpegName)
{
    if (auto* entry = codec_registry::findByFfmpeg(ffmpegName))
        return entry->spec;
    return std::nullopt;
}


std::optional<CodecSpec>
CodecNegotiator::specFromVideoCodec(const av::VideoCodec& codec)
{
    if (auto* entry = codec_registry::resolveVideo(codec))
        return entry->spec;
    return std::nullopt;
}


std::optional<CodecSpec>
CodecNegotiator::specFromAudioCodec(const av::AudioCodec& codec)
{
    if (auto* entry = codec_registry::resolveAudio(codec))
        return entry->spec;
    return std::nullopt;
}


CodecSpec CodecNegotiator::requireVideoSpec(const av::VideoCodec& codec)
{
    if (auto spec = specFromVideoCodec(codec);
        spec && spec->mediaType == CodecMediaType::Video) {
        return *spec;
    }

    if (!codec.specified())
        throw std::invalid_argument("Video track requires an explicit codec");
    if (!codec.encoder.empty())
        throw std::invalid_argument("Unsupported video encoder: " + codec.encoder);
    throw std::invalid_argument("Unsupported video codec: " + codec.name);
}


CodecSpec CodecNegotiator::requireAudioSpec(const av::AudioCodec& codec)
{
    if (auto spec = specFromAudioCodec(codec);
        spec && spec->mediaType == CodecMediaType::Audio) {
        return *spec;
    }

    if (!codec.specified())
        throw std::invalid_argument("Audio track requires an explicit codec");
    if (!codec.encoder.empty())
        throw std::invalid_argument("Unsupported audio encoder: " + codec.encoder);
    throw std::invalid_argument("Unsupported audio codec: " + codec.name);
}


av::VideoCodec CodecNegotiator::resolveWebRtcVideoCodec(const av::VideoCodec& codec)
{
    auto spec = requireVideoSpec(codec);
    auto resolved = NegotiatedCodec{
        spec.rtpName,
        spec.ffmpegName,
        spec.payloadType,
        spec.clockRate,
        spec.fmtp
    }.toWebRtcVideoCodec(codec.width, codec.height, codec.fps);
    resolved.sampleRate =
        codec.sampleRate > 0 ? codec.sampleRate : static_cast<int>(spec.clockRate);
    resolved.bitRate = codec.bitRate;
    resolved.quality = codec.quality;
    resolved.compliance = codec.compliance;
    resolved.enabled = codec.enabled;
    resolved.pixelFmt = codec.pixelFmt;
    for (const auto& [key, value] : codec.options)
        resolved.options[key] = value;
    return resolved;
}


av::AudioCodec CodecNegotiator::resolveWebRtcAudioCodec(const av::AudioCodec& codec)
{
    auto spec = requireAudioSpec(codec);
    auto resolved = NegotiatedCodec{
        spec.rtpName,
        spec.ffmpegName,
        spec.payloadType,
        spec.clockRate,
        spec.fmtp
    }.toWebRtcAudioCodec(codec.channels > 0 ? codec.channels : DEFAULT_AUDIO_CHANNELS);
    if (spec.id != CodecId::Opus && codec.sampleRate > 0)
        resolved.sampleRate = codec.sampleRate;
    resolved.bitRate = codec.bitRate;
    resolved.quality = codec.quality;
    resolved.compliance = codec.compliance;
    resolved.enabled = codec.enabled;
    if (!codec.sampleFmt.empty())
        resolved.sampleFmt = codec.sampleFmt;
    for (const auto& [key, value] : codec.options)
        resolved.options[key] = value;
    return resolved;
}


std::optional<CodecSpec>
CodecNegotiator::detectCodec(std::string_view sdp, CodecMediaType mediaType)
{
    return codec_registry::detectInSdp(sdp, mediaType);
}


std::optional<CodecSpec>
CodecNegotiator::detectCodecInMedia(const rtc::Description::Media& media,
                                    CodecMediaType mediaType)
{
    return codec_registry::detectInMedia(media, mediaType);
}


#ifdef HAVE_FFMPEG
AVCodecID CodecNegotiator::decoderCodecId(const CodecSpec& spec)
{
    return codec_registry::decoderCodecId(spec);
}
#endif


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
    if (auto* entry = codec_registry::findByRtp(rtpName))
        codec_registry::applyWebRtcDefaults(codec, entry->spec);
    return codec;
}


av::AudioCodec NegotiatedCodec::toWebRtcAudioCodec(int channels) const
{
    int rate = (rtpName == "opus") ? 48000 : static_cast<int>(clockRate);
    auto codec = av::AudioCodec(rtpName, ffmpegName, channels, rate);
    if (auto* entry = codec_registry::findByRtp(rtpName))
        codec_registry::applyWebRtcDefaults(codec, entry->spec);
    return codec;
}


} // namespace wrtc
} // namespace icy
