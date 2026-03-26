///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// File Streamer
//
// Reads a media file (MP4, MKV, etc.), encodes to H.264, and
// streams to a browser peer via WebRTC. Loops the file continuously.
//
// Pipeline:
//   MediaCapture (file) → VideoPacketEncoder → WebRtcTrackSender → [browser]
//
// Usage:
//   file-streamer -file <path> -host <symple-server> -port <port> -user <id>
//
/// @addtogroup webrtc
/// @{


#include "icy/application.h"
#include "icy/av/mediacapture.h"
#include "icy/av/videopacketencoder.h"
#include "icy/logger.h"
#include "icy/packetstream.h"
#include "icy/symple/client.h"
#include "icy/webrtc/codecnegotiator.h"
#include "icy/webrtc/peersession.h"
#include "icy/webrtc/support/symplesignaller.h"

#include <iostream>
#include <memory>
#include <string>


using namespace icy;


class FileStreamer
{
public:
    smpl::Client client;
    std::unique_ptr<wrtc::SympleSignaller> signaller;
    std::unique_ptr<wrtc::PeerSession> session;
    std::shared_ptr<av::MediaCapture> capture;
    std::shared_ptr<av::VideoPacketEncoder> encoder;
    PacketStream stream;
    std::string sourceFile;
    wrtc::WebRtcTrackSender* videoSender = nullptr;
    bool pipelineReady = false;

    FileStreamer(const smpl::Client::Options& opts, const std::string& file)
        : client(opts)
        , stream("file-stream")
        , sourceFile(file)
    {
    }

    void start()
    {
        capture = std::make_shared<av::MediaCapture>();
        capture->openFile(sourceFile);
        capture->setLoopInput(true);
        capture->setLimitFramerate(true);

        client.Announce += slot(this, &FileStreamer::onAnnounce);
        client.StateChange += slot(this, &FileStreamer::onStateChange);
        client.CreatePresence += slot(this, &FileStreamer::onCreatePresence);

        client.start();
    }

    void shutdown()
    {
        stopStreaming();
        stream.close();
        if (session)
            session->hangup("shutdown");
        session.reset();
        encoder.reset();
        capture.reset();
        client.stop();
    }

private:
    void createSession()
    {
        av::VideoCodec videoCodec = wrtc::CodecNegotiator::resolveWebRtcVideoCodec(
            av::VideoCodec("H264", "libx264", 640, 480, 30));

        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        config.media.videoCodec = videoCodec;
        config.enableDataChannel = true;
        config.dataChannelLabel = "control";

        signaller = std::make_unique<wrtc::SympleSignaller>(client);
        session = std::make_unique<wrtc::PeerSession>(*signaller, config);

        session->IncomingCall += [this](const std::string& peerId) {
            std::cout << "Incoming call from " << peerId << " - accepting" << '\n';
            session->accept();
        };

        session->StateChanged += [this](wrtc::PeerSession::State state) {
            std::cout << "Call state: " << wrtc::stateToString(state) << '\n';

            if (state == wrtc::PeerSession::State::Active) {
                startStreaming();
            }
            else if (state == wrtc::PeerSession::State::Ended) {
                stopStreaming();
            }
        };

        // Data channel for seek commands from browser.
        session->DataReceived += [this](rtc::message_variant msg) {
            if (auto* text = std::get_if<std::string>(&msg)) {
                std::cout << "Control: " << *text << '\n';
            }
        };
    }

    void startStreaming()
    {
        if (!session || !session->media().hasVideo())
            return;

        if (!pipelineReady) {
            encoder = std::make_shared<av::VideoPacketEncoder>();

            // Input params: the decoded format from the file
            capture->getEncoderVideoCodec(encoder->iparams);

            // Output params: H.264 for WebRTC
            encoder->oparams = wrtc::CodecNegotiator::resolveWebRtcVideoCodec(
                av::VideoCodec("H264", "libx264", 640, 480, 30));

            videoSender = &session->media().videoSender();

            // Pipeline: capture → encoder → WebRTC sender
            stream.attachSource(capture.get(), false, true);
            stream.attach(encoder, 1, true);
            stream.attach(videoSender, 5, false);
            pipelineReady = true;
        }

        stream.start();

        capture->start();
        std::cout << "Streaming " << sourceFile << '\n';
    }

    void stopStreaming()
    {
        if (capture)
            capture->stop();
        stream.stop();
    }

    void onAnnounce(const int& status)
    {
        if (status != 200)
            std::cerr << "Auth failed: " << status << '\n';
    }

    void onStateChange(void*, smpl::ClientState& state, const smpl::ClientState&)
    {
        if (state.id() == smpl::ClientState::Online) {
            std::cout << "Online as " << client.ourID() << '\n';
            client.joinRoom("public");
            createSession();
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "icey";
        peer["type"] = "file-streamer";
    }
};


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    smpl::Client::Options opts;
    opts.host = "127.0.0.1";
    opts.port = 4500;
    std::string file = ICY_DATA_DIR "/test.mp4";

    for (int i = 1; i + 1 < argc; i += 2) {
        std::string key = argv[i];
        std::string val = argv[i + 1];
        if (key == "-host") opts.host = val;
        else if (key == "-port") opts.port = static_cast<uint16_t>(std::stoi(val));
        else if (key == "-token") opts.token = val;
        else if (key == "-user") opts.user = val;
        else if (key == "-name") opts.name = val;
        else if (key == "-file") file = val;
    }

    if (opts.user.empty()) {
        opts.user = "file-streamer";
        opts.name = "File Streamer";
    }

    FileStreamer app(opts, file);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<FileStreamer*>(opaque)->shutdown();
    }, &app);

    Logger::destroy();
    return 0;
}

/// @}
