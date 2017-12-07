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

#include <assert.h>


using std::endl;


namespace scy {
namespace http {


Connection::Connection(const net::TCPSocket::Ptr& socket)
    : _socket(socket)
    , _adapter(nullptr)
    //, _timeout(30 * 60 * 1000),
    , _closed(false)
    , _shouldSendHeader(true)
{
    // LTrace("Create: ", _socket)
}


Connection::~Connection()
{
    // LTrace("Destroy")
    replaceAdapter(nullptr);

    // NOTE: Call close from impl to avoid pure virtual
    // close();
}


ssize_t Connection::send(const char* data, size_t len, int flags)
{
    // LTrace("Send: ", len)
    assert(!_closed);
    if (_closed)
        return -1;

    return _adapter->send(data, len, flags);
}


ssize_t Connection::sendHeader()
{
    if (!_shouldSendHeader)
        return 0;
    _shouldSendHeader = false;
    assert(outgoingHeader());

    // std::ostringstream os;
    // outgoingHeader()->write(os);
    // std::string head(os.str().c_str(), os.str().length());

    std::string head;
    head.reserve(256);
    outgoingHeader()->write(head);

    // Send headers directly to the Socket,
    // bypassing the ConnectionAdapter
    return _socket->send(head.c_str(), head.length());
}


void Connection::close()
{
    // LTrace("Close: ", _closed)
    if (_closed)
        return;
    _closed = true;

    if (_socket)
        _socket->close();

    onClose();
}


void Connection::replaceAdapter(net::SocketAdapter* adapter)
{
    // LTrace("Replace adapter: ", adapter)

    // Detach the old adapter form all callbacks
    if (_adapter) {
        _socket->removeReceiver(_adapter);
        _adapter->removeReceiver(this);
        _adapter->setSender(nullptr);

        // LTrace("Replace adapter: Delete existing: ", _adapter)
        deleteLater<net::SocketAdapter>(_adapter, _socket->loop());
        _adapter = nullptr;
    }

    // Setup the data flow: Connection <-> ConnectionAdapter <-> Socket
    if (adapter) {
        adapter->addReceiver(this);
        adapter->setSender(_socket.get());

        // Attach the ConnectionAdapter to receive Socket callbacks.
        // The given adapter will process raw packets into HTTP or
        // WebSocket frames depending on the adapter type.
        _socket->addReceiver(adapter);
        _adapter = adapter;
    }
}


void Connection::setError(const scy::Error& err)
{
    // LTrace("Set error: ", err.message)

    _error = err;
}


void Connection::onSocketConnect(net::Socket& socket)
{
    // LTrace("On socket connect")

    // Only useful for client connections
}


void Connection::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    // LTrace("On socket recv")

    // Handle payload data
    onPayload(buffer);
}


void Connection::onSocketError(net::Socket& socket, const scy::Error& error)
{
    // LDebug("On socket error: " << error.err << ": ", error.message)

    if (error.err == UV_EOF) {
        // Close the connection when the other side does
        // close();
    } else {
        // Other errors will set the error state
        setError(error);
    }
}


void Connection::onSocketClose(net::Socket& socket)
{
    // LTrace("On socket close")

    // Close the connection when the socket closes
    close();
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
    return _socket
        && _socket->transport() == net::SSLTCP;
}


//
// HTTP Client Connection Adapter
//


ConnectionAdapter::ConnectionAdapter(Connection* connection, http_parser_type type)
    : SocketAdapter(connection->socket().get())
    , _connection(connection)
    , _parser(type)
{
    // LTrace("Create: ", connection)

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
    // LTrace("Destroy: ", _connection)
}


ssize_t ConnectionAdapter::send(const char* data, size_t len, int flags)
{
    // LTrace("Send: ", len)

    // try {
        // Send headers on initial send
        if (_connection &&
            _connection->shouldSendHeader()) {
            ssize_t res = _connection->sendHeader();

            // The initial packet may be empty to push the headers through
            if (len == 0)
                return res;
        }

        // Other packets should not be empty
        assert(len > 0);

        // Send body / chunk
        return SocketAdapter::send(data, len, flags);
    // }
    // catch (std::exception& exc) {
    //     LError("Send error: ", exc.what())
    //
    //     // Swallow the exception, the socket error will
    //     // cause the connection to close on next iteration.
    //     return -1;
    // }
}


void ConnectionAdapter::removeReceiver(SocketAdapter* adapter)
{
    if (_connection == adapter)
        _connection = nullptr;

    net::SocketAdapter::removeReceiver(adapter);
}


void ConnectionAdapter::onSocketRecv(net::Socket& socket, const MutableBuffer& buf, const net::Address&)
{
    // LTrace("On socket recv: ", buf.size())

    if (_parser.complete()) {
        // Buggy HTTP servers might send late data or multiple responses,
        // in which case the parser state might already be HPE_OK.
        // In this case we discard the late message and log the error here,
        // rather than complicate the app with this error handling logic.
        // This issue was noted using Webrick with Ruby 1.9.
        LWarn("Dropping late HTTP response: ", buf.str())
        return;
    }

    // Parse incoming HTTP messages
    _parser.parse(bufferCast<const char*>(buf), buf.size());
}


//
// Parser callbacks

void ConnectionAdapter::onParserHeader(const std::string& /* name */,
                                       const std::string& /* value */)
{
}


void ConnectionAdapter::onParserHeadersEnd(bool upgrade)
{
    // LTrace("On headers end: ", _parser.upgrade())

    if (_connection/* && _receiver */)
        _connection->onHeaders();

    // Set the position to the end of the headers once
    // they have been handled. Subsequent body chunks will
    // now start at the correct position.
    // _connection.incomingBuffer().position(_parser._parser.nread);
}


void ConnectionAdapter::onParserChunk(const char* buf, size_t len)
{
    // LTrace("On parser chunk: ", len)

    // Dispatch the payload
    if (_connection/* && _receiver */) {
        net::SocketAdapter::onSocketRecv(*_connection->socket().get(),
            mutableBuffer(const_cast<char*>(buf), len),
            _connection->socket()->peerAddress());
    }
}


void ConnectionAdapter::onParserError(const scy::Error& err)
{
    LWarn("On parser error: ", err.message)

#if 0
    // HACK: Handle those peski flash policy requests here
    auto base = dynamic_cast<net::TCPSocket*>(_connection.socket().get());
    if (base && std::string(base->buffer().data(), 22) == "<policy-file-request/>") {

        // Send an all access policy file by default
        // TODO: User specified flash policy
        std::string policy;

        // Add the following headers for HTTP policy response
        // policy += "HTTP/1.1 200 OK\r\nContent-Type: text/x-cross-domain-policy\r\nX-Permitted-Cross-Domain-Policies: all\r\n\r\n";
        policy += "<?xml version=\"1.0\"?><cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>";
        base->send(policy.c_str(), policy.length() + 1);
    }
#endif

    // Set error and close the connection on parser error
    if (_connection/* && _receiver */) {
        _connection->setError(err.message);
        _connection->close(); // do we want to force this?
    }
}


void ConnectionAdapter::onParserEnd()
{
    // LTrace("On parser end")

    if (_connection/* && _receiver */)
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
    // LTrace("Create: ", connection)

    IncomingProgress.sender = this;
    OutgoingProgress.sender = this;

    Incoming.autoStart(true);
    Outgoing.autoStart(true);

    // The Outgoing stream pumps data into the ConnectionAdapter,
    // which in turn proxies to the output Socket.
    Outgoing.emitter += slot(_connection->adapter(),
        static_cast<void (net::SocketAdapter::*)(IPacket&)>(&net::SocketAdapter::sendPacket));

    // The Incoming stream receives data from the ConnectionAdapter.
    //_connection->adapter()->Recv += slot(this, &ConnectionStream::onRecv);
    _connection->adapter()->addReceiver(this);
}


ConnectionStream::~ConnectionStream()
{
    // LTrace("Destroy")

    Outgoing.close();
    Incoming.close();
}


ssize_t ConnectionStream::send(const char* data, size_t len, int flags)
{
    // LTrace("Send: ", len)

    // Send outgoing data to the stream if adapters are attached,
    // or just proxy to the connection.
    if (Outgoing.numAdapters() > 0) {
        Outgoing.write(data, len);
    } else {
        return _connection->send(data, len, flags);
    }
    return len;
}


void ConnectionStream::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    // Push received data onto the Incoming stream.
    Incoming.write(bufferCast<const char*>(buffer), buffer.size());
}


Connection::Ptr ConnectionStream::connection()
{
    return _connection;
}


} // namespace http
} // namespace scy


/// @\}
