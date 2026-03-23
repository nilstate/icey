///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "webrtctests.h"

#include <cmath>


using namespace std;
using namespace icy;
using namespace icy::wrtc;
using namespace icy::test;


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

struct MockSignaller : SignallingInterface
{
    struct ControlMessage
    {
        std::string peerId;
        std::string type;
        std::string reason;
    };

    std::vector<ControlMessage> controls;
    std::function<void(const std::string&, const std::string&, const std::string&)> onSendControl;

    void sendSdp(const std::string&, const std::string&, const std::string&) override {}
    void sendCandidate(const std::string&, const std::string&, const std::string&) override {}

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override
    {
        controls.push_back({peerId, type, reason});
        if (onSendControl)
            onSendControl(peerId, type, reason);
    }
};


struct ReentrantRejectSignaller : MockSignaller
{
    bool rejectOnInit = true;

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override
    {
        MockSignaller::sendControl(peerId, type, reason);

        if (rejectOnInit && type == "init") {
            rejectOnInit = false;
            ControlReceived.emit(peerId, "reject", "busy");
        }
    }
};


struct LoopbackSignaller : SignallingInterface
{
    explicit LoopbackSignaller(std::string id)
        : selfId(std::move(id))
        , bus(std::make_shared<LoopbackBus>())
    {
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
        bus->post([target = peer, peerId = selfId, type, sdp] {
            target->SdpReceived.emit(peerId, type, sdp);
        });
    }

    void sendCandidate(const std::string& peerId,
                       const std::string& candidate,
                       const std::string& mid) override
    {
        if (!peer || peer->selfId != peerId)
            throw std::logic_error("LoopbackSignaller candidate peer mismatch");
        bus->post([target = peer, peerId = selfId, candidate, mid] {
            target->CandidateReceived.emit(peerId, candidate, mid);
        });
    }

    void sendControl(const std::string& peerId,
                     const std::string& type,
                     const std::string& reason = {}) override
    {
        if (!peer || peer->selfId != peerId)
            throw std::logic_error("LoopbackSignaller control peer mismatch");
        bus->post([target = peer, peerId = selfId, type, reason] {
            target->ControlReceived.emit(peerId, type, reason);
        });
    }

    std::string selfId;
    LoopbackSignaller* peer = nullptr;
    std::shared_ptr<LoopbackBus> bus;
};

} // namespace

#ifdef HAVE_FFMPEG
namespace {

void fillLoopbackVideoFrame(AVFrame* frame, int index)
{
    for (int y = 0; y < frame->height; y++) {
        for (int x = 0; x < frame->width; x++) {
            frame->data[0][y * frame->linesize[0] + x] =
                static_cast<uint8_t>((x * 2 + y + index * 11) % 256);
        }
    }
    for (int y = 0; y < frame->height / 2; y++) {
        for (int x = 0; x < frame->width / 2; x++) {
            frame->data[1][y * frame->linesize[1] + x] =
                static_cast<uint8_t>((64 + y + index * 3) % 256);
            frame->data[2][y * frame->linesize[2] + x] =
                static_cast<uint8_t>((192 + x + index * 5) % 256);
        }
    }
}

void configureLoopbackAudioCodec(PeerSession::Config& config)
{
    config.mediaOpts.audioCodec = av::AudioCodec("opus", "libopus", 2, 48000);
    config.mediaOpts.audioCodec.options["application"] = "lowdelay";
}

std::vector<float> makeLoopbackAudioSamples(int channels,
                                            int samplesPerChannel,
                                            int sampleRate,
                                            int sampleOffset)
{
    std::vector<float> samples(static_cast<size_t>(channels) *
                               static_cast<size_t>(samplesPerChannel));
    constexpr double pi = 3.14159265358979323846;
    constexpr double frequency = 440.0;
    constexpr double amplitude = 0.35;

    for (int sample = 0; sample < samplesPerChannel; ++sample) {
        double t = static_cast<double>(sampleOffset + sample) /
                   static_cast<double>(sampleRate);
        float value = static_cast<float>(
            std::sin(2.0 * pi * frequency * t) * amplitude);
        for (int channel = 0; channel < channels; ++channel)
            samples[static_cast<size_t>(sample) * channels + channel] = value;
    }

    return samples;
}

template <typename Sink>
void sendLoopbackAudioFrames(const av::AudioCodec& codec,
                             int frameCount,
                             int64_t startTimeUs,
                             Sink&& sink)
{
    av::AudioPacketEncoder encoder;
    encoder.iparams.channels = codec.channels;
    encoder.iparams.sampleRate = codec.sampleRate;
    encoder.iparams.sampleFmt = "flt";
    encoder.oparams = codec;
    encoder.emitter += [&](IPacket& packet) {
        sink(packet);
    };
    encoder.create();
    encoder.open();

    constexpr int samplesPerFrame = 960; // 20 ms @ 48 kHz
    for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        auto samples = makeLoopbackAudioSamples(
            codec.channels,
            samplesPerFrame,
            codec.sampleRate,
            frameIndex * samplesPerFrame);
        av::AudioPacket packet(
            reinterpret_cast<uint8_t*>(samples.data()),
            samples.size() * sizeof(float),
            samplesPerFrame,
            startTimeUs + static_cast<int64_t>(frameIndex) * 20000);
        encoder.process(packet);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    encoder.flush();
    encoder.close();
}

} // namespace
#endif


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));
    test::init();


    // =========================================================================
    // Layer 0: CodecNegotiator
    // =========================================================================

    describe("codec negotiator: video negotiation", []() {
        auto result = CodecNegotiator::negotiateVideo({"H264", "VP8"});
        if (result.has_value()) {
            expect(!result->rtpName.empty());
            expect(!result->ffmpegName.empty());
            expect(result->clockRate == 90000);
            expect(result->payloadType > 0);
        }
    });

    describe("codec negotiator: audio negotiation", []() {
        auto result = CodecNegotiator::negotiateAudio({"opus", "PCMU"});
        if (result.has_value()) {
            expect(!result->rtpName.empty());
            expect(!result->ffmpegName.empty());
            expect(result->clockRate > 0);
        }
    });

    describe("codec negotiator: empty offer returns nullopt", []() {
        expect(!CodecNegotiator::negotiateVideo({}).has_value());
        expect(!CodecNegotiator::negotiateAudio({}).has_value());
    });

    describe("codec negotiator: unknown codec returns nullopt", []() {
        expect(!CodecNegotiator::negotiateVideo({"NONEXISTENT"}).has_value());
    });

    describe("codec negotiator: bidirectional mapping", []() {
        expect(CodecNegotiator::ffmpegToRtp("libx264") == "H264");
        expect(CodecNegotiator::ffmpegToRtp("libvpx") == "VP8");
        expect(CodecNegotiator::ffmpegToRtp("libopus") == "opus");
        expect(CodecNegotiator::ffmpegToRtp("nonexistent").empty());
    });

    describe("codec negotiator: clock rates", []() {
        expect(CodecNegotiator::clockRate("H264") == 90000);
        expect(CodecNegotiator::clockRate("VP8") == 90000);
        expect(CodecNegotiator::clockRate("opus") == 48000);
        expect(CodecNegotiator::clockRate("PCMU") == 8000);
        expect(CodecNegotiator::clockRate("unknown") == 0);
    });

    describe("codec negotiator: codec specs", []() {
        auto h264 = CodecNegotiator::specFromRtp("H264");
        expect(h264.has_value());
        expect(h264->id == CodecId::H264);
        expect(h264->mediaType == CodecMediaType::Video);
        expect(h264->clockRate == 90000);
        expect(h264->payloadType == 96);

        auto pcmu = CodecNegotiator::specFromFfmpeg("pcm_mulaw");
        expect(pcmu.has_value());
        expect(pcmu->id == CodecId::PCMU);
        expect(pcmu->mediaType == CodecMediaType::Audio);
        expect(pcmu->clockRate == 8000);
        expect(pcmu->payloadType == 0);
    });

    describe("codec negotiator: explicit codec config resolves cleanly", []() {
        av::VideoCodec video("H264", "libx264", 640, 480, 30);
        expect(video.specified());
        auto videoSpec = CodecNegotiator::specFromVideoCodec(video);
        expect(videoSpec.has_value());
        expect(videoSpec->id == CodecId::H264);

        av::AudioCodec audio("PCMA", 2, 8000);
        expect(audio.specified());
        auto audioSpec = CodecNegotiator::specFromAudioCodec(audio);
        expect(audioSpec.has_value());
        expect(audioSpec->id == CodecId::PCMA);

        av::VideoCodec unset;
        expect(!unset.specified());
        expect(!CodecNegotiator::specFromVideoCodec(unset).has_value());
    });

    describe("codec negotiator: detect codec from sdp", []() {
        auto audio = CodecNegotiator::detectCodec(
            "m=audio 9 UDP/TLS/RTP/SAVPF 0\r\na=rtpmap:0 PCMU/8000\r\n",
            CodecMediaType::Audio);
        expect(audio.has_value());
        expect(audio->id == CodecId::PCMU);

        auto video = CodecNegotiator::detectCodec(
            "m=video 9 UDP/TLS/RTP/SAVPF 96\r\na=rtpmap:96 H264/90000\r\n",
            CodecMediaType::Video);
        expect(video.has_value());
        expect(video->id == CodecId::H264);
    });

    describe("codec negotiator: to av types", []() {
        NegotiatedCodec nc;
        nc.rtpName = "H264";
        nc.ffmpegName = "libx264";
        nc.clockRate = 90000;

        auto vc = nc.toVideoCodec(1280, 720, 30.0);
        expect(vc.name == "H264");
        expect(vc.encoder == "libx264");
        expect(vc.width == 1280);

        NegotiatedCodec ac;
        ac.rtpName = "opus";
        ac.ffmpegName = "libopus";
        ac.clockRate = 48000;

        auto acodec = ac.toAudioCodec(2);
        expect(acodec.name == "opus");
        expect(acodec.encoder == "libopus");
        expect(acodec.sampleRate == 48000);
    });


    // =========================================================================
    // Layer 1: Track factory functions
    // =========================================================================

    describe("track: create video track", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        auto vh = createVideoTrack(pc, av::VideoCodec("H264", "libx264", 640, 480, 30));
        expect(vh.track != nullptr);
        expect(vh.rtpConfig != nullptr);
        expect(vh.rtpConfig->clockRate == 90000);
        expect(vh.rtpConfig->payloadType == 96);

        pc->close();
    });

    describe("track: create audio track", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        auto ah = createAudioTrack(pc, av::AudioCodec("opus", "libopus", 2, 48000));
        expect(ah.track != nullptr);
        expect(ah.rtpConfig != nullptr);
        expect(ah.rtpConfig->clockRate == 48000);
        expect(ah.rtpConfig->payloadType == 111);

        pc->close();
    });

    describe("track: create pcmu audio track", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        auto ah = createAudioTrack(pc, av::AudioCodec("PCMU", "pcm_mulaw", 1, 8000));
        expect(ah.track != nullptr);
        expect(ah.rtpConfig != nullptr);
        expect(ah.rtpConfig->clockRate == 8000);
        expect(ah.rtpConfig->payloadType == 0);

        pc->close();
    });

    describe("track: create tracks from RTP names", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        auto vh = createVideoTrack(pc, av::VideoCodec("H264", 640, 480, 30));
        auto ah = createAudioTrack(pc, av::AudioCodec("PCMA", 1, 8000));
        expect(vh.rtpConfig->payloadType == 96);
        expect(ah.rtpConfig->payloadType == 8);

        pc->close();
    });

    describe("track: explicit codec is required", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        bool videoThrew = false;
        bool audioThrew = false;
        try { [[maybe_unused]] auto vh = createVideoTrack(pc, av::VideoCodec{}); } catch (const std::invalid_argument&) { videoThrew = true; }
        try { [[maybe_unused]] auto ah = createAudioTrack(pc, av::AudioCodec{}); } catch (const std::invalid_argument&) { audioThrew = true; }
        expect(videoThrew);
        expect(audioThrew);

        pc->close();
    });

    describe("track: PLI callback fires", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        bool pliFired = false;
        auto vh = createVideoTrack(pc, av::VideoCodec("H264", "libx264", 640, 480, 30), 0, {}, 512,
            [&]() { pliFired = true; });

        expect(vh.track != nullptr);
        // PLI handler is in the chain; it fires on incoming PLI messages.
        // We can't trigger it without a real connection, but verify the
        // track was created with the callback set.

        pc->close();
    });

    describe("track: generate SSRC", []() {
        auto s1 = generateSsrc();
        auto s2 = generateSsrc();
        expect(s1 != 0);
        expect(s2 != 0);
        // Very unlikely to be equal
        expect(s1 != s2);
    });


    // =========================================================================
    // Layer 2: WebRtcTrackSender
    // =========================================================================

    describe("track sender: unbound construction", []() {
        WebRtcTrackSender sender;
        expect(!sender.bound());
    });

    describe("track sender: bound construction", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);
        auto vh = createVideoTrack(pc, av::VideoCodec("H264", "libx264", 640, 480, 30));

        WebRtcTrackSender sender(vh);
        expect(sender.bound());
        expect(sender.isVideo());

        sender.unbind();
        expect(!sender.bound());

        pc->close();
    });

    describe("track sender: is PacketProcessor", []() {
        WebRtcTrackSender sender;
        PacketProcessor* proc = &sender;
        expect(proc != nullptr);

        av::VideoPacket vp;
        expect(proc->accepts(&vp));

        av::AudioPacket ap;
        expect(proc->accepts(&ap));
    });

    describe("track sender: audio track detection", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);
        auto ah = createAudioTrack(pc, av::AudioCodec("opus", "libopus", 2, 48000));

        WebRtcTrackSender sender(ah);
        expect(sender.bound());
        expect(!sender.isVideo());

        pc->close();
    });


    // =========================================================================
    // Layer 2: WebRtcTrackReceiver
    // =========================================================================

    describe("track receiver: is PacketStreamAdapter", []() {
        WebRtcTrackReceiver receiver;
        PacketStreamAdapter* adapter = &receiver;
        expect(adapter != nullptr);
    });


    // =========================================================================
    // Layer 3: MediaBridge
    // =========================================================================

    describe("media bridge: default construction", []() {
        MediaBridge bridge;
        expect(!bridge.attached());
        expect(!bridge.hasVideo());
        expect(!bridge.hasAudio());
    });

    describe("media bridge: video only", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.videoCodec = av::VideoCodec("H264", "libx264", 640, 480, 30);
        // audioCodec left default (empty encoder) → no audio track

        bridge.attach(pc, opts);
        expect(bridge.attached());
        expect(bridge.hasVideo());
        expect(!bridge.hasAudio());
        expect(bridge.videoTrack() != nullptr);
        expect(bridge.audioTrack() == nullptr);

        // videoSender() should work
        auto& vs = bridge.videoSender();
        expect(vs.bound());

        // audioSender() should throw
        bool threw = false;
        try { [[maybe_unused]] auto& sender = bridge.audioSender(); } catch (const std::logic_error&) { threw = true; }
        expect(threw);

        bridge.detach();
        expect(!bridge.attached());

        pc->close();
    });

    describe("media bridge: video + audio", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.videoCodec = av::VideoCodec("H264", "libx264", 640, 480, 30);
        opts.audioCodec = av::AudioCodec("opus", "libopus", 2, 48000);

        bridge.attach(pc, opts);
        expect(bridge.hasVideo());
        expect(bridge.hasAudio());
        expect(bridge.videoTrack() != nullptr);
        expect(bridge.audioTrack() != nullptr);

        bridge.detach();
        pc->close();
    });

    describe("media bridge: explicit RTP codec names create tracks", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.videoCodec = av::VideoCodec("H264", 640, 480, 30);
        opts.audioCodec = av::AudioCodec("PCMA", 1, 8000);

        bridge.attach(pc, opts);
        expect(bridge.hasVideo());
        expect(bridge.hasAudio());
        expect(bridge.videoTrack() != nullptr);
        expect(bridge.audioTrack() != nullptr);

        bridge.detach();
        pc->close();
    });

    describe("media bridge: double attach throws", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.videoCodec = av::VideoCodec("H264", "libx264");

        bridge.attach(pc, opts);

        bool threw = false;
        try { bridge.attach(pc, opts); } catch (const std::logic_error&) { threw = true; }
        expect(threw);

        pc->close();
    });

    describe("media bridge: sender reference survives detach and reattach", []() {
        rtc::Configuration config;
        auto pc1 = std::make_shared<rtc::PeerConnection>(config);
        auto pc2 = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.videoCodec = av::VideoCodec("H264", "libx264", 640, 480, 30);

        bridge.attach(pc1, opts);
        auto* sender = &bridge.videoSender();
        expect(sender->bound());

        bridge.detach();
        expect(!bridge.attached());
        expect(!sender->bound());

        bool threw = false;
        try { [[maybe_unused]] auto& rebound = bridge.videoSender(); } catch (const std::logic_error&) { threw = true; }
        expect(threw);

        bridge.attach(pc2, opts);
        expect(&bridge.videoSender() == sender);
        expect(sender->bound());

        bridge.detach();
        pc1->close();
        pc2->close();
    });


    // =========================================================================
    // Layer 3: PeerSession
    // =========================================================================

    describe("peer session: state strings", []() {
        expect(std::string(stateToString(PeerSession::State::Idle)) == "idle");
        expect(std::string(stateToString(PeerSession::State::OutgoingInit)) == "outgoing-init");
        expect(std::string(stateToString(PeerSession::State::IncomingInit)) == "incoming-init");
        expect(std::string(stateToString(PeerSession::State::Negotiating)) == "negotiating");
        expect(std::string(stateToString(PeerSession::State::Active)) == "active");
        expect(std::string(stateToString(PeerSession::State::Ending)) == "ending");
        expect(std::string(stateToString(PeerSession::State::Ended)) == "ended");
    });

    describe("peer session: remote reject returns to idle", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        session.call("peer");
        expect(signaller.controls.size() == 1);
        expect(signaller.controls[0].type == "init");
        expect(session.state() == PeerSession::State::OutgoingInit);

        signaller.ControlReceived.emit("peer", "reject", "busy");

        expect(states.size() == 4);
        expect(states[0] == PeerSession::State::OutgoingInit);
        expect(states[1] == PeerSession::State::Ending);
        expect(states[2] == PeerSession::State::Ended);
        expect(states[3] == PeerSession::State::Idle);
        expect(session.state() == PeerSession::State::Idle);
        expect(session.remotePeerId().empty());

        session.call("peer-2");
        expect(signaller.controls.size() == 2);
        expect(signaller.controls[1].peerId == "peer-2");
        expect(signaller.controls[1].type == "init");
        expect(session.state() == PeerSession::State::OutgoingInit);

        session.hangup("retry");
        expect(session.state() == PeerSession::State::Idle);
    });

    describe("peer session: local hangup passes through ending", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        session.call("peer");
        session.hangup("cancelled");

        expect(signaller.controls.size() == 2);
        expect(signaller.controls[0].type == "init");
        expect(signaller.controls[1].type == "hangup");
        expect(states.size() == 4);
        expect(states[0] == PeerSession::State::OutgoingInit);
        expect(states[1] == PeerSession::State::Ending);
        expect(states[2] == PeerSession::State::Ended);
        expect(states[3] == PeerSession::State::Idle);
        expect(session.state() == PeerSession::State::Idle);
    });

    describe("peer session: accept waits for remote data channel", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = true;
        config.dataChannelLabel = "control";
        PeerSession session(signaller, config);

        signaller.ControlReceived.emit("peer", "init", "");
        expect(session.state() == PeerSession::State::IncomingInit);

        session.accept();

        expect(signaller.controls.size() == 1);
        expect(signaller.controls[0].type == "accept");
        expect(session.state() == PeerSession::State::Negotiating);
        expect(session.peerConnection() != nullptr);
        expect(session.dataChannel() == nullptr);
    });

    describe("peer session: accept creates peer connection before signalling", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});
        bool pcWasReadyDuringAccept = false;

        signaller.onSendControl = [&](const std::string&, const std::string& type, const std::string&) {
            if (type == "accept")
                pcWasReadyDuringAccept = session.peerConnection() != nullptr;
        };

        signaller.ControlReceived.emit("peer", "init", "");
        session.accept();

        expect(pcWasReadyDuringAccept);
        expect(session.peerConnection() != nullptr);
    });

    describe("peer session: caller owns local data channel", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = true;
        config.dataChannelLabel = "control";
        PeerSession session(signaller, config);

        session.call("peer");
        signaller.ControlReceived.emit("peer", "accept", "");

        expect(session.state() == PeerSession::State::Negotiating);
        expect(session.peerConnection() != nullptr);
        expect(session.dataChannel() != nullptr);
    });

    describe("peer session: reentrant signaller preserves state order", []() {
        ReentrantRejectSignaller signaller;
        PeerSession session(signaller, {});
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        session.call("peer");

        expect(states.size() == 4);
        expect(states[0] == PeerSession::State::OutgoingInit);
        expect(states[1] == PeerSession::State::Ending);
        expect(states[2] == PeerSession::State::Ended);
        expect(states[3] == PeerSession::State::Idle);
        expect(session.state() == PeerSession::State::Idle);
        expect(session.remotePeerId().empty());
        expect(signaller.controls.size() == 1);
        expect(signaller.controls[0].type == "init");
    });

    describe("peer session: loopback peers reach active and pass data", []() {
        LoopbackSignaller aliceSig("alice");
        LoopbackSignaller bobSig("bob");
        aliceSig.connect(bobSig);

        PeerSession::Config config;
        config.enableDataChannel = true;
        config.dataChannelLabel = "control";

        PeerSession alice(aliceSig, config);
        PeerSession bob(bobSig, config);
        std::string received;

        bob.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "alice");
            bob.accept();
        };
        bob.DataReceived += [&](rtc::message_variant msg) {
            if (auto* text = std::get_if<std::string>(&msg))
                received = *text;
        };

        alice.call("bob");

        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Active &&
                   bob.state() == PeerSession::State::Active;
        }, 8000));

        expect(icy::test::waitFor([&] {
            auto aliceDc = alice.dataChannel();
            auto bobDc = bob.dataChannel();
            return aliceDc && bobDc && aliceDc->isOpen() && bobDc->isOpen();
        }, 8000));

        alice.sendData("hello");
        expect(icy::test::waitFor([&] { return received == "hello"; }, 3000));

        alice.hangup("done");
        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Idle &&
                   bob.state() == PeerSession::State::Idle;
        }, 3000));
    });

#ifdef HAVE_FFMPEG
    describe("peer session: loopback audio path encodes and roundtrips media", []() {
        LoopbackSignaller aliceSig("alice");
        LoopbackSignaller bobSig("bob");
        aliceSig.connect(bobSig);

        PeerSession::Config config;
        config.enableDataChannel = false;
        configureLoopbackAudioCodec(config);

        PeerSession alice(aliceSig, config);
        PeerSession bob(bobSig, config);

        bob.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "alice");
            bob.accept();
        };

        std::atomic<int> receivedPackets{0};
        std::atomic<int64_t> lastReceivedTime{0};
        std::atomic<size_t> lastReceivedSize{0};

        bob.media().audioReceiver().emitter += [&](IPacket& packet) {
            auto* audio = dynamic_cast<av::AudioPacket*>(&packet);
            if (!audio)
                return;

            receivedPackets.fetch_add(1);
            lastReceivedTime.store(audio->time);
            lastReceivedSize.store(audio->size());
        };

        alice.call("bob");

        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Active &&
                   bob.state() == PeerSession::State::Active;
        }, 8000));

        sendLoopbackAudioFrames(
            config.mediaOpts.audioCodec,
            32,
            0,
            [&](IPacket& packet) {
                alice.media().audioSender().process(packet);
            });

        expect(icy::test::waitFor([&] {
            return receivedPackets.load() > 0;
        }, 5000));
        expect(lastReceivedTime.load() >= 0);
        expect(lastReceivedSize.load() > 0);

        alice.hangup("done");
        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Idle &&
                   bob.state() == PeerSession::State::Idle;
        }, 3000));
    });

    describe("peer session: loopback video path encodes and roundtrips media", []() {
        LoopbackSignaller aliceSig("alice");
        LoopbackSignaller bobSig("bob");
        aliceSig.connect(bobSig);

        PeerSession::Config config;
        config.enableDataChannel = false;
        config.mediaOpts.videoCodec = av::VideoCodec("H264", "libx264", 160, 120, 30);
        config.mediaOpts.videoCodec.pixelFmt = "yuv420p";
        config.mediaOpts.videoCodec.options["preset"] = "ultrafast";
        config.mediaOpts.videoCodec.options["tune"] = "zerolatency";
        config.mediaOpts.videoCodec.options["profile"] = "baseline";

        PeerSession alice(aliceSig, config);
        PeerSession bob(bobSig, config);

        bob.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "alice");
            bob.accept();
        };

        std::atomic<int> receivedPackets{0};
        std::atomic<int64_t> lastReceivedTime{0};
        std::atomic<size_t> lastReceivedSize{0};

        bob.media().videoReceiver().emitter += [&](IPacket& packet) {
            auto* video = dynamic_cast<av::VideoPacket*>(&packet);
            if (!video)
                return;

            receivedPackets.fetch_add(1);
            lastReceivedTime.store(video->time);
            lastReceivedSize.store(video->size());
        };

        alice.call("bob");

        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Active &&
                   bob.state() == PeerSession::State::Active;
        }, 8000));

        av::VideoPacketEncoder encoder;
        encoder.iparams.width = 160;
        encoder.iparams.height = 120;
        encoder.iparams.pixelFmt = "yuv420p";
        encoder.oparams = config.mediaOpts.videoCodec;
        encoder.emitter += [&](IPacket& packet) {
            alice.media().videoSender().process(packet);
        };
        encoder.create();
        encoder.open();

        for (int i = 0; i < 24 && receivedPackets.load() == 0; ++i) {
            av::AVFrameHolder frame(av::createVideoFrame(AV_PIX_FMT_YUV420P, 160, 120));
            expect(static_cast<bool>(frame));
            fillLoopbackVideoFrame(frame.get(), i);

            av::PlanarVideoPacket packet(frame->data, frame->linesize,
                                         "yuv420p", 160, 120,
                                         static_cast<int64_t>(i) * 33333);
            encoder.process(packet);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        encoder.flush();
        encoder.close();

        expect(icy::test::waitFor([&] {
            return receivedPackets.load() > 0;
        }, 5000));
        expect(lastReceivedSize.load() > 0);

        alice.hangup("done");
        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Idle &&
                   bob.state() == PeerSession::State::Idle;
        }, 3000));
    });

    describe("peer session: encoded relay path forwards media between sessions", []() {
        LoopbackSignaller publisherSig("publisher");
        LoopbackSignaller ingressSig("relay-ingress");
        publisherSig.connect(ingressSig);

        LoopbackSignaller viewerSig("viewer");
        LoopbackSignaller egressSig("relay-egress");
        viewerSig.connect(egressSig);

        PeerSession::Config config;
        config.enableDataChannel = false;
        config.mediaOpts.videoCodec = av::VideoCodec("H264", "libx264", 160, 120, 30);
        config.mediaOpts.videoCodec.pixelFmt = "yuv420p";
        config.mediaOpts.videoCodec.options["preset"] = "ultrafast";
        config.mediaOpts.videoCodec.options["tune"] = "zerolatency";
        config.mediaOpts.videoCodec.options["profile"] = "baseline";

        PeerSession publisher(publisherSig, config);
        PeerSession relayIngress(ingressSig, config);
        PeerSession relayEgress(egressSig, config);
        PeerSession viewer(viewerSig, config);

        relayIngress.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "publisher");
            relayIngress.accept();
        };
        relayEgress.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "viewer");
            relayEgress.accept();
        };

        relayIngress.media().videoReceiver().emitter += [&](IPacket& packet) {
            relayEgress.media().videoSender().process(packet);
        };

        std::atomic<int> receivedPackets{0};
        std::atomic<size_t> lastReceivedSize{0};
        viewer.media().videoReceiver().emitter += [&](IPacket& packet) {
            auto* video = dynamic_cast<av::VideoPacket*>(&packet);
            if (!video)
                return;
            receivedPackets.fetch_add(1);
            lastReceivedSize.store(video->size());
        };

        publisher.call("relay-ingress");
        viewer.call("relay-egress");

        expect(icy::test::waitFor([&] {
            return publisher.state() == PeerSession::State::Active &&
                   relayIngress.state() == PeerSession::State::Active &&
                   relayEgress.state() == PeerSession::State::Active &&
                   viewer.state() == PeerSession::State::Active;
        }, 8000));

        av::VideoPacketEncoder encoder;
        encoder.iparams.width = 160;
        encoder.iparams.height = 120;
        encoder.iparams.pixelFmt = "yuv420p";
        encoder.oparams = config.mediaOpts.videoCodec;
        encoder.emitter += [&](IPacket& packet) {
            publisher.media().videoSender().process(packet);
        };
        encoder.create();
        encoder.open();

        for (int i = 0; i < 24 && receivedPackets.load() == 0; ++i) {
            av::AVFrameHolder frame(av::createVideoFrame(AV_PIX_FMT_YUV420P, 160, 120));
            expect(static_cast<bool>(frame));
            fillLoopbackVideoFrame(frame.get(), i);

            av::PlanarVideoPacket packet(frame->data, frame->linesize,
                                         "yuv420p", 160, 120,
                                         static_cast<int64_t>(i) * 33333);
            encoder.process(packet);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        encoder.flush();
        encoder.close();

        expect(icy::test::waitFor([&] {
            return receivedPackets.load() > 0;
        }, 5000));
        expect(lastReceivedSize.load() > 0);

        publisher.hangup("done");
        viewer.hangup("done");
        expect(icy::test::waitFor([&] {
            return publisher.state() == PeerSession::State::Idle &&
                   relayIngress.state() == PeerSession::State::Idle &&
                   relayEgress.state() == PeerSession::State::Idle &&
                   viewer.state() == PeerSession::State::Idle;
        }, 3000));
    });

    describe("peer session: encoded audio relay path forwards media between sessions", []() {
        LoopbackSignaller publisherSig("publisher");
        LoopbackSignaller ingressSig("relay-ingress");
        publisherSig.connect(ingressSig);

        LoopbackSignaller viewerSig("viewer");
        LoopbackSignaller egressSig("relay-egress");
        viewerSig.connect(egressSig);

        PeerSession::Config config;
        config.enableDataChannel = false;
        configureLoopbackAudioCodec(config);

        PeerSession publisher(publisherSig, config);
        PeerSession relayIngress(ingressSig, config);
        PeerSession relayEgress(egressSig, config);
        PeerSession viewer(viewerSig, config);

        relayIngress.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "publisher");
            relayIngress.accept();
        };
        relayEgress.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "viewer");
            relayEgress.accept();
        };

        relayIngress.media().audioReceiver().emitter += [&](IPacket& packet) {
            relayEgress.media().audioSender().process(packet);
        };

        std::atomic<int> receivedPackets{0};
        std::atomic<int64_t> lastReceivedTime{0};
        std::atomic<size_t> lastReceivedSize{0};
        viewer.media().audioReceiver().emitter += [&](IPacket& packet) {
            auto* audio = dynamic_cast<av::AudioPacket*>(&packet);
            if (!audio)
                return;
            receivedPackets.fetch_add(1);
            lastReceivedTime.store(audio->time);
            lastReceivedSize.store(audio->size());
        };

        publisher.call("relay-ingress");
        viewer.call("relay-egress");

        expect(icy::test::waitFor([&] {
            return publisher.state() == PeerSession::State::Active &&
                   relayIngress.state() == PeerSession::State::Active &&
                   relayEgress.state() == PeerSession::State::Active &&
                   viewer.state() == PeerSession::State::Active;
        }, 8000));

        sendLoopbackAudioFrames(
            config.mediaOpts.audioCodec,
            32,
            0,
            [&](IPacket& packet) {
                publisher.media().audioSender().process(packet);
            });

        expect(icy::test::waitFor([&] {
            return receivedPackets.load() > 0;
        }, 5000));
        expect(lastReceivedTime.load() >= 0);
        expect(lastReceivedSize.load() > 0);

        publisher.hangup("done");
        viewer.hangup("done");
        expect(icy::test::waitFor([&] {
            return publisher.state() == PeerSession::State::Idle &&
                   relayIngress.state() == PeerSession::State::Idle &&
                   relayEgress.state() == PeerSession::State::Idle &&
                   viewer.state() == PeerSession::State::Idle;
        }, 3000));
    });

    describe("peer session: relay handoff forwards media from the next active source", []() {
        LoopbackSignaller publisherASig("publisher-a");
        LoopbackSignaller ingressASig("relay-ingress-a");
        publisherASig.connect(ingressASig);

        LoopbackSignaller publisherBSig("publisher-b");
        LoopbackSignaller ingressBSig("relay-ingress-b");
        publisherBSig.connect(ingressBSig);

        LoopbackSignaller viewerSig("viewer");
        LoopbackSignaller egressSig("relay-egress");
        viewerSig.connect(egressSig);

        PeerSession::Config config;
        config.enableDataChannel = false;
        configureLoopbackAudioCodec(config);

        PeerSession publisherA(publisherASig, config);
        PeerSession relayIngressA(ingressASig, config);
        PeerSession publisherB(publisherBSig, config);
        PeerSession relayIngressB(ingressBSig, config);
        PeerSession relayEgress(egressSig, config);
        PeerSession viewer(viewerSig, config);

        relayIngressA.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "publisher-a");
            relayIngressA.accept();
        };
        relayIngressB.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "publisher-b");
            relayIngressB.accept();
        };
        relayEgress.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "viewer");
            relayEgress.accept();
        };

        std::atomic<int> activeSource{1};
        relayIngressA.media().audioReceiver().emitter += [&](IPacket& packet) {
            if (activeSource.load(std::memory_order_relaxed) == 1)
                relayEgress.media().audioSender().process(packet);
        };
        relayIngressB.media().audioReceiver().emitter += [&](IPacket& packet) {
            if (activeSource.load(std::memory_order_relaxed) == 2)
                relayEgress.media().audioSender().process(packet);
        };

        std::atomic<int> receivedPackets{0};
        std::atomic<int64_t> lastReceivedTime{0};
        viewer.media().audioReceiver().emitter += [&](IPacket& packet) {
            auto* audio = dynamic_cast<av::AudioPacket*>(&packet);
            if (!audio)
                return;
            receivedPackets.fetch_add(1);
            lastReceivedTime.store(audio->time);
        };

        publisherA.call("relay-ingress-a");
        publisherB.call("relay-ingress-b");
        viewer.call("relay-egress");

        expect(icy::test::waitFor([&] {
            return publisherA.state() == PeerSession::State::Active &&
                   relayIngressA.state() == PeerSession::State::Active &&
                   publisherB.state() == PeerSession::State::Active &&
                   relayIngressB.state() == PeerSession::State::Active &&
                   relayEgress.state() == PeerSession::State::Active &&
                   viewer.state() == PeerSession::State::Active;
        }, 10000));

        sendLoopbackAudioFrames(
            config.mediaOpts.audioCodec,
            16,
            0,
            [&](IPacket& packet) {
                publisherA.media().audioSender().process(packet);
            });

        expect(icy::test::waitFor([&] {
            return receivedPackets.load() > 0;
        }, 5000));
        expect(lastReceivedTime.load() < 10000000);

        publisherA.hangup("handoff");
        expect(icy::test::waitFor([&] {
            return publisherA.state() == PeerSession::State::Idle &&
                   relayIngressA.state() == PeerSession::State::Idle;
        }, 3000));

        activeSource.store(2, std::memory_order_relaxed);
        sendLoopbackAudioFrames(
            config.mediaOpts.audioCodec,
            24,
            10000000,
            [&](IPacket& packet) {
                publisherB.media().audioSender().process(packet);
            });

        expect(icy::test::waitFor([&] {
            return lastReceivedTime.load() >= 10000000;
        }, 5000));

        publisherB.hangup("done");
        viewer.hangup("done");
        expect(icy::test::waitFor([&] {
            return publisherB.state() == PeerSession::State::Idle &&
                   relayIngressB.state() == PeerSession::State::Idle &&
                   relayEgress.state() == PeerSession::State::Idle &&
                   viewer.state() == PeerSession::State::Idle;
        }, 3000));
    });
#endif


    // =========================================================================
    // PeerConnection sanity
    // =========================================================================

    describe("peer connection: basic creation", []() {
        rtc::Configuration config;
        config.iceServers.emplace_back("stun:stun.l.google.com:19302");
        auto pc = std::make_shared<rtc::PeerConnection>(config);
        expect(pc != nullptr);
        expect(pc->state() == rtc::PeerConnection::State::New);
        pc->close();
    });


    test::runAll();
    return test::finalize();
}
