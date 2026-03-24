///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Device Enumerator
//
// Lists all video/audio capture and output devices with their capabilities,
// available hardware codecs, and FFmpeg input/output formats.
//
// Usage: just run it.
//
/// @addtogroup av
/// @{


#include "icy/av/devicemanager.h"
#include "icy/logger.h"

#include <iostream>


using namespace icy;


void printDeviceList(const char* heading, std::vector<av::Device>& devices)
{
    std::cout << heading << std::endl;
    if (devices.empty()) {
        std::cout << "  None detected" << std::endl;
        return;
    }

    for (auto& dev : devices) {
        std::cout << "  " << dev.name;
        if (dev.isDefault) std::cout << " [default]";
        if (!dev.isConnected) std::cout << " [disconnected]";
        if (dev.isInUse) std::cout << " [in use]";
        std::cout << " (" << dev.id << ")" << std::endl;

        for (auto& cap : dev.videoCapabilities) {
            std::cout << "    " << cap.width << "x" << cap.height
                      << " @ " << cap.minFps << "-" << cap.maxFps << " fps"
                      << " (" << cap.pixelFormat << ")" << std::endl;
        }

        for (auto& cap : dev.audioCapabilities) {
            std::cout << "    " << cap.sampleRate << " Hz, "
                      << cap.channels << " ch"
                      << " (" << cap.sampleFormat << ")" << std::endl;
        }
    }
}


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    {
        av::DeviceManager devman;

        std::vector<av::Device> devices;

        devman.getCameras(devices);
        printDeviceList("=== Cameras ===", devices);

        devman.getMicrophones(devices);
        printDeviceList("\n=== Microphones ===", devices);

        devman.getSpeakers(devices);
        printDeviceList("\n=== Speakers ===", devices);

        // Hardware codecs
        std::cout << "\n=== Hardware Codecs ===" << std::endl;
        auto hwCodecs = devman.getHardwareCodecs();
        if (hwCodecs.empty()) {
            std::cout << "  None detected" << std::endl;
        } else {
            for (auto& codec : hwCodecs) {
                std::cout << "  " << codec.name << " (" << codec.type << ")" << std::endl;
            }
        }

        // FFmpeg formats
        std::cout << "\nInput formats:\n\t";
        av::printInputFormats(std::cout);
        std::cout << "\nOutput formats:\n\t";
        av::printOutputFormats(std::cout);
        std::cout << std::endl;
    }
    Logger::destroy();
    return 0;
}

/// @}
