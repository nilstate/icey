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
#include "icy/symple/client.h"


namespace icy {
namespace wrtc {


/// SignallingInterface implementation using the Symple call protocol.
///
/// Speaks the `call:init`, `call:accept`, `call:reject`, `call:offer`,
/// `call:answer`, `call:candidate`, and `call:hangup` envelope over a
/// connected Symple client.
///
/// The remote peer identifier on this signalling boundary is the full
/// Symple address string (`user|id`).
class SympleSignaller : public SignallingInterface
{
public:
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
    void send(const std::string& action,
              const std::string& to,
              const json::Value& data = {});

    smpl::Client& _client;
};


} // namespace wrtc
} // namespace icy


/// @}
