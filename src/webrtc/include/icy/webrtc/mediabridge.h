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
///   PacketStream stream;
///   stream.attachSource(&bridge.videoReceiver(), false, true);
///   stream.attach(decoder, 1, true);
///   stream.attach(fileEncoder, 5, true);
///   stream.start();
class WEBRTC_API MediaBridge
{
public:
    struct Options
    {
        /// Video codec for the send track. Leave encoder empty to skip
        /// creating a video track.
        av::VideoCodec videoCodec;

        /// Audio codec for the send track. Leave encoder empty to skip
        /// creating an audio track.
        av::AudioCodec audioCodec;

        uint32_t videoSsrc = 0;   ///< 0 = auto-generate
        uint32_t audioSsrc = 0;   ///< 0 = auto-generate
        std::string cname;        ///< CNAME for RTCP (auto if empty)
        unsigned nackBufferSize = 512;
    };

    MediaBridge();
    ~MediaBridge();

    MediaBridge(const MediaBridge&) = delete;
    MediaBridge& operator=(const MediaBridge&) = delete;

    /// Create tracks on the PeerConnection and set up handler chains.
    /// Only creates tracks for codecs with a non-empty encoder name.
    void attach(std::shared_ptr<rtc::PeerConnection> pc,
                const Options& opts);

    /// Detach all tracks and adapters.
    void detach();

    void requestKeyframe();
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
    Signal<void()> KeyframeRequested;

    /// Remote peer reports estimated bandwidth (bits/sec).
    Signal<void(unsigned int)> BitrateEstimate;

    //
    // Accessors
    //

    [[nodiscard]] std::shared_ptr<rtc::Track> videoTrack() const;
    [[nodiscard]] std::shared_ptr<rtc::Track> audioTrack() const;
    [[nodiscard]] bool hasVideo() const;
    [[nodiscard]] bool hasAudio() const;
    [[nodiscard]] bool attached() const;

private:
    std::shared_ptr<rtc::PeerConnection> _pc;

    // Send side (one per track, optional)
    std::unique_ptr<WebRtcTrackSender> _videoSender;
    std::unique_ptr<WebRtcTrackSender> _audioSender;
    TrackHandle _videoHandle;
    TrackHandle _audioHandle;

    // Receive side (one per track, created on onTrack)
    WebRtcTrackReceiver _videoReceiver;
    WebRtcTrackReceiver _audioReceiver;

    mutable std::mutex _mutex;
};


} // namespace wrtc
} // namespace icy


/// @\}
