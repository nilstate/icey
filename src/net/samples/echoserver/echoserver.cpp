#include "scy/application.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"
#include "tcpechoserver.h"
#include "udpechoserver.h"


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
    // Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    // Logger::instance().setWriter(new AsyncLogWriter);
    net::SSLManager::initNoVerifyServer();
    {
        Servers srvs;
        srvs.tcp.start("0.0.0.0", TcpPort);
        srvs.ssl.start("0.0.0.0", SslPort);
        srvs.udp.start("0.0.0.0", UdpPort);

        std::cout << "TCP Lerver listening on " << srvs.tcp.server->address() << std::endl;
        std::cout << "SSL Server listening on " << srvs.ssl.server->address() << std::endl;
        std::cout << "UDP Lerver listening on " << srvs.udp.server->address() << std::endl;

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


#if 0 // raw libuv echo server
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uv.h>

uv_loop_t *loop;
struct sockaddr_in addr;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread != UV_EOF) {
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
            uv_close((uv_handle_t*)client, NULL);
        }
    }
    else if (nread > 0) {
        uv_write_t *req = (uv_write_t *)malloc(sizeof(uv_write_t));
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        uv_write(req, client, &wrbuf, 1, echo_write);
    }

    if (buf->base) {
        free(buf->base);
    }
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*)client) == 0) {
        uv_read_start((uv_stream_t*)client, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*)client, NULL);
    }
}

int main() {
    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    uv_ip4_addr("0.0.0.0", 1337, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*)&server, 128, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}
#endif
