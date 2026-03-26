///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/support/sympleserversignaller.h"
#include "callprotocol.h"
#include "icy/symple/address.h"

#include <stdexcept>


namespace icy {
namespace wrtc {


SympleServerSignaller::SympleServerSignaller(smpl::Server& server,
                                             std::string localAddress,
                                             std::string remoteAddress)
    : _server(server)
    , _localAddress(std::move(localAddress))
    , _remoteAddress(std::move(remoteAddress))
    , _dispatch([this]() { flushPending(); }, server.loop())
{
    if (!callproto::isFullPeerAddress(_localAddress))
        throw std::invalid_argument("SympleServerSignaller localAddress must be user|id");
    if (!_remoteAddress.empty() && !callproto::isFullPeerAddress(_remoteAddress))
        throw std::invalid_argument("SympleServerSignaller remoteAddress must be user|id");
}


SympleServerSignaller::~SympleServerSignaller()
{
    {
        std::lock_guard lock(_mutex);
        _closing = true;
        _pending.clear();
    }
    _dispatch.close();
}


void SympleServerSignaller::sendSdp(const std::string& peerId,
                                    const std::string& type,
                                    const std::string& sdp)
{
    json::Value data;
    data["type"] = type;
    data["sdp"] = sdp;
    send(type, peerId, data);
}


void SympleServerSignaller::sendCandidate(const std::string& peerId,
                                          const std::string& candidate,
                                          const std::string& mid)
{
    json::Value data;
    data["candidate"] = candidate;
    data["sdpMid"] = mid;
    send("candidate", peerId, data);
}


void SympleServerSignaller::sendControl(const std::string& peerId,
                                        const std::string& type,
                                        const std::string& reason)
{
    json::Value data;
    if (!reason.empty())
        data["reason"] = reason;
    send(type, peerId, data);
}


void SympleServerSignaller::onMessage(const json::Value& msg)
{
    auto parsed = callproto::parseSympleMessage(msg);
    if (!parsed)
        return;
    if (!_remoteAddress.empty() && parsed->peerAddress != _remoteAddress)
        return;

    switch (parsed->kind) {
    case callproto::Kind::Sdp:
        SdpReceived.emit(parsed->peerAddress, parsed->sdpType, parsed->sdp);
        break;
    case callproto::Kind::Candidate:
        CandidateReceived.emit(parsed->peerAddress, parsed->candidate, parsed->mid);
        break;
    case callproto::Kind::Control:
        ControlReceived.emit(parsed->peerAddress, parsed->action, parsed->reason);
        break;
    }
}


void SympleServerSignaller::setRemoteAddress(std::string remoteAddress)
{
    if (!remoteAddress.empty() && !callproto::isFullPeerAddress(remoteAddress))
        throw std::invalid_argument("SympleServerSignaller remoteAddress must be user|id");
    std::lock_guard lock(_mutex);
    _remoteAddress = std::move(remoteAddress);
}


void SympleServerSignaller::send(const std::string& action,
                                 const std::string& to,
                                 const json::Value& data)
{
    json::Value msg = callproto::makeSympleMessage(action, to, data, _localAddress);

    if (std::this_thread::get_id() == _dispatch.tid()) {
        (void)sendNow(to, msg);
        return;
    }

    {
        std::lock_guard lock(_mutex);
        if (_closing)
            return;
        _pending.push_back({to, std::move(msg)});
    }
    _dispatch.post();
}


bool SympleServerSignaller::sendNow(const std::string& to, const json::Value& msg)
{
    smpl::Address address(to);
    if (!address.id.empty())
        return _server.sendTo(address.id, msg);
    if (!address.user.empty())
        return _server.sendToUser(address.user, msg);
    return false;
}


void SympleServerSignaller::flushPending()
{
    std::deque<OutboundMessage> pending;
    {
        std::lock_guard lock(_mutex);
        pending.swap(_pending);
    }

    for (auto& outbound : pending)
        (void)sendNow(outbound.peerAddress, outbound.msg);
}


} // namespace wrtc
} // namespace icy
