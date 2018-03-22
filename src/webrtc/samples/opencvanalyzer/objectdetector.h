///
//
// LibSourcey
// Copyright (c) 2005, Motorola Solutions Inc.
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#ifndef SCY_WebRTC_ObjectDetector_H
#define SCY_WebRTC_ObjectDetector_H

#include "config.h"

#include "scy/base.h"

#ifdef HAVE_FFMPEG

#include "scy/av/av.h"
#include "scy/av/multiplexencoder.h"

#include "api/peerconnectioninterface.h"
#include <opencv/cv.h>
#include <opencv2/dnn.hpp>

#include "json.hpp"
#include "threadsafequeue.h"


namespace scy {
    namespace wrtc {


        class ObjectDetector : public rtc::VideoSinkInterface<webrtc::VideoFrame>
        {
        public:
            ObjectDetector(
                    std::string* networkBin,
                    std::string* networkConfig,
                    float minConfidence,
                    ThreadSafeQueue<nlohmann::json> *detectionQueue);
            ~ObjectDetector();
            void setVideoTrack(webrtc::VideoTrackInterface* track);
            void OnFrame(const webrtc::VideoFrame& frame) override;
        protected:
            rtc::scoped_refptr<webrtc::VideoTrackInterface> _videoTrack;
            cv::dnn::Net _net;
            int _frameNumber;
            float _minConfidence;
            ThreadSafeQueue<nlohmann::json> *_detectionQueue;
        };


    } } // namespace scy::wrtc


#endif // HAVE_FFMPEG
#endif


/// @\}
