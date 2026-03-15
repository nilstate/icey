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
#include "scy/net/socket.h"
#include "scy/net/socketemitter.h"
#include "scy/packetfactory.h"
#include "scy/packetsignal.h"


namespace scy {
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
} // namespace scy


/// @\}
