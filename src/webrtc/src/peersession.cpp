///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "scy/webrtc/peersession.h"
#include "scy/logger.h"

#include <stdexcept>
#include <utility>


namespace scy {
namespace wrtc {


PeerSession::PeerSession(smpl::Client& signaller, const Config& config)
    : _signaller(signaller)
    , _config(config)
{
    _signaller += packetSlot(this, &PeerSession::onSympleMessage);
}


PeerSession::~PeerSession()
{
    _signaller -= packetSlot(this, &PeerSession::onSympleMessage);
    if (_state != State::Idle && _state != State::Ended) {
        try { hangup("destroyed"); } catch (...) {}
    }
}


// ---------------------------------------------------------------------------
// Call control
// ---------------------------------------------------------------------------

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
        sendCallMessage("call:init", peerId);
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
        sendCallMessage("call:accept", _remotePeerId);
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
        json::Value data;
        data["reason"] = reason;
        sendCallMessage("call:reject", _remotePeerId, data);
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
        if (!_remotePeerId.empty()) {
            json::Value data;
            data["reason"] = reason;
            sendCallMessage("call:hangup", _remotePeerId, data);
        }
        doEndCall(reason);
    }
    StateChanged.emit(State::Ended);
}


// ---------------------------------------------------------------------------
// Data channel
// ---------------------------------------------------------------------------

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


// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

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
// Symple message routing
// ---------------------------------------------------------------------------

void PeerSession::onSympleMessage(smpl::Message& msg)
{
    auto it = msg.find("subtype");
    if (it == msg.end())
        return;

    std::string subtype = it->get<std::string>();
    if (subtype.substr(0, 5) != "call:")
        return;

    std::string peerId = msg.from().toString();
    LDebug("Received ", subtype, " from ", peerId);

    if (subtype == "call:init")
        onCallInit(peerId);
    else if (subtype == "call:accept")
        onCallAccept(peerId);
    else if (subtype == "call:reject") {
        std::string reason = "declined";
        if (msg.find("data") != msg.end() &&
            msg["data"].find("reason") != msg["data"].end())
            reason = msg["data"]["reason"].get<std::string>();
        onCallReject(peerId, reason);
    }
    else if (subtype == "call:offer")
        onCallOffer(peerId, msg);
    else if (subtype == "call:answer")
        onCallAnswer(peerId, msg);
    else if (subtype == "call:candidate")
        onCallCandidate(peerId, msg);
    else if (subtype == "call:hangup") {
        std::string reason = "remote hangup";
        if (msg.find("data") != msg.end() &&
            msg["data"].find("reason") != msg["data"].end())
            reason = msg["data"]["reason"].get<std::string>();
        onCallHangup(peerId, reason);
    }
}


void PeerSession::onCallInit(const std::string& peerId)
{
    {
        std::lock_guard lock(_mutex);
        if (_state != State::Idle) {
            json::Value data;
            data["reason"] = "busy";
            sendCallMessage("call:reject", peerId, data);
            return;
        }
        _remotePeerId = peerId;
        _state = State::Incoming;
    }
    StateChanged.emit(State::Incoming);
    IncomingCall.emit(peerId);
}


void PeerSession::onCallAccept(const std::string& peerId)
{
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


void PeerSession::onCallReject(const std::string& peerId, const std::string& reason)
{
    {
        std::lock_guard lock(_mutex);
        if (_state != State::Ringing || peerId != _remotePeerId)
            return;

        LInfo("Remote rejected call: ", reason);
        doEndCall("rejected");
    }
    StateChanged.emit(State::Ended);
}


void PeerSession::onCallOffer(const std::string& peerId, const smpl::Message& msg)
{
    std::lock_guard lock(_mutex);
    if (peerId != _remotePeerId || !_pc)
        return;

    auto& data = msg["data"];
    std::string type = data["type"].get<std::string>();
    std::string sdp = data["sdp"].get<std::string>();

    LDebug("Processing remote offer");

    if (!_media.attached())
        _media.attach(_pc, _config.mediaOpts);

    _pc->setRemoteDescription(rtc::Description(sdp, type));
    _pc->setLocalDescription(rtc::Description::Type::Answer);
}


void PeerSession::onCallAnswer(const std::string& peerId, const smpl::Message& msg)
{
    std::lock_guard lock(_mutex);
    if (peerId != _remotePeerId || !_pc)
        return;

    auto& data = msg["data"];
    std::string type = data["type"].get<std::string>();
    std::string sdp = data["sdp"].get<std::string>();

    LDebug("Processing remote answer");
    _pc->setRemoteDescription(rtc::Description(sdp, type));
}


void PeerSession::onCallCandidate(const std::string& peerId, const smpl::Message& msg)
{
    std::lock_guard lock(_mutex);
    if (peerId != _remotePeerId || !_pc)
        return;

    auto& data = msg["data"];
    std::string candidate = data["candidate"].get<std::string>();
    std::string sdpMid = data["sdpMid"].get<std::string>();

    LDebug("Adding remote ICE candidate");
    _pc->addRemoteCandidate(rtc::Candidate(candidate, sdpMid));
}


void PeerSession::onCallHangup(const std::string& peerId, const std::string& reason)
{
    {
        std::lock_guard lock(_mutex);
        if (peerId != _remotePeerId)
            return;

        LInfo("Remote hung up: ", reason);
        doEndCall(reason);
    }
    StateChanged.emit(State::Ended);
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

        json::Value data;
        data["type"] = desc.typeString();
        data["sdp"] = std::string(desc);

        std::string subtype = (desc.type() == rtc::Description::Type::Offer)
            ? "call:offer" : "call:answer";

        std::lock_guard lock(_mutex);
        sendCallMessage(subtype, _remotePeerId, data);
    });

    _pc->onLocalCandidate([this](rtc::Candidate candidate) {
        LDebug("Local ICE candidate");

        json::Value data;
        data["candidate"] = std::string(candidate);
        data["sdpMid"] = candidate.mid();

        std::lock_guard lock(_mutex);
        sendCallMessage("call:candidate", _remotePeerId, data);
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


void PeerSession::sendCallMessage(const std::string& subtype,
                                   const std::string& to,
                                   const json::Value& data)
{
    smpl::Message msg;
    msg.setType("message");
    msg["subtype"] = subtype;
    msg.setTo(smpl::Address(to));
    if (!data.empty())
        msg["data"] = data;

    _signaller.send(msg);
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
} // namespace scy
