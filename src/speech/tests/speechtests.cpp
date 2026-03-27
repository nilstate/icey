///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//

#include "icy/speech/event.h"
#include "icy/speech/speechqueue.h"
#include "icy/speech/voiceactivitydetector.h"
#include "icy/test.h"

#include <atomic>
#include <vector>


using namespace icy;
using namespace icy::test;

namespace {

#ifdef HAVE_FFMPEG

struct TestAudioPacket
{
    std::vector<float> left;
    std::vector<float> right;
    uint8_t* buffers[4] = {nullptr, nullptr, nullptr, nullptr};
    av::PlanarAudioPacket packet;

    TestAudioPacket(float amplitude, int64_t timeUsec, size_t numSamples = 480)
        : left(numSamples, amplitude)
        , right(numSamples, amplitude)
        , packet(buffers, 2, numSamples, "fltp", timeUsec)
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
    test::init();

    describe("speech event serializes to json", []() {
        speech::SpeechEvent event;
        event.type = "speech:start";
        event.source = "microphone/main";
        event.detector = "energy_vad";
        event.emittedAtUsec = 42000;
        event.audio = {
            .sequence = 9,
            .timeUsec = 32000,
            .durationUsec = 10000,
            .sampleRate = 48000,
            .channels = 2,
            .numSamples = 480,
            .sampleFmt = "fltp",
        };
        event.active = true;
        event.level = 0.22f;
        event.peak = 0.44f;
        event.data["speaker"] = "operator";

        const auto json = speech::toJson(event);
        expect(json["type"] == "speech:start");
        expect(json["source"] == "microphone/main");
        expect(json["detector"] == "energy_vad");
        expect(json["audio"]["sequence"] == 9);
        expect(json["audio"]["sampleRate"] == 48000);
        expect(json["active"] == true);
        expect(json["level"] == 0.22f);
        expect(json["peak"] == 0.44f);
        expect(json["data"]["speaker"] == "operator");
    });

    describe("speech queue clones packets at the async boundary", []() {
#ifdef HAVE_FFMPEG
        speech::SpeechQueue queue(8);
        expect(queue.retention() == PacketRetention::Cloned);

        std::atomic<int> received{-1};
        queue.emitter += [&](IPacket& packet) {
            auto& audio = static_cast<av::PlanarAudioPacket&>(packet);
            received = static_cast<int>(reinterpret_cast<float*>(audio.buffer[0])[0] * 1000.0f);
        };

        TestAudioPacket audio(0.25f, 10000);
        queue.process(audio.packet);
        audio.left[0] = 0.75f;

        expect(waitFor([&]() {
            return received.load() == 250;
        }, 2000));

        queue.close();
#endif
    });

    describe("voice activity detector emits start update and end", []() {
#ifdef HAVE_FFMPEG
        speech::VoiceActivityDetector detector({
            .source = "mic/front",
            .sampleRateHint = 48000,
            .startThreshold = 0.05f,
            .stopThreshold = 0.02f,
            .minSilenceUsec = 20000,
            .updateIntervalUsec = 15000,
        });

        std::vector<speech::SpeechEvent> events;
        detector.Event += [&](const speech::SpeechEvent& event) {
            events.push_back(event);
        };

        TestAudioPacket start(0.10f, 0);
        TestAudioPacket hold1(0.10f, 10000);
        TestAudioPacket hold2(0.10f, 20000);
        TestAudioPacket quiet1(0.0f, 30000);
        TestAudioPacket quiet2(0.0f, 60000);

        detector.process(start.packet);
        detector.process(hold1.packet);
        detector.process(hold2.packet);
        detector.process(quiet1.packet);
        detector.process(quiet2.packet);

        expect(events.size() == 3);
        expect(events[0].type == "speech:start");
        expect(events[0].active == true);
        expect(events[0].source == "mic/front");
        expect(events[0].audio.sampleRate == 48000);
        expect(events[1].type == "speech:update");
        expect(events[1].active == true);
        expect(events[2].type == "speech:end");
        expect(events[2].active == false);

        const auto stats = detector.stats();
        expect(stats.seen == 5);
        expect(stats.emitted == 3);
        expect(stats.active == false);
        expect(stats.lastLevel == 0.0f);
#endif
    });

    runAll();
    return finalize();
}
