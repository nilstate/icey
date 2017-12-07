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


using std::endl;


namespace scy {
namespace http {


Server::Server(const std::string& host, short port, net::TCPSocket::Ptr socket, ServerConnectionFactory* factory)
    : _address(host, port)
    , _socket(socket)
    , _timer(5000, 5000, socket->loop())
    , _factory(factory)
{
    // LTrace("Create")
}


Server::Server(const net::Address& address, net::TCPSocket::Ptr socket, ServerConnectionFactory* factory)
    : _address(address)
    , _socket(socket)
    , _timer(5000, 5000, socket->loop())
    , _factory(factory)
{
    // LTrace("Create")
}


Server::~Server()
{
    // LTrace("Destroy")
    shutdown();
    if (_factory)
        delete _factory;
}


void Server::start()
{
    _socket->addReceiver(this);
    _socket->AcceptConnection += slot(this, &Server::onClientSocketAccept);
    _socket->bind(_address);
    _socket->listen(1000);

    LDebug("HTTP server listening on ", _address)

    _timer.Timeout += slot(this, &Server::onTimer);
    _timer.start();
}


void Server::shutdown()
{
    // LTrace("Shutdown")

    if (_socket) {
        _socket->removeReceiver(this);
        _socket->AcceptConnection -= slot(this, &Server::onClientSocketAccept);
        _socket->close();
    }

    _timer.stop();

    Shutdown.emit();
}


ServerResponder* Server::createResponder(ServerConnection& conn)
{
    // The initial HTTP request headers have already
    // been parsed at this point, but the request body may
    // be incomplete (especially if chunked).
    return _factory->createResponder(conn);
}


void Server::onClientSocketAccept(const net::TCPSocket::Ptr& socket)
{
    // LTrace("On accept socket connection")

    ServerConnection::Ptr conn = _factory->createConnection(*this, socket);
    conn->Close += slot(this, &Server::onConnectionClose);
    _connections.push_back(conn);
}


void Server::onConnectionReady(ServerConnection& conn)
{
    // LTrace("On connection ready")

    for (auto it = _connections.begin(); it != _connections.end(); ++it) {
        if (it->get() == &conn) {
            Connection.emit(*it);
            return;
        }
    }
}


void Server::onConnectionClose(ServerConnection& conn)
{
    // LTrace("On connection closed")

    for (auto it = _connections.begin(); it != _connections.end(); ++it) {
        if (it->get() == &conn) {
            _connections.erase(it);
            return;
        }
    }
}


void Server::onSocketClose(net::Socket& socket)
{
    // LTrace("On server socket close")
}


void Server::onTimer()
{
    // LDebug("Num active HTTP server connections: ", connections.size())

    // TODO: cleanup timed out pending connections
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
    , _responder(nullptr)
    , _upgrade(false)
{
    // LTrace("Create")

    replaceAdapter(new ConnectionAdapter(this, HTTP_REQUEST));
}


ServerConnection::~ServerConnection()
{
    // LTrace("Destroy")

    close();

    if (_responder) {
        delete _responder;
    }
}


Server& ServerConnection::server()
{
    return _server;
}


void ServerConnection::onHeaders()
{
    // LTrace("On headers")

#if 0
    // Send a raw HTTP response
    std::ostringstream res;
    res << "HTTP/1.1 200 OK\r\n"
        << "Connection: close\r\n"
        << "Content-Length: 0" << "\r\n"
        << "\r\n";
    std::string response(res.str());
    send.send(response.c_str(), response.size());

    // Send a test HTTP response
    _response.add("Content-Length", "0");
    _response.add("Connection", "close"); // "keep-alive"
    sendHeader();
    return;
#endif

    // Upgrade the connection if required
    _upgrade = dynamic_cast<ConnectionAdapter*>(adapter())->parser().upgrade();
    if (_upgrade && util::icompare(request().get("Upgrade", ""), "websocket") == 0) {
    // if (util::icompare(request().get("Connection", ""), "upgrade") == 0 &&
    //     util::icompare(request().get("Upgrade", ""), "websocket") == 0) {
        // LTrace("Upgrading to WebSocket: ", request())

        // Note: To upgrade the connection we need to replace the
        // underlying SocketAdapter instance. Since we are currently
        // inside the default ConnectionAdapter's HTTP parser callback
        // scope we just swap the SocketAdapter instance pointers and do
        // a deferred delete on the old adapter. No more callbacks will be
        // received from the old adapter after replaceAdapter is called.
        auto wsAdapter = new ws::ConnectionAdapter(this, ws::ServerSide);
        replaceAdapter(wsAdapter);

        // Send the handshake request to the WS adapter for handling.
        // If the request fails the underlying socket will be closed
        // resulting in the destruction of the current connection.

        // std::ostringstream oss;
        // request().write(oss);
        // request().clear();
        // std::string buffer(oss.str());

        std::string buffer;
        buffer.reserve(256);
        request().write(buffer);
        request().clear();

        wsAdapter->onSocketRecv(*socket().get(), mutableBuffer(buffer), socket()->peerAddress());
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
    // LTrace("On payload: ", buffer.size())

    // The connection may have been closed inside a previous callback.
    if (_closed) {
        // LTrace("On payload: Closed")
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
    // LTrace("On complete")

    // The connection may have been closed inside a previous callback.
    if (_closed) {
        // LTrace("On complete: Closed")
        return;
    }

    // The HTTP request is complete.
    // The request handler can give a response.
    if (_responder)
        _responder->onRequest(_request, _response);
}


void ServerConnection::onClose()
{
    // LTrace("On close")

    if (_responder)
        _responder->onClose();

    Close.emit(*this);
}


http::Message* ServerConnection::incomingHeader()
{
    return reinterpret_cast<http::Message*>(&_request);
}


http::Message* ServerConnection::outgoingHeader()
{
    return reinterpret_cast<http::Message*>(&_response);
}


} // namespace http
} // namespace scy


/// @\}
