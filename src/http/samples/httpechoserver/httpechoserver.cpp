#include "httpechoserver.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"
#include <iostream>


using namespace scy;


int main(int argc, char** argv)
{
    // NOTE: For best performance the HTTP server should be compiled on
    // Linux kernel 3.9 or newer in RELEASE mode with ENABLE_LOGGING=OFF

    // Match libuv's thread pool size to CPU cores for better I/O throughput
#ifdef SCY_UNIX
    int ncores = std::thread::hardware_concurrency();
    // std::cout << "threadpool size: " << ncores << std::endl;
    setenv("UV_THREADPOOL_SIZE", std::to_string(ncores).c_str(), 1);
#endif

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <key.pem> <cert.pem>" << '\n';
        return 1;
    }

    // Initialize SSL context with the provided key and certificate
    net::SSLManager::initNoVerifyServer(argv[1], argv[2]);

    // Start the HTTPS echo server (blocks until shutdown signal)
    // See httpechoserver.h for additional server variants:
    // raiseEchoServer(), runMulticoreEchoServers(), raiseBenchmarkServer()
    raiseHTTPSEchoServer();

    net::SSLManager::instance().shutdown();

    Logger::destroy();
    return 0;
}
