///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

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
#include <cstring>
#include <memory>
#include <unordered_map>


namespace scy {
namespace http {


class HTTP_API Server;
class HTTP_API ServerResponder;


/// HTTP server connection.
class HTTP_API ServerConnection : public Connection
{
public:
    using Ptr = std::shared_ptr<ServerConnection>;

    ServerConnection(Server& server, net::TCPSocket::Ptr socket);
    virtual ~ServerConnection();

    [[nodiscard]] Server& server();
    [[nodiscard]] bool upgraded() const { return _upgrade; }

    /// Reset this connection for reuse with a new socket.
    /// Called by the connection pool to avoid allocating a new ServerConnection.
    void reset(net::TCPSocket::Ptr socket);

    /// Update the last activity timestamp (called on request completion).
    void touch() { _lastActivity = std::time(nullptr); }

    /// Return seconds since last activity.
    [[nodiscard]] double idleSeconds() const { return std::difftime(std::time(nullptr), _lastActivity); }

    LocalSignal<void(ServerConnection&, const MutableBuffer&)> Payload; ///< Signals when raw data is received
    LocalSignal<void(ServerConnection&)> Close;                         ///< Signals when the connection is closed

protected:
    virtual void onHeaders() override;
    virtual void onPayload(const MutableBuffer& buffer) override;
    virtual void onComplete() override;
    virtual void onClose() override;

    http::Message* incomingHeader() override;
    http::Message* outgoingHeader() override;

protected:
    Server& _server;
    std::unique_ptr<ServerResponder> _responder;
    std::time_t _lastActivity{0};
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
    ServerResponder(ServerConnection& connection)
        : _connection(connection)
    {
    }

    virtual ~ServerResponder() = default;

    virtual void onHeaders(Request& /* request */) {}
    virtual void onPayload(const MutableBuffer& /* body */) {}
    virtual void onRequest(Request& /* request */, Response& /* response */) {}
    virtual void onClose() {};

    [[nodiscard]] ServerConnection& connection()
    {
        return _connection;
    }

    [[nodiscard]] Request& request()
    {
        return _connection.request();
    }

    [[nodiscard]] Response& response()
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
    virtual std::unique_ptr<ServerResponder> createResponder(ServerConnection& connection)
    {
        return nullptr;
    }
};


/// Caches the formatted Date header, updated once per second.
/// Avoids per-request time formatting and string allocation.
struct DateCache
{
    char buf[64]{};
    size_t len = 0;
    std::time_t lastSecond = 0;

    void update()
    {
        auto now = std::time(nullptr);
        if (now != lastSecond) {
            lastSecond = now;
            std::tm tm;
#ifdef _WIN32
            gmtime_s(&tm, &now);
#else
            gmtime_r(&now, &tm);
#endif
            len = std::strftime(buf, sizeof(buf),
                "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", &tm);
        }
    }

    [[nodiscard]] const char* data() const { return buf; }
    [[nodiscard]] size_t size() const { return len; }
};


/// LIFO connection pool for reusing ServerConnection objects.
/// Avoids per-request heap allocation by resetting and reusing
/// connections instead of destroying and recreating them.
class ConnectionPool
{
public:
    ServerConnection::Ptr acquire()
    {
        if (_pool.empty()) return nullptr;
        auto conn = std::move(_pool.back());
        _pool.pop_back();
        return conn;
    }

    bool release(ServerConnection::Ptr conn)
    {
        if (_pool.size() >= _maxSize) return false;
        _pool.push_back(std::move(conn));
        return true;
    }

    void setMaxSize(size_t n) { _maxSize = n; }
    [[nodiscard]] size_t size() const { return _pool.size(); }

private:
    std::vector<ServerConnection::Ptr> _pool;
    size_t _maxSize = 128;
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
           std::unique_ptr<ServerConnectionFactory> factory = std::make_unique<ServerConnectionFactory>());
    Server(const net::Address& address,
           net::TCPSocket::Ptr socket = net::makeSocket<net::TCPSocket>(),
           std::unique_ptr<ServerConnectionFactory> factory = std::make_unique<ServerConnectionFactory>());
    virtual ~Server();

    /// Start the HTTP server.
    void start();

    /// Shutdown the HTTP server.
    void shutdown();

    /// Enable SO_REUSEPORT for multicore server instances.
    /// Must be called before start(). Allows multiple server
    /// instances to bind the same address:port with kernel-level
    /// load balancing (Linux 3.9+).
    void setReusePort(bool enable = true) { _reusePort = enable; }

    /// Set the maximum number of pooled connections (default 128).
    /// Set to 0 to disable connection pooling entirely.
    void setMaxPooledConnections(size_t n) { _pool.setMaxSize(n); }

    /// Set the keep-alive idle timeout in seconds (default 30).
    /// Connections idle longer than this are closed by the timer.
    /// Set to 0 to disable idle timeout.
    void setKeepAliveTimeout(int seconds) { _keepAliveTimeout = seconds; }

    /// Return the server bind address.
    [[nodiscard]] net::Address& address();

    /// Return the cached Date header for use in responses.
    [[nodiscard]] const DateCache& dateCache() const { return _dateCache; }

    /// Signals when a new connection has been created.
    /// A reference to the new connection object is provided.
    LocalSignal<void(ServerConnection::Ptr)> Connection;

    /// Signals when the server is shutting down.
    LocalSignal<void()> Shutdown;

protected:
    std::unique_ptr<ServerResponder> createResponder(ServerConnection& conn);

    void onClientSocketAccept(const net::TCPSocket::Ptr& socket);
    void onConnectionReady(ServerConnection& conn);
    void onConnectionClose(ServerConnection& conn);
    bool onSocketClose(net::Socket& socket);
    void onTimer();

protected:
    net::Address _address;
    net::TCPSocket::Ptr _socket;
    Timer _timer;
    std::unique_ptr<ServerConnectionFactory> _factory;
    std::unordered_map<ServerConnection*, ServerConnection::Ptr> _connections;
    ConnectionPool _pool;
    DateCache _dateCache;
    int _keepAliveTimeout{30};
    bool _reusePort{false};

    friend class ServerConnection;
};


} // namespace http
} // namespace scy


/// @}
