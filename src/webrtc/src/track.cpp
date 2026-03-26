///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/track.h"

#include "codecregistry.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"

#include <random>
#include <stdexcept>


namespace icy {
namespace wrtc {


namespace {


void stripUnsupportedReceiveFormats(rtc::Description::Media& media)
{
    std::vector<int> auxPayloadTypes;
    for (int pt : media.payloadTypes()) {
        const auto* map = media.rtpMap(pt);
        if (!map)
            continue;
        for (const auto& fmtp : map->fmtps) {
            if (fmtp.rfind("apt=", 0) == 0) {
                auxPayloadTypes.push_back(pt);
                break;
            }
        }
    }

    for (int pt : auxPayloadTypes)
        media.removeRtpMap(pt);
}


std::shared_ptr<rtc::MediaHandler>
makeVideoHandlerChain(const CodecSpec& spec,
                      const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig,
                      unsigned nackBuffer,
                      std::function<void()> onPli,
                      std::function<void(unsigned int)> onRemb)
{
    auto depacketizer = codec_registry::makeDepacketizer(spec);
    auto packetizer = codec_registry::makePacketizer(spec, rtpConfig);
    depacketizer->addToChain(packetizer);

    packetizer->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
    packetizer->addToChain(std::make_shared<rtc::RtcpNackResponder>(nackBuffer));

    if (onPli)
        packetizer->addToChain(std::make_shared<rtc::PliHandler>(std::move(onPli)));
    if (onRemb)
        packetizer->addToChain(std::make_shared<rtc::RembHandler>(std::move(onRemb)));

    packetizer->addToChain(std::make_shared<rtc::RtcpReceivingSession>());
    return depacketizer;
}


std::shared_ptr<rtc::MediaHandler>
makeAudioHandlerChain(const CodecSpec& spec,
                      const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig)
{
    auto depacketizer = codec_registry::makeDepacketizer(spec);
    auto packetizer = codec_registry::makePacketizer(spec, rtpConfig);
    depacketizer->addToChain(packetizer);
    packetizer->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
    packetizer->addToChain(std::make_shared<rtc::RtcpReceivingSession>());
    return depacketizer;
}


} // namespace


uint32_t generateSsrc()
{
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(1, 0xFFFFFFFE);
    return dist(gen);
}


TrackHandle createVideoTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::VideoCodec& codec,
    uint32_t ssrc,
    const std::string& cname,
    const std::string& mid,
    rtc::Description::Direction direction,
    unsigned nackBuffer,
    std::function<void()> onPli,
    std::function<void(unsigned int)> onRemb,
    int payloadType)
{
    auto spec = CodecNegotiator::requireVideoSpec(codec);
    if (payloadType >= 0)
        spec.payloadType = payloadType;

    if (ssrc == 0)
        ssrc = generateSsrc();
    std::string cn = cname.empty() ? "icey" : cname;

    rtc::Description::Video media(mid.empty() ? "video" : mid, direction);
    codec_registry::addCodec(media, spec);
    media.addSSRC(ssrc, cn, cn, mid.empty() ? "video" : mid);

    auto track = pc->addTrack(media);
    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, cn, spec.payloadType, spec.clockRate);

    track->setMediaHandler(makeVideoHandlerChain(
        spec, rtpConfig, nackBuffer, std::move(onPli), std::move(onRemb)));

    LInfo("Video track created: ", spec.rtpName, " SSRC=", ssrc, " PT=", spec.payloadType);
    return {track, rtpConfig};
}


TrackHandle createAudioTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::AudioCodec& codec,
    uint32_t ssrc,
    const std::string& cname,
    const std::string& mid,
    rtc::Description::Direction direction,
    int payloadType)
{
    auto spec = CodecNegotiator::requireAudioSpec(codec);
    if (payloadType >= 0)
        spec.payloadType = payloadType;

    if (ssrc == 0)
        ssrc = generateSsrc();
    std::string cn = cname.empty() ? "icey" : cname;

    rtc::Description::Audio media(mid.empty() ? "audio" : mid, direction);
    codec_registry::addCodec(media, spec);
    media.addSSRC(ssrc, cn, cn, mid.empty() ? "audio" : mid);

    auto track = pc->addTrack(media);
    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, cn, spec.payloadType, spec.clockRate);

    track->setMediaHandler(makeAudioHandlerChain(spec, rtpConfig));

    LInfo("Audio track created: ", spec.rtpName, " SSRC=", ssrc, " PT=", spec.payloadType);
    return {track, rtpConfig};
}


std::shared_ptr<rtc::Track> createVideoReceiveTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::VideoCodec& codec,
    const std::string& mid,
    rtc::Description::Direction direction,
    int payloadType)
{
    auto spec = CodecNegotiator::requireVideoSpec(codec);
    if (payloadType >= 0)
        spec.payloadType = payloadType;

    rtc::Description::Video media(mid.empty() ? "video" : mid, direction);
    codec_registry::addCodec(media, spec);

    auto track = pc->addTrack(media);
    if (!setupReceiveTrack(track))
        throw std::runtime_error("Unsupported negotiated video codec");

    LInfo("Video receive track created: ", spec.rtpName, " PT=", spec.payloadType);
    return track;
}


std::shared_ptr<rtc::Track> createAudioReceiveTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::AudioCodec& codec,
    const std::string& mid,
    rtc::Description::Direction direction,
    int payloadType)
{
    auto spec = CodecNegotiator::requireAudioSpec(codec);
    if (payloadType >= 0)
        spec.payloadType = payloadType;

    rtc::Description::Audio media(mid.empty() ? "audio" : mid, direction);
    codec_registry::addCodec(media, spec);

    auto track = pc->addTrack(media);
    if (!setupReceiveTrack(track))
        throw std::runtime_error("Unsupported negotiated audio codec");

    LInfo("Audio receive track created: ", spec.rtpName, " PT=", spec.payloadType);
    return track;
}


bool setupReceiveTrack(std::shared_ptr<rtc::Track> track)
{
    auto desc = track->description();
    stripUnsupportedReceiveFormats(desc);
    track->setDescription(desc);

    if (desc.type() == "video") {
        auto spec = codec_registry::detectInMedia(desc, CodecMediaType::Video);
        if (!spec) {
            LWarn("Unsupported remote video codec on track");
            return false;
        }

        track->setMediaHandler(codec_registry::makeDepacketizer(*spec));
        track->chainMediaHandler(std::make_shared<rtc::RtcpReceivingSession>());

        LDebug("Receive video track set up: ", spec->rtpName, " PT=", spec->payloadType);
        return true;
    }

    if (desc.type() == "audio") {
        auto spec = codec_registry::detectInMedia(desc, CodecMediaType::Audio);
        if (!spec) {
            LWarn("Unsupported remote audio codec on track");
            return false;
        }

        track->setMediaHandler(codec_registry::makeDepacketizer(*spec));
        track->chainMediaHandler(std::make_shared<rtc::RtcpReceivingSession>());

        LDebug("Receive audio track set up");
        return true;
    }

    LWarn("Unsupported remote track type: ", desc.type());
    return false;
}


} // namespace wrtc
} // namespace icy
