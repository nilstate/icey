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


#include "icy/net/packetsocket.h"
#include "icy/packettransaction.h"

#include <stdexcept>


namespace icy {
namespace net {


/// Request/response helper for packet types emitted from a socket.
template <class PacketT>
class Net_API Transaction : public PacketTransaction<PacketT>
    , public PacketSocketEmitter
{
public:
    using BaseT = PacketTransaction<PacketT>;

    /// Constructs a Transaction on the given socket targeting @p peerAddress.
    /// @param socket      The socket to send/receive packets on.
    /// @param peerAddress The remote address for the request and response matching.
    /// @param timeout     Milliseconds to wait for a response before failing.
    /// @param retries     Number of additional send attempts on timeout.
    Transaction(const net::Socket::Ptr& socket, const Address& peerAddress,
                int timeout = 10000, int retries = 1)
        : PacketTransaction<PacketT>(timeout, retries, socket->loop())
        , PacketSocketEmitter(socket)
        , _peerAddress(peerAddress)
    {
        LTrace("Create");
    }

    /// Sends the request packet to the peer address and starts the timeout timer.
    /// Sets state to Failed and returns false if the packet could not be sent.
    /// @return true if the packet was sent and the timer started successfully.
    virtual bool send() override
    {
        LTrace("Send");
        if (impl->sendPacket(BaseT::_request, _peerAddress) > 0)
            return BaseT::send();
        BaseT::setState(this, TransactionState::Failed);
        return false;
    }

    /// Cancels the transaction and stops the timeout timer.
    virtual void cancel() override
    {
        LTrace("Cancel");
        BaseT::cancel();
    }

    /// Stops the timer and unregisters callbacks.
    virtual void dispose() override
    {
        LTrace("Dispose");
        BaseT::dispose();
    }

    /// Returns the remote peer address used for this transaction.
    Address peerAddress() const
    {
        return _peerAddress;
    }

protected:
    template <typename U> friend class icy::IntrusivePtr;

    virtual ~Transaction() = default;

    /// Checks whether @p packet is a matching response for the pending request.
    /// If it matches, the transaction completes; socket data propagation stops.
    /// @param packet The received packet to test.
    /// @return true to stop further propagation of the socket data event.
    virtual bool onPacket(IPacket& packet) override
    {
        LTrace("On packet: ", packet.size());
        // Stop socket data propagation if we handled the packet
        return BaseT::handlePotentialResponse(static_cast<PacketT&>(packet));
    }

    /// Called when a confirmed response is received; emits the response via PacketSignal.
    virtual void onResponse() override
    {
        LTrace("On success: ", BaseT::_response.toString());
        PacketSignal::emit(BaseT::_response);
    }

    /// Returns true if @p packet is a valid response for this transaction.
    ///
    /// The base implementation matches the local socket address against the
    /// PacketInfo socket address and the stored peer address against the
    /// PacketInfo peer address. Subclasses may override for stricter matching.
    /// @param packet The candidate response packet.
    /// @return true if the packet satisfies the response criteria.
    virtual bool checkResponse(const PacketT& packet) override
    {
        if (!packet.info)
            throw std::logic_error("Transaction::checkResponse: socket must provide packet info");
        auto info = static_cast<net::PacketInfo*>(packet.info.get());
        return impl->address() == info->socket->address() && _peerAddress == info->peerAddress;
    }

    Address _peerAddress;
};


} // namespace net
} // namespace icy


/// @}
