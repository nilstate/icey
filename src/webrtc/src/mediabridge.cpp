///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/mediabridge.h"
#include "icy/logger.h"

#include <stdexcept>


namespace icy {
namespace wrtc {


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

    // Create video track if codec encoder is specified.
    if (!opts.videoCodec.encoder.empty()) {
        _videoHandle = createVideoTrack(
            _pc, opts.videoCodec, opts.videoSsrc, cname, opts.nackBufferSize,
            [this]() { KeyframeRequested.emit(); },
            [this](unsigned int bps) { BitrateEstimate.emit(bps); });
        _videoSender = std::make_unique<WebRtcTrackSender>(_videoHandle);
    }

    // Create audio track if codec encoder is specified.
    if (!opts.audioCodec.encoder.empty()) {
        _audioHandle = createAudioTrack(
            _pc, opts.audioCodec, opts.audioSsrc, cname);
        _audioSender = std::make_unique<WebRtcTrackSender>(_audioHandle);
    }

    // Handle incoming remote tracks.
    _pc->onTrack([this](std::shared_ptr<rtc::Track> track) {
        setupReceiveTrack(track);

        auto desc = track->description();
        if (desc.type() == "video") {
            _videoReceiver.bind(track);
        }
        else if (desc.type() == "audio") {
            _audioReceiver.bind(track);
        }
    });
}


void MediaBridge::detach()
{
    std::lock_guard lock(_mutex);

    if (_videoSender)
        _videoSender->unbind();
    if (_audioSender)
        _audioSender->unbind();

    // Clear callbacks before releasing the PeerConnection to prevent
    // dangling `this` captures if the caller keeps the PC alive.
    if (_pc)
        _pc->onTrack(nullptr);

    _videoSender.reset();
    _audioSender.reset();
    _videoHandle = {};
    _audioHandle = {};
    _pc.reset();
}


void MediaBridge::requestKeyframe()
{
    std::lock_guard lock(_mutex);
    if (_videoHandle.track)
        _videoHandle.track->requestKeyframe();
}


void MediaBridge::requestBitrate(unsigned int bitrate)
{
    std::lock_guard lock(_mutex);
    if (_videoHandle.track)
        _videoHandle.track->requestBitrate(bitrate);
}


WebRtcTrackSender& MediaBridge::videoSender()
{
    if (!_videoSender)
        throw std::logic_error("No video track created");
    return *_videoSender;
}


WebRtcTrackSender& MediaBridge::audioSender()
{
    if (!_audioSender)
        throw std::logic_error("No audio track created");
    return *_audioSender;
}


WebRtcTrackReceiver& MediaBridge::videoReceiver() { return _videoReceiver; }
WebRtcTrackReceiver& MediaBridge::audioReceiver() { return _audioReceiver; }


std::shared_ptr<rtc::Track> MediaBridge::videoTrack() const
{
    std::lock_guard lock(_mutex);
    return _videoHandle.track;
}


std::shared_ptr<rtc::Track> MediaBridge::audioTrack() const
{
    std::lock_guard lock(_mutex);
    return _audioHandle.track;
}


bool MediaBridge::hasVideo() const
{
    std::lock_guard lock(_mutex);
    return _videoSender != nullptr;
}


bool MediaBridge::hasAudio() const
{
    std::lock_guard lock(_mutex);
    return _audioSender != nullptr;
}


bool MediaBridge::attached() const
{
    std::lock_guard lock(_mutex);
    return _pc != nullptr;
}


} // namespace wrtc
} // namespace icy
