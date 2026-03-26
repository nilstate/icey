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
#include "icy/av/videoconverter.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}


namespace icy {
namespace av {


/// Base video context from which all video encoders and decoders derive.
struct VideoContext
{
    VideoContext();
    virtual ~VideoContext() noexcept;

    VideoContext(const VideoContext&) = delete;
    VideoContext& operator=(const VideoContext&) = delete;
    VideoContext(VideoContext&&) = delete;
    VideoContext& operator=(VideoContext&&) = delete;

    /// Initialise the AVCodecContext with codec-specific defaults.
    /// Overridden by VideoEncoder and VideoDecoder.
    virtual void create();

    /// Open the codec and create the pixel format conversion context if required.
    /// Throws std::runtime_error if the codec context has not been created.
    virtual void open();

    /// Close the codec context, free the frame, and reset timestamps.
    virtual void close();

    /// Decode a compressed video packet and emit the resulting frame.
    /// @param ipacket  The compressed video packet to decode.
    /// @return True if an output frame was decoded and emitted, false otherwise.
    [[nodiscard]] virtual bool decode(AVPacket& ipacket);

    /// Encode a buffer of interleaved video data.
    /// @param data  Pointer to the interleaved frame buffer.
    /// @param size  Size of the buffer in bytes.
    /// @param pts   Presentation timestamp in stream time base units.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] virtual bool encode(uint8_t* data, int size, int64_t pts);

    /// Encode a planar video frame.
    /// @param data      Array of per-plane data pointers (up to 4 planes).
    /// @param linesize  Array of per-plane byte strides.
    /// @param pts       Presentation timestamp in stream time base units.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] virtual bool encode(uint8_t* data[4], int linesize[4], int64_t pts);

    /// Encode a single AVFrame.
    /// @param iframe  The source video frame; must have all fields set correctly.
    /// @return True if an encoded packet was produced and emitted.
    [[nodiscard]] virtual bool encode(AVFrame* iframe);

    /// Flush any frames buffered inside the codec and emit remaining output.
    virtual void flush();

    /// Convert the video frame and return the result.
    ///
    /// The input frame will only be converted if it doesn't match the output
    /// format. If the frame is not converted the input frame will be returned.
    /// If the input frame format does not match the `VideoConverter` context
    /// then the `VideoConverter` will be recreated with the input frame params.
    virtual AVFrame* convert(AVFrame* iframe);

    /// Recreate the VideoConverter if the input or output parameters have changed.
    /// Called automatically by open() and convert().
    /// @return True if the converter was (re)created, false if it is already up to date.
    virtual bool recreateConverter();

    PacketSignal emitter;

    VideoCodec iparams;                   ///< input parameters
    VideoCodec oparams;                   ///< output parameters
    AVStream* stream;                     ///< encoder or decoder stream
    AVCodecContext* ctx;                  ///< encoder or decoder context
    const AVCodec* codec;                 ///< encoder or decoder codec
    AVFrame* frame;                       ///< encoder or decoder frame
    std::unique_ptr<VideoConverter> conv; ///< video conversion context
    int64_t time;                         ///< stream time in codec time base
    int64_t pts;                          ///< last packet pts value
    double seconds;                       ///< video time in seconds
    std::string error;                    ///< error message
};


//
// Inline helpers
//


/// Allocate a new AVFrame with the given pixel format and dimensions.
/// Uses av_frame_get_buffer for reference-counted allocation with 16-byte alignment.
/// @param pixelFmt  The pixel format for the frame.
/// @param width     The frame width in pixels.
/// @param height    The frame height in pixels.
/// @return A newly allocated AVFrame, or nullptr on failure.
AVFrame* createVideoFrame(AVPixelFormat pixelFmt, int width, int height);

/// Perform a deep copy of an AVFrame including its buffer data and properties.
/// @param source  The source frame to copy.
/// @return A newly allocated AVFrame with copied data. Caller owns the result.
AVFrame* cloneVideoFrame(AVFrame* source);

/// Populate a VideoCodec from an open AVStream and AVCodecContext.
/// @param stream  The AVStream for frame rate information (may be null).
/// @param ctx     The FFmpeg codec context to read from.
/// @param params  The VideoCodec struct to fill with codec, dimension, and format info.
void initVideoCodecFromContext(const AVStream* stream, const AVCodecContext* ctx, VideoCodec& params);

/// Select the best supported pixel format for a codec given the requested parameters.
/// Returns the requested format if supported, otherwise the first format with the same plane count.
/// @param codec   The FFmpeg codec whose supported formats are queried.
/// @param params  The video codec parameters specifying the desired pixel format.
/// @return The chosen AVPixelFormat.
AVPixelFormat selectPixelFormat(const AVCodec* codec, VideoCodec& params);


} // namespace av
} // namespace icy


#endif


/// @}
