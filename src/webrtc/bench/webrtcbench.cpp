#include "icy/loop.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/webrtc/peersession.h"

#include "icy/av/audiopacketencoder.h"
#include "icy/av/packet.h"

#include "benchutil.h"

#include <array>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

using namespace icy;
using namespace icy::wrtc;

namespace {

struct LoopbackBus
{
    LoopbackBus()
        : worker([this](std::stop_token stopToken) { run(stopToken); })
    {
    }

    ~LoopbackBus()
    {
        {
            std::lock_guard lock(mutex);
            stopping = true;
        }
        cv.notify_all();
    }

    void post(std::function<void()> fn)
    {
        {
            std::lock_guard lock(mutex);
            queue.emplace_back(std::move(fn));
        }
        cv.notify_one();
    }

    void run(std::stop_token stopToken)
    {
        std::unique_lock lock(mutex);
        for (;;) {
            cv.wait(lock, [this, &stopToken] {
                return stopping || stopToken.stop_requested() || !queue.empty();
            });

            if ((stopping || stopToken.stop_requested()) && queue.empty())
                return;

            auto next = std::move(queue.front());
            queue.pop_front();
            lock.unlock();
            next();
            lock.lock();
        }
    }

    std::mutex mutex;
    std::condition_variable cv;
    std::deque<std::function<void()>> queue;
    std::jthread worker;
    bool stopping = false;
};


struct LoopbackSignaller : SignallingInterface
{
    explicit LoopbackSignaller(std::string id)
        : selfId(std::move(id))
        , bus(std::make_shared<LoopbackBus>())
    {
    }

    ~LoopbackSignaller()
    {
        alive->store(false, std::memory_order_release);
    }

    void connect(LoopbackSignaller& other)
    {
        peer = &other;
        other.peer = this;
        other.bus = bus;
    }

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override
    {
        if (!peer || peer->selfId != peerId)
            throw std::logic_error("LoopbackSignaller SDP peer mismatch");
        bus->post([target = peer, guard = peer->alive, peerId = selfId, type, sdp] {
            if (!guard->load(std::memory_order_acquire))
                return;
            target->SdpReceived.emit(peerId, type, sdp);
        });
    }

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override
    {
        if (!peer || peer->selfId != peerId)
            throw std::logic_error("LoopbackSignaller candidate peer mismatch");
        bus->post([target = peer, guard = peer->alive, peerId = selfId, candidate, mid] {
            if (!guard->load(std::memory_order_acquire))
                return;
            target->CandidateReceived.emit(peerId, candidate, mid);
        });
    }

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override
    {
        if (!peer || peer->selfId != peerId)
            throw std::logic_error("LoopbackSignaller control peer mismatch");
        bus->post([target = peer, guard = peer->alive, peerId = selfId, type, reason] {
            if (!guard->load(std::memory_order_acquire))
                return;
            target->ControlReceived.emit(peerId, type, reason);
        });
    }

    std::string selfId;
    LoopbackSignaller* peer = nullptr;
    std::shared_ptr<LoopbackBus> bus;
    std::shared_ptr<std::atomic<bool>> alive = std::make_shared<std::atomic<bool>>(true);
};


bool waitUntil(const std::function<bool()>& predicate,
               std::chrono::milliseconds timeout = std::chrono::seconds(10))
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate())
            return true;
        uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return predicate();
}


bench::Measurement measureSendOnly(const bench::Options& options,
                                   uint64_t iterations,
                                   const std::function<void()>& sample,
                                   const std::function<void()>& drain)
{
    for (uint32_t warmup = 0; warmup < options.warmupSamples; ++warmup) {
        sample();
        drain();
    }

    std::vector<double> samples;
    samples.reserve(options.samples);
    double best = 0.0;

    for (uint32_t index = 0; index < options.samples; ++index) {
        const uint64_t start = time::hrtime();
        sample();
        const uint64_t end = time::hrtime();
        drain();

        const double nsPerOp = (end - start) * 1.0 / static_cast<double>(iterations);
        samples.push_back(nsPerOp);
        if (index == 0 || nsPerOp < best)
            best = nsPerOp;
    }

    std::sort(samples.begin(), samples.end());
    return {
        .nsPerOp = samples[samples.size() / 2],
        .bestNsPerOp = best,
        .iterations = iterations,
        .samples = options.samples,
    };
}


PeerSession::Config makeAudioConfig()
{
    PeerSession::Config config;
    config.media.audioCodec = CodecNegotiator::resolveWebRtcAudioCodec(
        av::AudioCodec("opus", "libopus", 2, 48000));
    config.enableDataChannel = false;
    return config;
}


struct AudioLoopbackPair
{
    AudioLoopbackPair()
        : aliceSig("alice")
        , bobSig("bob")
        , alice(aliceSig, makeAudioConfig())
        , bob(bobSig, makeAudioConfig())
    {
        aliceSig.connect(bobSig);
        bob.IncomingCall += [this](const std::string&) { bob.accept(); };
        bob.media().audioReceiver().emitter += [this](IPacket& packet) {
            if (dynamic_cast<av::AudioPacket*>(&packet) == nullptr)
                return;
            received.fetch_add(1, std::memory_order_release);
            std::lock_guard lock(receiveMutex);
            receiveCv.notify_all();
        };

        alice.call("bob");
        if (!waitUntil([this] {
                return alice.state() == PeerSession::State::Active &&
                       bob.state() == PeerSession::State::Active;
            })) {
            throw std::runtime_error("Timed out waiting for loopback session to become active");
        }
    }

    ~AudioLoopbackPair()
    {
        const auto aliceState = alice.state();
        if (aliceState != PeerSession::State::Idle &&
            aliceState != PeerSession::State::Ended) {
            alice.hangup("bench");
        }

        const auto bobState = bob.state();
        if (bobState != PeerSession::State::Idle &&
            bobState != PeerSession::State::Ended) {
            bob.hangup("bench");
        }
    }

    void waitForReceived(uint64_t target) const
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while (std::chrono::steady_clock::now() < deadline) {
            if (received.load(std::memory_order_acquire) >= target)
                return;

            uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);

            std::unique_lock lock(receiveMutex);
            receiveCv.wait_for(lock, std::chrono::milliseconds(2), [&] {
                return received.load(std::memory_order_acquire) >= target;
            });
        }
        throw std::runtime_error("Timed out waiting for loopback audio delivery");
    }

    LoopbackSignaller aliceSig;
    LoopbackSignaller bobSig;
    PeerSession alice;
    PeerSession bob;
    mutable std::mutex receiveMutex;
    mutable std::condition_variable receiveCv;
    std::atomic<uint64_t> received{0};
};


std::vector<float> makeAudioSamples(int channels,
                                    int samplesPerChannel,
                                    int sampleRate)
{
    std::vector<float> samples(static_cast<size_t>(channels) *
                               static_cast<size_t>(samplesPerChannel));
    constexpr double pi = 3.14159265358979323846;
    constexpr double frequency = 440.0;
    constexpr double amplitude = 0.35;

    for (int sample = 0; sample < samplesPerChannel; ++sample) {
        const double t = static_cast<double>(sample) /
                         static_cast<double>(sampleRate);
        const float value = static_cast<float>(
            std::sin(2.0 * pi * frequency * t) * amplitude);
        for (int channel = 0; channel < channels; ++channel)
            samples[static_cast<size_t>(sample) * channels + channel] = value;
    }

    return samples;
}


av::AudioPacket makeEncodedAudioPacket(const av::AudioCodec& codec)
{
    av::AudioPacketEncoder encoder;
    encoder.iparams.channels = codec.channels;
    encoder.iparams.sampleRate = codec.sampleRate;
    encoder.iparams.sampleFmt = "flt";
    encoder.oparams = codec;

    std::unique_ptr<IPacket> encoded;
    encoder.emitter += [&](IPacket& packet) {
        encoded = packet.clone();
    };

    encoder.create();
    encoder.open();

    constexpr int samplesPerFrame = 960;
    auto samples = makeAudioSamples(codec.channels, samplesPerFrame, codec.sampleRate);
    av::AudioPacket input(
        reinterpret_cast<uint8_t*>(samples.data()),
        samples.size() * sizeof(float),
        samplesPerFrame,
        0);
    encoder.process(input);
    encoder.flush();
    encoder.close();

    auto* audio = dynamic_cast<av::AudioPacket*>(encoded.get());
    if (!audio)
        throw std::runtime_error("Failed to encode benchmark audio packet");
    return *audio;
}


void benchSenderDispatch(bench::Reporter& reporter)
{
    AudioLoopbackPair pair;
    auto& sender = pair.alice.media().audioSender();
    auto packet = makeEncodedAudioPacket(makeAudioConfig().media.audioCodec);

    constexpr uint64_t iterations = 200;
    uint64_t drainTarget = 0;

    const auto measurement = measureSendOnly(
        reporter.options(),
        iterations,
        [&] {
            drainTarget = pair.received.load(std::memory_order_acquire) + iterations;
            int64_t timeUs = 0;
            for (uint64_t index = 0; index < iterations; ++index) {
                packet.time = timeUs;
                sender.process(packet);
                timeUs += 20000;
            }
        },
        [&] { pair.waitForReceived(drainTarget); });

    reporter.add({
        .name = "webrtc audio sender dispatch",
        .measurement = measurement,
        .metrics = {bench::metric("payload_bytes", static_cast<uint64_t>(packet.size())),
                    bench::metric("packets", iterations)},
    });
}


void benchReceiverDispatch(bench::Reporter& reporter)
{
    AudioLoopbackPair pair;
    auto& sender = pair.alice.media().audioSender();
    auto packet = makeEncodedAudioPacket(makeAudioConfig().media.audioCodec);

    constexpr uint64_t iterations = 200;
    uint64_t delivered = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        const uint64_t target =
            pair.received.load(std::memory_order_acquire) + iterations;
        int64_t timeUs = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            packet.time = timeUs;
            sender.process(packet);
            timeUs += 20000;
        }
        pair.waitForReceived(target);
        delivered = iterations;
    });

    reporter.add({
        .name = "webrtc audio receiver dispatch",
        .measurement = measurement,
        .metrics = {bench::metric("payload_bytes", static_cast<uint64_t>(packet.size())),
                    bench::metric("delivered", delivered)},
    });
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);
    benchSenderDispatch(reporter);
    benchReceiverDispatch(reporter);
    return reporter.finish();
}
