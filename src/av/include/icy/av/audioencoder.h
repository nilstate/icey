///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/base.h"

#ifdef HAVE_FFMPEG

#include "icy/av/audiobuffer.h"
#include "icy/av/audiocontext.h"
#include "icy/av/ffmpeg.h"
#include "icy/av/format.h"
#include "icy/av/packet.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
}


namespace icy {
namespace av {


/// Encodes raw audio samples into a compressed format
struct AudioEncoder : public AudioContext
{
    /// Construct an encoder, optionally tied to an existing muxer context.
    /// @param format  The AVFormatContext that will receive encoded packets, or nullptr for standalone use.
    AudioEncoder(AVFormatContext* format = nullptr);
    ~AudioEncoder() noexcept override;

    /// Initialise the AVCodecContext using oparams.
    /// Adds an audio stream to @p format if one was provided at construction.
    virtual void create() override;

    /// Close and free the AVCodecContext, FIFO buffer, and associated resources.
    virtual void close() override;

    /// Encode interleaved audio samples.
    ///
    /// @param samples    The input samples to encode.
    /// @param numSamples The number of input samples per channel.
    /// @param pts        The input samples presentation timestamp.
    [[nodiscard]] bool encode(uint8_t* samples, const int numSamples, const int64_t pts) override;

    /// Encode planar audio samples.
    ///
    /// @param samples    The input samples to encode.
    /// @param numSamples The number of input samples per channel.
    /// @param pts        The input samples presentation timestamp.
    [[nodiscard]] bool encode(uint8_t* samples[4], const int numSamples, const int64_t pts) override;

    /// Encode a single AVFrame (typically from a decoder or resampler).
    /// @param iframe  The source audio frame with all fields set.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] bool encode(AVFrame* iframe) override;

    /// Flush remaining packets to be encoded.
    /// This method should be called once before stream closure.
    void flush() override;

    av::AudioBuffer fifo;
    AVFormatContext* format;

    /// Running PTS counter for encoder output, in encoder time_base units.
    /// Anchored to the first input packet's PTS, then advanced by
    /// frame->nb_samples per encoded frame. AV_NOPTS_VALUE before anchoring.
    int64_t nextOutputPts = AV_NOPTS_VALUE;
};


} // namespace av
} // namespace icy


#endif


/// @}
