///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/mediabridge.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/logger.h"

#include <stdexcept>


namespace icy {
namespace wrtc {

namespace {
bool keepsPayloadType(const rtc::Description::Media::RtpMap& map, int payloadType)
{
    if (map.payloadType == payloadType)
        return true;

    const auto apt = "apt=" + std::to_string(payloadType);
    for (const auto& fmtp : map.fmtps) {
        if (fmtp == apt)
            return true;
    }
    return false;
}


void narrowToPayloadType(rtc::Description::Media& media, int payloadType)
{
    if (payloadType < 0 || !media.hasPayloadType(payloadType))
        return;

    auto pts = media.payloadTypes();
    for (int pt : pts) {
        if (pt == payloadType)
            continue;
        if (!media.hasPayloadType(pt))
            continue;

        const auto* map = media.rtpMap(pt);
        if (map && keepsPayloadType(*map, payloadType))
            continue;

        media.removeRtpMap(pt);
    }
}


void stripSenderAttributes(rtc::Description::Media& media)
{
    media.clearSSRCs();

    auto attrs = media.attributes();
    for (const auto& attr : attrs) {
        if (attr.rfind("msid:", 0) == 0)
            media.removeAttribute(attr);
    }
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


void applyReceiveVideoDescription(const std::shared_ptr<rtc::Track>& track,
                                  const MediaBridge::Options& opts)
{
    auto media = track->description();
    media.setDirection(opts.videoDirection);
    stripSenderAttributes(media);
    if (opts.videoPayloadType >= 0)
        narrowToPayloadType(media, opts.videoPayloadType);
    track->setDescription(media);
}


void applyReceiveAudioDescription(const std::shared_ptr<rtc::Track>& track,
                                  const MediaBridge::Options& opts)
{
    auto media = track->description();
    media.setDirection(opts.audioDirection);
    stripSenderAttributes(media);
    if (opts.audioPayloadType >= 0)
        narrowToPayloadType(media, opts.audioPayloadType);
    track->setDescription(media);
}
} // namespace


MediaBridge::MediaBridge() = default;


MediaBridge::~MediaBridge()
{
    detach();
}


void MediaBridge::attach(std::shared_ptr<rtc::PeerConnection> pc,
                         const Options& opts)
{
    std::lock_guard lock(_mutex);

    if (_pc)
        throw std::logic_error("MediaBridge already attached");

    _pc = std::move(pc);
    std::string cname = opts.cname.empty() ? "icey" : opts.cname;

    const bool wantVideo = opts.videoCodec.specified();
    const bool wantAudio = opts.audioCodec.specified();

    if (wantVideo) {
        const bool videoSends = sends(opts.videoDirection);
        const bool videoReceives = receives(opts.videoDirection);
        _videoReceiveTrack.reset();
        if (videoSends) {
            _videoHandle = createVideoTrack(
                _pc, opts.videoCodec, opts.videoSsrc, cname, opts.videoMid,
                opts.videoDirection,
                opts.nackBufferSize,
                [this]() { KeyframeRequested.emit(); },
                [this](unsigned int bps) { BitrateEstimate.emit(bps); },
                opts.videoPayloadType);
            _videoSender.bind(_videoHandle);
            if (videoReceives) {
                _videoReceiveTrack = _videoHandle.track;
                _videoReceiver.bind(_videoHandle.track);
            }
        }
        else if (videoReceives) {
            _videoReceiveTrack = createVideoReceiveTrack(
                _pc, opts.videoCodec, opts.videoMid, opts.videoDirection,
                opts.videoPayloadType);
            _videoReceiver.bind(_videoReceiveTrack);
        }
    }

    if (wantAudio) {
        const bool audioSends = sends(opts.audioDirection);
        const bool audioReceives = receives(opts.audioDirection);
        _audioReceiveTrack.reset();
        if (audioSends) {
            _audioHandle = createAudioTrack(
                _pc, opts.audioCodec, opts.audioSsrc, cname, opts.audioMid,
                opts.audioDirection,
                opts.audioPayloadType);
            _audioSender.bind(_audioHandle);
            if (audioReceives) {
                _audioReceiveTrack = _audioHandle.track;
                _audioReceiver.bind(_audioHandle.track);
            }
        }
        else if (audioReceives) {
            _audioReceiveTrack = createAudioReceiveTrack(
                _pc, opts.audioCodec, opts.audioMid, opts.audioDirection,
                opts.audioPayloadType);
            _audioReceiver.bind(_audioReceiveTrack);
        }
    }

    // Tracks not created locally are bound from the actual reciprocated track
    // in onTrack(). Pure receive-only tracks are created locally above so
    // outgoing watch/record sessions can advertise media lines without
    // inventing sender state.
    _pc->onTrack([this, opts](std::shared_ptr<rtc::Track> track) {
        auto desc = track->description();
        LDebug("MediaBridge onTrack: type=", desc.type(), " mid=", track->mid());
        if (desc.type() == "video") {
            if (_videoReceiveTrack || _videoHandle.track)
                return;
            applyReceiveVideoDescription(track, opts);
        }
        else if (desc.type() == "audio") {
            if (_audioReceiveTrack || _audioHandle.track)
                return;
            applyReceiveAudioDescription(track, opts);
        }
        LDebug("MediaBridge setup start: type=", desc.type(), " mid=", track->mid());
        if (!setupReceiveTrack(track)) {
            LWarn("Ignoring remote ", desc.type(), " track with unsupported codec");
            return;
        }
        LDebug("MediaBridge setup done: type=", desc.type(), " mid=", track->mid());
        if (desc.type() == "video") {
            _videoReceiveTrack = track;
            LDebug("MediaBridge retained remote video track: mid=", track->mid());
            _videoReceiver.bind(track);
        }
        else if (desc.type() == "audio") {
            _audioReceiveTrack = track;
            LDebug("MediaBridge retained remote audio track: mid=", track->mid());
            _audioReceiver.bind(track);
        }
    });
}


void MediaBridge::detach()
{
    std::lock_guard lock(_mutex);

    _videoSender.unbind();
    _audioSender.unbind();

    // Clear callbacks before releasing the PeerConnection to prevent
    // dangling `this` captures if the caller keeps the PC alive.
    if (_pc)
        _pc->onTrack(nullptr);

    _videoHandle = {};
    _audioHandle = {};
    _videoReceiveTrack.reset();
    _audioReceiveTrack.reset();
    _pc.reset();
}


void MediaBridge::requestKeyframe()
{
    std::lock_guard lock(_mutex);
    auto track = _videoReceiveTrack ? _videoReceiveTrack : _videoHandle.track;
    if (track)
        track->requestKeyframe();
}


void MediaBridge::requestBitrate(unsigned int bitrate)
{
    std::lock_guard lock(_mutex);
    auto track = _videoReceiveTrack ? _videoReceiveTrack : _videoHandle.track;
    if (track)
        track->requestBitrate(bitrate);
}


WebRtcTrackSender& MediaBridge::videoSender()
{
    if (!_videoHandle.track || !_videoHandle.rtpConfig)
        throw std::logic_error("No video send track created");
    return _videoSender;
}


WebRtcTrackSender& MediaBridge::audioSender()
{
    if (!_audioHandle.track || !_audioHandle.rtpConfig)
        throw std::logic_error("No audio send track created");
    return _audioSender;
}


WebRtcTrackReceiver& MediaBridge::videoReceiver() { return _videoReceiver; }
WebRtcTrackReceiver& MediaBridge::audioReceiver() { return _audioReceiver; }


std::shared_ptr<rtc::Track> MediaBridge::videoTrack() const
{
    std::lock_guard lock(_mutex);
    return _videoReceiveTrack ? _videoReceiveTrack : _videoHandle.track;
}


std::shared_ptr<rtc::Track> MediaBridge::audioTrack() const
{
    std::lock_guard lock(_mutex);
    return _audioReceiveTrack ? _audioReceiveTrack : _audioHandle.track;
}


bool MediaBridge::hasVideo() const
{
    std::lock_guard lock(_mutex);
    return _videoHandle.track != nullptr || _videoReceiveTrack != nullptr;
}


bool MediaBridge::hasAudio() const
{
    std::lock_guard lock(_mutex);
    return _audioHandle.track != nullptr || _audioReceiveTrack != nullptr;
}


bool MediaBridge::attached() const
{
    std::lock_guard lock(_mutex);
    return _pc != nullptr;
}


} // namespace wrtc
} // namespace icy
