///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/videocapture.h"
#include "scy/util.h"

#ifdef HAVE_FFMPEG

#include "scy/av/devicemanager.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


using std::endl;


namespace scy {
namespace av {


VideoCapture::VideoCapture()
{
}


VideoCapture::VideoCapture(const std::string& device, int width, int height,
                           double framerate, const std::string& pixelFmt)
{
    openVideo(device, width, height, framerate);
}


VideoCapture::VideoCapture(const std::string& device, const av::VideoCodec& params)
{
    openVideo(device, params.width, params.height, params.fps, params.pixelFmt);
}



VideoCapture::~VideoCapture()
{
}


void VideoCapture::openVideo(const std::string& device, const av::VideoCodec& params)
{
    openVideo(device, params.width, params.height, params.fps, params.pixelFmt);
}


void VideoCapture::openVideo(const std::string& device, int width, int height,
                             double framerate, const std::string& pixelFmt)
{
    SDebug << "Opening camera: " << device << ", "
           << "width=" << width << ", "
           << "height=" << height << ", "
           << "framerate=" << framerate  << ", "
           << "pixelFmt=" << pixelFmt << endl;

    DeviceManager devman;
    auto iformat = devman.findVideoInputFormat();
    if (!iformat)
        throw std::runtime_error("Couldn't find camera input format.");

    AVDictionary* iparams = nullptr;
    AVDictionaryCleanup cleanup{ &iparams };

    // NOTE: A pixel format must be passed or the capture may error out with:
    // [mjpeg @ 0x7fb5f40016c0] Specified pixel format -1 is invalid or not supported
    // TODO: Use yuv420p default on linux, is this OK for Windows too?
    std::string pixfmt = pixelFmt.empty() ? "yuv420p" : pixelFmt;

    // Set custom parameters for devices.
    // NOTE: This doesn't work for DirectShow.
#ifndef SCY_WIN
    if (width > 0 && height > 0)
        av_dict_set(&iparams, "video_size", util::format("%dx%d", width, height).c_str(), 0);
    if (framerate > 0)
        av_dict_set(&iparams, "framerate", util::format("%f", framerate).c_str(), 0);

    // Set the desired pixel format
    av_dict_set(&iparams, "pixel_format", pixfmt.data(), 0); //bgr24, yuv420p
#endif

    // Use realtime PTS calculation for live sources
    setRealtimePTS(true);

    // Open the stream
    openStream(device.c_str(), iformat, &iparams);

    // Set the decoder video output parameters for conversion context.
    // NOTE: Must be done after `openStream`.
    // If the input device wouldn't accept our parameters then we will
    // perform pixel conversions and resizing ourself (on the decoder).
    if (_video) {
        if (!pixfmt.empty())
            _video->oparams.pixelFmt = pixfmt; // bgr24, yuv420p
        if (width > 0)
            _video->oparams.width = width;
        if (height > 0)
            _video->oparams.height = height;
        if (framerate > 0)
            _video->oparams.fps = framerate;
    }
}


} // namespace av
} // namespace scy


#endif


/// @\}
