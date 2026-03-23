///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/audioencoder.h"
#include "icy/av/audioresampler.h"

#ifdef HAVE_FFMPEG

#include "icy/av/ffmpeg.h"
#include "icy/logger.h"




namespace icy {
namespace av {


AudioEncoder::AudioEncoder(AVFormatContext* format)
    : format(format)
{
    LTrace("Create");
}


AudioEncoder::~AudioEncoder() noexcept
{
    LTrace("Destroy");
    close();
}


// Initialize one input frame for writing to the output file.
// The frame will be exactly frame_size samples large.
static AVFrame* initOutputFrame(AVCodecContext* ctx)
{
    AVFrame* frame;

    // Create a new frame to store the audio samples.
    if (!(frame = av_frame_alloc())) {
        throw std::runtime_error("Cannot allocate output frame");
    }

    // Set the frame's parameters, especially its size and format.
    frame->nb_samples = ctx->frame_size;
    frame->format = ctx->sample_fmt;
    frame->sample_rate = ctx->sample_rate;
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100)
    av_channel_layout_copy(&frame->ch_layout, &ctx->ch_layout);
#else
    frame->channel_layout = ctx->channel_layout;
#endif

    // Allocate the samples of the created frame.
    int error;
    if ((error = av_frame_get_buffer(frame, 0)) < 0) {
        av_frame_free(&frame);
        throw std::runtime_error("Could allocate output frame samples: " + averror(error));
    }

    return frame;
}


void AudioEncoder::create()
{
    LTrace("Create");
    int err;

    // Find the audio encoder
    if (!(codec = avcodec_find_encoder_by_name(oparams.encoder.c_str()))) {
        if (!format || !(codec = avcodec_find_encoder(format->oformat->audio_codec)))
            throw std::runtime_error("Cannot find an audio encoder for: " + oparams.encoder);
    }

    // Allocate stream from the AVFormatContext if available
    if (format) {
        // Create a new audio stream in the output file container.
        if (!(stream = avformat_new_stream(format, codec))) {
            throw std::runtime_error("Cannot create the audio stream: Out of memory");
        }
    }

    // Allocate the AVCodecContext
    ctx = avcodec_alloc_context3(codec);
    if (!ctx)
        throw std::runtime_error("Cannot allocate encoder context.");

    if (!oparams.bitRate)
        oparams.bitRate = DEFAULT_AUDIO_BIT_RATE;
    if (!oparams.sampleRate)
        oparams.sampleRate = DEFAULT_AUDIO_SAMPLE_RATE;
    if (!oparams.channels)
        oparams.channels = DEFAULT_AUDIO_CHANNELS;
    if (oparams.sampleFmt.empty())
        oparams.sampleFmt = DEFAULT_AUDIO_SAMPLE_FMT;

    // Now we'll setup the parameters of AVCodecContext
    ctx->bit_rate = oparams.bitRate;
    ctx->sample_fmt = selectSampleFormat(codec, oparams);
    ctx->sample_rate = oparams.sampleRate;
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100)
    av_channel_layout_default(&ctx->ch_layout, oparams.channels);
#else
    ctx->channels = oparams.channels;
    ctx->channel_layout = av_get_default_channel_layout(oparams.channels);
#endif

    ctx->strict_std_compliance = oparams.compliance;

    // Some container formats (like MP4) require global headers to be present
    if (format && format->oformat->flags & AVFMT_GLOBALHEADER) {
        ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    // Apply user-specified encoder options.
    for (const auto& [key, value] : oparams.options) {
        int ret = av_opt_set(ctx->priv_data, key.c_str(), value.c_str(), 0);
        if (ret < 0)
            LWarn("Ignoring unknown audio encoder option: ", key, "=", value);
    }

    // Open the encoder for the audio stream to use it later.
    if ((err = avcodec_open2(ctx, codec, nullptr)) < 0) {
        throw std::runtime_error("Cannot open the audio codec: " + averror(err));
    }

    // Copy codec parameters to the stream
    if (stream) {
        int ret = avcodec_parameters_from_context(stream->codecpar, ctx);
        if (ret < 0)
            throw std::runtime_error("Cannot copy audio codec parameters to stream: " + averror(ret));
        stream->time_base = ctx->time_base;
    }

    // Use the encoder's desired frame size for processing.
    outputFrameSize = ctx->frame_size;
    if (!outputFrameSize)
        throw std::runtime_error("Audio codec returned zero frame size");

    // Create the encode frame
    frame = initOutputFrame(ctx);

    // Create the FIFO buffer based on the specified output sample format.
    fifo.alloc(oparams.sampleFmt, oparams.channels, outputFrameSize);

    // Update parameters that may have changed
    initAudioCodecFromContext(ctx, oparams);
}


void AudioEncoder::close()
{
    LTrace("Closing");

    AudioContext::close();
}

void emitPacket(AudioEncoder* enc, AVPacket* opacket)
{
    const auto timeBase = enc->stream ? enc->stream->time_base : enc->ctx->time_base;

    enc->time = opacket->pts > 0
        ? av_rescale_q(opacket->pts, timeBase, AVRational{1, AV_TIME_BASE})
        : 0;
    enc->seconds = opacket->pts * av_q2d(timeBase);

    // Set the encoder pts in stream time base
    enc->pts = opacket->pts;

    if (!opacket->data || !opacket->size)
        return;

    AudioPacket audio(opacket->data, opacket->size, enc->outputFrameSize, enc->time);
    audio.avpacket = opacket;
    enc->emitter.emit(audio);
}


int flushBuffer(AudioEncoder* enc)
{
    // Read frames from the FIFO while available
    int num = 0;
    while (enc->fifo.read((void**)enc->frame->data, enc->frame->nb_samples) &&
           enc->encode(enc->frame)) {
        // NOTE: The encoder calculate the correct pts value for us,
        // so let's just increment the pts counter.
        if (enc->frame->pts != AV_NOPTS_VALUE) {
            enc->frame->pts++;
        }
        num++;
    }

    return num;
}


bool AudioEncoder::encode(uint8_t* samples, const int numSamples, const int64_t pts)
{
    // Resample input data or add it to the buffer directly
    if (resampler) {
        if (!resampler->resample((uint8_t**)&samples, numSamples)) {
            LTrace("Samples buffered by resampler");
            return false;
        }

        // Add the converted input samples to the FIFO buffer.
        fifo.write((void**)resampler->outSamples, resampler->outNumSamples);
    } else {
        // Add the input samples to the FIFO buffer.
        fifo.write((void**)&samples, numSamples);
    }

    // Set a timestamp value on the frame to be encoded if given.
    if (pts != AV_NOPTS_VALUE) {
        frame->pts = pts;
    }

    return flushBuffer(this) > 0;
}


bool AudioEncoder::encode(uint8_t* samples[4], const int numSamples, const int64_t pts)
{
    LTrace("Encoding audio packet: ", numSamples);

    // Resample input data or add it to the buffer directly
    if (resampler) {
        if (!resampler->resample((uint8_t**)samples, numSamples)) {
            LTrace("Samples buffered by resampler");
            return false;
        }

        STrace << "Resampled audio packet: " << numSamples << " <=> "
               << resampler->outNumSamples;

        // Add the converted input samples to the FIFO buffer.
        fifo.write((void**)resampler->outSamples, resampler->outNumSamples);
    } else {
        // Add the input samples to the FIFO buffer.
        fifo.write((void**)samples, numSamples);
    }

    // Set a timestamp value on the frame to be encoded if given.
    if (pts != AV_NOPTS_VALUE) {
        frame->pts = pts;
    }

    return flushBuffer(this) > 0;
}


bool AudioEncoder::encode(AVFrame* iframe)
{
    LTrace("Encoding audio frame");

    // Validate input frame format matches encoder expectations
    if (iframe && iframe->format != ctx->sample_fmt)
        throw std::runtime_error("Audio frame sample format mismatch");
    if (iframe && iframe->nb_samples != ctx->frame_size)
        throw std::runtime_error("Audio frame size mismatch");

    // Send the frame to the encoder
    int ret = avcodec_send_frame(ctx, iframe);
    if (ret < 0) {
        throw std::runtime_error("Cannot encode audio frame: " + averror(ret));
    }

    bool frameEncoded = false;
    AVPacket* opacket = av_packet_alloc();
    if (!opacket)
        throw std::runtime_error("Cannot allocate output packet");

    // Receive all available encoded packets
    while (ret >= 0) {
        ret = avcodec_receive_packet(ctx, opacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }
        if (ret < 0) {
            av_packet_free(&opacket);
            throw std::runtime_error("Cannot encode audio frame: " + averror(ret));
        }

        // fps.tick();
        opacket->flags |= AV_PKT_FLAG_KEY;
        if (stream) {
            opacket->stream_index = stream->index;
        }
        STrace << "Audio frame encoded:\n"
               << "\n\tFrame PTS: " << (iframe ? iframe->pts : 0)
               << "\n\tPTS: " << opacket->pts << "\n\tDTS: " << opacket->dts
               << "\n\tDuration: " << opacket->duration;

        emitPacket(this, opacket);
        frameEncoded = true;
        av_packet_unref(opacket);
    }

    if (!frameEncoded) {
        LTrace("No frame encoded");
    }

    av_packet_free(&opacket);
    return frameEncoded;
}


void AudioEncoder::flush()
{
    LTrace("Flush");

    // Flush any remaining frames in the FIFO
    flushBuffer(this);

    // Send nullptr frame to signal end of stream
    (void)encode(nullptr);
}


} // namespace av
} // namespace icy


#endif


/// @\}
