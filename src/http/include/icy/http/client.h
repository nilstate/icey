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

#include "icy/http/connection.h"
#include "icy/http/websocket.h"
#include "icy/net/dns.h"
#include "icy/net/socket.h"
#include "icy/net/sslsocket.h"
#include "icy/net/tcpsocket.h"
#include "icy/packetio.h"
#include "icy/timer.h"

#include <memory>


namespace icy {
namespace http {


class HTTP_API Client;

/// HTTP client connection for managing request/response lifecycle.
class HTTP_API ClientConnection : public Connection
{
public:
    using Ptr = std::shared_ptr<ClientConnection>;

    /// Creates a ClientConnection to the given URL, pre-populating the request URI and Host header.
    /// The response status is initialised to 502 Bad Gateway until a real response is received.
    /// @param url Target URL. Scheme, host, port and path are extracted automatically.
    /// @param socket TCP socket to use. Defaults to a plain TCPSocket; pass an SSLSocket for HTTPS.
    ClientConnection(const URL& url, const net::TCPSocket::Ptr& socket = std::make_shared<net::TCPSocket>());

    virtual ~ClientConnection();

    /// Submits the internal HTTP request.
    ///
    /// Calls connect() internally if the socket is not already connecting
    /// or connected. The actual request will be sent when the socket is connected.
    /// @throws std::runtime_error if already connecting.
    virtual void submit();

    /// Submits the given HTTP request, replacing the internal request object.
    ///
    /// Calls connect() internally if the socket is not already connecting
    /// or connected. The actual request will be sent when the socket is connected.
    /// @param req The HTTP request to send. Replaces the internal request.
    /// @throws std::runtime_error if already connecting.
    virtual void submit(http::Request& req);

    /// Sends raw data to the peer, initiating a connection first if needed.
    /// Data is buffered internally until the connection is established.
    /// @param data Pointer to the data buffer.
    /// @param len Number of bytes to send.
    /// @param flags Socket send flags (unused for HTTP).
    /// @return Number of bytes sent or buffered.
    virtual ssize_t send(const char* data, size_t len, int flags = 0) override;

    /// Sets the output stream to which incoming response body data is written.
    /// The stream pointer is owned by the connection and freed with it.
    /// Must be called before submit().
    /// @param os Pointer to the output stream. Takes ownership.
    /// @throws std::runtime_error if already connecting.
    virtual void setReadStream(std::ostream* os);

    /// Returns a reference to the read stream cast to the specified type.
    /// @tparam StreamT The concrete stream type to cast to.
    /// @return Reference to the stream.
    /// @throws std::runtime_error if no read stream has been set.
    /// @throws std::bad_cast if the stream is not of type StreamT.
    template <class StreamT>
    StreamT& readStream()
    {
        if (!_readStream)
            throw std::runtime_error("No stream reader associated with HTTP client.");

        return *dynamic_cast<StreamT*>(_readStream.get());
    }

    /// Optional unmanaged client data pointer. Not used by the connection internally.
    void* opaque;

    //
    /// Connection interface

    /// @private Called when the response headers have been parsed.
    void onHeaders() override;
    /// @private Called for each chunk of incoming response body data.
    void onPayload(const MutableBuffer& buffer) override;
    /// @private Called when the full HTTP response has been received.
    void onComplete() override;
    /// @private Called when the connection is closed.
    void onClose() override;

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
    struct PendingWrite
    {
        Buffer data;
        int flags = 0;
    };

    URL _url;
    bool _connect;
    bool _active;
    bool _complete;
    std::vector<PendingWrite> _outgoingBuffer;
    std::unique_ptr<std::ostream> _readStream;
};


using ClientConnectionPtrVec = std::vector<ClientConnection::Ptr>;


//
// HTTP Connection Helpers
//


/// Creates a ClientConnection (or subtype) for the given URL without registering
/// it with a Client instance. The socket and adapter are chosen based on the URL scheme:
/// - "http"  -> TCPSocket
/// - "https" -> SSLSocket
/// - "ws"    -> TCPSocket + WebSocket adapter
/// - "wss"   -> SSLSocket + WebSocket adapter
///
/// @tparam ConnectionT Concrete connection type derived from ClientConnection.
/// @param url Target URL. Must have a recognised scheme.
/// @param loop Event loop to use. Defaults to the default libuv loop.
/// @return Shared pointer to the created connection.
/// @throws std::runtime_error if the URL scheme is not recognised.
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

    /// Creates and registers a typed client connection for the given URL.
    /// The connection type is inferred from the URL scheme (http, https, ws, wss).
    /// @tparam ConnectionT Concrete connection type derived from ClientConnection.
    /// @param url Target URL. The scheme determines the socket and adapter type.
    /// @param loop Event loop to use. Defaults to the default libuv loop.
    /// @return Shared pointer to the created connection.
    template <class ConnectionT>
    ClientConnection::Ptr createConnectionT(const URL& url, uv::Loop* loop = uv::defaultLoop())
    {
        auto connection = http::createConnectionT<ConnectionT>(url, loop);
        if (connection) {
            addConnection(connection);
        }
        return connection;
    }

    /// Creates and registers a ClientConnection for the given URL.
    /// The socket type is chosen based on the URL scheme (http/https/ws/wss).
    /// @param url Target URL.
    /// @param loop Event loop to use. Defaults to the default libuv loop.
    /// @return Shared pointer to the created connection.
    ClientConnection::Ptr createConnection(const URL& url, uv::Loop* loop = uv::defaultLoop())
    {
        auto connection = http::createConnectionT<ClientConnection>(url, loop);
        if (connection) {
            addConnection(connection);
        }
        return connection;
    }

    /// Registers a connection with this client so it is tracked and cleaned up on shutdown.
    /// @param conn The connection to add.
    virtual void addConnection(ClientConnection::Ptr conn);

    /// Removes a previously registered connection from the client.
    /// @param conn Raw pointer to the connection to remove.
    /// @throws std::logic_error if the connection is not tracked by this client.
    virtual void removeConnection(ClientConnection* conn);

    NullSignal Shutdown;

protected:
    void onConnectionClose(Connection& conn);

    friend class ClientConnection;

    ClientConnectionPtrVec _connections;
};


/// Creates a ClientConnection for the given URL and optionally registers it with a Client.
/// Equivalent to calling Client::createConnection() when `client` is non-null.
/// @param url Target URL. The scheme determines the socket and adapter type.
/// @param client Optional Client instance to register the connection with.
/// @param loop Event loop to use. Defaults to the default libuv loop.
/// @return Shared pointer to the created connection.
inline ClientConnection::Ptr createConnection(const URL& url, http::Client* client = nullptr,
                                              uv::Loop* loop = uv::defaultLoop())
{
    auto connection = createConnectionT<ClientConnection>(url, loop);
    if (client && connection)
        client->addConnection(connection);

    return connection;
}


} // namespace http
} // namespace icy


/// @}
