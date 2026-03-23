///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/base.h"

#ifdef HAVE_FFMPEG

#include "icy/av/videoencoder.h"
#include "icy/packetstream.h"


namespace icy {
namespace av {


/// PacketProcessor that encodes raw video frames (PlanarVideoPacket
/// or VideoPacket) into compressed packets via VideoEncoder.
///
/// Drop this into a PacketStream between a source that emits raw
/// frames (e.g. MediaCapture) and a sink that expects encoded data
/// (e.g. WebRtcTrackSender or MultiplexPacketEncoder).
///
/// Example:
///
///   auto encoder = std::make_shared<av::VideoPacketEncoder>();
///   encoder->iparams = capture->videoCodec();   // decoded format
///   encoder->oparams = av::VideoCodec("H264", "libx264", 640, 480, 30);
///
///   PacketStream stream;
///   stream.attachSource(capture);
///   stream.attach(encoder, 1, true);
///   stream.attach(&webrtcSender, 5, false);
///   stream.start();
class AV_API VideoPacketEncoder : public VideoEncoder
    , public PacketProcessor
{
public:
    VideoPacketEncoder(AVFormatContext* format = nullptr);
    ~VideoPacketEncoder() noexcept override;

    VideoPacketEncoder(const VideoPacketEncoder&) = delete;
    VideoPacketEncoder& operator=(const VideoPacketEncoder&) = delete;

    /// Process a VideoPacket or PlanarVideoPacket from the stream.
    /// Encodes the frame and emits the resulting compressed packet.
    void process(IPacket& packet) override;

    /// Accept VideoPacket and PlanarVideoPacket types.
    bool accepts(IPacket* packet) override;

protected:
    void onStreamStateChange(const PacketStreamState& state) override;

    friend class PacketStream;

    bool _initialized = false;
    mutable std::mutex _mutex;
};


} // namespace av
} // namespace icy


#endif


/// @}
