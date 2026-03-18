///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/connection.h"
#include "scy/http/client.h"
#include "scy/http/server.h"
#include "scy/logger.h"
#include "scy/memory.h"

#include <stdexcept>


namespace scy {
namespace http {


Connection::Connection(const net::TCPSocket::Ptr& socket)
    : _socket(socket)
    , _adapter(nullptr)
    , _closed(false)
    , _shouldSendHeader(true)
{
}


Connection::~Connection() noexcept
{
    replaceAdapter(nullptr);
}


ssize_t Connection::send(const char* data, size_t len, int flags)
{
    if (_closed)
        return -1;

    return _adapter->send(data, len, flags);
}


ssize_t Connection::sendHeader()
{
    if (!_shouldSendHeader)
        return 0;
    _shouldSendHeader = false;
    if (!outgoingHeader()) {
        throw std::runtime_error("Connection::sendHeader: no outgoing header");
    }

    std::string head;
    head.reserve(512);
    outgoingHeader()->write(head);

    // Send headers directly to the Socket,
    // bypassing the ConnectionAdapter
    return _socket->send(head.c_str(), head.length());
}


void Connection::close()
{
    if (_closed)
        return;
    _closed = true;

    if (_socket)
        _socket->close();

    onClose();
}


void Connection::replaceAdapter(std::unique_ptr<net::SocketAdapter> adapter)
{
    // Detach the old adapter from all callbacks
    if (_adapter) {
        _socket->removeReceiver(_adapter);
        _adapter->removeReceiver(this);
        _adapter->setSender(nullptr);

        deleteLater(_adapter, _socket->loop());
        _adapter = nullptr;
    }

    // Setup the data flow: Connection <-> ConnectionAdapter <-> Socket
    if (adapter) {
        _adapter = adapter.release(); // ownership transfers to deleteLater on next replace
        _adapter->addReceiver(this);
        _adapter->setSender(_socket.get());

        // Attach the ConnectionAdapter to receive Socket callbacks.
        // The given adapter will process raw packets into HTTP or
        // WebSocket frames depending on the adapter type.
        _socket->addReceiver(_adapter);
    }
}


void Connection::replaceAdapter(std::nullptr_t)
{
    replaceAdapter(std::unique_ptr<net::SocketAdapter>(nullptr));
}


void Connection::setError(const scy::Error& err)
{
    _error = err;
}


bool Connection::onSocketConnect(net::Socket& socket)
{
    // Only useful for client connections
    return false;
}


bool Connection::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    // Handle payload data
    onPayload(buffer);
    return false;
}


bool Connection::onSocketError(net::Socket& socket, const scy::Error& error)
{
    if (error.err == UV_EOF) {
        // Close the connection when the other side does
    } else {
        // Other errors will set the error state
        setError(error);
    }
    return false;
}


bool Connection::onSocketClose(net::Socket& socket)
{
    // Close the connection when the socket closes
    close();
    return false;
}


Request& Connection::request()
{
    return _request;
}


Response& Connection::response()
{
    return _response;
}


net::TCPSocket::Ptr& Connection::socket()
{
    return _socket;
}


net::SocketAdapter* Connection::adapter() const
{
    return _adapter;
}


bool Connection::closed() const
{
    return _closed;
}


scy::Error Connection::error() const
{
    return _error;
}


bool Connection::shouldSendHeader() const
{
    return _shouldSendHeader;
}


void Connection::shouldSendHeader(bool flag)
{
    _shouldSendHeader = flag;
}


bool Connection::secure() const
{
    return _socket && _socket->transport() == net::SSLTCP;
}


//
// HTTP Client Connection Adapter
//


ConnectionAdapter::ConnectionAdapter(Connection* connection, llhttp_type_t type)
    : SocketAdapter(connection->socket().get())
    , _connection(connection)
    , _parser(type)
{
    // Set the connection as the default receiver
    SocketAdapter::addReceiver(connection);

    // Setup the HTTP parser
    _parser.setObserver(this);
    if (type == HTTP_REQUEST)
        _parser.setRequest(&connection->request());
    else
        _parser.setResponse(&connection->response());
}


ConnectionAdapter::~ConnectionAdapter()
{
}


ssize_t ConnectionAdapter::send(const char* data, size_t len, int flags)
{
    // Send headers on initial send
    if (_connection &&
        _connection->shouldSendHeader()) {
        ssize_t res = _connection->sendHeader();

        // The initial packet may be empty to push the headers through
        if (len == 0)
            return res;
    }

    // Other packets should not be empty
    if (len == 0) {
        throw std::runtime_error("ConnectionAdapter::send: empty payload after headers");
    }

    // Send body / chunk
    return SocketAdapter::send(data, len, flags);
}


void ConnectionAdapter::removeReceiver(SocketAdapter* adapter)
{
    if (_connection == adapter)
        _connection = nullptr;

    net::SocketAdapter::removeReceiver(adapter);
}


bool ConnectionAdapter::onSocketRecv(net::Socket& socket, const MutableBuffer& buf, const net::Address&)
{
    if (_parser.complete()) {
        // Buggy HTTP servers might send late data or multiple responses,
        // in which case the parser state might already be HPE_OK.
        // In this case we discard the late message and log the error here,
        // rather than complicate the app with this error handling logic.
        LWarn("Dropping late HTTP response: ", buf.str());
        return false;
    }

    // Parse incoming HTTP messages
    _parser.parse(bufferCast<const char*>(buf), buf.size());
    return false;
}


//
// Parser callbacks

void ConnectionAdapter::onParserHeader(const std::string& /* name */,
                                       const std::string& /* value */)
{
}


void ConnectionAdapter::onParserHeadersEnd(bool upgrade)
{
    if (_connection)
        _connection->onHeaders();
}


void ConnectionAdapter::onParserChunk(const char* buf, size_t len)
{
    // Dispatch the payload
    if (_connection) {
        net::SocketAdapter::onSocketRecv(*_connection->socket().get(),
                                         mutableBuffer(const_cast<char*>(buf), len),
                                         _connection->socket()->peerAddress());
    }
}


void ConnectionAdapter::onParserError(const scy::Error& err)
{
    LWarn("On parser error: ", err.message);

    // Set error and close the connection on parser error
    if (_connection) {
        _connection->setError(err.message);
        _connection->close();
    }
}


void ConnectionAdapter::onParserEnd()
{
    if (_connection)
        _connection->onComplete();
}


Parser& ConnectionAdapter::parser()
{
    return _parser;
}


Connection* ConnectionAdapter::connection()
{
    return _connection;
}


//
// HTTP Connection Stream
//


ConnectionStream::ConnectionStream(Connection::Ptr connection)
    : _connection(connection)
{
    IncomingProgress.sender = this;
    OutgoingProgress.sender = this;

    Incoming.autoStart(true);
    Outgoing.autoStart(true);

    // The Outgoing stream pumps data into the ConnectionAdapter,
    // which in turn proxies to the output Socket.
    Outgoing.emitter += slot(_connection->adapter(),
                             static_cast<void (net::SocketAdapter::*)(IPacket&)>(&net::SocketAdapter::sendPacket));

    // The Incoming stream receives data from the ConnectionAdapter.
    _connection->adapter()->addReceiver(this);
}


ConnectionStream::~ConnectionStream()
{
    Outgoing.close();
    Incoming.close();
}


ssize_t ConnectionStream::send(const char* data, size_t len, int flags)
{
    // Send outgoing data to the stream if adapters are attached,
    // or just proxy to the connection.
    if (Outgoing.numAdapters() > 0) {
        Outgoing.write(data, len);
    } else {
        return _connection->send(data, len, flags);
    }
    return len;
}


bool ConnectionStream::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    // Push received data onto the Incoming stream.
    Incoming.write(bufferCast<const char*>(buffer), buffer.size());
    return false;
}


Connection::Ptr ConnectionStream::connection()
{
    return _connection;
}


} // namespace http
} // namespace scy


/// @}
