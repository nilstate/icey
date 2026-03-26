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


/// Base context for audio encoding and decoding via FFmpeg
struct AudioContext
{
    AudioContext();
    virtual ~AudioContext() noexcept;

    AudioContext(const AudioContext&) = delete;
    AudioContext& operator=(const AudioContext&) = delete;
    AudioContext(AudioContext&&) = delete;
    AudioContext& operator=(AudioContext&&) = delete;

    /// Initialise the AVCodecContext with codec-specific defaults.
    /// Implemented by AudioEncoder and AudioDecoder.
    virtual void create() = 0;

    /// Open the codec and create the resampler if input/output parameters differ.
    /// Throws std::runtime_error if the codec context has not been created.
    virtual void open();

    /// Close the codec context, free the frame, and reset timestamps.
    virtual void close();

    /// Decode a compressed audio packet and emit the resulting samples.
    /// @param ipacket  The compressed audio packet to decode.
    /// @return True if one or more decoded frames were emitted, false otherwise.
    [[nodiscard]] virtual bool decode(AVPacket& ipacket);

    /// Encode a buffer of interleaved audio samples.
    /// @param samples     Pointer to the interleaved sample buffer.
    /// @param numSamples  Number of samples per channel.
    /// @param pts         Presentation timestamp in stream time base units.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] virtual bool encode(uint8_t* samples, int numSamples, int64_t pts);

    /// Encode a buffer of planar audio samples.
    /// @param samples     Array of per-channel sample buffers (up to 4 planes).
    /// @param numSamples  Number of samples per channel.
    /// @param pts         Presentation timestamp in stream time base units.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] virtual bool encode(uint8_t* samples[4], int numSamples, int64_t pts);

    /// Encode a single AVFrame.
    /// @param iframe  The source audio frame; must have all fields set correctly.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] virtual bool encode(AVFrame* iframe);

    /// Flush any frames buffered inside the codec and emit remaining output.
    virtual void flush();

    /// Recreate the AudioResampler using the current iparams and oparams.
    /// Called automatically by open() when format conversion is required.
    /// @return True if the resampler was successfully created.
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


/// Populate an AudioCodec from an open AVCodecContext.
/// @param ctx     The FFmpeg codec context to read from.
/// @param params  The AudioCodec struct to fill with codec, channel, sample rate, and format info.
void initAudioCodecFromContext(const AVCodecContext* ctx, AudioCodec& params);

/// Select the best supported sample format for a codec given the requested parameters.
/// Returns the requested format if supported, otherwise the first format with the same planarity.
/// @param codec   The FFmpeg codec whose supported formats are queried.
/// @param params  The audio codec parameters specifying the desired sample format.
/// @return The chosen AVSampleFormat, or AV_SAMPLE_FMT_NONE if none is compatible.
AVSampleFormat selectSampleFormat(const AVCodec* codec, av::AudioCodec& params);

/// Check whether a specific sample format is in the codec's supported list.
/// @param codec         The FFmpeg codec to query.
/// @param sampleFormat  The AVSampleFormat to look up.
/// @return True if the format is supported.
bool isSampleFormatSupported(const AVCodec* codec, enum AVSampleFormat sampleFormat);

/// Return true if the named sample format is planar (e.g. "fltp", "s16p").
/// @param pixfmt  The sample format name string.
bool formatIsPlanar(const std::string& pixfmt);

/// Return true if the given AVSampleFormat is planar.
/// @param format  The AVSampleFormat to check.
bool formatIsPlanar(AVSampleFormat format);


} // namespace av
} // namespace icy


#endif


/// @}
