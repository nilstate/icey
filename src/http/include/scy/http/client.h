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

#include "scy/http/connection.h"
#include "scy/http/websocket.h"
#include "scy/net/dns.h"
#include "scy/net/socket.h"
#include "scy/net/sslsocket.h"
#include "scy/net/tcpsocket.h"
#include "scy/packetio.h"
#include "scy/timer.h"

#include <memory>


namespace scy {
namespace http {


class HTTP_API Client;

/// HTTP client connection for managing request/response lifecycle.
class HTTP_API ClientConnection : public Connection
{
public:
    using Ptr = std::shared_ptr<ClientConnection>;

    /// Create a standalone connection with the given host.
    ClientConnection(const URL& url, const net::TCPSocket::Ptr& socket = std::make_shared<net::TCPSocket>());

    virtual ~ClientConnection();

    /// Send the HTTP request.
    ///
    /// Calls connect() internally if the socket is not
    /// already connecting or connected. The actual request
    /// will be sent when the socket is connected.
    virtual void send();

    /// Send the given HTTP request.
    /// The given request will overwrite the internal HTTP
    /// request object.
    ///
    /// Calls connect() internally if the socket is not
    /// already connecting or connected. The actual request
    /// will be sent when the socket is connected.
    virtual void send(http::Request& req);

    /// Send raw data to the peer.
    /// Calls send() internally.
    ///
    /// Throws an exception if the socket is not already or connected.
    virtual ssize_t send(const char* data, size_t len, int flags = 0) override;

    /// Set the output stream for writing response data to.
    /// The stream pointer is managed internally,
    /// and will be freed along with the connection.
    virtual void setReadStream(std::ostream* os);

    /// Return the cast read stream pointer or nullptr.
    template <class StreamT>
    StreamT& readStream()
    {
        if (!_readStream)
            throw std::runtime_error("No stream reader associated with HTTP client.");

        return *dynamic_cast<StreamT*>(_readStream.get());
    }

    /// Optional unmanaged client data pointer.
    void* opaque;

    //
    /// Connection interface

    virtual void onHeaders();
    virtual void onPayload(const MutableBuffer& buffer);
    virtual void onComplete();
    virtual void onClose();

    //
    /// Status signals

    NullSignal Connect;                         ///< Signals when the client socket is connected and data can flow
    Signal<void(Response&)> Headers;            ///< Signals when the response HTTP header has been received
    Signal<void(const MutableBuffer&)> Payload; ///< Signals when raw data is received
    Signal<void(const Response&)> Complete;     ///< Signals when the HTTP transaction is complete
    Signal<void(Connection&)> Close;            ///< Signals when the connection is closed
    ProgressSignal IncomingProgress;            ///< Signals download progress (0-100%)

protected:
    /// Connects to the server endpoint.
    /// All sent data is buffered until the connection is made.
    virtual void connect();

    http::Message* incomingHeader();
    http::Message* outgoingHeader();

    bool onSocketConnect(net::Socket& socket);

protected:
    URL _url;
    bool _connect;
    bool _active;
    bool _complete;
    std::vector<std::string> _outgoingBuffer;
    std::unique_ptr<std::ostream> _readStream;
};


using ClientConnectionPtrVec = std::vector<ClientConnection::Ptr>;


//
// HTTP Connection Helpers
//


template <class ConnectionT>
inline ClientConnection::Ptr createConnectionT(const URL& url, uv::Loop* loop = uv::defaultLoop())
{
    ClientConnection::Ptr conn;

    if (url.scheme() == "http") {
        conn = std::make_shared<ConnectionT>(url, std::make_shared<net::TCPSocket>(loop));
    } else if (url.scheme() == "https") {
        conn = std::make_shared<ConnectionT>(url, std::make_shared<net::SSLSocket>(loop));
    } else if (url.scheme() == "ws") {
        conn = std::make_shared<ConnectionT>(url, std::make_shared<net::TCPSocket>(loop));
        conn->replaceAdapter(std::make_unique<ws::ConnectionAdapter>(conn.get(), ws::ClientSide));
    } else if (url.scheme() == "wss") {
        conn = std::make_shared<ConnectionT>(url, std::make_shared<net::SSLSocket>(loop));
        conn->replaceAdapter(std::make_unique<ws::ConnectionAdapter>(conn.get(), ws::ClientSide));
    } else
        throw std::runtime_error("Unknown connection type for URL: " + url.str());

    return conn;
}


//
// HTTP Client
//


/// HTTP client for creating and managing outgoing connections.
class HTTP_API Client
{
public:
    Client();
    virtual ~Client();

    /// Return the default HTTP Client singleton.
    static Client& instance();

    /// Destroys the default HTTP Client singleton.
    static void destroy();

    /// Shutdown the Client and close all connections.
    void shutdown();

    template <class ConnectionT>
    ClientConnection::Ptr createConnectionT(const URL& url, uv::Loop* loop = uv::defaultLoop())
    {
        auto connection = http::createConnectionT<ConnectionT>(url, loop);
        if (connection) {
            addConnection(connection);
        }
        return connection;
    }

    ClientConnection::Ptr createConnection(const URL& url, uv::Loop* loop = uv::defaultLoop())
    {
        auto connection = http::createConnectionT<ClientConnection>(url, loop);
        if (connection) {
            addConnection(connection);
        }
        return connection;
    }

    virtual void addConnection(ClientConnection::Ptr conn);
    virtual void removeConnection(ClientConnection* conn);

    NullSignal Shutdown;

protected:
    void onConnectionClose(Connection& conn);

    friend class ClientConnection;

    ClientConnectionPtrVec _connections;
};


inline ClientConnection::Ptr createConnection(const URL& url, http::Client* client = nullptr,
                                              uv::Loop* loop = uv::defaultLoop())
{
    auto connection = createConnectionT<ClientConnection>(url, loop);
    if (client && connection)
        client->addConnection(connection);

    return connection;
}


} // namespace http
} // namespace scy


/// @}
