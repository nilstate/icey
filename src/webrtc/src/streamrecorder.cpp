///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "scy/webrtc/streamrecorder.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"
#include "scy/filesystem.h"
#include "scy/logger.h"

#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "media/engine/webrtcvideocapturerfactory.h"
#include "modules/video_capture/video_capture_factory.h"


namespace scy {
namespace wrtc {


StreamRecorder::StreamRecorder(const av::EncoderOptions& options)
    : _encoder(options)
{
    // Disable audio and video until tracks are set
    _encoder.options().oformat.video.enabled = false;
    _encoder.options().oformat.audio.enabled = false;
}


StreamRecorder::~StreamRecorder()
{
    if (_videoTrack)
        _videoTrack->RemoveSink(this);

    if (_audioTrack)
        _audioTrack->RemoveSink(this);
}


void StreamRecorder::setVideoTrack(webrtc::VideoTrackInterface* track)
{
    assert(!_videoTrack);
    _videoTrack = track;
    _videoTrack->AddOrUpdateSink(this, rtc::VideoSinkWants());
    _encoder.options().iformat.video.enabled = true;
    _awaitingVideo = true;
}


void StreamRecorder::setAudioTrack(webrtc::AudioTrackInterface* track)
{
    assert(!_audioTrack);
    _audioTrack = track;
    _audioTrack->AddSink(this);
    _encoder.options().iformat.audio.enabled = true;
    _awaitingAudio = true;
}


void StreamRecorder::OnFrame(const webrtc::VideoFrame& yuvframe)
{
    LTrace("On video frame: ", yuvframe.width(), 'x', yuvframe.height())

    // TODO: Recreate encoder context on input stream change
    if (_awaitingVideo) {
        _awaitingVideo = false;

        auto& ivideo = _encoder.options().iformat.video;
        ivideo.width = yuvframe.width();
        ivideo.height = yuvframe.height();
        ivideo.pixelFmt = "yuv420p";
        ivideo.fps = 25;

        if (_shouldInit) {
            try {
                _encoder.init();
                _shouldInit = false;
            } catch (std::exception& exc) {
              LError("Failed to init encoder: ", exc.what())
              _encoder.uninit();
            }
        }
    }

    if (_encoder.isActive()) {
        // Set AVFrame->data pointers manually so we don't need to copy any data
        // or convert the pixel format from YUV to some contiguous format.
        auto yuvbuffer = yuvframe.video_frame_buffer()->GetI420();
        auto frame = _encoder.video()->frame;
        frame->data[0] = (uint8_t*)yuvbuffer->DataY();
        frame->data[1] = (uint8_t*)yuvbuffer->DataU();
        frame->data[2] = (uint8_t*)yuvbuffer->DataV();
        frame->width = yuvframe.width();
        frame->height = yuvframe.height();
        frame->pts = AV_NOPTS_VALUE; // set by encoder

        // Encode the video frame
        _encoder.encodeVideo(frame);
    }
}


void StreamRecorder::OnData(const void* audio_data, int bits_per_sample,
                            int sample_rate, size_t number_of_channels,
                            size_t number_of_frames)
{
    STrace << "On audio frame: "
           << "number_of_frames=" << number_of_frames << ", "
           << "number_of_channels=" << number_of_channels << ", "
           << "sample_rate=" << sample_rate << ", "
           << "bits_per_sample=" << bits_per_sample << std::endl;

    // FIXME: For some reason the first couple of samples come though as mono,
    // so let's just skip those for now.
    if (number_of_channels < 2) {
        LWarn("Dropping initial mono samples")
        return;
    }

    // TODO: Recreate encoder context on input stream change
    if (_awaitingAudio) {
        _awaitingAudio = false;

        auto& iaudio = _encoder.options().iformat.audio;
        iaudio.channels = number_of_channels;
        iaudio.sampleRate = sample_rate;
        iaudio.sampleFmt = "s16";

        // WebRTC always sets 16 bits per sample, hence we use the s16 sample
        // format. Set an assertion just incase this ever changes or varies.
        assert(bits_per_sample == 16);

        if (_shouldInit) {
            try {
                _encoder.init();
                _shouldInit = false;
            } catch (std::exception& exc) {
                LError("Failed to init encoder: ", exc.what())
                _encoder.uninit();
            }
        }
    }

    if (_encoder.isActive())
        _encoder.encodeAudio((uint8_t*)audio_data, number_of_frames);
}


} } // namespace scy::wrtc


/// @\}


#endif // HAVE_FFMPEG
