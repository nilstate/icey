///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/videoencoder.h"

#ifdef HAVE_FFMPEG

#include "scy/logger.h"




namespace scy {
namespace av {


VideoConverter::VideoConverter()
    : ctx(nullptr)
    , oframe(nullptr)
{
}


VideoConverter::~VideoConverter() noexcept
{
    close();
}


void VideoConverter::create()
{
    //#if 0
    STrace << "Create:"
           << "\n\tInput Width: " << iparams.width
           << "\n\tInput Height: " << iparams.height
           << "\n\tInput Pixel Format: " << iparams.pixelFmt
           << "\n\tOutput Width: " << oparams.width
           << "\n\tOutput Height: " << oparams.height
           << "\n\tOutput Pixel Format: " << oparams.pixelFmt;
    //#endif

    if (ctx)
        throw std::runtime_error("Conversion context already initialized.");

    oframe = createVideoFrame(av_get_pix_fmt(oparams.pixelFmt.c_str()),
                              oparams.width, oparams.height);

    ctx = sws_getContext(
        iparams.width, iparams.height, av_get_pix_fmt(iparams.pixelFmt.c_str()),
        oparams.width, oparams.height, av_get_pix_fmt(oparams.pixelFmt.c_str()),
        /* SWS_FAST_BILINEAR */ SWS_BICUBIC, nullptr, nullptr, nullptr);
    if (!ctx)
        throw std::runtime_error("Invalid conversion context.");
}


void VideoConverter::close()
{
    LTrace("Closing");

    if (oframe) {
        av_frame_free(&oframe);
        oframe = nullptr;
    }

    if (ctx) {
        sws_freeContext(ctx);
        ctx = nullptr;
    }
}


AVFrame* VideoConverter::convert(AVFrame* iframe)
{
    STrace << "Convert:"
           << "\n\tIn Format: " << iparams.pixelFmt
           << "\n\tIn Size: " << iframe->width << "x" << iframe->height
           << "\n\tOut Format: " << oparams.pixelFmt
           << "\n\tOut Size: " << oparams.width << "x" << oparams.height
           << "\n\tPTS: " << iframe->pts
;

    if (!iframe || !iframe->data[0])
        throw std::runtime_error("Invalid input frame for conversion");
    if (iframe->width != iparams.width || iframe->height != iparams.height)
        throw std::runtime_error("Input frame dimensions do not match converter input parameters");
    if (oframe->format != av_get_pix_fmt(oparams.pixelFmt.c_str()))
        throw std::runtime_error("Output frame pixel format mismatch");

    if (!ctx)
        throw std::runtime_error("Conversion context must be initialized.");

    if (sws_scale(ctx, iframe->data, iframe->linesize, 0, iparams.height,
                  oframe->data, oframe->linesize) < 0)
        throw std::runtime_error("Pixel format conversion not supported.");

    // Copy input frame properties to output frame
    av_frame_copy_props(oframe, iframe);

    // Set the input PTS or a monotonic value to keep the encoder happy.
    // The actual setting of the PTS is outside the scope of this encoder.
    // oframe->pts = iframe->pts != AV_NOPTS_VALUE ? iframe->pts : ctx->frame_number;

    return oframe;
}


} // namespace av
} // namespace scy


#endif


/// @\}
