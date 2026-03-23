///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
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
{
    _signaller.SdpReceived += slot(this, &PeerSession::onSdpReceived);
    _signaller.CandidateReceived += slot(this, &PeerSession::onCandidateReceived);
    _signaller.ControlReceived += slot(this, &PeerSession::onControlReceived);
}


PeerSession::~PeerSession()
{
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
    StateChanged.emit(State::OutgoingInit);
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
        remotePeerId = _remotePeerId;
    }
    createPeerConnection(false);
    StateChanged.emit(State::Negotiating);
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
    StateChanged.emit(State::Ending);
    _signaller.sendControl(remotePeerId, "reject", reason);
    if (dc)
        dc->close();
    if (pc)
        pc->close();
    finishEndCall();
    StateChanged.emit(State::Ended);
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
    StateChanged.emit(State::Ending);
    if (!remotePeerId.empty())
        _signaller.sendControl(remotePeerId, "hangup", reason);
    if (dc)
        dc->close();
    if (pc)
        pc->close();
    finishEndCall();
    StateChanged.emit(State::Ended);
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


// ---------------------------------------------------------------------------
// Signalling callbacks
// ---------------------------------------------------------------------------

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
        StateChanged.emit(State::IncomingInit);
        IncomingCall.emit(peerId);
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
            if (pc) {
                pc->setLocalDescription(rtc::Description::Type::Offer);
            }
        }
        StateChanged.emit(State::Negotiating);
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
        StateChanged.emit(State::Ending);
        if (dc)
            dc->close();
        if (pc)
            pc->close();
        finishEndCall();
        StateChanged.emit(State::Ended);
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
        StateChanged.emit(State::Ending);
        if (dc)
            dc->close();
        if (pc)
            pc->close();
        finishEndCall();
        StateChanged.emit(State::Ended);
        transitionEndedToIdle();
    }
}


void PeerSession::onSdpReceived(const std::string& peerId,
                                 const std::string& type,
                                 const std::string& sdp)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId || !_pc)
            return;
        if (_state != State::Negotiating && _state != State::Active)
            return;
        pc = _pc;
    }

    if (type == "offer") {
        LDebug("Processing remote offer");
        pc->setRemoteDescription(rtc::Description(sdp, type));
        pc->setLocalDescription(rtc::Description::Type::Answer);
    }
    else if (type == "answer") {
        LDebug("Processing remote answer");
        pc->setRemoteDescription(rtc::Description(sdp, type));
    }
}


void PeerSession::onCandidateReceived(const std::string& peerId,
                                       const std::string& candidate,
                                       const std::string& mid)
{
    std::shared_ptr<rtc::PeerConnection> pc;
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId || !_pc)
            return;
        if (_state != State::Negotiating && _state != State::Active)
            return;
        pc = _pc;
    }

    LDebug("Adding remote ICE candidate");
    pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}


// ---------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------

std::shared_ptr<rtc::PeerConnection> PeerSession::createPeerConnection(bool createDataChannel)
{
    auto pc = std::make_shared<rtc::PeerConnection>(_config.rtcConfig);
    std::shared_ptr<rtc::DataChannel> dc;
    setupPeerConnectionCallbacks(pc);

    if (createDataChannel && _config.enableDataChannel) {
        dc = pc->createDataChannel(_config.dataChannelLabel);
        dc->onOpen([this]() { LDebug("Data channel open"); });
        dc->onMessage([this](rtc::message_variant msg) {
            DataReceived.emit(std::move(msg));
        });
    }

    pc->onDataChannel([this, pc](std::shared_ptr<rtc::DataChannel> dc) {
        std::lock_guard lock(_mutex);
        if (_pc != pc || _dc)
            return;

        _dc = dc;
        _dc->onMessage([this](rtc::message_variant msg) {
            DataReceived.emit(std::move(msg));
        });
    });

    {
        std::lock_guard lock(_mutex);
        _pc = std::move(pc);
        _dc = std::move(dc);
        pc = _pc;
    }

    if (!_media.attached())
        _media.attach(pc, _config.mediaOpts);

    return pc;
}


void PeerSession::setupPeerConnectionCallbacks(const std::shared_ptr<rtc::PeerConnection>& pc)
{
    pc->onLocalDescription([this, pc](rtc::Description desc) {
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

    pc->onLocalCandidate([this, pc](rtc::Candidate candidate) {
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

    pc->onStateChange([this, pc](rtc::PeerConnection::State rtcState) {
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
            StateChanged.emit(newState);
            if (shouldFinish) {
                finishEndCall();
                StateChanged.emit(State::Ended);
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
        StateChanged.emit(State::Idle);
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
