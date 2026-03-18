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

#include "scy/http/parser.h"
#include "scy/http/request.h"
#include "scy/http/response.h"
#include "scy/http/url.h"
#include "scy/net/socketadapter.h"
#include "scy/net/tcpsocket.h"
#include "scy/packetqueue.h"
#include "scy/timer.h"

#include <memory>
#include <stdexcept>


namespace scy {
namespace http {


class HTTP_API ConnectionStream;
class HTTP_API ConnectionAdapter;
/// @ingroup http
class HTTP_API Connection : public net::SocketAdapter
{
public:
    using Ptr = std::shared_ptr<Connection>;

    Connection(const net::TCPSocket::Ptr& socket = std::make_shared<net::TCPSocket>());
    virtual ~Connection() noexcept;

    virtual void onHeaders() = 0;
    virtual void onPayload(const MutableBuffer&) = 0;
    virtual void onComplete() = 0;
    virtual void onClose() = 0;

    /// Send raw data to the peer.
    virtual ssize_t send(const char* data, size_t len, int flags = 0) override;

    /// Send the outdoing HTTP header.
    virtual ssize_t sendHeader();

    /// Close the connection and schedule the object for
    /// deferred deletion.
    virtual void close();

    /// Return true if the connection is closed.
    [[nodiscard]] bool closed() const;

    /// Return the error object if any.
    [[nodiscard]] scy::Error error() const;

    /// Return true if headers should be automatically sent.
    [[nodiscard]] bool shouldSendHeader() const;

    /// Set true to prevent auto-sending HTTP headers.
    void shouldSendHeader(bool flag);

    /// Assign the new ConnectionAdapter and setup the chain.
    /// The flow is: Connection <-> ConnectionAdapter <-> Socket.
    /// Takes ownership of the adapter (deferred deletion via uv loop).
    virtual void replaceAdapter(std::unique_ptr<net::SocketAdapter> adapter);

    /// Overload for nullptr (used in destructor to clear adapter).
    virtual void replaceAdapter(std::nullptr_t);

    /// Return true if the connection uses TLS/SSL.
    [[nodiscard]] bool secure() const;

    /// Return the underlying socket pointer.
    [[nodiscard]] net::TCPSocket::Ptr& socket();

    /// Return the underlying adapter pointer.
    [[nodiscard]] net::SocketAdapter* adapter() const;

    /// The HTTP request headers.
    [[nodiscard]] Request& request();

    /// The HTTP response headers.
    [[nodiscard]] Response& response();

    virtual http::Message* incomingHeader() = 0;
    virtual http::Message* outgoingHeader() = 0;

protected:
    /// Set the internal error.
    /// Note: Setting the error does not `close()` the connection.
    virtual void setError(const scy::Error& err);

    /// net::SocketAdapter interface
    virtual bool onSocketConnect(net::Socket& socket) override;
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress) override;
    virtual bool onSocketError(net::Socket& socket, const scy::Error& error) override;
    virtual bool onSocketClose(net::Socket& socket) override;

protected:
    net::TCPSocket::Ptr _socket;
    net::SocketAdapter* _adapter;
    Request _request;
    Response _response;
    scy::Error _error;
    std::string _headerBuf;
    bool _closed;
    bool _shouldSendHeader;

    friend class ConnectionStream;
    friend class ConnectionAdapter;
};


//
// Connection Adapter
//


/// @ingroup http
/// Default HTTP socket adapter for reading and writing HTTP messages
class HTTP_API ConnectionAdapter : public ParserObserver
    , public net::SocketAdapter
{
public:
    ConnectionAdapter(Connection* connection, llhttp_type_t type);
    virtual ~ConnectionAdapter();

    virtual ssize_t send(const char* data, size_t len, int flags = 0);

    /// Remove the given receiver.
    ///
    /// By default this function does nothing unless the given receiver
    /// matches the current receiver.
    virtual void removeReceiver(SocketAdapter* adapter);

    [[nodiscard]] Parser& parser();
    [[nodiscard]] Connection* connection();

    /// Reset the adapter for reuse with a new socket and request.
    /// Used by the connection pool to avoid destroying/recreating adapters.
    void reset(net::SocketAdapter* sender, http::Request* request);

protected:
    /// SocketAdapter interface
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress);

    /// HTTP Parser interface
    virtual void onParserHeader(const std::string& name, const std::string& value);
    virtual void onParserHeadersEnd(bool upgrade);
    virtual void onParserChunk(const char* buf, size_t len);
    virtual void onParserError(const scy::Error& err);
    virtual void onParserEnd();

    Connection* _connection;
    Parser _parser;
};


//
// Progress Signal
//


/// @ingroup http
/// HTTP progress signal for upload and download progress notifications.
class HTTP_API ProgressSignal : public Signal<void(const double&)>
{
public:
    void* sender;
    uint64_t current;
    uint64_t total;

    ProgressSignal()
        : sender(nullptr)
        , current(0)
        , total(0)
    {
    }

    [[nodiscard]] double progress() const { return (current / (total * 1.0)) * 100; }

    void update(int nread)
    {
        if (current > total) {
            throw std::runtime_error("ProgressSignal: current exceeds total");
        }
        current += nread;
        emit(progress());
    }
};


//
// Connection Stream
//


/// @ingroup http
/// Packet stream wrapper for a HTTP connection.
class HTTP_API ConnectionStream : public net::SocketAdapter
{
public:
    ConnectionStream(Connection::Ptr connection);
    virtual ~ConnectionStream();

    /// Send data via the Outgoing stream.
    virtual ssize_t send(const char* data, size_t len, int flags = 0);

    /// Return a reference to the underlying connection.
    [[nodiscard]] Connection::Ptr connection();

    /// The Outgoing stream is responsible for packetizing
    /// raw application data into the agreed upon HTTP
    /// format and sending it to the peer.
    PacketStream Outgoing;

    /// The Incoming stream emits incoming HTTP packets
    /// for processing by the application.
    ///
    /// This is useful for example when writing incoming data to a file.
    PacketStream Incoming;

    ProgressSignal IncomingProgress; ///< Fired on download progress
    ProgressSignal OutgoingProgress; ///< Fired on upload progress

protected:
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress);

    Connection::Ptr _connection;
};


} // namespace http
} // namespace scy


/// @}
