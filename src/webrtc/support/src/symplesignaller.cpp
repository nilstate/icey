///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/support/symplesignaller.h"
#include "callprotocol.h"


namespace icy {
namespace wrtc {


SympleSignaller::SympleSignaller(smpl::Client& client)
    : _client(client)
{
    _client += packetSlot(this, &SympleSignaller::onMessage);
}


SympleSignaller::~SympleSignaller()
{
    _client -= packetSlot(this, &SympleSignaller::onMessage);
}


void SympleSignaller::sendSdp(const std::string& peerId,
                              const std::string& type,
                              const std::string& sdp)
{
    json::Value data;
    data["type"] = type;
    data["sdp"] = sdp;
    send(type, peerId, data);
}


void SympleSignaller::sendCandidate(const std::string& peerId,
                                    const std::string& candidate,
                                    const std::string& mid)
{
    json::Value data;
    data["candidate"] = candidate;
    data["sdpMid"] = mid;
    send("candidate", peerId, data);
}


void SympleSignaller::sendControl(const std::string& peerId,
                                  const std::string& type,
                                  const std::string& reason)
{
    json::Value data;
    if (!reason.empty())
        data["reason"] = reason;
    send(type, peerId, data);
}


void SympleSignaller::onMessage(smpl::Message& msg)
{
    auto parsed = callproto::parseSympleMessage(msg);
    if (!parsed)
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


void SympleSignaller::send(const std::string& action,
                           const std::string& to,
                           const json::Value& data)
{
    smpl::Message msg(callproto::makeSympleMessage(action, to, data));
    _client.send(msg);
}


} // namespace wrtc
} // namespace icy
