///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//

#include "icy/packet.h"
#include "icy/packetstream.h"
#include "icy/vision/framepacket.h"
#include "icy/vision/framesampler.h"
#include "icy/vision/motiondetector.h"

#ifdef HAVE_FFMPEG
#include "icy/av/videocontext.h"

extern "C" {
#include <libavutil/pixfmt.h>
}
#endif

#include "benchutil.h"

#include <array>
#include <cstdint>
#include <cstring>


using namespace icy;

namespace {

struct BenchSource : PacketStreamAdapter
{
    PacketSignal emitter;

    BenchSource()
        : PacketStreamAdapter(emitter)
    {
    }

    void push(IPacket& packet)
    {
        emitter.emit(packet);
    }
};

#ifdef HAVE_FFMPEG
struct BenchPlanarFrame
{
    av::AVFrameHolder frame;
    av::PlanarVideoPacket packet;

    BenchPlanarFrame()
        : frame(av::createVideoFrame(AV_PIX_FMT_YUV420P, 640, 360))
        , packet(frame->data, frame->linesize, "yuv420p", 640, 360, 33333)
    {
        std::memset(frame->data[0], 42, static_cast<size_t>(frame->linesize[0]) * 360);
        std::memset(frame->data[1], 96, static_cast<size_t>(frame->linesize[1]) * 180);
        std::memset(frame->data[2], 160, static_cast<size_t>(frame->linesize[2]) * 180);
    }
};
#endif

 #ifdef HAVE_FFMPEG
void runSamplerBenchmark(bench::Reporter& reporter,
                         const char* name,
                         vision::FrameSamplerConfig config)
{
    PacketStream stream(name);
    BenchSource source;
    auto sampler = std::make_shared<vision::FrameSampler>(config);
    uint64_t forwarded = 0;

    stream.attachSource(&source, false, false);
    stream.attach(sampler, 1);
    stream.emitter += [&](IPacket&) { ++forwarded; };
    stream.start();

    constexpr uint64_t iterations = 100000;
    BenchPlanarFrame frame;
    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        forwarded = 0;
        sampler->reset();
        for (uint64_t index = 0; index < iterations; ++index) {
            frame.packet.time = static_cast<int64_t>(index) * 33333;
            source.push(frame.packet);
        }
    });

    reporter.add({
        .name = name,
        .measurement = measurement,
        .metrics = {
            bench::metric("forwarded", forwarded),
            bench::metric("width", static_cast<uint64_t>(frame.packet.width)),
            bench::metric("height", static_cast<uint64_t>(frame.packet.height)),
            bench::metric("every_n", static_cast<uint64_t>(config.everyNthFrame)),
        },
    });
    stream.close();
}
#endif

} // namespace


int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);

#ifdef HAVE_FFMPEG
    runSamplerBenchmark(reporter, "vision sampler forward every frame", {
        .everyNthFrame = 1,
        .minIntervalUsec = 0,
    });
    runSamplerBenchmark(reporter, "vision sampler every fourth frame", {
        .everyNthFrame = 4,
        .minIntervalUsec = 0,
    });
    runSamplerBenchmark(reporter, "vision sampler 50ms gate", {
        .everyNthFrame = 1,
        .minIntervalUsec = 50000,
    });

    {
        vision::MotionDetector detector({
            .source = "bench/camera",
            .gridWidth = 16,
            .gridHeight = 9,
            .warmupFrames = 1,
            .threshold = 0.05f,
            .minEventIntervalUsec = 0,
        });
        BenchPlanarFrame frame;
        uint64_t emitted = 0;
        detector.Event += [&](const vision::VisionEvent&) {
            ++emitted;
        };

        constexpr uint64_t iterations = 100000;
        const auto measurement = bench::measure(reporter.options(), iterations, [&] {
            emitted = 0;
            detector.reset();
            for (uint64_t index = 0; index < iterations; ++index) {
                frame.frame->data[0][0] = static_cast<uint8_t>(index & 0xffu);
                frame.packet.time = static_cast<int64_t>(index) * 33333;
                auto packet = vision::VisionFramePacket(
                    frame.packet,
                    vision::makeVisionFrameContext(
                        frame.packet,
                        index + 1,
                        "bench/camera",
                        "bench/stream",
                        frame.packet.time + 1000));
                detector.process(packet);
            }
        });

        reporter.add({
            .name = "vision motion detector",
            .measurement = measurement,
            .metrics = {
                bench::metric("grid_width", static_cast<uint64_t>(detector.config().gridWidth)),
                bench::metric("grid_height", static_cast<uint64_t>(detector.config().gridHeight)),
                bench::metric("events", emitted),
            },
        });
    }
#else
    reporter.add({
        .name = "vision sampler unavailable",
        .measurement = {},
        .metrics = {
            bench::metric("path", std::string("no_ffmpeg")),
        },
    });
#endif

    return reporter.finish();
}
