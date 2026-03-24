///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audioresampler.h"

#ifdef HAVE_FFMPEG

#include "icy/logger.h"
#include "icy/util.h"




namespace icy {
namespace av {


AudioResampler::AudioResampler(const AudioCodec& iparams,
                               const AudioCodec& oparams)
    : ctx(nullptr)
    , iparams(iparams)
    , oparams(oparams)
    , outSamples(nullptr)
    , outNumSamples(0)
    , outBufferSize(0)
    , maxNumSamples(0)
    , inSampleFmt(AV_SAMPLE_FMT_NONE)
    , outSampleFmt(AV_SAMPLE_FMT_NONE)
{
}


AudioResampler::~AudioResampler() noexcept
{
    close();
}


void AudioResampler::open()
{
    if (ctx)
        throw std::runtime_error("Resample context already initialized");

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100)
    AVChannelLayout inChLayout = {};
    AVChannelLayout outChLayout = {};
    av_channel_layout_default(&inChLayout, iparams.channels);
    av_channel_layout_default(&outChLayout, oparams.channels);
#else
    int64_t inChLayout = av_get_default_channel_layout(iparams.channels);
    int64_t outChLayout = av_get_default_channel_layout(oparams.channels);
#endif

    inSampleFmt = av_get_sample_fmt(iparams.sampleFmt.c_str());
    outSampleFmt = av_get_sample_fmt(oparams.sampleFmt.c_str());

    STrace << "Create audio resampler:\n"
           << "\n\tIn Nb Channels: " << iparams.channels
           << "\n\tIn Sample Rate: " << iparams.sampleRate
           << "\n\tIn Sample Fmt: " << iparams.sampleFmt
           << "\n\tOut Nb Channels: " << oparams.channels
           << "\n\tOut Sample Rate: " << oparams.sampleRate
           << "\n\tOut Sample Fmt: " << oparams.sampleFmt;

    if (!iparams.channels || !oparams.channels)
        throw std::runtime_error("Input and output channel count must be set");
    if (!iparams.sampleRate || !oparams.sampleRate)
        throw std::runtime_error("Input and output sample rate must be set");
    if (iparams.sampleFmt.empty() || oparams.sampleFmt.empty())
        throw std::runtime_error("Input and output sample format must be set");
    if (inSampleFmt == AV_SAMPLE_FMT_NONE || outSampleFmt == AV_SAMPLE_FMT_NONE)
        throw std::runtime_error("Invalid sample format");

    int ret;

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100)
    // Use the new channel layout API (FFmpeg 5.1+)
    ret = swr_alloc_set_opts2(&ctx,
                              &outChLayout, outSampleFmt, oparams.sampleRate,
                              &inChLayout, inSampleFmt, iparams.sampleRate,
                              0, nullptr);
    if (ret < 0 || !ctx) {
        throw std::runtime_error("Cannot allocate resample context: " + averror(ret));
    }
    ret = swr_init(ctx);
#else
    ctx = swr_alloc();
    if (!ctx) {
        throw std::runtime_error("Cannot allocate resample context");
    }

    av_opt_set_int(ctx, "in_channel_layout", inChLayout, 0);
    av_opt_set_int(ctx, "out_channel_layout", outChLayout, 0);
    av_opt_set_int(ctx, "in_sample_rate", iparams.sampleRate, 0);
    av_opt_set_int(ctx, "out_sample_rate", oparams.sampleRate, 0);
    av_opt_set_sample_fmt(ctx, "in_sample_fmt", inSampleFmt, 0);
    av_opt_set_sample_fmt(ctx, "out_sample_fmt", outSampleFmt, 0);

    ret = swr_init(ctx);
#endif
    if (ret < 0) {
        close();
        throw std::runtime_error("Cannot initialize resample context: " + averror(ret));
    }

    LTrace("Create: OK");
}


void AudioResampler::close()
{
    LTrace("Closing");

    if (ctx) {
        swr_free(&ctx);
        ctx = nullptr;
    }

    if (outSamples) {
        av_freep(&(outSamples)[0]);
        //av_free(outSamples);
        outSamples = nullptr;
    }

    outNumSamples = 0;
    maxNumSamples = 0;
    outBufferSize = 0;
}


int AudioResampler::resample(uint8_t** inSamples, int inNumSamples)
{
    if (!ctx)
        throw std::runtime_error("Conversion context must be initialized.");

    int ret, requiredNumSamples;

    // Compute the output number of samples
    // https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/resampling_audio.c
    requiredNumSamples = av_rescale_rnd(
        swr_get_delay(ctx, static_cast<int64_t>(iparams.sampleRate)) +
            static_cast<int64_t>(inNumSamples),
        static_cast<int64_t>(oparams.sampleRate),
        static_cast<int64_t>(iparams.sampleRate), AV_ROUND_UP);

    // Resize the output buffer if required
    if (requiredNumSamples > maxNumSamples) {
        ret = av_samples_alloc_array_and_samples(
            &outSamples, nullptr, oparams.channels, requiredNumSamples,
            outSampleFmt, 0);
        if (ret < 0) {
            throw std::runtime_error("Cannot allocate buffer for converted output samples: " + averror(ret));
        }

        LTrace("Resizing resampler buffer: ", outBufferSize);
        maxNumSamples = requiredNumSamples;
    }

    if (!requiredNumSamples || !maxNumSamples)
        throw std::runtime_error("Invalid resampler buffer state");

    // Convert the samples using the resampler.
    ret = swr_convert(ctx, outSamples, maxNumSamples,
                      const_cast<const uint8_t**>(inSamples), inNumSamples);
    if (ret < 0) {
        close();
        throw std::runtime_error("Cannot convert input samples: " + averror(ret));
    }

    // Set the actual number of output samples
    outNumSamples = ret;

    // Set the output buffer size in bytes.
    // This may be useful for implementations that need to know the size of
    // converted output samples in bytes, such as for writing to files.
    outBufferSize = av_samples_get_buffer_size(nullptr, oparams.channels,
                                               outNumSamples, outSampleFmt, 0);

    // STrace << "Resampled audio frame:"
    //        << "\n\tIn Nb Samples: " << inNumSamples
    //        << "\n\tIn Channels: " << iparams.channels
    //        << "\n\tIn Sample Rate: " << iparams.sampleRate
    //        << "\n\tIn Sample Fmt: " << iparams.sampleFmt
    //        << "\n\tOut Nb Samples: " << outNumSamples
    //        << "\n\tOut Max Nb Samples: " << maxNumSamples
    //        << "\n\tOut Buffer Size: " << outBufferSize
    //        << "\n\tOut Channels: " << oparams.channels
    //        << "\n\tOut Sample Rate: " << oparams.sampleRate
    //        << "\n\tOut Lample Fmt: " << oparams.sampleFmt << std::endl;

    // assert(outNumSamples > 0);
    // assert(outBufferSize > 0);

    return ret;
}


} // namespace av
} // namespace icy


#endif


/// @}
