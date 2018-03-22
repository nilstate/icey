///
//
// LibSourcey
// Copyright (C) 2005, Sourcey <https://sourcey.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef SCY_OpenCVVideoCapturer_H
#define SCY_OpenCVVideoCapturer_H


#include <iostream>
#include <string>

#include "scy/av/mediafactory.h"
#include "scy/av/videocapture.h"

#include "media/base/videocapturer.h"


namespace scy {
namespace wrtc {


class OpenCVVideoCapturer : public cricket::VideoCapturer
/// OpenCVVideoCapturer implements a simple cricket::VideoCapturer which
/// gets decoded remote video frames from media channel.
/// It's used as the remote video source's VideoCapturer so that the remote
/// video can be used as a cricket::VideoCapturer and in that way a remote
/// video stream can implement the MediaStreamSourceInterface.
{
public:
    OpenCVVideoCapturer(int deviceId);
    virtual ~OpenCVVideoCapturer();

    /// cricket::VideoCapturer implementation.
    virtual cricket::CaptureState
    Start(const cricket::VideoFormat& capture_format);
    virtual void Stop();
    virtual bool IsRunning();
    virtual bool GetPreferredFourccs(std::vector<uint32_t>* fourccs);
    virtual bool GetBestCaptureFormat(const cricket::VideoFormat& desired,
                                      cricket::VideoFormat* best_format);
    virtual bool IsScreencast() const;

private:
    void onFrameCaptured(void* sender, av::MatrixPacket& packet);

    av::VideoCapture::Ptr capture;
};


// class OpenCVVideoCapturerFactory : public cricket::VideoDeviceCapturerFactory
// {
// public:
//     OpenCVVideoCapturerFactory() {}
//     virtual ~OpenCVVideoCapturerFactory() {}
//
//     virtual cricket::VideoCapturer* Create(const cricket::Device& device) {
//
//         // XXX: WebRTC uses device name to instantiate the capture, which is
//         always 0.
//         return new OpenCVVideoCapturer(util::strtoi<int>(device.id));
//     }
// };


} } // namespace scy::wrtc


#endif
