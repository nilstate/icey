///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/peersession.h"
#include "icy/logger.h"


namespace icy {
namespace wrtc {

namespace {

MediaBridge::Options makeBridgeOptions(const PeerSession::MediaConfig& media)
{
    MediaBridge::Options opts;
    opts.videoCodec = media.videoCodec;
    opts.audioCodec = media.audioCodec;
    opts.videoDirection = media.videoDirection;
    opts.audioDirection = media.audioDirection;
    return opts;
}

} // namespace


std::shared_ptr<rtc::PeerConnection> PeerSession::createPeerConnection(
    bool createDataChannel,
    const MediaBridge::Options* mediaOpts)
{
    auto pc = std::make_shared<rtc::PeerConnection>(_config.rtcConfig);
    std::shared_ptr<rtc::DataChannel> dc;
    auto callbackGuard = _callbackGuard;
    setupPeerConnectionCallbacks(pc);

    if (createDataChannel && _config.enableDataChannel) {
        dc = pc->createDataChannel(_config.dataChannelLabel);
        dc->onOpen([callbackGuard]() {
            if (!callbackGuard->alive.load(std::memory_order_acquire))
                return;
            LDebug("Data channel open");
        });
        dc->onMessage([this, callbackGuard](rtc::message_variant msg) {
            if (!callbackGuard->alive.load(std::memory_order_acquire))
                return;
            enqueueCallback([this, msg = std::move(msg)]() mutable {
                DataReceived.emit(std::move(msg));
            });
        });
    }

    pc->onDataChannel([this, pc, callbackGuard](std::shared_ptr<rtc::DataChannel> dc) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        std::lock_guard lock(_mutex);
        if (_pc != pc || _dc)
            return;

        _dc = dc;
        _dc->onMessage([this, callbackGuard](rtc::message_variant msg) {
            if (!callbackGuard->alive.load(std::memory_order_acquire))
                return;
            enqueueCallback([this, msg = std::move(msg)]() mutable {
                DataReceived.emit(std::move(msg));
            });
        });
    });

    {
        std::lock_guard lock(_mutex);
        _pc = std::move(pc);
        _dc = std::move(dc);
        _remoteDescriptionSet = false;
        _pendingRemoteCandidates.clear();
        pc = _pc;
    }

    if (!_media.attached())
        _media.attach(pc, mediaOpts ? *mediaOpts : makeBridgeOptions(_config.media));

    return pc;
}


void PeerSession::setupPeerConnectionCallbacks(const std::shared_ptr<rtc::PeerConnection>& pc)
{
    auto callbackGuard = _callbackGuard;

    pc->onLocalDescription([this, pc, callbackGuard](rtc::Description desc) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        LDebug("Local SDP ready: ", desc.typeString());
        std::string peerId;
        {
            std::lock_guard lock(_mutex);
            if (_pc != pc)
                return;
            peerId = _remotePeerId;
        }
        if (!peerId.empty())
            _signaller.sendSdp(peerId, desc.typeString(), std::string(desc));
    });

    pc->onLocalCandidate([this, pc, callbackGuard](rtc::Candidate candidate) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        LDebug("Local ICE candidate");
        std::string peerId;
        {
            std::lock_guard lock(_mutex);
            if (_pc != pc)
                return;
            peerId = _remotePeerId;
        }
        if (!peerId.empty())
            _signaller.sendCandidate(peerId, std::string(candidate), candidate.mid());
    });

    pc->onStateChange([this, pc, callbackGuard](rtc::PeerConnection::State rtcState) {
        if (!callbackGuard->alive.load(std::memory_order_acquire))
            return;

        LDebug("PeerConnection state: ", static_cast<int>(rtcState));

        State newState = State::Idle;
        bool changed = false;
        std::shared_ptr<rtc::PeerConnection> endedPc;
        std::shared_ptr<rtc::DataChannel> dc;
        bool shouldFinish = false;

        {
            std::lock_guard lock(_mutex);
            if (_pc != pc)
                return;
            switch (rtcState) {
            case rtc::PeerConnection::State::Connected:
                if (_state == State::Negotiating) {
                    _state = State::Active;
                    newState = State::Active;
                    changed = true;
                }
                break;
            case rtc::PeerConnection::State::Failed:
                beginEndCall("connection failed", endedPc, dc);
                newState = State::Ending;
                changed = true;
                shouldFinish = true;
                break;
            case rtc::PeerConnection::State::Disconnected:
                beginEndCall("disconnected", endedPc, dc);
                newState = State::Ending;
                changed = true;
                shouldFinish = true;
                break;
            case rtc::PeerConnection::State::Closed:
                if (_state != State::Idle && _state != State::Ended && _state != State::Ending) {
                    beginEndCall("closed", endedPc, dc);
                    newState = State::Ending;
                    changed = true;
                    shouldFinish = true;
                }
                break;
            default:
                break;
            }
        }

        if (dc)
            dc->close();
        if (endedPc)
            endedPc->close();

        if (changed) {
            enqueueCallback([this, newState]() { StateChanged.emit(newState); });
            if (shouldFinish) {
                finishEndCall();
                enqueueCallback([this]() { StateChanged.emit(State::Ended); });
                transitionEndedToIdle();
            }
        }
    });
}


void PeerSession::beginEndCall(const std::string& reason,
                               std::shared_ptr<rtc::PeerConnection>& pc,
                               std::shared_ptr<rtc::DataChannel>& dc)
{
    LDebug("Ending call: ", reason);
    _state = State::Ending;
    _media.detach();

    dc = std::move(_dc);
    pc = std::move(_pc);
    _remoteDescriptionSet = false;
    _pendingRemoteCandidates.clear();

    _remotePeerId.clear();
}


void PeerSession::finishEndCall()
{
    std::lock_guard lock(_mutex);
    if (_state == State::Ending)
        _state = State::Ended;
}


void PeerSession::transitionEndedToIdle()
{
    bool changed = false;
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Ended && !_pc && !_dc && _remotePeerId.empty()) {
            _state = State::Idle;
            changed = true;
        }
    }

    if (changed)
        enqueueCallback([this]() { StateChanged.emit(State::Idle); });
}


} // namespace wrtc
} // namespace icy
