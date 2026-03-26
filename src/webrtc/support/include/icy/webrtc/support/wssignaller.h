///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtcsupport
/// @{


#pragma once


#include "icy/webrtc/signalling.h"
#include "icy/webrtc/support/support.h"

#include "icy/json/json.h"

#include <mutex>
#include <string>


namespace icy {
namespace wrtc {


/// SignallingInterface implementation using plain JSON over WebSocket.
class WebSocketSignaller : public SignallingInterface
{
public:
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

    void receive(const std::string& json);

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
