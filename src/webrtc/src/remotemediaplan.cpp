///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "remotemediaplan.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"

#include <cctype>
#include <optional>
#include <stdexcept>
#include <string_view>


namespace icy {
namespace wrtc {

namespace {

struct MediaSectionBounds
{
    size_t begin = std::string::npos;
    size_t end = std::string::npos;
};


std::optional<MediaSectionBounds> findMediaSection(const std::string& sdp,
                                                   std::string_view mediaType)
{
    std::string sectionHeader = "m=" + std::string(mediaType) + " ";
    size_t section = sdp.find(sectionHeader);
    if (section == std::string::npos)
        return std::nullopt;

    size_t sectionEnd = sdp.find("\r\nm=", section + 2);
    return MediaSectionBounds{section, sectionEnd == std::string::npos ? sdp.size() : sectionEnd};
}


std::optional<std::string> findSectionValue(const std::string& sdp,
                                            const MediaSectionBounds& section,
                                            std::string_view prefix)
{
    size_t searchPos = section.begin;
    while (true) {
        size_t valuePos = sdp.find(prefix, searchPos);
        if (valuePos == std::string::npos || valuePos >= section.end)
            break;

        size_t valueStart = valuePos + prefix.size();
        size_t valueEnd = sdp.find("\r\n", valueStart);
        if (valueEnd == std::string::npos)
            valueEnd = sdp.size();
        return sdp.substr(valueStart, valueEnd - valueStart);
    }

    return std::nullopt;
}


bool iequalsAscii(std::string_view a, std::string_view b)
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}


std::optional<CodecSpec> findSupportedCodecForMediaType(const std::string& sdp,
                                                        std::string_view mediaType,
                                                        CodecMediaType codecType)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;

    std::string_view sectionText(sdp.data() + section->begin, section->end - section->begin);
    return CodecNegotiator::detectCodec(sectionText, codecType);
}


std::optional<int> findPayloadTypeForCodec(const std::string& sdp,
                                           std::string_view mediaType,
                                           std::string_view codecName)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;

    size_t lineStart = section->begin;
    while (lineStart < section->end) {
        size_t lineEnd = sdp.find("\r\n", lineStart);
        if (lineEnd == std::string::npos || lineEnd > section->end)
            lineEnd = section->end;

        std::string_view line(sdp.data() + lineStart, lineEnd - lineStart);
        constexpr std::string_view prefix = "a=rtpmap:";
        if (line.substr(0, prefix.size()) == prefix) {
            auto space = line.find(' ', prefix.size());
            auto slash = line.find('/', space == std::string_view::npos ? prefix.size() : space + 1);
            if (space != std::string_view::npos && slash != std::string_view::npos) {
                auto ptText = line.substr(prefix.size(), space - prefix.size());
                auto name = line.substr(space + 1, slash - space - 1);
                if (iequalsAscii(name, codecName))
                    return std::stoi(std::string(ptText));
            }
        }

        lineStart = lineEnd + 2;
    }

    return std::nullopt;
}


std::optional<std::string> findMidForMediaType(const std::string& sdp,
                                               std::string_view mediaType)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;
    return findSectionValue(sdp, *section, "\r\na=mid:");
}


std::optional<rtc::Description::Direction> findDirectionForMediaType(
    const std::string& sdp,
    std::string_view mediaType)
{
    auto section = findMediaSection(sdp, mediaType);
    if (!section)
        return std::nullopt;

    std::string_view sectionText(sdp.data() + section->begin, section->end - section->begin);
    if (sectionText.find("\r\na=sendonly") != std::string_view::npos)
        return rtc::Description::Direction::SendOnly;
    if (sectionText.find("\r\na=recvonly") != std::string_view::npos)
        return rtc::Description::Direction::RecvOnly;
    if (sectionText.find("\r\na=inactive") != std::string_view::npos)
        return rtc::Description::Direction::Inactive;
    return rtc::Description::Direction::SendRecv;
}


bool sends(rtc::Description::Direction direction)
{
    return direction == rtc::Description::Direction::SendRecv ||
           direction == rtc::Description::Direction::SendOnly;
}


bool receives(rtc::Description::Direction direction)
{
    return direction == rtc::Description::Direction::SendRecv ||
           direction == rtc::Description::Direction::RecvOnly;
}


bool receiveOnly(rtc::Description::Direction direction)
{
    return receives(direction) && !sends(direction);
}


rtc::Description::Direction negotiateAnswerDirection(
    rtc::Description::Direction localDirection,
    rtc::Description::Direction remoteOfferDirection)
{
    const bool localCanSend = sends(localDirection) && receives(remoteOfferDirection);
    const bool localCanReceive = receives(localDirection) && sends(remoteOfferDirection);
    if (localCanSend && localCanReceive)
        return rtc::Description::Direction::SendRecv;
    if (localCanSend)
        return rtc::Description::Direction::SendOnly;
    if (localCanReceive)
        return rtc::Description::Direction::RecvOnly;
    return rtc::Description::Direction::Inactive;
}

} // namespace


MediaBridge::Options resolveAnswerMediaOptions(const MediaBridge::Options& localOptions,
                                               const std::string& remoteOfferSdp)
{
    auto opts = localOptions;

    if (auto mid = findMidForMediaType(remoteOfferSdp, "audio")) {
        opts.audioMid = *mid;
        if (auto direction = findDirectionForMediaType(remoteOfferSdp, "audio"))
            opts.audioDirection = negotiateAnswerDirection(opts.audioDirection, *direction);
        if (opts.audioCodec.specified() || receiveOnly(opts.audioDirection)) {
            auto spec = CodecNegotiator::specFromAudioCodec(opts.audioCodec);
            auto payloadType = spec
                ? findPayloadTypeForCodec(remoteOfferSdp, "audio", spec->rtpName)
                : std::nullopt;
            if (payloadType) {
                opts.audioPayloadType = *payloadType;
            }
            else if (receiveOnly(opts.audioDirection)) {
                auto fallback = findSupportedCodecForMediaType(
                    remoteOfferSdp, "audio", CodecMediaType::Audio);
                if (fallback) {
                    opts.audioCodec = av::AudioCodec(
                        fallback->rtpName,
                        DEFAULT_AUDIO_CHANNELS,
                        static_cast<int>(fallback->clockRate));
                    opts.audioPayloadType = findPayloadTypeForCodec(
                        remoteOfferSdp, "audio", fallback->rtpName).value_or(fallback->payloadType);
                    LInfo("Negotiated receive-only audio codec: ", fallback->rtpName);
                }
                else {
                    LWarn("Remote offer does not contain a supported audio codec");
                    opts.audioCodec = {};
                    opts.audioDirection = rtc::Description::Direction::Inactive;
                }
            }
            else {
                LWarn("Remote offer does not contain the configured audio codec");
                opts.audioCodec = {};
                opts.audioDirection = rtc::Description::Direction::Inactive;
            }
        }
    }
    else {
        opts.audioCodec = {};
        opts.audioDirection = rtc::Description::Direction::Inactive;
    }

    if (auto mid = findMidForMediaType(remoteOfferSdp, "video")) {
        opts.videoMid = *mid;
        if (auto direction = findDirectionForMediaType(remoteOfferSdp, "video"))
            opts.videoDirection = negotiateAnswerDirection(opts.videoDirection, *direction);
        if (opts.videoCodec.specified() || receiveOnly(opts.videoDirection)) {
            auto spec = CodecNegotiator::specFromVideoCodec(opts.videoCodec);
            auto payloadType = spec
                ? findPayloadTypeForCodec(remoteOfferSdp, "video", spec->rtpName)
                : std::nullopt;
            if (payloadType) {
                opts.videoPayloadType = *payloadType;
            }
            else if (receiveOnly(opts.videoDirection)) {
                auto fallback = findSupportedCodecForMediaType(
                    remoteOfferSdp, "video", CodecMediaType::Video);
                if (fallback) {
                    opts.videoCodec = av::VideoCodec(
                        fallback->rtpName,
                        opts.videoCodec.width,
                        opts.videoCodec.height,
                        opts.videoCodec.fps,
                        opts.videoCodec.bitRate,
                        static_cast<int>(fallback->clockRate),
                        opts.videoCodec.pixelFmt);
                    opts.videoPayloadType = findPayloadTypeForCodec(
                        remoteOfferSdp, "video", fallback->rtpName).value_or(fallback->payloadType);
                    LInfo("Negotiated receive-only video codec: ", fallback->rtpName);
                }
                else {
                    LWarn("Remote offer does not contain a supported video codec");
                    opts.videoCodec = {};
                    opts.videoDirection = rtc::Description::Direction::Inactive;
                }
            }
            else {
                LWarn("Remote offer does not contain the configured video codec");
                opts.videoCodec = {};
                opts.videoDirection = rtc::Description::Direction::Inactive;
            }
        }
    }
    else {
        opts.videoCodec = {};
        opts.videoDirection = rtc::Description::Direction::Inactive;
    }

    return opts;
}


} // namespace wrtc
} // namespace icy
