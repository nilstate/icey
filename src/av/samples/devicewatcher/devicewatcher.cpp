///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Device Watcher
//
// Demonstrates hot-plug detection and device change monitoring.
// Prints initial device state, then watches for changes until Ctrl-C.
//
// Usage: just run it. Plug/unplug a camera or microphone to see events.
//
/// @addtogroup av
/// @{


#include "icy/application.h"
#include "icy/av/devicemanager.h"
#include "icy/logger.h"

#include <iostream>


using namespace icy;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    {
        av::DeviceManager devman;

        // Print initial device state
        devman.print(std::cout);

        // Connect to device change signal
        devman.DevicesChanged += [&devman]() {
            std::cout << "\n*** Device change detected ***\n" << std::endl;

            std::vector<av::Device> cameras, mics, speakers;
            devman.getCameras(cameras);
            devman.getMicrophones(mics);
            devman.getSpeakers(speakers);

            std::cout << "Cameras: " << cameras.size()
                      << ", Mics: " << mics.size()
                      << ", Speakers: " << speakers.size() << std::endl;

            for (auto& cam : cameras) {
                std::cout << "  " << cam.name;
                if (!cam.isConnected) std::cout << " [disconnected]";
                if (cam.isInUse) std::cout << " [in use]";
                std::cout << std::endl;
            }
        };

        // Start watching for device changes
        devman.watcher()->start();

        std::cout << "\nWatching for device changes. "
                  << "Plug/unplug a camera or microphone. "
                  << "Press Ctrl-C to exit." << std::endl;

        waitForShutdown(nullptr, nullptr);

        devman.watcher()->stop();
    }
    Logger::destroy();
    return 0;
}

/// @}
