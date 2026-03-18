///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{


#include "scy/socketio/client.h"
#include "scy/http/client.h"
#include "scy/net/sslsocket.h"
#include "scy/net/tcpsocket.h"

#include <ctime>
#include <sstream>
#include <stdexcept>


namespace scy {
namespace sockio {


//
// TCP Client
//


Client* createTCPClient(const Client::Options& options, uv::Loop* loop)
{
    return new Client(std::make_shared<net::TCPSocket>(loop), options);
}


TCPClient::TCPClient(const Client::Options& options, uv::Loop* loop)
    : Client(std::make_shared<net::TCPSocket>(loop), options)
{
}


//
// SSL Client
//


Client* createSSLClient(const Client::Options& options, uv::Loop* loop)
{
    return new Client(std::make_shared<net::SSLSocket>(loop), options);
}


SSLClient::SSLClient(const Client::Options& options, uv::Loop* loop)
    : Client(std::make_shared<net::SSLSocket>(loop), options)
{
}


//
// Base Client
//


Client::Client(const net::Socket::Ptr& socket, const Client::Options& options)
    : _pingTimeoutTimer(socket->loop())
    , _reconnectTimer(socket->loop())
    , _options(options)
    , _ws(socket)
    , _pingTimeout(0)
    , _pingInterval(0)
    , _reconnecting(false)
    , _wasOnline(false)
{
    _ws.addReceiver(this);
}


Client::~Client()
{
    _ws.removeReceiver(this);
    close();
}


void Client::connect()
{
    LTrace("Socket.IO Connecting");

    if (_options.host.empty() || !_options.port)
        throw std::runtime_error("The Socket.IO server address is not set.");

    reset();

    setState(this, ClientState::Connecting);

    // Build the request URI
    // Engine.IO v4: EIO=4, transport=websocket
    std::ostringstream url;
    url << "/socket.io/?EIO=4&transport=websocket";
    if (!_sessionID.empty()) {
        url << "&sid=" << _sessionID;
    }
    url << "&t=" << std::time(nullptr);

    _ws.request().setURI(url.str());
    _ws.request().setHost(_options.host, _options.port);
    _ws.socket->connect(_options.host, _options.port);
}


void Client::close()
{
    LTrace("Closing");
    if (_sessionID.empty())
        return;

    reset();
    onClose();
    LTrace("Closing: OK");
}


int Client::send(const std::string& message, bool ack)
{
    Packet packet(message, ack);
    return send(packet);
}


int Client::send(const json::Value& message, bool ack)
{
    Packet packet(message, ack);
    return send(packet);
}


int Client::send(const sockio::Packet& packet)
{
    return _ws.sendPacket(packet);
}


int Client::send(const std::string& event, const char* message, bool ack)
{
    Packet packet(event, std::string(message), ack);
    return send(packet);
}


int Client::send(const std::string& event, const std::string& message, bool ack)
{
    Packet packet(event, message, ack);
    return send(packet);
}


int Client::send(const std::string& event, const json::Value& message, bool ack)
{
    Packet packet(event, message, ack);
    return send(packet);
}


Transaction* Client::createTransaction(const sockio::Packet& request, long timeout)
{
    return new Transaction(*this, request, timeout);
}


void Client::sendConnect()
{
    // Send Socket.IO CONNECT packet for default namespace
    // Wire format: "40" (frame=4 Message, type=0 Connect)
    LTrace("Sending Socket.IO CONNECT");
    _ws.send("40", 2);
}


void Client::onServerPing()
{
    // Engine.IO v4: server sends ping ("2"), client responds with pong ("3")
    LTrace("Received server ping, sending pong");
    _ws.send("3", 1);

    // Reset the ping timeout timer - we know the connection is alive
    _pingTimeoutTimer.stop();
    if (_pingInterval > 0 && _pingTimeout > 0) {
        _pingTimeoutTimer.setTimeout(_pingInterval + _pingTimeout);
        _pingTimeoutTimer.start();
    }
}


void Client::reset()
{
    // Note: Only reset session related variables here.
    // Do not reset host and port variables.

    _pingTimeoutTimer.Timeout -= slot(this, &Client::onPingTimeoutTimer);
    _pingTimeoutTimer.stop();

    _reconnectTimer.Timeout -= slot(this, &Client::onReconnectTimer);
    _reconnectTimer.stop();

    _ws.socket->close();

    _error.reset();
    _sessionID = "";
    _pingInterval = 0;
    _pingTimeout = 0;
}


void Client::setError(const scy::Error& error)
{
    LError("Set error: ", error.message);

    // Set the wasOnline flag if previously online before error
    if (stateEquals(ClientState::Online))
        _wasOnline = true;

    _error = error;
    setState(this, ClientState::Error);

    // Start the reconnection timer if required
    if (_options.reconnection) {
        startReconnectTimer();
    } else {
        close();
    }
}


void Client::onConnect()
{
    LTrace("On connect");

    setState(this, ClientState::Connected);

    // Stop the reconnection timer if required
    stopReconnectTimer();
}


void Client::startReconnectTimer()
{
    if (!_options.reconnection)
        return;

    _reconnectTimer.Timeout += slot(this, &Client::onReconnectTimer);
    _reconnectTimer.setTimeout(_options.reconnectDelay);
    _reconnectTimer.start();
    _reconnectTimer.handle().ref();
    _reconnecting = true;
}


void Client::stopReconnectTimer()
{
    if (_reconnecting) {
        _reconnectTimer.Timeout -= slot(this, &Client::onReconnectTimer);
        _reconnectTimer.handle().unref();
        _reconnectTimer.stop();
        _reconnecting = false;
    }
}


void Client::onOnline()
{
    LTrace("On online");

    setState(this, ClientState::Online);

    // Start the ping timeout timer.
    // Engine.IO v4: server pings us. If we don't receive a ping within
    // pingInterval + pingTimeout, the connection is dead.
    if (_pingInterval > 0 && _pingTimeout > 0) {
        _pingTimeoutTimer.Timeout += slot(this, &Client::onPingTimeoutTimer);
        _pingTimeoutTimer.setTimeout(_pingInterval + _pingTimeout);
        _pingTimeoutTimer.start();
    }
}


void Client::onClose()
{
    LTrace("On close");

    stopReconnectTimer();

    // Back to initial Closed state
    setState(this, ClientState::Closed);
    _wasOnline = false;
}


//
// Socket Callbacks

bool Client::onSocketConnect(net::Socket& socket)
{
    onConnect();
    return false;
}


bool Client::onSocketError(net::Socket& socket, const scy::Error& error)
{
    LTrace("On socket error: ", error.message);
    setError(error);
    return false;
}


bool Client::onSocketClose(net::Socket& socket)
{
    LTrace("On socket close");

    // If no error was set, treat as unexpected disconnect
    if (!_error.any()) {
        scy::Error err;
        err.message = "Disconnected from server";
        setError(err);
    }
    return false;
}


bool Client::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    LTrace("On socket recv: ", buffer.size());

    sockio::Packet pkt;
    char* buf = bufferCast<char*>(buffer);
    size_t len = buffer.size();
    size_t nread = 0;
    while (len > 0 && (nread = pkt.read(constBuffer(buf, len))) > 0) {
        onPacket(pkt);
        buf += nread;
        len -= nread;
    }
    if (len == buffer.size())
        LWarn("Failed to parse incoming Socket.IO packet.");
    return false;
}


void Client::onHandshake(sockio::Packet& packet)
{
    LTrace("On handshake: ", state());

    json::Value json = packet.json();
    if (json.is_object()) {
        if (json.contains("sid"))
            _sessionID = json["sid"].get<std::string>();
        if (json.contains("pingInterval"))
            _pingInterval = json["pingInterval"].get<int>();
        if (json.contains("pingTimeout"))
            _pingTimeout = json["pingTimeout"].get<int>();
    }

    LDebug("On handshake: "
           "sid=",
           _sessionID, ", "
                       "pingInterval=",
           _pingInterval, ", "
                          "pingTimeout=",
           _pingTimeout);

    // After receiving the Engine.IO Open packet, send Socket.IO CONNECT
    sendConnect();
}


void Client::onMessage(sockio::Packet& packet)
{
    LTrace("On message: ", packet.toString());

    switch (packet.type()) {
        case Type::Connect:
            // Server confirmed namespace connection - we're online
            onOnline();
            break;

        case Type::Disconnect:
            // Server disconnected us from the namespace
            {
                scy::Error err;
                err.message = "Server disconnected";
                setError(err);
            }
            break;

        case Type::Event:
            emit(packet);
            break;

        case Type::Ack:
            emit(packet);
            break;

        case Type::ConnectError: {
            std::string msg = "Connection rejected";
            json::Value json = packet.json();
            if (json.is_object() && json.contains("message"))
                msg = json["message"].get<std::string>();
            scy::Error err;
            err.message = msg;
            setError(err);
        } break;

        case Type::BinaryEvent:
        case Type::BinaryAck:
            LWarn("Binary packets not yet implemented");
            break;

        default:
            LWarn("Unknown Socket.IO packet type: ", static_cast<int>(packet.type()));
            break;
    }
}


void Client::onPacket(sockio::Packet& packet)
{
    LTrace("On packet: ", packet.frameString(), " ", packet.typeString());

    // Handle packets by Engine.IO frame type
    switch (packet.frame()) {
        case Frame::Open:
            onHandshake(packet);
            break;

        case Frame::Close:
            reset();
            onClose();
            break;

        case Frame::Ping:
            // Engine.IO v4: server sends ping, client responds with pong
            onServerPing();
            break;

        case Frame::Pong:
            // Shouldn't happen in EIO v4 (client doesn't send pings)
            // but handle gracefully
            break;

        case Frame::Message:
            onMessage(packet);
            break;

        case Frame::Upgrade:
        case Frame::Noop:
            break;

        default:
            LWarn("Unknown Engine.IO frame type: ", static_cast<int>(packet.frame()));
            break;
    }
}


void Client::onPingTimeoutTimer()
{
    LWarn("Ping timeout - no ping from server within expected interval");

    scy::Error err;
    err.message = "Ping timeout";
    setError(err);
}


void Client::onReconnectTimer()
{
    LTrace("On reconnect timer");
    try {
        connect();
    } catch (std::exception& exc) {
        LError("Reconnection attempt failed: ", exc.what());
    }
    _reconnectTimer.again();
}


http::ws::WebSocket& Client::ws()
{
    return _ws;
}


std::string Client::sessionID() const
{
    return _sessionID;
}


Error Client::error() const
{
    return _error;
}


bool Client::isOnline() const
{
    return stateEquals(ClientState::Online);
}


Client::Options& Client::options()
{
    return _options;
}


bool Client::reconnecting() const
{
    return _reconnecting;
}


bool Client::wasOnline() const
{
    return _wasOnline;
}


} // namespace sockio
} // namespace scy


/// @}
