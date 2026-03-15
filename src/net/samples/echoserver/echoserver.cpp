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
    net::SSLManager::initNoVerifyServer(argv[1], argv[2]);
    {
        Servers srvs;
        srvs.tcp.start("0.0.0.0", TcpPort);
        srvs.ssl.start("0.0.0.0", SslPort);
        srvs.udp.start("0.0.0.0", UdpPort);

        std::cout << "TCP server listening on " << srvs.tcp.server->address() << std::endl;
        std::cout << "SSL server listening on " << srvs.ssl.server->address() << std::endl;
        std::cout << "UDP server listening on " << srvs.udp.server->address() << std::endl;

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
