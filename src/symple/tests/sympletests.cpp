#include "sympletests.h"
#include "../src/client/roomstate.h"
#include "../src/client/rosterstate.h"
#include "../src/protocol.h"
#include "../src/server/presence.h"
#include "../src/server/routingdetail.h"
#include "icy/application.h"
#include "icy/loop.h"
#include "icy/symple/command.h"
#include "icy/symple/event.h"
#include "icy/symple/peer.h"
#include "icy/symple/presence.h"
#include "icy/symple/roster.h"

#include <algorithm>
#include <stdexcept>


using namespace icy;
namespace icy_test = icy::test;

namespace {

bool clientHasRoom(const smpl::Client& client, const std::string& room)
{
    auto rooms = client.rooms();
    return std::find(rooms.begin(), rooms.end(), room) != rooms.end();
}

}


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

    icy_test::describe("peer: copy preserves explicit fields", []() {
        icy::smpl::Peer peer;
        peer.setID("media-server");
        peer.setUser("media-server");
        peer.setName("Media Server");
        peer.setType("media-server");
        peer["online"] = true;

        icy::smpl::Peer copy(peer);
        expect(copy.id() == "media-server");
        expect(copy.user() == "media-server");
        expect(copy.name() == "Media Server");
        expect(copy.type() == "media-server");
        expect(copy.value("online", false));
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

    icy_test::describe("protocol: make presence sanitizes reserved fields", []() {
        icy::smpl::Peer peer;
        peer.setID("abc123");
        peer.setUser("alice");
        peer.setName("Alice");
        peer.setType("browser");
        peer["online"] = true;

        icy::json::Value extra = icy::json::Value::object();
        extra["id"] = "forged";
        extra["user"] = "mallory";
        extra["online"] = false;
        extra["agent"] = "smoke";

        auto msg = icy::smpl::proto::makePresence(peer, peer.id(), true, &extra);
        expect(msg.value("type", "") == "presence");
        expect(msg.value("from", "") == "alice|abc123");
        expect(msg["data"].value("id", "") == "abc123");
        expect(msg["data"].value("user", "") == "alice");
        expect(msg["data"].value("name", "") == "Alice");
        expect(msg["data"].value("type", "") == "browser");
        expect(msg["data"].value("online", false));
        expect(msg["data"].value("agent", "") == "smoke");
    });

    icy_test::describe("protocol: parse welcome validates peer and rooms", []() {
        icy::json::Value msg;
        msg["type"] = "welcome";
        msg["status"] = 200;
        msg["peer"] = icy::json::Value::object();
        msg["peer"]["id"] = "abc123";
        msg["peer"]["user"] = "alice";
        msg["peer"]["online"] = true;
        msg["rooms"] = icy::json::Value::array();
        msg["rooms"].push_back("alice");
        msg["rooms"].push_back("public");
        msg["rooms"].push_back("");

        auto parsed = icy::smpl::proto::parseWelcome(msg);
        expect(parsed.has_value());
        expect(parsed->peer.id() == "abc123");
        expect(parsed->peer.user() == "alice");
        expect(parsed->rooms.count("alice") == 1);
        expect(parsed->rooms.count("public") == 1);
        expect(parsed->rooms.count("") == 0);
    });

    icy_test::describe("protocol: parse welcome rejects missing peer id", []() {
        icy::json::Value msg;
        msg["type"] = "welcome";
        msg["status"] = 200;
        msg["peer"] = icy::json::Value::object();
        msg["peer"]["user"] = "alice";
        msg["peer"]["online"] = true;

        std::string error;
        auto parsed = icy::smpl::proto::parseWelcome(msg, &error);
        expect(!parsed.has_value());
        expect(error == "Invalid welcome: missing peer ID");
    });

    icy_test::describe("client room state: join and leave acks reconcile desired rooms", []() {
        std::unordered_set<std::string> desired{"public"};
        std::unordered_set<std::string> current;
        std::unordered_set<std::string> pendingJoins{"public"};
        std::unordered_set<std::string> pendingLeaves;

        expect(!icy::smpl::detail::applyJoinAck(
            desired, current, pendingJoins, pendingLeaves, "public"));
        expect(current.count("public") == 1);
        expect(pendingJoins.count("public") == 0);

        expect(icy::smpl::detail::noteDesiredLeave(desired, current, pendingLeaves, "public"));
        expect(pendingLeaves.count("public") == 1);

        expect(!icy::smpl::detail::applyLeaveAck(
            desired, current, pendingJoins, pendingLeaves, "public"));
        expect(current.count("public") == 0);
        expect(pendingLeaves.count("public") == 0);
    });

    icy_test::describe("client room state: welcome resets transient room state", []() {
        std::unordered_set<std::string> current{"stale"};
        std::unordered_set<std::string> pendingJoins{"join"};
        std::unordered_set<std::string> pendingLeaves{"leave"};
        std::unordered_set<std::string> welcomeRooms{"alice", "public"};

        icy::smpl::detail::applyWelcome(current, pendingJoins, pendingLeaves, welcomeRooms);

        expect(current.count("alice") == 1);
        expect(current.count("public") == 1);
        expect(current.count("stale") == 0);
        expect(pendingJoins.empty());
        expect(pendingLeaves.empty());
    });

    icy_test::describe("client roster state: presence updates connect and disconnect peers", []() {
        icy::smpl::Roster roster;

        icy::json::Value online;
        online["id"] = "abc123";
        online["user"] = "alice";
        online["name"] = "Alice";
        online["online"] = true;

        auto connected = icy::smpl::detail::applyPresenceData(roster, online);
        expect(connected.kind == icy::smpl::detail::PresenceUpdate::Kind::Connected);
        expect(connected.livePeer != nullptr);
        expect(roster.get("abc123") != nullptr);

        icy::json::Value offline = online;
        offline["online"] = false;

        auto disconnected = icy::smpl::detail::applyPresenceData(roster, offline);
        expect(disconnected.kind == icy::smpl::detail::PresenceUpdate::Kind::Disconnected);
        expect(disconnected.snapshot.id() == "abc123");
        expect(roster.get("abc123") == nullptr);
    });

    icy_test::describe("routing detail: shared room detection and recipient dedupe", []() {
        std::unordered_set<std::string> aliceRooms{"public", "video"};
        std::unordered_set<std::string> bobRooms{"video", "control"};
        std::unordered_set<std::string> carolRooms{"other"};

        expect(icy::smpl::detail::sharesAnyRoom(aliceRooms, bobRooms));
        expect(!icy::smpl::detail::sharesAnyRoom(aliceRooms, carolRooms));

        icy::smpl::detail::RoomMemberMap indexedRooms;
        indexedRooms["public"] = {"alice", "bob"};
        indexedRooms["video"] = {"alice", "bob", "carol"};

        auto recipients = icy::smpl::detail::collectRecipients(
            indexedRooms,
            std::unordered_set<std::string>{"public", "video"},
            "alice");

        expect(recipients.size() == 2);
        expect(recipients.count("bob") == 1);
        expect(recipients.count("carol") == 1);
    });

    icy_test::describe("presence detail: canonical peer presence keeps server owned fields", []() {
        icy::smpl::Peer peer;
        peer.setID("abc123");
        peer.setUser("alice");
        peer.setName("Alice");
        peer["online"] = true;

        icy::json::Value extra = icy::json::Value::object();
        extra["user"] = "mallory";
        extra["online"] = false;
        extra["agent"] = "browser";

        auto msg = icy::smpl::detail::makePeerPresence(peer, peer.id(), true, &extra);
        expect(msg["data"].value("id", "") == "abc123");
        expect(msg["data"].value("user", "") == "alice");
        expect(msg["data"].value("online", false));
        expect(msg["data"].value("agent", "") == "browser");
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

        auto* found = roster.get("abc");
        expect(found != nullptr);
        expect(found->user() == "alice");

        auto* notFound = roster.get("nonexistent");
        expect(notFound == nullptr);

        roster.erase("abc");
        expect(roster.get("abc") == nullptr);
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

    icy_test::describe("server: virtual peer registration", []() {
        icy::smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 20});

        icy::smpl::Peer peer;
        peer.setID("media-server");
        peer.setUser("media-server");
        peer.setName("Media Server");
        peer.setType("media-server");
        peer["online"] = true;

        bool delivered = false;
        server.addVirtualPeer(peer, {"public"}, [&](const icy::json::Value&) {
            delivered = true;
        });

        expect(server.peerCount() == 1);
        server.removeVirtualPeer("media-server");
        expect(server.peerCount() == 0);
        expect(!delivered);

        server.shutdown();
    });

    icy_test::describe("client: sees virtual peer on connect", []() {
        smpl::Server server;
        server.Authenticate += [&](smpl::ServerPeer&, const json::Value&, bool& allowed, std::vector<std::string>& rooms) {
            allowed = true;
            rooms.push_back("public");
        };
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 21, .dynamicRooms = true});

        smpl::Peer peer;
        peer.setID("media-server");
        peer.setUser("media-server");
        peer.setName("Media Server");
        peer.setType("media-server");
        peer["online"] = true;
        server.addVirtualPeer(peer, {"public"}, [&](const icy::json::Value&) {});

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 21;
        opts.user = "alice";
        opts.reconnection = false;
        TestClient client(opts);

        client.connect();
        expect(test::waitFor([&] { return client.gotOnline; }));
        expect(test::waitFor([&] {
            return client.client.roster().get("media-server") != nullptr;
        }));
        auto* remote = client.client.roster().get("media-server");
        expect(remote != nullptr);
        expect(remote->user() == "media-server");
        expect(remote->type() == "media-server");

        client.client.close();
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
        expect(test::waitFor([&] { return clientHasRoom(alice.client, "test") && clientHasRoom(bob.client, "test"); }));

        alice.client.sendPresence(false);
        bob.client.sendPresence(false);
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
        expect(test::waitFor([&] { return clientHasRoom(sender.client, "test") && clientHasRoom(receiver.client, "test"); }));

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

    icy_test::describe("client: transport connect failure surfaces as error", []() {
        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 99;
        opts.user = "alice";
        opts.reconnection = false;

        smpl::Client client(opts);
        bool gotError = false;
        bool gotClosed = false;

        client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Error)
                gotError = true;
            if (state.id() == smpl::ClientState::Closed)
                gotClosed = true;
        };

        client.connect();
        expect(test::waitFor([&] { return gotError; }, 3000));
        expect(!gotClosed);

        client.close();
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
        expect(test::waitFor([&] { return clientHasRoom(alice.client, "test") && clientHasRoom(bob.client, "test"); }));

        alice.client.sendPresence(false);
        bob.client.sendPresence(false);
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


    // =========================================================================
    // Presence data sanitization: server overrides identity fields
    //
    icy_test::describe("hardening: presence data sanitized", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 10;
        server.start(sopts);

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 10;
        optsA.user = "alice";
        optsA.reconnection = false;
        TestClient alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 10;
        optsB.user = "bob";
        optsB.reconnection = false;
        TestClient bob(optsB);

        alice.connect();
        expect(test::waitFor([&] { return alice.gotOnline; }));

        bob.connect();
        expect(test::waitFor([&] { return bob.gotOnline; }));
        alice.client.joinRoom("shared");
        bob.client.joinRoom("shared");
        expect(test::waitFor([&] { return clientHasRoom(alice.client, "shared"); }));
        expect(test::waitFor([&] { return clientHasRoom(bob.client, "shared"); }));

        alice.client.sendPresence(false);
        bob.client.sendPresence(false);
        expect(test::waitFor([&] { return alice.gotRemotePresence && bob.gotRemotePresence; }));

        json::Value forged;
        forged["type"] = "presence";
        forged["to"] = json::Value::array();
        forged["to"].push_back("shared");
        forged["data"] = json::Value::object();
        forged["data"]["id"] = "FORGED_ID";
        forged["data"]["user"] = "FORGED_USER";
        forged["data"]["online"] = false;
        forged["data"]["agent"] = "presence-sanitized";
        alice.client.send(forged.dump());

        expect(test::waitFor([&] {
            auto* peer = bob.client.roster().get(alice.client.ourID());
            return peer != nullptr && peer->value("agent", "") == "presence-sanitized";
        }));

        auto* alicePeer = bob.client.roster().get(alice.client.ourID());
        expect(alicePeer != nullptr);
        expect(alicePeer->user() == "alice");
        expect(alicePeer->value("agent", "") == "presence-sanitized");
        expect((*alicePeer)["online"].get<bool>());
        expect(bob.client.roster().get("FORGED_ID") == nullptr);

        alice.client.close();
        bob.client.close();
        server.shutdown();
    });


    // =========================================================================
    // Direct messages require a shared room for both bare-user and user|id forms
    //
    icy_test::describe("hardening: direct message requires shared room", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 11;
        server.start(sopts);

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 11;
        optsA.user = "alice";
        optsA.reconnection = false;
        smpl::Client alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 11;
        optsB.user = "bob";
        optsB.reconnection = false;
        smpl::Client bob(optsB);

        bool aliceOnline = false;
        bool bobOnline = false;
        struct MessageCounter
        {
            int count = 0;
            void onMessage(smpl::Message& msg)
            {
                if (msg.contains("data") &&
                    msg["data"].is_object() &&
                    msg["data"].contains("text")) {
                    ++count;
                }
            }
        } counter;

        alice.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Online)
                aliceOnline = true;
        };
        bob.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Online)
                bobOnline = true;
        };
        bob += packetSlot(&counter, &MessageCounter::onMessage);

        alice.connect();
        bob.connect();
        expect(test::waitFor([&] { return aliceOnline && bobOnline; }));

        smpl::Message byUser;
        byUser.setTo(smpl::Address("bob"));
        byUser["data"]["text"] = "blocked-user";
        alice.send(byUser);

        smpl::Message bySession;
        bySession.setTo(smpl::Address("bob|" + bob.ourID()));
        bySession["data"]["text"] = "blocked-session";
        alice.send(bySession);

        icy::sleep(300);
        expect(counter.count == 0);

        alice.close();
        bob.close();
        server.shutdown();
    });


    // =========================================================================
    // Reconnect room-state: welcome.rooms is authoritative, desired rooms rejoin
    //
    icy_test::describe("hardening: reconnect room state is authoritative", []() {
        std::string assignedRoom = "alpha";

        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 12;
        server.start(sopts);
        server.Authenticate += [&](smpl::ServerPeer&, const json::Value&, bool& allowed, std::vector<std::string>& rooms) {
            allowed = true;
            rooms.push_back(assignedRoom);
        };

        smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT + 12;
        opts.user = "alice";
        opts.reconnection = false;
        smpl::Client client(opts);

        int onlineTransitions = 0;
        client.StateChange += [&](void*, smpl::ClientState& state, const smpl::ClientState&) {
            if (state.id() == smpl::ClientState::Online)
                ++onlineTransitions;
        };

        auto hasRoom = [&](const std::string& room) {
            auto rooms = client.rooms();
            return std::find(rooms.begin(), rooms.end(), room) != rooms.end();
        };

        client.connect();
        expect(test::waitFor([&] { return onlineTransitions == 1; }));

        client.joinRoom("desired");
        expect(test::waitFor([&] { return hasRoom("alpha") && hasRoom("desired") && hasRoom("alice"); }));

        client.close();
        icy::sleep(200);

        assignedRoom = "beta";
        client.connect();
        expect(test::waitFor([&] { return onlineTransitions == 2; }));
        expect(test::waitFor([&] { return hasRoom("beta") && hasRoom("desired") && hasRoom("alice"); }));
        expect(!hasRoom("alpha"));

        client.close();
        server.shutdown();
    });


    // =========================================================================
    // Multi-room fanout should dedupe recipients
    //
    icy_test::describe("hardening: room broadcast dedupes recipients", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 13;
        sopts.dynamicRooms = true;
        server.start(sopts);

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 13;
        optsA.user = "alice";
        optsA.reconnection = false;
        TestClient alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 13;
        optsB.user = "bob";
        optsB.reconnection = false;
        TestClient bob(optsB);

        struct MessageCounter
        {
            int count = 0;
            void onMessage(smpl::Message& msg)
            {
                if (msg.contains("data") &&
                    msg["data"].is_object() &&
                    msg["data"].value("text", "") == "DEDUP")
                    ++count;
            }
        } counter;

        bob.client += packetSlot(&counter, &MessageCounter::onMessage);

        alice.connect();
        bob.connect();
        expect(test::waitFor([&] { return alice.gotOnline && bob.gotOnline; }));

        alice.client.joinRoom("one");
        alice.client.joinRoom("two");
        bob.client.joinRoom("one");
        bob.client.joinRoom("two");
        expect(test::waitFor([&] {
            auto rooms = alice.client.rooms();
            return std::find(rooms.begin(), rooms.end(), "one") != rooms.end() &&
                   std::find(rooms.begin(), rooms.end(), "two") != rooms.end();
        }));
        expect(test::waitFor([&] {
            auto rooms = bob.client.rooms();
            return std::find(rooms.begin(), rooms.end(), "one") != rooms.end() &&
                   std::find(rooms.begin(), rooms.end(), "two") != rooms.end();
        }));

        json::Value msg;
        msg["type"] = "message";
        msg["to"] = json::Value::array();
        msg["to"].push_back("one");
        msg["to"].push_back("two");
        msg["data"] = json::Value::object();
        msg["data"]["text"] = "DEDUP";
        alice.client.send(msg.dump());

        expect(test::waitFor([&] { return counter.count == 1; }));

        alice.client.close();
        bob.client.close();
        server.shutdown();
    });


    // =========================================================================
    // Room broadcast authorization: can't broadcast to rooms you're not in
    //
    icy_test::describe("hardening: room broadcast requires membership", []() {
        smpl::Server server;
        smpl::Server::Options sopts;
        sopts.host = SERVER_HOST;
        sopts.port = SERVER_PORT + 14;
        sopts.dynamicRooms = true;
        server.start(sopts);

        smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 14;
        optsA.user = "alice";
        optsA.reconnection = false;
        TestClient alice(optsA);

        smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 14;
        optsB.user = "bob";
        optsB.reconnection = false;
        TestClient bob(optsB);

        alice.connect();
        expect(test::waitFor([&] { return alice.gotOnline; }));

        bob.connect();
        expect(test::waitFor([&] { return bob.gotOnline; }));

        // Bob joins a private room
        bob.client.joinRoom("private");
        expect(test::waitFor([&] { return clientHasRoom(bob.client, "private"); }));

        // Alice tries to broadcast to "private" room she's not in
        json::Value msg;
        msg["type"] = "message";
        msg["to"] = json::Value::array();
        msg["to"].push_back("private");
        msg["data"] = json::Value::object();
        msg["data"]["text"] = "SNEAKY";
        alice.client.send(msg.dump());

        icy::sleep(500);

        // Bob should NOT have received the message
        expect(bob.receivedMessage != "SNEAKY");

        alice.client.close();
        bob.client.close();
        server.shutdown();
    });


    icy_test::runAll();
    return icy_test::finalize();
}
