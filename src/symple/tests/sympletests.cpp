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
        std::make_unique<scy::ConsoleChannel>("debug", scy::Level::Trace));
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
        // Start server
        scy::smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT, .dynamicRooms = true});

        bool peerConnected = false;
        server.PeerConnected += [&](scy::smpl::ServerPeer& peer) {
            peerConnected = true;
        };

        // Create client
        scy::smpl::Client::Options opts;
        opts.host = SERVER_HOST;
        opts.port = SERVER_PORT;
        opts.user = "testuser";
        opts.name = "Test User";

        scy::smpl::Client client(opts);

        bool gotOnline = false;
        int announceStatus = 0;

        client.Announce += [&](const int& status) {
            announceStatus = status;
        };

        client.StateChange += [&](void*, scy::smpl::ClientState& state, const scy::smpl::ClientState&) {
            LDebug("Test: client state -> ", state.toString());
            if (state.id() == scy::smpl::ClientState::Online)
                gotOnline = true;
        };

        client.connect();

        // Run event loop for up to 3 seconds
        auto start = std::chrono::steady_clock::now();
        while (!gotOnline &&
               std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
            uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        }

        expect(gotOnline);
        expect(announceStatus == 200);
        expect(!client.ourID().empty());
        expect(peerConnected);
        expect(server.peerCount() == 1);

        client.close();
        server.shutdown();
    });


    scy_test::describe("client: two peers see each other's presence", []() {
        scy::smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 1, .dynamicRooms = true});

        // Client A
        scy::smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 1;
        optsA.user = "alice";
        optsA.name = "Alice";

        scy::smpl::Client clientA(optsA);
        bool aOnline = false;
        bool aGotPresence = false;

        clientA.StateChange += [&](void*, scy::smpl::ClientState& state, const scy::smpl::ClientState&) {
            if (state.id() == scy::smpl::ClientState::Online) {
                aOnline = true;
                clientA.joinRoom("test");
            }
        };

        clientA.PeerConnected += [&](scy::smpl::Peer& peer) {
            if (peer.user() != "alice")
                aGotPresence = true;
        };

        // Client B
        scy::smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 1;
        optsB.user = "bob";
        optsB.name = "Bob";

        scy::smpl::Client clientB(optsB);
        bool bOnline = false;
        bool bGotPresence = false;

        clientB.StateChange += [&](void*, scy::smpl::ClientState& state, const scy::smpl::ClientState&) {
            if (state.id() == scy::smpl::ClientState::Online) {
                bOnline = true;
                clientB.joinRoom("test");
            }
        };

        clientB.PeerConnected += [&](scy::smpl::Peer& peer) {
            if (peer.user() != "bob")
                bGotPresence = true;
        };

        clientA.connect();

        // Wait for A to come online before connecting B
        auto start = std::chrono::steady_clock::now();
        while (!aOnline &&
               std::chrono::steady_clock::now() - start < std::chrono::seconds(2)) {
            uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        }
        expect(aOnline);

        clientB.connect();

        // Wait for both to exchange presence
        start = std::chrono::steady_clock::now();
        while ((!bOnline || !aGotPresence || !bGotPresence) &&
               std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
            uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        }

        expect(bOnline);
        expect(server.peerCount() == 2);
        // B should have seen A's presence (A was online when B joined the user room)
        // A should have seen B's presence broadcast

        clientA.close();
        clientB.close();
        server.shutdown();
    });


    scy_test::describe("client: message routing", []() {
        scy::smpl::Server server;
        server.start({.host = SERVER_HOST, .port = SERVER_PORT + 2, .dynamicRooms = true});

        // Client A
        scy::smpl::Client::Options optsA;
        optsA.host = SERVER_HOST;
        optsA.port = SERVER_PORT + 2;
        optsA.user = "sender";

        scy::smpl::Client clientA(optsA);
        bool aOnline = false;
        std::string aId;

        clientA.StateChange += [&](void*, scy::smpl::ClientState& state, const scy::smpl::ClientState&) {
            if (state.id() == scy::smpl::ClientState::Online) {
                aOnline = true;
                aId = clientA.ourID();
            }
        };

        // Client B
        scy::smpl::Client::Options optsB;
        optsB.host = SERVER_HOST;
        optsB.port = SERVER_PORT + 2;
        optsB.user = "receiver";

        scy::smpl::Client clientB(optsB);
        bool bOnline = false;
        std::string receivedText;

        clientB.StateChange += [&](void*, scy::smpl::ClientState& state, const scy::smpl::ClientState&) {
            if (state.id() == scy::smpl::ClientState::Online)
                bOnline = true;
        };

        // Listen for all packets and check for our message
        static_cast<scy::PacketSignal&>(clientB).attach(
            [&receivedText](scy::IPacket& pkt) {
                auto* msg = dynamic_cast<scy::smpl::Message*>(&pkt);
                if (msg && msg->find("data") != msg->end() && (*msg)["data"].contains("text"))
                    receivedText = (*msg)["data"]["text"].get<std::string>();
            });

        clientA.connect();
        clientB.connect();

        auto start = std::chrono::steady_clock::now();
        while ((!aOnline || !bOnline) &&
               std::chrono::steady_clock::now() - start < std::chrono::seconds(2)) {
            uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        }
        expect(aOnline);
        expect(bOnline);

        // A sends message to B
        scy::smpl::Message msg;
        msg.setTo(scy::smpl::Address("receiver"));
        msg["data"]["text"] = "hello from sender";
        clientA.send(msg);

        // Wait for delivery
        start = std::chrono::steady_clock::now();
        while (receivedText.empty() &&
               std::chrono::steady_clock::now() - start < std::chrono::seconds(2)) {
            uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        }

        expect(receivedText == "hello from sender");

        clientA.close();
        clientB.close();
        server.shutdown();
    });


    scy_test::runAll();
    return scy_test::finalize();
}
