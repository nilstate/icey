///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/client.h"
#include "scy/logger.h"
#include "scy/util.h"

#include <algorithm>
#include <stdexcept>


namespace scy {
namespace http {


//
// Client Connection
//


ClientConnection::ClientConnection(const URL& url, const net::TCPSocket::Ptr& socket)
    : Connection(socket)
    , _url(url)
    , _connect(false)
    , _active(false)
    , _complete(false)
{
    auto uri = url.pathEtc();
    if (!uri.empty())
        _request.setURI(uri);
    _request.setHost(url.host(), url.port());

    // Set default error status
    _response.setStatus(http::StatusCode::BadGateway);

    replaceAdapter(std::make_unique<ConnectionAdapter>(this, HTTP_RESPONSE));
}


ClientConnection::~ClientConnection()
{
}


void ClientConnection::send()
{
    if (_connect) {
        throw std::runtime_error("ClientConnection::send: already connecting");
    }
    connect();
}


void ClientConnection::send(http::Request& req)
{
    if (_connect) {
        throw std::runtime_error("ClientConnection::send: already connecting");
    }
    _request = req;
    connect();
}


ssize_t ClientConnection::send(const char* data, size_t len, int flags)
{
    connect();

    if (_active)
        // Raw data will be pushed onto the Outgoing packet stream
        return Connection::send(data, len);
    else
        _outgoingBuffer.push_back(std::string(data, len));
    return static_cast<ssize_t>(len);
}


void ClientConnection::connect()
{
    if (!_connect) {
        _connect = true;
        _socket->connect(_url.host(), _url.port());
    }
}


void ClientConnection::setReadStream(std::ostream* os)
{
    if (_connect) {
        throw std::runtime_error("ClientConnection::setReadStream: already connecting");
    }

    _readStream.reset(os);
}


http::Message* ClientConnection::incomingHeader()
{
    return static_cast<http::Message*>(&_response);
}


http::Message* ClientConnection::outgoingHeader()
{
    return static_cast<http::Message*>(&_request);
}


//
// Socket Callbacks

bool ClientConnection::onSocketConnect(net::Socket& socket)
{
    // Set the connection to active
    _active = true;

    // Emit the connect signal so raw connections like
    // websockets can kick off the data flow
    Connect.emit();

    // Flush queued packets
    if (!_outgoingBuffer.empty()) {
        for (const auto& packet : _outgoingBuffer) {
            send(packet.c_str(), packet.length());
        }
        _outgoingBuffer.clear();
    } else {
        // Send the header
        sendHeader();
    }
    return false;
}


//
// Connection Callbacks

void ClientConnection::onHeaders()
{
    // Initialize download progress tracking from Content-Length header
    auto contentLength = _response.getContentLength();
    if (contentLength > 0) {
        IncomingProgress.total = contentLength;
        IncomingProgress.current = 0;
    }

    Headers.emit(_response);
}


void ClientConnection::onPayload(const MutableBuffer& buffer)
{
    // Write to the STL read stream if available
    if (_readStream) {
        _readStream->write(bufferCast<const char*>(buffer), buffer.size());
        _readStream->flush();
    }

    // Update download progress if total is known
    if (IncomingProgress.total > 0)
        IncomingProgress.update(static_cast<int>(buffer.size()));

    Payload.emit(buffer);
}


void ClientConnection::onComplete()
{
    if (_complete) {
        throw std::runtime_error("ClientConnection::onComplete: already complete");
    }
    _complete = true; // in case close() is called inside callback

    // Release any file handles
    if (_readStream) {
        auto fstream = dynamic_cast<std::ofstream*>(_readStream.get());
        if (fstream) {
            fstream->close();
        }
    }

    Complete.emit(_response);
}


void ClientConnection::onClose()
{
    if (!_complete)
        onComplete();
    Close.emit(*this);
}


//
// HTTP Client
//


Singleton<Client>& singleton()
{
    static Singleton<Client> singleton;
    return singleton;
}


Client& Client::instance()
{
    return *singleton().get();
}


void Client::destroy()
{
    singleton().destroy();
}


Client::Client()
{
}


Client::~Client()
{
    shutdown();
}


void Client::shutdown()
{
    Shutdown.emit();

    auto conns = _connections;
    for (auto conn : conns) {
        conn->close(); // close and remove via callback
    }
    if (!_connections.empty())
        LWarn("Client::shutdown: ", _connections.size(), " connections still active");
}


void Client::addConnection(ClientConnection::Ptr conn)
{
    conn->Close += slot(this, &Client::onConnectionClose, -1, -1); // lowest priority
    _connections.push_back(conn);
}


void Client::removeConnection(ClientConnection* conn)
{
    auto it = std::find_if(_connections.begin(), _connections.end(),
                           [conn](const ClientConnection::Ptr& c) { return conn == c.get(); });
    if (it != _connections.end()) {
        _connections.erase(it);
        return;
    }
    throw std::logic_error("Client::removeConnection: unknown connection");
}


void Client::onConnectionClose(Connection& conn)
{
    removeConnection(static_cast<ClientConnection*>(&conn));
}


} // namespace http
} // namespace scy


/// @}
