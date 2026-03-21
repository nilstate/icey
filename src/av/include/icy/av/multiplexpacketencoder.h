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
#include "icy/av/multiplexencoder.h"
#include "icy/packetstream.h"


namespace icy {
namespace av {


/// Encodes and multiplexes a realtime video stream form
/// audio / video capture sources.
/// FFmpeg is used for encoding.
class AV_API MultiplexPacketEncoder : public MultiplexEncoder
    , public PacketProcessor
{
public:
    /// Construct the encoder with the given options.
    /// @param options  The encoder configuration (input/output formats and file paths).
    MultiplexPacketEncoder(const EncoderOptions& options = EncoderOptions());
    virtual ~MultiplexPacketEncoder() noexcept;

    MultiplexPacketEncoder(const MultiplexPacketEncoder&) = delete;
    MultiplexPacketEncoder& operator=(const MultiplexPacketEncoder&) = delete;
    MultiplexPacketEncoder(MultiplexPacketEncoder&&) = delete;
    MultiplexPacketEncoder& operator=(MultiplexPacketEncoder&&) = delete;

    /// Encode a VideoPacket, dispatching to the planar or interleaved encode path as appropriate.
    /// @param packet  The video packet to encode.
    virtual void encode(VideoPacket& packet);

    /// Encode an AudioPacket, dispatching to the planar or interleaved encode path as appropriate.
    /// @param packet  The audio packet to encode.
    virtual void encode(AudioPacket& packet);

    /// @return True if the packet is an av::MediaPacket (audio or video).
    virtual bool accepts(IPacket* packet) override;

    /// Dispatch the incoming packet to encode(VideoPacket&) or encode(AudioPacket&).
    /// Throws std::invalid_argument if the packet type is unrecognised.
    /// @param packet  The incoming media packet.
    virtual void process(IPacket& packet) override;

protected:
    virtual void onStreamStateChange(const PacketStreamState& state) override;

    friend class PacketStream;

    mutable std::mutex _mutex;
};


} // namespace av
} // namespace icy


#endif
