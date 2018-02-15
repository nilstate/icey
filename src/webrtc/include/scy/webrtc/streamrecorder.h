///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_StreamRecorder_H
#define SCY_WebRTC_StreamRecorder_H


#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/av.h"
#include "scy/av/multiplexencoder.h"

#include "webrtc/api/peerconnectioninterface.h"


namespace scy {
namespace wrtc {


class StreamRecorder : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                       public webrtc::AudioTrackSinkInterface
{
public:
    StreamRecorder(const av::EncoderOptions& options);
    ~StreamRecorder();

    void setVideoTrack(webrtc::VideoTrackInterface* track);
    void setAudioTrack(webrtc::AudioTrackInterface* track);

    /// VideoSinkInterface implementation
    void OnFrame(const webrtc::VideoFrame& frame) override;

    /// AudioTrackSinkInterface implementation
    void OnData(const void* audio_data, int bits_per_sample, int sample_rate,
                size_t number_of_channels, size_t number_of_frames) override;

protected:
    av::MultiplexEncoder _encoder;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> _videoTrack;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> _audioTrack;
    bool _awaitingVideo;
    bool _awaitingAudio;
};


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG
#endif


/// @\}
