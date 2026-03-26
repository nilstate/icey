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


#include "icy/webrtc/track.h"
#include "icy/webrtc/trackreceiver.h"
#include "icy/webrtc/tracksender.h"
#include "icy/webrtc/webrtc.h"

#include "icy/av/codec.h"
#include "icy/signal.h"

#include <rtc/rtc.hpp>

#include <memory>
#include <mutex>
#include <string>


namespace icy {
namespace wrtc {


/// Convenience wrapper that creates WebRTC tracks on a PeerConnection
/// and exposes per-track sender/receiver adapters for PacketStream
/// integration.
///
/// For video-only, leave audioCodec default (disabled).
/// For audio-only, leave videoCodec default (disabled).
/// For data-channel-only, don't create a MediaBridge at all.
///
/// For full control over individual tracks, use createVideoTrack(),
/// createAudioTrack(), WebRtcTrackSender, and WebRtcTrackReceiver
/// directly instead of this class.
///
/// The sender and receiver adapter objects are stable for the lifetime of the
/// MediaBridge. attach() and detach() rebind or unbind the underlying WebRTC
/// tracks without invalidating references previously obtained from
/// videoSender(), audioSender(), videoReceiver(), or audioReceiver().
///
/// Example - send camera to browser:
///
///   MediaBridge bridge;
///   bridge.attach(pc, {.videoCodec = {"H264", "libx264", 1280, 720, 30}});
///
///   PacketStream stream;
///   stream.attachSource(capture);
///   stream.attach(encoder, 1, true);
///   stream.attach(&bridge.videoSender(), 5, false);
///   stream.start();
///
/// Example - receive from browser and record:
///
///   bridge.videoReceiver().emitter +=
///       packetSlot(&recorder, &Recorder::onEncodedVideo);
///
/// The receiver emits owning encoded packets. Feed those into a decoder or
/// recorder callback; see `samples/media-recorder` for a complete example.
class WEBRTC_API MediaBridge
{
public:
    /// Configuration options for the WebRTC media bridge
    struct Options
    {
        /// Video codec for the send track. Leave both name and encoder empty
        /// to skip creating a video track.
        av::VideoCodec videoCodec;

        /// Audio codec for the send track. Leave both name and encoder empty
        /// to skip creating an audio track.
        av::AudioCodec audioCodec;

        uint32_t videoSsrc = 0;   ///< 0 = auto-generate
        uint32_t audioSsrc = 0;   ///< 0 = auto-generate
        int videoPayloadType = -1; ///< Reuse negotiated offer payload type when answering, -1 = default.
        int audioPayloadType = -1; ///< Reuse negotiated offer payload type when answering, -1 = default.
        std::string cname;        ///< CNAME for RTCP (auto if empty)
        std::string videoMid;     ///< Explicit MID for the negotiated video m-line when answering an offer.
        std::string audioMid;     ///< Explicit MID for the negotiated audio m-line when answering an offer.
        rtc::Description::Direction videoDirection = rtc::Description::Direction::SendRecv;
        rtc::Description::Direction audioDirection = rtc::Description::Direction::SendRecv;
        unsigned nackBufferSize = 512; ///< Max RTP packets retained for video NACK retransmission.
    };

    /// Construct a detached bridge with stable sender and receiver adapters.
    MediaBridge();
    /// Destroy the bridge and release any attached PeerConnection state.
    ~MediaBridge();

    /// Deleted copy constructor; MediaBridge owns live track and adapter state.
    MediaBridge(const MediaBridge&) = delete;
    MediaBridge& operator=(const MediaBridge&) = delete;

    /// Create tracks on the PeerConnection and set up handler chains.
    /// Only creates tracks for codecs with a non-empty encoder name.
    void attach(std::shared_ptr<rtc::PeerConnection> pc,
                const Options& opts);

    /// Detach all tracks and adapters.
    void detach();

    /// Request an immediate keyframe (IDR) from the remote sender.
    /// Sends a PLI (Picture Loss Indication) RTCP message on the video track.
    /// No-op if no video track is attached.
    void requestKeyframe();

    /// Request that the remote sender reduce to a target bitrate.
    /// Sends a TMMBR RTCP message on the video track.
    /// @param bitrate  Target bitrate in bits per second.
    void requestBitrate(unsigned int bitrate);

    //
    // Per-track PacketStream adapters
    //

    /// Video send processor. Attach to a PacketStream after a VideoEncoder.
    /// Throws if no video track was created.
    [[nodiscard]] WebRtcTrackSender& videoSender();

    /// Audio send processor. Attach to a PacketStream after an AudioEncoder.
    /// Throws if no audio track was created.
    [[nodiscard]] WebRtcTrackSender& audioSender();

    /// Video receive adapter. Attach as a PacketStream source.
    /// Only valid after a remote video track arrives.
    [[nodiscard]] WebRtcTrackReceiver& videoReceiver();

    /// Audio receive adapter. Attach as a PacketStream source.
    /// Only valid after a remote audio track arrives.
    [[nodiscard]] WebRtcTrackReceiver& audioReceiver();

    //
    // RTCP feedback signals
    //

    /// Remote peer requests a keyframe. Connect to encoder to force IDR.
    ThreadSignal<void()> KeyframeRequested;

    /// Remote peer reports estimated bandwidth (bits/sec).
    ThreadSignal<void(unsigned int)> BitrateEstimate;

    //
    // Accessors
    //

    /// The underlying libdatachannel video track, or nullptr if none was created.
    [[nodiscard]] std::shared_ptr<rtc::Track> videoTrack() const;

    /// The underlying libdatachannel audio track, or nullptr if none was created.
    [[nodiscard]] std::shared_ptr<rtc::Track> audioTrack() const;

    /// True if a video track was created at attach().
    [[nodiscard]] bool hasVideo() const;

    /// True if an audio track was created at attach().
    [[nodiscard]] bool hasAudio() const;

    /// True if attach() has been called and a PeerConnection is held.
    [[nodiscard]] bool attached() const;

private:
    std::shared_ptr<rtc::PeerConnection> _pc;

    // Send side. The adapter objects stay alive for the whole MediaBridge
    // lifetime so PacketStream attachments do not dangle across attach/detach.
    WebRtcTrackSender _videoSender;
    WebRtcTrackSender _audioSender;
    TrackHandle _videoHandle;
    TrackHandle _audioHandle;

    // Receive side (one per track, created on onTrack)
    WebRtcTrackReceiver _videoReceiver;
    WebRtcTrackReceiver _audioReceiver;
    std::shared_ptr<rtc::Track> _videoReceiveTrack;
    std::shared_ptr<rtc::Track> _audioReceiveTrack;

    mutable std::mutex _mutex;
};


} // namespace wrtc
} // namespace icy


/// @}
