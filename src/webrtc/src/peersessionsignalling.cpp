///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/peersession.h"
#include "icy/logger.h"
#include "remotemediaplan.h"


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


void PeerSession::onControlReceived(const std::string& peerId,
                                    const std::string& type,
                                    const std::string& reason)
{
    if (type == "init") {
        bool busy = false;
        {
            std::lock_guard lock(_mutex);
            if (_state == State::Ended)
                _state = State::Idle;
            if (_state != State::Idle) {
                busy = true;
            }
            else {
                _remotePeerId = peerId;
                _state = State::IncomingInit;
            }
        }
        if (busy) {
            _signaller.sendControl(peerId, "reject", "busy");
            return;
        }
        enqueueCallback([this]() { StateChanged.emit(State::IncomingInit); });
        enqueueCallback([this, peerId]() { IncomingCall.emit(peerId); });
    }
    else if (type == "accept") {
        bool shouldConnect = false;
        {
            std::lock_guard lock(_mutex);
            if (_state != State::OutgoingInit || peerId != _remotePeerId)
                return;

            LInfo("Remote accepted call");
            _state = State::Negotiating;
            shouldConnect = true;
        }
        if (shouldConnect) {
            auto pc = createPeerConnection(true);
            if (pc)
                pc->setLocalDescription(rtc::Description::Type::Offer);
        }
        enqueueCallback([this]() { StateChanged.emit(State::Negotiating); });
    }
    else if (type == "reject") {
        std::shared_ptr<rtc::PeerConnection> pc;
        std::shared_ptr<rtc::DataChannel> dc;
        {
            std::lock_guard lock(_mutex);
            if (_state != State::OutgoingInit || peerId != _remotePeerId)
                return;

            LInfo("Remote rejected call: ", reason);
            beginEndCall("rejected", pc, dc);
        }
        enqueueCallback([this]() { StateChanged.emit(State::Ending); });
        if (dc)
            dc->close();
        if (pc)
            pc->close();
        finishEndCall();
        enqueueCallback([this]() { StateChanged.emit(State::Ended); });
        transitionEndedToIdle();
    }
    else if (type == "hangup") {
        std::shared_ptr<rtc::PeerConnection> pc;
        std::shared_ptr<rtc::DataChannel> dc;
        {
            std::lock_guard lock(_mutex);
            if (peerId != _remotePeerId)
                return;

            LInfo("Remote hung up: ", reason);
            beginEndCall(reason, pc, dc);
        }
        enqueueCallback([this]() { StateChanged.emit(State::Ending); });
        if (dc)
            dc->close();
        if (pc)
            pc->close();
        finishEndCall();
        enqueueCallback([this]() { StateChanged.emit(State::Ended); });
        transitionEndedToIdle();
    }
}


void PeerSession::onSdpReceived(const std::string& peerId,
                                const std::string& type,
                                const std::string& sdp)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    bool createIncomingPc = false;
    std::optional<MediaBridge::Options> mediaOpts;
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId)
            return;
        if (_state != State::Negotiating && _state != State::Active)
            return;

        if (!_pc) {
            if (type != "offer" || _state != State::Negotiating)
                return;
            createIncomingPc = true;
            mediaOpts = resolveAnswerMediaOptions(makeBridgeOptions(_config.media), sdp);
        }
        else {
            pc = _pc;
        }
    }

    if (createIncomingPc) {
        pc = createPeerConnection(false, mediaOpts ? &*mediaOpts : nullptr);
        if (!pc)
            return;
    }

    std::vector<PendingCandidate> pendingCandidates;
    if (type == "offer") {
        LDebug("Processing remote offer");
        pc->setRemoteDescription(rtc::Description(sdp, type));
        pc->setLocalDescription(rtc::Description::Type::Answer);
    }
    else if (type == "answer") {
        LDebug("Processing remote answer");
        pc->setRemoteDescription(rtc::Description(sdp, type));
    }

    {
        std::lock_guard lock(_mutex);
        if (_pc != pc)
            return;
        _remoteDescriptionSet = true;
        pendingCandidates.swap(_pendingRemoteCandidates);
    }

    for (const auto& pending : pendingCandidates) {
        LDebug("Adding deferred remote ICE candidate");
        pc->addRemoteCandidate(rtc::Candidate(pending.candidate, pending.mid));
    }
}


void PeerSession::onCandidateReceived(const std::string& peerId,
                                      const std::string& candidate,
                                      const std::string& mid)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    bool defer = false;
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId)
            return;
        if (_state != State::Negotiating && _state != State::Active)
            return;
        pc = _pc;
        if (!_pc || !_remoteDescriptionSet) {
            _pendingRemoteCandidates.push_back({candidate, mid});
            defer = true;
        }
    }

    if (defer) {
        LDebug("Queueing remote ICE candidate until remote description is set");
        return;
    }

    LDebug("Adding remote ICE candidate");
    pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}


} // namespace wrtc
} // namespace icy
