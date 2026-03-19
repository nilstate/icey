#include "sympletests.h"
#include "icy/application.h"
#include "icy/loop.h"
#include "icy/symple/command.h"
#include "icy/symple/event.h"
#include "icy/symple/peer.h"
#include "icy/symple/presence.h"
#include "icy/symple/roster.h"

#include <stdexcept>


using namespace icy;
namespace icy_test = icy::test;


int main(int argc, char** argv)
{
    icy::Logger::instance().add(
        std::make_unique<icy::ConsoleChannel>("debug", icy::Level::Debug));
    icy_test::init();


    // =========================================================================
    // Address
    //
    icy_test::describe("address: user|id parse", []() {
        icy::smpl::Address a("alice|abc123");
        expect(a.user == "alice");
        expect(a.id == "abc123");
        expect(a.valid());
        expect(a.toString() == "alice|abc123");
    });

    icy_test::describe("address: user only", []() {
        icy::smpl::Address a("alice");
        expect(a.user == "alice");
        expect(a.id.empty());
        expect(a.valid());
    });

    icy_test::describe("address: empty invalid", []() {
        icy::smpl::Address a("");
        expect(!a.valid());
    });


    // =========================================================================
    // Peer
    //
    icy_test::describe("peer: construction from json", []() {
        icy::json::Value data;
        data["id"] = "abc123";
        data["user"] = "alice";
        data["name"] = "Alice";
        data["online"] = true;

        icy::smpl::Peer peer(data);
        expect(peer.id() == "abc123");
        expect(peer.user() == "alice");
        expect(peer.name() == "Alice");
        expect(peer.valid());
    });

    icy_test::describe("peer: address format", []() {
        icy::json::Value data;
        data["id"] = "x1";
        data["user"] = "bob";
        data["online"] = true;

        icy::smpl::Peer peer(data);
        auto addr = peer.address();
        expect(addr.user == "bob");
        expect(addr.id == "x1");
        expect(addr.toString() == "bob|x1");
    });


    // =========================================================================
    // Message
    //
    icy_test::describe("message: construction and fields", []() {
        icy::smpl::Message m;
        m.setType("message");
        m.setTo(icy::smpl::Address("bob|123"));
        m.setFrom(icy::smpl::Address("alice|456"));
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
    icy_test::describe("presence: probe flag", []() {
        icy::smpl::Presence p;
        expect(!p.isProbe());
        p.setProbe(true);
        expect(p.isProbe());
    });


    // =========================================================================
    // Roster
    //
    icy_test::describe("roster: add and get", []() {
        icy::smpl::Roster roster;
        auto peer = std::make_unique<icy::smpl::Peer>();
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
    icy_test::describe("server: start and shutdown", []() {
        icy::smpl::Server server;
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
    icy_test::describe("client: connect and authenticate", []() {
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


    icy_test::describe("client: two peers presence", []() {
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


    icy_test::describe("client: message routing", []() {
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


    icy_test::describe("client: auth failure", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 3;
        sopts.authentication = true;
        server.start(sopts);

        server.Authenticate += [](smpl::ServerPeer&, const json::Value&, bool& allowed, std::vector<std::string>&) {
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


    icy_test::describe("client: disconnect presence", []() {
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


    // =========================================================================
    // Server Hardening
    //
    icy_test::describe("hardening: max connections", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 5;
        sopts.maxConnections = 1;
        server.start(sopts);

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 5;
        optsA.user = "alice";
        optsA.reconnection = false;
        TestClient alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 5;
        optsB.user = "bob";
        optsB.reconnection = false;
        TestClient bob(optsB);

        alice.connect();
        expect(test::waitFor([&] { return alice.gotOnline; }));
        expect(server.peerCount() == 1);

        // Bob should be rejected (max 1 connection)
        bob.connect();
        bool bobClosed = false;
        bob.client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Closed ||
                state.id() == smpl::ClientState::Error)
                bobClosed = true;
        };
        expect(test::waitFor([&] { return bobClosed; }));
        expect(server.peerCount() == 1);

        alice.client.close();
        server.shutdown();
    });


    icy_test::describe("hardening: max message size", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 6;
        sopts.maxMessageSize = 128; // very small limit
        server.start(sopts);

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 6;
        opts.user = "alice";
        opts.reconnection = false;

        smpl::Client client(opts);
        bool gotOnline = false;
        bool disconnected = false;

        client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Online)
                gotOnline = true;
            if (state.id() == smpl::ClientState::Closed ||
                state.id() == smpl::ClientState::Error)
                disconnected = true;
        };

        client.connect();
        expect(test::waitFor([&] { return gotOnline; }));

        // Send an oversized message - server should close the connection
        std::string bigPayload(256, 'x');
        smpl::Message msg;
        msg["data"]["text"] = bigPayload;
        msg.setTo(smpl::Address("nobody"));
        try { client.send(msg); } catch (...) {}

        expect(test::waitFor([&] { return disconnected; }));

        client.close();
        server.shutdown();
    });


    icy_test::describe("hardening: rate limiting", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 7;
        sopts.rateLimit = 3.0;   // 3 messages
        sopts.rateSeconds = 10.0; // per 10 seconds
        server.start(sopts);

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 7;
        opts.user = "spammer";
        opts.reconnection = false;
        TestClient spammer(opts);

        spammer.connect();
        expect(test::waitFor([&] { return spammer.gotOnline; }));

        // Flood messages - sender should be disconnected after exceeding rate
        bool disconnected = false;
        spammer.client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Closed ||
                state.id() == smpl::ClientState::Error)
                disconnected = true;
        };

        for (int i = 0; i < 20; i++) {
            smpl::Message msg;
            msg.setTo(smpl::Address("nobody"));
            msg["data"]["seq"] = i;
            try { spammer.client.send(msg); } catch (...) { break; }
        }

        expect(test::waitFor([&] { return disconnected; }));
        server.shutdown();
    });


    icy_test::describe("hardening: graceful shutdown broadcast", []() {
        smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 8});

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 8;
        opts.user = "alice";
        opts.reconnection = false;

        smpl::Client client(opts);
        bool gotOnline = false;
        bool closed = false;

        client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Online)
                gotOnline = true;
            if (state.id() == smpl::ClientState::Closed)
                closed = true;
        };

        client.connect();
        expect(test::waitFor([&] { return gotOnline; }));

        // Shutdown broadcasts event then closes
        server.shutdown();

        // The client should be disconnected (server closed the connection)
        expect(test::waitFor([&] { return closed; }, 2000));

        client.close();
    });


    icy_test::runAll();
    return icy_test::finalize();
}
