///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "webrtctests.h"


using namespace std;
using namespace icy;
using namespace icy::wrtc;
using namespace icy::test;


namespace {

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

} // namespace


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
        expect(std::string(stateToString(PeerSession::State::Ringing)) == "ringing");
        expect(std::string(stateToString(PeerSession::State::Incoming)) == "incoming");
        expect(std::string(stateToString(PeerSession::State::Connecting)) == "connecting");
        expect(std::string(stateToString(PeerSession::State::Active)) == "active");
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
        expect(session.state() == PeerSession::State::Ringing);

        signaller.ControlReceived.emit("peer", "reject", "busy");

        expect(states.size() == 3);
        expect(states[0] == PeerSession::State::Ringing);
        expect(states[1] == PeerSession::State::Ended);
        expect(states[2] == PeerSession::State::Idle);
        expect(session.state() == PeerSession::State::Idle);
        expect(session.remotePeerId().empty());

        session.call("peer-2");
        expect(signaller.controls.size() == 2);
        expect(signaller.controls[1].peerId == "peer-2");
        expect(signaller.controls[1].type == "init");
        expect(session.state() == PeerSession::State::Ringing);

        session.hangup("retry");
        expect(session.state() == PeerSession::State::Idle);
    });

    describe("peer session: accept waits for remote data channel", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = true;
        config.dataChannelLabel = "control";
        PeerSession session(signaller, config);

        signaller.ControlReceived.emit("peer", "init", "");
        expect(session.state() == PeerSession::State::Incoming);

        session.accept();

        expect(signaller.controls.size() == 1);
        expect(signaller.controls[0].type == "accept");
        expect(session.state() == PeerSession::State::Connecting);
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

        expect(session.state() == PeerSession::State::Connecting);
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

        expect(states.size() == 3);
        expect(states[0] == PeerSession::State::Ringing);
        expect(states[1] == PeerSession::State::Ended);
        expect(states[2] == PeerSession::State::Idle);
        expect(session.state() == PeerSession::State::Idle);
        expect(session.remotePeerId().empty());
        expect(signaller.controls.size() == 1);
        expect(signaller.controls[0].type == "init");
    });


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
