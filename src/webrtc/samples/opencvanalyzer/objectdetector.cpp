///
//
// LibSourcey
// Copyright (c) 2017, Motorola Solutions Inc.
//
// SPDX-License-Identifier:	LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#include "objectdetector.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/dnn/shape_utils.hpp>

using namespace cv;
using namespace cv::dnn;

#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "media/engine/webrtcvideocapturerfactory.h"
#include "modules/video_capture/video_capture_factory.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

const size_t inWidth = 300;
const size_t inHeight = 300;
const float WHRatio = inWidth / (float)inHeight;
const float inScaleFactor = 0.007843f;
const float meanVal = 127.5;
const char* classNames[] = {"background",
                            "aeroplane", "bicycle", "bird", "boat",
                            "bottle", "bus", "car", "cat", "chair",
                            "cow", "diningtable", "dog", "horse",
                            "motorbike", "person", "pottedplant",
                            "sheep", "sofa", "train", "tvmonitor"};

namespace scy {
    namespace wrtc {

        ObjectDetector::ObjectDetector(
                std::string* networkBinPath,
                std::string* networkConfigPath,
                float minConfidence,
                ThreadSafeQueue<nlohmann::json> *detectionQueue)
        {
            _net = cv::dnn::readNetFromCaffe(*networkConfigPath, *networkBinPath);
            _minConfidence = minConfidence;
            _detectionQueue = detectionQueue;
        }


        ObjectDetector::~ObjectDetector()
        {
            if (_videoTrack)
                _videoTrack->RemoveSink(this);
        }

        void ObjectDetector::setVideoTrack(webrtc::VideoTrackInterface* track)
        {
            assert(!_videoTrack);
            _videoTrack = track;
            _videoTrack->AddOrUpdateSink(this, rtc::VideoSinkWants());
            _frameNumber = 0;
        }


        void ObjectDetector::OnFrame(const webrtc::VideoFrame& yuvframe)
        {
            LTrace("On video frame: ", yuvframe.width(), 'x', yuvframe.height())

            if (++_frameNumber % PROCESS_EVERY_FRAME_NUMBER == 0) {
                Size cropSize;
                cv::TickMeter timer;
                timer.start();
                int width = yuvframe.width();
                int height = yuvframe.height();

                if (width / (float)height > WHRatio)
                {
                    cropSize = Size(static_cast<int>(height * WHRatio), height);
                }
                else
                {
                    cropSize = Size(width, static_cast<int>(width / WHRatio));
                }

                Rect crop(Point((width - cropSize.width) / 2, (height - cropSize.height) / 2), cropSize);

                // get gray frame
                uint8_t* brightnessData = (uint8_t*)yuvframe.video_frame_buffer()->GetI420()->DataY();

                Mat frame;
                frame.create(height, width, CV_8UC1);
                frame.data = brightnessData;

                cv::cvtColor(frame, frame, CV_GRAY2RGB);

                // convert Mat to batch of images
                Mat inputBlob = blobFromImage(frame, inScaleFactor,
                                              Size(inWidth, inHeight), meanVal, false);

                // set the network input
                _net.setInput(inputBlob, "data");

                // compute output
                Mat detection = _net.forward("detection_out");

                Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

                frame = frame(crop);

                timer.stop();

                bool detected = false;
                nlohmann::json detectionsArrayJson = nlohmann::json::array();

                for(int i = 0; i < detectionMat.rows; i++)
                {
                    float confidence = detectionMat.at<float>(i, 2);
                    if(confidence > _minConfidence)
                    {
                        detected = true;
                        auto objectClass = (size_t)(detectionMat.at<float>(i, 1));
                        auto xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                        auto yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                        auto xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                        auto yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                        ostringstream ss;
                        ss << confidence;
                        nlohmann::json detectionJson;
                        detectionJson["class"] = String(classNames[objectClass]);
                        detectionJson["confidence"] = String(ss.str());

                        nlohmann::json detectionBoxJson;
                        detectionBoxJson["leftBottomX"] = xLeftBottom;
                        detectionBoxJson["leftBottomY"] = yLeftBottom;
                        detectionBoxJson["rightTopX"] = xRightTop;
                        detectionBoxJson["rightTopY"] = yRightTop;

                        detectionJson["box"] = detectionBoxJson;
                        detectionsArrayJson.push_back(detectionJson);

//                        This code is useful for check detections on OpenCV side before sending to client

//                        Rect detectionBox(xLeftBottom,
//                                    yLeftBottom,
//                                    (xRightTop - xLeftBottom),
//                                    (yRightTop - yLeftBottom));
//
//                        rectangle(frame, detectionBox, Scalar(0, 255, 0));
//
//                        int baseLine = 0;
//                        Size labelSize = getTextSize(String(classNames[objectClass]), FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
//                        rectangle(frame, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height),
//                                              Size(labelSize.width, labelSize.height + baseLine)),
//                                  Scalar(255, 255, 255), CV_FILLED);
//                        putText(frame, String(classNames[objectClass]), Point(xLeftBottom, yLeftBottom),
//                                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));

                        std::cout << "Time: " << (double)timer.getTimeMilli() / timer.getCounter() << " ms (average from " << timer.getCounter() << " iterations)" << std::endl;
                    }
                }

                if (detected) {
                    nlohmann::json detectionsJson = {{"detections", detectionsArrayJson}};
                    _detectionQueue->push(detectionsJson);
                }

//                imshow("detections", frame);
//                waitKey(0);
            }
        }
    } } // namespace scy::wrtc


/// @\}
