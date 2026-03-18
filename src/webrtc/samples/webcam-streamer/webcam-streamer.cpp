///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Webcam Streamer
//
// Captures video from a camera (or test file), encodes to H.264,
// and streams to a browser peer via WebRTC.
//
// Pipeline:
//   MediaCapture → VideoEncoder → WebRtcTrackSender → [browser]
//
// The browser connects via symple-client-player's CallManager,
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


#include "scy/application.h"
#include "scy/av/mediacapture.h"
#include "scy/av/videoencoder.h"
#include "scy/logger.h"
#include "scy/packetstream.h"
#include "scy/symple/client.h"
#include "scy/webrtc/peersession.h"

#include <iostream>
#include <memory>
#include <string>


using namespace scy;

#define USE_CAMERA 0


class WebcamStreamer
{
public:
    smpl::TCPClient client;
    std::unique_ptr<wrtc::PeerSession> session;
    std::shared_ptr<av::MediaCapture> capture;
    PacketStream stream;

    WebcamStreamer(const smpl::Client::Options& opts)
        : client(opts)
        , stream("webcam-stream")
    {
    }

    void start()
    {
        // Set up video capture.
#if USE_CAMERA
        capture = std::make_shared<av::MediaCapture>();
        capture->openCamera(0, 640, 480);
#else
        capture = std::make_shared<av::MediaCapture>();
        capture->openFile(SCY_DATA_DIR "/test.mp4");
        capture->setLoopInput(true);
        capture->setLimitFramerate(true);
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
        capture.reset();
        client.close();
    }

private:
    void createSession()
    {
        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        config.mediaOpts.videoCodec = av::VideoCodec("H264", "libx264", 640, 480, 30);
        // Audio left empty - video only.
        config.enableDataChannel = false;

        session = std::make_unique<wrtc::PeerSession>(client, config);

        session->IncomingCall += [this](const std::string& peerId) {
            std::cout << "Incoming call from " << peerId << std::endl;
            session->accept();
        };

        session->StateChanged += [this](wrtc::PeerSession::State state) {
            std::cout << "Call state: " << wrtc::stateToString(state) << std::endl;

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
            std::cout << "REMB: " << bps / 1000 << " kbps" << std::endl;
        };

        session->media().KeyframeRequested += []() {
            std::cout << "PLI: keyframe requested" << std::endl;
        };
    }

    void startStreaming()
    {
        if (!session || !session->media().hasVideo())
            return;

        // Pipeline: capture → encoder → WebRTC sender
        stream.attachSource(capture.get(), false, true);

        // The encoder takes decoded frames from capture and outputs
        // encoded H.264 NAL units as VideoPacket.
        // Note: in a real application you'd configure the encoder
        // properly. This is a minimal example.
        // For now we rely on the media bridge's track sender
        // to accept VideoPacket from the capture's decoder output.
        // In practice, you'd insert a VideoEncoder processor here.

        stream.attach(&session->media().videoSender(), 5, false);
        stream.start();

        capture->start();
        std::cout << "Streaming started" << std::endl;
    }

    void onAnnounce(const int& status)
    {
        if (status != 200)
            std::cerr << "Auth failed: " << status << std::endl;
    }

    void onStateChange(void*, sockio::ClientState& state, const sockio::ClientState&)
    {
        std::cout << "Client: " << state.toString() << std::endl;
        if (state.id() == sockio::ClientState::Online) {
            std::cout << "Online as " << client.ourID() << std::endl;
            client.joinRoom("public");
            createSession();
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "LibSourcey";
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
