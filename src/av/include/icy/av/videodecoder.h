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

#include "icy/av/packet.h"
#include "icy/av/videocontext.h"


namespace icy {
namespace av {


/// Decodes compressed video packets into raw frames
struct VideoDecoder : public VideoContext
{
    /// Construct a decoder for the given stream.
    /// The codec parameters are read from the stream's codecpar.
    /// @param stream  The AVStream to decode; must remain valid for the lifetime of this decoder.
    VideoDecoder(AVStream* stream);
    ~VideoDecoder() noexcept override;

    /// Initialise the AVCodecContext from the stream's codec parameters.
    virtual void create() override;

    /// Open the codec and initialise any required pixel format conversion context.
    virtual void open() override;

    /// Close and free the AVCodecContext and associated resources.
    virtual void close() override;

    /// Decode the given compressed video packet and emit the decoded frame.
    /// Input packets must use the raw AVStream time base; time base conversion
    /// to microseconds is performed internally.
    /// @param ipacket  The compressed video packet to decode.
    /// @return True if an output frame was decoded and emitted, false otherwise.
    [[nodiscard]] bool decode(AVPacket& ipacket) override;

    /// Flush any frames buffered inside the decoder.
    /// Call repeatedly after the last packet until false is returned.
    void flush() override;
};


} // namespace av
} // namespace icy


#endif


/// @}
