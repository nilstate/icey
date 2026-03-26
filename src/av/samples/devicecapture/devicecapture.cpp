///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Device Capture
//
// Opens a camera using capability-aware parameter negotiation and captures
// frames for 5 seconds. Demonstrates using negotiateVideoCapture to pick
// the best device parameters before opening.
//
// Usage: ./devicecapture [camera-name]
//
/// @addtogroup av
/// @{


#include "icy/application.h"
#include "icy/av/devicemanager.h"
#include "icy/av/videocapture.h"
#include "icy/packetstream.h"
#include "icy/logger.h"

#include <chrono>
#include <iostream>
#include <thread>


using namespace icy;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    {
        av::DeviceManager devman;
        std::string deviceName = (argc > 1) ? argv[1] : "";

        // Negotiate best parameters for 720p @ 30fps
        auto result = devman.negotiateVideoCapture(deviceName, 1280, 720, 30.0);

        if (!result) {
            std::cerr << "No camera found";
            if (!deviceName.empty())
                std::cerr << ": " << deviceName;
            std::cerr << std::endl;

            // List available cameras
            std::vector<av::Device> cameras;
            if (devman.getCameras(cameras)) {
                std::cerr << "Available cameras:" << std::endl;
                for (auto& c : cameras)
                    std::cerr << "  " << c.name << std::endl;
            }
            return 1;
        }

        auto& [camera, cap] = *result;

        // Print device info
        std::cout << "Camera: " << camera.name << std::endl;
        if (!camera.videoCapabilities.empty()) {
            std::cout << "Capabilities:" << std::endl;
            for (auto& vc : camera.videoCapabilities) {
                std::cout << "  " << vc.width << "x" << vc.height
                          << " @ " << vc.maxFps << " fps"
                          << " (" << vc.pixelFormat << ")" << std::endl;
            }
        }

        std::cout << "\nOpening at " << cap.width << "x" << cap.height
                  << " @ " << cap.maxFps << " fps"
                  << " (" << cap.pixelFormat << ")" << std::endl;

        // Open capture with negotiated parameters
        av::VideoCapture capture;
        capture.openVideo(camera.id, cap.width, cap.height,
                          cap.maxFps, cap.pixelFormat);

        // Count frames for 5 seconds
        int frameCount = 0;
        PacketStream stream;
        stream.attachSource(&capture, false, true);
        stream.emitter += [&frameCount](IPacket& pkt) {
            frameCount++;
            if (frameCount % 30 == 0)
                std::cout << "  Captured " << frameCount << " frames" << std::endl;
        };

        stream.start();

        std::cout << "Capturing for 5 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        stream.stop();
        std::cout << "Done. Total frames: " << frameCount << std::endl;
    }
    Logger::destroy();
    return 0;
}

/// @}
