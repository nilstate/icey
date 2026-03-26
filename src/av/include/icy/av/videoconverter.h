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

#include "icy/av/packet.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


namespace icy {
namespace av {


/// Converts video frames between pixel formats and resolutions
struct VideoConverter
{
    VideoConverter();
    virtual ~VideoConverter() noexcept;

    VideoConverter(const VideoConverter&) = delete;
    VideoConverter& operator=(const VideoConverter&) = delete;
    VideoConverter(VideoConverter&&) = delete;
    VideoConverter& operator=(VideoConverter&&) = delete;

    /// Initialise the libswscale context and allocate the output frame.
    /// Uses iparams and oparams to configure the conversion pipeline.
    /// Throws std::runtime_error if already initialised or if parameters are invalid.
    virtual void create();

    /// Free the libswscale context and the output frame.
    virtual void close();

    /// Convert @p iframe to the output pixel format and resolution.
    /// The returned frame is owned by this converter and is overwritten on the next call.
    /// @param iframe  The source AVFrame; must match iparams dimensions and pixel format.
    /// @return The converted output AVFrame.
    virtual AVFrame* convert(AVFrame* iframe);

    SwsContext* ctx;    ///< libswscale conversion context.
    AVFrame* oframe;    ///< Reusable output frame allocated by create().
    VideoCodec iparams; ///< Expected input video parameters.
    VideoCodec oparams; ///< Target output video parameters.
};


} // namespace av
} // namespace icy


#endif


/// @}
