///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/videodecoder.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"
#include "scy/logger.h"




namespace scy {
namespace av {


VideoDecoder::VideoDecoder(AVStream* stream)
{
    this->stream = stream;
}


VideoDecoder::~VideoDecoder() noexcept
{
    close();
}


void VideoDecoder::create()
{
    if (!stream)
        throw std::runtime_error("VideoDecoder: stream is null");
    LTrace("Create: ", stream->index);

    codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec)
        throw std::runtime_error("Video codec missing or unsupported.");

    ctx = avcodec_alloc_context3(codec);
    if (!ctx)
        throw std::runtime_error("Cannot allocate video decoder context.");

    int ret = avcodec_parameters_to_context(ctx, stream->codecpar);
    if (ret < 0)
        throw std::runtime_error("Cannot copy video codec parameters: " + averror(ret));

    frame = av_frame_alloc();
    if (frame == nullptr)
        throw std::runtime_error("Cannot allocate video input frame.");

    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0)
        throw std::runtime_error("Cannot open the video codec: " + averror(ret));

    // Set the default input and output parameters are set here once the codec
    // context has been opened. The output pixel format, width or height can be
    // modified after this call and a conversion context will be created on the
    // next call to open() to output the desired format.
    initVideoCodecFromContext(stream, ctx, iparams);
    initVideoCodecFromContext(stream, ctx, oparams);
}


void VideoDecoder::open()
{
    VideoContext::open();
}


void VideoDecoder::close()
{
    VideoContext::close();
}


inline void emitPacket(VideoDecoder* dec, AVFrame* frame)
{
    // Use best_effort_timestamp which is set by the decoder
    frame->pts = frame->best_effort_timestamp;

    // Set the decoder time in microseconds
    int64_t pts = frame->pts;
    dec->time = pts > 0 ? static_cast<int64_t>(pts *
                                               av_q2d(dec->stream->time_base) * AV_TIME_BASE)
                        : 0;

    // Set the decoder pts in stream time base
    dec->pts = pts;

    // Set the decoder seconds since stream start
    dec->seconds = (pts - dec->stream->start_time) * av_q2d(dec->stream->time_base);

    STrace << "Decoded video frame:"
           << "\n\tFrame PTS: " << frame->pts
           << "\n\tTimestamp: " << dec->time
           << "\n\tPTS: " << pts
           << "\n\tSeconds: " << dec->seconds
;

    PlanarVideoPacket video(frame->data, frame->linesize, dec->oparams.pixelFmt,
                            frame->width, frame->height, dec->time);
    video.avframe = frame;

    dec->emitter.emit(video);
}


bool VideoDecoder::decode(AVPacket& ipacket)
{
    if (!ctx || !codec || !frame)
        throw std::runtime_error("VideoDecoder not initialized");
    if (stream && ipacket.stream_index != stream->index)
        return false;

    // Send packet to the decoder
    int ret = avcodec_send_packet(ctx, &ipacket);
    if (ret < 0) {
        error = "Video decoder error: " + averror(ret);
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
            error = "Video decoder error: " + averror(ret);
            LError(error);
            throw std::runtime_error(error);
        }

        // fps.tick();
        emitPacket(this, convert(frame));
        decoded = true;
    }

    return decoded;
}


void VideoDecoder::flush()
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
            LTrace("Flushed video frame");
            emitPacket(this, convert(frame));
        }
    }
}


} // namespace av
} // namespace scy


#endif


/// @\}
