///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#ifndef SCY_HTTP_Server_H
#define SCY_HTTP_Server_H


#include "scy/base.h"
#include "scy/datetime.h"
#include "scy/http/connection.h"
#include "scy/http/parser.h"
#include "scy/http/request.h"
#include "scy/http/response.h"
#include "scy/logger.h"
#include "scy/net/socket.h"
#include "scy/timer.h"
#include <ctime>


namespace scy {
namespace http {


class HTTP_API Server;
class HTTP_API ServerResponder;


/// HTTP server connection.
class HTTP_API ServerConnection : public Connection
{
public:
    typedef std::shared_ptr<ServerConnection> Ptr;

    ServerConnection(Server& server, net::TCPSocket::Ptr socket);
    virtual ~ServerConnection();

    Server& server();

    Signal<void(ServerConnection&, const MutableBuffer&)> Payload; ///< Signals when raw data is received
    Signal<void(ServerConnection&)> Close; ///< Signals when the connection is closed

protected:
    virtual void onHeaders() override;
    virtual void onPayload(const MutableBuffer& buffer) override;
    virtual void onComplete() override;
    virtual void onClose() override;

    http::Message* incomingHeader() override;
    http::Message* outgoingHeader() override;

protected:
    Server& _server;
    ServerResponder* _responder;
    bool _upgrade;
};


/// The abstract base class for HTTP ServerResponders
/// created by HTTP Server.
///
/// Derived classes should override the onRequest() method.
///
/// A new ServerResponder object can be created for
/// each new HTTP request that is received by the HTTP Server.
///
class HTTP_API ServerResponder
{
public:
    ServerResponder(ServerConnection& connection) :
        _connection(connection)
    {
    }

    virtual ~ServerResponder() = default;

    virtual void onHeaders(Request& /* request */) {}
    virtual void onPayload(const MutableBuffer& /* body */) {}
    virtual void onRequest(Request& /* request */, Response& /* response */) {}
    virtual void onClose() {};

    ServerConnection& connection()
    {
        return _connection;
    }

    Request& request()
    {
        return _connection.request();
    }

    Response& response()
    {
        return _connection.response();
    }

protected:
    ServerConnection& _connection;

private:
    ServerResponder(const ServerResponder&) = delete;
    ServerResponder& operator=(const ServerResponder&) = delete;
};


/// This implementation of a ServerConnectionFactory
/// is used by HTTP Server to create ServerConnection objects.
class HTTP_API ServerConnectionFactory
{
public:
    ServerConnectionFactory() = default;
    virtual ~ServerConnectionFactory() = default;

    /// Factory method for instantiating the ServerConnection
    /// instance using the given Socket.
    virtual ServerConnection::Ptr createConnection(Server& server, const net::TCPSocket::Ptr& socket)
    {
        return std::make_shared<ServerConnection>(server, socket);
    }

    /// Factory method for instantiating the ServerResponder
    /// instance using the given ServerConnection.
    virtual ServerResponder* createResponder(ServerConnection& connection)
    {
        return nullptr;
    }
};


/// HTTP server implementation.
///
/// This HTTP server is not strictly standards compliant.
/// It was created to be a fast (nocopy where possible)
/// solution for streaming media to web browsers.
class HTTP_API Server : public net::SocketAdapter
{
public:
    Server(const std::string& host, short port,
        net::TCPSocket::Ptr socket = net::makeSocket<net::TCPSocket>(),
        ServerConnectionFactory* factory = new ServerConnectionFactory());
    Server(const net::Address& address,
        net::TCPSocket::Ptr socket = net::makeSocket<net::TCPSocket>(),
        ServerConnectionFactory* factory = new ServerConnectionFactory());
    virtual ~Server();

    /// Start the HTTP server.
    void start();

    /// Shutdown the HTTP server.
    void shutdown();

    /// Return the server bind address.
    net::Address& address();

    /// Signals when a new connection has been created.
    /// A reference to the new connection object is provided.
    Signal<void(ServerConnection::Ptr)> Connection;

    /// Signals when the server is shutting down.
    NullSignal Shutdown;

protected:
    ServerResponder* createResponder(ServerConnection& conn);

    void onClientSocketAccept(const net::TCPSocket::Ptr& socket);
    void onConnectionReady(ServerConnection& conn);
    void onConnectionClose(ServerConnection& conn);
    void onSocketClose(net::Socket& socket);
    void onTimer();

protected:
    net::Address _address;
    net::TCPSocket::Ptr _socket;
    Timer _timer;
    ServerConnectionFactory* _factory;
    std::vector<ServerConnection::Ptr> _connections;

    friend class ServerConnection;
};


} // namespace http
} // namespace scy


#endif


/// @\}
