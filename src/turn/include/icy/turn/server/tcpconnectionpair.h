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


#include "icy/memory.h"
#include "icy/net/socketemitter.h"
#include "icy/net/tcpsocket.h"
#include "icy/stun/message.h"
#include "icy/timeout.h"
#include "icy/turn/turn.h"


namespace icy {
namespace turn {


class TURN_API TCPAllocation;

/// ConnectionBind request timeout (30 seconds, in milliseconds)
static constexpr int kConnectionBindTimeout = 30 * 1000;


/// Paired TCP connections forming a single TURN relay pipe between a client
/// and a peer. Lifetime managed by IntrusivePtr; the owning TCPAllocation
/// holds the Ptr in its pair map.
class TURN_API TCPConnectionPair : public RefCounted<TCPConnectionPair>
{
public:
    using Ptr = IntrusivePtr<TCPConnectionPair>;

    /// Constructs a pair with a randomly assigned connection ID.
    /// The caller must add the pair to the allocation's pair map.
    /// @param allocation The TCPAllocation that owns this pair.
    TCPConnectionPair(TCPAllocation& allocation);
    ~TCPConnectionPair();

    bool doPeerConnect(const net::Address& peerAddr);
    bool makeDataConnection();

    void setPeerSocket(const net::TCPSocket::Ptr& socket);
    void setClientSocket(const net::TCPSocket::Ptr& socket);

    bool onPeerConnectSuccess(net::Socket& socket);
    bool onPeerConnectError(net::Socket& socket, const Error& error);

    bool onClientDataReceived(net::Socket& socket, const MutableBuffer& buffer,
                              const net::Address& peerAddress);
    bool onPeerDataReceived(net::Socket& socket, const MutableBuffer& buffer,
                            const net::Address& peerAddress);

    bool onConnectionClosed(net::Socket& socket);

    void startTimeout();
    void requestDeletion();

    [[nodiscard]] bool expired() const;

    TCPAllocation& allocation;

    net::SocketEmitter client; // net::TCPSocket
    net::SocketEmitter peer;   // net::TCPSocket

    Buffer earlyPeerData;
    uint32_t connectionID;
    bool isDataConnection;
    bool pendingDelete = false;
    Timeout timeout;
    stun::TransactionID transactionID;

private:
    TCPConnectionPair(const TCPConnectionPair&) = delete;
    TCPConnectionPair& operator=(const TCPConnectionPair&) = delete;
    TCPConnectionPair(TCPConnectionPair&&) = delete;
    TCPConnectionPair& operator=(TCPConnectionPair&&) = delete;
};


} // namespace turn
} // namespace icy


/// @\}
