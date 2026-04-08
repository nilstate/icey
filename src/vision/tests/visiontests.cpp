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
#include "icy/vision/framenormalizer.h"
#include "icy/vision/framepacket.h"
#include "icy/vision/framesampler.h"
#include "icy/vision/motiondetector.h"

#ifdef HAVE_FFMPEG
#include "icy/av/videocontext.h"

extern "C" {
#include <libavutil/pixfmt.h>
}
#endif

#include <atomic>
#include <mutex>
#include <thread>
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


vision::VisionFramePacket makeVisionFramePacket(const av::PlanarVideoPacket& packet,
                                                uint64_t frameId = 0,
                                                const std::string& sourceId = "camera/front",
                                                const std::string& streamId = "stream/front",
                                                int64_t receivedAtUsec = 0)
{
    if (receivedAtUsec == 0)
        receivedAtUsec = packet.time + 1000;
    return vision::VisionFramePacket(
        packet,
        vision::makeVisionFrameContext(packet, frameId, sourceId, streamId, receivedAtUsec));
}


class GatedVisionQueue : public vision::DetectionQueue
{
public:
    explicit GatedVisionQueue(int maxFrames = 8)
        : vision::DetectionQueue(maxFrames)
    {
    }

    std::atomic<bool> enteredDispatch{false};
    std::atomic<bool> releaseDispatch{false};

protected:
    void dispatch(vision::VisionFramePacket& packet) override
    {
        enteredDispatch = true;
        while (!releaseDispatch.load())
            icy::sleep(1);

        vision::DetectionQueue::dispatch(packet);
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
            .sourceId = "camera/front",
            .streamId = "stream/front",
            .frameId = 7,
            .ptsUsec = 33333,
            .receivedAtUsec = 35000,
            .width = 1280,
            .height = 720,
            .pixelFmt = "yuv420p",
            .keyframe = true,
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

        vision::Track track;
        track.trackId = 12;
        track.label = "person";
        track.confidence = 0.98f;
        track.region = detection.region;
        track.data["state"] = "active";
        event.tracks.push_back(track);
        event.data["phase"] = "warm";

        const auto json = vision::toJson(event);
        expect(json["schema"] == "vision.event.v1");
        expect(json["type"] == "detect");
        expect(json["source"] == "camera/front");
        expect(json["detector"] == "motion");
        expect(json["emittedAtUsec"] == 424242);
        expect(json["frame"]["sourceId"] == "camera/front");
        expect(json["frame"]["streamId"] == "stream/front");
        expect(json["frame"]["frameId"] == 7);
        expect(json["frame"]["ptsUsec"] == 33333);
        expect(json["frame"]["receivedAtUsec"] == 35000);
        expect(json["frame"]["pixelFmt"] == "yuv420p");
        expect(json["frame"]["keyframe"] == true);
        expect(json["detections"].is_array());
        expect(json["detections"].size() == 1);
        expect(json["detections"][0]["label"] == "person");
        expect(json["detections"][0]["trackId"] == 12);
        expect(json["detections"][0]["data"]["zone"] == "entry");
        expect(json["tracks"].is_array());
        expect(json["tracks"].size() == 1);
        expect(json["tracks"][0]["trackId"] == 12);
        expect(json["tracks"][0]["data"]["state"] == "active");
        expect(json["data"]["phase"] == "warm");
        expect(json.dump().find("\"schema\":\"vision.event.v1\"") != std::string::npos);
    });

    describe("vision event emitter forwards events", []() {
        vision::VisionEventEmitter emitter;
        bool invoked = false;

        emitter.Event += [&](const vision::VisionEvent& event) {
            invoked = event.schema == "vision.event.v1" &&
                      event.type == "detect";
        };

        emitter.emit(vision::VisionEvent{});
        expect(invoked);
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
        std::atomic<uint64_t> receivedFrameId{0};
        queue.emitter += [&](IPacket& packet) {
            auto& frame = static_cast<vision::VisionFramePacket&>(packet);
            received = frame.buffer[0][0];
            receivedFrameId = frame.context.frameId;
        };

        TestPlanarFrame frame(64, 48, 33333, 17);
        auto packet = makeVisionFramePacket(frame.packet, 9);
        queue.process(packet);
        frame.frame->data[0][0] = 99;

        expect(waitFor([&]() {
            return received.load() == 17;
        }, 2000));
        expect(receivedFrameId.load() == 9);

        queue.close();
#endif
    });

    describe("vision frame normalizer attaches stable metadata without conversion", []() {
#ifdef HAVE_FFMPEG
        vision::FrameNormalizer normalizer({
            .sourceId = "camera/front",
            .streamId = "stream/front",
            .pixelFmt = "",
        });
        std::vector<vision::VisionFramePacket> emitted;
        normalizer.emitter += [&](IPacket& packet) {
            emitted.push_back(static_cast<vision::VisionFramePacket&>(packet));
        };

        TestPlanarFrame frame(64, 48, 33333, 44);
        normalizer.process(frame.packet);
        frame.frame->data[0][0] = 99;

        expect(emitted.size() == 1);
        expect(emitted[0].width == 64);
        expect(emitted[0].height == 48);
        expect(emitted[0].context.sourceId == "camera/front");
        expect(emitted[0].context.streamId == "stream/front");
        expect(emitted[0].context.frameId == 1);
        expect(emitted[0].context.ptsUsec == 33333);
        expect(emitted[0].context.receivedAtUsec > 0);
        expect(emitted[0].context.pixelFmt == "yuv420p");
        expect(emitted[0].buffer[0][0] == 44);

        const auto stats = normalizer.stats();
        expect(stats.seen == 1);
        expect(stats.emitted == 1);
        expect(stats.dropped == 0);
        expect(stats.converted == 0);
#endif
    });

    describe("vision frame normalizer resizes detector input when requested", []() {
#ifdef HAVE_FFMPEG
        vision::FrameNormalizer normalizer({
            .sourceId = "camera/front",
            .streamId = "stream/front",
            .width = 32,
            .height = 24,
            .pixelFmt = "yuv420p",
        });
        std::vector<vision::VisionFramePacket> emitted;
        normalizer.emitter += [&](IPacket& packet) {
            emitted.push_back(static_cast<vision::VisionFramePacket&>(packet));
        };

        TestPlanarFrame frame(64, 48, 44444, 88);
        normalizer.process(frame.packet);

        expect(emitted.size() == 1);
        expect(emitted[0].width == 32);
        expect(emitted[0].height == 24);
        expect(emitted[0].context.width == 32);
        expect(emitted[0].context.height == 24);
        expect(emitted[0].context.pixelFmt == "yuv420p");
        expect(emitted[0].context.frameId == 1);

        const auto stats = normalizer.stats();
        expect(stats.seen == 1);
        expect(stats.emitted == 1);
        expect(stats.converted == 1);
#endif
    });

    describe("vision detection queue drops stale normalized frames first", []() {
#ifdef HAVE_FFMPEG
        GatedVisionQueue queue(1);
        std::mutex mutex;
        std::vector<uint64_t> delivered;
        queue.emitter += [&](IPacket& packet) {
            std::lock_guard<std::mutex> guard(mutex);
            delivered.push_back(static_cast<vision::VisionFramePacket&>(packet).context.frameId);
        };

        TestPlanarFrame frame1(64, 48, 0, 16);
        TestPlanarFrame frame2(64, 48, 33333, 32);
        TestPlanarFrame frame3(64, 48, 66666, 48);
        auto packet1 = makeVisionFramePacket(frame1.packet, 1);
        auto packet2 = makeVisionFramePacket(frame2.packet, 2);
        auto packet3 = makeVisionFramePacket(frame3.packet, 3);

        queue.process(packet1);
        expect(waitFor([&]() {
            return queue.enteredDispatch.load();
        }, 2000));

        queue.process(packet2);
        queue.process(packet3);
        queue.releaseDispatch = true;

        expect(waitFor([&]() {
            std::lock_guard<std::mutex> guard(mutex);
            return delivered.size() == 2;
        }, 2000));

        queue.close();

        std::lock_guard<std::mutex> guard(mutex);
        expect(delivered.size() == 2);
        expect(delivered[0] == 1);
        expect(delivered[1] == 3);
        expect(queue.dropped() == 1);
#endif
    });

    describe("vision detection queue drops blocked work during close", []() {
#ifdef HAVE_FFMPEG
        GatedVisionQueue queue(4);
        std::atomic<int> received{0};
        queue.emitter += [&](IPacket&) {
            ++received;
        };

        TestPlanarFrame frame(64, 48, 0, 16);
        auto packet = makeVisionFramePacket(frame.packet, 1);
        queue.process(packet);

        expect(waitFor([&]() {
            return queue.enteredDispatch.load();
        }, 2000));

        std::atomic<bool> closeReturned{false};
        std::thread closer([&]() {
            queue.close();
            closeReturned = true;
        });

        icy::sleep(25);
        queue.releaseDispatch = true;

        expect(waitFor([&]() {
            return closeReturned.load();
        }, 2000));

        closer.join();
        expect(received.load() == 0);
#endif
    });

    describe("vision motion detector emits a detection when normalized frame luma changes", []() {
#ifdef HAVE_FFMPEG
        vision::MotionDetector detector({
            .source = "camera/front",
            .streamId = "stream/front",
            .gridWidth = 8,
            .gridHeight = 8,
            .warmupFrames = 1,
            .threshold = 0.10f,
            .minEventIntervalUsec = 0,
        });

        TestPlanarFrame baseline(64, 48, 0, 16);
        TestPlanarFrame changed(64, 48, 33333, 220);
        TestPlanarFrame still(64, 48, 66666, 220);
        auto baselinePacket = makeVisionFramePacket(baseline.packet, 1);
        auto changedPacket = makeVisionFramePacket(changed.packet, 2);
        auto stillPacket = makeVisionFramePacket(still.packet, 3);

        std::vector<vision::VisionEvent> events;
        detector.Event += [&](const vision::VisionEvent& event) {
            events.push_back(event);
        };

        detector.process(baselinePacket);
        detector.process(changedPacket);
        detector.process(stillPacket);

        expect(events.size() == 1);
        expect(events[0].source == "camera/front");
        expect(events[0].detector == "motion");
        expect(events[0].frame.sourceId == "camera/front");
        expect(events[0].frame.streamId == "stream/front");
        expect(events[0].frame.frameId == 2);
        expect(events[0].frame.ptsUsec == 33333);
        expect(events[0].emittedAtUsec == 34333);
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
