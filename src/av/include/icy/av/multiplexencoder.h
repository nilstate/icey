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

#ifdef HAVE_FFMPEG

#include "icy/av/audioencoder.h"
#include "icy/av/ffmpeg.h"
#include "icy/av/iencoder.h"
#include "icy/av/packet.h"
#include "icy/av/videoencoder.h"
#include "icy/packetstream.h"
#include <fstream>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/fifo.h>
#include <libswscale/swscale.h>
}


namespace icy {
namespace av {


/// Multiplexing encoder that writes synchronized audio and video streams.
class AV_API MultiplexEncoder : public IEncoder
{
public:
    /// Construct the encoder with the given options.
    /// @param options  The encoder configuration (input/output formats and file paths).
    MultiplexEncoder(const EncoderOptions& options = EncoderOptions());
    ~MultiplexEncoder() noexcept override;

    MultiplexEncoder(const MultiplexEncoder&) = delete;
    MultiplexEncoder& operator=(const MultiplexEncoder&) = delete;
    MultiplexEncoder(MultiplexEncoder&&) = delete;
    MultiplexEncoder& operator=(MultiplexEncoder&&) = delete;

    /// Open the output container, create codec streams, and write the format header.
    virtual void init() override;

    /// Flush encoded packets, write the format trailer, and close the output container.
    virtual void uninit() override;

    /// Release all resources allocated by init() without writing a trailer.
    void cleanup() override;

    /// Create the video encoder and add the stream to the output container.
    void createVideo() override;

    /// Flush and free the video encoder and its stream.
    void freeVideo() override;

    /// Encode a single video frame.
    /// All frame values must be set, such as size, pixel format, and PTS.
    [[nodiscard]] bool encodeVideo(AVFrame* frame) override;

    /// Encode a single interleaved video frame.
    /// If the frame time is specified it should be the microseconds
    /// offset since the start of the input stream. If no time is specified
    /// a real-time value will be assigned to the frame.
    ///
    /// @param buffer      The raw video frame buffer.
    /// @param bufferSize  The buffer size in bytes.
    /// @param width       The frame width in pixels.
    /// @param height      The frame height in pixels.
    /// @param time        The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.
    [[nodiscard]] virtual bool encodeVideo(uint8_t* buffer, int bufferSize, int width, int height,
                                           int64_t time = AV_NOPTS_VALUE);

    /// Encode a single planar video frame.
    ///
    /// @param data      Array of per-plane data pointers (up to 4 planes).
    /// @param linesize  Array of per-plane byte strides.
    /// @param width     The frame width in pixels.
    /// @param height    The frame height in pixels.
    /// @param time      The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.
    [[nodiscard]] virtual bool encodeVideo(uint8_t* data[4], int linesize[4], int width, int height,
                                           int64_t time = AV_NOPTS_VALUE);

    /// Create the audio encoder and add the stream to the output container.
    void createAudio() override;

    /// Flush and free the audio encoder and its stream.
    void freeAudio() override;

    /// Encode a single interleaved audio frame.
    ///
    /// @param buffer      The interleaved audio sample buffer.
    /// @param numSamples  The number of samples per channel.
    /// @param time        The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.
    [[nodiscard]] virtual bool encodeAudio(uint8_t* buffer, int numSamples,
                                           int64_t time = AV_NOPTS_VALUE);

    /// Encode a single planar audio frame.
    ///
    /// @param data        Array of per-plane sample buffers (one per channel).
    /// @param numSamples  The number of samples per channel.
    /// @param time        The timestamp in microseconds, or AV_NOPTS_VALUE for realtime.
    [[nodiscard]] virtual bool encodeAudio(uint8_t* data[4], int numSamples,
                                           int64_t time = AV_NOPTS_VALUE);

    /// Flush any buffered or queued packets to the output container.
    void flush() override;

    /// @return A reference to the encoder's configuration options.
    EncoderOptions& options();

    /// @return The active VideoEncoder, or nullptr if video has not been created.
    VideoEncoder* video();

    /// @return The active AudioEncoder, or nullptr if audio has not been created.
    AudioEncoder* audio();

    PacketSignal emitter;

protected:
    bool writeOutputPacket(AVPacket& packet);

    /// Convert input microseconds to the stream time base.
    ///
    /// @param stream  The target stream for time base conversion.
    /// @param pts     Pointer to the timestamp; converted in place.
    bool updateStreamPts(AVStream* stream, int64_t* pts);

    void onVideoEncoded(av::VideoPacket& packet);
    void onAudioEncoded(av::AudioPacket& packet);

    EncoderOptions _options;
    AVFormatContext* _formatCtx;
    std::unique_ptr<VideoEncoder> _video;
    std::unique_ptr<AudioEncoder> _audio;
    AVIOContext* _ioCtx;
    std::unique_ptr<uint8_t[]> _ioBuffer;
    int64_t _pts;
    mutable std::mutex _mutex;
};


} // namespace av
} // namespace icy


#endif


/// @}
