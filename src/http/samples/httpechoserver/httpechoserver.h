#include "scy/application.h"
#include "scy/http/server.h"
#include "scy/net/sslmanager.h"
#include "scy/net/sslsocket.h"


namespace scy {


constexpr uint16_t HttpPort = 1337;
const net::Address address("0.0.0.0", HttpPort);


void raiseEchoServer()
{
    http::Server srv(address);
    srv.start();

    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.send(bufferCast<const char*>(buffer), buffer.size());
            conn.close();
        };
    };

    std::cout << "HTTP server listening on " << address << std::endl;
    waitForShutdown();
}


void raiseHTTPSEchoServer()
{
    http::Server srv(address, std::make_shared<net::SSLSocket>(
                                  net::SSLManager::instance().defaultServerContext()));
    srv.start();

    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.send(bufferCast<const char*>(buffer), buffer.size());
            conn.close();
        };
    };

    std::cout << "HTTPS server listening on " << address << std::endl;
    waitForShutdown();
}


void raiseMulticoreEchoServer()
{
    uv::ScopedLoop loop;

    http::Server srv(address, net::makeSocket<net::TCPSocket>(loop));
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.send(bufferCast<const char*>(buffer), buffer.size());
            conn.close();
        };
    };

    waitForShutdown([&](void*) {
        srv.shutdown();
    },
                    nullptr, loop);
}


// Raise a server instance for each CPU core
void runMulticoreEchoServers()
{
    std::vector<std::unique_ptr<Thread>> threads;
    int ncpus = std::thread::hardware_concurrency();
    for (int i = 0; i < ncpus; ++i) {
        threads.push_back(std::make_unique<Thread>(std::bind(raiseMulticoreEchoServer)));
    }

    std::cout << "HTTP echo multicore(" << ncpus << ") server listening on " << address << std::endl;

    for (auto& thread : threads) {
        thread->join();
    }
}


// -----------------------------------------------------------------------------
// Benchmark server

void raiseBenchmarkServer()
{
    http::Server srv(address);
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->response().add("Content-Length", "0");
        conn->response().add("Connection", "close"); // "keep-alive"
        conn->sendHeader();
        // conn->send("hello universe", 14);
        // conn->close();
    };

    std::cout << "HTTP server listening on " << address << std::endl;
    waitForShutdown();
}


void raiseMulticoreBenchmarkServer()
{
    uv::ScopedLoop loop;

    http::Server srv(address, net::makeSocket<net::TCPSocket>(loop));
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->response().add("Content-Length", "0");
        conn->response().add("Connection", "close"); // "keep-alive"
        conn->sendHeader();
    };

    waitForShutdown([&](void*) {
        srv.shutdown();
    },
                    nullptr, loop);
}


// Raise a server instance for each CPU core
void runMulticoreBenchmarkServers()
{
    std::vector<std::unique_ptr<Thread>> threads;
    int ncpus = std::thread::hardware_concurrency();
    for (int i = 0; i < ncpus; ++i) {
        threads.push_back(std::make_unique<Thread>(std::bind(raiseMulticoreBenchmarkServer)));
    }

    std::cout << "HTTP multicore(" << ncpus << ") server listening on " << address << std::endl;

    for (auto& thread : threads) {
        thread->join();
    }
}


} // namespace scy
