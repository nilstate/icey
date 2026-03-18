///
//
// LibSourcey HTTP Benchmark
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//

#include "scy/application.h"
#include "scy/http/server.h"
#include "scy/logger.h"

#include <uv.h>
#include <llhttp.h>

#include <cstring>
#include <iostream>
#include <thread>


using namespace scy;


constexpr uint16_t BenchmarkPort = 1337;
static const net::Address address("0.0.0.0", BenchmarkPort);


// =============================================================================
// LibSourcey HTTP server variants
// =============================================================================

/// Single-threaded benchmark server.
/// Sends a minimal HTTP response with no body.
void runSingleCore()
{
    http::Server srv(address);
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->response().add("Content-Length", "0");
        conn->response().add("Connection", "close");
        conn->sendHeader();
    };

    std::cout << "LibSourcey HTTP benchmark (single-core) listening on port "
              << BenchmarkPort << std::endl;
    waitForShutdown();
}


/// Single-threaded keep-alive benchmark server.
/// Sends a minimal HTTP response and keeps the connection open.
void runKeepAlive()
{
    http::Server srv(address);
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->response().set("Content-Length", "0");
        conn->sendHeader();
    };

    std::cout << "LibSourcey HTTP benchmark (keep-alive) listening on port "
              << BenchmarkPort << std::endl;
    waitForShutdown();
}


/// Per-core server instance for multicore mode.
void runMulticoreInstance()
{
    uv::ScopedLoop loop;

    http::Server srv(address, net::makeSocket<net::TCPSocket>(loop));
    srv.setReusePort();
    srv.start();

    srv.Connection += [&](http::ServerConnection::Ptr conn) {
        conn->response().add("Content-Length", "0");
        conn->response().add("Connection", "close");
        conn->sendHeader();
    };

    waitForShutdown([&](void*) {
        srv.shutdown();
    },
                    nullptr, loop);
}


/// Multicore benchmark server.
/// Spawns one server instance per CPU core with SO_REUSEPORT.
void runMultiCore()
{
    int ncpus = std::thread::hardware_concurrency();
    std::vector<std::unique_ptr<Thread>> threads;

    for (int i = 0; i < ncpus; ++i) {
        threads.push_back(std::make_unique<Thread>(std::bind(runMulticoreInstance)));
    }

    std::cout << "LibSourcey HTTP benchmark (multicore x" << ncpus
              << ") listening on port " << BenchmarkPort << std::endl;

    for (auto& thread : threads) {
        thread->join();
    }
}


/// Single-threaded echo server for echo benchmarks.
void runEcho()
{
    http::Server srv(address);
    srv.start();

    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            conn.send(bufferCast<const char*>(buffer), buffer.size());
            conn.close();
        };
    };

    std::cout << "LibSourcey HTTP echo (single-core) listening on port "
              << BenchmarkPort << std::endl;
    waitForShutdown();
}


// =============================================================================
// Raw libuv + llhttp baseline (theoretical maximum)
// =============================================================================

namespace rawuv {

static constexpr char RESPONSE_CLOSE[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n"
    "\r\n";
static constexpr size_t RESPONSE_CLOSE_LEN = sizeof(RESPONSE_CLOSE) - 1;

static constexpr char RESPONSE_KEEPALIVE[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 0\r\n"
    "\r\n";
static constexpr size_t RESPONSE_KEEPALIVE_LEN = sizeof(RESPONSE_KEEPALIVE) - 1;

static bool keepalive = false;

struct Client
{
    uv_tcp_t handle;
    llhttp_t parser;
    uv_write_t write_req;  // reused per connection
    char readbuf[4096];     // fixed read buffer, no heap alloc
};

static llhttp_settings_t settings;
static uv_loop_t* loop;

void on_close(uv_handle_t* handle)
{
    delete static_cast<Client*>(handle->data);
}

void on_write_close(uv_write_t* req, int /*status*/)
{
    uv_close(reinterpret_cast<uv_handle_t*>(req->handle), on_close);
}

void on_write_keepalive(uv_write_t* /*req*/, int /*status*/)
{
    // Connection stays open for next request
}

int on_message_complete(llhttp_t* parser)
{
    auto* client = static_cast<Client*>(parser->data);

    if (keepalive) {
        uv_buf_t buf = uv_buf_init(const_cast<char*>(RESPONSE_KEEPALIVE), RESPONSE_KEEPALIVE_LEN);
        uv_write(&client->write_req,
                 reinterpret_cast<uv_stream_t*>(&client->handle),
                 &buf, 1, on_write_keepalive);
    } else {
        uv_buf_t buf = uv_buf_init(const_cast<char*>(RESPONSE_CLOSE), RESPONSE_CLOSE_LEN);
        uv_write(&client->write_req,
                 reinterpret_cast<uv_stream_t*>(&client->handle),
                 &buf, 1, on_write_close);
    }
    return 0;
}

void alloc_cb(uv_handle_t* handle, size_t /*suggested*/, uv_buf_t* buf)
{
    // Use per-connection fixed buffer: zero heap allocation per read
    auto* client = static_cast<Client*>(handle->data);
    buf->base = client->readbuf;
    buf->len = sizeof(client->readbuf);
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* /*buf*/)
{
    auto* client = static_cast<Client*>(tcp->data);
    if (nread > 0) {
        llhttp_errno_t err = llhttp_execute(&client->parser, client->readbuf, nread);
        if (err != HPE_OK && err != HPE_PAUSED_UPGRADE) {
            uv_close(reinterpret_cast<uv_handle_t*>(&client->handle), on_close);
        }
    } else if (nread < 0) {
        uv_close(reinterpret_cast<uv_handle_t*>(&client->handle), on_close);
    }
}

void on_connection(uv_stream_t* server, int status)
{
    if (status < 0) return;

    auto* client = new Client();
    uv_tcp_init(loop, &client->handle);
    client->handle.data = client;

    llhttp_init(&client->parser, HTTP_REQUEST, &settings);
    client->parser.data = client;

    if (uv_accept(server, reinterpret_cast<uv_stream_t*>(&client->handle)) == 0) {
        uv_tcp_nodelay(&client->handle, 1);
        uv_read_start(reinterpret_cast<uv_stream_t*>(&client->handle), alloc_cb, on_read);
    } else {
        uv_close(reinterpret_cast<uv_handle_t*>(&client->handle), on_close);
    }
}

void run()
{
    llhttp_settings_init(&settings);
    settings.on_message_complete = on_message_complete;

    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", BenchmarkPort, &addr);
    uv_tcp_bind(&server, reinterpret_cast<const struct sockaddr*>(&addr), 0);
    uv_listen(reinterpret_cast<uv_stream_t*>(&server), 1000, on_connection);

    std::cout << "Raw libuv+llhttp benchmark ("
              << (keepalive ? "keep-alive" : "single-core")
              << ") listening on port " << BenchmarkPort << std::endl;

    uv_run(loop, UV_RUN_DEFAULT);
}

} // namespace rawuv


// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv)
{
    // Match libuv's thread pool size to CPU cores
#ifdef SCY_UNIX
    int ncores = std::thread::hardware_concurrency();
    setenv("UV_THREADPOOL_SIZE", std::to_string(ncores).c_str(), 1);
#endif

    std::string mode = (argc > 1) ? argv[1] : "single";

    if (mode == "single") {
        runSingleCore();
    } else if (mode == "keepalive") {
        runKeepAlive();
    } else if (mode == "multi") {
        runMultiCore();
    } else if (mode == "echo") {
        runEcho();
    } else if (mode == "raw") {
        rawuv::run();
    } else if (mode == "raw-keepalive") {
        rawuv::keepalive = true;
        rawuv::run();
    } else {
        std::cerr << "Usage: " << argv[0] << " [single|keepalive|multi|echo|raw|raw-keepalive]" << std::endl;
        return 1;
    }

    Logger::destroy();
    return 0;
}
