///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audiodecoder.h"
#include "icy/av/audioresampler.h"

#ifdef HAVE_FFMPEG

#include "icy/av/ffmpeg.h"
#include "icy/logger.h"




namespace icy {
namespace av {


AudioDecoder::AudioDecoder(AVStream* stream)
{
    this->stream = stream;
}


AudioDecoder::~AudioDecoder() noexcept
{
    close();
}


void AudioDecoder::create()
{
    if (!stream)
        throw std::runtime_error("AudioDecoder: stream is null");
    LTrace("Create: ", stream->index);

    codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec)
        throw std::runtime_error("Cannot find audio decoder.");

    ctx = avcodec_alloc_context3(codec);
    if (!ctx)
        throw std::runtime_error("Cannot allocate audio decoder context.");

    int ret = avcodec_parameters_to_context(ctx, stream->codecpar);
    if (ret < 0)
        throw std::runtime_error("Cannot copy audio codec parameters: " + averror(ret));

    frame = av_frame_alloc();
    if (!frame)
        throw std::runtime_error("Cannot allocate input frame.");

    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0)
        throw std::runtime_error("Cannot open the audio codec: " + averror(ret));

    // Set the default input and output parameters are set here once the codec
    // context has been opened. The output sample format, channels or sample
    // rate can be modified after this call and a conversion context will be
    // created on the next call to open() to output the desired format.
    initAudioCodecFromContext(ctx, iparams);
    initAudioCodecFromContext(ctx, oparams);
}


void AudioDecoder::close()
{
    AudioContext::close();
}


bool emitPacket(AudioDecoder* dec)
{
    // Set the decoder time in microseconds
    int64_t pts = dec->frame->pts;
    dec->time = pts > 0 ? static_cast<int64_t>(pts *
                                               av_q2d(dec->stream->time_base) * AV_TIME_BASE)
                        : 0;

    // Set the decoder pts in stream time base
    dec->pts = pts;

    // Set the decoder seconds since stream start
    dec->seconds = (pts - dec->stream->start_time) * av_q2d(dec->stream->time_base);

    if (dec->resampler) {
        if (!dec->resampler->resample((uint8_t**)dec->frame->extended_data, dec->frame->nb_samples)) {
            LDebug("Samples buffered by resampler");
            return false;
        }

        PlanarAudioPacket audio(dec->resampler->outSamples,
                                dec->oparams.channels, dec->resampler->outNumSamples,
                                dec->oparams.sampleFmt, dec->time);
        dec->outputFrameSize = dec->resampler->outNumSamples;
        dec->emitter.emit(audio);
    } else {
        PlanarAudioPacket audio(dec->frame->data,
                                dec->oparams.channels, dec->frame->nb_samples,
                                dec->oparams.sampleFmt, dec->time);
        dec->outputFrameSize = dec->frame->nb_samples;
        dec->emitter.emit(audio);
    }

    return true;
}


bool AudioDecoder::decode(AVPacket& ipacket)
{
    if (!ctx || !codec || !frame)
        throw std::runtime_error("AudioDecoder not initialized");
    if (stream && ipacket.stream_index != stream->index)
        return false;

    // Send packet to the decoder
    int ret = avcodec_send_packet(ctx, &ipacket);
    if (ret < 0) {
        error = "Audio decoder error: " + averror(ret);
        LError(error);
        throw std::runtime_error(error);
    }

    bool decoded = false;
    while (ret >= 0) {
        ret = avcodec_receive_frame(ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }
        if (ret < 0) {
            error = "Audio decoder error: " + averror(ret);
            LError(error);
            throw std::runtime_error(error);
        }

        // fps.tick();
        emitPacket(this);
        decoded = true;
    }

    return decoded;
}


void AudioDecoder::flush()
{
    // Send nullptr packet to flush the decoder
    avcodec_send_packet(ctx, nullptr);

    int ret = 0;
    while (ret >= 0) {
        ret = avcodec_receive_frame(ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }
        if (ret >= 0) {
            LTrace("Flushed audio frame: ", frame->pts);
            emitPacket(this);
        }
    }
}


} // namespace av
} // namespace icy


#endif


/// @}
