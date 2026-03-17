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
#include "scy/packetsignal.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"
#include "scy/av/format.h"
#include "scy/av/fpscounter.h"
#include "scy/av/packet.h"
#include "scy/av/videoconverter.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}


namespace scy {
namespace av {


/// Base video context from which all video encoders and decoders derive.
struct VideoContext
{
    VideoContext();
    virtual ~VideoContext() noexcept;

    VideoContext(const VideoContext&) = delete;
    VideoContext& operator=(const VideoContext&) = delete;
    VideoContext(VideoContext&&) = delete;
    VideoContext& operator=(VideoContext&&) = delete;

    /// Create the `AVCodecContext` using default values
    virtual void create();

    /// Open the `AVCodecContext`
    virtual void open();

    /// Close the `AVCodecContext`
    virtual void close();

    /// Convert the video frame and return the result.
    ///
    /// The input frame will only be converted if it doesn't match the output
    /// format. If the frame is not converted the input frame will be returned.
    /// If the input frame format does not match the `VideoConverter` context
    /// then the `VideoConverter` will be recreated with the input frame params.
    virtual AVFrame* convert(AVFrame* iframe);

    virtual bool recreateConverter();

    PacketSignal emitter;

    VideoCodec iparams;                   ///< input parameters
    VideoCodec oparams;                   ///< output parameters
    AVStream* stream;                     ///< encoder or decoder stream
    AVCodecContext* ctx;                  ///< encoder or decoder context
    const AVCodec* codec;                 ///< encoder or decoder codec
    AVFrame* frame;                       ///< encoder or decoder frame
    std::unique_ptr<VideoConverter> conv; ///< video conversion context
    int64_t time;                         ///< stream time in codec time base
    int64_t pts;                          ///< last packet pts value
    double seconds;                       ///< video time in seconds
    std::string error;                    ///< error message
};


//
// Inline helpers
//


AVFrame* createVideoFrame(AVPixelFormat pixelFmt, int width, int height);
AVFrame* cloneVideoFrame(AVFrame* source);
void initVideoCodecFromContext(const AVStream* stream, const AVCodecContext* ctx, VideoCodec& params);
AVPixelFormat selectPixelFormat(const AVCodec* codec, VideoCodec& params);


} // namespace av
} // namespace scy


#endif


/// @\}
