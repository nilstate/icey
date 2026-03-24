///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "icy/webrtc/signalling.h"
#include "icy/webrtc/webrtc.h"

#include <mutex>
#include <string>


namespace icy {
namespace wrtc {


/// SignallingInterface implementation using plain JSON over WebSocket.
///
/// No Symple, no Socket.IO, no external dependencies. Just raw
/// WebSocket messages with a simple JSON schema:
///
///   {"type": "offer",     "peerId": "...", "sdp": "..."}
///   {"type": "answer",    "peerId": "...", "sdp": "..."}
///   {"type": "candidate", "peerId": "...", "candidate": "...", "mid": "..."}
///   {"type": "init",      "peerId": "..."}
///   {"type": "accept",    "peerId": "..."}
///   {"type": "reject",    "peerId": "...", "reason": "..."}
///   {"type": "hangup",    "peerId": "...", "reason": "..."}
///
/// For the media-server app, the server side implements this directly
/// on its embedded WebSocket server. For standalone clients, connect
/// to a signalling endpoint that relays these messages between peers.
///
/// Usage (client side):
///   http::ClientConnection ws("wss://server/ws");
///   WebSocketSignaller signaller(ws);
///   PeerSession session(signaller, config);
///
/// Usage (server side):
///   Implement in your WebSocket responder by calling the
///   receive methods when messages arrive, and connecting
///   send methods to the WebSocket write path.
class WEBRTC_API WebSocketSignaller : public SignallingInterface
{
public:
    /// Construct with a local peer ID (identifies us in messages).
    explicit WebSocketSignaller(const std::string& localPeerId = {});
    ~WebSocketSignaller() override;

    WebSocketSignaller(const WebSocketSignaller&) = delete;
    WebSocketSignaller& operator=(const WebSocketSignaller&) = delete;

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override;

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override;

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override;

    /// Parse an incoming JSON message and fire the appropriate signal.
    /// Call this from your WebSocket message handler.
    void receive(const std::string& json);

    /// Emitted when a message needs to be sent over the WebSocket.
    /// Connect this to your WebSocket's send method.
    /// The string is a JSON-encoded message ready to transmit.
    Signal<void(const std::string&)> SendMessage;

    void setLocalPeerId(const std::string& id) { _localPeerId = id; }
    [[nodiscard]] const std::string& localPeerId() const { return _localPeerId; }

private:
    void sendJson(const json::Value& msg);

    std::string _localPeerId;
    std::mutex _mutex;
};


} // namespace wrtc
} // namespace icy


/// @}
