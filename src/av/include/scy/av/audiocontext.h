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

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


namespace scy {
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
} // namespace scy


#endif


/// @\}
