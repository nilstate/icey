///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Webcam Streamer
//
// Captures video from a camera (or test file), encodes to H.264,
// and streams to a browser peer via WebRTC.
//
// Pipeline:
//   MediaCapture → VideoPacketEncoder → WebRtcTrackSender → [browser]
//
// The browser connects via symple-player's CallManager,
// which handles the call:init/accept/offer/answer/candidate protocol.
//
// Usage:
//   webcam-streamer -host <symple-server> -port <port> -user <id>
//
// Set USE_CAMERA to 1 to stream from a camera, or leave at 0 to
// stream from the bundled test file.
//
/// @addtogroup webrtc
/// @{


#include "icy/application.h"
#include "icy/av/devicemanager.h"
#include "icy/av/mediacapture.h"
#include "icy/av/videopacketencoder.h"
#include "icy/logger.h"
#include "icy/packetstream.h"
#include "icy/symple/client.h"
#include "icy/webrtc/peersession.h"
#include "symplesignaller.h"

#include <iostream>
#include <memory>
#include <string>


using namespace icy;

#define USE_CAMERA 0


class WebcamStreamer
{
public:
    smpl::Client client;
    std::unique_ptr<wrtc::SympleSignaller> signaller;
    std::unique_ptr<wrtc::PeerSession> session;
    std::shared_ptr<av::MediaCapture> capture;
    std::shared_ptr<av::VideoPacketEncoder> encoder;
    PacketStream stream;
    av::Device::VideoCapability videoCap;

    WebcamStreamer(const smpl::Client::Options& opts)
        : client(opts)
        , stream("webcam-stream")
    {
    }

    void start()
    {
        // Set up video capture.
#if USE_CAMERA
        // Use DeviceManager to find the best camera parameters
        av::DeviceManager devman;
        auto camResult = devman.negotiateVideoCapture("", 1280, 720, 30.0);

        capture = std::make_shared<av::MediaCapture>();
        if (camResult) {
            auto& [camera, cap] = *camResult;
            videoCap = cap;
            std::cout << "Using camera: " << camera.name
                      << " at " << cap.width << "x" << cap.height
                      << " @ " << cap.maxFps << " fps" << '\n';
            capture->openVideo(camera.id, cap.width, cap.height,
                               cap.maxFps, cap.pixelFormat);
        } else {
            std::cerr << "No camera found, falling back to test file" << '\n';
            capture->openFile(ICY_DATA_DIR "/test.mp4");
            capture->setLoopInput(true);
            capture->setLimitFramerate(true);
            videoCap = {640, 480, 30, 30, "yuv420p"};
        }
#else
        capture = std::make_shared<av::MediaCapture>();
        capture->openFile(ICY_DATA_DIR "/test.mp4");
        capture->setLoopInput(true);
        capture->setLimitFramerate(true);
        videoCap = {640, 480, 30, 30, "yuv420p"};
#endif

        client.Announce += slot(this, &WebcamStreamer::onAnnounce);
        client.StateChange += slot(this, &WebcamStreamer::onStateChange);
        client.CreatePresence += slot(this, &WebcamStreamer::onCreatePresence);

        client.connect();
    }

    void shutdown()
    {
        stream.stop();
        if (session)
            session->hangup("shutdown");
        session.reset();
        encoder.reset();
        capture.reset();
        client.close();
    }

private:
    void createSession()
    {
        // Configure the video codec for WebRTC browser playback.
        av::VideoCodec videoCodec("H264", "libx264",
            videoCap.width, videoCap.height, videoCap.maxFps);
        videoCodec.options["preset"] = "ultrafast";
        videoCodec.options["tune"] = "zerolatency";
        videoCodec.options["profile"] = "baseline";

        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        config.mediaOpts.videoCodec = videoCodec;
        // Audio left empty - video only.
        config.enableDataChannel = false;

        signaller = std::make_unique<wrtc::SympleSignaller>(client);
        session = std::make_unique<wrtc::PeerSession>(*signaller, config);

        session->IncomingCall += [this](const std::string& peerId) {
            std::cout << "Incoming call from " << peerId << '\n';
            session->accept();
        };

        session->StateChanged += [this](wrtc::PeerSession::State state) {
            std::cout << "Call state: " << wrtc::stateToString(state) << '\n';

            if (state == wrtc::PeerSession::State::Active) {
                // Connection established - start the pipeline.
                startStreaming();
            }
            else if (state == wrtc::PeerSession::State::Ended) {
                stream.stop();
            }
        };

        // Adaptive bitrate: adjust encoder when REMB arrives.
        session->media().BitrateEstimate += [](unsigned int bps) {
            std::cout << "REMB: " << bps / 1000 << " kbps" << '\n';
        };

        session->media().KeyframeRequested += []() {
            std::cout << "PLI: keyframe requested" << '\n';
        };
    }

    void startStreaming()
    {
        if (!session || !session->media().hasVideo())
            return;

        // Create encoder: decoded frames → H.264 NAL units
        encoder = std::make_shared<av::VideoPacketEncoder>();

        // Input params: the decoded format from the capture source
        capture->getEncoderVideoCodec(encoder->iparams);

        // Output params: H.264 for WebRTC with browser-safe settings
        encoder->oparams = av::VideoCodec("H264", "libx264",
            videoCap.width, videoCap.height, videoCap.maxFps);
        encoder->oparams.options["preset"] = "ultrafast";
        encoder->oparams.options["tune"] = "zerolatency";
        encoder->oparams.options["profile"] = "baseline";

        // Pipeline: capture → encoder → WebRTC sender
        stream.attachSource(capture.get(), false, true);
        stream.attach(encoder, 1, true);
        stream.attach(&session->media().videoSender(), 5, false);
        stream.start();

        capture->start();
        std::cout << "Streaming started" << '\n';
    }

    void onAnnounce(const int& status)
    {
        if (status != 200)
            std::cerr << "Auth failed: " << status << '\n';
    }

    void onStateChange(void*, smpl::ClientState& state, const smpl::ClientState&)
    {
        std::cout << "Client: " << state.toString() << '\n';
        if (state.id() == smpl::ClientState::Online) {
            std::cout << "Online as " << client.ourID() << '\n';
            client.joinRoom("public");
            createSession();
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "Icey";
        peer["type"] = "streamer";
    }
};


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    smpl::Client::Options opts;
    opts.host = "127.0.0.1";
    opts.port = 4500;

    for (int i = 1; i + 1 < argc; i += 2) {
        std::string key = argv[i];
        std::string val = argv[i + 1];
        if (key == "-host") opts.host = val;
        else if (key == "-port") opts.port = static_cast<uint16_t>(std::stoi(val));
        else if (key == "-token") opts.token = val;
        else if (key == "-user") opts.user = val;
        else if (key == "-name") opts.name = val;
    }

    if (opts.user.empty()) {
        opts.user = "webcam-streamer";
        opts.name = "Webcam Streamer";
    }

    WebcamStreamer app(opts);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<WebcamStreamer*>(opaque)->shutdown();
    }, &app);

    Logger::destroy();
    return 0;
}
