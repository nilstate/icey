///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "scy/webrtc/videopacketsource.h"

#ifdef HAVE_FFMPEG

#include "scy/av/ffmpeg.h"
#include "scy/av/videocontext.h"
#include "scy/av/videodecoder.h"
#include "scy/av/fpscounter.h"

#include "api/video/i420_buffer.h"

#include <chrono>

using std::endl;

#define SCY_WebRTC_USE_DECODER_PTS 1


namespace scy {
namespace wrtc {


VideoPacketSource::VideoPacketSource(int width, int height, int fps, uint32_t fourcc)
    : _captureFormat(cricket::VideoFormat(width, height, cricket::VideoFormat::FpsToInterval(fps), fourcc))
    , _rotation(webrtc::kVideoRotation_0)
    , _timestampOffset(0)
    , _nextTimestamp(0)
    , _source(nullptr)
{
    // Default supported formats. Use SetSupportedFormats to over write.
    std::vector<cricket::VideoFormat> formats;
    formats.push_back(_captureFormat);
    SetSupportedFormats(formats);
}


VideoPacketSource::VideoPacketSource(const cricket::VideoFormat& captureFormat)
    : _captureFormat(captureFormat)
    , _rotation(webrtc::kVideoRotation_0)
    , _timestampOffset(0)
    , _nextTimestamp(0)
    , _source(nullptr)
{
    // Default supported formats. Use SetSupportedFormats to over write.
    std::vector<cricket::VideoFormat> formats;
    formats.push_back(_captureFormat);
    SetSupportedFormats(formats);

    // formats.push_back(cricket::VideoFormat(1280, 720, _fpsInterval, _codec));
    // formats.push_back(cricket::VideoFormat(640, 480, _fpsInterval, _codec));
    // formats.push_back(cricket::VideoFormat(320, 240, _fpsInterval, _codec));
    // formats.push_back(cricket::VideoFormat(160, 120, _fpsInterval, _codec));
}


VideoPacketSource::~VideoPacketSource()
{
    LDebug(": Destroying")
}


void VideoPacketSource::setPacketSource(PacketSignal* source)
{
    assert(!_source);
    // if (_source)
    //     _source->detach(packetSlot(this, &VideoPacketSource::onVideoCaptured));
    _source = source;
}


cricket::CaptureState VideoPacketSource::Start(const cricket::VideoFormat& format)
{
    LDebug("Start")

    // NOTE: The requested format must match the input format until
    // we implememnt pixel format conversion and resizing inside
    // this class.
    RTC_CHECK(_captureFormat == format);
    if (capture_state() == cricket::CS_RUNNING) {
        LWarn("Start called when it's already started.")
        return capture_state();
    }

    if (_source)
        _source->attach(packetSlot(this, &VideoPacketSource::onVideoCaptured));

    SetCaptureFormat(&format);
    return cricket::CS_RUNNING;
}


void VideoPacketSource::Stop()
{
    LDebug("Stop")
    if (capture_state() == cricket::CS_STOPPED) {
        LWarn("Stop called when it's already stopped.")
        return;
    }

    if (_source)
        _source->detach(packetSlot(this, &VideoPacketSource::onVideoCaptured));

    SetCaptureFormat(nullptr);
    SetCaptureState(cricket::CS_STOPPED);
}


void VideoPacketSource::onVideoCaptured(av::PlanarVideoPacket& packet)
{
    LTrace("On video frame: ", packet.width, 'x', packet.height)

    assert(packet.width > 0);
    assert(packet.height > 0);

    int adapted_width;
    int adapted_height;
    int crop_width;
    int crop_height;
    int crop_x;
    int crop_y;
    int64_t timestamp;
    int64_t translated_camera_time_us;

#if SCY_WebRTC_USE_DECODER_PTS
    // Set the packet timestamp.
    // Since the stream may not be playing from the beginning we
    // store the first packet timestamp and subtract it from
    // subsequent packets.
    if (!_timestampOffset)
        _timestampOffset = -packet.time;
    timestamp = packet.time + _timestampOffset;

    // NOTE: Initial packet time cannot be 0 for some reason.
    // WebRTC sets the initial packet time to 1000 so we will do the same.
    timestamp += 1000;
#else
     _nextTimestamp += _captureFormat.interval;
     timestamp = _nextTimestamp / rtc::kNumNanosecsPerMicrosec;
#endif

    if (!AdaptFrame(packet.width, packet.height,
        timestamp, //rtc::TimeNanos() / rtc::kNumNanosecsPerMicrosec,
        rtc::TimeMicros(), //0, 0,
        &adapted_width, &adapted_height,
        &crop_width, &crop_height,
        &crop_x, &crop_y, &translated_camera_time_us)) {
        LWarn("Adapt frame failed", packet.time)
        return;
    }

    rtc::scoped_refptr<webrtc::I420Buffer> buffer = webrtc::I420Buffer::Copy(
            packet.width, packet.height,
            packet.buffer[0], packet.linesize[0],
            packet.buffer[1], packet.linesize[1],
            packet.buffer[2], packet.linesize[2]);

    OnFrame(webrtc::VideoFrame(
        buffer, _rotation,
        translated_camera_time_us), // timestamp
        packet.width, packet.height);

#if 0 // Old code pre f5297a0
    cricket::CapturedFrame frame;
    frame.width = packet.width;
    frame.height = packet.height;
    frame.pixel_width = 1;
    frame.pixel_height = 1;
    frame.fourcc = cricket::FOURCC_NV12;
    frame.data = packet.data();
    frame.data_size = packet.size();
    // frame.time_stamp = packet.time; // time in microseconds is ignored

    SignalFrameCaptured(this, &frame);
#endif
}


bool VideoPacketSource::IsRunning()
{
    return capture_state() == cricket::CS_RUNNING;
}


bool VideoPacketSource::GetPreferredFourccs(std::vector<uint32_t>* fourccs)
{
    if (!fourccs)
        return false;

    // This class does not yet support multiple pixel formats.
    fourccs->push_back(_captureFormat.fourcc);
    return true;
}


bool VideoPacketSource::GetBestCaptureFormat(const cricket::VideoFormat& desired, cricket::VideoFormat* best_format)
{
    if (!best_format)
        return false;

    // Use the supported format as the best format.
    // best_format->width = desired.width;
    // best_format->height = desired.height;
    // best_format->fourcc = _codec;
    // best_format->interval = desired.interval;

    *best_format = _captureFormat;
    return true;
}


bool VideoPacketSource::IsScreencast() const
{
    return false;
}


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG


/// @\}
