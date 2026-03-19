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

#include "icy/av/mediacapture.h"


namespace icy {
namespace av {


/// This class implements a cross platform audio capturer.
class AV_API VideoCapture : public MediaCapture
{
public:
    using Ptr = std::shared_ptr<VideoCapture>;

    VideoCapture();
    VideoCapture(const std::string& device, const av::VideoCodec& params);
    VideoCapture(const std::string& device, int width = 0, int height = 0,
                 double framerate = 0, const std::string& pixelFmt = "");
    virtual ~VideoCapture() noexcept;

    void openVideo(const std::string& device, const av::VideoCodec& params) override;
    virtual void openVideo(const std::string& device, int width = 0,
                           int height = 0, double framerate = 0,
                           const std::string& pixelFmt = "");
};


} // namespace av
} // namespace icy


#endif


/// @\}
