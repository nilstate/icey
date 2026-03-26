///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/server/tcpallocation.h"
#include "icy/logger.h"
#include "icy/turn/server/server.h"

#include <stdexcept>


namespace icy {
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

    // Clear the pair map; IntrusivePtr releases each pair.
    _pairs.clear();

    LTrace("Destroy TCP allocation: OK");
}


void TCPAllocation::onPeerAccept(const net::TCPSocket::Ptr& socket)
{
    LTrace("Peer connection accepted: ", socket->peerAddress());

    if (!hasPermission(socket->peerAddress())) {
        LTrace("No permission for peer: ", socket->peerAddress());
        return;
    }
    LTrace("Has permission for: ", socket->peerAddress());

    auto pair = makeIntrusive<TCPConnectionPair>(*this);
    pair->setPeerSocket(socket);

    stun::Message response(stun::Message::Indication,
                           stun::Message::ConnectionAttempt);

    response.add<stun::XorPeerAddress>().setAddress(socket->peerAddress());
    response.add<stun::ConnectionID>().setValue(pair->connectionID);

    sendToControl(response);

    STrace << "Peer connection accepted with ID: " << pair->connectionID;

    _pairs[pair->connectionID] = std::move(pair);
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

    // Clean up expired or pending-delete connection pairs.
    // Collect keys first since erase modifies the map.
    std::vector<uint32_t> expired;
    for (auto& [key, pair] : _pairs) {
        if (pair->expired() || pair->pendingDelete) {
            LTrace("On timer: Removing expired/deleted peer: ", key);
            expired.push_back(key);
        }
    }
    for (auto key : expired) {
        _pairs.erase(key);
    }

    return ServerAllocation::onTimer();
}


void TCPAllocation::handleConnectRequest(Request& request)
{
    LTrace("Handle Connect request");

    auto peerAttr = request.get<stun::XorPeerAddress>();
    if (!peerAttr || peerAttr->family() == stun::AddressFamily::Undefined) {
        server().respondError(request, kErrorBadRequest, "Bad Request");
        return;
    }

    auto pair = makeIntrusive<TCPConnectionPair>(*this);
    pair->transactionID = request.transactionID();
    _pairs[pair->connectionID] = pair;
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

        auto it = _pairs.find(connAttr->value());
        if (it == _pairs.end()) {
            throw std::runtime_error("No client for ConnectionBind request: " +
                                     util::itostr(connAttr->value()));
        }
        pair = it->second.get();

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
            throw std::runtime_error("BUG: Data connection binding failed");
        }

        if (!pair->isDataConnection)
            throw std::logic_error("Data connection flag not set after makeDataConnection");
    } catch (std::exception& exc) {
        LError("ConnectionBind error: ", exc.what());
        server().respondError(request, kErrorBadRequest, "Bad Request");

        if (pair && !pair->isDataConnection) {
            _pairs.erase(pair->connectionID);
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

    stun::Message response(success ? stun::Message::SuccessResponse
                                    : stun::Message::ErrorResponse,
                           stun::Message::Connect);
    response.setTransactionID(pair->transactionID);

    if (success) {
        response.add<stun::ConnectionID>().setValue(pair->connectionID);
    } else {
        auto& err = response.add<stun::ErrorCode>();
        err.setErrorCode(kErrorConnectionTimeoutOrFailure);
        err.setReason("Connection Timeout or Failure");
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
} // namespace icy


/// @}
