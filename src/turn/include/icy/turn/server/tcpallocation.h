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


#include "icy/collection.h"
#include "icy/net/tcpsocket.h"
#include "icy/turn/server/serverallocation.h"
#include "icy/turn/server/tcpconnectionpair.h"


namespace icy {
namespace turn {


class TURN_API Server;


using TCPConnectionPairMap = PointerCollection<uint32_t, TCPConnectionPair>;


class TURN_API TCPAllocation : public ServerAllocation
{
public:
    TCPAllocation(Server& server, const net::Socket::Ptr& control,
                  const FiveTuple& tuple, const std::string& username,
                  const uint32_t& lifetime);
    virtual ~TCPAllocation();

    bool handleRequest(Request& request);
    void handleConnectRequest(Request& request);
    void handleConnectionBindRequest(Request& request);

    /// Sends a Connect request response to control.
    void sendPeerConnectResponse(TCPConnectionPair* pair, bool success);

    int sendToControl(stun::Message& message);

    net::TCPSocket& control();
    [[nodiscard]] net::Address relayedAddress() const;
    TCPConnectionPairMap& pairs();

    bool onTimer();

    /// Accepts incoming peer sockets for ConnectionBind requests.
    void onPeerAccept(const net::TCPSocket::Ptr& sock);

    /// Callback for handling controll connection destruction.
    /// The allocation will be deleted.
    bool onControlClosed(net::Socket& socket);

protected:
    net::SocketEmitter _control;  // net::TCPSocket
    net::SocketEmitter _acceptor; // net::TCPSocket
    TCPConnectionPairMap _pairs;
};


} // namespace turn
} // namespace icy


/// @\}
