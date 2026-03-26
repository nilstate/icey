///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/base.h"

#ifdef HAVE_FFMPEG

#include "icy/av/audioencoder.h"
#include "icy/packetstream.h"


namespace icy {
namespace av {


/// PacketProcessor that encodes raw audio samples (PlanarAudioPacket
/// or AudioPacket) into compressed packets via AudioEncoder.
///
/// Drop this into a PacketStream between a source that emits raw
/// audio (e.g. MediaCapture) and a sink that expects encoded data
/// (e.g. WebRtcTrackSender or MultiplexPacketEncoder).
///
/// Example:
///
///   auto encoder = std::make_shared<av::AudioPacketEncoder>();
///   encoder->oparams = av::AudioCodec("opus", "libopus", 2, 48000);
///
///   PacketStream stream;
///   stream.attachSource(capture);
///   stream.attach(encoder, 1, true);
///   stream.attach(&webrtcSender, 5, false);
///   stream.start();
class AV_API AudioPacketEncoder : public AudioEncoder
    , public PacketProcessor
{
public:
    AudioPacketEncoder(AVFormatContext* format = nullptr);
    ~AudioPacketEncoder() noexcept override;

    AudioPacketEncoder(const AudioPacketEncoder&) = delete;
    AudioPacketEncoder& operator=(const AudioPacketEncoder&) = delete;

    /// Process an AudioPacket or PlanarAudioPacket from the stream.
    /// Encodes the samples and emits the resulting compressed packet.
    void process(IPacket& packet) override;

    /// Accept AudioPacket and PlanarAudioPacket types.
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
