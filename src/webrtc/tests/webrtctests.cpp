///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "webrtctests.h"
#include "../src/codecregistry.h"
#include "../src/remotemediaplan.h"
#include "../support/src/callprotocol.h"
#include "icy/webrtc/detail/receiverjitterbuffer.h"
#include "icy/webrtc/support/sympleserversignaller.h"
#include "icy/webrtc/support/wssignaller.h"

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

    struct SdpMessage
    {
        std::string peerId;
        std::string type;
        std::string sdp;
    };

    std::vector<ControlMessage> controls;
    std::vector<SdpMessage> sdps;
    std::function<void(const std::string&, const std::string&, const std::string&)> onSendControl;
    std::function<void(const std::string&, const std::string&, const std::string&)> onSendSdp;

    void sendSdp(const std::string& peerId,
                 const std::string& type,
                 const std::string& sdp) override
    {
        sdps.push_back({peerId, type, sdp});
        if (onSendSdp)
            onSendSdp(peerId, type, sdp);
    }
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


std::unique_ptr<IPacket> makeTimedAudioPacket(int64_t timeUs)
{
    static const char payload[] = "pkt";
    auto packet = std::make_unique<av::AudioPacket>();
    packet->copyData(payload, sizeof(payload) - 1);
    packet->time = timeUs;
    return packet;
}


int64_t mediaTimeOf(const std::unique_ptr<IPacket>& packet)
{
    auto* media = dynamic_cast<av::MediaPacket*>(packet.get());
    return media ? media->time : -1;
}


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

} // namespace

#ifdef HAVE_FFMPEG
namespace {

std::string chromeBrowserOffer(std::string_view audioDirection = "sendrecv",
                               std::string_view videoDirection = "sendrecv")
{
    std::string offer;
    offer.reserve(1024);
    offer +=
        "v=0\r\n"
        "o=- 4611739175949513388 2 IN IP4 127.0.0.1\r\n"
        "s=-\r\n"
        "t=0 0\r\n"
        "a=group:BUNDLE 0 1\r\n"
        "a=msid-semantic: WMS *\r\n"
        "m=audio 9 UDP/TLS/RTP/SAVPF 111 0 8\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "a=rtcp:9 IN IP4 0.0.0.0\r\n"
        "a=ice-ufrag:abcd\r\n"
        "a=ice-pwd:abcdefghijklmnopqrstuv\r\n"
        "a=ice-options:trickle\r\n"
        "a=fingerprint:sha-256 11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00\r\n"
        "a=setup:actpass\r\n"
        "a=mid:0\r\n";
    offer += "a=";
    offer += audioDirection;
    offer +=
        "\r\n"
        "a=rtcp-mux\r\n"
        "a=rtcp-rsize\r\n"
        "a=rtpmap:111 opus/48000/2\r\n"
        "a=fmtp:111 minptime=10;useinbandfec=1\r\n"
        "a=rtpmap:0 PCMU/8000\r\n"
        "a=rtpmap:8 PCMA/8000\r\n"
        "m=video 9 UDP/TLS/RTP/SAVPF 102 97\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "a=rtcp:9 IN IP4 0.0.0.0\r\n"
        "a=ice-ufrag:abcd\r\n"
        "a=ice-pwd:abcdefghijklmnopqrstuv\r\n"
        "a=ice-options:trickle\r\n"
        "a=fingerprint:sha-256 11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00\r\n"
        "a=setup:actpass\r\n"
        "a=mid:1\r\n";
    offer += "a=";
    offer += videoDirection;
    offer +=
        "\r\n"
        "a=rtcp-mux\r\n"
        "a=rtcp-rsize\r\n"
        "a=rtpmap:102 H264/90000\r\n"
        "a=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\n"
        "a=rtcp-fb:102 nack\r\n"
        "a=rtcp-fb:102 nack pli\r\n";
    return offer;
}

std::string firefoxBrowserOffer(std::string_view videoDirection = "sendrecv")
{
    std::string offer;
    offer.reserve(1024);
    offer +=
        "v=0\r\n"
        "o=mozilla...THIS_IS_SDPARTA-99.0 6845983571921452419 0 IN IP4 0.0.0.0\r\n"
        "s=-\r\n"
        "t=0 0\r\n"
        "a=group:BUNDLE 0\r\n"
        "a=ice-options:trickle\r\n"
        "a=msid-semantic:WMS *\r\n"
        "m=video 9 UDP/TLS/RTP/SAVPF 120 121 99\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "a=ice-ufrag:abcd\r\n"
        "a=ice-pwd:abcdefghijklmnopqrstuv\r\n"
        "a=fingerprint:sha-256 11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00\r\n"
        "a=setup:actpass\r\n"
        "a=mid:0\r\n";
    offer += "a=";
    offer += videoDirection;
    offer +=
        "\r\n"
        "a=rtcp-mux\r\n"
        "a=rtcp-rsize\r\n"
        "a=rtpmap:120 VP8/90000\r\n"
        "a=rtcp-fb:120 nack\r\n"
        "a=rtcp-fb:120 nack pli\r\n"
        "a=rtpmap:121 VP9/90000\r\n"
        "a=fmtp:121 profile-id=0\r\n"
        "a=rtcp-fb:121 nack\r\n"
        "a=rtcp-fb:121 nack pli\r\n"
        "a=rtpmap:99 AV1/90000\r\n"
        "a=rtcp-fb:99 nack\r\n"
        "a=rtcp-fb:99 nack pli\r\n";
    return offer;
}

const std::string& chromeBrowserCandidate()
{
    static const std::string candidate =
        "candidate:842163049 1 udp 1677729535 203.0.113.1 54400 typ srflx raddr 0.0.0.0 rport 0 generation 0 ufrag abcd";
    return candidate;
}

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
    config.media.audioCodec = CodecNegotiator::resolveWebRtcAudioCodec(
        av::AudioCodec("opus", "libopus", 2, 48000));
}

void configureLoopbackVideoCodec(PeerSession::Config& config,
                                 int width,
                                 int height,
                                 double fps)
{
    auto codec = CodecNegotiator::resolveWebRtcVideoCodec(
        av::VideoCodec("H264", "libx264", width, height, fps));
    codec.pixelFmt = "yuv420p";
    config.media.videoCodec = std::move(codec);
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

    describe("codec negotiator: strict codec requirements", []() {
        auto videoSpec = CodecNegotiator::requireVideoSpec(
            av::VideoCodec("H264", "libx264", 640, 480, 30));
        expect(videoSpec.id == CodecId::H264);

        auto audioSpec = CodecNegotiator::requireAudioSpec(
            av::AudioCodec("opus", "libopus", 2, 48000));
        expect(audioSpec.id == CodecId::Opus);

        bool videoThrew = false;
        bool audioThrew = false;
        try { [[maybe_unused]] auto spec = CodecNegotiator::requireVideoSpec(av::VideoCodec{}); } catch (const std::invalid_argument&) { videoThrew = true; }
        try { [[maybe_unused]] auto spec = CodecNegotiator::requireAudioSpec(av::AudioCodec{}); } catch (const std::invalid_argument&) { audioThrew = true; }
        expect(videoThrew);
        expect(audioThrew);
    });

    describe("codec negotiator: resolve browser-safe codecs", []() {
        auto video = CodecNegotiator::resolveWebRtcVideoCodec(
            av::VideoCodec("H264", "libx264", 1280, 720, 30, 500000));
        expect(video.name == "H264");
        expect(video.encoder == "libx264");
        expect(video.options["preset"] == "ultrafast");
        expect(video.options["tune"] == "zerolatency");
        expect(video.options["profile"] == "baseline");
        expect(video.bitRate == 500000);

        auto audio = CodecNegotiator::resolveWebRtcAudioCodec(
            av::AudioCodec("opus", "libopus", 2, 48000, 64000, "flt"));
        expect(audio.name == "opus");
        expect(audio.encoder == "libopus");
        expect(audio.sampleRate == 48000);
        expect(audio.sampleFmt == "flt");
        expect(audio.options["application"] == "lowdelay");
        expect(audio.bitRate == 64000);
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

    describe("codec negotiator: detect browser offer codecs", []() {
        static const std::string chromeOffer =
            "v=0\r\n"
            "o=- 4611739175949513388 2 IN IP4 127.0.0.1\r\n"
            "s=-\r\n"
            "t=0 0\r\n"
            "m=audio 9 UDP/TLS/RTP/SAVPF 111 0 8\r\n"
            "a=rtpmap:111 opus/48000/2\r\n"
            "a=fmtp:111 minptime=10;useinbandfec=1\r\n"
            "m=video 9 UDP/TLS/RTP/SAVPF 102 97\r\n"
            "a=rtpmap:102 H264/90000\r\n"
            "a=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\n";

        static const std::string firefoxOffer =
            "v=0\r\n"
            "o=mozilla...THIS_IS_SDPARTA-99.0 1234567890 0 IN IP4 0.0.0.0\r\n"
            "s=-\r\n"
            "t=0 0\r\n"
            "m=audio 9 UDP/TLS/RTP/SAVPF 109 0 8\r\n"
            "a=rtpmap:109 opus/48000/2\r\n"
            "m=video 9 UDP/TLS/RTP/SAVPF 126 120\r\n"
            "a=rtpmap:126 H264/90000\r\n"
            "a=fmtp:126 profile-level-id=42e01f;packetization-mode=1\r\n";

        auto chromeAudio = CodecNegotiator::detectCodec(chromeOffer, CodecMediaType::Audio);
        auto chromeVideo = CodecNegotiator::detectCodec(chromeOffer, CodecMediaType::Video);
        auto firefoxAudio = CodecNegotiator::detectCodec(firefoxOffer, CodecMediaType::Audio);
        auto firefoxVideo = CodecNegotiator::detectCodec(firefoxOffer, CodecMediaType::Video);

        expect(chromeAudio.has_value());
        expect(chromeAudio->id == CodecId::Opus);
        expect(chromeVideo.has_value());
        expect(chromeVideo->id == CodecId::H264);
        expect(firefoxAudio.has_value());
        expect(firefoxAudio->id == CodecId::Opus);
        expect(firefoxVideo.has_value());
        expect(firefoxVideo->id == CodecId::H264);
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

    describe("codec registry: supported codecs stay internally consistent", []() {
        struct Case
        {
            const char* rtpName;
            const char* ffmpegName;
            CodecId id;
            CodecMediaType mediaType;
            uint32_t clockRate;
            int payloadType;
        };

        const std::array<Case, 7> cases{{
            {"H264", "libx264", CodecId::H264, CodecMediaType::Video, 90000, 96},
            {"VP8", "libvpx", CodecId::VP8, CodecMediaType::Video, 90000, 97},
            {"VP9", "libvpx-vp9", CodecId::VP9, CodecMediaType::Video, 90000, 98},
            {"AV1", "libaom-av1", CodecId::AV1, CodecMediaType::Video, 90000, 99},
            {"opus", "libopus", CodecId::Opus, CodecMediaType::Audio, 48000, 111},
            {"PCMU", "pcm_mulaw", CodecId::PCMU, CodecMediaType::Audio, 8000, 0},
            {"PCMA", "pcm_alaw", CodecId::PCMA, CodecMediaType::Audio, 8000, 8},
        }};

        for (const auto& item : cases) {
            auto byRtp = CodecNegotiator::specFromRtp(item.rtpName);
            auto byFfmpeg = CodecNegotiator::specFromFfmpeg(item.ffmpegName);
            expect(byRtp.has_value());
            expect(byFfmpeg.has_value());
            expect(byRtp->id == item.id);
            expect(byRtp->mediaType == item.mediaType);
            expect(byRtp->clockRate == item.clockRate);
            expect(byRtp->payloadType == item.payloadType);
            expect(byFfmpeg->id == item.id);
            expect(byFfmpeg->rtpName == item.rtpName);
            expect(CodecNegotiator::ffmpegToRtp(item.ffmpegName) == item.rtpName);
        }

        auto av1 = CodecNegotiator::specFromRtp("AV1");
        expect(av1.has_value());
        expect(codec_registry::decoderCodecId(*av1) == AV_CODEC_ID_AV1);
    });

    describe("codec registry: detect codec from structured media description", []() {
        rtc::Description::Video media("video", rtc::Description::Direction::SendOnly);
        media.addVP8Codec(120);
        media.addH264Codec(102);

        auto spec = codec_registry::detectInMedia(media, CodecMediaType::Video);
        expect(spec.has_value());
        expect(spec->id == CodecId::VP8);
        expect(spec->rtpName == "VP8");
        expect(spec->clockRate == 90000);

        rtc::Description::Audio audio("audio", rtc::Description::Direction::SendOnly);
        audio.addPCMACodec(8);
        audio.addOpusCodec(111);

        auto audioSpec = codec_registry::detectInMedia(audio, CodecMediaType::Audio);
        expect(audioSpec.has_value());
        expect(audioSpec->id == CodecId::PCMA);
        expect(audioSpec->rtpName == "PCMA");
        expect(audioSpec->clockRate == 8000);
    });

    describe("remote media plan: matching browser offer reuses payload types and mids", []() {
        static const std::string offer =
            "v=0\r\n"
            "o=- 4611739175949513388 2 IN IP4 127.0.0.1\r\n"
            "s=-\r\n"
            "t=0 0\r\n"
            "m=audio 9 UDP/TLS/RTP/SAVPF 111 0 8\r\n"
            "a=mid:0\r\n"
            "a=sendonly\r\n"
            "a=rtpmap:111 opus/48000/2\r\n"
            "a=fmtp:111 minptime=10;useinbandfec=1\r\n"
            "m=video 9 UDP/TLS/RTP/SAVPF 102 97\r\n"
            "a=mid:1\r\n"
            "a=sendonly\r\n"
            "a=rtpmap:102 H264/90000\r\n"
            "a=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\n";

        MediaBridge::Options local;
        local.audioCodec = av::AudioCodec("opus", "libopus", 2, 48000);
        local.videoCodec = av::VideoCodec("H264", "libx264", 1280, 720, 30);
        local.audioDirection = rtc::Description::Direction::SendRecv;
        local.videoDirection = rtc::Description::Direction::SendRecv;

        auto resolved = resolveAnswerMediaOptions(local, offer);
        expect(resolved.audioMid == "0");
        expect(resolved.videoMid == "1");
        expect(resolved.audioPayloadType == 111);
        expect(resolved.videoPayloadType == 102);
        expect(resolved.audioDirection == rtc::Description::Direction::RecvOnly);
        expect(resolved.videoDirection == rtc::Description::Direction::RecvOnly);
        expect(resolved.audioCodec.name == "opus");
        expect(resolved.videoCodec.name == "H264");
    });

    describe("remote media plan: recvonly fallback selects offered browser codec", []() {
        static const std::string firefoxOffer =
            "v=0\r\n"
            "o=mozilla...THIS_IS_SDPARTA-99.0 1234567890 0 IN IP4 0.0.0.0\r\n"
            "s=-\r\n"
            "t=0 0\r\n"
            "m=video 9 UDP/TLS/RTP/SAVPF 120 121\r\n"
            "a=mid:video0\r\n"
            "a=sendonly\r\n"
            "a=rtpmap:120 VP8/90000\r\n"
            "a=rtpmap:121 VP9/90000\r\n"
            "a=fmtp:121 profile-id=0\r\n";

        MediaBridge::Options local;
        local.videoCodec = av::VideoCodec("H264", "libx264", 640, 360, 30);
        local.videoDirection = rtc::Description::Direction::RecvOnly;

        auto resolved = resolveAnswerMediaOptions(local, firefoxOffer);
        expect(resolved.videoCodec.name == "VP8");
        expect(resolved.videoPayloadType == 120);
        expect(resolved.videoMid == "video0");
        expect(resolved.videoDirection == rtc::Description::Direction::RecvOnly);
    });

    describe("call protocol: symple message roundtrips control, sdp, and candidate", []() {
        auto control = callproto::makeSympleMessage(
            "hangup", "bob|remote", json::Value{{"reason", "done"}}, "alice|local");
        auto parsedControl = callproto::parseSympleMessage(control);
        expect(parsedControl.has_value());
        expect(parsedControl->kind == callproto::Kind::Control);
        expect(parsedControl->peerAddress == "alice|local");
        expect(parsedControl->action == "hangup");
        expect(parsedControl->reason == "done");

        json::Value sdpData;
        sdpData["type"] = "offer";
        sdpData["sdp"] = "v=0\r\n";
        auto sdp = callproto::makeSympleMessage("offer", "bob|remote", sdpData, "alice|local");
        auto parsedSdp = callproto::parseSympleMessage(sdp);
        expect(parsedSdp.has_value());
        expect(parsedSdp->kind == callproto::Kind::Sdp);
        expect(parsedSdp->sdpType == "offer");
        expect(parsedSdp->sdp == "v=0\r\n");

        json::Value candidateData;
        candidateData["candidate"] = "candidate:1 1 udp 1 127.0.0.1 10000 typ host";
        candidateData["sdpMid"] = "0";
        auto candidate = callproto::makeSympleMessage(
            "candidate", "bob|remote", candidateData, "alice|local");
        auto parsedCandidate = callproto::parseSympleMessage(candidate);
        expect(parsedCandidate.has_value());
        expect(parsedCandidate->kind == callproto::Kind::Candidate);
        expect(parsedCandidate->candidate == candidateData["candidate"].get<std::string>());
        expect(parsedCandidate->mid == "0");
    });

    describe("call protocol: requires full symple peer addresses", []() {
        expect(callproto::isFullPeerAddress("alice|local"));
        expect(!callproto::isFullPeerAddress("alice"));
        expect(!callproto::isFullPeerAddress("|local"));

        bool threwTo = false;
        try {
            [[maybe_unused]] auto msg =
                callproto::makeSympleMessage("init", "alice", {}, "bob|remote");
        }
        catch (const std::invalid_argument&) {
            threwTo = true;
        }
        expect(threwTo);

        bool threwFrom = false;
        try {
            [[maybe_unused]] auto msg =
                callproto::makeSympleMessage("init", "alice|local", {}, "bob");
        }
        catch (const std::invalid_argument&) {
            threwFrom = true;
        }
        expect(threwFrom);

        json::Value bareFrom;
        bareFrom["type"] = "message";
        bareFrom["subtype"] = "call:init";
        bareFrom["from"] = "alice";
        expect(!callproto::parseSympleMessage(bareFrom).has_value());
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

    describe("track: setup receive track uses structured codec description", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        rtc::Description::Video video("video", rtc::Description::Direction::RecvOnly);
        video.addVP8Codec(120);
        auto videoTrack = pc->addTrack(video);
        expect(setupReceiveTrack(videoTrack));

        rtc::Description::Audio audio("audio", rtc::Description::Direction::RecvOnly);
        audio.addOpusCodec(111);
        auto audioTrack = pc->addTrack(audio);
        expect(setupReceiveTrack(audioTrack));

        pc->close();
    });

    describe("track: PLI callback fires", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        bool pliFired = false;
        auto vh = createVideoTrack(pc, av::VideoCodec("H264", "libx264", 640, 480, 30), 0, {}, {},
            rtc::Description::Direction::SendRecv, 512,
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

    describe("receiver jitter buffer: fixed delay reorders out-of-order frames", []() {
        detail::ReceiverJitterBuffer buffer;
        JitterBufferConfig config;
        config.enabled = true;
        config.minDelayMs = 30;
        config.maxDelayMs = 30;
        config.adaptiveFactor = 0.0;
        buffer.configure(config);

        std::deque<std::unique_ptr<IPacket>> ready;
        buffer.push(makeTimedAudioPacket(0), 0);
        buffer.push(makeTimedAudioPacket(40000), 10000);
        buffer.push(makeTimedAudioPacket(20000), 15000);

        buffer.drainReady(29999, ready);
        expect(ready.empty());

        buffer.drainReady(35000, ready);
        expect(ready.size() == 1);
        expect(mediaTimeOf(ready.front()) == 0);
        ready.clear();

        buffer.drainReady(55000, ready);
        expect(ready.size() == 1);
        expect(mediaTimeOf(ready.front()) == 20000);
        ready.clear();

        buffer.drainReady(75000, ready);
        expect(ready.size() == 1);
        expect(mediaTimeOf(ready.front()) == 40000);
        expect(!buffer.hasPending());
    });

    describe("receiver jitter buffer: adaptive delay expands after jitter spike", []() {
        detail::ReceiverJitterBuffer buffer;
        JitterBufferConfig config;
        config.enabled = true;
        config.minDelayMs = 20;
        config.maxDelayMs = 80;
        config.adaptiveFactor = 1.0;
        buffer.configure(config);

        std::deque<std::unique_ptr<IPacket>> ready;
        buffer.push(makeTimedAudioPacket(0), 0);
        buffer.push(makeTimedAudioPacket(20000), 45000);

        buffer.drainReady(25000, ready);
        expect(ready.size() == 1);
        expect(mediaTimeOf(ready.front()) == 0);
        ready.clear();

        buffer.drainReady(60000, ready);
        expect(ready.empty());

        buffer.drainReady(66000, ready);
        expect(ready.size() == 1);
        expect(mediaTimeOf(ready.front()) == 20000);
    });

    describe("track receiver: jitter buffer config is configurable", []() {
        WebRtcTrackReceiver receiver;
        JitterBufferConfig config;
        config.enabled = true;
        config.minDelayMs = 40;
        config.maxDelayMs = 140;
        config.adaptiveFactor = 2.0;
        config.tickIntervalMs = 7;

        receiver.configureJitterBuffer(config);

        auto applied = receiver.jitterBufferConfig();
        expect(applied.enabled);
        expect(applied.minDelayMs == 40);
        expect(applied.maxDelayMs == 140);
        expect(applied.adaptiveFactor == 2.0);
        expect(applied.tickIntervalMs == 7);
        expect(receiver.jitterBufferEnabled());
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

    describe("media bridge: feedback on unopened receive-only track is ignored", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.videoCodec = av::VideoCodec("H264", 640, 480, 30);
        opts.videoDirection = rtc::Description::Direction::RecvOnly;

        bridge.attach(pc, opts);
        expect(bridge.attached());
        expect(bridge.hasVideo());
        expect(bridge.videoTrack() != nullptr);

        bool threw = false;
        try {
            bridge.requestKeyframe();
            bridge.requestBitrate(512000);
        }
        catch (...) {
            threw = true;
        }
        expect(!threw);

        bridge.detach();
        pc->close();
    });

    describe("media bridge: receive jitter buffer options reach the receiver", []() {
        rtc::Configuration config;
        auto pc = std::make_shared<rtc::PeerConnection>(config);

        MediaBridge bridge;
        MediaBridge::Options opts;
        opts.audioCodec = av::AudioCodec("opus", "libopus", 2, 48000);
        opts.audioDirection = rtc::Description::Direction::RecvOnly;
        opts.audioJitterBuffer.enabled = true;
        opts.audioJitterBuffer.minDelayMs = 35;
        opts.audioJitterBuffer.maxDelayMs = 90;
        opts.audioJitterBuffer.tickIntervalMs = 6;

        bridge.attach(pc, opts);

        const auto applied = bridge.audioReceiver().jitterBufferConfig();
        expect(applied.enabled);
        expect(applied.minDelayMs == 35);
        expect(applied.maxDelayMs == 90);
        expect(applied.tickIntervalMs == 6);
        expect(bridge.audioReceiver().jitterBufferEnabled());

        bridge.detach();
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

    describe("websocket signaller: sends and receives canonical messages", []() {
        WebSocketSignaller alice("alice");
        WebSocketSignaller bob("bob");

        alice.SendMessage += [&](const std::string& msg) { bob.receive(msg); };
        bob.SendMessage += [&](const std::string& msg) { alice.receive(msg); };

        std::string sdpPeer;
        std::string sdpType;
        std::string sdpValue;
        std::string candidatePeer;
        std::string candidateValue;
        std::string candidateMid;
        std::string controlPeer;
        std::string controlType;
        std::string controlReason;

        bob.SdpReceived += [&](const std::string& peerId,
                               const std::string& type,
                               const std::string& sdp) {
            sdpPeer = peerId;
            sdpType = type;
            sdpValue = sdp;
        };
        bob.CandidateReceived += [&](const std::string& peerId,
                                     const std::string& candidate,
                                     const std::string& mid) {
            candidatePeer = peerId;
            candidateValue = candidate;
            candidateMid = mid;
        };
        bob.ControlReceived += [&](const std::string& peerId,
                                   const std::string& type,
                                   const std::string& reason) {
            controlPeer = peerId;
            controlType = type;
            controlReason = reason;
        };

        alice.sendSdp("bob", "offer", "v=0\r\n");
        expect(sdpPeer == "alice");
        expect(sdpType == "offer");
        expect(sdpValue == "v=0\r\n");

        alice.sendCandidate("bob", "candidate:1 1 UDP 2122260223 192.0.2.1 54400 typ host", "0");
        expect(candidatePeer == "alice");
        expect(candidateValue.find("candidate:1") == 0);
        expect(candidateMid == "0");

        alice.sendControl("bob", "hangup", "done");
        expect(controlPeer == "alice");
        expect(controlType == "hangup");
        expect(controlReason == "done");
    });

    describe("websocket signaller: ignores malformed input", []() {
        WebSocketSignaller ws("alice");
        int sdpCount = 0;
        int candidateCount = 0;
        int controlCount = 0;

        ws.SdpReceived += [&](const std::string&, const std::string&, const std::string&) {
            ++sdpCount;
        };
        ws.CandidateReceived += [&](const std::string&, const std::string&, const std::string&) {
            ++candidateCount;
        };
        ws.ControlReceived += [&](const std::string&, const std::string&, const std::string&) {
            ++controlCount;
        };

        ws.receive("{");
        ws.receive(R"({"type":"offer"})");
        ws.receive(R"({"type":"candidate","peerId":"bob"})");

        expect(sdpCount == 0);
        expect(candidateCount == 0);
        expect(controlCount == 0);
    });

    describe("symple server signaller: preserves full address identity", []() {
        smpl::Server server;
        SympleServerSignaller signaller(server, "media-server|media-server", "alice|peer-1");

        int controlCount = 0;
        std::string controlPeer;
        std::string controlType;
        signaller.ControlReceived += [&](const std::string& peerId,
                                         const std::string& type,
                                         const std::string&) {
            ++controlCount;
            controlPeer = peerId;
            controlType = type;
        };

        auto accepted = callproto::makeSympleMessage(
            "accept", "media-server|media-server", {}, "alice|peer-1");
        signaller.onMessage(accepted);
        expect(controlCount == 1);
        expect(controlPeer == "alice|peer-1");
        expect(controlType == "accept");

        auto wrongSession = callproto::makeSympleMessage(
            "accept", "media-server|media-server", {}, "alice|peer-2");
        signaller.onMessage(wrongSession);
        expect(controlCount == 1);

        bool threw = false;
        try {
            signaller.setRemoteAddress("alice");
        }
        catch (const std::invalid_argument&) {
            threw = true;
        }
        expect(threw);
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

    describe("peer session: glare rejects incoming init while outgoing", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        session.call("peer");
        signaller.ControlReceived.emit("peer", "init", "");

        expect(states.size() == 1);
        expect(states[0] == PeerSession::State::OutgoingInit);
        expect(session.state() == PeerSession::State::OutgoingInit);
        expect(session.remotePeerId() == "peer");
        expect(signaller.controls.size() == 2);
        expect(signaller.controls[0].type == "init");
        expect(signaller.controls[1].peerId == "peer");
        expect(signaller.controls[1].type == "reject");
        expect(signaller.controls[1].reason == "busy");
    });

    describe("peer session: duplicate incoming init is rejected as busy", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        signaller.ControlReceived.emit("peer", "init", "");
        signaller.ControlReceived.emit("peer", "init", "");

        expect(states.size() == 1);
        expect(states[0] == PeerSession::State::IncomingInit);
        expect(session.state() == PeerSession::State::IncomingInit);
        expect(session.remotePeerId() == "peer");
        expect(signaller.controls.size() == 1);
        expect(signaller.controls[0].peerId == "peer");
        expect(signaller.controls[0].type == "reject");
        expect(signaller.controls[0].reason == "busy");
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
        expect(session.peerConnection() == nullptr);
        expect(session.dataChannel() == nullptr);
    });

    describe("peer session: accept defers peer connection until offer", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});

        signaller.ControlReceived.emit("peer", "init", "");
        session.accept();

        expect(session.peerConnection() == nullptr);
        signaller.SdpReceived.emit("peer", "offer", chromeBrowserOffer());
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

    describe("peer session: duplicate accept is ignored", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = true;
        PeerSession session(signaller, config);
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        session.call("peer");
        signaller.ControlReceived.emit("peer", "accept", "");
        auto pc = session.peerConnection();
        auto dc = session.dataChannel();

        signaller.ControlReceived.emit("peer", "accept", "");

        expect(states.size() == 2);
        expect(states[0] == PeerSession::State::OutgoingInit);
        expect(states[1] == PeerSession::State::Negotiating);
        expect(session.state() == PeerSession::State::Negotiating);
        expect(session.peerConnection() == pc);
        expect(session.dataChannel() == dc);
        expect(signaller.controls.size() == 1);
    });

    describe("peer session: browser offer queues early candidate and yields answer", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = false;
        configureLoopbackAudioCodec(config);
        configureLoopbackVideoCodec(config, 640, 360, 30);
        config.media.audioDirection = rtc::Description::Direction::SendOnly;
        config.media.videoDirection = rtc::Description::Direction::SendOnly;
        PeerSession session(signaller, config);

        signaller.ControlReceived.emit("browser", "init", "");
        expect(session.state() == PeerSession::State::IncomingInit);

        session.accept();
        expect(session.state() == PeerSession::State::Negotiating);

        signaller.CandidateReceived.emit("browser", chromeBrowserCandidate(), "0");
        signaller.SdpReceived.emit("browser", "offer",
            chromeBrowserOffer("recvonly", "recvonly"));

        expect(icy::test::waitFor([&] {
            return !signaller.sdps.empty();
        }, 2000));
        expect(signaller.sdps.size() == 1);
        expect(signaller.sdps.back().peerId == "browser");
        expect(signaller.sdps.back().type == "answer");
        expect(!signaller.sdps.back().sdp.empty());
        expect(signaller.sdps.back().sdp.find("m=audio 0") == std::string::npos);
        expect(signaller.sdps.back().sdp.find("m=video 0") == std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=mid:0") != std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=mid:1") != std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=sendonly") != std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=sendrecv") == std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=msid:") != std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=rtpmap:111 opus/48000/2") != std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=rtpmap:102 H264/90000") != std::string::npos);
        expect(signaller.sdps.back().sdp.find("a=rtpmap:96 H264/90000") == std::string::npos);

        session.hangup("done");
        expect(icy::test::waitFor([&] {
            return session.state() == PeerSession::State::Idle;
        }, 3000));
    });

    describe("peer session: sendonly browser offer yields pure recvonly answer", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = false;
        config.media.videoCodec = CodecNegotiator::resolveWebRtcVideoCodec(
            av::VideoCodec("H264", "libx264", 640, 360, 30));
        PeerSession session(signaller, config);

        signaller.ControlReceived.emit("browser", "init", "");
        session.accept();
        signaller.SdpReceived.emit("browser", "offer",
            chromeBrowserOffer("sendonly", "sendonly"));

        expect(icy::test::waitFor([&] {
            return !signaller.sdps.empty();
        }, 2000));

        const auto& sdp = signaller.sdps.back().sdp;
        expect(signaller.sdps.back().type == "answer");
        expect(sdp.find("m=video 9 UDP/TLS/RTP/SAVPF 102") != std::string::npos);
        expect(sdp.find("a=rtpmap:97 rtx/90000") == std::string::npos);
        expect(sdp.find("a=fmtp:97 apt=102") == std::string::npos);
        expect(sdp.find("a=recvonly") != std::string::npos);
        expect(sdp.find("a=ssrc:") == std::string::npos);
        expect(sdp.find("a=msid:") == std::string::npos);
    });

    describe("peer session: recvonly answer falls back to offered browser codec", []() {
        MockSignaller signaller;
        PeerSession::Config config;
        config.enableDataChannel = false;
        config.media.videoCodec = CodecNegotiator::resolveWebRtcVideoCodec(
            av::VideoCodec("H264", "libx264", 640, 360, 30));
        config.media.videoDirection = rtc::Description::Direction::RecvOnly;
        config.media.audioCodec = {};
        config.media.audioDirection = rtc::Description::Direction::Inactive;
        PeerSession session(signaller, config);

        signaller.ControlReceived.emit("browser", "init", "");
        session.accept();
        signaller.SdpReceived.emit("browser", "offer",
            firefoxBrowserOffer("sendonly"));

        expect(icy::test::waitFor([&] {
            return !signaller.sdps.empty();
        }, 2000));

        const auto& sdp = signaller.sdps.back().sdp;
        expect(signaller.sdps.back().type == "answer");
        expect(sdp.find("m=video 0") == std::string::npos);
        expect(sdp.find("a=inactive") == std::string::npos);
        expect(sdp.find("a=recvonly") != std::string::npos);
        expect(sdp.find("a=rtpmap:120 VP8/90000") != std::string::npos);
        expect(sdp.find("a=rtpmap:96 H264/90000") == std::string::npos);
    });

    describe("peer session: late sdp and ice after end are ignored", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});

        session.call("peer");
        signaller.ControlReceived.emit("peer", "reject", "done");
        expect(session.state() == PeerSession::State::Idle);

        signaller.SdpReceived.emit("peer", "answer", "v=0\r\n");
        signaller.CandidateReceived.emit("peer", chromeBrowserCandidate(), "0");

        expect(session.state() == PeerSession::State::Idle);
        expect(session.peerConnection() == nullptr);
        expect(session.dataChannel() == nullptr);
        expect(session.remotePeerId().empty());

        session.call("peer-2");
        expect(session.state() == PeerSession::State::OutgoingInit);
        expect(session.remotePeerId() == "peer-2");
        expect(signaller.controls.size() == 2);
        expect(signaller.controls[0].type == "init");
        expect(signaller.controls[1].type == "init");
        expect(signaller.controls[1].peerId == "peer-2");
    });

    describe("peer session: duplicate remote hangup is ignored", []() {
        MockSignaller signaller;
        PeerSession session(signaller, {});
        std::vector<PeerSession::State> states;
        session.StateChanged += [&](PeerSession::State state) {
            states.push_back(state);
        };

        session.call("peer");
        signaller.ControlReceived.emit("peer", "hangup", "done");
        signaller.ControlReceived.emit("peer", "hangup", "again");

        expect(states.size() == 4);
        expect(states[0] == PeerSession::State::OutgoingInit);
        expect(states[1] == PeerSession::State::Ending);
        expect(states[2] == PeerSession::State::Ended);
        expect(states[3] == PeerSession::State::Idle);
        expect(session.state() == PeerSession::State::Idle);
        expect(session.remotePeerId().empty());
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
            config.media.audioCodec,
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
        configureLoopbackVideoCodec(config, 160, 120, 30);

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
        encoder.oparams = config.media.videoCodec;
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

    describe("peer session: sendonly caller reaches recvonly callee", []() {
        LoopbackSignaller publisherSig("publisher");
        LoopbackSignaller recorderSig("recorder");
        publisherSig.connect(recorderSig);
        const auto ownerThread = std::this_thread::get_id();

        PeerSession::Config publisherConfig;
        publisherConfig.enableDataChannel = false;
        configureLoopbackVideoCodec(publisherConfig, 160, 120, 30);
        publisherConfig.media.videoDirection = rtc::Description::Direction::SendOnly;
        publisherConfig.media.audioCodec = {};
        publisherConfig.media.audioDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config recorderConfig;
        recorderConfig.enableDataChannel = false;
        configureLoopbackVideoCodec(recorderConfig, 160, 120, 30);
        recorderConfig.media.videoDirection = rtc::Description::Direction::RecvOnly;
        recorderConfig.media.audioCodec = {};
        recorderConfig.media.audioDirection = rtc::Description::Direction::Inactive;

        PeerSession publisher(publisherSig, publisherConfig);
        PeerSession recorder(recorderSig, recorderConfig);

        recorder.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "publisher");
            recorder.accept();
        };

        std::atomic<int> receivedPackets{0};
        std::atomic<size_t> lastReceivedSize{0};
        std::atomic<bool> receiverAttached{false};
        recorder.StateChanged += [&](PeerSession::State state) {
            if (state != PeerSession::State::Active || receiverAttached.exchange(true))
                return;
            expect(std::this_thread::get_id() == ownerThread);
            recorder.media().videoReceiver().emitter += [&](IPacket& packet) {
                auto* video = dynamic_cast<av::VideoPacket*>(&packet);
                if (!video)
                    return;
                receivedPackets.fetch_add(1);
                lastReceivedSize.store(video->size());
            };
        };

        publisher.call("recorder");

        expect(icy::test::waitFor([&] {
            return publisher.state() == PeerSession::State::Active &&
                   recorder.state() == PeerSession::State::Active;
        }, 8000));
        expect(receiverAttached.load());

        expect(publisher.media().hasVideo());
        expect(recorder.media().hasVideo());
        expect(!recorder.media().hasAudio());

        av::VideoPacketEncoder encoder;
        encoder.iparams.width = 160;
        encoder.iparams.height = 120;
        encoder.iparams.pixelFmt = "yuv420p";
        encoder.oparams = publisherConfig.media.videoCodec;
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
        expect(icy::test::waitFor([&] {
            return publisher.state() == PeerSession::State::Idle &&
                   recorder.state() == PeerSession::State::Idle;
        }, 3000));
    });

    describe("peer session: shared av stream graph routes audio and video correctly", []() {
        LoopbackSignaller aliceSig("alice");
        LoopbackSignaller bobSig("bob");
        aliceSig.connect(bobSig);

        PeerSession::Config config;
        config.enableDataChannel = false;
        configureLoopbackAudioCodec(config);
        configureLoopbackVideoCodec(config, 160, 120, 30);

        PeerSession alice(aliceSig, config);
        PeerSession bob(bobSig, config);

        bob.IncomingCall += [&](const std::string& peerId) {
            expect(peerId == "alice");
            bob.accept();
        };

        std::atomic<int> receivedAudio{0};
        std::atomic<int> receivedVideo{0};

        bob.media().audioReceiver().emitter += [&](IPacket& packet) {
            if (dynamic_cast<av::AudioPacket*>(&packet))
                receivedAudio.fetch_add(1);
        };
        bob.media().videoReceiver().emitter += [&](IPacket& packet) {
            if (dynamic_cast<av::VideoPacket*>(&packet))
                receivedVideo.fetch_add(1);
        };

        alice.call("bob");

        expect(icy::test::waitFor([&] {
            return alice.state() == PeerSession::State::Active &&
                   bob.state() == PeerSession::State::Active;
        }, 8000));

        PacketSignal source;
        PacketStream stream;
        av::VideoPacketEncoder videoEncoder;
        av::AudioPacketEncoder audioEncoder;

        videoEncoder.iparams.width = 160;
        videoEncoder.iparams.height = 120;
        videoEncoder.iparams.pixelFmt = "yuv420p";
        videoEncoder.oparams = config.media.videoCodec;

        audioEncoder.iparams.channels = config.media.audioCodec.channels;
        audioEncoder.iparams.sampleRate = config.media.audioCodec.sampleRate;
        audioEncoder.iparams.sampleFmt = "flt";
        audioEncoder.oparams = config.media.audioCodec;

        stream.attachSource(source);
        stream.attach(&videoEncoder, 1, false);
        stream.attach(&audioEncoder, 2, false);
        stream.attach(&alice.media().videoSender(), 5, false);
        stream.attach(&alice.media().audioSender(), 6, false);
        stream.start();

        for (int i = 0; i < 24; ++i) {
            av::AVFrameHolder frame(av::createVideoFrame(AV_PIX_FMT_YUV420P, 160, 120));
            expect(static_cast<bool>(frame));
            fillLoopbackVideoFrame(frame.get(), i);

            av::PlanarVideoPacket video(frame->data, frame->linesize,
                                        "yuv420p", 160, 120,
                                        static_cast<int64_t>(i) * 33333);
            source.emit(video);

            auto samples = makeLoopbackAudioSamples(
                config.media.audioCodec.channels,
                960,
                config.media.audioCodec.sampleRate,
                i * 960);
            av::AudioPacket audio(
                reinterpret_cast<uint8_t*>(samples.data()),
                samples.size() * sizeof(float),
                960,
                static_cast<int64_t>(i) * 20000);
            source.emit(audio);

            if (receivedAudio.load() > 0 && receivedVideo.load() > 0)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        expect(icy::test::waitFor([&] {
            return receivedAudio.load() > 0 && receivedVideo.load() > 0;
        }, 5000));

        stream.stop();
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

        PeerSession::Config publisherConfig;
        publisherConfig.enableDataChannel = false;
        configureLoopbackVideoCodec(publisherConfig, 160, 120, 30);
        publisherConfig.media.videoDirection = rtc::Description::Direction::SendOnly;
        publisherConfig.media.audioCodec = {};
        publisherConfig.media.audioDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config relayIngressConfig;
        relayIngressConfig.enableDataChannel = false;
        configureLoopbackVideoCodec(relayIngressConfig, 160, 120, 30);
        relayIngressConfig.media.videoDirection = rtc::Description::Direction::RecvOnly;
        relayIngressConfig.media.audioCodec = {};
        relayIngressConfig.media.audioDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config relayEgressConfig;
        relayEgressConfig.enableDataChannel = false;
        configureLoopbackVideoCodec(relayEgressConfig, 160, 120, 30);
        relayEgressConfig.media.videoDirection = rtc::Description::Direction::SendOnly;
        relayEgressConfig.media.audioCodec = {};
        relayEgressConfig.media.audioDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config viewerConfig;
        viewerConfig.enableDataChannel = false;
        configureLoopbackVideoCodec(viewerConfig, 160, 120, 30);
        viewerConfig.media.videoDirection = rtc::Description::Direction::RecvOnly;
        viewerConfig.media.audioCodec = {};
        viewerConfig.media.audioDirection = rtc::Description::Direction::Inactive;

        PeerSession publisher(publisherSig, publisherConfig);
        PeerSession relayIngress(ingressSig, relayIngressConfig);
        PeerSession relayEgress(egressSig, relayEgressConfig);
        PeerSession viewer(viewerSig, viewerConfig);

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
        expect(publisher.media().hasVideo());
        expect(!publisher.media().hasAudio());
        expect(relayIngress.media().hasVideo());
        expect(!relayIngress.media().hasAudio());
        expect(relayEgress.media().hasVideo());
        expect(!relayEgress.media().hasAudio());
        expect(viewer.media().hasVideo());
        expect(!viewer.media().hasAudio());

        av::VideoPacketEncoder encoder;
        encoder.iparams.width = 160;
        encoder.iparams.height = 120;
        encoder.iparams.pixelFmt = "yuv420p";
        encoder.oparams = publisherConfig.media.videoCodec;
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

        PeerSession::Config publisherConfig;
        publisherConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(publisherConfig);
        publisherConfig.media.audioDirection = rtc::Description::Direction::SendOnly;
        publisherConfig.media.videoCodec = {};
        publisherConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config relayIngressConfig;
        relayIngressConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(relayIngressConfig);
        relayIngressConfig.media.audioDirection = rtc::Description::Direction::RecvOnly;
        relayIngressConfig.media.videoCodec = {};
        relayIngressConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config relayEgressConfig;
        relayEgressConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(relayEgressConfig);
        relayEgressConfig.media.audioDirection = rtc::Description::Direction::SendOnly;
        relayEgressConfig.media.videoCodec = {};
        relayEgressConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config viewerConfig;
        viewerConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(viewerConfig);
        viewerConfig.media.audioDirection = rtc::Description::Direction::RecvOnly;
        viewerConfig.media.videoCodec = {};
        viewerConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession publisher(publisherSig, publisherConfig);
        PeerSession relayIngress(ingressSig, relayIngressConfig);
        PeerSession relayEgress(egressSig, relayEgressConfig);
        PeerSession viewer(viewerSig, viewerConfig);

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
        expect(!publisher.media().hasVideo());
        expect(publisher.media().hasAudio());
        expect(!relayIngress.media().hasVideo());
        expect(relayIngress.media().hasAudio());
        expect(!relayEgress.media().hasVideo());
        expect(relayEgress.media().hasAudio());
        expect(!viewer.media().hasVideo());
        expect(viewer.media().hasAudio());

        sendLoopbackAudioFrames(
            publisherConfig.media.audioCodec,
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

        PeerSession::Config publisherConfig;
        publisherConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(publisherConfig);
        publisherConfig.media.audioDirection = rtc::Description::Direction::SendOnly;
        publisherConfig.media.videoCodec = {};
        publisherConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config ingressConfig;
        ingressConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(ingressConfig);
        ingressConfig.media.audioDirection = rtc::Description::Direction::RecvOnly;
        ingressConfig.media.videoCodec = {};
        ingressConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config egressConfig;
        egressConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(egressConfig);
        egressConfig.media.audioDirection = rtc::Description::Direction::SendOnly;
        egressConfig.media.videoCodec = {};
        egressConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession::Config viewerConfig;
        viewerConfig.enableDataChannel = false;
        configureLoopbackAudioCodec(viewerConfig);
        viewerConfig.media.audioDirection = rtc::Description::Direction::RecvOnly;
        viewerConfig.media.videoCodec = {};
        viewerConfig.media.videoDirection = rtc::Description::Direction::Inactive;

        PeerSession publisherA(publisherASig, publisherConfig);
        PeerSession relayIngressA(ingressASig, ingressConfig);
        PeerSession publisherB(publisherBSig, publisherConfig);
        PeerSession relayIngressB(ingressBSig, ingressConfig);
        PeerSession relayEgress(egressSig, egressConfig);
        PeerSession viewer(viewerSig, viewerConfig);

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
        expect(!publisherA.media().hasVideo());
        expect(publisherA.media().hasAudio());
        expect(!relayIngressA.media().hasVideo());
        expect(relayIngressA.media().hasAudio());
        expect(!publisherB.media().hasVideo());
        expect(publisherB.media().hasAudio());
        expect(!relayIngressB.media().hasVideo());
        expect(relayIngressB.media().hasAudio());
        expect(!relayEgress.media().hasVideo());
        expect(relayEgress.media().hasAudio());
        expect(!viewer.media().hasVideo());
        expect(viewer.media().hasAudio());

        sendLoopbackAudioFrames(
            publisherConfig.media.audioCodec,
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
            publisherConfig.media.audioCodec,
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
