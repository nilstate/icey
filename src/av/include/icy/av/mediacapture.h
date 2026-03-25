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

#include "icy/av/audiodecoder.h"
#include "icy/av/ffmpeg.h"
#include "icy/av/icapture.h"
#include "icy/av/packet.h"
#include "icy/av/videodecoder.h"
#include "icy/interface.h"
#include "icy/packetsignal.h"

#include <atomic>


namespace icy {
namespace av {


/// Unified capture and decode source for files and live media devices.
class AV_API MediaCapture : public ICapture
    , public basic::Runnable
{
public:
    using Ptr = std::shared_ptr<MediaCapture>;

    MediaCapture();
    ~MediaCapture() noexcept override;

    MediaCapture(const MediaCapture&) = delete;
    MediaCapture& operator=(const MediaCapture&) = delete;
    MediaCapture(MediaCapture&&) = delete;
    MediaCapture& operator=(MediaCapture&&) = delete;

    /// Open a media file for decoding. Automatically detects video and audio streams.
    /// @param file  Path to the media file.
    void openFile(const std::string& file) override;
    // #ifdef HAVE_FFMPEG_AVDEVICE
    // virtual void openCamera(const std::string& device, int width = -1, int height = -1, double framerate = -1);
    // virtual void openMicrophone(const std::string& device, int channels = -1, int sampleRate = -1);
    // #endif

    /// Stop the capture thread and close the media stream and all decoders.
    void close() override;

    /// Start the background capture and decode thread.
    /// Throws std::runtime_error if no media streams have been opened.
    virtual void start() override;

    /// Signal the capture thread to stop and join it before returning.
    virtual void stop() override;

    /// Entry point for the background capture thread.
    /// Reads and decodes packets from the format context until EOF or stop() is called.
    virtual void run() override;

    /// Fill @p format with the combined encoder-ready video and audio codec parameters.
    /// @param format  Output Format struct to populate.
    void getEncoderFormat(Format& format) override;

    /// Fill @p params with the decoder's output audio codec parameters.
    /// Throws std::runtime_error if audio parameters have not been initialised.
    /// @param params  Output AudioCodec struct to populate.
    void getEncoderAudioCodec(AudioCodec& params) override;

    /// Fill @p params with the decoder's output video codec parameters.
    /// Throws std::runtime_error if video parameters have not been initialised.
    /// @param params  Output VideoCodec struct to populate.
    void getEncoderVideoCodec(VideoCodec& params) override;

    /// Continuously loop the input file when set.
    void setLoopInput(bool flag);

    /// Limit playback to video FPS.
    void setLimitFramerate(bool flag);

    /// Set to use realtime PTS calculation.
    /// This is preferred when using live captures as FFmpeg-provided values are
    /// not always reliable.
    void setRealtimePTS(bool flag);

    /// @return The underlying AVFormatContext (thread-safe, mutex-protected).
    AVFormatContext* formatCtx() const;

    /// @return The active VideoDecoder, or nullptr if no video stream was opened.
    VideoDecoder* video() const;

    /// @return The active AudioDecoder, or nullptr if no audio stream was opened.
    AudioDecoder* audio() const;

    /// @return True if the capture thread has been asked to stop.
    bool stopping() const;

    /// @return The last error message, or an empty string if no error has occurred.
    std::string error() const;

    /// Signals that the capture thread is closing.
    /// This signal is emitted from the capture thread context.
    NullSignal Closing;

protected:
    /// Open the underlying media stream.
    ///
    /// @param filename      The file path or device name to open.
    /// @param inputFormat   The forced input format, or nullptr for auto-detect.
    /// @param formatParams  Optional format parameters; may be updated by FFmpeg on return.
    void openStream(const std::string& filename, const AVInputFormat* inputFormat, AVDictionary** formatParams) override;

    void emit(IPacket& packet);

protected:
    mutable std::mutex _mutex;
    Thread _thread;
    AVFormatContext* _formatCtx;
    std::unique_ptr<VideoDecoder> _video;
    std::unique_ptr<AudioDecoder> _audio;
    std::string _error;
    std::atomic<bool> _stopping;
    std::atomic<bool> _looping;
    std::atomic<bool> _realtime;
    std::atomic<bool> _ratelimit;
};


} // namespace av
} // namespace icy


#endif


/// @}
