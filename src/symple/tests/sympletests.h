#pragma once


#include "scy/base.h"
#include "scy/logger.h"
#include "scy/symple/client.h"
#include "scy/symple/server.h"
#include "scy/test.h"

#include <iostream>
#include <stdexcept>


#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 14500


namespace scy {


/// Test client that connects to the C++ Symple server.
class TestClient
{
public:
    bool gotOnline = false;
    bool gotRemotePresence = false;
    std::string receivedMessage;
    std::string user;
    smpl::Client client;

    TestClient(const smpl::Client::Options& options)
        : user(options.user)
        , client(options)
    {
        client += packetSlot(this, &TestClient::onRecvMessage);
        client += packetSlot(this, &TestClient::onRecvPresence);
        client.Announce += slot(this, &TestClient::onClientAnnounce);
        client.StateChange += slot(this, &TestClient::onClientStateChange);
        client.CreatePresence += slot(this, &TestClient::onCreatePresence);
    }

    ~TestClient()
    {
        client.close();
    }

    void connect()
    {
        client.connect();
    }

    void onRecvPresence(smpl::Presence& presence)
    {
        LDebug(user, ": presence from ", presence.from().toString());
        if (presence.from().user != user)
            gotRemotePresence = true;
    }

    void onRecvMessage(smpl::Message& message)
    {
        LDebug(user, ": message received");
        if (message.find("data") != message.end() &&
            message["data"].contains("text"))
            receivedMessage = message["data"]["text"].get<std::string>();
    }

    void onClientAnnounce(const int& status)
    {
        LDebug(user, ": announce ", status);
    }

    void onClientStateChange(void*, smpl::ClientState& state, const smpl::ClientState&)
    {
        LDebug(user, ": state -> ", state.toString());
        if (state.id() == smpl::ClientState::Online) {
            gotOnline = true;
            client.joinRoom("test");
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "TestClient";
        peer["version"] = "4.0.0";
    }
};


} // namespace scy
