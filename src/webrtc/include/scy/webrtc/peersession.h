///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "scy/webrtc/mediabridge.h"
#include "scy/webrtc/signalling.h"
#include "scy/webrtc/webrtc.h"

#include "scy/signal.h"

#include <rtc/rtc.hpp>

#include <memory>
#include <mutex>
#include <string>


namespace scy {
namespace wrtc {


/// Manages a WebRTC peer connection lifecycle over any signalling
/// transport that implements SignallingInterface.
///
/// Works with SympleSignaller (Symple call protocol), WebSocketSignaller
/// (plain JSON over WSS), or any custom implementation.
///
/// Media is optional. Set mediaOpts codecs to enable tracks.
/// Leave codec encoders empty for data-channel-only sessions.
class WEBRTC_API PeerSession
{
public:
    enum class State
    {
        Idle,        ///< No active call
        Ringing,     ///< Outgoing call, waiting for accept
        Incoming,    ///< Incoming call, waiting for user action
        Connecting,  ///< Accepted, WebRTC negotiation in progress
        Active,      ///< Media flowing
        Ended        ///< Call ended (auto-resets to Idle)
    };

    struct Config
    {
        rtc::Configuration rtcConfig;
        MediaBridge::Options mediaOpts;
        bool enableDataChannel = true;
        std::string dataChannelLabel = "data";
    };

    /// Construct with any signalling implementation.
    /// The signaller must outlive this PeerSession.
    PeerSession(SignallingInterface& signaller, const Config& config);
    ~PeerSession();

    PeerSession(const PeerSession&) = delete;
    PeerSession& operator=(const PeerSession&) = delete;

    void call(const std::string& peerId);
    void accept();
    void reject(const std::string& reason = "declined");
    void hangup(const std::string& reason = "hangup");

    void sendData(const std::string& message);
    void sendData(const std::byte* data, size_t size);

    Signal<void(State)> StateChanged;
    Signal<void(const std::string&)> IncomingCall;
    Signal<void(rtc::message_variant)> DataReceived;

    [[nodiscard]] State state() const;
    [[nodiscard]] std::string remotePeerId() const;
    [[nodiscard]] MediaBridge& media();
    [[nodiscard]] const MediaBridge& media() const;
    [[nodiscard]] std::shared_ptr<rtc::PeerConnection> peerConnection();
    [[nodiscard]] std::shared_ptr<rtc::DataChannel> dataChannel();

private:
    void onSdpReceived(const std::string& peerId, const std::string& type, const std::string& sdp);
    void onCandidateReceived(const std::string& peerId, const std::string& candidate, const std::string& mid);
    void onControlReceived(const std::string& peerId, const std::string& type, const std::string& reason);

    void createPeerConnection();
    void setupPeerConnectionCallbacks();
    void doEndCall(const std::string& reason);

    SignallingInterface& _signaller;
    Config _config;
    MediaBridge _media;
    State _state = State::Idle;
    std::string _remotePeerId;
    std::shared_ptr<rtc::PeerConnection> _pc;
    std::shared_ptr<rtc::DataChannel> _dc;
    mutable std::mutex _mutex;
};


[[nodiscard]] WEBRTC_API const char* stateToString(PeerSession::State state);


} // namespace wrtc
} // namespace scy


/// @\}
