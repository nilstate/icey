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
#include "icy/net/socket.h"
#include "icy/net/socketemitter.h"
#include "icy/packetfactory.h"
#include "icy/packetsignal.h"


namespace icy {
namespace net {


//
// Packet Socket Emitter
//


/// Socket adapter that emits received data as packets
class Net_API PacketSocketEmitter : public SocketEmitter
    , public PacketSignal
{
public:
    /// Creates the PacketSocketEmitter and attaches it to the given socket.
    ///
    /// The emitter should be assigned a higher priority than plain socket
    /// adapters so that packet parsing occurs before generic data callbacks.
    /// Packets are created and dispatched using the registered factory strategies;
    /// strategies with the highest priority are tried first.
    /// @param socket Optional socket to attach to immediately.
    PacketSocketEmitter(const Socket::Ptr& socket = nullptr);

    virtual ~PacketSocketEmitter() noexcept;

    /// Parses raw received data into packets via the factory and forwards each
    /// parsed packet to onPacket(). Returns true if propagation should stop.
    /// @param socket      The socket that received the data.
    /// @param buffer      The raw received data buffer.
    /// @param peerAddress The sender's address.
    /// @return true if the event was consumed and should not propagate further.
    virtual bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress) override;

    /// Process a parsed packet. Returns true to stop propagation.
    virtual bool onPacket(IPacket& pkt);

    /// The packet factory.
    PacketFactory factory;
};


} // namespace net
} // namespace icy


/// @}
