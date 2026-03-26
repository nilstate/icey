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


#include "icy/webrtc/mediabridge.h"
#include "icy/webrtc/signalling.h"
#include "icy/webrtc/webrtc.h"

#include "icy/signal.h"
#include "icy/synchronizer.h"

#include <rtc/rtc.hpp>

#include <deque>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>


namespace icy {
namespace wrtc {


/// Manages a WebRTC peer connection lifecycle over any signalling
/// transport that implements SignallingInterface.
///
/// Works with SympleSignaller (Symple call protocol), WebSocketSignaller
/// (plain JSON over WSS), or any custom implementation.
///
/// Media is optional. Set `Config::media` codecs to enable tracks.
/// Leave codec encoders empty for data-channel-only sessions.
class WEBRTC_API PeerSession
{
public:
    /// High-level lifecycle phases for a single peer-to-peer call session.
    enum class State
    {
        Idle,          ///< No active call
        OutgoingInit,  ///< Outgoing call announced, waiting for accept/reject
        IncomingInit,  ///< Incoming call announced, waiting for accept/reject
        Negotiating,   ///< PeerConnection exists and SDP/ICE negotiation is in progress
        Active,        ///< Media or data is flowing
        Ending,        ///< Local teardown is in progress
        Ended          ///< Call ended (transient; auto-resets to Idle)
    };

    /// Configuration for WebRTC peer session establishment.
    struct MediaConfig
    {
        av::VideoCodec videoCodec;   ///< Desired video codec for send/receive negotiation.
        av::AudioCodec audioCodec;   ///< Desired audio codec for send/receive negotiation.
        rtc::Description::Direction videoDirection = rtc::Description::Direction::SendRecv;
        rtc::Description::Direction audioDirection = rtc::Description::Direction::SendRecv;
    };

    /// Configuration for WebRTC peer session establishment.
    struct Config
    {
        rtc::Configuration rtcConfig;     ///< libdatachannel connection options, ICE servers, and transport settings.
        MediaConfig media;                ///< Desired media codecs and directions for the session.
        bool enableDataChannel = true;    ///< True to create a data channel on outgoing calls and accept one on incoming calls.
        std::string dataChannelLabel = "data"; ///< Label to use for the application data channel.
    };

    /// Construct with any signalling implementation.
    /// The signaller must outlive this PeerSession.
    PeerSession(SignallingInterface& signaller, const Config& config);
    ~PeerSession();

    /// Deleted copy constructor; PeerSession owns live signalling and RTC callbacks.
    PeerSession(const PeerSession&) = delete;
    PeerSession& operator=(const PeerSession&) = delete;

    /// Initiate an outgoing call to a remote peer.
    /// Sends a "init" control message and transitions to OutgoingInit.
    /// @param peerId  Remote peer identifier passed to the signaller.
    /// @throws std::logic_error if not currently in the Idle state.
    void call(const std::string& peerId);

    /// Accept an incoming call.
    /// Creates the PeerConnection, sends "accept", and transitions to Negotiating.
    /// @throws std::logic_error if not currently in the IncomingInit state.
    void accept();

    /// Reject an incoming call.
    /// Sends a "reject" control message and transitions to Ended.
    /// @param reason  Human-readable reason string forwarded to the remote peer.
    /// @throws std::logic_error if not currently in the IncomingInit state.
    void reject(const std::string& reason = "declined");

    /// Terminate any non-idle call phase.
    /// Sends a "hangup" control message, closes the PeerConnection, and
    /// transitions to Ended. Safe to call from any non-Idle/Ended state.
    /// @param reason  Human-readable reason string forwarded to the remote peer.
    void hangup(const std::string& reason = "hangup");

    /// Send a UTF-8 string message over the data channel.
    /// Silently dropped if the data channel is not open.
    /// @param message  String to send.
    void sendData(const std::string& message);

    /// Send raw binary data over the data channel.
    /// Silently dropped if the data channel is not open.
    /// @param data  Pointer to the byte buffer.
    /// @param size  Number of bytes to send.
    void sendData(const std::byte* data, size_t size);

    /// Emitted whenever the session state changes.
    /// Parameter: new State value.
    ThreadSignal<void(State)> StateChanged;

    /// Emitted when a remote peer initiates a call (state transitions to IncomingInit).
    /// Parameter: remote peer identifier.
    ThreadSignal<void(const std::string&)> IncomingCall;

    /// Emitted when a message arrives on the data channel.
    /// Parameter: rtc::message_variant (string or binary).
    ThreadSignal<void(rtc::message_variant)> DataReceived;

    /// Current session state. Thread-safe.
    [[nodiscard]] State state() const;

    /// Identifier of the remote peer for the current or most recent call.
    /// Empty when Idle.
    [[nodiscard]] std::string remotePeerId() const;

    /// Media bridge for this session. Valid for the lifetime of the PeerSession.
    [[nodiscard]] MediaBridge& media();
    /// @copydoc media()
    [[nodiscard]] const MediaBridge& media() const;

    /// The underlying PeerConnection, or nullptr when Idle/Ended.
    [[nodiscard]] std::shared_ptr<rtc::PeerConnection> peerConnection();

    /// The data channel, or nullptr if none is open.
    [[nodiscard]] std::shared_ptr<rtc::DataChannel> dataChannel();

private:
    struct CallbackGuard
    {
        std::atomic<bool> alive{true};
    };

    struct PendingCandidate
    {
        std::string candidate;
        std::string mid;
    };

    void onSdpReceived(const std::string& peerId, const std::string& type, const std::string& sdp);
    void onCandidateReceived(const std::string& peerId, const std::string& candidate, const std::string& mid);
    void onControlReceived(const std::string& peerId, const std::string& type, const std::string& reason);

    std::shared_ptr<rtc::PeerConnection> createPeerConnection(
        bool createDataChannel,
        const MediaBridge::Options* mediaOpts = nullptr);
    void setupPeerConnectionCallbacks(const std::shared_ptr<rtc::PeerConnection>& pc);
    void beginEndCall(const std::string& reason,
                      std::shared_ptr<rtc::PeerConnection>& pc,
                      std::shared_ptr<rtc::DataChannel>& dc);
    void finishEndCall();
    void transitionEndedToIdle();
    void enqueueCallback(std::function<void()> callback);
    void drainCallbacks();

    SignallingInterface& _signaller;
    Config _config;
    MediaBridge _media;
    State _state = State::Idle;
    std::string _remotePeerId;
    std::shared_ptr<rtc::PeerConnection> _pc;
    std::shared_ptr<rtc::DataChannel> _dc;
    std::shared_ptr<CallbackGuard> _callbackGuard = std::make_shared<CallbackGuard>();
    bool _remoteDescriptionSet = false;
    std::vector<PendingCandidate> _pendingRemoteCandidates;
    Synchronizer _callbackSync;
    std::deque<std::function<void()>> _pendingCallbacks;
    mutable std::mutex _callbackMutex;
    mutable std::mutex _mutex;
};


/// Convert a PeerSession::State to a lowercase C string for logging.
/// @param state  State value to convert.
/// @return       One of: "idle", "outgoing-init", "incoming-init", "negotiating",
///               "active", "ending", "ended".
[[nodiscard]] WEBRTC_API const char* stateToString(PeerSession::State state);


} // namespace wrtc
} // namespace icy


/// @}
