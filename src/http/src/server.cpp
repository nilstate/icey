///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/server.h"
#include "scy/http/websocket.h"
#include "scy/logger.h"
#include "scy/util.h"

#include <algorithm>
#include <memory>


namespace scy {
namespace http {


Server::Server(const std::string& host, short port, net::TCPSocket::Ptr socket, std::unique_ptr<ServerConnectionFactory> factory)
    : _address(host, port)
    , _socket(socket)
    , _timer(5000, 5000, socket->loop())
    , _factory(std::move(factory))
{
}


Server::Server(const net::Address& address, net::TCPSocket::Ptr socket, std::unique_ptr<ServerConnectionFactory> factory)
    : _address(address)
    , _socket(socket)
    , _timer(5000, 5000, socket->loop())
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
    _socket->listen(1000);

    LDebug("HTTP server listening on ", _address);

    _timer.Timeout += slot(this, &Server::onTimer);
    _timer.start();
}


void Server::shutdown()
{
    if (_socket) {
        _socket->removeReceiver(this);
        _socket->AcceptConnection -= slot(this, &Server::onClientSocketAccept);
        _socket->close();
    }

    _timer.stop();

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
    ServerConnection::Ptr conn = _factory->createConnection(*this, socket);
    conn->Close += slot(this, &Server::onConnectionClose);
    _connections.push_back(conn);
}


void Server::onConnectionReady(ServerConnection& conn)
{
    for (auto& c : _connections) {
        if (c.get() == &conn) {
            Connection.emit(c);
            return;
        }
    }
}


void Server::onConnectionClose(ServerConnection& conn)
{
    auto it = std::find_if(_connections.begin(), _connections.end(),
                           [&conn](const ServerConnection::Ptr& c) { return c.get() == &conn; });
    if (it != _connections.end())
        _connections.erase(it);
}


bool Server::onSocketClose(net::Socket& socket)
{
    return false;
}


void Server::onTimer()
{
    // Connection timeout cleanup not yet implemented
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
    if (_responder)
        _responder->onRequest(_request, _response);
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
} // namespace scy


/// @}
