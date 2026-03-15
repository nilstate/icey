///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"
#include "scy/av/format.h"
#include "scy/av/packet.h"
#include "scy/av/videocontext.h"


namespace scy {
namespace av {


struct VideoEncoder : public VideoContext
{
    VideoEncoder(AVFormatContext* format = nullptr);
    ~VideoEncoder() noexcept override;

    virtual void create() override;
    virtual void close() override;

    /// Encode a single video frame.
    /// This method is for interleaved video formats.
    ///
    /// @param data  The raw video frame buffer.
    /// @param size  The buffer size in bytes.
    /// @param pts   The presentation timestamp in stream time base units.
    [[nodiscard]] virtual bool encode(uint8_t* data, int size, int64_t pts);

    /// Encode a single video frame.
    /// This method is for planar video formats.
    ///
    /// @param data      Array of per-plane data pointers (up to 4 planes).
    /// @param linesize  Array of per-plane byte strides.
    /// @param pts       The presentation timestamp in stream time base units.
    [[nodiscard]] virtual bool encode(uint8_t* data[4], int linesize[4], int64_t pts);

    /// Encode a single AVFrame.
    [[nodiscard]] virtual bool encode(AVFrame* iframe);

    /// Flush remaining packets to be encoded.
    /// This method should be called once before stream closure.
    virtual void flush();

    AVFormatContext* format;
};


} // namespace av
} // namespace scy


#endif


/// @\}
