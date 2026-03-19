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


struct AudioEncoder : public AudioContext
{
    AudioEncoder(AVFormatContext* format = nullptr);
    ~AudioEncoder() noexcept override;

    virtual void create() override;
    virtual void close() override;

    /// Encode interleaved audio samples.
    ///
    /// @param samples    The input samples to encode.
    /// @param numSamples The number of input samples per channel.
    /// @param pts        The input samples presentation timestamp.
    /// @param opacket    The output packet data will be encoded to.
    [[nodiscard]] bool encode(uint8_t* samples, const int numSamples, const int64_t pts) override;

    /// Encode planar audio samples.
    ///
    /// @param samples    The input samples to encode.
    /// @param numSamples The number of input samples per channel.
    /// @param pts        The input samples presentation timestamp.
    /// @param opacket    The output packet data will be encoded to.
    [[nodiscard]] bool encode(uint8_t* samples[4], const int numSamples, const int64_t pts) override;

    /// Encode a single AVFrame from the decoder.
    [[nodiscard]] bool encode(AVFrame* iframe) override;

    /// Flush remaining packets to be encoded.
    /// This method should be called once before stream closure.
    void flush() override;

    av::AudioBuffer fifo;
    AVFormatContext* format;
};


} // namespace av
} // namespace icy


#endif


/// @\}
