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
// Uses SympleSignaller for signalling (compatible with
// symple-client-player's CallManager in the browser).
//
// Usage:
//   data-echo -host <symple-server> -port <port> -user <id> -name <name>
//
/// @addtogroup webrtc
/// @{


#include "scy/application.h"
#include "scy/logger.h"
#include "scy/symple/client.h"
#include "scy/webrtc/peersession.h"
#include "symplesignaller.h"

#include <iostream>
#include <memory>
#include <string>


using namespace scy;


class DataEchoApp
{
public:
    smpl::TCPClient client;
    std::unique_ptr<wrtc::SympleSignaller> signaller;
    std::unique_ptr<wrtc::PeerSession> session;

    DataEchoApp(const smpl::Client::Options& opts)
        : client(opts)
    {
    }

    void start()
    {
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
        signaller.reset();
        client.close();
    }

private:
    void createSession()
    {
        signaller = std::make_unique<wrtc::SympleSignaller>(client);

        wrtc::PeerSession::Config config;
        config.rtcConfig.iceServers.emplace_back("stun:stun.l.google.com:19302");
        config.enableDataChannel = true;
        config.dataChannelLabel = "echo";

        session = std::make_unique<wrtc::PeerSession>(*signaller, config);

        session->IncomingCall += [this](const std::string& peerId) {
            std::cout << "Incoming call from " << peerId << " - accepting" << '\n';
            session->accept();
        };

        session->DataReceived += [this](rtc::message_variant msg) {
            if (auto* text = std::get_if<std::string>(&msg)) {
                std::cout << "Received: " << *text << '\n';
                session->sendData(*text);
            }
            else if (auto* bin = std::get_if<rtc::binary>(&msg)) {
                std::cout << "Received " << bin->size() << " bytes" << '\n';
                session->sendData(
                    reinterpret_cast<const std::byte*>(bin->data()), bin->size());
            }
        };

        session->StateChanged += [](wrtc::PeerSession::State state) {
            std::cout << "Call state: " << wrtc::stateToString(state) << '\n';
        };
    }

    void onAnnounce(const int& status)
    {
        std::cout << "Announce: " << status << '\n';
        if (status != 200)
            std::cerr << "Authentication failed" << '\n';
    }

    void onStateChange(void*, sockio::ClientState& state, const sockio::ClientState&)
    {
        std::cout << "Client: " << state.toString() << '\n';
        if (state.id() == sockio::ClientState::Online) {
            std::cout << "Online as " << client.ourID() << '\n';
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
