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
#include <rtc/rtp.hpp>
#include <rtc/rtpdepacketizer.hpp>

#include <random>
#include <stdexcept>


namespace icy {
namespace wrtc {


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


void stripUnsupportedReceiveFormats(rtc::Description::Media& media)
{
    // We do not currently normalize auxiliary retransmission payloads back to
    // the original codec payload on the receive path. Do not negotiate any
    // codec entry that depends on another payload type via apt=.
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
    case CodecId::VP8:
        return std::make_shared<Vp8RtpDepacketizer>();
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

    // Build SDP media description.
    rtc::Description::Video media(
        mid.empty() ? "video" : mid,
        direction);
    addVideoCodec(media, spec);
    media.addSSRC(ssrc, cn, cn, mid.empty() ? "video" : mid);

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

    rtc::Description::Audio media(
        mid.empty() ? "audio" : mid,
        direction);
    addAudioCodec(media, spec);
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

    rtc::Description::Video media(
        mid.empty() ? "video" : mid,
        direction);
    addVideoCodec(media, spec);

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

    rtc::Description::Audio media(
        mid.empty() ? "audio" : mid,
        direction);
    addAudioCodec(media, spec);

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
