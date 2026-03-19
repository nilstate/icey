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
#include "icy/packetsignal.h"

#ifdef HAVE_FFMPEG

#include "icy/av/ffmpeg.h"
#include "icy/av/format.h"
#include "icy/av/fpscounter.h"
#include "icy/av/packet.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


namespace icy {
namespace av {


struct AudioResampler;


struct AudioContext
{
    AudioContext();
    virtual ~AudioContext() noexcept;

    AudioContext(const AudioContext&) = delete;
    AudioContext& operator=(const AudioContext&) = delete;
    AudioContext(AudioContext&&) = delete;
    AudioContext& operator=(AudioContext&&) = delete;

    /// Initialize the `AVCodecContext` with default values
    virtual void create() = 0;

    /// Open the AVCodecContext
    virtual void open();

    /// Close the AVCodecContext
    virtual void close();

    /// Decode the given input packet.
    /// Returns true if an output packet was created, false otherwise.
    [[nodiscard]] virtual bool decode(AVPacket& ipacket);

    /// Encode interleaved audio samples.
    [[nodiscard]] virtual bool encode(uint8_t* samples, int numSamples, int64_t pts);

    /// Encode planar audio samples.
    [[nodiscard]] virtual bool encode(uint8_t* samples[4], int numSamples, int64_t pts);

    /// Encode a single AVFrame.
    [[nodiscard]] virtual bool encode(AVFrame* iframe);

    /// Flush buffered frames.
    virtual void flush();

    /// Create the audio resampling context.
    virtual bool recreateResampler();

    PacketSignal emitter;

    AudioCodec iparams;                        ///< input parameters
    AudioCodec oparams;                        ///< output parameters
    AVStream* stream;                          ///< encoder or decoder stream
    AVCodecContext* ctx;                       ///< encoder or decoder context
    const AVCodec* codec;                      ///< encoder or decoder codec
    AVFrame* frame;                            ///< last encoded or decoded frame
    std::unique_ptr<AudioResampler> resampler; ///< audio resampler
    int outputFrameSize;                       ///< encoder or decoder output frame size
    int64_t time;                              ///< stream time in codec time base
    int64_t pts;                               ///< last packet pts value
    double seconds;                            ///< audio time in seconds
    std::string error;                         ///< error message
};


void initAudioCodecFromContext(const AVCodecContext* ctx, AudioCodec& params);
AVSampleFormat selectSampleFormat(const AVCodec* codec, av::AudioCodec& params);
bool isSampleFormatSupported(const AVCodec* codec, enum AVSampleFormat sampleFormat);
bool formatIsPlanar(const std::string& pixfmt);
bool formatIsPlanar(AVSampleFormat format);


} // namespace av
} // namespace icy


#endif


/// @\}
