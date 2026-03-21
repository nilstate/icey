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


#include "icy/net/packetsocket.h"
#include "icy/net/udpsocket.h"
#include "icy/turn/server/serverallocation.h"
#include "icy/turn/types.h"


namespace icy {
namespace turn {


class TURN_API Server;
class TURN_API IConnection;


/// Server-side UDP TURN allocation that relays datagrams to permitted peers.
/// Binds a dedicated UDP relay socket for peer communication and forwards
/// incoming datagrams to the client as Data Indications.
class TURN_API UDPAllocation : public ServerAllocation
{
public:
    /// Binds the relay UDP socket and registers the peer receive callback.
    /// @param server   Owning TURN server.
    /// @param tuple    5-tuple identifying this allocation.
    /// @param username Authenticated username from the Allocate request.
    /// @param lifetime Initial lifetime in seconds.
    UDPAllocation(Server& server, const FiveTuple& tuple,
                  const std::string& username, const uint32_t& lifetime);
    virtual ~UDPAllocation();

    /// Receive callback for the relay UDP socket; validates permissions, updates
    /// bandwidth usage, and forwards data to the client as a Data Indication.
    /// @param socket      The relay socket.
    /// @param buffer      Received datagram.
    /// @param peerAddress Source address of the peer datagram.
    bool onPeerDataReceived(net::Socket& socket, const MutableBuffer& buffer,
                            const net::Address& peerAddress);

    /// Extends the base handler with Send Indication support.
    /// @param request Incoming STUN request or indication.
    /// @return true if handled.
    bool handleRequest(Request& request);

    /// Handles a Send Indication from the client; validates permissions and
    /// relays the DATA attribute payload to the peer via the relay socket.
    /// Silently discards on permission or attribute errors.
    /// @param request Incoming Send Indication.
    void handleSendIndication(Request& request);

    /// Sends raw data to @p peerAddress via the relay UDP socket.
    /// Updates bandwidth usage and returns -1 if the allocation is expired.
    /// @param data        Payload bytes to relay.
    /// @param size        Number of bytes.
    /// @param peerAddress Destination peer address.
    /// @return Number of bytes sent, or -1 if the quota is exhausted.
    ssize_t send(const char* data, size_t size,
                 const net::Address& peerAddress);

    /// @return The bound address of the relay UDP socket.
    [[nodiscard]] net::Address relayedAddress() const;

private:
    net::SocketEmitter _relaySocket; // net::UDPSocket
};

} // namespace turn
} // namespace icy


/// @\}
