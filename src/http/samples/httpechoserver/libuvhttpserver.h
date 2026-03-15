#include <cstdio>
#include <cstdlib>
#include <signal.h>
#ifdef __unix__
#include <unistd.h> // _SC_NPROCESSORS_ONLN on OS X
#endif
#include "uv.h"
#include <llhttp.h>

#include <iostream>
#include <sstream>
#include <string>


#ifdef DEBUG
#define CHECK(status, msg)                                     \
    if (status != 0) {                                         \
        fprintf(stderr, "%s: %s\n", msg, uv_err_name(status)); \
        exit(1);                                               \
    }
#define UVERR(err, msg) fprintf(stderr, "%s: %s\n", msg, uv_err_name(err))
#define LOG_ERROR(msg) puts(msg);
#define LOG(msg) puts(msg);
#define LOGF(...) printf(__VA_ARGS__);
#else
#define CHECK(status, msg)
#define UVERR(err, msg)
#define LOG_ERROR(msg)
#define LOG(msg)
#define LOGF(...)
#endif


namespace scy {
namespace libuv {


static int request_num = 1;
static uv_loop_t* uv_loop;
static uv_tcp_t server;
static llhttp_settings_t parser_settings;

struct client_t
{
    uv_tcp_t handle;
    llhttp_t parser;
    uv_write_t write_req;
    int request_num;
    std::string path;
};

void on_close(uv_handle_t* handle)
{
    auto* client = static_cast<client_t*>(handle->data);
    LOGF("[ %5d ] connection closed\n\n", client->request_num);
    delete client;
}

void alloc_cb(uv_handle_t* /*handle*/, size_t suggested_size, uv_buf_t* buf)
{
    *buf = uv_buf_init(new char[suggested_size], suggested_size);
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
{
    LOGF("on read: %ld\n", nread);
    auto* client = static_cast<client_t*>(tcp->data);
    if (nread >= 0) {
        llhttp_errno_t err = llhttp_execute(
            &client->parser, buf->base, nread);
        if (err == HPE_PAUSED_UPGRADE) {
            LOG_ERROR("parse error: cannot handle http upgrade");
            uv_close(reinterpret_cast<uv_handle_t*>(&client->handle), on_close);
        } else if (err != HPE_OK) {
            LOG_ERROR("parse error");
            uv_close(reinterpret_cast<uv_handle_t*>(&client->handle), on_close);
        }
    } else {
        if (nread != UV_EOF) {
            UVERR(nread, "read");
        }
        uv_close(reinterpret_cast<uv_handle_t*>(&client->handle), on_close);
    }
    delete[] buf->base;
}

struct render_baton
{
    render_baton(client_t* _client)
        : client(_client)
        , request()
        , result()
        , response_code("200 OK")
        , content_type("text/plain")
        , error(false)
    {
        request.data = this;
    }
    client_t* client;
    uv_work_t request;
    std::string result;
    std::string response_code;
    std::string content_type;
    bool error;
};

void after_write(uv_write_t* req, int status)
{
    CHECK(status, "write");
    if (!uv_is_closing(reinterpret_cast<uv_handle_t*>(req->handle))) {
        render_baton* closure = static_cast<render_baton*>(req->data);
        delete closure;
        uv_close(reinterpret_cast<uv_handle_t*>(req->handle), on_close);
    }
}

bool endswith(std::string const& value, std::string const& search)
{
    if (value.length() >= search.length()) {
        return (0 == value.compare(value.length() - search.length(), search.length(), search));
    } else {
        return false;
    }
}

void render(uv_work_t* req)
{
    auto* closure = static_cast<render_baton*>(req->data);
    auto* client = closure->client;
    LOGF("[ %5d ] render\n", client->request_num);

    closure->response_code = "200 OK";
}

void after_render(uv_work_t* req, int /*status*/)
{
    auto* closure = static_cast<render_baton*>(req->data);
    auto* client = closure->client;

    LOGF("[ %5d ] after render\n", client->request_num);

    std::ostringstream rep;
    rep << "HTTP/1.1 " << closure->response_code << "\r\n"
        << "Content-Type: " << closure->content_type << "\r\n"
        << "Connection: close\r\n"
        << "Content-Length: " << closure->result.size() << "\r\n"
        << "Access-Control-Allow-Origin: *" << "\r\n"
        << "\r\n";
    rep << closure->result;
    std::string res = rep.str();
    uv_buf_t resbuf;
    resbuf.base = const_cast<char*>(res.c_str());
    resbuf.len = res.size();

    client->write_req.data = closure;

    int r = uv_write(&client->write_req,
                     reinterpret_cast<uv_stream_t*>(&client->handle),
                     &resbuf, 1, after_write);
    CHECK(r, "write buff");
}

int on_message_begin(llhttp_t* /*parser*/)
{
    LOGF("\n***MESSAGE BEGIN***\n");
    return 0;
}

int on_headers_complete(llhttp_t* /*parser*/)
{
    LOGF("\n***HEADERS COMPLETE***\n");
    return 0;
}

int on_url(llhttp_t* parser, const char* url, size_t length)
{
    auto* client = static_cast<client_t*>(parser->data);
    LOGF("[ %5d ] on_url\n", client->request_num);
    LOGF("Url: %.*s\n", (int)length, url);

    // Simple path extraction - find path portion of URL
    std::string urlStr(url, length);
    auto pathStart = urlStr.find('/');
    if (pathStart != std::string::npos) {
        auto queryStart = urlStr.find('?', pathStart);
        if (queryStart != std::string::npos)
            client->path = urlStr.substr(pathStart, queryStart - pathStart);
        else
            client->path = urlStr.substr(pathStart);
    } else {
        fprintf(stderr, "\n\n*** failed to parse PATH in URL %.*s ***\n\n", (int)length, url);
        return -1;
    }
    return 0;
}

int on_header_field(llhttp_t* /*parser*/, const char* at, size_t length)
{
    LOGF("Header field: %.*s\n", (int)length, at);
    return 0;
}

int on_header_value(llhttp_t* /*parser*/, const char* at, size_t length)
{
    LOGF("Header value: %.*s\n", (int)length, at);
    return 0;
}

int on_body(llhttp_t* /*parser*/, const char* at, size_t length)
{
    LOGF("Body: %.*s\n", (int)length, at);
    return 0;
}

int on_message_complete(llhttp_t* parser)
{
    auto* client = static_cast<client_t*>(parser->data);
    LOGF("[ %5d ] on_message_complete\n", client->request_num);
    render_baton* closure = new render_baton(client);
    int status = uv_queue_work(uv_loop,
                               &closure->request,
                               render,
                               after_render);
    CHECK(status, "uv_queue_work");

    return 0;
}

void on_connect(uv_stream_t* server_handle, int status)
{
    CHECK(status, "connect");

    client_t* client = new client_t();
    client->request_num = request_num;
    request_num++;

    LOGF("[ %5d ] new connection\n", request_num);

    uv_tcp_init(uv_loop, &client->handle);

    llhttp_init(&client->parser, HTTP_REQUEST, &parser_settings);
    client->parser.data = client;
    client->handle.data = client;

    int r = uv_accept(server_handle, reinterpret_cast<uv_stream_t*>(&client->handle));
    CHECK(r, "accept");

    uv_read_start(reinterpret_cast<uv_stream_t*>(&client->handle), alloc_cb, on_read);
}

#define MAX_WRITE_HANDLES 1000

void raiseBenchmarkServer()
{
    llhttp_settings_init(&parser_settings);
    parser_settings.on_url = on_url;
    parser_settings.on_message_begin = on_message_begin;
    parser_settings.on_headers_complete = on_headers_complete;
    parser_settings.on_message_complete = on_message_complete;
    parser_settings.on_header_field = on_header_field;
    parser_settings.on_header_value = on_header_value;
    parser_settings.on_body = on_body;
    uv_loop = uv_default_loop();
    int r = uv_tcp_init(uv_loop, &server);
    CHECK(r, "tcp_init");
    r = uv_tcp_keepalive(&server, 1, 60);
    CHECK(r, "tcp_keepalive");
    struct sockaddr_in address;
    r = uv_ip4_addr("0.0.0.0", 1337, &address);
    CHECK(r, "ip4_addr");
    r = uv_tcp_bind(&server, reinterpret_cast<const struct sockaddr*>(&address), 0);
    CHECK(r, "tcp_bind");
    r = uv_listen(reinterpret_cast<uv_stream_t*>(&server), MAX_WRITE_HANDLES, on_connect);
    CHECK(r, "uv_listen");
    LOG("listening on port 1337");
    uv_run(uv_loop, UV_RUN_DEFAULT);
}


} // namespace libuv
} // namespace scy
