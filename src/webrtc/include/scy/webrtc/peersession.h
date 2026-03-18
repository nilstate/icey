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
#include "scy/webrtc/webrtc.h"

#include "scy/signal.h"
#include "scy/symple/client.h"

#include <rtc/rtc.hpp>

#include <memory>
#include <mutex>
#include <string>


namespace scy {
namespace wrtc {


/// Manages a WebRTC peer connection lifecycle with Symple signalling.
///
/// This is the Symple-specific convenience class. It speaks the call
/// protocol defined in symple-client-player's call-manager.js:
///
///   call:init       Caller initiates
///   call:accept     Callee accepts
///   call:reject     Callee rejects
///   call:offer      SDP offer
///   call:answer     SDP answer
///   call:candidate  ICE candidate
///   call:hangup     Either side ends
///
/// For custom signalling backends, use createVideoTrack(),
/// WebRtcTrackSender, WebRtcTrackReceiver, and rtc::PeerConnection
/// directly. PeerSession is not required.
///
/// Media is optional. Set mediaOpts codecs to enable video/audio tracks.
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

    PeerSession(smpl::Client& signaller, const Config& config);
    ~PeerSession();

    PeerSession(const PeerSession&) = delete;
    PeerSession& operator=(const PeerSession&) = delete;

    //
    // Call control
    //

    void call(const std::string& peerId);
    void accept();
    void reject(const std::string& reason = "declined");
    void hangup(const std::string& reason = "hangup");

    //
    // Data channel
    //

    void sendData(const std::string& message);
    void sendData(const std::byte* data, size_t size);

    //
    // Signals
    //

    Signal<void(State)> StateChanged;
    Signal<void(const std::string&)> IncomingCall;
    Signal<void(rtc::message_variant)> DataReceived;

    //
    // Accessors
    //

    [[nodiscard]] State state() const;
    [[nodiscard]] std::string remotePeerId() const;

    /// Access the MediaBridge for PacketStream integration.
    /// Only valid after the PeerConnection is created (Connecting state).
    [[nodiscard]] MediaBridge& media();
    [[nodiscard]] const MediaBridge& media() const;

    [[nodiscard]] std::shared_ptr<rtc::PeerConnection> peerConnection();
    [[nodiscard]] std::shared_ptr<rtc::DataChannel> dataChannel();

private:
    void onSympleMessage(smpl::Message& msg);
    void onCallInit(const std::string& peerId);
    void onCallAccept(const std::string& peerId);
    void onCallReject(const std::string& peerId, const std::string& reason);
    void onCallOffer(const std::string& peerId, const smpl::Message& msg);
    void onCallAnswer(const std::string& peerId, const smpl::Message& msg);
    void onCallCandidate(const std::string& peerId, const smpl::Message& msg);
    void onCallHangup(const std::string& peerId, const std::string& reason);

    void createPeerConnection();
    void setupPeerConnectionCallbacks();
    void doEndCall(const std::string& reason);
    void sendCallMessage(const std::string& subtype,
                         const std::string& to,
                         const json::Value& data = {});

    smpl::Client& _signaller;
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
