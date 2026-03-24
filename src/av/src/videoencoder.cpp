///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/videoencoder.h"

#ifdef HAVE_FFMPEG

#include "icy/logger.h"




namespace icy {
namespace av {


VideoEncoder::VideoEncoder(AVFormatContext* format)
    : format(format)
{
}


VideoEncoder::~VideoEncoder() noexcept
{
    close();
}


void VideoEncoder::create()
{
    // Find the video encoder
    codec = avcodec_find_encoder_by_name(oparams.encoder.c_str());
    if (!codec) {
        if (format)
            codec = avcodec_find_encoder(format->oformat->video_codec);
        if (!codec)
            throw std::runtime_error("Video encoder not found: " + oparams.encoder);
    }

    // Allocate stream and AVCodecContext from the AVFormatContext if available
    if (format) {
        // Add a video stream that uses the format's default video
        // codec to the format context's streams[] array.
        stream = avformat_new_stream(format, codec);
        if (!stream)
            throw std::runtime_error("Cannot create video stream.");
    }

    // Allocate the AVCodecContext
    ctx = avcodec_alloc_context3(codec);
    if (!ctx)
        throw std::runtime_error("Cannot allocate encoder context.");

    if (!oparams.enabled)
        throw std::runtime_error("Video output parameters are not enabled");

    ctx->codec_id = codec->id;
    ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    ctx->pix_fmt = selectPixelFormat(codec, oparams);

    // Resolution must be a multiple of two
    ctx->width = oparams.width;
    ctx->height = oparams.height;

    // For fixed-fps content timebase should be 1/framerate
    // and timestamp increments should be identically 1.
    ctx->time_base.den = int(oparams.fps ? oparams.fps : 1000);
    ctx->time_base.num = 1;

    // Define encoding parameters
    if (oparams.bitRate) {
        ctx->bit_rate = oparams.bitRate;
        ctx->bit_rate_tolerance = oparams.bitRate * 1000; // needed when time_base.num > 1
    }

    // Set some defaults for codecs of note.
    // These can be overridden by oparams.options below.
    switch (ctx->codec_id) {
        case AV_CODEC_ID_H264:
            if (oparams.options.find("preset") == oparams.options.end())
                av_opt_set(ctx->priv_data, "preset", "medium", 0);
            break;
        case AV_CODEC_ID_MJPEG:
        case AV_CODEC_ID_LJPEG:
            ctx->flags |= AV_CODEC_FLAG_QSCALE;
            ctx->global_quality = ctx->qmin * FF_QP2LAMBDA;
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            ctx->max_b_frames = 2;
            break;
        default:
            break;
    }

    // Apply user-specified encoder options.
    for (const auto& [key, value] : oparams.options) {
        int ret = av_opt_set(ctx->priv_data, key.c_str(), value.c_str(), 0);
        if (ret < 0)
            LWarn("Ignoring unknown video encoder option: ", key, "=", value);
    }

    // Some formats want stream headers to be separate
    if (format && format->oformat->flags & AVFMT_GLOBALHEADER) {
        ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    // Allocate the input frame
    frame = createVideoFrame(av_get_pix_fmt(iparams.pixelFmt.c_str()),
                             iparams.width, iparams.height);
    if (!frame)
        throw std::runtime_error("Cannot allocate input frame.");

    int ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0)
        throw std::runtime_error("Cannot open the video codec: " + averror(ret));

    // Copy codec parameters to the stream
    if (stream) {
        int ret = avcodec_parameters_from_context(stream->codecpar, ctx);
        if (ret < 0)
            throw std::runtime_error("Cannot copy video codec parameters to stream: " + averror(ret));
        stream->time_base = ctx->time_base;
    }

    // Update any output parameters that might have changed
    initVideoCodecFromContext(stream, ctx, oparams);
}


void VideoEncoder::close()
{
    LTrace("Closing");

    VideoContext::close();
}


bool VideoEncoder::encode(uint8_t* data[4], int linesize[4], int64_t pts)
{
    if (!data || !data[0] || !linesize[0])
        throw std::runtime_error("Invalid video input data");
    if (!frame || !codec)
        throw std::runtime_error("Video encoder not initialized");

    // Populate the input frame with data from the given buffer.
    for (int i = 0; i < 4; ++i) {
        frame->data[i] = data[i];
        frame->linesize[i] = linesize[i];
    }

    frame->pts = pts;
    return encode(frame);
}


bool VideoEncoder::encode(uint8_t* data, int size, int64_t pts)
{
    if (!data || !size)
        throw std::runtime_error("Invalid video input data");
    if (!frame || !codec)
        throw std::runtime_error("Video encoder not initialized");

    // Populate the input frame with data from the given buffer.
    frame->data[0] = reinterpret_cast<uint8_t*>(data);
    frame->pts = pts;

    return encode(frame);
}


void emitPacket(VideoEncoder* enc, AVPacket* opacket)
{
    const auto timeBase = enc->stream ? enc->stream->time_base : enc->ctx->time_base;

    // Standalone packet encoders do not have an AVStream, but their packets
    // still need stable microsecond timestamps for downstream WebRTC/media use.
    enc->time = opacket->pts > 0
        ? av_rescale_q(opacket->pts, timeBase, AVRational{1, AV_TIME_BASE})
        : 0;
    enc->seconds = opacket->pts * av_q2d(timeBase);

    // Set the encoder pts in stream time base
    enc->pts = opacket->pts;

    if (!opacket->data || !opacket->size)
        return;

    VideoPacket video(opacket->data, opacket->size, enc->ctx->width,
                      enc->ctx->height, enc->time);
    video.avpacket = opacket;
    enc->emitter.emit(video);
}


bool VideoEncoder::encode(AVFrame* iframe)
{
    if (!ctx || !codec)
        throw std::runtime_error("Video encoder not initialized");

    // Send the frame to the encoder (convert first if needed)
    int ret = avcodec_send_frame(ctx, convert(iframe));
    if (ret < 0) {
        error = "Video encoder error: " + averror(ret);
        LError(error);
        throw std::runtime_error(error);
    }

    bool encoded = false;
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
            error = "Video encoder error: " + averror(ret);
            LError(error);
            throw std::runtime_error(error);
        }

        // fps.tick();
        if (stream) {
            opacket->stream_index = stream->index;

            STrace << "Encoded frame:"
                   << "\n\tScaled PTS: " << opacket->pts
                   << "\n\tScaled DTS: " << opacket->dts
                   << "\n\tDuration: " << opacket->duration;
        }

        emitPacket(this, opacket);

        encoded = true;
        av_packet_unref(opacket);
    }

    av_packet_free(&opacket);
    return encoded;
}


void VideoEncoder::flush()
{
    // Send nullptr frame to signal end of stream
    encode(nullptr);
}


} // namespace av
} // namespace icy


#endif


/// @}
