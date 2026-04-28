///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/server.h"
#include "icy/http/websocket.h"
#include "icy/logger.h"
#include "icy/util.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string_view>

namespace stdfs = std::filesystem;


namespace icy {
namespace http {

namespace {

Timestamp normalizeHttpTimestamp(const Timestamp& ts)
{
    return Timestamp::fromEpochTime(ts.epochTime());
}

std::chrono::system_clock::time_point toSystemTime(stdfs::file_time_type fileTime)
{
    // Cross-platform conversion from filesystem time to system_clock time.
    // file_clock::to_sys is not a member of MSVC's file_clock implementation,
    // so we use the standard "delta calibration" idiom: compute the offset
    // between file_clock::now() and system_clock::now() and apply it. Works
    // on libstdc++, libc++, and MSVC, and tolerates the libc++ case where
    // file_clock returns nanosecond precision (system_clock::duration on
    // libstdc++ and microsecond on libc++).
    using namespace std::chrono;
    return time_point_cast<system_clock::duration>(
        system_clock::now() + (fileTime - stdfs::file_time_type::clock::now()));
}

std::string makeWeakETag(uint64_t size, long long version)
{
    char buf[64];
    std::snprintf(buf, sizeof(buf), "W/\"%llx-%llx\"",
                  static_cast<unsigned long long>(size),
                  static_cast<unsigned long long>(version));
    return buf;
}

std::string normalizeETagToken(std::string token)
{
    util::trimInPlace(token);
    if (token.size() >= 2 &&
        (token[0] == 'W' || token[0] == 'w') &&
        token[1] == '/') {
        token.erase(0, 2);
        util::trimInPlace(token);
    }
    return token;
}

bool matchesIfNoneMatch(std::string_view header, std::string_view currentEtag)
{
    const std::string normalizedCurrent = normalizeETagToken(std::string(currentEtag));
    for (auto token : util::split(header, ',')) {
        util::trimInPlace(token);
        if (token == "*")
            return true;
        if (normalizeETagToken(std::move(token)) == normalizedCurrent)
            return true;
    }
    return false;
}

bool isGetOrHead(const Request& request)
{
    return util::icompare(request.getMethod(), Method::Get) == 0 ||
           util::icompare(request.getMethod(), Method::Head) == 0;
}

bool parseHttpDate(std::string_view value, Timestamp& out)
{
    try {
        int tzd = 0;
        out = Timestamp::fromEpochTime(
            DateTimeParser::parse(std::string(value), tzd).timestamp().epochTime());
        return true;
    }
    catch (...) {
        return false;
    }
}

void setConditionalResponse(Response& response, StatusCode status)
{
    response.setStatus(status);
    response.setChunkedTransferEncoding(false);
    response.setContentLength(0);
}

} // namespace


bool statStaticFile(std::string_view path, StaticFileInfo& info)
{
    std::error_code ec;
    const stdfs::path filePath(path);
    if (!stdfs::is_regular_file(filePath, ec) || ec)
        return false;

    const auto size = stdfs::file_size(filePath, ec);
    if (ec)
        return false;

    const auto fileTime = stdfs::last_write_time(filePath, ec);
    if (ec)
        return false;

    const auto systemTime = toSystemTime(fileTime);
    const auto version = std::chrono::time_point_cast<std::chrono::nanoseconds>(
        systemTime).time_since_epoch().count();

    info.size = size;
    info.lastModified = normalizeHttpTimestamp(
        Timestamp::fromEpochTime(std::chrono::system_clock::to_time_t(systemTime)));
    info.etag = makeWeakETag(info.size, version);
    return true;
}


bool prepareStaticFileResponse(const Request& request,
                               Response& response,
                               const StaticFileInfo& info)
{
    response.setContentLength(info.size);
    response.set("ETag", info.etag);
    response.set("Last-Modified",
                 DateTimeFormatter::format(info.lastModified,
                                           DateTimeFormat::HTTP_FORMAT));

    const auto& ifNoneMatch = request.get("If-None-Match", Message::EMPTY);
    if (!ifNoneMatch.empty()) {
        if (matchesIfNoneMatch(ifNoneMatch, info.etag)) {
            setConditionalResponse(
                response,
                isGetOrHead(request) ? StatusCode::NotModified
                                     : StatusCode::PreconditionFailed);
            return true;
        }
        return false;
    }

    if (!isGetOrHead(request))
        return false;

    const auto& ifModifiedSince = request.get("If-Modified-Since", Message::EMPTY);
    if (ifModifiedSince.empty())
        return false;

    Timestamp since;
    if (!parseHttpDate(ifModifiedSince, since))
        return false;

    if (info.lastModified.epochTime() <= since.epochTime()) {
        setConditionalResponse(response, StatusCode::NotModified);
        return true;
    }

    return false;
}


Server::Server(const std::string& host, short port, uv::Loop* loop, std::unique_ptr<ServerConnectionFactory> factory)
    : _loop(loop)
    , _address(host, port)
    , _socket(net::makeSocket<net::TCPSocket>(loop))
    , _timer(1000, 1000, loop)
    , _factory(std::move(factory))
{
}


Server::Server(const net::Address& address, uv::Loop* loop, std::unique_ptr<ServerConnectionFactory> factory)
    : _loop(loop)
    , _address(address)
    , _socket(net::makeSocket<net::TCPSocket>(loop))
    , _timer(1000, 1000, loop)
    , _factory(std::move(factory))
{
}


Server::Server(const std::string& host, short port, net::TCPSocket::Ptr socket, std::unique_ptr<ServerConnectionFactory> factory)
    : _loop(socket->loop())
    , _address(host, port)
    , _socket(std::move(socket))
    , _timer(1000, 1000, _loop)
    , _factory(std::move(factory))
{
}


Server::Server(const net::Address& address, net::TCPSocket::Ptr socket, std::unique_ptr<ServerConnectionFactory> factory)
    : _loop(socket->loop())
    , _address(address)
    , _socket(std::move(socket))
    , _timer(1000, 1000, _loop)
    , _factory(std::move(factory))
{
}


Server::~Server()
{
    stop();
}


void Server::start()
{
    _socket->addReceiver(this);
    _socket->AcceptConnection += slot(this, &Server::onClientSocketAccept);
    _socket->bind(_address);

    // Enable kernel-level socket load balancing (Linux 3.9+).
    // This allows multiple server instances to bind to the same
    // address:port, with the kernel distributing accepts evenly.
    if (_reusePort)
        (void)_socket->setReusePort();

    _socket->listen(1000);

    LDebug("HTTP server listening on ", _address);

    _dateCache.update();
    _timer.Timeout += slot(this, &Server::onTimer);
    _timer.start();
}


void Server::stop()
{
    // Stop accepting new connections
    if (_socket) {
        _socket->removeReceiver(this);
        _socket->AcceptConnection -= slot(this, &Server::onClientSocketAccept);
        _socket->close();
    }

    _timer.stop();

    // Close all active connections. Disconnect our close callback first
    // to prevent onConnectionClose from modifying the map during iteration.
    //
    // Connection destruction must be deferred: Connection::close() calls
    // uv_close() which is async. The uv handle's write callbacks still
    // reference the C++ Stream object. If we destroy the connection now,
    // those callbacks access freed memory.
    //
    // Solution: move the shared_ptrs into an idle callback that releases
    // them on the next event loop iteration, after uv_close completes.
    auto* loop = _loop;
    auto* deferred = new std::vector<ServerConnection::Ptr>();
    for (auto& [ptr, conn] : _connections) {
        conn->Close -= slot(this, &Server::onConnectionClose);
        conn->close();
        deferred->push_back(std::move(conn));
    }
    _connections.clear();

    // Also defer pool cleanup
    while (auto pooled = _pool.acquire())
        deferred->push_back(std::move(pooled));

    // Release the connections on the next event loop iteration
    if (!deferred->empty()) {
        auto* idle = new uv_idle_t;
        uv_idle_init(loop, idle);
        idle->data = deferred;
        uv_idle_start(idle, [](uv_idle_t* handle) {
            uv_idle_stop(handle);
            delete static_cast<std::vector<ServerConnection::Ptr>*>(handle->data);
            uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* h) {
                delete reinterpret_cast<uv_idle_t*>(h);
            });
        });
    } else {
        delete deferred;
    }

    Shutdown.emit();
}


std::unique_ptr<ServerResponder> Server::createResponder(ServerConnection& conn)
{
    // The initial HTTP request headers have already
    // been parsed at this point, but the request body may
    // be incomplete (especially if chunked).
    return _factory->createResponder(conn);
}


void Server::onClientSocketAccept(const net::TCPSocket::Ptr& socket)
{
    ServerConnection::Ptr conn = _pool.acquire();
    if (conn) {
        conn->reset(socket);
    } else {
        conn = _factory->createConnection(*this, socket);
    }
    conn->markActive();
    conn->Close += slot(this, &Server::onConnectionClose);
    _connections.emplace(conn.get(), conn);
}


void Server::onConnectionReady(ServerConnection& conn)
{
    auto it = _connections.find(&conn);
    if (it != _connections.end())
        Connection.emit(it->second);
}


void Server::onConnectionClose(ServerConnection& conn)
{
    auto it = _connections.find(&conn);
    if (it != _connections.end()) {
        auto ptr = std::move(it->second);
        _connections.erase(it);

        // Return to pool if reusable and error-free.
        if (conn.reusableForPool() && !conn.error().any() && _pool.release(ptr)) {
            // Pooled for reuse — nothing else to do.
        } else if (ptr) {
            // Defer destruction: this callback may be firing from
            // inside the connection's own method chain (e.g. onHeaders
            // -> createResponder -> rejected -> close -> onClose -> here).
            // Releasing the shared_ptr now would destroy the connection
            // while we're still on its call stack. This also covers
            // the case where the pool is full and rejected the connection.
            auto* deferred = new ServerConnection::Ptr(std::move(ptr));
            auto* idle = new uv_idle_t;
            uv_idle_init(_loop, idle);
            idle->data = deferred;
            uv_idle_start(idle, [](uv_idle_t* handle) {
                uv_idle_stop(handle);
                delete static_cast<ServerConnection::Ptr*>(handle->data);
                uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* h) {
                    delete reinterpret_cast<uv_idle_t*>(h);
                });
            });
        }
    }
}


bool Server::onSocketClose(net::Socket& socket)
{
    return false;
}


void Server::onTimer()
{
    _dateCache.update();

    // Close idle keep-alive connections
    if (_keepAliveTimeout > 0) {
        std::vector<ServerConnection*> idle;
        for (auto& [ptr, conn] : _connections) {
            if (conn->idleTimeoutEnabled() && conn->idleSeconds() > _keepAliveTimeout)
                idle.push_back(ptr);
        }
        for (auto* ptr : idle) {
            auto it = _connections.find(ptr);
            if (it != _connections.end())
                it->second->close();
        }
    }
}


net::Address& Server::address()
{
    return _address;
}


//
// Server Connection
//


ServerConnection::ServerConnection(Server& server, net::TCPSocket::Ptr socket)
    : Connection(socket)
    , _server(server)
{
    replaceAdapter(std::make_unique<ConnectionAdapter>(this, HTTP_REQUEST));
    touch();
}


ServerConnection::~ServerConnection()
{
    close();
}


Server& ServerConnection::server()
{
    return _server;
}


bool ServerConnection::idleTimeoutEnabled() const
{
    switch (_state) {
        case ServerConnectionState::ReceivingHeaders:
        case ServerConnectionState::ReceivingBody:
        case ServerConnectionState::DispatchingOrSending:
            return true;
        case ServerConnectionState::Streaming:
        case ServerConnectionState::Upgraded:
        case ServerConnectionState::Closing:
        case ServerConnectionState::Closed:
            return false;
    }
    return false;
}


bool ServerConnection::reusableForPool() const
{
    return _mode == ServerConnectionMode::Http;
}


void ServerConnection::reset(net::TCPSocket::Ptr socket)
{
    // Detach from old socket
    if (_socket) {
        _socket->removeReceiver(_adapter);
    }

    // Reset connection state
    _responder.reset();
    _closed = false;
    _shouldSendHeader = true;
    _error.reset();
    _mode = ServerConnectionMode::Http;
    _state = ServerConnectionState::ReceivingHeaders;

    // Clear request/response (keeps string/vector capacity)
    _request.clear();
    _request.setMethod("");
    _request.setURI("");
    _request.setVersion(Message::HTTP_1_1);
    _response.clear();
    _response.setStatus(StatusCode::OK);
    _response.setReason(getStatusCodeReason(StatusCode::OK));
    _response.setVersion(Message::HTTP_1_1);

    // Swap socket
    _socket = std::move(socket);

    // Reset adapter and rewire
    auto* adapter = static_cast<ConnectionAdapter*>(_adapter);
    adapter->reset(_socket.get(), &_request);
    _socket->addReceiver(_adapter);
    _adapter->addReceiver(this);
    touch();
}


void ServerConnection::beginStreaming()
{
    if (_closed || upgraded())
        return;
    setState(ServerConnectionState::Streaming);
}


void ServerConnection::endStreaming()
{
    endStreaming(ServerConnectionState::DispatchingOrSending);
}


void ServerConnection::endStreaming(ServerConnectionState nextState)
{
    if (_state != ServerConnectionState::Streaming || _closed || upgraded())
        return;
    setState(nextState);
}


ssize_t ServerConnection::sendHeader()
{
    if (_closed)
        return -1;

    if (!upgraded()) {
        if (_state == ServerConnectionState::ReceivingHeaders ||
            _state == ServerConnectionState::ReceivingBody) {
            setState(ServerConnectionState::DispatchingOrSending);
        }
        if (responseLooksStreaming())
            beginStreaming();
    }

    return Connection::sendHeader();
}


void ServerConnection::close()
{
    if (_closed)
        return;

    setState(ServerConnectionState::Closing);
    Connection::close();
}


void ServerConnection::setState(ServerConnectionState state)
{
    _state = state;
}


bool ServerConnection::requestHasBody() const
{
    return _request.isChunkedTransferEncoding() ||
           (_request.hasContentLength() && _request.getContentLength() > 0);
}


bool ServerConnection::responseLooksStreaming() const
{
    if (_response.isChunkedTransferEncoding())
        return true;

    std::string_view contentType(_response.getContentType());
    constexpr std::string_view sse = "text/event-stream";
    constexpr std::string_view multipart = "multipart/x-mixed-replace";
    if (contentType.size() >= sse.size() &&
        util::icompare(contentType.substr(0, sse.size()), sse) == 0) {
        return true;
    }
    if (contentType.size() >= multipart.size() &&
        util::icompare(contentType.substr(0, multipart.size()), multipart) == 0) {
        return true;
    }
    return false;
}


void ServerConnection::onHeaders()
{
    // Upgrade the connection if required
    bool isUpgrade = dynamic_cast<ConnectionAdapter*>(adapter())->parser().upgrade() &&
                     util::icompare(request().get("Upgrade", ""), "websocket") == 0;
    if (isUpgrade) {
        _mode = ServerConnectionMode::Upgraded;
        setState(ServerConnectionState::Upgraded);

        // Note: To upgrade the connection we need to replace the
        // underlying SocketAdapter instance. Since we are currently
        // inside the default ConnectionAdapter's HTTP parser callback
        // scope we just swap the SocketAdapter instance pointers and do
        // a deferred delete on the old adapter. No more callbacks will be
        // received from the old adapter after replaceAdapter is called.
        auto wsAdapter = std::make_unique<ws::ConnectionAdapter>(this, ws::ServerSide);
        auto* wsAdapterRaw = wsAdapter.get();
        replaceAdapter(std::move(wsAdapter));

        // Send the handshake request to the WS adapter for handling.
        // If the request fails the underlying socket will be closed
        // resulting in the destruction of the current connection.
        std::string buffer;
        buffer.reserve(256);
        request().write(buffer);
        request().clear();

        wsAdapterRaw->onSocketRecv(*socket().get(), mutableBuffer(buffer), socket()->peerAddress());
    } else {
        _mode = ServerConnectionMode::Http;
        setState(requestHasBody() ? ServerConnectionState::ReceivingBody
                                  : ServerConnectionState::DispatchingOrSending);
    }

    // Notify the server the connection is ready for data flow
    _server.onConnectionReady(*this);

    // Instantiate the responder now that request headers have been parsed
    _responder = _server.createResponder(*this);

    if (isUpgrade && !_closed) {
        // Fire the WebSocket handshake-complete signal now that the
        // Connection signal has been emitted and the responder created,
        // so application handlers are attached before the connect event.
        auto* wsAdapter = dynamic_cast<ws::ConnectionAdapter*>(adapter());
        if (wsAdapter)
            wsAdapter->onHandshakeComplete();
    } else if (_responder) {
        _responder->onHeaders(_request);
    }
}


void ServerConnection::onPayload(const MutableBuffer& buffer)
{
    // The connection may have been closed inside a previous callback.
    if (_closed) {
        return;
    }

    if (_state == ServerConnectionState::ReceivingHeaders ||
        _state == ServerConnectionState::DispatchingOrSending) {
        setState(ServerConnectionState::ReceivingBody);
    }

    // Send payload to the responder or signal
    if (_responder)
        _responder->onPayload(buffer);
    else
        Payload.emit(*this, buffer);
}


void ServerConnection::onComplete()
{
    // The connection may have been closed inside a previous callback.
    if (_closed) {
        return;
    }

    // The HTTP request is complete.
    markActive();
    if (!upgraded() && !streaming())
        setState(ServerConnectionState::DispatchingOrSending);

    if (_responder)
        _responder->onRequest(_request, _response);

    // For keep-alive connections, reset state for the next request.
    // The parser's on_message_begin callback clears Request/Response headers.
    if (_closed || upgraded() || streaming()) {
        return;
    }

    if (_request.getKeepAlive()) {
        _responder.reset();
        _response = Response();
        _shouldSendHeader = true;
        setState(ServerConnectionState::ReceivingHeaders);
    }
}


void ServerConnection::onClose()
{
    setState(ServerConnectionState::Closed);

    if (_responder)
        _responder->onClose();

    Close.emit(*this);
}


http::Message* ServerConnection::incomingHeader()
{
    return static_cast<http::Message*>(&_request);
}


http::Message* ServerConnection::outgoingHeader()
{
    return static_cast<http::Message*>(&_response);
}


} // namespace http
} // namespace icy


/// @}
