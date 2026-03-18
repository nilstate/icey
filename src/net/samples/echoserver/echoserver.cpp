///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Echo Server
//
// Demonstrates TCP, SSL, and UDP echo servers using the template-based
// EchoServer<SocketT> pattern. Each server echoes received data back
// to the client.
//
// Usage: echoserver <key.pem> <cert.pem>
//
// Test:
//   TCP:  echo "hello" | nc localhost 1337
//   SSL:  echo "hello" | openssl s_client -connect localhost:1338 -quiet
//   UDP:  echo "hello" | nc -u localhost 1339
//
/// @addtogroup net
/// @{


#include "scy/application.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"
#include "tcpechoserver.h"
#include "udpechoserver.h"
#include <iostream>


using std::endl;
using namespace scy;


const uint16_t TcpPort = 1337;
const uint16_t SslPort = 1338;
const uint16_t UdpPort = 1339;


/// Bundles all three echo server variants so they share a lifetime.
struct Servers
{
    net::TCPEchoServer tcp;
    net::SSLEchoServer ssl;
    net::UDPEchoServer udp;
};


int main(int argc, char** argv)
{
    // Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    // Logger::instance().setWriter(std::make_unique<AsyncLogWriter>());
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <key.pem> <cert.pem>" << std::endl;
        return 1;
    }

    // SSL needs a server context with key/cert before any SSL sockets are created
    net::SSLManager::initNoVerifyServer(argv[1], argv[2]);
    {
        // Start TCP, SSL, and UDP echo servers on consecutive ports
        Servers srvs;
        srvs.tcp.start("0.0.0.0", TcpPort);
        srvs.ssl.start("0.0.0.0", SslPort);
        srvs.udp.start("0.0.0.0", UdpPort);

        std::cout << "TCP server listening on " << srvs.tcp.server->address() << std::endl;
        std::cout << "SSL server listening on " << srvs.ssl.server->address() << std::endl;
        std::cout << "UDP server listening on " << srvs.udp.server->address() << std::endl;

        // Block until SIGINT/SIGTERM, then shut down all servers cleanly
        waitForShutdown([&](void*) {
            srvs.tcp.shutdown();
            srvs.ssl.shutdown();
            srvs.udp.shutdown();
        });
    }
    net::SSLManager::instance().shutdown();
    Logger::destroy();
    return 0;
}
