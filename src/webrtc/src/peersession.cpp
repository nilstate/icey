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
    {
        std::lock_guard lock(_mutex);
        if (_state != State::Idle)
            throw std::logic_error(
                std::string("Cannot call: already in state ") + stateToString(_state));

        LInfo("Initiating call to ", peerId);
        _remotePeerId = peerId;
        _state = State::Ringing;
        _signaller.sendControl(peerId, "init");
    }
    StateChanged.emit(State::Ringing);
}


void PeerSession::accept()
{
    {
        std::lock_guard lock(_mutex);
        if (_state != State::Incoming)
            throw std::logic_error("Cannot accept: no incoming call");

        LInfo("Accepting call from ", _remotePeerId);
        _state = State::Connecting;
        _signaller.sendControl(_remotePeerId, "accept");
        createPeerConnection();
    }
    StateChanged.emit(State::Connecting);
}


void PeerSession::reject(const std::string& reason)
{
    {
        std::lock_guard lock(_mutex);
        if (_state != State::Incoming)
            throw std::logic_error("Cannot reject: no incoming call");

        LInfo("Rejecting call from ", _remotePeerId, " reason=", reason);
        _signaller.sendControl(_remotePeerId, "reject", reason);
        doEndCall("rejected");
    }
    StateChanged.emit(State::Ended);
}


void PeerSession::hangup(const std::string& reason)
{
    {
        std::lock_guard lock(_mutex);
        if (_state == State::Idle || _state == State::Ended)
            return;

        LInfo("Hanging up, reason=", reason);
        if (!_remotePeerId.empty())
            _signaller.sendControl(_remotePeerId, "hangup", reason);
        doEndCall(reason);
    }
    StateChanged.emit(State::Ended);
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
        {
            std::lock_guard lock(_mutex);
            if (_state != State::Idle) {
                _signaller.sendControl(peerId, "reject", "busy");
                return;
            }
            _remotePeerId = peerId;
            _state = State::Incoming;
        }
        StateChanged.emit(State::Incoming);
        IncomingCall.emit(peerId);
    }
    else if (type == "accept") {
        {
            std::lock_guard lock(_mutex);
            if (_state != State::Ringing || peerId != _remotePeerId)
                return;

            LInfo("Remote accepted call");
            _state = State::Connecting;
            createPeerConnection();
            _media.attach(_pc, _config.mediaOpts);
            _pc->setLocalDescription(rtc::Description::Type::Offer);
        }
        StateChanged.emit(State::Connecting);
    }
    else if (type == "reject") {
        {
            std::lock_guard lock(_mutex);
            if (_state != State::Ringing || peerId != _remotePeerId)
                return;

            LInfo("Remote rejected call: ", reason);
            doEndCall("rejected");
        }
        StateChanged.emit(State::Ended);
    }
    else if (type == "hangup") {
        {
            std::lock_guard lock(_mutex);
            if (peerId != _remotePeerId)
                return;

            LInfo("Remote hung up: ", reason);
            doEndCall(reason);
        }
        StateChanged.emit(State::Ended);
    }
}


void PeerSession::onSdpReceived(const std::string& peerId,
                                 const std::string& type,
                                 const std::string& sdp)
{
    std::lock_guard lock(_mutex);
    if (peerId != _remotePeerId || !_pc)
        return;

    if (type == "offer") {
        LDebug("Processing remote offer");
        if (!_media.attached())
            _media.attach(_pc, _config.mediaOpts);
        _pc->setRemoteDescription(rtc::Description(sdp, type));
        _pc->setLocalDescription(rtc::Description::Type::Answer);
    }
    else if (type == "answer") {
        LDebug("Processing remote answer");
        _pc->setRemoteDescription(rtc::Description(sdp, type));
    }
}


void PeerSession::onCandidateReceived(const std::string& peerId,
                                       const std::string& candidate,
                                       const std::string& mid)
{
    std::lock_guard lock(_mutex);
    if (peerId != _remotePeerId || !_pc)
        return;

    LDebug("Adding remote ICE candidate");
    _pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}


// ---------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------

void PeerSession::createPeerConnection()
{
    _pc = std::make_shared<rtc::PeerConnection>(_config.rtcConfig);
    setupPeerConnectionCallbacks();

    if (_config.enableDataChannel) {
        _dc = _pc->createDataChannel(_config.dataChannelLabel);
        _dc->onOpen([this]() { LDebug("Data channel open"); });
        _dc->onMessage([this](rtc::message_variant msg) {
            DataReceived.emit(std::move(msg));
        });
    }

    _pc->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        std::lock_guard lock(_mutex);
        if (!_dc) {
            _dc = dc;
            _dc->onMessage([this](rtc::message_variant msg) {
                DataReceived.emit(std::move(msg));
            });
        }
    });
}


void PeerSession::setupPeerConnectionCallbacks()
{
    _pc->onLocalDescription([this](rtc::Description desc) {
        LDebug("Local SDP ready: ", desc.typeString());
        std::string peerId;
        {
            std::lock_guard lock(_mutex);
            peerId = _remotePeerId;
        }
        _signaller.sendSdp(peerId, desc.typeString(), std::string(desc));
    });

    _pc->onLocalCandidate([this](rtc::Candidate candidate) {
        LDebug("Local ICE candidate");
        std::string peerId;
        {
            std::lock_guard lock(_mutex);
            peerId = _remotePeerId;
        }
        _signaller.sendCandidate(peerId, std::string(candidate), candidate.mid());
    });

    _pc->onStateChange([this](rtc::PeerConnection::State rtcState) {
        LDebug("PeerConnection state: ", static_cast<int>(rtcState));

        State newState = State::Idle;
        bool changed = false;

        {
            std::lock_guard lock(_mutex);
            switch (rtcState) {
            case rtc::PeerConnection::State::Connected:
                if (_state == State::Connecting) {
                    _state = State::Active;
                    newState = State::Active;
                    changed = true;
                }
                break;
            case rtc::PeerConnection::State::Failed:
                doEndCall("connection failed");
                newState = State::Ended;
                changed = true;
                break;
            case rtc::PeerConnection::State::Disconnected:
                doEndCall("disconnected");
                newState = State::Ended;
                changed = true;
                break;
            case rtc::PeerConnection::State::Closed:
                if (_state != State::Idle && _state != State::Ended) {
                    doEndCall("closed");
                    newState = State::Ended;
                    changed = true;
                }
                break;
            default:
                break;
            }
        }

        if (changed)
            StateChanged.emit(newState);
    });
}


void PeerSession::doEndCall(const std::string& reason)
{
    LDebug("Ending call: ", reason);
    _media.detach();

    if (_dc) {
        _dc->close();
        _dc.reset();
    }
    if (_pc) {
        _pc->close();
        _pc.reset();
    }

    _remotePeerId.clear();
    _state = State::Ended;
}


const char* stateToString(PeerSession::State state)
{
    switch (state) {
    case PeerSession::State::Idle:       return "idle";
    case PeerSession::State::Ringing:    return "ringing";
    case PeerSession::State::Incoming:   return "incoming";
    case PeerSession::State::Connecting: return "connecting";
    case PeerSession::State::Active:     return "active";
    case PeerSession::State::Ended:      return "ended";
    }
    return "unknown";
}


} // namespace wrtc
} // namespace icy
