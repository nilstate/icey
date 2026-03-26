///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtcsupport
/// @{


#pragma once


#include "icy/webrtc/signalling.h"
#include "icy/webrtc/support/support.h"

#include "icy/json/json.h"
#include "icy/synchronizer.h"
#include "icy/symple/server.h"

#include <deque>
#include <mutex>
#include <string>


namespace icy {
namespace wrtc {


/// Server-side Symple signaller for virtual peers and embedded apps.
///
/// This adapter sends signalling messages through a `smpl::Server` on behalf
/// of a local virtual peer address, and accepts inbound Symple call messages
/// through `onMessage()`.
///
/// The remote peer identifier on this signalling boundary is the full
/// Symple address string (`user|id`).
class SympleServerSignaller : public SignallingInterface
{
public:
    SympleServerSignaller(smpl::Server& server,
                          std::string localAddress,
                          std::string remoteAddress = {});
    ~SympleServerSignaller() override;

    SympleServerSignaller(const SympleServerSignaller&) = delete;
    SympleServerSignaller& operator=(const SympleServerSignaller&) = delete;

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override;

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override;

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override;

    void onMessage(const json::Value& msg);

    void setRemoteAddress(std::string remoteAddress);
    [[nodiscard]] const std::string& localAddress() const { return _localAddress; }
    [[nodiscard]] const std::string& remoteAddress() const { return _remoteAddress; }

private:
    struct OutboundMessage
    {
        std::string peerAddress;
        json::Value msg;
    };

    void send(const std::string& action,
              const std::string& to,
              const json::Value& data = {});
    bool sendNow(const std::string& to, const json::Value& msg);
    void flushPending();

    smpl::Server& _server;
    std::string _localAddress;
    std::string _remoteAddress;
    Synchronizer _dispatch;
    std::mutex _mutex;
    std::deque<OutboundMessage> _pending;
    bool _closing = false;
};


} // namespace wrtc
} // namespace icy


/// @}
