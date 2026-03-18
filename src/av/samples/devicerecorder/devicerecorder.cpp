///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Device Recorder
//
// Records H.264 video + AAC audio from the default camera and microphone
// to an MP4 file using the PacketStream pipeline.
//
// Requires a camera and/or microphone to be connected. For a device-free
// alternative, see the filetranscode sample.
//
// Usage: just run it. Press Ctrl-C to stop recording.
// Output: deviceoutput.mp4
//
/// @addtogroup av
/// @{


#include "scy/application.h"
#include "scy/av/audiocapture.h"
#include "scy/av/devicemanager.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/av/videocapture.h"
#include "scy/packetstream.h"


// This demo showcases how to implement a H.264 multiplex recorder from realtime
// device captures using LibSourcey.

#define OUTPUT_FILENAME "deviceoutput.mp4"
#define OUTPUT_FORMAT av::Format("MP4 Realtime", "mp4",                               \
                                 {"libx264", 400, 300, 25, 48000, 128000, "yuv420p"}, \
                                 {"aac", 2, 44100, 64000, "fltp"});

using namespace scy;
using std::endl;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace)); // Debug
    {
        // Create a PacketStream to pass packets
        // from device captures to the encoder
        PacketStream stream;

        // Output format is set explicitly; input format will be populated
        // from the capture devices so the encoder knows the source parameters
        av::EncoderOptions options;
        options.ofile = OUTPUT_FILENAME;
        options.oformat = OUTPUT_FORMAT;
        options.iformat.audio.enabled = false; // enabled below if device found
        options.iformat.video.enabled = false; // enabled below if device found

        // Create a device manager instance to enumerate system devices
        av::Device device;
        av::DeviceManager devman;

        // Create and attach the default video capture
        av::VideoCapture video;
        if (devman.getDefaultCamera(device)) {
            LInfo("Using video device: ", device.name);
            video.openVideo(device.id, {640, 480});
            video.getEncoderFormat(options.iformat); // populate input format from capture
            stream.attachSource(&video, false, true);
        }

        // Create and attach the default audio capture
        av::AudioCapture audio;
        if (devman.getDefaultMicrophone(device)) {
            LInfo("Using audio device: ", device.name);
            audio.openAudio(device.id, {2, 44100});
            audio.getEncoderFormat(options.iformat);
            stream.attachSource(&audio, false, true);
        }

        // Create and attach the multiplex encoder
        av::MultiplexPacketEncoder encoder(options);
        encoder.init();
        stream.attach(&encoder, 5, false); // priority 5: runs after capture sources

        // Start the stream
        stream.start();

        // Keep recording until Ctrl-C is pressed
        LInfo("Recording video: ", OUTPUT_FILENAME);
        waitForShutdown([](void* opaque) {
            reinterpret_cast<PacketStream*>(opaque)->stop();
        },
                        &stream);
    }

    // Logger::destroy();
    return 0;
}
