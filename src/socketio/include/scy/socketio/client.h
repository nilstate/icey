///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{

#pragma once

#include "scy/collection.h"
#include "scy/http/websocket.h"
#include "scy/json/json.h"
#include "scy/socketio/packet.h"
#include "scy/socketio/transaction.h"

#include <cstdint>
#include <string>


namespace scy {
namespace sockio {


struct ClientState : public State
{
    enum Type
    {
        Closed = 0x00,
        Connecting = 0x01,
        Connected = 0x02,
        Online = 0x04,
        Error = 0x08
    };

    std::string str(unsigned int id) const override
    {
        switch (id) {
            case Closed: return "Closed";
            case Connecting: return "Connecting";
            case Connected: return "Connected";
            case Online: return "Online";
            case Error: return "Error";
            default: return "undefined";
        }
    }
};


struct SocketIO_API ClientOptions
{
    std::string host = "127.0.0.1";
    uint16_t port = 4500;

    /// Whether or not to reconnect if disconnected from the server.
    bool reconnection = true;

    /// The number of times to attempt to reconnect if disconnected
    /// from the server. (0 = unlimited)
    int reconnectAttempts = 0;
    int reconnectDelay = 6 * 1000; // 6 secs
};


class SocketIO_API Client : public Stateful<ClientState>
    , public net::SocketAdapter
    , public PacketSignal
{
public:
    using Options = ClientOptions;
    using net::SocketAdapter::send; // inherit base send overloads

public:
    Client(const net::Socket::Ptr& socket, const Options& options = Options());
    virtual ~Client();

    virtual void connect();
    virtual void close();

    /// Send a default message packet.
    virtual int send(const std::string& message, bool ack = false);
    virtual int send(const json::Value& message, bool ack = false);

    /// Send an event packet.
    virtual int send(const std::string& event, const char* message, bool ack = false);
    virtual int send(const std::string& event, const std::string& message, bool ack = false);
    virtual int send(const std::string& event, const json::Value& message, bool ack = false);

    /// Send the given packet.
    virtual int send(const sockio::Packet& packet);

    /// Create a packet transaction.
    Transaction* createTransaction(const sockio::Packet& request, long timeout = 10000);

    /// Return a reference to the client options object.
    [[nodiscard]] Options& options();

    /// Return the underlying WebSocket instance.
    [[nodiscard]] http::ws::WebSocket& ws();

    /// Return the current session ID assigned by the server.
    [[nodiscard]] std::string sessionID() const;

    /// Return the error object (if any).
    [[nodiscard]] scy::Error error() const;

    /// Return true if the client is in Online state.
    [[nodiscard]] bool isOnline() const;

    /// Return true if currently reconnecting.
    [[nodiscard]] bool reconnecting() const;

    /// Return true if the client was previously in the Online state.
    [[nodiscard]] bool wasOnline() const;

    virtual const char* className() const { return "SocketIO::Client"; }

protected:
    virtual void setError(const scy::Error& error);

    /// Reset variables and timers at the beginning and end of each session.
    virtual void reset();

    virtual void onConnect();
    virtual void onOnline();
    virtual void onClose();
    virtual void onPacket(sockio::Packet& packet);
    virtual void onHandshake(sockio::Packet& packet);
    virtual void onMessage(sockio::Packet& packet);

    bool onSocketConnect(net::Socket& socket) override;
    bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress) override;
    bool onSocketError(net::Socket& socket, const scy::Error& error) override;
    bool onSocketClose(net::Socket& socket) override;

    /// Send the Socket.IO CONNECT packet for the default namespace.
    virtual void sendConnect();

    /// Handle a server ping by responding with pong.
    virtual void onServerPing();

    /// Called when no ping received within the expected interval.
    virtual void onPingTimeoutTimer();

    virtual void startReconnectTimer();
    virtual void stopReconnectTimer();
    virtual void onReconnectTimer();

protected:
    Timer _pingTimeoutTimer;
    Timer _reconnectTimer;
    scy::Error _error;
    std::string _sessionID;
    Options _options;
    http::ws::WebSocket _ws;
    int _pingTimeout;
    int _pingInterval;
    bool _reconnecting;
    bool _wasOnline;
};


//
// TCP Client
//

Client* createTCPClient(const Client::Options& options = Client::Options(),
                        uv::Loop* loop = uv::defaultLoop());

class SocketIO_API TCPClient : public Client
{
public:
    TCPClient(const Client::Options& options = Client::Options(),
              uv::Loop* loop = uv::defaultLoop());
};


//
// SSL Client
//

Client* createSSLClient(const Client::Options& options = Client::Options(),
                        uv::Loop* loop = uv::defaultLoop());

class SocketIO_API SSLClient : public Client
{
public:
    SSLClient(const Client::Options& options = Client::Options(),
              uv::Loop* loop = uv::defaultLoop());
};


} // namespace sockio
} // namespace scy


/// @}
