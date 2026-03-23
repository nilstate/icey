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


namespace icy {
namespace wrtc {


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
    // Determine RTP codec name from FFmpeg encoder name.
    std::string rtpName = "H264";
    if (!codec.encoder.empty()) {
        auto rtp = CodecNegotiator::ffmpegToRtp(codec.encoder);
        if (!rtp.empty())
            rtpName = rtp;
    }

    if (ssrc == 0)
        ssrc = generateSsrc();
    std::string cn = cname.empty() ? "icey" : cname;
    int pt = CodecNegotiator::defaultPayloadType(rtpName);
    uint32_t clock = CodecNegotiator::clockRate(rtpName);

    // Build SDP media description.
    rtc::Description::Video media("video", rtc::Description::Direction::SendRecv);

    if (rtpName == "H264")
        media.addH264Codec(pt);
    else if (rtpName == "VP8")
        media.addVP8Codec(pt);
    else if (rtpName == "VP9")
        media.addVP9Codec(pt);
    else if (rtpName == "H265")
        media.addH265Codec(pt);
    else
        media.addH264Codec(pt);

    media.addSSRC(ssrc, cn);

    // Add track to PeerConnection.
    auto track = pc->addTrack(media);

    // Build RTP config.
    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, cn, pt, clock);

    // Build media handler chain:
    //   Packetizer → SrReporter → NackResponder [→ PliHandler] [→ RembHandler]
    std::shared_ptr<rtc::MediaHandler> packetizer;
    if (rtpName == "H264") {
        packetizer = std::make_shared<rtc::H264RtpPacketizer>(
            rtc::NalUnit::Separator::LongStartSequence, rtpConfig);
    }
    else if (rtpName == "H265") {
        packetizer = std::make_shared<rtc::H265RtpPacketizer>(
            rtc::NalUnit::Separator::LongStartSequence, rtpConfig);
    }
    else {
        packetizer = std::make_shared<rtc::RtpPacketizer>(rtpConfig);
    }

    packetizer->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
    packetizer->addToChain(std::make_shared<rtc::RtcpNackResponder>(nackBuffer));

    if (onPli)
        packetizer->addToChain(std::make_shared<rtc::PliHandler>(std::move(onPli)));
    if (onRemb)
        packetizer->addToChain(std::make_shared<rtc::RembHandler>(std::move(onRemb)));

    track->setMediaHandler(packetizer);

    LInfo("Video track created: ", rtpName, " SSRC=", ssrc, " PT=", pt);
    return {track, rtpConfig};
}


TrackHandle createAudioTrack(
    std::shared_ptr<rtc::PeerConnection> pc,
    const av::AudioCodec& codec,
    uint32_t ssrc,
    const std::string& cname)
{
    std::string rtpName = "opus";
    if (!codec.encoder.empty()) {
        auto rtp = CodecNegotiator::ffmpegToRtp(codec.encoder);
        if (!rtp.empty())
            rtpName = rtp;
    }

    if (ssrc == 0)
        ssrc = generateSsrc();
    std::string cn = cname.empty() ? "icey" : cname;
    int pt = CodecNegotiator::defaultPayloadType(rtpName);
    uint32_t clock = CodecNegotiator::clockRate(rtpName);

    rtc::Description::Audio media("audio", rtc::Description::Direction::SendRecv);

    if (rtpName == "opus")
        media.addOpusCodec(pt);
    else if (rtpName == "PCMU")
        media.addPCMUCodec(pt);
    else if (rtpName == "PCMA")
        media.addPCMACodec(pt);
    else
        media.addOpusCodec(pt);

    media.addSSRC(ssrc, cn);

    auto track = pc->addTrack(media);

    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, cn, pt, clock);

    // Audio packetizer: Opus uses its dedicated packetizer;
    // PCMU/PCMA and others use the generic RTP packetizer.
    std::shared_ptr<rtc::MediaHandler> packetizer;
    if (rtpName == "opus")
        packetizer = std::make_shared<rtc::OpusRtpPacketizer>(rtpConfig);
    else
        packetizer = std::make_shared<rtc::RtpPacketizer>(rtpConfig);
    packetizer->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
    track->setMediaHandler(packetizer);

    LInfo("Audio track created: ", rtpName, " SSRC=", ssrc, " PT=", pt);
    return {track, rtpConfig};
}


void setupReceiveTrack(std::shared_ptr<rtc::Track> track)
{
    auto desc = track->description();
    auto sdp = std::string(desc.generateSdp("\r\n", ""));

    if (desc.type() == "video") {
        std::shared_ptr<rtc::MediaHandler> depacketizer;

        if (sdp.find("H264") != std::string::npos ||
            sdp.find("h264") != std::string::npos) {
            depacketizer = std::make_shared<rtc::H264RtpDepacketizer>();
        }
        else if (sdp.find("H265") != std::string::npos ||
                 sdp.find("h265") != std::string::npos) {
            depacketizer = std::make_shared<rtc::H265RtpDepacketizer>();
        }
        else {
            depacketizer = std::make_shared<rtc::RtpDepacketizer>(90000);
        }

        auto session = std::make_shared<rtc::RtcpReceivingSession>();
        track->setMediaHandler(depacketizer);
        track->chainMediaHandler(session);

        LDebug("Receive video track set up: ", sdp.substr(0, 40));
    }
    else if (desc.type() == "audio") {
        std::shared_ptr<rtc::MediaHandler> depacketizer;

        if (sdp.find("PCMU") != std::string::npos) {
            depacketizer = std::make_shared<rtc::PCMURtpDepacketizer>();
        }
        else if (sdp.find("PCMA") != std::string::npos) {
            depacketizer = std::make_shared<rtc::PCMARtpDepacketizer>();
        }
        else {
            depacketizer = std::make_shared<rtc::OpusRtpDepacketizer>();
        }

        auto session = std::make_shared<rtc::RtcpReceivingSession>();
        track->setMediaHandler(depacketizer);
        track->chainMediaHandler(session);

        LDebug("Receive audio track set up");
    }
}


} // namespace wrtc
} // namespace icy
