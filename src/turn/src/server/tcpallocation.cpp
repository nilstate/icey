///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "scy/turn/server/tcpallocation.h"
#include "scy/logger.h"
#include "scy/turn/server/server.h"

#include <stdexcept>


namespace scy {
namespace turn {


TCPAllocation::TCPAllocation(Server& server, const net::Socket::Ptr& control,
                             const FiveTuple& tuple,
                             const std::string& username,
                             const uint32_t& lifetime)
    : ServerAllocation(server, tuple, username, lifetime)
    , _control(std::dynamic_pointer_cast<net::TCPSocket>(control))
    , _acceptor(std::make_shared<net::TCPSocket>())
{
    // Bind a socket acceptor for incoming peer connections.
    _acceptor->bind(net::Address(server.options().listenAddr.host(), 0));
    _acceptor->listen();
    _acceptor.as<net::TCPSocket>()->AcceptConnection += slot(this, &TCPAllocation::onPeerAccept);

    // The allocation will be deleted if the control connection is lost.
    _control.Close += slot(this, &TCPAllocation::onControlClosed);

    LTrace("Initializing on ", _acceptor->address());
}


TCPAllocation::~TCPAllocation()
{
    LTrace("Destroy TCP allocation");

    _acceptor.as<net::TCPSocket>()
        ->AcceptConnection -= slot(this, &TCPAllocation::onPeerAccept);
    _acceptor->close();

    _control.Close -= slot(this, &TCPAllocation::onControlClosed);
    _control->close();

    this->pairs().clear();


    LTrace("Destroy TCP allocation: OK");
}


void TCPAllocation::onPeerAccept(const net::TCPSocket::Ptr& socket)
{
    LTrace("Peer connection accepted: ", socket->peerAddress());

    if (!hasPermission(socket->peerAddress().host())) {
        LTrace("No permission for peer: ", socket->peerAddress());
        return;
    }
    LTrace("Has permission for: ", socket->peerAddress());

    auto pair = new TCPConnectionPair(*this);
    pair->setPeerSocket(socket);

    stun::Message response(stun::Message::Indication,
                           stun::Message::ConnectionAttempt);

    auto addrAttr = new stun::XorPeerAddress;
    addrAttr->setAddress(socket->peerAddress());
    response.add(addrAttr);

    auto connAttr = new stun::ConnectionID;
    connAttr->setValue(pair->connectionID);
    response.add(connAttr);

    sendToControl(response);

    STrace << "Peer connection accepted with ID: " << pair->connectionID
           << std::endl;
}


bool TCPAllocation::handleRequest(Request& request)
{
    LTrace("Handle request");

    if (!ServerAllocation::handleRequest(request)) {
        if (request.methodType() == stun::Message::Connect)
            handleConnectRequest(request);
        else if (request.methodType() == stun::Message::ConnectionBind)
            handleConnectionBindRequest(request);
        else
            return false;
    }

    return true;
}


bool TCPAllocation::onTimer()
{
    LTrace("On timer");

    // Clean up any expired Connect request peer connections.
    // Collect keys first since free() modifies the map.
    std::vector<uint32_t>
        expired;
    for (auto& [key, pair] : this->pairs().map()) {
        if (pair->expired() || pair->pendingDelete) {
            LTrace("On timer: Removing expired/deleted peer");
            expired.push_back(key);
        }
    }
    for (auto key : expired) {
        this->pairs().free(key);
    }

    return ServerAllocation::onTimer();
}


void TCPAllocation::handleConnectRequest(Request& request)
{
    LTrace("Handle Connect request");

    auto peerAttr = request.get<stun::XorPeerAddress>();
    if (!peerAttr || peerAttr->family() != stun::AddressFamily::IPv4) {
        server().respondError(request, kErrorBadRequest, "Bad Request");
        return;
    }

    auto pair = new TCPConnectionPair(*this);
    pair->transactionID = request.transactionID();
    pair->doPeerConnect(peerAttr->address());
}


void TCPAllocation::handleConnectionBindRequest(Request& request)
{
    LTrace("Handle ConnectionBind Request");

    if (request.methodType() != stun::Message::ConnectionBind)
        throw std::logic_error("handleConnectionBindRequest called with wrong method");

    TCPConnectionPair* pair = nullptr;
    auto socket = _server.getTCPSocket(request.remoteAddress);
    try {
        if (!socket)
            throw std::runtime_error("Invalid TCP socket");

        if (request.transport != net::TCP)
            throw std::runtime_error("TLS not supported");

        auto connAttr = request.get<stun::ConnectionID>();
        if (!connAttr)
            throw std::runtime_error(
                "ConnectionBind missing CONNECTION-ID attribute");

        pair = pairs().get(connAttr->value(), false);
        if (!pair) {
            throw std::runtime_error("No client for ConnectionBind request: " +
                                     util::itostr(connAttr->value()));
        }

        if (pair->isDataConnection) {
            throw std::runtime_error("Already a peer data connection: " +
                                     util::itostr(connAttr->value()));
        }

        stun::Message response(stun::Message::SuccessResponse,
                               stun::Message::ConnectionBind);
        response.setTransactionID(request.transactionID());

        // Send the response back over the client connection
        socket->sendPacket(response);

        // Reassign the socket base instance to the client connection.
        pair->setClientSocket(socket);
        if (!pair->makeDataConnection()) {
            // Must have a client and peer by now
            throw std::runtime_error("BUG: Data connection binding failed");
        }

        if (!pair->isDataConnection)
            throw std::logic_error("Data connection flag not set after makeDataConnection");
    } catch (std::exception& exc) {
        LError("ConnectionBind error: ", exc.what());
        server()
            .respondError(request, kErrorBadRequest, "Bad Request");

        if (pair && !pair->isDataConnection) {
            pairs().free(pair->connectionID);
        }

        // Close the incoming connection
        socket->close();
    }
}


void TCPAllocation::sendPeerConnectResponse(TCPConnectionPair* pair, bool success)
{
    LTrace("Send peer Connect response: ", success);

    if (pair->transactionID.empty())
        throw std::logic_error("sendPeerConnectResponse called with empty transactionID");

    stun::Message response(stun::Message::SuccessResponse,
                           stun::Message::Connect);
    response.setTransactionID(pair->transactionID);

    if (success) {
        auto connAttr = new stun::ConnectionID;
        connAttr->setValue(pair->connectionID);
        response.add(connAttr);
    } else {
        auto errorCodeAttr = new stun::ErrorCode();
        errorCodeAttr->setErrorCode(kErrorConnectionTimeoutOrFailure);
        errorCodeAttr->setReason("Connection Timeout or Failure");
        response.add(errorCodeAttr);
    }

    sendToControl(response);
}


int TCPAllocation::sendToControl(stun::Message& message)
{
    LTrace("Send to control: ", message);
    return _control->sendPacket(message, 0);
}


bool TCPAllocation::onControlClosed(net::Socket& socket)
{
    LTrace("Control socket disconnected");

    // The allocation will be destroyed on the
    // next timer call to IAllocation::deleted()
    _deleted = true;
    return false;
}


net::TCPSocket& TCPAllocation::control()
{
    return *_control.as<net::TCPSocket>();
}


TCPConnectionPairMap& TCPAllocation::pairs()
{
    return _pairs;
}


net::Address TCPAllocation::relayedAddress() const
{
    return _acceptor->address();
}


} // namespace turn
} // namespace scy


/// @\}
