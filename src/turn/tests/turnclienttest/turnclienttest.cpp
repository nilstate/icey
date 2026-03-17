#include "scy/logger.h"
#include "scy/turn/fivetuple.h"
#include "scy/turn/permission.h"
#include "scy/util.h"
#include "turnclienttest.h"
#include "tcpinitiator.h"
#include "tcpresponder.h"
#include "turnserver.h"
#include "udpinitiator.h"
#include "udpresponder.h"

#include <iostream>
#include <thread>


using namespace std;
using namespace scy;
using namespace scy::test;


int main(int argc, char** argv)
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    // Logger::instance().setWriter(std::make_unique<AsyncLogWriter>());
    test::init();

    turn::Client::Options co;
    co.serverAddr = net::Address(TURN_SERVER_IP, TURN_SERVER_PORT);
    co.username = TURN_SERVER_USERNAME;
    co.password = TURN_SERVER_PASSWORD;
    co.lifetime = 120 * 1000; // 1 minute
    co.timeout = 10 * 1000;
    co.timerInterval = 3 * 1000;


    // =========================================================================
    // TURN TCP Client
    //
    describe("TURN TCP client", [&]() {
#if RAISE_LOCAL_SERVER
        RunTestServer()
#endif
            TCPInitiator initiator(0, co);
        TCPResponder responder(0);

        initiator.AllocationCreated += [&]() {
            LDebug("Initiator allocation created");

            // Start the responder when the allocation is created
            responder.connect(initiator.client.relayedAddress());
        };

        initiator.TestComplete += [&](bool success) {
            LDebug("Test complete: ", success);
            expect(success);
            initiator.shutdown();
            responder.shutdown();
#if RAISE_LOCAL_SERVER
            srv.stop();
#endif
        };

        initiator.initiate(TURN_AUTHORIZE_PEER_IP);

        uv::runLoop();
    });


    // =========================================================================
    // TURN UDP Client
    //
    describe("TURN UDP client", [&]() {
#if RAISE_LOCAL_SERVER
        RunTestServer()
#endif
            UDPInitiator initiator(0, co);
        UDPResponder responder(0);

        initiator.AllocationCreated += [&]() {
            LDebug("Initiator allocation created");

            // Start the responder when the allocation is created
            responder.connect(initiator.client.relayedAddress());

            // Set the local responder address for UDP send indications
            initiator.responderAddress = net::Address(
                TURN_AUTHORIZE_PEER_IP, responder.socket.address().port());
        };

        initiator.TestComplete += [&](bool success) {
            LDebug("Test complete: ", success);
            expect(success);
            initiator.shutdown();
            responder.shutdown();
#if RAISE_LOCAL_SERVER
            srv.stop();
#endif
        };

        initiator.initiate(TURN_AUTHORIZE_PEER_IP);

        uv::runLoop();
    });


    // =========================================================================
    // FiveTuple
    //
    describe("five tuple", []() {
        net::Address remote("192.168.1.1", 5000);
        net::Address local("10.0.0.1", 3478);

        turn::FiveTuple ft(remote, local, net::TransportType::UDP);
        expect(ft.remote() == remote);
        expect(ft.local() == local);
        expect(ft.transport() == net::TransportType::UDP);
    });

    describe("five tuple equality", []() {
        net::Address r1("1.2.3.4", 100);
        net::Address l1("5.6.7.8", 200);

        turn::FiveTuple a(r1, l1, net::TransportType::UDP);
        turn::FiveTuple b(r1, l1, net::TransportType::UDP);
        expect(a == b);

        // Different transport
        turn::FiveTuple c(r1, l1, net::TransportType::TCP);
        expect(!(a == c));

        // Different remote
        turn::FiveTuple d(net::Address("9.9.9.9", 100), l1, net::TransportType::UDP);
        expect(!(a == d));
    });

    describe("five tuple ordering", []() {
        turn::FiveTuple a(net::Address("1.0.0.1", 1), net::Address("2.0.0.1", 2), net::TransportType::UDP);
        turn::FiveTuple b(net::Address("1.0.0.2", 1), net::Address("2.0.0.1", 2), net::TransportType::UDP);

        // Just ensure < doesn't crash and provides a total order
        expect((a < b) || (b < a) || (a == b));
    });

    describe("five tuple toString", []() {
        turn::FiveTuple ft(net::Address("10.0.0.1", 5000), net::Address("10.0.0.2", 3478), net::TransportType::TCP);
        std::string str = ft.toString();
        expect(!str.empty());
        expect(str.find("10.0.0.1") != std::string::npos);
    });


    // =========================================================================
    // Permission
    //
    describe("permission", []() {
        turn::Permission perm("192.168.1.50");
        expect(perm.ip == "192.168.1.50");
        expect(perm == std::string("192.168.1.50"));
        expect(!(perm == std::string("10.0.0.1")));
    });

    describe("permission timeout", []() {
        turn::Permission perm("1.2.3.4");
        // Permission starts with a timeout (PERMISSION_LIFETIME = 300s)
        expect(!perm.timeout.expired());

        perm.refresh();
        expect(!perm.timeout.expired());
    });

    describe("permission list", []() {
        turn::PermissionList perms;
        perms.push_back(turn::Permission("10.0.0.1"));
        perms.push_back(turn::Permission("10.0.0.2"));
        perms.push_back(turn::Permission("10.0.0.3"));

        expect(perms.size() == 3);

        // Find by IP using operator==
        auto it = std::find(perms.begin(), perms.end(), std::string("10.0.0.2"));
        expect(it != perms.end());
        expect(it->ip == "10.0.0.2");

        auto notFound = std::find(perms.begin(), perms.end(), std::string("99.99.99.99"));
        expect(notFound == perms.end());
    });


    test::runAll();
    return test::finalize();
}
