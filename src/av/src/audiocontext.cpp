///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audiocontext.h"
#include "icy/av/audioresampler.h"


#ifdef HAVE_FFMPEG

#include "icy/av/ffmpeg.h"
#include "icy/logger.h"




namespace icy {
namespace av {


AudioContext::AudioContext()
    : stream(nullptr)
    , codec(nullptr)
    , frame(nullptr)
    , outputFrameSize(0)
    , time(0)
    , pts(AV_NOPTS_VALUE)
    , seconds(0)
{
    initializeFFmpeg();
}


AudioContext::~AudioContext() noexcept
{
    close();
    uninitializeFFmpeg();
}


void AudioContext::open()
{
    SDebug << "Open: "
           << "\n\tInput: " << iparams.toString()
           << "\n\tOutput: " << oparams.toString();

    if (!ctx || !avcodec_is_open(ctx) || !codec)
        throw std::runtime_error("Audio codec not open");

    // Create the resampler if resampling is required
    if (iparams.channels != oparams.channels ||
        iparams.sampleRate != oparams.sampleRate ||
        iparams.sampleFmt != oparams.sampleFmt) {
        recreateResampler();
    }
}


void AudioContext::close()
{
    if (frame) {
        av_frame_free(&frame);
        frame = nullptr;
    }
    if (ctx) {
        avcodec_free_context(&ctx);
        ctx = nullptr;
    }
    if (stream) {
        // The stream pointer is managed by the AVFormatContext
        stream = nullptr;
    }
    resampler.reset();

    time = 0;
    pts = AV_NOPTS_VALUE;
    seconds = 0;
}


bool AudioContext::decode(AVPacket& /*ipacket*/)
{
    return false;
}


bool AudioContext::encode(uint8_t* /*samples*/, int /*numSamples*/, int64_t /*pts*/)
{
    return false;
}


bool AudioContext::encode(uint8_t* /*samples*/[4], int /*numSamples*/, int64_t /*pts*/)
{
    return false;
}


bool AudioContext::encode(AVFrame* /*iframe*/)
{
    return false;
}


void AudioContext::flush()
{
}


bool AudioContext::recreateResampler()
{
    // Recreate the resampler context
    LDebug("Recreating audio resampler context");
    resampler = std::make_unique<AudioResampler>();
    resampler->iparams = iparams;
    resampler->oparams = oparams;
    resampler->open();
    return true;
}


//
// Helper functions
//


void initAudioCodecFromContext(const AVCodecContext* ctx, AudioCodec& params)
{
    params.enabled = true;
    params.encoder = avcodec_get_name(ctx->codec_id);
    params.sampleFmt = av_get_sample_fmt_name(ctx->sample_fmt);
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100)
    params.channels = ctx->ch_layout.nb_channels;
#else
    params.channels = ctx->channels;
#endif
    params.sampleRate = ctx->sample_rate;
    params.bitRate = ctx->bit_rate;
}


bool isSampleFormatSupported(const AVCodec* codec, enum AVSampleFormat sampleFormat)
{
    const enum AVSampleFormat* p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sampleFormat)
            return true;
        p++;
    }
    return false;
}


bool formatIsPlanar(AVSampleFormat format)
{
    return av_sample_fmt_is_planar((AVSampleFormat)format) == 1;
}


bool formatIsPlanar(const std::string& pixfmt)
{
    enum AVSampleFormat format = av_get_sample_fmt(pixfmt.c_str());
    return av_sample_fmt_is_planar(format) == 1;
}


AVSampleFormat selectSampleFormat(const AVCodec* codec, av::AudioCodec& params)
{
    enum AVSampleFormat compatible = AV_SAMPLE_FMT_NONE;
    enum AVSampleFormat requested = av_get_sample_fmt(params.sampleFmt.c_str());
    int planar = av_sample_fmt_is_planar(requested);
    const enum AVSampleFormat* p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        if (compatible == AV_SAMPLE_FMT_NONE &&
            av_sample_fmt_is_planar(*p) == planar)
            compatible = *p; // or use the first compatible format
        if (*p == requested)
            return requested; // always try to return requested format
        p++;
    }
    return compatible;
}


} // namespace av
} // namespace icy


#endif


/// @}
