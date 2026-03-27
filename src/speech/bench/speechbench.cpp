///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//

#include "benchutil.h"
#include "icy/speech/voiceactivitydetector.h"

#ifdef HAVE_FFMPEG
#include <vector>
#endif


using namespace icy;

namespace {

#ifdef HAVE_FFMPEG

struct BenchAudioPacket
{
    std::vector<float> left;
    std::vector<float> right;
    uint8_t* buffers[4] = {nullptr, nullptr, nullptr, nullptr};
    av::PlanarAudioPacket packet;

    BenchAudioPacket()
        : left(480, 0.10f)
        , right(480, 0.10f)
        , packet(buffers, 2, left.size(), "fltp", 0)
    {
        buffers[0] = reinterpret_cast<uint8_t*>(left.data());
        buffers[1] = reinterpret_cast<uint8_t*>(right.data());
        packet.buffer[0] = buffers[0];
        packet.buffer[1] = buffers[1];
        packet.channels = 2;
        packet.sampleFmt = "fltp";
    }
};

#endif

} // namespace


int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);

#ifdef HAVE_FFMPEG
    speech::VoiceActivityDetector detector({
        .source = "bench/mic",
        .sampleRateHint = 48000,
        .startThreshold = 0.05f,
        .stopThreshold = 0.02f,
        .minSilenceUsec = 20000,
        .updateIntervalUsec = 20000,
    });

    BenchAudioPacket audio;
    uint64_t emitted = 0;
    detector.Event += [&](const speech::SpeechEvent&) {
        ++emitted;
    };

    constexpr uint64_t iterations = 100000;
    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        emitted = 0;
        detector.reset();
        for (uint64_t index = 0; index < iterations; ++index) {
            audio.packet.time = static_cast<int64_t>(index) * 10000;
            detector.process(audio.packet);
        }
    });

    reporter.add({
        .name = "speech energy vad fltp",
        .measurement = measurement,
        .metrics = {
            bench::metric("num_samples", static_cast<uint64_t>(audio.packet.numSamples)),
            bench::metric("channels", static_cast<uint64_t>(audio.packet.channels)),
            bench::metric("events", emitted),
        },
    });
#else
    reporter.add({
        .name = "speech energy vad unavailable",
        .measurement = {},
        .metrics = {
            bench::metric("path", std::string("no_ffmpeg")),
        },
    });
#endif

    return reporter.finish();
}
