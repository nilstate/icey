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

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


namespace icy {
namespace av {


struct VideoConverter
{
    VideoConverter();
    virtual ~VideoConverter() noexcept;

    VideoConverter(const VideoConverter&) = delete;
    VideoConverter& operator=(const VideoConverter&) = delete;
    VideoConverter(VideoConverter&&) = delete;
    VideoConverter& operator=(VideoConverter&&) = delete;

    virtual void create();
    virtual void close();

    virtual AVFrame* convert(AVFrame* iframe);

    SwsContext* ctx;
    AVFrame* oframe;
    VideoCodec iparams;
    VideoCodec oparams;
};


} // namespace av
} // namespace icy


#endif


/// @\}
