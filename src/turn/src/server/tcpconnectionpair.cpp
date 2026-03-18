///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "scy/turn/server/tcpconnectionpair.h"
#include "scy/crypto/crypto.h"
#include "scy/net/util.h"
#include "scy/turn/server/server.h"
#include "scy/turn/server/tcpallocation.h"

#include <stdexcept>


namespace scy {
namespace turn {


TCPConnectionPair::TCPConnectionPair(TCPAllocation& allocation)
    : allocation(allocation)
    , client(nullptr)
    , peer(nullptr)
    , earlyPeerData(0)
    , connectionID(util::randomNumber())
    , isDataConnection(false)
{
    while (!allocation.pairs().add(connectionID, this, false)) {
        connectionID = util::randomNumber();
    }
    LTrace("Create: ", connectionID);
}


TCPConnectionPair::~TCPConnectionPair()
{
    LTrace("Destroy: ", connectionID);

    if (client.impl) {
        client.Recv -= slot(this, &TCPConnectionPair::onClientDataReceived);
        client.Close -= slot(this, &TCPConnectionPair::onConnectionClosed);
        client->close();
    }
    if (peer.impl) {
        peer.Recv -= slot(this, &TCPConnectionPair::onPeerDataReceived);
        peer.Connect -= slot(this, &TCPConnectionPair::onPeerConnectSuccess);
        peer.Error -= slot(this, &TCPConnectionPair::onPeerConnectError);
        peer.Close -= slot(this, &TCPConnectionPair::onConnectionClosed);
        peer->close();
    }

    if (!allocation.pairs().exists(connectionID))
        LWarn("Connection pair not found in allocation pairs during destruction: ", connectionID);
    allocation.pairs()
        .remove(connectionID);
}


bool TCPConnectionPair::doPeerConnect(const net::Address& peerAddr)
{
    try {
        if (transactionID.empty())
            throw std::logic_error("transactionID must be set before doPeerConnect");
        peer.swap(std::make_shared<net::TCPSocket>());
        peer.impl->opaque = static_cast<void*>(this);
        peer.Close += slot(this, &TCPConnectionPair::onConnectionClosed);

        // Start receiving early media
        peer.Recv += slot(this, &TCPConnectionPair::onPeerDataReceived);

        // Connect request specific events
        peer.Connect += slot(this, &TCPConnectionPair::onPeerConnectSuccess);
        peer.Error += slot(this, &TCPConnectionPair::onPeerConnectError);

        client->connect(peerAddr);
    } catch (std::exception& exc) {
        LError("Peer connect error: ", exc.what());
        return false;
    }
    return true;
}


void TCPConnectionPair::setPeerSocket(const net::TCPSocket::Ptr& socket)
{
    STrace << "Set peer socket: " << connectionID << ": "
           << socket->peerAddress() << std::endl;

    if (peer.impl)
        throw std::logic_error("peer socket already set for connection pair");
    peer.swap(socket);
    peer.Close += slot(this, &TCPConnectionPair::onConnectionClosed);

    // Receive and buffer early media from peer
    peer.Recv += slot(this, &TCPConnectionPair::onPeerDataReceived);

    net::setServerSocketRecvBufSize<uv_tcp_t>(
        *socket.get(), SERVER_SOCK_BUF_SIZE);
}


void TCPConnectionPair::setClientSocket(const net::TCPSocket::Ptr& socket)
{
    STrace << "Set client socket: " << connectionID << ": "
           << socket->peerAddress() << std::endl;

    if (client.impl)
        throw std::logic_error("client socket already set for connection pair");
    client.swap(socket);
    client.Close += slot(this, &TCPConnectionPair::onConnectionClosed);

    net::setServerSocketRecvBufSize<uv_tcp_t>(
        *socket.get(), SERVER_SOCK_BUF_SIZE);
}


bool TCPConnectionPair::makeDataConnection()
{
    LTrace("Make data connection: ", connectionID);
    if (!peer.impl || !client.impl)
        return false;

    peer.Recv += slot(this, &TCPConnectionPair::onPeerDataReceived);
    client.Recv += slot(this, &TCPConnectionPair::onClientDataReceived);

    // Relase and unbind the client socket from the server.
    // The client socket instance, events and data will be
    // managed by the TCPConnectionPair from now on.

    allocation.server().releaseTCPSocket(*client.impl.get());

    // Send early data from peer to client
    if (earlyPeerData.size()) {
        LTrace("Flushing early media: ", earlyPeerData.size());
        client->send(earlyPeerData.data(), earlyPeerData.size());
        earlyPeerData.clear();
    }

    return (isDataConnection = true);
}


bool TCPConnectionPair::onPeerDataReceived(net::Socket&,
                                           const MutableBuffer& buffer,
                                           const net::Address& peerAddress)
{
    LTrace("Peer > Client: ", buffer.size());

    const char* buf = bufferCast<const char*>(buffer);
    size_t len = buffer.size();
    if (client.impl) {

        allocation.updateUsage(len);
        if (allocation.deleted())
            return false;

        client->send(buf, len);
    }

    // Buffer early media
    else {
        size_t maxSize =
            allocation.server().options().earlyMediaBufferSize;
        LDebug("Buffering early data: ", len);
        if (len > maxSize) {
            LWarn("Dropping early media: Oversize packet: ", len);
            return false;
        }
        if (earlyPeerData.size() > maxSize) {
            LWarn("Dropping early media: Buffer at capacity >= ", maxSize);
            return false;
        }

        earlyPeerData.insert(earlyPeerData.end(), buf, buf + len);
    }
    return false;
}


bool TCPConnectionPair::onClientDataReceived(net::Socket&,
                                             const MutableBuffer& buffer,
                                             const net::Address& peerAddress)
{
    LTrace("Client > Peer: ", buffer.size());

    if (peer.impl) {
        allocation.updateUsage(buffer.size());
        if (allocation.deleted())
            return false;

        peer->send(bufferCast<char*>(buffer), buffer.size());
    }
    return false;
}


bool TCPConnectionPair::onPeerConnectSuccess(net::Socket& socket)
{
    LTrace("Peer Connect request success");
    if (&socket != peer.impl.get())
        throw std::logic_error("onPeerConnectSuccess called with unexpected socket");
    peer.Connect -= slot(this, &TCPConnectionPair::onPeerConnectSuccess);
    peer.Error -= slot(this, &TCPConnectionPair::onPeerConnectError);

    // If no ConnectionBind request associated with this peer data
    // connection is received after 30 seconds, the peer data connection
    // MUST be closed.

    allocation.sendPeerConnectResponse(this, true);

    startTimeout();
    return false;
}


bool TCPConnectionPair::onPeerConnectError(net::Socket& socket, const Error& error)
{
    LTrace("Peer Connect request error: ", error.message);
    if (&socket != peer.impl.get())
        throw std::logic_error("onPeerConnectError called with unexpected socket");
    allocation.sendPeerConnectResponse(this, false);

    // The TCPConnectionPair will be cleaned up on next timer tick
    return false;
}


bool TCPConnectionPair::onConnectionClosed(net::Socket& socket)
{
    LTrace("Connection pair socket closed: ", connectionID, ": ", &socket);
    // Flag for deletion by the parent allocation's timer instead of `delete this`
    requestDeletion();
    return false;
}


void TCPConnectionPair::requestDeletion()
{
    pendingDelete = true;
}


void TCPConnectionPair::startTimeout()
{
    timeout.reset();
}


bool TCPConnectionPair::expired() const
{
    return timeout.running() && timeout.expired();
}


} // namespace turn
} // namespace scy


/// @\}
