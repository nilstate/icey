#include "scy/base.h"
#include "scy/logger.h"
#include "scy/net/address.h"
#include "scy/net/socketemitter.h"
#include "scy/net/sslcontext.h"
#include "scy/net/sslmanager.h"
#include "scy/net/sslsocket.h"
#include "scy/net/tcpsocket.h"
#include "scy/net/udpsocket.h"
#include "scy/test.h"
#include "scy/time.h"

#include "../samples/echoserver/tcpechoserver.h"
#include "../samples/echoserver/udpechoserver.h"
#include "clientsockettest.h"


using namespace scy;
using namespace scy::test;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();

    net::SSLManager::initNoVerifyServer(
        std::string(SCY_SOURCE_DIR) + "/net/tests/key.pem",
        std::string(SCY_SOURCE_DIR) + "/net/tests/cert.pem");
    net::SSLManager::initNoVerifyClient();


    // =========================================================================
    // Address Test
    //
    describe("address", []() {
        net::Address sa1("192.168.1.100", 100);
        expect(sa1.host() == "192.168.1.100");
        expect(sa1.port() == 100);

        net::Address sa2("192.168.1.100", "100");
        expect(sa2.host() == "192.168.1.100");
        expect(sa2.port() == 100);

        net::Address sa3("192.168.1.100", "ftp");
        expect(sa3.host() == "192.168.1.100");
        expect(sa3.port() == 21);

        net::Address sa7("192.168.2.120:88");
        expect(sa7.host() == "192.168.2.120");
        expect(sa7.port() == 88);

        net::Address sa8("[192.168.2.120]:88");
        expect(sa8.host() == "192.168.2.120");
        expect(sa8.port() == 88);

        try {
            net::Address sa4("192.168.1.100", "f00bar");
            expect(0 && "bad service name - must throw");
        } catch (std::exception&) {
        }

        try {
            net::Address sa6("192.168.2.120", "80000");
            expect(0 && "invalid port - must throw");
        } catch (std::exception&) {
        }

        try {
            net::Address sa5("192.168.2.260", 80);
            expect(0 && "invalid address - must throw");
        } catch (std::exception&) {
        }

        try {
            net::Address sa9("[192.168.2.260:", 88);
            expect(0 && "invalid address - must throw");
        } catch (std::exception&) {
        }

        try {
            net::Address sa9("[192.168.2.260]");
            expect(0 && "invalid address - must throw");
        } catch (std::exception&) {
        }
    });


    // =========================================================================
    // Address: IPv6
    //
    describe("ipv6 address", []() {
        net::Address sa1("::1", 8080);
        expect(sa1.host() == "::1");
        expect(sa1.port() == 8080);
        expect(sa1.family() == net::Address::IPv6);

        net::Address sa2("[::1]:9090");
        expect(sa2.host() == "::1");
        expect(sa2.port() == 9090);

        net::Address sa3("::ffff:192.168.1.1", 80);
        expect(sa3.port() == 80);
    });


    // =========================================================================
    // Address: copy, assignment, comparison
    //
    describe("address copy and comparison", []() {
        net::Address a1("192.168.1.100", 100);
        net::Address a2(a1); // copy
        expect(a1 == a2);
        expect(!(a1 != a2));

        net::Address a3("192.168.1.100", 200);
        expect(a1 != a3);
        expect(a1 < a3); // same host, lower port

        // assignment
        net::Address a4("10.0.0.1", 80);
        a4 = a1;
        expect(a4 == a1);

        // swap
        net::Address a5("10.0.0.1", 80);
        net::Address a6("192.168.1.1", 443);
        a5.swap(a6);
        expect(a5.host() == "192.168.1.1");
        expect(a5.port() == 443);
        expect(a6.host() == "10.0.0.1");
        expect(a6.port() == 80);
    });


    // =========================================================================
    // Address: validation
    //
    describe("address validation", []() {
        expect(net::Address::validateIP("192.168.1.1"));
        expect(net::Address::validateIP("::1"));
        expect(net::Address::validateIP("0.0.0.0"));
        expect(!net::Address::validateIP("999.999.999.999"));
        expect(!net::Address::validateIP("not_an_ip"));
        expect(!net::Address::validateIP(""));
    });


    // =========================================================================
    // Address: toString
    //
    describe("address toString", []() {
        net::Address sa("192.168.1.100", 8080);
        std::string str = sa.toString();
        expect(str == "192.168.1.100:8080");

        net::Address wildcard;
        expect(wildcard.host() == "0.0.0.0");
        expect(wildcard.port() == 0);
    });


    // =========================================================================
    // Address: service resolution
    //
    describe("address service resolution", []() {
        expect(net::Address::resolveService("http") == 80);
        expect(net::Address::resolveService("https") == 443);
        expect(net::Address::resolveService("ftp") == 21);
        expect(net::Address::resolveService("8080") == 8080);
    });


    // =========================================================================
    // TCP Socket Test
    //
    describe("tcp socket test", []() {
        net::TCPEchoServer srv;
        srv.start("127.0.0.1", 1337);
        srv.server->unref();

        net::ClientSocketTest<net::TCPSocket> test(1337);
        test.run();
        uv::runLoop();

        expect(test.passed);
    });


    // =========================================================================
    // SSL Socket Test
    //
    describe("ssl socket test", []() {
        net::SSLEchoServer srv;
        srv.start("127.0.0.1", 1338);
        srv.server->unref();

        net::ClientSocketTest<net::SSLSocket> test(1338);
        test.run();
        uv::runLoop();

        expect(test.passed);
    });


    // =========================================================================
    // UDP Socket Test
    //
    describe("udp socket test", []() {
        net::UDPEchoServer srv;
        srv.start("127.0.0.1", 1339);
        srv.server->unref();

        net::ClientSocketTest<net::UDPSocket> test(1339);
        test.socket.bind(net::Address("0.0.0.0", 0));
        test.run();
        uv::runLoop();

        expect(test.passed);
    });

    // =========================================================================
    // DNS Resolver Test
    //
    describe("dns resolver test", []() {
        // net::resolveDNS("sourcey.com", 80, [&](const net::DNSResult& dns) {
        //     expect(dns.success());
        // });
        // net::resolveDNS("thishadbetternotexizt.com", 8888, [&](const net::DNSResult& dns) {
        //     expect(!dns.success());
        // });
        // uv::runLoop();
        {
            // bool success = false;
            // auto wrap = new net::GetAddrInfoReq();
            // wrap->callback = [&](const net::GetAddrInfoEvent& event) {
            //     expect(event.status == 0);
            //     success = event.status == 0;
            // };
            // wrap->resolve("sourcey.com", 80);
            // uv::runLoop();
            // expect(success);
            bool success = false;
            net::dns::resolve("sourcey.com", 80, [&](int err, const net::Address& addr) {
                expect(err == 0);
                success = err == 0;
            });
            uv::runLoop();
            expect(success);
        }
        {
            // bool success = false;
            // auto wrap = new net::GetAddrInfoReq();
            // wrap->callback = [&](const net::GetAddrInfoEvent& event) {
            //     expect(event.status != 0);
            //     success = event.status != 0;
            // };
            // wrap->resolve("hostthatdoesntexist.what", 80);
            // uv::runLoop();
            // expect(success);
            bool success = false;
            net::dns::resolve("hostthatdoesntexist.what", 80, [&](int err, const net::Address& addr) {
                expect(err != 0);
                success = err != 0;
            });
            uv::runLoop();
            expect(success);
        }
    });


    // =========================================================================
    // TCP Socket Error Test
    //
    describe("tcp socket error test", []() {
        {
            net::TCPSocket socket;
            socket.connect("192.169.7.888", 4500); // Connection refused
            uv::runLoop();
            expect(socket.error().any());
        }
        {
            net::TCPSocket socket;
            socket.connect("hostthatdoesntexist.what", 80); // DNS resolution will fail
            uv::runLoop();
            expect(socket.error().any());
        }
    });

    // =========================================================================
    // Socket Destructor Scope Test
    //
    describe("socket destructor scope test", []() {
        {
            net::TCPSocket socket;
            socket.connect("127.0.0.1", 80); // leave a dangling connect request
        }
        {
            net::TCPSocket socket;
            socket.connect("sourcey.com", 80); // with DNS
        }
        {
            net::TCPSocket socket;
            socket.bind(net::Address("0.0.0.1", 7331));
            socket.listen();
        }
        {
            net::SSLSocket socket;
            socket.connect("127.0.0.1", 80);
        }
        {
            net::SSLSocket socket;
            socket.connect("sourcey.com", 80);
        }
        {
            net::UDPSocket socket;
            socket.connect("127.0.0.1", 80);
        }
        {
            net::UDPSocket socket;
            socket.connect("sourcey.com", 80);
        }
        {
            net::UDPSocket socket;
            socket.bind(net::Address("0.0.0.1", 7331));
        }
        uv::runLoop();
    });

    // =========================================================================
    // TCP Socket Reconnection Test
    //
    describe("tcp socket reconnection test", []() {
        int connected = 0;
        net::SocketEmitter socket(std::make_shared<net::TCPSocket>());
        socket->connect("sourcey.com", 80);
        socket.Connect += [&](net::Socket& sock) -> bool {
            connected++;
            sock.close();
            return false;
        };
        socket.Close += [&](net::Socket& sock) -> bool {
            // connect again on close
            if (connected == 1) {
                sock.connect("sourcey.com", 80);
                // assert(0);
            }
            return false;
        };

        uv::runLoop();
        expect(connected == 2);
    });


    // =========================================================================
    // SSL Hostname Verification Test
    //
    // NOTE: This test requires network access to connect to an external host.
    // It will fail in environments without internet connectivity (e.g. some CI).
    //
    describe("ssl hostname verification", []() {
        // Create a client SSL context with certificate verification enabled
        // and system CA certificates loaded
        auto ctx = std::make_shared<net::SSLContext>(
            net::SSLContext::CLIENT_USE, "", // no caLocation, use default CAs
            net::SSLContext::VERIFY_RELAXED,
            9,     // verification depth
            true,  // loadDefaultCAs
            "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

        bool connected = false;
        bool gotError = false;

        auto sslSock = std::make_shared<net::SSLSocket>(ctx);
        net::SocketEmitter socket(sslSock);

        socket.Connect += [&](net::Socket& sock) -> bool {
            connected = true;
            sock.close();
            return false;
        };
        socket.Error += [&](net::Socket& sock, const scy::Error& err) -> bool {
            LError("SSL hostname verification error: ", err.message);
            gotError = true;
            return false;
        };

        // connect() sets the hostname automatically for SNI and verification
        socket->connect("www.google.com", 443);

        uv::runLoop();

        expect(connected);
        expect(!gotError);
    });


    // =========================================================================
    // UDP Socket Reconnection Test
    //
    describe("udp socket reconnection test", []() {
        net::UDPEchoServer srv;
        srv.start("127.0.0.1", 1337);
        srv.server->unref();

        int connected = 0;
        net::SocketEmitter socket(std::make_shared<net::UDPSocket>());
        socket.Connect += [&](net::Socket& sock) -> bool {
            connected++;
            sock.close();
            return false;
        };
        socket.Close += [&](net::Socket& sock) -> bool {
            // connect again on close
            if (connected == 1) {
                sock.connect("127.0.0.1", 1337);
            }
            return false;
        };
        socket->connect("127.0.0.1", 1337);

        uv::runLoop();
        expect(connected == 2);
    });

    test::runAll();

    return test::finalize();
}
