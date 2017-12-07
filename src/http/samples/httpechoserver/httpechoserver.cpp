#include "httpechoserver.h"
#include "libuvhttpserver.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"


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

    net::SSLManager::initNoVerifyServer();

    // Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    // Logger::instance().setWriter(new AsyncLogWriter);
//
// #if SCY_HAS_KERNEL_SOCKET_LOAD_BALANCING
    // runMulticoreBenchmarkServers();
// #else
    // raiseBenchmarkServer();
// #endif
    // runMulticoreEchoServers();
    raiseHTTPSEchoServer();
    // rlibuv::raiseBenchmarkServer();


    net::SSLManager::instance().shutdown();

    Logger::destroy();
    return 0;
}
