///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Device Enumerator
//
// Lists all video/audio capture devices and available FFmpeg input/output
// formats. No devices need to be connected; the format listing works on
// any machine with FFmpeg compiled in.
//
// Usage: just run it.
//
/// @addtogroup av
/// @{


#include "scy/av/devicemanager.h"
#include "scy/logger.h"


using namespace scy;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    {
        // List all video/audio capture devices detected by FFmpeg
        av::DeviceManager devman;
        devman.print(std::cout);

        // List FFmpeg demuxers (input) and muxers (output) compiled into this build
        std::cout << "Input formats:\n\t";
        av::printInputFormats(std::cout);
        std::cout << '\n';

        std::cout << "Output formats:\n\t";
        av::printOutputFormats(std::cout);
        std::cout << '\n';
    }
    Logger::destroy();
    return 0;
}
