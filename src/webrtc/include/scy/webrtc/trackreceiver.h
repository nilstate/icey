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


#include "scy/webrtc/webrtc.h"

#include "scy/av/packet.h"
#include "scy/packetsignal.h"
#include "scy/packetstream.h"

#include <rtc/rtc.hpp>

#include <memory>


namespace scy {
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
///       setupReceiveTrack(track);
///       videoReceiver.bind(track);
///   });
///
///   PacketStream stream;
///   stream.attachSource(&videoReceiver, false, true);
///   stream.attach(decoder, 1, true);
///   stream.start();
///
/// Emits owning packets - the frame data is copied, so downstream
/// processors can safely queue packets asynchronously.
///
/// Emits VideoPacket for video tracks, AudioPacket for audio tracks.
/// Downstream processors (VideoDecoder, AudioDecoder) can
/// dynamic_cast to the correct type.
class WEBRTC_API WebRtcTrackReceiver : public PacketStreamAdapter
{
public:
    WebRtcTrackReceiver();

    /// Bind to a remote track. Must be called after setupReceiveTrack().
    /// The track type (video/audio) is detected from the SDP description.
    void bind(std::shared_ptr<rtc::Track> track);

    PacketSignal emitter;
};


} // namespace wrtc
} // namespace scy


/// @\}
