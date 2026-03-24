///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "icy/base.h"
#include "icy/datetime.h"
#include "icy/http/connection.h"
#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/logger.h"
#include "icy/net/socket.h"
#include "icy/timer.h"
#include <ctime>
#include <cstring>
#include <memory>
#include <unordered_map>


namespace icy {
namespace http {


class HTTP_API Server;
class HTTP_API ServerResponder;


enum class ServerConnectionState : uint8_t
{
    ReceivingHeaders,
    ReceivingBody,
    DispatchingOrSending,
    Streaming,
    Upgraded,
    Closing,
    Closed,
};


enum class ServerConnectionMode : uint8_t
{
    Http,
    Upgraded,
};


/// HTTP server connection.
class HTTP_API ServerConnection : public Connection
{
public:
    using Ptr = std::shared_ptr<ServerConnection>;

    /// Creates a ServerConnection attached to the given server and socket.
    /// @param server The owning HTTP server instance.
    /// @param socket The accepted TCP socket for this connection.
    ServerConnection(Server& server, net::TCPSocket::Ptr socket);
    virtual ~ServerConnection();

    /// Returns the owning Server instance.
    [[nodiscard]] Server& server();

    /// Returns the current server-side connection state.
    [[nodiscard]] ServerConnectionState state() const { return _state; }

    /// Returns the current transport mode.
    [[nodiscard]] ServerConnectionMode mode() const { return _mode; }

    /// Returns true if the connection has been upgraded (e.g. to WebSocket).
    [[nodiscard]] bool upgraded() const { return _mode == ServerConnectionMode::Upgraded; }

    /// Returns true if the connection is in long-lived streaming mode.
    [[nodiscard]] bool streaming() const { return _state == ServerConnectionState::Streaming; }

    /// Returns true if the server idle timer is allowed to reap this connection.
    [[nodiscard]] bool idleTimeoutEnabled() const;

    /// Returns true if the closed connection can be returned to the reuse pool.
    [[nodiscard]] bool reusableForPool() const;

    /// Refresh the idle timer.
    void markActive() override { touch(); }

    /// Reset this connection for reuse with a new socket.
    /// Called by the connection pool to avoid allocating a new ServerConnection.
    void reset(net::TCPSocket::Ptr socket);

    /// Update the last activity timestamp.
    void touch() { _lastActivity = std::time(nullptr); }

    /// Return seconds since last activity.
    [[nodiscard]] double idleSeconds() const { return std::difftime(std::time(nullptr), _lastActivity); }

    /// Explicitly mark the response as long-lived streaming.
    /// Streaming connections are excluded from the keep-alive idle reaper.
    void beginStreaming() override;

    /// Exit streaming mode and return to the given HTTP state.
    void endStreaming() override;
    void endStreaming(ServerConnectionState nextState);

    /// Send the outgoing HTTP header.
    ssize_t sendHeader() override;

    /// Close the connection with an explicit terminal state transition.
    void close() override;

    LocalSignal<void(ServerConnection&, const MutableBuffer&)> Payload; ///< Signals when raw data is received
    LocalSignal<void(ServerConnection&)> Close;                         ///< Signals when the connection is closed

protected:
    virtual void onHeaders() override;
    virtual void onPayload(const MutableBuffer& buffer) override;
    virtual void onComplete() override;
    virtual void onClose() override;

    http::Message* incomingHeader() override;
    http::Message* outgoingHeader() override;

    void setState(ServerConnectionState state);
    [[nodiscard]] bool requestHasBody() const;
    [[nodiscard]] bool responseLooksStreaming() const;

protected:
    Server& _server;
    std::unique_ptr<ServerResponder> _responder;
    std::time_t _lastActivity{0};
    ServerConnectionState _state{ServerConnectionState::ReceivingHeaders};
    ServerConnectionMode _mode{ServerConnectionMode::Http};
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
    /// Creates a ServerResponder for the given connection.
    /// @param connection The server connection this responder handles.
    ServerResponder(ServerConnection& connection)
        : _connection(connection)
    {
    }

    virtual ~ServerResponder() = default;

    /// Called when the incoming request headers have been parsed.
    /// @param request The parsed HTTP request with headers populated.
    virtual void onHeaders(Request& request) { (void)request; }

    /// Called for each chunk of incoming request body data.
    /// @param body Buffer containing a chunk of the request body.
    virtual void onPayload(const MutableBuffer& body) { (void)body; }

    /// Called when the complete HTTP request has been received.
    /// Derived classes should write their response here.
    /// @param request The fully received HTTP request.
    /// @param response The HTTP response to populate and send.
    virtual void onRequest(Request& request, Response& response)
    {
        (void)request;
        (void)response;
    }

    /// Called when the connection is closed.
    virtual void onClose() {};

    /// Returns the underlying server connection.
    [[nodiscard]] ServerConnection& connection()
    {
        return _connection;
    }

    /// Returns the current HTTP request from the underlying connection.
    [[nodiscard]] Request& request()
    {
        return _connection.request();
    }

    /// Returns the current HTTP response from the underlying connection.
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

    /// Refreshes the cached Date header string if the current second has changed.
    /// No-op if called multiple times within the same second.
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

    /// Returns a pointer to the formatted "Date: ...\r\n" header string.
    [[nodiscard]] const char* data() const { return buf; }

    /// Returns the byte length of the formatted Date header string.
    [[nodiscard]] size_t size() const { return len; }
};


/// LIFO connection pool for reusing ServerConnection objects.
/// Avoids per-request heap allocation by resetting and reusing
/// connections instead of destroying and recreating them.
class ConnectionPool
{
public:
    /// Takes a connection from the pool for reuse.
    /// @return A pooled connection, or nullptr if the pool is empty.
    ServerConnection::Ptr acquire()
    {
        if (_pool.empty()) return nullptr;
        auto conn = std::move(_pool.back());
        _pool.pop_back();
        return conn;
    }

    /// Returns a connection to the pool after use.
    /// @param conn The connection to return.
    /// @return true if accepted into the pool; false if the pool is full.
    bool release(ServerConnection::Ptr& conn)
    {
        if (_pool.size() >= _maxSize) return false;
        _pool.push_back(std::move(conn));
        return true;
    }

    /// Sets the maximum number of connections the pool will hold.
    /// @param n Maximum pool capacity.
    void setMaxSize(size_t n) { _maxSize = n; }

    /// Returns the current number of connections held in the pool.
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
    /// Constructs an HTTP server on the given host and port using an internally created TCP socket.
    /// @param host Bind address (e.g. "0.0.0.0" or "127.0.0.1").
    /// @param port TCP port to listen on.
    /// @param loop Event loop to use. Defaults to the default libuv loop.
    /// @param factory Connection and responder factory. Defaults to the base factory.
    Server(const std::string& host, short port,
           uv::Loop* loop = uv::defaultLoop(),
           std::unique_ptr<ServerConnectionFactory> factory = std::make_unique<ServerConnectionFactory>());

    /// Constructs an HTTP server on the given address using an internally created TCP socket.
    /// @param address Bind address and port.
    /// @param loop Event loop to use. Defaults to the default libuv loop.
    /// @param factory Connection and responder factory.
    Server(const net::Address& address,
           uv::Loop* loop = uv::defaultLoop(),
           std::unique_ptr<ServerConnectionFactory> factory = std::make_unique<ServerConnectionFactory>());

    /// Constructs an HTTP server on the given host and port using a caller-supplied socket.
    /// Useful for HTTPS by passing an SSLSocket. The event loop is derived from the socket.
    /// @param host Bind address.
    /// @param port TCP port to listen on.
    /// @param socket Pre-created socket (e.g. SSLSocket for HTTPS).
    /// @param factory Connection and responder factory.
    Server(const std::string& host, short port,
           net::TCPSocket::Ptr socket,
           std::unique_ptr<ServerConnectionFactory> factory = std::make_unique<ServerConnectionFactory>());

    /// Constructs an HTTP server on the given address using a caller-supplied socket.
    /// The event loop is derived from the socket.
    /// @param address Bind address and port.
    /// @param socket Pre-created socket (e.g. SSLSocket for HTTPS).
    /// @param factory Connection and responder factory.
    Server(const net::Address& address,
           net::TCPSocket::Ptr socket,
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

    /// Return the number of active connections (all states).
    [[nodiscard]] size_t connectionCount() const { return _connections.size(); }

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

    /// Return the event loop this server runs on.
    [[nodiscard]] uv::Loop* loop() const { return _loop; }

protected:
    uv::Loop* _loop;
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
} // namespace icy


/// @}
