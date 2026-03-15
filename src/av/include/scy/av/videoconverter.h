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

#include "scy/av/packet.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


namespace scy {
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
} // namespace scy


#endif


/// @\}
