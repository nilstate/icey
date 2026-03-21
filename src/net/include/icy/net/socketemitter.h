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
#include "icy/logger.h"
#include "icy/net/socket.h"
#include "icy/net/socketadapter.h"
#include "icy/packetfactory.h"
#include "icy/packetsignal.h"


namespace icy {
namespace net {


/// SocketAdapter class that adds signal callbacks for sockets.
/// Asde from adding a signal interface the class works as a wrapper for
/// the socket instance, and is designed to be used the same way
/// as a `std::unique_ptr` by overriding the ->() operator.
class Net_API SocketEmitter : public SocketAdapter
{
public:
    /// Creates the SocketEmitter and optionally attaches it to a socket.
    /// If @p socket is provided, this emitter registers itself as a receiver.
    /// @param socket Optional socket to attach to; pass nullptr to attach later via swap().
    SocketEmitter(const Socket::Ptr& socket = nullptr);

    /// Copy constructor; copies all signal connections and attaches to the same socket.
    /// @param that The SocketEmitter to copy from.
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
    LocalSignal<bool(Socket&, const icy::Error&)> Error;

    /// Signals that the underlying socket is closed.
    LocalSignal<bool(Socket&)> Close;

    /// Attaches a SocketAdapter as a receiver; wires it to all four socket signals.
    /// @param adapter The adapter to attach; its priority determines signal ordering.
    virtual void addReceiver(SocketAdapter* adapter) override;

    /// Detaches a SocketAdapter from all four socket signals.
    /// @param adapter The adapter to detach.
    virtual void removeReceiver(SocketAdapter* adapter) override;

    /// Replaces the underlying socket with @p socket.
    ///
    /// Throws std::logic_error if the emitter already has an attached socket.
    /// @param socket The new socket to attach.
    virtual void swap(const Socket::Ptr& socket);

    /// Returns the underlying socket cast to type T, or nullptr if the cast fails.
    /// @tparam T  Derived socket type to cast to.
    /// @return Pointer to the socket as T, or nullptr on type mismatch.
    template <class T>
    T* as()
    {
        return dynamic_cast<T*>(impl.get());
    }

    /// Returns a raw pointer to the underlying socket for direct method access.
    /// Follows shared_ptr semantics; the caller must not delete the returned pointer.
    /// @return Raw pointer to the socket (never null if a socket was attached).
    Socket* operator->() const
    {
        return impl.get();
    }

    /// Pointer to the underlying socket.
    /// Sent data will be proxied to this socket.
    Socket::Ptr impl;

protected:
    /// Forwards the connect event to chained adapters, then fires the Connect signal.
    bool onSocketConnect(Socket& socket) override;
    /// Forwards the recv event to chained adapters, then fires the Recv signal.
    bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress) override;
    /// Forwards the error event to chained adapters, then fires the Error signal.
    bool onSocketError(Socket& socket, const icy::Error& error) override;
    /// Forwards the close event to chained adapters, then fires the Close signal.
    bool onSocketClose(Socket& socket) override;
};


} // namespace net
} // namespace icy


/// @\}
