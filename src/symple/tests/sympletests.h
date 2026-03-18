#pragma once


#include "scy/application.h"
#include "scy/base.h"
#include "scy/logger.h"
#include "scy/symple/client.h"
#include "scy/symple/server.h"
#include "scy/test.h"

#include <iostream>
#include <stdexcept>


#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 14500  // Use non-standard port to avoid conflicts


namespace scy {


/// Test client that connects to the C++ Symple server.
class TestClient
{
public:
    bool gotOnline = false;
    bool gotRemotePresence = false;
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
        LInfo(user, ": connecting");
        client.connect();
    }

    bool completed()
    {
        return gotOnline && gotRemotePresence;
    }

    void check()
    {
        expect(gotOnline);
        expect(gotRemotePresence);
    }

    void onRecvPresence(smpl::Presence& presence)
    {
        LInfo(user, ": presence from ", presence.from().toString());

        // Only count presence from other users
        if (presence.from().user != user) {
            gotRemotePresence = true;
        }
    }

    void onRecvMessage(smpl::Message& message)
    {
        LInfo(user, ": message: ", message.dump(4));
    }

    void onClientAnnounce(const int& status)
    {
        LInfo(user, ": announce status ", status);
        if (status != 200)
            throw std::runtime_error("Announce failed: " + std::to_string(status));
    }

    void onClientStateChange(void*, smpl::ClientState& state, const smpl::ClientState&)
    {
        LInfo(user, ": state -> ", state.toString());

        switch (state.id()) {
        case smpl::ClientState::Online:
            gotOnline = true;
            client.joinRoom("test");
            break;
        case smpl::ClientState::Error:
            throw std::runtime_error(user + ": client error");
            break;
        default:
            break;
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "TestClient";
        peer["version"] = "4.0.0";
    }
};


} // namespace scy
