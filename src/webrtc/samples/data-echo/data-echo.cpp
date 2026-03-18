///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Data Channel Echo Server
//
// Connects to a Symple server and waits for incoming calls.
// When a browser peer connects, opens a data channel and echoes
// back any messages received.
//
// This is the simplest WebRTC sample - no media, no FFmpeg.
// Use it to verify that PeerSession, Symple signalling, and
// libdatachannel's ICE/DTLS/SCTP stack work end-to-end.
//
// Usage:
//   data-echo -host <symple-server> -port <port> -user <id> -name <name>
//
// Requires a running Symple server (symple-server from the symple repo).
// Connect from a browser using symple-client + RTCPeerConnection.
//
/// @addtogroup webrtc
/// @{


#include "scy/application.h"
#include "scy/logger.h"
#include "scy/symple/client.h"
#include "scy/webrtc/peersession.h"

#include <iostream>
#include <memory>
#include <string>


using namespace scy;


class DataEchoApp
{
public:
    smpl::TCPClient client;
    std::unique_ptr<wrtc::PeerSession> session;

    DataEchoApp(const smpl::Client::Options& opts)
        : client(opts)
    {
    }

    void start()
    {
        // Wire Symple lifecycle signals.
        client.Announce += slot(this, &DataEchoApp::onAnnounce);
        client.StateChange += slot(this, &DataEchoApp::onStateChange);
        client.CreatePresence += slot(this, &DataEchoApp::onCreatePresence);

        client.connect();
    }

    void shutdown()
    {
        if (session)
            session->hangup("shutdown");
        session.reset();
        client.close();
    }

private:
    void createSession()
    {
        // Data-channel only: leave codec encoders empty.
        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        config.enableDataChannel = true;
        config.dataChannelLabel = "echo";
        // No media tracks - codecs left default (empty encoder).

        session = std::make_unique<wrtc::PeerSession>(client, config);

        session->IncomingCall += [this](const std::string& peerId) {
            std::cout << "Incoming call from " << peerId << " - accepting" << std::endl;
            session->accept();
        };

        session->DataReceived += [this](rtc::message_variant msg) {
            if (auto* text = std::get_if<std::string>(&msg)) {
                std::cout << "Received: " << *text << std::endl;
                session->sendData(*text);  // echo back
            }
            else if (auto* bin = std::get_if<rtc::binary>(&msg)) {
                std::cout << "Received " << bin->size() << " bytes" << std::endl;
                session->sendData(
                    reinterpret_cast<const std::byte*>(bin->data()), bin->size());
            }
        };

        session->StateChanged += [](wrtc::PeerSession::State state) {
            std::cout << "Call state: " << wrtc::stateToString(state) << std::endl;
        };
    }

    void onAnnounce(const int& status)
    {
        std::cout << "Announce: " << status << std::endl;
        if (status != 200)
            std::cerr << "Authentication failed" << std::endl;
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
        peer["type"] = "echo";
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
        opts.user = "echo-server";
        opts.name = "Data Echo";
    }

    DataEchoApp app(opts);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<DataEchoApp*>(opaque)->shutdown();
    }, &app);

    Logger::destroy();
    return 0;
}
