///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/videocontext.h"

#ifdef HAVE_FFMPEG

#include "icy/logger.h"




namespace icy {
namespace av {


VideoContext::VideoContext()
    : stream(nullptr)
    , ctx(nullptr)
    , codec(nullptr)
    , frame(nullptr)
    , time(0)
    , pts(AV_NOPTS_VALUE)
    , seconds(0)
{
    LTrace("Create");
    initializeFFmpeg();
}


VideoContext::~VideoContext() noexcept
{
    LTrace("Destroy");

    close();
    uninitializeFFmpeg();
}


void VideoContext::create()
{
}


void VideoContext::open()
{
    SDebug << "Open: "
           << "\n\tInput: " << iparams.toString()
           << "\n\tOutput: " << oparams.toString();

    if (!ctx || !avcodec_is_open(ctx) || !codec)
        throw std::runtime_error("Video codec not open");

    // Create the video conversion context if required
    recreateConverter();
}


void VideoContext::close()
{
    LTrace("Closing");

    if (frame) {
        av_frame_free(&frame);
        frame = nullptr;
    }

    if (ctx) {
        avcodec_free_context(&ctx);
        ctx = nullptr;
    }

    conv.reset();

    // Streams are managed by the AVFormatContext
    stream = nullptr;

    time = 0;
    pts = AV_NOPTS_VALUE;
    seconds = 0;
    error = "";

    LTrace("Closing: OK");
}


bool VideoContext::decode(AVPacket& /*ipacket*/)
{
    return false;
}


bool VideoContext::encode(uint8_t* /*data*/, int /*size*/, int64_t /*pts*/)
{
    return false;
}


bool VideoContext::encode(uint8_t* /*data*/[4], int /*linesize*/[4], int64_t /*pts*/)
{
    return false;
}


bool VideoContext::encode(AVFrame* /*iframe*/)
{
    return false;
}


void VideoContext::flush()
{
}


AVFrame* VideoContext::convert(AVFrame* iframe)
{
    // While flushing the input frame may be null
    if (!iframe)
        return nullptr;

    if (iframe->width != iparams.width || iframe->height != iparams.height)
        throw std::runtime_error("Input frame dimensions mismatch");

    // Recreate the video conversion context on the fly
    // if the input resolution changes.
    iparams.width = iframe->width;
    iparams.height = iframe->height;
    iparams.pixelFmt = av_get_pix_fmt_name((AVPixelFormat)iframe->format);
    recreateConverter();

    // Return the input frame if no conversion is required
    if (!conv)
        return iframe;

    // Convert the input frame and return the result
    return conv->convert(iframe);
}


bool VideoContext::recreateConverter()
{
    // Check if conversion is required
    // This check is only for when uninitialized
    if (!conv &&
        iparams.width == oparams.width &&
        iparams.height == oparams.height &&
        iparams.pixelFmt == oparams.pixelFmt) {
        return false;
    }

    // Check if the conversion context needs to be recreated
    if (conv && (conv->iparams.width == iparams.width && conv->iparams.height == iparams.height && conv->iparams.pixelFmt == iparams.pixelFmt) &&
        (conv->oparams.width == oparams.width &&
         conv->oparams.height == oparams.height &&
         conv->oparams.pixelFmt == oparams.pixelFmt)) {
        return false;
    }

    // Recreate the conversion context
    LDebug("Recreating video conversion context");
    conv = std::make_unique<VideoConverter>();
    conv->iparams = iparams;
    conv->oparams = oparams;
    conv->create();
    return true;
}


//
// Helper functions
//


AVFrame* createVideoFrame(AVPixelFormat pixelFmt, int width, int height)
{
    auto picture = av_frame_alloc();
    if (!picture)
        return nullptr;

    picture->width = width;
    picture->height = height;
    picture->format = pixelFmt;

    // Allocate frame buffer via av_frame_get_buffer for proper refcounting
    if (av_frame_get_buffer(picture, 16) < 0) {
        av_frame_free(&picture);
        return nullptr;
    }

    return picture;
}


AVFrame* cloneVideoFrame(AVFrame* source)
{
    AVFrame* copy = av_frame_alloc();
    copy->format = source->format;
    copy->width = source->width;
    copy->height = source->height;
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100)
    av_channel_layout_copy(&copy->ch_layout, &source->ch_layout);
#else
    copy->channels = source->channels;
    copy->channel_layout = source->channel_layout;
#endif
    copy->nb_samples = source->nb_samples;
    av_frame_get_buffer(copy, 32);
    av_frame_copy(copy, source);
    av_frame_copy_props(copy, source);
    return copy;
}


void initVideoCodecFromContext(const AVStream* stream, const AVCodecContext* ctx, VideoCodec& params)
{
    params.enabled = true;
    params.encoder = avcodec_get_name(ctx->codec_id);
    params.pixelFmt = av_get_pix_fmt_name(ctx->pix_fmt);
    params.width = ctx->width;
    params.height = ctx->height;
    params.sampleRate = ctx->sample_rate;
    params.bitRate = ctx->bit_rate;
    if (stream && stream->r_frame_rate.num) {
        params.fps = stream->r_frame_rate.num / stream->r_frame_rate.den;
    }
}

AVPixelFormat selectPixelFormat(const AVCodec* codec, VideoCodec& params)
{
    enum AVPixelFormat compatible = AV_PIX_FMT_NONE;
    enum AVPixelFormat requested = av_get_pix_fmt(params.pixelFmt.c_str());
    int nplanes = av_pix_fmt_count_planes(requested);
    const enum AVPixelFormat* p = codec->pix_fmts;
    while (*p != AV_PIX_FMT_NONE) {
        if (compatible == AV_PIX_FMT_NONE && (nplanes == 0 || av_pix_fmt_count_planes(*p) == nplanes))
            compatible = *p; // or use the first compatible format
        if (*p == requested)
            return requested; // always try to return requested format
        p++;
    }
    if (compatible == AV_PIX_FMT_NONE)
        compatible = p[0];
    return compatible;
}


} // namespace av
} // namespace icy


#endif


/// @}
