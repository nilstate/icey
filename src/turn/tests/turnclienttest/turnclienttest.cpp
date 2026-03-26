#include "icy/logger.h"
#include "icy/turn/fivetuple.h"
#include "icy/turn/iallocation.h"
#include "icy/turn/permission.h"
#include "icy/turn/server/permissionpolicy.h"
#include "icy/util.h"
#include "turnclienttest.h"
#include "tcpinitiator.h"
#include "tcpresponder.h"
#include "turnserver.h"
#include "udpinitiator.h"
#include "udpresponder.h"

#include <iostream>
#include <map>
#include <thread>


using namespace std;
using namespace icy;
using namespace icy::test;


int main(int argc, char** argv)
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();

    turn::Client::Options co;
    co.serverAddr = net::Address(TURN_SERVER_IP, TURN_SERVER_PORT);
    co.username = TURN_SERVER_USERNAME;
    co.password = TURN_SERVER_PASSWORD;
    co.lifetime = 120 * 1000; // 2 minutes
    co.timeout = 10 * 1000;
    co.timerInterval = 3 * 1000;


    // =========================================================================
    // TURN TCP Client (integration; requires event loop)
    //
    describe("TURN TCP client integration", [&]() {
        turn::ServerOptions so;
        so.software = "icey STUN/TURN Server [rfc5766]";
        so.realm = "0state.com";
        so.allocationDefaultLifetime = 1 * 60 * 1000;
        so.allocationMaxLifetime = 15 * 60 * 1000;
        so.timerInterval = 5 * 1000;
        so.listenAddr = net::Address("127.0.0.1", 3478);
        so.externalIP = TURN_SERVER_EXTERNAL_IP;
        TestServer srv(so);
        srv.start();

        TCPInitiator initiator(0, co);
        TCPResponder responder(0);

        bool testDone = false;
        bool testSuccess = false;

        initiator.AllocationCreated += [&]() {
            LDebug("Initiator allocation created");
            responder.connect(initiator.client.relayedAddress());
        };

        initiator.TestComplete += [&](bool success) {
            LDebug("Test complete: ", success);
            testSuccess = success;
            testDone = true;
        };

        initiator.start(TURN_AUTHORIZE_PEER_IP);

        expect(test::waitFor([&] { return testDone; }, 30000));
        expect(testSuccess);

        initiator.stop();
        responder.stop();
        srv.stop();

        // Drain the event loop to let libuv close callbacks complete
        // before the next test starts binding new sockets.
        test::waitFor([] { return false; }, 200);
    });


    // =========================================================================
    // TURN UDP Client (integration; requires event loop)
    //
    describe("TURN UDP client integration", [&]() {
        turn::ServerOptions so;
        so.software = "icey STUN/TURN Server [rfc5766]";
        so.realm = "0state.com";
        so.allocationDefaultLifetime = 1 * 60 * 1000;
        so.allocationMaxLifetime = 15 * 60 * 1000;
        so.timerInterval = 5 * 1000;
        so.listenAddr = net::Address("0.0.0.0", 3479); // wildcard bind exercises relay host resolution
        so.externalIP = "";
        TestServer srv(so);
        srv.start();

        turn::Client::Options uco = co;
        uco.serverAddr = net::Address(TURN_SERVER_IP, 3479);

        UDPInitiator initiator(0, uco);
        UDPResponder responder(0);

        bool testDone = false;
        bool testSuccess = false;

        initiator.AllocationCreated += [&]() {
            LDebug("Initiator allocation created");
            expect(initiator.client.relayedAddress().host() == "127.0.0.1");
            responder.connect(initiator.client.relayedAddress());
            initiator.responderAddress = net::Address(
                TURN_AUTHORIZE_PEER_IP, responder.socket.address().port());
        };

        initiator.TestComplete += [&](bool success) {
            LDebug("Test complete: ", success);
            testSuccess = success;
            testDone = true;
        };

        initiator.start(TURN_AUTHORIZE_PEER_IP);

        expect(test::waitFor([&] { return testDone; }, 30000));
        expect(testSuccess);

        initiator.stop();
        responder.stop();
        srv.stop();
        test::waitFor([] { return false; }, 200);
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

        // Different hosts should produce a strict ordering
        expect((a < b) || (b < a));
        expect(!(a == b));
    });

    describe("five tuple ordering strict weak", []() {
        // Verify the ordering is usable as a std::map key with varied tuples
        turn::FiveTuple tuples[] = {
            {net::Address("1.0.0.1", 100), net::Address("2.0.0.1", 200), net::TransportType::UDP},
            {net::Address("1.0.0.1", 100), net::Address("2.0.0.1", 200), net::TransportType::TCP},
            {net::Address("1.0.0.1", 101), net::Address("2.0.0.1", 200), net::TransportType::UDP},
            {net::Address("1.0.0.2", 100), net::Address("2.0.0.1", 200), net::TransportType::UDP},
            {net::Address("1.0.0.1", 100), net::Address("2.0.0.2", 200), net::TransportType::UDP},
        };

        // All distinct tuples should be distinguishable as map keys
        std::map<turn::FiveTuple, int> m;
        for (int i = 0; i < 5; i++) {
            m[tuples[i]] = i;
        }
        expect(m.size() == 5);

        // Each key should map back to its original value
        for (int i = 0; i < 5; i++) {
            expect(m[tuples[i]] == i);
        }
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

    describe("permission policy uses binary keys and numeric local checks", []() {
        turn::PermissionList perms;
        perms.push_back(turn::Permission(net::Address("10.0.0.1", 3478)));

        turn::PermissionPolicy policy(true);
        expect(policy.allowsExplicit(perms, turn::Permission::Key::fromAddress(net::Address("10.0.0.1", 5000))));
        expect(!policy.allowsExplicit(perms, turn::Permission::Key::fromAddress(net::Address("10.0.0.2", 5000))));
        expect(policy.allowsImplicit(turn::Permission::Key::fromAddress(net::Address("127.0.0.1", 1))));
        expect(policy.allowsImplicit(turn::Permission::Key::fromAddress(net::Address("172.16.0.1", 1))));
        expect(!policy.allowsImplicit(turn::Permission::Key::fromAddress(net::Address("172.32.0.1", 1))));
        expect(policy.allowsImplicit(turn::Permission::Key::fromAddress(net::Address("::1", 1))));
        expect(policy.allowsImplicit(turn::Permission::Key::fromAddress(net::Address("fe80::1", 1))));
        expect(!policy.allowsImplicit(turn::Permission::Key::fromAddress(net::Address("2001:db8::1", 1))));
    });


    // =========================================================================
    // IAllocation - lifetime and bandwidth
    //
    describe("allocation lifetime", []() {
        struct TestAlloc : public turn::IAllocation {
            TestAlloc(int64_t lifetime) : IAllocation(turn::FiveTuple(), "test", lifetime) {}
            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        TestAlloc alloc(10); // 10 seconds
        expect(!alloc.expired());
        expect(!alloc.deleted());
        expect(alloc.timeRemaining() > 0);
        expect(alloc.timeRemaining() <= 10);
        expect(alloc.username() == "test");
        expect(alloc.lifetime() == 10);
    });

    describe("allocation bandwidth limit", []() {
        struct TestAlloc : public turn::IAllocation {
            TestAlloc() : IAllocation(turn::FiveTuple(), "test", 600) {}
            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        TestAlloc alloc;
        expect(alloc.bandwidthRemaining() > 1000000);
        expect(alloc.bandwidthUsed() == 0);
        expect(!alloc.expired());

        alloc.setBandwidthLimit(1000);
        expect(alloc.bandwidthLimit() == 1000);
        expect(alloc.bandwidthRemaining() == 1000);

        alloc.updateUsage(400);
        expect(alloc.bandwidthUsed() == 400);
        expect(alloc.bandwidthRemaining() == 600);
        expect(!alloc.expired());

        alloc.updateUsage(600);
        expect(alloc.bandwidthUsed() == 1000);
        expect(alloc.bandwidthRemaining() == 0);
        expect(alloc.expired());
    });

    describe("allocation permissions", []() {
        struct TestAlloc : public turn::IAllocation {
            TestAlloc() : IAllocation(turn::FiveTuple(), "test", 600) {}
            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        TestAlloc alloc;
        expect(!alloc.hasPermission("10.0.0.1"));

        alloc.addPermission("10.0.0.1");
        expect(alloc.hasPermission("10.0.0.1"));
        expect(!alloc.hasPermission("10.0.0.2"));

        turn::IPList ips = {"10.0.0.2", "10.0.0.3"};
        alloc.addPermissions(ips);
        expect(alloc.hasPermission("10.0.0.2"));
        expect(alloc.hasPermission("10.0.0.3"));

        alloc.removePermission("10.0.0.2");
        expect(!alloc.hasPermission("10.0.0.2"));

        alloc.removeAllPermissions();
        expect(!alloc.hasPermission("10.0.0.1"));
    });

    describe("allocation permissions ignore port on address hot path", []() {
        struct TestAlloc : public turn::IAllocation {
            TestAlloc() : IAllocation(turn::FiveTuple(), "test", 600) {}
            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        TestAlloc alloc;
        alloc.addPermission(net::Address("10.0.0.1", 3478));
        expect(alloc.hasPermission(net::Address("10.0.0.1", 5000)));
        expect(!alloc.hasPermission(net::Address("10.0.0.2", 5000)));
    });

    describe("server allocation local permission checks are numeric", []() {
        struct TestObserver : public turn::ServerObserver {
            void onServerAllocationCreated(turn::Server*, turn::IAllocation*) override {}
            void onServerAllocationRemoved(turn::Server*, turn::IAllocation*) override {}
            turn::AuthenticationState authenticateRequest(turn::Server*, turn::Request&) override
            {
                return turn::AuthenticationState::Authorized;
            }
        };

        struct TestServerAlloc : public turn::ServerAllocation {
            TestServerAlloc(turn::Server& server)
                : ServerAllocation(server, turn::FiveTuple(), "test", 600)
            {
            }

            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        turn::ServerOptions so;
        so.enableLocalIPPermissions = true;
        TestObserver observer;
        turn::Server server(observer, so);
        TestServerAlloc alloc(server);

        expect(alloc.hasPermission(net::Address("10.1.2.3", 5000)));
        expect(alloc.hasPermission(net::Address("172.16.0.1", 5000)));
        expect(alloc.hasPermission(net::Address("172.31.255.255", 5000)));
        expect(!alloc.hasPermission(net::Address("172.32.0.1", 5000)));
        expect(alloc.hasPermission(net::Address("192.168.1.1", 5000)));
        expect(alloc.hasPermission(net::Address("127.0.0.1", 5000)));
        expect(!alloc.hasPermission(net::Address("8.8.8.8", 5000)));
    });

    describe("allocation permission refresh", []() {
        struct TestAlloc : public turn::IAllocation {
            TestAlloc() : IAllocation(turn::FiveTuple(), "test", 600) {}
            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        TestAlloc alloc;
        alloc.addPermission("10.0.0.1");
        auto perms1 = alloc.permissions();
        expect(perms1.size() == 1);

        // Adding the same IP should refresh, not duplicate
        alloc.addPermission("10.0.0.1");
        auto perms2 = alloc.permissions();
        expect(perms2.size() == 1);
    });

    describe("allocation set lifetime", []() {
        struct TestAlloc : public turn::IAllocation {
            TestAlloc() : IAllocation(turn::FiveTuple(), "test", 5) {}
            net::Address relayedAddress() const override { return net::Address("0.0.0.0", 0); }
        };

        TestAlloc alloc;
        expect(alloc.lifetime() == 5);
        expect(alloc.timeRemaining() > 0);

        alloc.setLifetime(600);
        expect(alloc.lifetime() == 600);
        expect(alloc.timeRemaining() > 5);
    });


    test::runAll();
    return test::finalize();
}
