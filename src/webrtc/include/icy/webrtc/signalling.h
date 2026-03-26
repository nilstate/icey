///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "icy/webrtc/webrtc.h"
#include "icy/signal.h"

#include <string>


namespace icy {
namespace wrtc {


/// Transport-agnostic signalling interface for WebRTC session setup.
///
/// Implementations handle the exchange of SDP offers/answers and ICE
/// candidates between peers over whatever transport is available:
/// Symple, plain WebSocket, REST, MQTT, carrier pigeon, etc.
///
/// PeerSession takes a reference to this interface. Implement it
/// to plug in your own signalling backend.
///
/// The three message categories:
///   - SDP: offer/answer exchange (the session description)
///   - Candidate: trickle ICE candidates
///   - Control: call lifecycle (init, accept, reject, hangup)
class WEBRTC_API SignallingInterface
{
public:
    virtual ~SignallingInterface() = default;

    //
    // Send to remote peer
    //

    /// Send an SDP offer or answer to the remote peer.
    /// @param peerId   Remote peer identifier.
    /// @param type     "offer" or "answer".
    /// @param sdp      The SDP string.
    virtual void sendSdp(const std::string& peerId,
                         const std::string& type,
                         const std::string& sdp) = 0;

    /// Send an ICE candidate to the remote peer.
    /// @param peerId     Remote peer identifier.
    /// @param candidate  The candidate string (from RTCIceCandidate).
    /// @param mid        The sdpMid value.
    virtual void sendCandidate(const std::string& peerId,
                               const std::string& candidate,
                               const std::string& mid) = 0;

    /// Send a control message to the remote peer.
    /// @param peerId   Remote peer identifier.
    /// @param type     Control type: "init", "accept", "reject", "hangup".
    /// @param reason   Optional reason string (for reject/hangup).
    virtual void sendControl(const std::string& peerId,
                             const std::string& type,
                             const std::string& reason = {}) = 0;

    //
    // Receive from remote peer (connect to these signals)
    //

    /// Fires when an SDP offer or answer arrives from a remote peer.
    /// Parameters: peerId, type ("offer"/"answer"), sdp.
    ThreadSignal<void(const std::string&, const std::string&, const std::string&)> SdpReceived;

    /// Fires when an ICE candidate arrives from a remote peer.
    /// Parameters: peerId, candidate, mid.
    ThreadSignal<void(const std::string&, const std::string&, const std::string&)> CandidateReceived;

    /// Fires when a control message arrives from a remote peer.
    /// Parameters: peerId, type ("init"/"accept"/"reject"/"hangup"), reason.
    ThreadSignal<void(const std::string&, const std::string&, const std::string&)> ControlReceived;
};


} // namespace wrtc
} // namespace icy


/// @}
