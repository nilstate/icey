///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/peersession.h"
#include "icy/logger.h"

#include <stdexcept>


namespace icy {
namespace wrtc {

PeerSession::PeerSession(SignallingInterface& signaller, const Config& config)
    : _signaller(signaller)
    , _config(config)
    , _callbackSync([this]() { drainCallbacks(); })
{
    // PeerSession owns SDP exchange explicitly through SignallingInterface.
    // Auto-negotiation races against the init/accept/offer protocol and can
    // emit stray local offers on the incoming path.
    _config.rtcConfig.disableAutoNegotiation = true;

    _signaller.SdpReceived += slot(this, &PeerSession::onSdpReceived);
    _signaller.CandidateReceived += slot(this, &PeerSession::onCandidateReceived);
    _signaller.ControlReceived += slot(this, &PeerSession::onControlReceived);
}


PeerSession::~PeerSession()
{
    _callbackGuard->alive.store(false, std::memory_order_release);
    {
        std::lock_guard lock(_callbackMutex);
        _pendingCallbacks.clear();
    }
    _callbackSync.close();

    _signaller.SdpReceived.detach(this);
    _signaller.CandidateReceived.detach(this);
    _signaller.ControlReceived.detach(this);

    if (_state != State::Idle && _state != State::Ended) {
        try { hangup("destroyed"); } catch (...) {}
    }
}


void PeerSession::call(const std::string& peerId)
{
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Ended)
            _state = State::Idle;
        if (_state != State::Idle)
            throw std::logic_error(
                std::string("Cannot call: already in state ") + stateToString(_state));

        LInfo("Initiating call to ", peerId);
        _remotePeerId = peerId;
        remotePeerId = _remotePeerId;
        _state = State::OutgoingInit;
    }
    enqueueCallback([this]() { StateChanged.emit(State::OutgoingInit); });
    _signaller.sendControl(remotePeerId, "init");
}


void PeerSession::accept()
{
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state != State::IncomingInit)
            throw std::logic_error("Cannot accept: no incoming call");

        LInfo("Accepting call from ", _remotePeerId);
        _state = State::Negotiating;
        _remoteDescriptionSet = false;
        _pendingRemoteCandidates.clear();
        remotePeerId = _remotePeerId;
    }
    enqueueCallback([this]() { StateChanged.emit(State::Negotiating); });
    _signaller.sendControl(remotePeerId, "accept");
}


void PeerSession::reject(const std::string& reason)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state != State::IncomingInit)
            throw std::logic_error("Cannot reject: no incoming call");

        LInfo("Rejecting call from ", _remotePeerId, " reason=", reason);
        remotePeerId = _remotePeerId;
        beginEndCall("rejected", pc, dc);
    }
    enqueueCallback([this]() { StateChanged.emit(State::Ending); });
    _signaller.sendControl(remotePeerId, "reject", reason);
    if (dc)
        dc->close();
    if (pc)
        pc->close();
    finishEndCall();
    enqueueCallback([this]() { StateChanged.emit(State::Ended); });
    transitionEndedToIdle();
}


void PeerSession::hangup(const std::string& reason)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> dc;
    std::string remotePeerId;
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Idle || _state == State::Ended || _state == State::Ending)
            return;

        LInfo("Hanging up, reason=", reason);
        remotePeerId = _remotePeerId;
        beginEndCall(reason, pc, dc);
    }
    enqueueCallback([this]() { StateChanged.emit(State::Ending); });
    if (!remotePeerId.empty())
        _signaller.sendControl(remotePeerId, "hangup", reason);
    if (dc)
        dc->close();
    if (pc)
        pc->close();
    finishEndCall();
    enqueueCallback([this]() { StateChanged.emit(State::Ended); });
    transitionEndedToIdle();
}


void PeerSession::sendData(const std::string& message)
{
    std::lock_guard lock(_mutex);
    if (_dc && _dc->isOpen())
        _dc->send(message);
}


void PeerSession::sendData(const std::byte* data, size_t size)
{
    std::lock_guard lock(_mutex);
    if (_dc && _dc->isOpen())
        _dc->send(data, size);
}


PeerSession::State PeerSession::state() const
{
    std::lock_guard lock(_mutex);
    return _state;
}

std::string PeerSession::remotePeerId() const
{
    std::lock_guard lock(_mutex);
    return _remotePeerId;
}

MediaBridge& PeerSession::media() { return _media; }
const MediaBridge& PeerSession::media() const { return _media; }

std::shared_ptr<rtc::PeerConnection> PeerSession::peerConnection()
{
    std::lock_guard lock(_mutex);
    return _pc;
}

std::shared_ptr<rtc::DataChannel> PeerSession::dataChannel()
{
    std::lock_guard lock(_mutex);
    return _dc;
}


void PeerSession::enqueueCallback(std::function<void()> callback)
{
    if (!_callbackGuard->alive.load(std::memory_order_acquire))
        return;

    {
        std::lock_guard lock(_callbackMutex);
        _pendingCallbacks.emplace_back(std::move(callback));
    }
    _callbackSync.post();
}


void PeerSession::drainCallbacks()
{
    std::deque<std::function<void()>> pending;
    {
        std::lock_guard lock(_callbackMutex);
        pending.swap(_pendingCallbacks);
    }

    if (!_callbackGuard->alive.load(std::memory_order_acquire))
        return;

    for (auto& callback : pending) {
        if (!_callbackGuard->alive.load(std::memory_order_acquire))
            return;
        callback();
    }
}


const char* stateToString(PeerSession::State state)
{
    switch (state) {
    case PeerSession::State::Idle:         return "idle";
    case PeerSession::State::OutgoingInit: return "outgoing-init";
    case PeerSession::State::IncomingInit: return "incoming-init";
    case PeerSession::State::Negotiating:  return "negotiating";
    case PeerSession::State::Active:       return "active";
    case PeerSession::State::Ending:       return "ending";
    case PeerSession::State::Ended:        return "ended";
    }
    return "unknown";
}


} // namespace wrtc
} // namespace icy
