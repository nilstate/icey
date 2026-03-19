#include "sympletests.h"
#include "scy/application.h"
#include "scy/loop.h"
#include "scy/symple/command.h"
#include "scy/symple/event.h"
#include "scy/symple/peer.h"
#include "scy/symple/presence.h"
#include "scy/symple/roster.h"

#include <stdexcept>


using namespace scy;
namespace scy_test = scy::test;


int main(int argc, char** argv)
{
    scy::Logger::instance().add(
        std::make_unique<scy::ConsoleChannel>("debug", scy::Level::Debug));
    scy_test::init();


    // =========================================================================
    // Address
    //
    scy_test::describe("address: user|id parse", []() {
        scy::smpl::Address a("alice|abc123");
        expect(a.user == "alice");
        expect(a.id == "abc123");
        expect(a.valid());
        expect(a.toString() == "alice|abc123");
    });

    scy_test::describe("address: user only", []() {
        scy::smpl::Address a("alice");
        expect(a.user == "alice");
        expect(a.id.empty());
        expect(a.valid());
    });

    scy_test::describe("address: empty invalid", []() {
        scy::smpl::Address a("");
        expect(!a.valid());
    });


    // =========================================================================
    // Peer
    //
    scy_test::describe("peer: construction from json", []() {
        scy::json::Value data;
        data["id"] = "abc123";
        data["user"] = "alice";
        data["name"] = "Alice";
        data["online"] = true;

        scy::smpl::Peer peer(data);
        expect(peer.id() == "abc123");
        expect(peer.user() == "alice");
        expect(peer.name() == "Alice");
        expect(peer.valid());
    });

    scy_test::describe("peer: address format", []() {
        scy::json::Value data;
        data["id"] = "x1";
        data["user"] = "bob";
        data["online"] = true;

        scy::smpl::Peer peer(data);
        auto addr = peer.address();
        expect(addr.user == "bob");
        expect(addr.id == "x1");
        expect(addr.toString() == "bob|x1");
    });


    // =========================================================================
    // Message
    //
    scy_test::describe("message: construction and fields", []() {
        scy::smpl::Message m;
        m.setType("message");
        m.setTo(scy::smpl::Address("bob|123"));
        m.setFrom(scy::smpl::Address("alice|456"));
        m["subtype"] = "chat";
        m["data"]["text"] = "hello";

        expect(m.type() == "message");
        expect(m.to().user == "bob");
        expect(m.from().user == "alice");
        expect(m.valid());
    });


    // =========================================================================
    // Presence
    //
    scy_test::describe("presence: probe flag", []() {
        scy::smpl::Presence p;
        expect(!p.isProbe());
        p.setProbe(true);
        expect(p.isProbe());
    });


    // =========================================================================
    // Roster
    //
    scy_test::describe("roster: add and get", []() {
        scy::smpl::Roster roster;
        auto peer = std::make_unique<scy::smpl::Peer>();
        (*peer)["id"] = "abc";
        (*peer)["user"] = "alice";
        (*peer)["online"] = true;
        roster.add("abc", std::move(peer));

        auto* found = roster.get("abc", false);
        expect(found != nullptr);
        expect(found->user() == "alice");

        auto* notFound = roster.get("nonexistent", false);
        expect(notFound == nullptr);

        roster.free("abc");
        expect(roster.get("abc", false) == nullptr);
    });


    // =========================================================================
    // Server
    //
    scy_test::describe("server: start and shutdown", []() {
        scy::smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT});
        expect(server.peerCount() == 0);
        server.shutdown();
    });


    // =========================================================================
    // Client + Server Integration
    //
    // NOTE: These tests require the libuv event loop to run.
    // The test framework runs tests synchronously, so we use
    // runLoop() to pump events for a limited time.
    //
    scy_test::describe("client: connect and authenticate", []() {
        smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT, .dynamicRooms = true});

        bool peerConnected = false;
        server.PeerConnected += [&](smpl::ServerPeer&) { peerConnected = true; };

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT;
        opts.user = "testuser";
        opts.name = "Test User";
        opts.reconnection = false;

        smpl::Client client(opts);
        bool gotOnline = false;
        int announceStatus = 0;

        client.Announce += [&](const int& s) { announceStatus = s; };
        client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Online)
                gotOnline = true;
        };

        client.connect();
        expect(test::waitFor([&] { return gotOnline; }));
        expect(announceStatus == 200);
        expect(!client.ourID().empty());
        expect(peerConnected);
        expect(server.peerCount() == 1);

        client.close();
        server.shutdown();
    });


    scy_test::describe("client: two peers presence", []() {
        smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 1, .dynamicRooms = true});

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 1;
        optsA.user = "alice";
        optsA.reconnection = false;
        TestClient alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 1;
        optsB.user = "bob";
        optsB.reconnection = false;
        TestClient bob(optsB);

        alice.connect();
        expect(test::waitFor([&] { return alice.gotOnline; }));

        bob.connect();
        expect(test::waitFor([&] { return bob.gotOnline; }));
        expect(test::waitFor([&] { return alice.gotRemotePresence && bob.gotRemotePresence; }));
        expect(server.peerCount() == 2);

        alice.client.close();
        bob.client.close();
        server.shutdown();
    });


    scy_test::describe("client: message routing", []() {
        smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 2, .dynamicRooms = true});

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 2;
        optsA.user = "sender";
        optsA.reconnection = false;
        TestClient sender(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 2;
        optsB.user = "receiver";
        optsB.reconnection = false;
        TestClient receiver(optsB);

        sender.connect();
        receiver.connect();
        expect(test::waitFor([&] { return sender.gotOnline && receiver.gotOnline; }));

        smpl::Message msg;
        msg.setTo(smpl::Address("receiver"));
        msg["data"]["text"] = "hello";
        sender.client.send(msg);

        expect(test::waitFor([&] { return !receiver.receivedMessage.empty(); }));
        expect(receiver.receivedMessage == "hello");

        sender.client.close();
        receiver.client.close();
        server.shutdown();
    });


    scy_test::describe("client: auth failure", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 3;
        sopts.authentication = true;
        server.start(sopts);

        server.Authenticate += [](smpl::ServerPeer&, const json::Value&, bool& allowed) {
            allowed = false;
        };

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 3;
        opts.user = "baduser";
        opts.reconnection = false;

        smpl::Client client(opts);
        bool gotError = false;
        int announceStatus = 0;

        client.Announce += [&](const int& s) { announceStatus = s; };
        client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Error)
                gotError = true;
        };

        client.connect();
        expect(test::waitFor([&] { return gotError; }));
        expect(announceStatus == 401);
        expect(client.ourID().empty());

        client.close();
        server.shutdown();
    });


    scy_test::describe("client: disconnect presence", []() {
        smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 4, .dynamicRooms = true});

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 4;
        optsA.user = "alice";
        optsA.reconnection = false;
        TestClient alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 4;
        optsB.user = "bob";
        optsB.reconnection = false;
        TestClient bob(optsB);

        alice.connect();
        expect(test::waitFor([&] { return alice.gotOnline; }));

        bob.connect();
        expect(test::waitFor([&] { return bob.gotOnline; }));
        expect(test::waitFor([&] { return alice.gotRemotePresence; }));

        // Bob disconnects; alice should see offline presence
        bool aliceGotOffline = false;
        alice.client.PeerDisconnected += [&](smpl::Peer& peer) {
            if (peer.user() == "bob")
                aliceGotOffline = true;
        };

        bob.client.close();
        expect(test::waitFor([&] { return aliceGotOffline; }));
        expect(server.peerCount() == 1);

        alice.client.close();
        server.shutdown();
    });


    scy_test::runAll();
    return scy_test::finalize();
}
