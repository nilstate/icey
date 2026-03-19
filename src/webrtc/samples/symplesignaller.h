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
#include "icy/symple/client.h"


namespace icy {
namespace wrtc {


/// SignallingInterface implementation using the Symple protocol.
///
/// Speaks the call:init/accept/reject/offer/answer/candidate/hangup
/// protocol defined in symple-client-player's call-manager.js.
/// Compatible with any browser running symple-client + CallManager.
///
/// Usage:
///   smpl::Client symple(opts);
///   symple.connect();
///
///   SympleSignaller signaller(symple);
///   PeerSession session(signaller, config);
class WEBRTC_API SympleSignaller : public SignallingInterface
{
public:
    /// Construct bound to a Symple client.
    /// The Symple client must outlive this signaller.
    explicit SympleSignaller(smpl::Client& client);
    ~SympleSignaller() override;

    SympleSignaller(const SympleSignaller&) = delete;
    SympleSignaller& operator=(const SympleSignaller&) = delete;

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override;

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override;

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override;

private:
    void onMessage(smpl::Message& msg);
    void send(const std::string& subtype,
              const std::string& to,
              const json::Value& data = {});

    smpl::Client& _client;
};


} // namespace wrtc
} // namespace icy


/// @\}
