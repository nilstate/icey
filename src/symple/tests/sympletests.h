#pragma once


#include "scy/base.h"
#include "scy/filesystem.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"
#include "scy/pipe.h"
#include "scy/process.h"
#include "scy/symple/client.h"
#include "scy/test.h"

#include <iostream>
#include <stdexcept>


#define SERVER_HOST "localhost"
#define USE_SSL 0
#if USE_SSL
#define SERVER_PORT 443
#else
#define SERVER_PORT 4500
#endif


namespace scy {


// =============================================================================
// Test Client
//
class TestClient
{
public:
    bool gotOnline;
    bool gotRemotePresence;
    std::string user;

#if USE_SSL
    smpl::SSLClient client;
#else
    smpl::TCPClient client;
#endif

    TestClient(const smpl::Client::Options& options)
        : gotOnline(false)
        , gotRemotePresence(false)
    {
        user = options.user;

        client.options() = options;

        client += slot(this, &TestClient::onRecvPacket);
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
        LInfo(user, ": connect");
        client.connect();
    }

    bool completed()
    {
        SInfo << user << ": completed: "
              // << "user=" << client.options().user << ", "
              // << "name=" << client.options().name << ", "
              << "gotOnline=" << gotOnline << ", "
              << "gotRemotePresence=" << gotRemotePresence;
        return gotOnline && gotRemotePresence;
    }

    void check()
    {
        SInfo << user << ": check: "
              // << "user=" << client.options().user << ", "
              // << "name=" << client.options().name << ", "
              << "gotOnline=" << gotOnline << ", "
              << "gotRemotePresence=" << gotRemotePresence;
        expect(gotOnline);
        expect(gotRemotePresence);
    }

    void onRecvPacket(IPacket& raw)
    {
        LDebug("####### On raw packet: ", raw.className());

        // Handle incoming raw packets here
    }

    void onRecvPresence(smpl::Presence& presence)
    {
        LInfo(user, ": On presence: ", presence.dump(4));

        expect(presence.data("version").get<std::string>() == "1.0.1");
        if (user == "l") {
            expect(presence.from().user == "r");
        } else if (user == "r") {
            expect(presence.from().user == "l");
        } else {
            expect(!"user should be 'l' or 'r'");
        }

        gotRemotePresence = true;
    }

    void onRecvMessage(smpl::Message& message)
    {
        LInfo(user, ": On message: ", message.dump(4));

        // Handle incoming Symple messages here
    }

    void onClientAnnounce(const int& status)
    {
        if (status != 200)
            throw std::runtime_error("Announce failed with status: " + std::to_string(status));
    }

    void onClientStateChange(void*, sockio::ClientState& state, const sockio::ClientState& oldState)
    {
        SInfo << user << ": Client state changed: " << state << ": "
              << client.ws().socket->address();

        switch (state.id()) {
            case sockio::ClientState::Connecting:
                break;
            case sockio::ClientState::Connected:
                break;
            case sockio::ClientState::Online:
                gotOnline = true;

                // Join the test room when online
                client.joinRoom("test");

                // Send a message when online
                // smpl::Message m;
                // m.setData("olay");
                // client.send(m, true);
                break;
            case sockio::ClientState::Error:
                throw std::runtime_error("Client entered error state");
                break;
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        LInfo(user, ": Updating Client Data");

        // Update the peer object to be broadcast with presence.
        // Any arbitrary data can be broadcast with presence.
        peer["agent"] = "Spot";
        peer["version"] = "1.0.1";
    }
};


void setTestServerCwd(std::string& cwd)
{
    fs::addnode(cwd, SCY_SOURCE_DIR);
    fs::addnode(cwd, "symple");
    fs::addnode(cwd, "tests");
    fs::addnode(cwd, "testserver");
}

// Helper to raise a test Symple server
bool installTestServerSync()
{
    bool success = false;
    Process proc({"npm", "install"});
    setTestServerCwd(proc.cwd);
    proc.onstdout = [](std::string line) {
        std::cout << "server npm stdout: " << line << '\n';
    };
    proc.onexit = [&](int64_t status) {
        std::cout << "server npm exit: " << status << '\n';
        success = status == 0;
    };
    proc.spawn();
    uv::runLoop();
    return success;
}


// Helper to raise a test Symple server
bool openTestServer(Process& proc, bool install = true)
{
    // Try to npm install the nodejs server
    if (install) {
        try {
            installTestServerSync();
        } catch (std::exception& exc) {
            // Sometimes this fails on windows, so swallow
            // and try to run the server even if npm fails
        }
    }

    // Run the nodejs server
    bool running = false, exited = false;
    setTestServerCwd(proc.cwd);
    proc.args = {"node", "server.js"};
    proc.onstdout = [&](std::string line) {
        std::cout << "server stdout: " << line << '\n';
        if (line.find("listening") != std::string::npos)
            running = true;
    };
    proc.onexit = [&](int64_t status) {
        std::cout << "server exit: " << status << '\n';
        exited = true;
    };
    proc.spawn();

    // Run the loop until the server is listening or exited in failure
    while (!exited && !running) {
        uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
    }

    std::cout << "server running: " << running << '\n';
    return running;
}


} // namespace scy


/// @\}
