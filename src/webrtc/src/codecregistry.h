#pragma once


#include "icy/webrtc/codecnegotiator.h"

#include <rtc/description.hpp>
#include <rtc/mediahandler.hpp>
#include <rtc/rtppacketizationconfig.hpp>

#ifdef HAVE_FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
}
#endif

#include <memory>
#include <optional>
#include <string_view>
#include <vector>


namespace icy {
namespace wrtc::codec_registry {


struct Entry
{
    CodecSpec spec;
#ifdef HAVE_FFMPEG
    AVCodecID decoderCodecId = AV_CODEC_ID_NONE;
#endif
};


[[nodiscard]] const Entry* findByRtp(std::string_view rtpName);
[[nodiscard]] const Entry* findByFfmpeg(std::string_view ffmpegName);
[[nodiscard]] std::optional<CodecSpec> detectInSdp(std::string_view sdp, CodecMediaType mediaType);
[[nodiscard]] std::optional<CodecSpec> detectInMedia(const rtc::Description::Media& media,
                                                     CodecMediaType mediaType);
[[nodiscard]] std::optional<NegotiatedCodec> negotiate(const std::vector<std::string>& offered,
                                                       CodecMediaType mediaType);

[[nodiscard]] bool hasEncoder(const std::string& ffmpegName);
[[nodiscard]] std::string ffmpegToRtp(std::string_view ffmpegName);

[[nodiscard]] const Entry* resolveVideo(const av::VideoCodec& codec);
[[nodiscard]] const Entry* resolveAudio(const av::AudioCodec& codec);

void addCodec(rtc::Description::Video& media, const CodecSpec& spec);
void addCodec(rtc::Description::Audio& media, const CodecSpec& spec);

[[nodiscard]] std::shared_ptr<rtc::MediaHandler>
makePacketizer(const CodecSpec& spec,
               const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig);

[[nodiscard]] std::shared_ptr<rtc::MediaHandler>
makeDepacketizer(const CodecSpec& spec);

void applyWebRtcDefaults(av::VideoCodec& codec, const CodecSpec& spec);
void applyWebRtcDefaults(av::AudioCodec& codec, const CodecSpec& spec);

#ifdef HAVE_FFMPEG
[[nodiscard]] AVCodecID decoderCodecId(const CodecSpec& spec);
#endif


} // namespace wrtc::codec_registry
} // namespace icy
