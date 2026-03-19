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


class TURN_API TCPConnectionPair
{
public:
    TCPConnectionPair(TCPAllocation& allocation);
    virtual ~TCPConnectionPair();

    /// Initiates an outgoing TCP connection to the
    /// peer for Connect requests.
    bool doPeerConnect(const net::Address& peerAddr);

    /// Binds the client <> peer relay pipe once the
    /// ConnectionBind request is successfull.
    bool makeDataConnection();

    void setPeerSocket(const net::TCPSocket::Ptr& socket);
    void setClientSocket(const net::TCPSocket::Ptr& socket);

    /// Connection success callback for Connect request.
    bool onPeerConnectSuccess(net::Socket& socket);

    /// Connection error callback for Connect request.
    bool onPeerConnectError(net::Socket& socket, const Error& error);

    bool onClientDataReceived(net::Socket& socket, const MutableBuffer& buffer,
                              const net::Address& peerAddress);
    bool onPeerDataReceived(net::Socket& socket, const MutableBuffer& buffer,
                            const net::Address& peerAddress);

    /// Callback for handing either client or peer connections
    /// which result in the destruction of the TCPConnectionPair.
    bool onConnectionClosed(net::Socket& socket);

    /// Starts the ConnectionBind request timeout.
    ///
    /// If no ConnectionBind request associated with this peer data
    /// connection is received after 30 seconds, the peer data
    /// connection MUST be closed.
    void startTimeout();

    /// Signals the parent allocation to handle deletion instead
    /// of destroying ourselves directly.
    void requestDeletion();

    TCPAllocation& allocation;

    /// The client socket, nullptr to start.
    net::SocketEmitter client; // net::TCPSocket

    /// The client socket, nullptr to start.
    net::SocketEmitter peer; // net::TCPSocket

    /// Return true if the peer `ConnectionBind` request timed out.
    [[nodiscard]] bool expired() const;

    /// Stores early peer > client data.
    Buffer earlyPeerData;

    /// The unique connection ID.
    uint32_t connectionID;

    /// True when p2p relay is flowing.
    bool isDataConnection;

    /// True when this pair has been flagged for deletion.
    bool pendingDelete = false;

    /// The ConnectionBind request timeout counter.
    Timeout timeout;

    stun::TransactionID transactionID;

private:
    /// NonCopyable and NonMovable
    TCPConnectionPair(const TCPConnectionPair&) = delete;
    TCPConnectionPair& operator=(const TCPConnectionPair&) = delete;
    TCPConnectionPair(TCPConnectionPair&&) = delete;
    TCPConnectionPair& operator=(TCPConnectionPair&&) = delete;
};


} // namespace turn
} // namespace icy


/// @\}
