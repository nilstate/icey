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
#include "icy/net/address.h"
#include "icy/net/dns.h"
#include "icy/net/net.h"
#include "icy/packetstream.h"
#include "icy/signal.h"


namespace icy {
namespace net {


class Net_API Socket;


/// SocketAdapter is the abstract interface for all socket classes.
/// A SocketAdapter can also be attached to a Socket in order to
/// override default Socket callbacks and behaviour, while still
/// maintaining the default Socket interface (see Socket::setAdapter).
///
/// This class also be extended to implement custom processing
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

    /// Returns the input SocketAdapter pointer
    std::vector<SocketAdapter*> receivers();

    /// These virtual methods can be overridden as necessary
    /// to intercept socket events before they hit the application.
    /// Return true to stop propagation to subsequent receivers.
    virtual bool onSocketConnect(Socket& socket);
    virtual bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress);
    virtual bool onSocketError(Socket& socket, const Error& error);
    virtual bool onSocketClose(Socket& socket);

    /// The priority of this adapter for STL sort operations.
    int priority = 0;

protected:
    virtual void cleanupReceivers();

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


/// @\}
