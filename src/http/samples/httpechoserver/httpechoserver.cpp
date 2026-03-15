#include "httpechoserver.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"
#include <iostream>


using namespace scy;


int main(int argc, char** argv)
{
    // NOTE: For best performance the HTTP server should be compiled on
    // Linux kernel 3.9 or newer in RELEASE mode with ENABLE_LOGGING=OFF
#ifdef SCY_UNIX
    int ncores = std::thread::hardware_concurrency();
    // std::cout << "threadpool size: " << ncores << std::endl;
    setenv("UV_THREADPOOL_SIZE", std::to_string(ncores).c_str(), 1);
#endif

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <key.pem> <cert.pem>" << std::endl;
        return 1;
    }
    net::SSLManager::initNoVerifyServer(argv[1], argv[2]);

    raiseHTTPSEchoServer();

    net::SSLManager::instance().shutdown();

    Logger::destroy();
    return 0;
}
