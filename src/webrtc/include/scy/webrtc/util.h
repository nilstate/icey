///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
///
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_WEBRTC_UTIL_H
#define SCY_WebRTC_WEBRTC_UTIL_H


#include "scy/webrtc/webrtc.h"

#include "webrtc/media/base/videocapturer.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/video_capture_factory.h"


namespace scy {
namespace wrtc {


std::vector<std::string> getVideoCaptureDevices();

std::unique_ptr<cricket::VideoCapturer> openWebRtcVideoCaptureDevice(const std::string& deviceName = "");


} } // namespace scy::wrtc


#endif // SCY_WebRTC_WEBRTC_H


/// @\}
