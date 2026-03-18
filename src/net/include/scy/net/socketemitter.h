///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "scy/base.h"
#include "scy/logger.h"
#include "scy/net/socket.h"
#include "scy/net/socketadapter.h"
#include "scy/packetfactory.h"
#include "scy/packetsignal.h"


namespace scy {
namespace net {


/// SocketAdapter class that adds signal callbacks for sockets.
/// Asde from adding a signal interface the class works as a wrapper for
/// the socket instance, and is designed to be used the same way
/// as a `std::unique_ptr` by overriding the ->() operator.
class Net_API SocketEmitter : public SocketAdapter
{
public:
    /// Creates the SocketEmitter
    SocketEmitter(const Socket::Ptr& socket = nullptr);

    /// Copy constructor
    SocketEmitter(const SocketEmitter& that);

    /// Destroys the SocketAdapter.
    virtual ~SocketEmitter() noexcept;

    /// Signals that the socket is connected.
    LocalSignal<bool(Socket&)> Connect;

    /// Signals when data is received by the socket.
    LocalSignal<bool(Socket&, const MutableBuffer&, const Address&)> Recv;

    /// Signals that the socket is closed in error.
    /// This signal will be sent just before the
    /// Closed signal.
    LocalSignal<bool(Socket&, const scy::Error&)> Error;

    /// Signals that the underlying socket is closed.
    LocalSignal<bool(Socket&)> Close;

    /// Adds an input SocketAdapter for receiving socket signals.
    virtual void addReceiver(SocketAdapter* adapter) override;

    /// Removes an input SocketAdapter.
    virtual void removeReceiver(SocketAdapter* adapter) override;

    /// Swap the underlying socket pointer.
    virtual void swap(const Socket::Ptr& socket);

    /// Cast getter for the underlying socket.
    template <class T>
    T* as()
    {
        return dynamic_cast<T*>(impl.get());
    }

    /// Accessor to the underlying socket.
    Socket* operator->() const
    {
        return impl.get();
    }

    /// Pointer to the underlying socket.
    /// Sent data will be proxied to this socket.
    Socket::Ptr impl;

protected:
    /// Internal callback events.
    bool onSocketConnect(Socket& socket) override;
    bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress) override;
    bool onSocketError(Socket& socket, const scy::Error& error) override;
    bool onSocketClose(Socket& socket) override;
};


} // namespace net
} // namespace scy


/// @\}
