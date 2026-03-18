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
// Uses negotiateVideoCapture/negotiateAudioCapture to pick the best
// parameters the device actually supports.
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

#include <iostream>


#define OUTPUT_FILENAME "deviceoutput.mp4"

using namespace scy;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    {
        PacketStream stream;
        av::DeviceManager devman;

        // Negotiate best video parameters from the default camera
        auto videoResult = devman.negotiateVideoCapture("", 1280, 720, 30.0);
        auto audioResult = devman.negotiateAudioCapture("", 44100, 2);

        if (!videoResult && !audioResult) {
            std::cerr << "No camera or microphone found" << std::endl;
            return 1;
        }

        // Use negotiated params or sensible defaults for output format
        int vw = 1280, vh = 720;
        double vfps = 30;
        int ach = 2, asr = 44100;

        if (videoResult) {
            vw = videoResult->second.width;
            vh = videoResult->second.height;
            vfps = videoResult->second.maxFps;
        }
        if (audioResult) {
            ach = audioResult->second.channels;
            asr = audioResult->second.sampleRate;
        }

        av::EncoderOptions options;
        options.ofile = OUTPUT_FILENAME;
        options.oformat = av::Format("MP4 Realtime", "mp4",
            {"libx264", vw, vh, vfps, 48000, 128000, "yuv420p"},
            {"aac", ach, asr, 64000, "fltp"});
        options.iformat.audio.enabled = false;
        options.iformat.video.enabled = false;

        // Open video capture
        av::VideoCapture video;
        if (videoResult) {
            auto& [camera, cap] = *videoResult;
            LInfo("Using camera: ", camera.name, " at ",
                  cap.width, "x", cap.height, " @ ", cap.maxFps, " fps");
            video.openVideo(camera.id, cap.width, cap.height, cap.maxFps, cap.pixelFormat);
            video.getEncoderFormat(options.iformat);
            stream.attachSource(&video, false, true);
        }

        // Open audio capture
        av::AudioCapture audio;
        if (audioResult) {
            auto& [mic, cap] = *audioResult;
            LInfo("Using microphone: ", mic.name, " at ",
                  cap.sampleRate, " Hz, ", cap.channels, " ch");
            audio.openAudio(mic.id, {cap.channels, cap.sampleRate});
            audio.getEncoderFormat(options.iformat);
            stream.attachSource(&audio, false, true);
        }

        av::MultiplexPacketEncoder encoder(options);
        encoder.init();
        stream.attach(&encoder, 5, false);

        stream.start();

        LInfo("Recording video: ", OUTPUT_FILENAME);
        waitForShutdown([](void* opaque) {
            reinterpret_cast<PacketStream*>(opaque)->stop();
        },
                        &stream);
    }
    return 0;
}
