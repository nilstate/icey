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


#include "icy/webrtc/webrtc.h"

#include "icy/av/packet.h"
#include "icy/packetsignal.h"
#include "icy/packetstream.h"

#include <rtc/rtc.hpp>

#include <memory>


namespace icy {
namespace wrtc {


/// PacketStreamAdapter that receives depacketized frames from a single
/// remote libdatachannel Track and emits them as VideoPacket or
/// AudioPacket into a PacketStream.
///
/// Call setupReceiveTrack() on the track first to install the correct
/// depacketizer, then bind this receiver to it.
///
/// Usage:
///   pc->onTrack([&](shared_ptr<rtc::Track> track) {
///       if (setupReceiveTrack(track))
///           videoReceiver.bind(track);
///   });
///
///   videoReceiver.emitter += packetSlot(&recorder, &Recorder::onEncodedVideo);
///
/// Emits owning packets - the frame data is copied, so downstream
/// processors can safely queue packets asynchronously.
///
/// Emits VideoPacket for video tracks, AudioPacket for audio tracks.
/// Use those packets to drive a decoder, recorder, or custom pipeline.
/// See `samples/media-recorder` for a complete receive -> decode -> file example.
class WEBRTC_API WebRtcTrackReceiver : public PacketStreamAdapter
{
public:
    /// Construct an unbound receiver. Call bind() to attach a remote track.
    WebRtcTrackReceiver();

    /// Bind to a remote track. Must be called after setupReceiveTrack()
    /// returned true.
    /// Installs an onFrame callback that converts each depacketized frame to
    /// a VideoPacket or AudioPacket and emits it on the PacketStream.
    /// The track type (video/audio) is detected from the SDP description.
    /// @param track  Remote track from the PeerConnection::onTrack callback.
    void bind(std::shared_ptr<rtc::Track> track);

    PacketSignal emitter;
};


} // namespace wrtc
} // namespace icy


/// @}
