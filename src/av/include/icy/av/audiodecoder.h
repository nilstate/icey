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

#include "icy/av/audiocontext.h"
#include "icy/av/packet.h"


namespace icy {
namespace av {


/// Decodes compressed audio packets into raw sample frames
struct AudioDecoder : public AudioContext
{
    /// Construct a decoder for the given stream.
    /// The codec parameters are read from the stream's codecpar.
    /// @param stream  The AVStream to decode; must remain valid for the lifetime of this decoder.
    AudioDecoder(AVStream* stream);
    ~AudioDecoder() noexcept override;

    /// Initialise the AVCodecContext from the stream's codec parameters.
    virtual void create() override;

    /// Close and free the AVCodecContext and associated resources.
    virtual void close() override;

    /// Decode the given compressed audio packet and emit the decoded samples.
    /// @param ipacket  The compressed audio packet to decode.
    /// @return True if one or more output frames were decoded and emitted, false otherwise.
    [[nodiscard]] bool decode(AVPacket& ipacket) override;

    /// Flush any frames buffered inside the decoder.
    /// Call this after the last packet to retrieve all remaining decoded output.
    void flush() override;
};


} // namespace av
} // namespace icy


#endif


/// @}
