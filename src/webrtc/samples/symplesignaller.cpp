///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "symplesignaller.h"
#include "icy/logger.h"


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
    // Map to Symple call protocol: "offer" → "call:offer", "answer" → "call:answer"
    std::string subtype = "call:" + type;

    json::Value data;
    data["type"] = type;
    data["sdp"] = sdp;

    send(subtype, peerId, data);
}


void SympleSignaller::sendCandidate(const std::string& peerId,
                                     const std::string& candidate,
                                     const std::string& mid)
{
    json::Value data;
    data["candidate"] = candidate;
    data["sdpMid"] = mid;

    send("call:candidate", peerId, data);
}


void SympleSignaller::sendControl(const std::string& peerId,
                                   const std::string& type,
                                   const std::string& reason)
{
    // Map control types: "init" → "call:init", "accept" → "call:accept", etc.
    std::string subtype = "call:" + type;

    json::Value data;
    if (!reason.empty())
        data["reason"] = reason;

    send(subtype, peerId, data);
}


void SympleSignaller::onMessage(smpl::Message& msg)
{
    auto it = msg.find("subtype");
    if (it == msg.end())
        return;

    std::string subtype = it->get<std::string>();
    if (subtype.substr(0, 5) != "call:")
        return;

    std::string peerId = msg.from().toString();
    std::string action = subtype.substr(5);  // strip "call:" prefix

    LDebug("Symple signalling: ", subtype, " from ", peerId);

    if (action == "offer" || action == "answer") {
        auto& data = msg["data"];
        std::string type = data["type"].get<std::string>();
        std::string sdp = data["sdp"].get<std::string>();
        SdpReceived.emit(peerId, type, sdp);
    }
    else if (action == "candidate") {
        auto& data = msg["data"];
        std::string candidate = data["candidate"].get<std::string>();
        std::string mid = data["sdpMid"].get<std::string>();
        CandidateReceived.emit(peerId, candidate, mid);
    }
    else if (action == "init" || action == "accept" || action == "reject" || action == "hangup") {
        std::string reason;
        if (msg.find("data") != msg.end() &&
            msg["data"].find("reason") != msg["data"].end())
            reason = msg["data"]["reason"].get<std::string>();
        ControlReceived.emit(peerId, action, reason);
    }
}


void SympleSignaller::send(const std::string& subtype,
                            const std::string& to,
                            const json::Value& data)
{
    smpl::Message msg;
    msg.setType("message");
    msg["subtype"] = subtype;
    msg.setTo(smpl::Address(to));
    if (!data.empty())
        msg["data"] = data;

    _client.send(msg);
}


} // namespace wrtc
} // namespace icy
