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

#include "icy/av/ffmpeg.h"
#include "icy/av/format.h"
#include "icy/av/packet.h"
#include "icy/av/videocontext.h"


namespace icy {
namespace av {


/// Encodes raw video frames into a compressed format
struct VideoEncoder : public VideoContext
{
    /// Construct an encoder, optionally tied to an existing muxer context.
    /// @param format  The AVFormatContext that will receive encoded packets, or nullptr for standalone use.
    VideoEncoder(AVFormatContext* format = nullptr);
    ~VideoEncoder() noexcept override;

    /// Initialise the AVCodecContext using oparams.
    /// Adds a video stream to @p format if one was provided at construction.
    virtual void create() override;

    /// Close and free the AVCodecContext and associated resources.
    virtual void close() override;

    /// Encode a single video frame.
    /// This method is for interleaved video formats.
    ///
    /// @param data  The raw video frame buffer.
    /// @param size  The buffer size in bytes.
    /// @param pts   The presentation timestamp in stream time base units.
    [[nodiscard]] bool encode(uint8_t* data, int size, int64_t pts) override;

    /// Encode a single video frame.
    /// This method is for planar video formats.
    ///
    /// @param data      Array of per-plane data pointers (up to 4 planes).
    /// @param linesize  Array of per-plane byte strides.
    /// @param pts       The presentation timestamp in stream time base units.
    [[nodiscard]] bool encode(uint8_t* data[4], int linesize[4], int64_t pts) override;

    /// Encode a single AVFrame (typically from a decoder or converter).
    /// @param iframe  The source video frame with all fields set.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] bool encode(AVFrame* iframe) override;

    /// Flush remaining packets to be encoded.
    /// This method should be called once before stream closure.
    void flush() override;

    AVFormatContext* format;
};


} // namespace av
} // namespace icy


#endif


/// @}
