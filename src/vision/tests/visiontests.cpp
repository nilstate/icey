///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//

#include "icy/test.h"
#include "icy/vision/detectionqueue.h"
#include "icy/vision/event.h"
#include "icy/vision/framesampler.h"
#include "icy/vision/motiondetector.h"

#ifdef HAVE_FFMPEG
#include "icy/av/videocontext.h"

extern "C" {
#include <libavutil/pixfmt.h>
}
#endif

#include <atomic>
#include <vector>


using namespace icy;
using namespace icy::test;

namespace {

#ifdef HAVE_FFMPEG

struct TestPlanarFrame
{
    av::AVFrameHolder frame;
    av::PlanarVideoPacket packet;

    TestPlanarFrame(int width, int height, int64_t timeUsec, uint8_t luma)
        : frame(av::createVideoFrame(AV_PIX_FMT_YUV420P, width, height))
        , packet(frame->data, frame->linesize, "yuv420p", width, height, timeUsec)
    {
        std::memset(frame->data[0], luma, static_cast<size_t>(frame->linesize[0]) * height);
        std::memset(frame->data[1], 96, static_cast<size_t>(frame->linesize[1]) * ((height + 1) / 2));
        std::memset(frame->data[2], 160, static_cast<size_t>(frame->linesize[2]) * ((height + 1) / 2));
    }
};

#endif

} // namespace


int main(int argc, char** argv)
{
    test::init();

    describe("vision event serializes to json", []() {
        vision::VisionEvent event;
        event.type = "detect";
        event.source = "camera/front";
        event.detector = "motion";
        event.emittedAtUsec = 424242;
        event.frame = {
            .sequence = 7,
            .timeUsec = 33333,
            .width = 1280,
            .height = 720,
            .pixelFmt = "yuv420p",
        };

        vision::Detection detection;
        detection.label = "person";
        detection.confidence = 0.98f;
        detection.trackId = 12;
        detection.region = {
            .x = 0.1f,
            .y = 0.2f,
            .width = 0.3f,
            .height = 0.4f,
        };
        detection.data["zone"] = "entry";
        event.detections.push_back(detection);
        event.data["phase"] = "warm";

        const auto json = vision::toJson(event);
        expect(json["type"] == "detect");
        expect(json["source"] == "camera/front");
        expect(json["detector"] == "motion");
        expect(json["time"] == 424242);
        expect(json["frame"]["sequence"] == 7);
        expect(json["frame"]["pixelFmt"] == "yuv420p");
        expect(json["detections"].is_array());
        expect(json["detections"].size() == 1);
        expect(json["detections"][0]["label"] == "person");
        expect(json["detections"][0]["trackId"] == 12);
        expect(json["detections"][0]["data"]["zone"] == "entry");
        expect(json["data"]["phase"] == "warm");
    });

    describe("vision frame sampler respects nth and interval filters", []() {
#ifdef HAVE_FFMPEG
        vision::FrameSampler sampler({.everyNthFrame = 2});
        std::vector<int64_t> forwardedTimes;
        sampler.emitter += [&](IPacket& packet) {
            forwardedTimes.push_back(static_cast<av::PlanarVideoPacket&>(packet).time);
        };

        std::vector<TestPlanarFrame> nthFrames;
        for (int index = 0; index < 5; ++index) {
            nthFrames.emplace_back(64, 48, static_cast<int64_t>(index) * 10000, static_cast<uint8_t>(index));
            sampler.process(nthFrames.back().packet);
        }

        auto nthStats = sampler.stats();
        expect(nthStats.seen == 5);
        expect(nthStats.forwarded == 3);
        expect(nthStats.dropped == 2);
        expect(forwardedTimes.size() == 3);
        expect(forwardedTimes[0] == 0);
        expect(forwardedTimes[1] == 20000);
        expect(forwardedTimes[2] == 40000);

        sampler.setConfig({.everyNthFrame = 1, .minIntervalUsec = 20000});
        forwardedTimes.clear();

        std::vector<TestPlanarFrame> intervalFrames;
        intervalFrames.emplace_back(64, 48, 0, 11);
        intervalFrames.emplace_back(64, 48, 10000, 22);
        intervalFrames.emplace_back(64, 48, 25000, 33);
        intervalFrames.emplace_back(64, 48, 35000, 44);

        for (auto& frame : intervalFrames)
            sampler.process(frame.packet);

        auto intervalStats = sampler.stats();
        expect(intervalStats.seen == 4);
        expect(intervalStats.forwarded == 2);
        expect(intervalStats.dropped == 2);
        expect(forwardedTimes.size() == 2);
        expect(forwardedTimes[0] == 0);
        expect(forwardedTimes[1] == 25000);
#endif
    });

    describe("vision detection queue clones frames at the async boundary", []() {
#ifdef HAVE_FFMPEG
        vision::DetectionQueue queue(8);
        expect(queue.retention() == PacketRetention::Cloned);
        std::atomic<int> received{-1};
        queue.emitter += [&](IPacket& packet) {
            auto& frame = static_cast<av::PlanarVideoPacket&>(packet);
            received = frame.buffer[0][0];
        };

        TestPlanarFrame frame(64, 48, 33333, 17);
        queue.process(frame.packet);
        frame.frame->data[0][0] = 99;

        expect(waitFor([&]() {
            return received.load() == 17;
        }, 2000));

        queue.close();
#endif
    });

    describe("vision motion detector emits a detection when frame luma changes", []() {
#ifdef HAVE_FFMPEG
        vision::MotionDetector detector({
            .source = "camera/front",
            .gridWidth = 8,
            .gridHeight = 8,
            .warmupFrames = 1,
            .threshold = 0.10f,
            .minEventIntervalUsec = 0,
        });

        std::vector<vision::VisionEvent> events;
        detector.Event += [&](const vision::VisionEvent& event) {
            events.push_back(event);
        };

        TestPlanarFrame baseline(64, 48, 0, 16);
        TestPlanarFrame changed(64, 48, 33333, 220);
        TestPlanarFrame still(64, 48, 66666, 220);

        detector.process(baseline.packet);
        detector.process(changed.packet);
        detector.process(still.packet);

        expect(events.size() == 1);
        expect(events[0].source == "camera/front");
        expect(events[0].detector == "motion");
        expect(events[0].detections.size() == 1);
        expect(events[0].detections[0].label == "motion");
        expect(events[0].detections[0].confidence > 0.10f);

        const auto stats = detector.stats();
        expect(stats.seen == 3);
        expect(stats.emitted == 1);
#endif
    });

    runAll();
    return finalize();
}
