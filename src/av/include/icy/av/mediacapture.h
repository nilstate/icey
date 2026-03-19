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


/// This class implements a cross platform audio, video, screen and
/// video file capturer.
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

    void openFile(const std::string& file) override;
    // #ifdef HAVE_FFMPEG_AVDEVICE
    // virtual void openCamera(const std::string& device, int width = -1, int height = -1, double framerate = -1);
    // virtual void openMicrophone(const std::string& device, int channels = -1, int sampleRate = -1);
    // #endif
    void close() override;

    virtual void start() override;
    virtual void stop() override;

    virtual void run() override;

    void getEncoderFormat(Format& format) override;
    void getEncoderAudioCodec(AudioCodec& params) override;
    void getEncoderVideoCodec(VideoCodec& params) override;

    /// Continuously loop the input file when set.
    void setLoopInput(bool flag);

    /// Limit playback to video FPS.
    void setLimitFramerate(bool flag);

    /// Set to use realtime PTS calculation.
    /// This is preferred when sing live captures as FFmpeg provided values are
    /// not always reliable.
    void setRealtimePTS(bool flag);

    AVFormatContext* formatCtx() const;
    VideoDecoder* video() const;
    AudioDecoder* audio() const;
    bool stopping() const;
    std::string error() const;

    /// Signals that the capture thread is closing.
    /// Careful, this signal is emitted from inside the tread contect.
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


/// @\}
