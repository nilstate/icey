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


class Net_API PacketSocketEmitter : public SocketEmitter
    , public PacketSignal
{
public:
    /// Creates the PacketSocketEmitter
    /// This class should have a higher priority than standard
    /// sockets so we can parse data packets first.
    /// Creates and dispatches a packet utilizing the available
    /// creation strategies. For best performance the most used
    /// strategies should have the highest priority.
    PacketSocketEmitter(const Socket::Ptr& socket = nullptr);

    virtual ~PacketSocketEmitter() noexcept;

    virtual bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress) override;

    /// Process a parsed packet. Returns true to stop propagation.
    virtual bool onPacket(IPacket& pkt);

    /// The packet factory.
    PacketFactory factory;
};


} // namespace net
} // namespace icy


/// @\}
