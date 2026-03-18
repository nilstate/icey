///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// File Transcoder
//
// Demonstrates the AV pipeline for file-to-file transcoding. No capture
// devices are required, making this sample runnable on any machine.
//
// The pipeline is: file source -> MultiplexPacketEncoder -> output file.
//
// Usage: filetranscode [input] [output]
//   Defaults: input = data/test.mp4, output = output.mp4
//
/// @addtogroup av
/// @{


#include "scy/application.h"
#include "scy/av/mediacapture.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/logger.h"
#include "scy/packetstream.h"

#include <iostream>
#include <string>


using namespace scy;


int main(int argc, char** argv)
{
    std::string inputFile = argc > 1 ? argv[1] : SCY_DATA_DIR "/test.mp4";
    std::string outputFile = argc > 2 ? argv[2] : "output.mp4";

    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));
    {
        // Open the input file
        av::MediaCapture capture;
        capture.openFile(inputFile);

        // Build encoder options: read input format from capture,
        // set output to H.264 + AAC in MP4 container
        av::EncoderOptions options;
        options.ofile = outputFile;
        options.oformat = av::Format("MP4", "mp4",
            av::VideoCodec("libx264", 0, 0, 0),    // 0 = match input dimensions/fps
            av::AudioCodec("aac", 0, 0, 0));        // 0 = match input channels/rate
        capture.getEncoderFormat(options.iformat);

        // Wire the pipeline: capture -> encoder
        PacketStream stream;
        stream.attachSource(&capture, false, true);

        av::MultiplexPacketEncoder encoder(options);
        encoder.init();
        stream.attach(&encoder, 5, false);

        // Start transcoding
        stream.start();
        capture.start();

        std::cout << "Transcoding: " << inputFile << " -> " << outputFile << '\n';

        // Wait for the capture to finish reading the file
        waitForShutdown([](void* opaque) {
            reinterpret_cast<PacketStream*>(opaque)->stop();
        }, &stream);
    }

    std::cout << "Done." << '\n';
    Logger::destroy();
    return 0;
}
