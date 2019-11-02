///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/audioencoder.h"
#include "scy/av/audioresampler.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"
#include "scy/logger.h"


using std::endl;


namespace scy {
namespace av {


AudioEncoder::AudioEncoder(AVFormatContext* format)
    : format(format)
{
    LTrace("Create")
}


AudioEncoder::~AudioEncoder()
{
    LTrace("Destroy")
    close();
}


// Initialize one input frame for writing to the output file.
// The frame will be exactly frame_size samples large.
static AVFrame* initOutputFrame(AVCodecContext* ctx)
{
    AVFrame* frame;
    int error;

    // Create a new frame to store the audio samples.
    if (!(frame = av_frame_alloc())) {
        throw std::runtime_error("Cannot allocate output frame");
    }

    // Set the frame's parameters, especially its size and format.
    // av_frame_get_buffer needs this to allocate memory for the
    // audio samples of the frame.
    // Default channel layouts based on the number of channels
    // are assumed for simplicity.
    frame->nb_samples = ctx->frame_size;
    frame->channel_layout = ctx->channel_layout;
    frame->format = ctx->sample_fmt;
    frame->sample_rate = ctx->sample_rate;

    // Allocate the samples of the created frame. This call will make
    // sure that the audio frame can hold as many samples as specified.
    if ((error = av_frame_get_buffer(frame, 0)) < 0) {
        av_frame_free(&frame);
        throw std::runtime_error("Could allocate output frame samples: " + averror(error));
    }

    return frame;
}


void AudioEncoder::create()
{
    LTrace("Create")
    int err;

    // Find the audio encoder
    if (!(codec = avcodec_find_encoder_by_name(oparams.encoder.c_str()))) {
        if (!format || !(codec = avcodec_find_encoder(format->oformat->audio_codec)))
            throw std::runtime_error("Cannot find an audio encoder for: " + oparams.encoder);
    }

    // Allocate stream and AVCodecContext from the AVFormatContext if available
    if (format) {
        format->oformat->audio_codec = codec->id;

        // Create a new audio stream in the output file container.
        if (!(stream = avformat_new_stream(format, codec))) {
            throw std::runtime_error("Cannot create the audio stream: Out of memory");
        }

        ctx = stream->codec;
    }

    // Otherwise allocate the standalone AVCodecContext
    else {
        ctx = avcodec_alloc_context3(codec);
        if (!ctx)
            throw std::runtime_error("Cannot allocate encoder context.");
    }

    // assert(oparams.bitRate);
    // assert(oparams.sampleRate);
    // assert(oparams.channels);
    // assert(!oparams.sampleFmt.empty());

    // int bitRate = oparams.bitRate ? oparams.bitRate : DEFAULT_AUDIO_BIT_RATE
    if (!oparams.bitRate)
        oparams.bitRate = DEFAULT_AUDIO_BIT_RATE;
    if (!oparams.sampleRate)
        oparams.sampleRate = DEFAULT_AUDIO_SAMPLE_RATE;
    if (!oparams.channels)
        oparams.channels = DEFAULT_AUDIO_CHANNELS;
    if (oparams.sampleFmt.empty())
        oparams.sampleFmt = DEFAULT_AUDIO_SAMPLE_FMT;

    // Now we'll setup the parameters of AVCodecContext
    avcodec_get_context_defaults3(ctx, codec);
    ctx->bit_rate = oparams.bitRate;
    ctx->sample_fmt = selectSampleFormat(codec, oparams);
    ctx->sample_rate = oparams.sampleRate;
    ctx->channels = oparams.channels;
    ctx->channel_layout = av_get_default_channel_layout(oparams.channels);

    // // Set the sample rate for the container.
    // ctx->time_base.den = oparams.sampleRate;
    // ctx->time_base.num = 1;

    ctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    // Some container formats (like MP4) require global headers to be present
    // Mark the encoder so that it behaves accordingly.
    if (format && format->oformat->flags & AVFMT_GLOBALHEADER) {
        #if LIBAVCODEC_VERSION_CHECK(52, 30, 2, 30, 2)
            ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        #else
            ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
        #endif
    }

    // Open the encoder for the audio stream to use it later.
    if ((err = avcodec_open2(ctx, codec, nullptr)) < 0) {
        throw std::runtime_error("Cannot open the audio codec: " + averror(err));
    }

    // Use the encoder's desired frame size for processing.
    outputFrameSize = ctx->frame_size;
    assert(outputFrameSize);

    // Create the encode frame
    frame = initOutputFrame(ctx);

    // Create the FIFO buffer based on the specified output sample format.
    // NOTE: We read from a FIFO buffer as many codecs require a FULL
    // `ctx->frame_size` samples for encoding.
    fifo.alloc(oparams.sampleFmt, oparams.channels, outputFrameSize);

    // Update parameters that may have changed
    initAudioCodecFromContext(ctx, oparams);

    // // Create the resampler if resampling if required
    // if (iparams.channels != oparams.channels ||
    //     iparams.sampleRate != oparams.sampleRate ||
    //     iparams.sampleFmt != oparams.sampleFmt) {
    //     recreateResampler();
    // }
}


// void AudioEncoder::open()
// {
//     LTrace("Create")
// }


void AudioEncoder::close()
{
    LTrace("Closing")

    AudioContext::close();

    if (resampler) {
        delete resampler;
        resampler = nullptr;
    }
}

void emitPacket(AudioEncoder* enc, AVPacket& opacket)
{
    // auto sampleFmt = av_get_sample_fmt(enc->oparams.sampleFmt.c_str());
    // assert(av_sample_fmt_is_planar(sampleFmt) == 0 && "planar formats not supported");

    if (enc->stream) {
        // Set the encoder time in microseconds
        // This value represents the number of microseconds
        // that have elapsed since the brginning of the stream.
        enc->time = opacket.pts > 0 ? static_cast<int64_t>(
            opacket.pts * av_q2d(enc->stream->time_base) * AV_TIME_BASE) : 0;

        // Set the encoder seconds since stream start
        // enc->seconds = enc->time / time::kNumMicrosecsPerSec;
        // enc->seconds = (opacket.pts - enc->stream->start_time) * av_q2d(enc->stream->time_base);
        enc->seconds = opacket.pts * av_q2d(enc->stream->time_base);
    }

    // Set the encoder pts in stream time base
    enc->pts = opacket.pts;

    assert(opacket.data);
    assert(opacket.size);
    // assert(opacket.pts >= 0);
    // assert(opacket.dts >= 0);

    AudioPacket audio(opacket.data, opacket.size, enc->outputFrameSize, enc->time);
    audio.source = &opacket;
    audio.opaque = enc;

    enc->emitter.emit(audio);
}


int flushBuffer(AudioEncoder* enc)
{
    // LTrace("Flush")

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
    // LTrace("Encoding audio packet: ", numSamples)

    // Resample input data or add it to the buffer directly
    if (resampler) {
        if (!resampler->resample((uint8_t**)&samples, numSamples)) {
            LTrace("Samples buffered by resampler")
            return false;
        }

        // STrace << "Resampled audio packet: " 
        //        << numSamples << " <=> "
        //        << resampler->outNumSamples << endl;

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
    LTrace("Encoding audio packet: ", numSamples)

    // Resample input data or add it to the buffer directly
    if (resampler) {
        if (!resampler->resample((uint8_t**)samples, numSamples)) {
            LTrace("Samples buffered by resampler")
            return false;
        }

        STrace << "Resampled audio packet: " << numSamples << " <=> "
            << resampler->outNumSamples << endl;

        // Add the converted input samples to the FIFO buffer.
        fifo.write((void**)resampler->outSamples, resampler->outNumSamples);
    }
    else {
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
    LTrace("Encoding audio frame")

    int frameEncoded, ret;

    // This method only receives input format sample frames
    assert(!iframe || iframe->format == ctx->sample_fmt);
    assert(!iframe || iframe->nb_samples == ctx->frame_size);

    // Set the packet data and size so that it is recognized as being empty.
    AVPacket opacket;
    av_init_packet(&opacket);
    opacket.data = nullptr;
    opacket.size = 0;

    // Encode the audio frame and store it in the temporary packet.
    // The output audio stream encoder is used to do this.
    if ((ret = avcodec_encode_audio2(ctx, &opacket, iframe, &frameEncoded)) < 0) {
        throw std::runtime_error("Cannot encode audio frame: " + averror(ret));
    }

    if (frameEncoded) {
        // fps.tick();
        opacket.flags |= AV_PKT_FLAG_KEY;
        if (stream) {
            opacket.stream_index = stream->index;
            //     if (opacket.pts != AV_NOPTS_VALUE)
            //         opacket.pts  = av_rescale_q(opacket.pts, ctx->time_base, stream->time_base);
            //     if (opacket.dts != AV_NOPTS_VALUE)
            //         opacket.dts  = av_rescale_q(opacket.dts, ctx->time_base, stream->time_base);
            //     if (opacket.duration > 0)
            //         opacket.duration = (int)av_rescale_q(opacket.duration, ctx->time_base, stream->time_base);
        }
        STrace << "Audio frame encoded:\n"
               << "\n\tFrame PTS: " << (iframe ? iframe->pts : 0)
               << "\n\tPTS: " << opacket.pts << "\n\tDTS: " << opacket.dts
               << "\n\tDuration: " << opacket.duration << endl;

        emitPacket(this, opacket);
    }
    else {
        LTrace("No frame encoded")
    }

    av_packet_unref(&opacket);

    return frameEncoded > 0;
}


void AudioEncoder::flush()
{
    LTrace("Flush")

    // Flush any remaining frames in the FIFO
    flushBuffer(this);

    do {
        // :)
    } while (encode(nullptr));
}


} // namespace av
} // namespace scy


#endif


/// @\}
