///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "scy/turn/client/tcpclient.h"
#include "scy/logger.h"
#include "scy/net/tcpsocket.h"

#include <any>

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>


namespace scy {
namespace turn {


TCPClient::TCPClient(TCPClientObserver& observer, const Client::Options& options)
    : Client(observer, options, net::makeSocket<net::TCPSocket>())
    , _observer(observer)
{
    LTrace("Create");
}


TCPClient::~TCPClient()
{
    LTrace("Destroy");
    shutdown();
}


void TCPClient::initiate()
{
    Client::initiate();
}


void TCPClient::shutdown()
{
    LTrace("Shutdown");

    // Destroy transactions and stop timer
    Client::shutdown();

    {
        auto connections = _connections.map();
        LTrace("Shutdown: Active connections: ", connections.size());

        for (auto& [key, conn] : connections) {
            onRelayConnectionClosed(*conn.impl.get());
        }
        if (!_connections.empty())
            LWarn("Connections not fully cleaned up after shutdown");
    }

    LTrace("Shutdown: OK");
}


void TCPClient::sendConnectRequest(const net::Address& peerAddress)
{
    LTrace("Send Connect request");

    auto transaction = createTransaction();
    transaction->request().setClass(stun::Message::Request);
    transaction->request().setMethod(stun::Message::Connect);

    auto peerAttr = new stun::XorPeerAddress;
    peerAttr->setAddress(peerAddress);
    transaction->request().add(peerAttr);

    sendAuthenticatedTransaction(transaction);
}


void TCPClient::sendData(const char* data, size_t size, const net::Address& peerAddress)
{
    LTrace("Send data to ", peerAddress);

    // Ensure permissions exist for the peer.
    if (!hasPermission(peerAddress.host()))
        throw std::runtime_error("No permission exists for peer: " + peerAddress.host());

    auto& conn = _connections.get(peerAddress);

    conn->send(data, size);
}


bool TCPClient::handleResponse(const stun::Message& response)
{
    if (!Client::handleResponse(response)) {
        if (response.methodType() == stun::Message::Connect &&
            response.classType() == stun::Message::SuccessResponse)
            handleConnectResponse(response);

        else if (response.methodType() == stun::Message::ConnectionAttempt)
            handleConnectionAttemptIndication(response);

        else if (response.methodType() == stun::Message::ConnectionBind &&
                 response.classType() == stun::Message::SuccessResponse)
            handleConnectionBindResponse(response);

        else if (response.methodType() == stun::Message::ConnectionBind &&
                 response.classType() == stun::Message::ErrorResponse)
            handleConnectionBindErrorResponse(response);

        else if (response.methodType() == stun::Message::Connect &&
                 response.classType() == stun::Message::ErrorResponse)
            handleConnectErrorResponse(response);

        else
            return false;
    }

    return true;
}


void TCPClient::handleConnectResponse(const stun::Message& response)
{
    auto transaction = static_cast<stun::Transaction*>(response.opaque);
    auto peerAttr = transaction->request().get<stun::XorPeerAddress>();
    if (!peerAttr || (peerAttr && peerAttr->family() != stun::AddressFamily::IPv4)) {
        LWarn("Connect response missing valid XorPeerAddress in original request");
        return;
    }

    auto connAttr = response.get<stun::ConnectionID>();
    if (!connAttr) {
        LWarn("Connect response missing ConnectionID attribute");
        return;
    }

    createAndBindConnection(connAttr->value(), peerAttr->address());
}


void TCPClient::handleConnectErrorResponse(const stun::Message& response)
{
    auto transaction = static_cast<stun::Transaction*>(response.opaque);
    auto peerAttr = transaction->request().get<stun::XorPeerAddress>();
    if (!peerAttr || (peerAttr && peerAttr->family() != stun::AddressFamily::IPv4)) {
        LWarn("Connect error response missing valid XorPeerAddress");
        return;
    }

    _observer.onRelayConnectionBindingFailed(*this, peerAttr->address());
}


void TCPClient::handleConnectionAttemptIndication(const stun::Message& response)
{
    auto peerAttr = response.get<stun::XorPeerAddress>();
    if (!peerAttr || (peerAttr && peerAttr->family() != stun::AddressFamily::IPv4)) {
        LWarn("ConnectionAttempt indication missing valid XorPeerAddress");
        return;
    }

    auto connAttr = response.get<stun::ConnectionID>();
    if (!connAttr) {
        LWarn("ConnectionAttempt indication missing ConnectionID");
        return;
    }

    if (_observer.onPeerConnectionAttempt(*this, peerAttr->address()))
        createAndBindConnection(connAttr->value(), peerAttr->address());
}


void TCPClient::handleConnectionBindResponse(const stun::Message& response)
{
    LTrace("ConnectionBind success response");

    auto transaction = static_cast<stun::Transaction*>(response.opaque);
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(transaction->impl->opaque));

    auto& conn = _connections.get(req->peerAddress);
    if (!conn.impl) {
        LWarn("ConnectionBind response: no connection found for peer");
        return;
    }

    // Data will now be transferred as-is to and from the peer.
    conn.Recv += slot(this, &TCPClient::onRelayDataReceived);
    _observer.onRelayConnectionCreated(*this,
                                       std::dynamic_pointer_cast<net::TCPSocket>(conn.impl), req->peerAddress);

    LTrace("ConnectionBind success response: OK");
}


void TCPClient::handleConnectionBindErrorResponse(const stun::Message& response)
{
    LTrace("ConnectionBind error response");

    auto transaction = static_cast<stun::Transaction*>(response.opaque);
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(transaction->impl->opaque));

    freeConnection(req->peerAddress);
}


bool TCPClient::createAndBindConnection(uint32_t connectionID, const net::Address& peerAddress)
{
    LTrace("Create and bind connection: ", peerAddress);

    net::SocketEmitter conn(net::makeSocket<net::TCPSocket>());
    conn.Connect += slot(this, &TCPClient::onRelayConnectionConnect);
    conn.Error += slot(this, &TCPClient::onRelayConnectionError);
    conn.Close += slot(this, &TCPClient::onRelayConnectionClosed);

    auto req = new RelayConnectionBinding;
    req->connectionID = connectionID;
    req->peerAddress = peerAddress;
    conn->opaque = static_cast<void*>(req);
    conn->connect(_options.serverAddr); // errors via callback

    _connections.add(peerAddress, conn);
    return true;
}


bool TCPClient::onRelayConnectionConnect(net::Socket& socket)
{
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(socket.opaque));
    auto& conn = _connections.get(req->peerAddress);

    LTrace("On relay connection connect");
    if (!_connections.has(req->peerAddress))
        throw std::logic_error("relay connection not found for peer address");

    auto transaction = createTransaction(conn.impl);
    transaction->request().setClass(stun::Message::Request);
    transaction->request().setMethod(stun::Message::ConnectionBind);

    auto connAttr = new stun::ConnectionID;
    connAttr->setValue(req->connectionID);
    transaction->request().add(connAttr);

    sendAuthenticatedTransaction(transaction);
    return false;
}


bool TCPClient::onRelayConnectionError(net::Socket& socket, const Error& /* error */)
{
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(socket.opaque));
    auto& conn = _connections.get(req->peerAddress);

    LTrace("Relay connection error: ", req->peerAddress);
    if (!_connections.has(req->peerAddress)) {
        LWarn("Relay connection error for unknown peer: ", req->peerAddress);
    }

    _observer.onRelayConnectionError(*this,
                                     std::dynamic_pointer_cast<net::TCPSocket>(conn.impl), req->peerAddress);
    return false;
}


bool TCPClient::onRelayConnectionClosed(net::Socket& socket)
{
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(socket.opaque));
    auto& conn = _connections.get(req->peerAddress);

    LTrace("Relay connection closed: ", req->peerAddress);
    if (!_connections.has(req->peerAddress)) {
        LWarn("Relay connection closed for unknown peer: ", req->peerAddress);
    }

    _observer.onRelayConnectionClosed(*this,
                                      std::dynamic_pointer_cast<net::TCPSocket>(conn.impl), req->peerAddress);
    freeConnection(req->peerAddress);
    return false;
}


bool TCPClient::onRelayDataReceived(net::Socket& socket,
                                    const MutableBuffer& buffer,
                                    const net::Address& peerAddress)
{
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(socket.opaque));
    if (!_connections.has(req->peerAddress))
        LWarn("Relay data received for unknown peer: ", req->peerAddress);

    _observer.onRelayDataReceived(*this, bufferCast<const char*>(buffer),
                                  buffer.size(), req->peerAddress);
    return false;
}


void TCPClient::freeConnection(const net::Address& peerAddress)
{
    LTrace("Freeing TCP connection: ", peerAddress);

    auto& conn = _connections.get(peerAddress);
    auto req = static_cast<RelayConnectionBinding*>(std::any_cast<void*>(conn->opaque));
    conn.Recv -= slot(this, &TCPClient::onRelayDataReceived);
    conn.Connect -= slot(this, &TCPClient::onRelayConnectionConnect);
    conn.Error -= slot(this, &TCPClient::onRelayConnectionError);
    conn.Close -= slot(this, &TCPClient::onRelayConnectionClosed);
    _connections.remove(peerAddress); // release socket
    delete req;
}


ConnectionManager& TCPClient::connections()
{
    return _connections;
}


int TCPClient::transportProtocol()
{
    return kProtocolTCP;
}


} // namespace turn
} // namespace scy


/// @\}
