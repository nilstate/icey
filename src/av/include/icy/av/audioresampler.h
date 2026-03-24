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

#include "icy/av/ffmpeg.h"
#include "icy/av/format.h"
#include "icy/av/packet.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}


namespace icy {
namespace av {


// ---------------------------------------------------------------------
//
/// Converts audio samples between different formats, sample rates, and channel layouts
struct AudioResampler
{
    AudioResampler(const AudioCodec& iparams = AudioCodec(),
                   const AudioCodec& oparams = AudioCodec());
    ~AudioResampler() noexcept;

    AudioResampler(const AudioResampler&) = delete;
    AudioResampler& operator=(const AudioResampler&) = delete;
    AudioResampler(AudioResampler&&) = delete;
    AudioResampler& operator=(AudioResampler&&) = delete;

    /// Initialise the libswresample context using iparams and oparams.
    /// Throws std::runtime_error if the context is already open or if
    /// required parameters (channels, sample rate, format) are missing.
    void open();

    /// Free the libswresample context and release the output sample buffer.
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
} // namespace icy


#endif


/// @}
