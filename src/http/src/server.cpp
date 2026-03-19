///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
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
#include <memory>


namespace icy {
namespace http {


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
    shutdown();
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


void Server::shutdown()
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
    conn->touch();
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

        // Return to pool if reusable (not upgraded, no error)
        if (!conn.upgraded() && !conn.error().any()) {
            _pool.release(std::move(ptr));
        } else {
            // Defer destruction: this callback may be firing from
            // inside the connection's own method chain (e.g. onHeaders
            // -> createResponder -> rejected -> close -> onClose -> here).
            // Releasing the shared_ptr now would destroy the connection
            // while we're still on its call stack.
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
            if (conn->idleSeconds() > _keepAliveTimeout)
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
    , _upgrade(false)
{
    replaceAdapter(std::make_unique<ConnectionAdapter>(this, HTTP_REQUEST));
}


ServerConnection::~ServerConnection()
{
    close();
}


Server& ServerConnection::server()
{
    return _server;
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
    _upgrade = false;

    // Clear request/response (keeps string/vector capacity)
    _request.clear();
    _request.setMethod("");
    _request.setURI("");
    _request.setVersion(Message::HTTP_1_1);
    _response.clear();
    _response.setStatus(StatusCode::OK);
    _response.setReason(getStatusCodeReason(StatusCode::OK));
    _response.setVersion(Message::HTTP_1_1);

    // Clear header buffer (keeps capacity)
    _headerBuf.clear();

    // Swap socket
    _socket = std::move(socket);

    // Reset adapter and rewire
    auto* adapter = static_cast<ConnectionAdapter*>(_adapter);
    adapter->reset(_socket.get(), &_request);
    _socket->addReceiver(_adapter);
    _adapter->addReceiver(this);
}


void ServerConnection::onHeaders()
{
    // Upgrade the connection if required
    _upgrade = dynamic_cast<ConnectionAdapter*>(adapter())->parser().upgrade();
    if (_upgrade && util::icompare(request().get("Upgrade", ""), "websocket") == 0) {
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
    }

    // Notify the server the connection is ready for data flow
    _server.onConnectionReady(*this);

    // Instantiate the responder now that request headers have been parsed
    _responder = _server.createResponder(*this);

    // If an upgraded connection has no responder, the factory rejected it
    // (e.g. max connections). Close the connection - the shared_ptr drop
    // is deferred by onConnectionClose to avoid destroying this object
    // while we're on its call stack.
    if (!_responder && _upgrade) {
        close();
        return;
    }

    // Upgraded connections don't receive the onHeaders callback
    if (_responder && !_upgrade)
        _responder->onHeaders(_request);
}


void ServerConnection::onPayload(const MutableBuffer& buffer)
{
    // The connection may have been closed inside a previous callback.
    if (_closed) {
        return;
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
    // The request handler can give a response.
    touch();
    if (_responder)
        _responder->onRequest(_request, _response);

    // For keep-alive connections, reset state for the next request.
    // The parser's on_message_begin callback clears Request/Response headers.
    if (!_closed && !_upgrade && _request.getKeepAlive()) {
        _responder.reset();
        _shouldSendHeader = true;
        _headerBuf.clear();
    }
}


void ServerConnection::onClose()
{
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
