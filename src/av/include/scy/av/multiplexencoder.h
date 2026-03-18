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

#ifdef HAVE_FFMPEG

#include "scy/av/audioencoder.h"
#include "scy/av/ffmpeg.h"
#include "scy/av/iencoder.h"
#include "scy/av/packet.h"
#include "scy/av/videoencoder.h"
#include "scy/packetstream.h"
#include <fstream>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/fifo.h>
#include <libswscale/swscale.h>
}


namespace scy {
namespace av {


/// This class implements a multiplex audio and video encoder.
class AV_API MultiplexEncoder : public IEncoder
{
public:
    MultiplexEncoder(const EncoderOptions& options = EncoderOptions());
    ~MultiplexEncoder() noexcept override;

    MultiplexEncoder(const MultiplexEncoder&) = delete;
    MultiplexEncoder& operator=(const MultiplexEncoder&) = delete;
    MultiplexEncoder(MultiplexEncoder&&) = delete;
    MultiplexEncoder& operator=(MultiplexEncoder&&) = delete;

    virtual void init() override;
    virtual void uninit() override;
    void cleanup() override;

    void createVideo() override;
    void freeVideo() override;

    /// Encode a single video frame.
    /// All frame values must be set, such as size, pizel format and PTS.
    [[nodiscard]] bool encodeVideo(AVFrame* frame) override;

    /// Encode a single interleaved video frame.
    /// If the frame time is specified it should be the microseconds
    /// offset since the start of the input stream. If no time is specified
    /// a realtime time value will be assigned to the frame.
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

    void createAudio() override;
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

    /// Flush and beffered or queued packets.
    void flush() override;

    EncoderOptions& options();
    VideoEncoder* video();
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
} // namespace scy


#endif


/// @\}
