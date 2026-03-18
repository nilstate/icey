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

#include "scy/av/mediacapture.h"


namespace scy {
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
} // namespace scy


#endif


/// @\}
