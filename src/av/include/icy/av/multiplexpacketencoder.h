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
    MultiplexPacketEncoder(const EncoderOptions& options = EncoderOptions());
    virtual ~MultiplexPacketEncoder() noexcept;

    MultiplexPacketEncoder(const MultiplexPacketEncoder&) = delete;
    MultiplexPacketEncoder& operator=(const MultiplexPacketEncoder&) = delete;
    MultiplexPacketEncoder(MultiplexPacketEncoder&&) = delete;
    MultiplexPacketEncoder& operator=(MultiplexPacketEncoder&&) = delete;

    virtual void encode(VideoPacket& packet);
    virtual void encode(AudioPacket& packet);

    virtual bool accepts(IPacket* packet) override;
    virtual void process(IPacket& packet) override;

protected:
    virtual void onStreamStateChange(const PacketStreamState& state) override;

    friend class PacketStream;

    mutable std::mutex _mutex;
};


} // namespace av
} // namespace icy


#endif
