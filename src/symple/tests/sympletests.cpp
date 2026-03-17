#include "sympletests.h"
#include "scy/symple/command.h"
#include "scy/symple/event.h"
#include "scy/symple/peer.h"
#include "scy/symple/presence.h"
#include "scy/symple/roster.h"

#include <stdexcept>


namespace scy_test = scy::test;


int main(int argc, char** argv)
{
    // Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    scy_test::init();

#if USE_SSL
    scy::net::SSLManager::initNoVerifyClient();
#endif


    // =========================================================================
    // Address
    //
    scy_test::describe("address", []() {
        scy::smpl::Address a1("user|");
        if (a1.user != "user")
            throw std::runtime_error("Expected user == 'user'");
        if (!a1.id.empty())
            throw std::runtime_error("Expected id to be empty");
        if (!a1.valid())
            throw std::runtime_error("Expected a1 to be valid");

        scy::smpl::Address a2("user");
        if (a2.user != "user")
            throw std::runtime_error("Expected user == 'user'");
        if (!a2.id.empty())
            throw std::runtime_error("Expected id to be empty");
        if (!a2.valid())
            throw std::runtime_error("Expected a2 to be valid");

        scy::smpl::Address a3("");
        if (!a3.user.empty())
            throw std::runtime_error("Expected user to be empty");
        if (!a3.id.empty())
            throw std::runtime_error("Expected id to be empty");
        if (a3.valid())
            throw std::runtime_error("Expected a3 to be invalid");

        scy::smpl::Address a4("|567257247245275");
        if (!a4.user.empty())
            throw std::runtime_error("Expected user to be empty");
        if (a4.id != "567257247245275")
            throw std::runtime_error("Expected id == '567257247245275'");
        if (!a4.valid())
            throw std::runtime_error("Expected a4 to be valid");
    });


    // =========================================================================
    // Client
    //
    scy_test::describe("client", []() {
        // Run the test server
        // If not available the test will fail gracefully with a warning
        // NOTE: The server must allow anonymous authentication for this test
        scy::Process proc;
        if (!scy::openTestServer(proc)) {
            std::cerr << "cannot start test server, skipping..." << std::endl;
            return;
        }

        scy::smpl::Client::Options loptions;
        loptions.host = SERVER_HOST;
        loptions.port = SERVER_PORT;
        loptions.user = "l";
        loptions.name = "Left";
        // loptions.token = "2NuMmyXw2YDuQfyPCKDO2Qtta";

        scy::smpl::Client::Options roptions;
        roptions.host = SERVER_HOST;
        roptions.port = SERVER_PORT;
        roptions.user = "r";
        roptions.name = "Right";
        // roptions.token = "2NuMmyXw2YDuQfyPCKDO2Qtta";

        scy::TestClient lclient(loptions);
        scy::TestClient rclient(roptions);

        lclient.connect();
        rclient.connect();

        while (!lclient.completed() || !rclient.completed()) {

            // LDebug("waiting for test completion");
            scy::uv::runLoop(scy::uv::defaultLoop(), UV_RUN_ONCE);

            // // Connect the rclient when lclient is online
            // if (lclient.client.isOnline() &&
            //     rclient.client.stateEquals(sockio::ClientState::Closed))
            //     rclient.connect();
        }

        lclient.check();
        rclient.check();
    });

    // =========================================================================
    // Peer
    //
    scy_test::describe("peer", []() {
        scy::smpl::Peer peer;
        peer.setID("abc123");
        peer.setUser("testuser");
        peer.setName("Test User");
        peer.setType("client");
        peer.setHost("localhost");

        expect(peer.id() == "abc123");
        expect(peer.user() == "testuser");
        expect(peer.name() == "Test User");
        expect(peer.type() == "client");
        expect(peer.host() == "localhost");
        expect(peer.valid());

        // Address combines user|id
        scy::smpl::Address addr = peer.address();
        expect(addr.user == "testuser");
        expect(addr.id == "abc123");
    });

    scy_test::describe("peer from json", []() {
        scy::json::value j;
        j["id"] = "xyz";
        j["user"] = "bob";
        j["name"] = "Bob";
        j["type"] = "server";

        scy::smpl::Peer peer(j);
        expect(peer.id() == "xyz");
        expect(peer.user() == "bob");
        expect(peer.name() == "Bob");
    });

    scy_test::describe("peer copy", []() {
        scy::smpl::Peer p1;
        p1.setID("id1");
        p1.setUser("user1");

        scy::smpl::Peer p2(p1);
        expect(p2.id() == "id1");
        expect(p2.user() == "user1");
    });


    // =========================================================================
    // Event
    //
    scy_test::describe("event", []() {
        scy::smpl::Event event;
        event.setName("click");
        event.setTime(1234567890.0);

        expect(event.name() == "click");
        expect(event.time() == 1234567890.0);
        expect(event.valid());
    });

    scy_test::describe("event from json", []() {
        scy::json::value j;
        j["type"] = "event";
        j["name"] = "hover";
        j["time"] = 42.0;

        scy::smpl::Event event(j);
        expect(event.name() == "hover");
        expect(event.time() == 42.0);
    });


    // =========================================================================
    // Command
    //
    scy_test::describe("command", []() {
        scy::smpl::Command cmd;
        cmd.setNode("audio.mute");
        cmd.setAction("toggle");

        expect(cmd.node() == "audio.mute");
        expect(cmd.action() == "toggle");
        expect(cmd.valid());
    });

    scy_test::describe("command from json", []() {
        scy::json::value j;
        j["type"] = "command";
        j["node"] = "video.play";
        j["action"] = "start";

        scy::smpl::Command cmd(j);
        expect(cmd.node() == "video.play");
        expect(cmd.action() == "start");
    });


    // =========================================================================
    // Presence
    //
    scy_test::describe("presence", []() {
        scy::smpl::Presence pres;
        expect(!pres.isProbe());

        pres.setProbe(true);
        expect(pres.isProbe());

        pres.setProbe(false);
        expect(!pres.isProbe());
    });


    // =========================================================================
    // Roster
    //
    scy_test::describe("roster", []() {
        scy::smpl::Roster roster;

        auto peer1 = new scy::smpl::Peer;
        peer1->setID("p1");
        peer1->setUser("alice");
        peer1->setHost("host1");

        auto peer2 = new scy::smpl::Peer;
        peer2->setID("p2");
        peer2->setUser("bob");
        peer2->setHost("host2");

        roster.add("alice|p1", peer1);
        roster.add("bob|p2", peer2);

        // getByHost
        auto found = roster.getByHost("host1");
        expect(found != nullptr);
        expect(found->user() == "alice");

        expect(roster.getByHost("unknown") == nullptr);
    });


    scy_test::runAll();

#if USE_SSL
    scy::net::SSLManager::instance().shutdown();
#endif
    scy::Logger::destroy();
    return scy_test::finalize();
}
