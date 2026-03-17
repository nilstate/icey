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

#include "scy/av/ffmpeg.h"
#include "scy/av/format.h"
#include "scy/av/packet.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}


namespace scy {
namespace av {


// ---------------------------------------------------------------------
//
struct AudioResampler
{
    AudioResampler(const AudioCodec& iparams = AudioCodec(),
                   const AudioCodec& oparams = AudioCodec());
    ~AudioResampler() noexcept;

    AudioResampler(const AudioResampler&) = delete;
    AudioResampler& operator=(const AudioResampler&) = delete;
    AudioResampler(AudioResampler&&) = delete;
    AudioResampler& operator=(AudioResampler&&) = delete;

    void open();
    void close();

    /// Convert the input samples to the output format.
    /// NOTE: Input buffers must be contiguous, therefore only interleaved
    /// input formats are accepted at this point.
    ///
    /// Converted samples are accessible via the `outSamples` class member.
    ///
    /// @param inSamples     Pointer to the input sample buffer array.
    /// @param inNumSamples  The number of input samples per channel.
    /// @return The number of converted samples, or zero if samples were internally buffered.
    int resample(uint8_t** inSamples, int inNumSamples);

    SwrContext* ctx; ///< the conversion context

    AudioCodec iparams;               ///< input audio parameters
    AudioCodec oparams;               ///< output audio parameters
    uint8_t** outSamples;             ///< the output samples buffer
    int outNumSamples;                ///< the number of samples currently in the output buffer
    int outBufferSize;                ///< the number of bytes currently in the buffer
    int maxNumSamples;                ///< the maximum number of samples that can be stored in
    enum AVSampleFormat inSampleFmt;  ///< input sample format
    enum AVSampleFormat outSampleFmt; ///< output sample format
};


} // namespace av
} // namespace scy


#endif


/// @\}
