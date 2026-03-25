///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "icy/base.h"
#include "icy/buffer.h"
#include "icy/net/address.h"
#include "icy/net/dns.h"
#include "icy/net/net.h"
#include "icy/packetstream.h"
#include "icy/signal.h"


namespace icy {
namespace net {


class Net_API Socket;


/// Abstract adapter interface for socket send/receive chains.
///
/// SocketAdapter is the abstract interface for all socket classes.
/// A SocketAdapter can also be attached to a Socket in order to
/// override default Socket callbacks and behaviour, while still
/// maintaining the default Socket interface (see Socket::setAdapter).
///
/// This class can also be extended to implement custom processing
/// for received socket data before it is dispatched to the application
/// (see PacketSocketEmitter and Transaction classes).
class Net_API SocketAdapter
{
public:
    /// Creates the SocketAdapter.
    SocketAdapter(SocketAdapter* sender = nullptr);

    /// Destroys the SocketAdapter.
    virtual ~SocketAdapter() noexcept;

    /// Sends the given data buffer to the connected peer.
    /// Returns the number of bytes sent or -1 on error.
    /// No exception will be thrown.
    /// For TCP sockets the given peer address must match the
    /// connected peer address.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, int flags = 0);
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, const Address& peerAddress, int flags = 0);

    /// Sends an owned payload buffer to the connected peer.
    ///
    /// The buffer is moved through the adapter chain and retained by the
    /// transport layer until async write completion.
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, int flags = 0);
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, const Address& peerAddress, int flags = 0);

    /// Sends the given packet to the connected peer.
    /// Returns the number of bytes sent or -1 on error.
    /// No exception will be thrown.
    /// For TCP sockets the given peer address must match the
    /// connected peer address.
    virtual ssize_t sendPacket(const IPacket& packet, int flags = 0);
    virtual ssize_t sendPacket(const IPacket& packet, const Address& peerAddress, int flags = 0);

    /// Sends the given packet to the connected peer.
    /// This method provides delegate compatability, and unlike
    /// other send methods throws an exception if the underlying
    /// socket is closed.
    virtual void sendPacket(IPacket& packet);

    /// Sets the pointer to the outgoing data adapter.
    /// Send methods proxy data to this adapter by default.
    virtual void setSender(SocketAdapter* adapter);

    /// Returns the output SocketAdapter pointer
    SocketAdapter* sender();

    /// Sets the pointer to the incoming data adapter.
    /// Events proxy data to this adapter by default.
    virtual void addReceiver(SocketAdapter* adapter);

    /// Remove the given receiver.
    ///
    /// By default this function does nothing unless the given receiver
    /// matches the current receiver.
    virtual void removeReceiver(SocketAdapter* adapter);

    /// Returns true if the given receiver is connected.
    virtual bool hasReceiver(SocketAdapter* adapter);

    /// Returns all currently registered input SocketAdapter pointers.
    /// Dead (removed) entries are excluded from the returned list.
    std::vector<SocketAdapter*> receivers();

    /// Called when the socket establishes a connection.
    /// Forwards the event to all registered receivers in priority order.
    /// Override to intercept before the application sees the event.
    /// @param socket The connected socket.
    /// @return true to stop propagation to subsequent receivers.
    virtual bool onSocketConnect(Socket& socket);

    /// Called when data is received from the socket.
    /// Forwards the event to all registered receivers in priority order.
    /// @param socket      The socket that received data.
    /// @param buffer      The received data buffer.
    /// @param peerAddress Address of the sender.
    /// @return true to stop propagation to subsequent receivers.
    virtual bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress);

    /// Called when the socket encounters an error.
    /// Forwards the event to all registered receivers in priority order.
    /// @param socket The socket that encountered the error.
    /// @param error  Error details.
    /// @return true to stop propagation to subsequent receivers.
    virtual bool onSocketError(Socket& socket, const Error& error);

    /// Called when the socket is closed.
    /// Forwards the event to all registered receivers in priority order.
    /// @param socket The socket that was closed.
    /// @return true to stop propagation to subsequent receivers.
    virtual bool onSocketClose(Socket& socket);

    /// The priority of this adapter for STL sort operations.
    int priority = 0;

protected:
    virtual void cleanupReceivers();

    /// Reference-counted handle to a SocketAdapter
    struct Ref
    {
        SocketAdapter* ptr;
        bool alive;
    };

    SocketAdapter* _sender;
    std::vector<Ref> _receivers;
    bool _dirty = false;
};


} // namespace net
} // namespace icy


/// @}
