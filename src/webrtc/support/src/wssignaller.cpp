///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/webrtc/support/wssignaller.h"
#include "icy/logger.h"


namespace icy {
namespace wrtc {


WebSocketSignaller::WebSocketSignaller(const std::string& localPeerId)
    : _localPeerId(localPeerId)
{
}


WebSocketSignaller::~WebSocketSignaller() = default;


void WebSocketSignaller::sendSdp(const std::string& peerId,
                                 const std::string& type,
                                 const std::string& sdp)
{
    json::Value msg;
    msg["type"] = type;
    msg["peerId"] = peerId;
    msg["sdp"] = sdp;
    if (!_localPeerId.empty())
        msg["from"] = _localPeerId;
    sendJson(msg);
}


void WebSocketSignaller::sendCandidate(const std::string& peerId,
                                       const std::string& candidate,
                                       const std::string& mid)
{
    json::Value msg;
    msg["type"] = "candidate";
    msg["peerId"] = peerId;
    msg["candidate"] = candidate;
    msg["mid"] = mid;
    if (!_localPeerId.empty())
        msg["from"] = _localPeerId;
    sendJson(msg);
}


void WebSocketSignaller::sendControl(const std::string& peerId,
                                     const std::string& type,
                                     const std::string& reason)
{
    json::Value msg;
    msg["type"] = type;
    msg["peerId"] = peerId;
    if (!reason.empty())
        msg["reason"] = reason;
    if (!_localPeerId.empty())
        msg["from"] = _localPeerId;
    sendJson(msg);
}


void WebSocketSignaller::receive(const std::string& jsonStr)
{
    try {
        auto msg = json::Value::parse(jsonStr);

        std::string type = msg.value("type", "");
        std::string peerId = msg.value("from", msg.value("peerId", ""));

        if (type.empty() || peerId.empty()) {
            LWarn("WebSocket signaller: malformed message (missing type or peerId)");
            return;
        }

        if (type == "offer" || type == "answer") {
            std::string sdp = msg.value("sdp", "");
            if (!sdp.empty())
                SdpReceived.emit(peerId, type, sdp);
        }
        else if (type == "candidate") {
            std::string candidate = msg.value("candidate", "");
            std::string mid = msg.value("mid", "");
            if (!candidate.empty())
                CandidateReceived.emit(peerId, candidate, mid);
        }
        else if (type == "init" || type == "accept" || type == "reject" || type == "hangup") {
            ControlReceived.emit(peerId, type, msg.value("reason", ""));
        }
    }
    catch (const std::exception& e) {
        LWarn("WebSocket signaller: parse error: ", e.what());
    }
}


void WebSocketSignaller::sendJson(const json::Value& msg)
{
    std::lock_guard lock(_mutex);
    SendMessage.emit(msg.dump());
}


} // namespace wrtc
} // namespace icy
