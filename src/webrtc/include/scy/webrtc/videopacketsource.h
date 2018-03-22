///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_VideoPacketSource_H
#define SCY_WebRTC_VideoPacketSource_H


#include "scy/base.h"
#include "scy/packetsignal.h"

#ifdef HAVE_FFMPEG

#include "scy/av/packet.h"

#include "media/base/videocapturer.h"


namespace scy {
namespace wrtc {


/// VideoPacketSource implements a simple `cricket::VideoCapturer` that
/// gets decoded remote video frames from a local media channel.
/// It's used as the remote video source's `VideoCapturer` so that the remote
/// video can be used as a `cricket::VideoCapturer` and in that way a remote
/// video stream can implement the `MediaStreamSourceInterface`.
class VideoPacketSource : public cricket::VideoCapturer
{
public:
    VideoPacketSource(int width, int height, int fps, uint32_t fourcc);
    VideoPacketSource(const cricket::VideoFormat& captureFormat);
    virtual ~VideoPacketSource();

    /// Set the source `av::VideoPacket` emitter.
    /// The pointer is not managed by this class.
    void setPacketSource(PacketSignal* source);

    /// Callback that fired when an `av::PlanarVideoPacket`
    /// is ready for processing.
    void onVideoCaptured(av::PlanarVideoPacket& packet);

    /// cricket::VideoCapturer implementation.
    virtual cricket::CaptureState Start(const cricket::VideoFormat& capture_format) override;
    virtual void Stop() override;
    virtual bool GetPreferredFourccs(std::vector<uint32_t>* fourccs) override;
    virtual bool GetBestCaptureFormat(const cricket::VideoFormat& desired,
                                      cricket::VideoFormat* best_format) override;
    virtual bool IsRunning() override;
    virtual bool IsScreencast() const override;

protected:
    cricket::VideoFormat _captureFormat;
    webrtc::VideoRotation _rotation;
    int64_t _timestampOffset;
    int64_t _nextTimestamp;
    PacketSignal* _source;
};


// class VideoPacketSourceFactory : public cricket::VideoDeviceCapturerFactory {
// public:
//     VideoPacketSourceFactory() {}
//     virtual ~VideoPacketSourceFactory() {}
//
//     virtual cricket::VideoCapturer* Create(const cricket::Device& device) {
//         return new VideoPacketSource(device.name);
//     }
// };


} } // namespace scy::wrtc


#endif // HAVE_FFMPEG
#endif


/// @\}
