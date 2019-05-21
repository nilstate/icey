///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/videoencoder.h"

#ifdef HAVE_FFMPEG

#include "scy/logger.h"


using std::endl;


namespace scy {
namespace av {


VideoEncoder::VideoEncoder(AVFormatContext* format)
    : format(format)
{
}


VideoEncoder::~VideoEncoder()
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
        format->oformat->video_codec = codec->id;

        // Add a video stream that uses the format's default video
        // codec to the format context's streams[] array.
        stream = avformat_new_stream(format, codec);
        if (!stream)
            throw std::runtime_error("Cannot create video stream.");

        // Testing realtime streams
        //
        // http://stackoverflow.com/questions/16768794/muxing-from-video-and-video-files-with-ffmpeg
        // stream->time_base.den = 1000; //realtime_ ? 1000 : fps_.num;
        // stream->time_base.num = 1; //realtime_ ? 1: fps_.den;
        //
        // stream->r_frame_rate.num = oparams.fps;
        // stream->r_frame_rate.den = 1;
        // stream->avg_frame_rate.den = 1;
        // stream->avg_frame_rate.num = oparams.fps;

        ctx = stream->codec;
    }

    // Otherwise allocate the standalone AVCodecContext
    else {
        ctx = avcodec_alloc_context3(codec);
        if (!ctx)
            throw std::runtime_error("Cannot allocate encoder context.");
    }

    assert(oparams.enabled);

    avcodec_get_context_defaults3(ctx, codec);
    ctx->codec_id = codec->id;
    ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    ctx->pix_fmt = selectPixelFormat(codec, oparams);
    ctx->frame_number = 0;
    // ctx->max_b_frames = 1;

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

    // Emit one intra frame every ten
    // ctx->gop_size = 10;

    // Set some defaults for codecs of note.
    // Also set optimal output pixel formats if the
    // default AV_PIX_FMT_YUV420P was given.
    switch (ctx->codec_id) {
        case AV_CODEC_ID_H264:
            // TODO: Use oparams.quality to determine profile?
            av_opt_set(ctx->priv_data, "preset", "slow", 0); // veryfast, slow, baseline
            break;
        case AV_CODEC_ID_MJPEG:
        case AV_CODEC_ID_LJPEG:
            // Use high quality JPEG
            // TODO: Use oparams.quality to determine values
            // ctx->mb_lmin        = ctx->lmin = ctx->qmin * FF_QP2LAMBDA;
            // ctx->mb_lmax        = ctx->lmax = ctx->qmax * FF_QP2LAMBDA;
            #if LIBAVCODEC_VERSION_CHECK(52, 30, 2, 30, 2)
                ctx->flags |= AV_CODEC_FLAG_QSCALE;
            #else
                ctx->flags |= CODEC_FLAG_QSCALE;
            #endif

            ctx->global_quality = ctx->qmin * FF_QP2LAMBDA;
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            ctx->max_b_frames = 2;
            break;
        default:
            break;
    }

    // Some formats want stream headers to be separate
    if (format && format->oformat->flags & AVFMT_GLOBALHEADER) {
        #if LIBAVCODEC_VERSION_CHECK(52, 30, 2, 30, 2)
            ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        #else
            ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
        #endif
    }

    // Allocate the input frame
    frame = createVideoFrame(av_get_pix_fmt(iparams.pixelFmt.c_str()),
                             iparams.width, iparams.height);
    if (!frame)
        throw std::runtime_error("Cannot allocate input frame.");

    int ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0)
        throw std::runtime_error("Cannot open the video codec: " + averror(ret));

    // Update any output parameters that might have changed
    initVideoCodecFromContext(stream, ctx, oparams);
}


void VideoEncoder::close()
{
    LTrace("Closing")

    VideoContext::close();

#if 0
    if (buffer) {
        av_free(buffer);
        buffer = nullptr;
    }

    // Free the stream
    if (stream && format && format->nb_streams) {
        for (unsigned int i = 0; i < format->nb_streams; i++) {
            if (format->streams[i] == stream) {
                LTrace("Closing: Removing stream: ", stream)
                av_freep(&format->streams[i]->codec);
                av_freep(&format->streams[i]);
                stream = nullptr;
                format->nb_streams--;
            }
        }
    }
#endif
}


bool VideoEncoder::encode(uint8_t* data[4], int linesize[4], int64_t pts)
{
    assert(data);
    assert(data[0]);
    assert(linesize[0]);
    assert(frame);
    assert(codec);

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
    assert(data);
    assert(size);
    assert(frame);
    assert(codec);

    // Populate the input frame with data from the given buffer.
    frame->data[0] = reinterpret_cast<uint8_t*>(data);
    frame->pts = pts;

    return encode(frame);
}


void emitPacket(VideoEncoder* enc, AVPacket& opacket)
{
    // auto pixelFmt = av_get_pix_fmt(enc->oparams.pixelFmt.c_str());
    // assert(av_pix_fmt_count_planes(pixelFmt) == 1 && "planar formats not supported");

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

    VideoPacket video(opacket.data, opacket.size, enc->ctx->coded_frame->width,
                      enc->ctx->coded_frame->height, enc->time);
    video.source = &opacket;
    video.opaque = enc;
    enc->emitter.emit(video);
}


bool VideoEncoder::encode(AVFrame* iframe)
{
    // assert(iframe);
    // assert(iframe->data[0]);
    assert(ctx);
    assert(codec);

    AVPacket opacket;
    av_init_packet(&opacket);
    opacket.data = nullptr; // using encoder assigned buffer
    opacket.size = 0;

    int frameEncoded = 0;
    int ret = avcodec_encode_video2(ctx, &opacket, convert(iframe), &frameEncoded);
    if (ret < 0) {
        error = "Video encoder error: " + averror(ret);
        LError(error)
        throw std::runtime_error(error);
    }

    if (frameEncoded) {
        // fps.tick();
        if (ctx->coded_frame->key_frame)
            opacket.flags |= AV_PKT_FLAG_KEY;
        if (stream) {
            opacket.stream_index = stream->index;
            // if (opacket.pts != AV_NOPTS_VALUE)
            //     opacket.pts = av_rescale_q(opacket.pts, ctx->time_base, stream->time_base);
            // if (opacket.dts != AV_NOPTS_VALUE)
            //     opacket.dts = av_rescale_q(opacket.dts, ctx->time_base, stream->time_base);
            // if (opacket.duration > 0)
            //     opacket.duration = (int)av_rescale_q(opacket.duration, ctx->time_base, stream->time_base);

            STrace << "Encoded frame:"
                 << "\n\tScaled PTS: " << opacket.pts
                 << "\n\tScaled DTS: " << opacket.dts
                 << "\n\tLcaled Duration: " << opacket.duration << std::endl;

            emitPacket(this, opacket);
        }

        return true;
    }

    av_packet_unref(&opacket);

    return false;
}


void VideoEncoder::flush()
{
    do {
        // :)
    } while (encode(nullptr));
}


} // namespace av
} // namespace scy


#endif


/// @\}
