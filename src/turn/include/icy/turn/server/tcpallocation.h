///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "icy/net/tcpsocket.h"
#include "icy/turn/server/serverallocation.h"
#include "icy/turn/server/tcpconnectionpair.h"

#include <map>


namespace icy {
namespace turn {


class TURN_API Server;


using TCPConnectionPairMap = std::map<uint32_t, TCPConnectionPair::Ptr>;


/// Server-side TCP TURN allocation with connection pair management per RFC 6062.
/// Maintains a control connection for STUN signalling and an acceptor socket
/// for incoming peer TCP connections. Each peer connection is tracked as a
/// TCPConnectionPair and bridged to the client after a successful ConnectionBind.
class TURN_API TCPAllocation : public ServerAllocation
{
public:
    /// @param server   Owning TURN server.
    /// @param control  The accepted TCP socket used for STUN control messages.
    /// @param tuple    5-tuple identifying this allocation.
    /// @param username Authenticated username from the Allocate request.
    /// @param lifetime Initial lifetime in seconds.
    TCPAllocation(Server& server, const net::Socket::Ptr& control,
                  const FiveTuple& tuple, const std::string& username,
                  const uint32_t& lifetime);
    virtual ~TCPAllocation();

    /// Extends the base handler with Connect and ConnectionBind methods.
    /// @param request Incoming STUN request.
    /// @return true if handled.
    bool handleRequest(Request& request);

    /// Handles a Connect request; creates a TCPConnectionPair and initiates a
    /// TCP connection to the peer address carried in XOR-PEER-ADDRESS.
    /// @param request Incoming Connect request.
    void handleConnectRequest(Request& request);

    /// Handles a ConnectionBind request; associates the incoming TCP data socket
    /// with the pending TCPConnectionPair and activates the relay pipe.
    /// @param request Incoming ConnectionBind request.
    void handleConnectionBindRequest(Request& request);

    /// Sends a Connect success or failure response to the control connection.
    /// @param pair    The connection pair whose peer connection attempt completed.
    /// @param success true if the peer TCP connection succeeded.
    void sendPeerConnectResponse(TCPConnectionPair* pair, bool success);

    /// Sends a STUN message to the client over the control TCP connection.
    /// @param message Message to send.
    /// @return Number of bytes sent, or a negative value on error.
    int sendToControl(stun::Message& message);

    /// @return Reference to the TCP control socket.
    net::TCPSocket& control();

    /// @return The listen address of the peer acceptor socket (used as the relayed address).
    [[nodiscard]] net::Address relayedAddress() const;

    /// @return Reference to the map of active TCPConnectionPairs keyed by connection ID.
    TCPConnectionPairMap& pairs();

    /// Periodic timer callback; removes expired or pending-delete connection pairs,
    /// then calls the base class timer.
    /// @return false if the allocation should be destroyed.
    bool onTimer();

    /// Accepts incoming peer sockets on the relay acceptor socket.
    /// Checks permissions and sends a ConnectionAttempt indication to the control.
    /// @param sock Newly accepted peer socket.
    void onPeerAccept(const net::TCPSocket::Ptr& sock);

    /// Called when the control connection closes; marks the allocation for deletion.
    /// @param socket The closed control socket.
    /// @return false (event propagation convention).
    bool onControlClosed(net::Socket& socket);

protected:
    net::SocketEmitter _control;  // net::TCPSocket
    net::SocketEmitter _acceptor; // net::TCPSocket
    TCPConnectionPairMap _pairs;
};


} // namespace turn
} // namespace icy


/// @\}
