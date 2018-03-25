///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#ifndef SCY_AV_MediaCapture_H
#define SCY_AV_MediaCapture_H


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/audiodecoder.h"
#include "scy/av/ffmpeg.h"
#include "scy/av/icapture.h"
#include "scy/av/packet.h"
#include "scy/av/videodecoder.h"
#include "scy/interface.h"
#include "scy/packetsignal.h"


namespace scy {
namespace av {


/// This class implements a cross platform audio, video, screen and
/// video file capturer.
class AV_API MediaCapture : public ICapture, public basic::Runnable
{
public:
    typedef std::shared_ptr<MediaCapture> Ptr;

    MediaCapture();
    virtual ~MediaCapture();

    virtual void openFile(const std::string& file);
    // #ifdef HAVE_FFMPEG_AVDEVICE
    // virtual void openCamera(const std::string& device, int width = -1, int height = -1, double framerate = -1);
    // virtual void openMicrophone(const std::string& device, int channels = -1, int sampleRate = -1);
    // #endif
    virtual void close();

    virtual void start() override;
    virtual void stop() override;

    virtual void run() override;

    virtual void getEncoderFormat(Format& format);
    virtual void getEncoderAudioCodec(AudioCodec& params);
    virtual void getEncoderVideoCodec(VideoCodec& params);

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
    virtual void openStream(const std::string& filename, AVInputFormat* inputFormat, AVDictionary** formatParams);

    void emit(IPacket& packet);

protected:
    mutable std::mutex _mutex;
    Thread _thread;
    AVFormatContext* _formatCtx;
    VideoDecoder* _video;
    AudioDecoder* _audio;
    std::string _error;
    bool _stopping;
    bool _looping;
    bool _realtime;
    bool _ratelimit;
};


} // namespace av
} // namespace scy


#endif
#endif // SCY_AV_MediaCapture_H


/// @\}
