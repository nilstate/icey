#pragma once

#include "icy/application.h"
#include "icy/http/server.h"
#include "icy/net/sslmanager.h"
#include "icy/net/sslsocket.h"


namespace icy {


constexpr uint16_t HttpPort = 1337;
const net::Address address("0.0.0.0", HttpPort);


void raiseEchoServer()
{
    http::Server srv(address);
    srv.start();

    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.sendOwned(Buffer(bufferCast<const char*>(buffer),
                                  bufferCast<const char*>(buffer) + buffer.size()));
            conn.close();
        };
    };

    std::cout << "HTTP server listening on " << address << '\n';
    waitForShutdown();
}


void raiseHTTPSEchoServer()
{
    http::Server srv(address, std::make_shared<net::SSLSocket>(
                                  net::SSLManager::instance().defaultServerContext()));
    srv.start();

    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.sendOwned(Buffer(bufferCast<const char*>(buffer),
                                  bufferCast<const char*>(buffer) + buffer.size()));
            conn.close();
        };
    };

    std::cout << "HTTPS server listening on " << address << '\n';
    waitForShutdown();
}


void raiseMulticoreEchoServer()
{
    uv::ScopedLoop loop;

    http::Server srv(address, net::makeSocket<net::TCPSocket>(loop));
    srv.setReusePort();
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.sendOwned(Buffer(bufferCast<const char*>(buffer),
                                  bufferCast<const char*>(buffer) + buffer.size()));
            conn.close();
        };
    };

    waitForShutdown([&](void*) {
        srv.stop();
    },
                    nullptr, loop);
}


// Raise a server instance for each CPU core
void runMulticoreEchoServers()
{
    std::vector<std::unique_ptr<Thread>> threads;
    int ncpus = std::thread::hardware_concurrency();
    for (int i = 0; i < ncpus; ++i) {
        threads.push_back(std::make_unique<Thread>([]() { raiseMulticoreEchoServer(); }));
    }

    std::cout << "HTTP echo multicore(" << ncpus << ") server listening on " << address << '\n';

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

    std::cout << "HTTP server listening on " << address << '\n';
    waitForShutdown();
}


void raiseMulticoreBenchmarkServer()
{
    uv::ScopedLoop loop;

    http::Server srv(address, net::makeSocket<net::TCPSocket>(loop));
    srv.setReusePort();
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->response().add("Content-Length", "0");
        conn->response().add("Connection", "close"); // "keep-alive"
        conn->sendHeader();
    };

    waitForShutdown([&](void*) {
        srv.stop();
    },
                    nullptr, loop);
}


// Raise a server instance for each CPU core
void runMulticoreBenchmarkServers()
{
    std::vector<std::unique_ptr<Thread>> threads;
    int ncpus = std::thread::hardware_concurrency();
    for (int i = 0; i < ncpus; ++i) {
        threads.push_back(std::make_unique<Thread>([]() { raiseMulticoreBenchmarkServer(); }));
    }

    std::cout << "HTTP multicore(" << ncpus << ") server listening on " << address << '\n';

    for (auto& thread : threads) {
        thread->join();
    }
}


} // namespace icy
