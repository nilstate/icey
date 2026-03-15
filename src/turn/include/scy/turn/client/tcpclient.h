///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "scy/collection.h"
#include "scy/net/tcpsocket.h"
#include "scy/turn/client/client.h"

#include <deque>
#include <string>


namespace scy {
namespace turn {


class TURN_API TCPClient;


struct TCPClientObserver : public ClientObserver
{
    virtual void onRelayConnectionCreated(TCPClient& client, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress) = 0;
    virtual void onRelayConnectionError(TCPClient& client, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress) {};
    virtual void onRelayConnectionClosed(TCPClient& client, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress) = 0;
    virtual void onRelayConnectionBindingFailed(TCPClient& client, const net::Address& peerAddress) {};
    virtual bool onPeerConnectionAttempt(TCPClient& client, const net::Address& peerAddress) { return true; };
};


struct RelayConnectionBinding
{
    uint32_t connectionID;
    net::Address peerAddress;
};


using ConnectionManager = KVCollection<net::Address, net::SocketEmitter>;
using ConnectionManagerMap = ConnectionManager::Map;


class TURN_API TCPClient : public Client
{
public:
    TCPClient(TCPClientObserver& observer, const Client::Options& options = Client::Options());
    virtual ~TCPClient();

    virtual void initiate();
    virtual void shutdown();

    virtual void sendConnectRequest(const net::Address& peerAddress);
    virtual void sendData(const char* data, size_t size, const net::Address& peerAddress);

    virtual bool handleResponse(const stun::Message& response);
    virtual void handleConnectResponse(const stun::Message& response);
    virtual void handleConnectErrorResponse(const stun::Message& response);
    virtual void handleConnectionBindResponse(const stun::Message& response);
    virtual void handleConnectionBindErrorResponse(const stun::Message& response);
    virtual void handleConnectionAttemptIndication(const stun::Message& response);

    virtual bool createAndBindConnection(uint32_t connectionID, const net::Address& peerAddress);
    // virtual void onRelayConnectionStateChange(void* sender, Net::SocketState& state, const Net::SocketState& oldState);

    virtual bool onRelayConnectionConnect(net::Socket& socket);
    virtual bool onRelayDataReceived(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress);
    virtual bool onRelayConnectionError(net::Socket& socket, const Error& error);
    virtual bool onRelayConnectionClosed(net::Socket& socket);

    void freeConnection(const net::Address& peerAddress);

    virtual int transportProtocol();
    ConnectionManager& connections();

    virtual const char* className() const { return "TURNTCPClient"; };

protected:
    TCPClientObserver& _observer;
    ConnectionManager _connections;
};


} // namespace turn
} // namespace scy


/// @\}
