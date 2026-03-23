///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/track.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"

#include <rtc/rtc.hpp>

#include <random>
#include <stdexcept>


namespace icy {
namespace wrtc {


namespace {

CodecSpec videoCodecSpec(const av::VideoCodec& codec)
{
    if (auto spec = CodecNegotiator::specFromVideoCodec(codec);
        spec && spec->mediaType == CodecMediaType::Video) {
        return *spec;
    }

    if (!codec.specified())
        throw std::invalid_argument("Video track requires an explicit codec");
    if (!codec.encoder.empty())
        throw std::invalid_argument("Unsupported video encoder: " + codec.encoder);
    throw std::invalid_argument("Unsupported video codec: " + codec.name);
}


CodecSpec audioCodecSpec(const av::AudioCodec& codec)
{
    if (auto spec = CodecNegotiator::specFromAudioCodec(codec);
        spec && spec->mediaType == CodecMediaType::Audio) {
        return *spec;
    }

    if (!codec.specified())
        throw std::invalid_argument("Audio track requires an explicit codec");
    if (!codec.encoder.empty())
        throw std::invalid_argument("Unsupported audio encoder: " + codec.encoder);
    throw std::invalid_argument("Unsupported audio codec: " + codec.name);
}


void addVideoCodec(rtc::Description::Video& media, const CodecSpec& spec)
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


void addAudioCodec(rtc::Description::Audio& media, const CodecSpec& spec)
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
makeVideoPacketizer(const CodecSpec& spec,
                    const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig)
{
    switch (spec.id) {
    case CodecId::H264:
        return std::make_shared<rtc::H264RtpPacketizer>(
            rtc::NalUnit::Separator::StartSequence, rtpConfig);
    case CodecId::H265:
        return std::make_shared<rtc::H265RtpPacketizer>(
            rtc::NalUnit::Separator::StartSequence, rtpConfig);
    default:
        return std::make_shared<rtc::RtpPacketizer>(rtpConfig);
    }
}


std::shared_ptr<rtc::MediaHandler>
makeAudioPacketizer(const CodecSpec& spec,
                    const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig)
{
    if (spec.id == CodecId::Opus)
        return std::make_shared<rtc::OpusRtpPacketizer>(rtpConfig);
    return std::make_shared<rtc::RtpPacketizer>(rtpConfig);
}


std::shared_ptr<rtc::MediaHandler>
makeVideoDepacketizer(const CodecSpec& spec)
{
    switch (spec.id) {
    case CodecId::H264:
        return std::make_shared<rtc::H264RtpDepacketizer>();
    case CodecId::H265:
        return std::make_shared<rtc::H265RtpDepacketizer>();
    default:
        return std::make_shared<rtc::RtpDepacketizer>(spec.clockRate ? spec.clockRate : 90000);
    }
}


std::shared_ptr<rtc::MediaHandler>
makeAudioDepacketizer(const CodecSpec& spec)
{
    switch (spec.id) {
    case CodecId::PCMU:
        return std::make_shared<rtc::PCMURtpDepacketizer>();
    case CodecId::PCMA:
        return std::make_shared<rtc::PCMARtpDepacketizer>();
    case CodecId::Opus:
        return std::make_shared<rtc::OpusRtpDepacketizer>();
    default:
        return std::make_shared<rtc::RtpDepacketizer>(spec.clockRate ? spec.clockRate : 48000);
    }
}


std::shared_ptr<rtc::MediaHandler>
makeVideoHandlerChain(const CodecSpec& spec,
                      const std::shared_ptr<rtc::RtpPacketizationConfig>& rtpConfig,
                      unsigned nackBuffer,
                      std::function<void()> onPli,
                      std::function<void(unsigned int)> onRemb)
{
    auto depacketizer = makeVideoDepacketizer(spec);
    auto packetizer = makeVideoPacketizer(spec, rtpConfig);
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
    auto depacketizer = makeAudioDepacketizer(spec);
    auto packetizer = makeAudioPacketizer(spec, rtpConfig);
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
    unsigned nackBuffer,
    std::function<void()> onPli,
    std::function<void(unsigned int)> onRemb)
{
    auto spec = videoCodecSpec(codec);

    if (ssrc == 0)
        ssrc = generateSsrc();
    std::string cn = cname.empty() ? "icey" : cname;

    // Build SDP media description.
    rtc::Description::Video media("video", rtc::Description::Direction::SendRecv);
    addVideoCodec(media, spec);
    media.addSSRC(ssrc, cn);

    // Add track to PeerConnection.
    auto track = pc->addTrack(media);

    // Build RTP config.
    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, cn, spec.payloadType, spec.clockRate);

    // Bidirectional media handler chain:
    //   incoming:  RtcpReceivingSession → RTCP handlers → Depacketizer
    //   outgoing:  Packetizer → RTCP sender/feedback handlers
    track->setMediaHandler(makeVideoHandlerChain(
        spec, rtpConfig, nackBuffer, std::move(onPli), std::move(onRemb)));

    LInfo("Video track created: ", spec.rtpName, " SSRC=", ssrc, " PT=", spec.payloadType);
    return {track, rtpConfig};
}


TrackHandle createAudioTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::AudioCodec& codec,
    uint32_t ssrc,
    const std::string& cname)
{
    auto spec = audioCodecSpec(codec);

    if (ssrc == 0)
        ssrc = generateSsrc();
    std::string cn = cname.empty() ? "icey" : cname;

    rtc::Description::Audio media("audio", rtc::Description::Direction::SendRecv);
    addAudioCodec(media, spec);
    media.addSSRC(ssrc, cn);

    auto track = pc->addTrack(media);

    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, cn, spec.payloadType, spec.clockRate);

    track->setMediaHandler(makeAudioHandlerChain(spec, rtpConfig));

    LInfo("Audio track created: ", spec.rtpName, " SSRC=", ssrc, " PT=", spec.payloadType);
    return {track, rtpConfig};
}


bool setupReceiveTrack(std::shared_ptr<rtc::Track> track)
{
    auto desc = track->description();
    auto sdp = std::string(desc.generateSdp("\r\n", ""));

    if (desc.type() == "video") {
        auto spec = CodecNegotiator::detectCodec(sdp, CodecMediaType::Video);
        if (!spec) {
            LWarn("Unsupported remote video codec in SDP");
            return false;
        }
        auto depacketizer = makeVideoDepacketizer(*spec);

        auto session = std::make_shared<rtc::RtcpReceivingSession>();
        track->setMediaHandler(depacketizer);
        track->chainMediaHandler(session);

        LDebug("Receive video track set up: ", sdp.substr(0, 40));
        return true;
    }

    if (desc.type() == "audio") {
        auto spec = CodecNegotiator::detectCodec(sdp, CodecMediaType::Audio);
        if (!spec) {
            LWarn("Unsupported remote audio codec in SDP");
            return false;
        }
        auto depacketizer = makeAudioDepacketizer(*spec);

        auto session = std::make_shared<rtc::RtcpReceivingSession>();
        track->setMediaHandler(depacketizer);
        track->chainMediaHandler(session);

        LDebug("Receive audio track set up");
        return true;
    }

    LWarn("Unsupported remote track type: ", desc.type());
    return false;
}


} // namespace wrtc
} // namespace icy
