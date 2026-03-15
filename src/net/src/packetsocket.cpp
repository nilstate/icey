///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/packetsocket.h"


using std::endl;


namespace scy {
namespace net {


//
// Packet Socket Adapter
//


PacketSocketEmitter::PacketSocketEmitter(const Socket::Ptr& socket)
    : SocketEmitter(socket)
{
    // LTrace("Create: ", socket);
}


PacketSocketEmitter::~PacketSocketEmitter() noexcept
{
}


bool PacketSocketEmitter::onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
{
    // LTrace("Recv: ", buffer.size());

    const char* buf = bufferCast<const char*>(buffer);
    size_t len = buffer.size();
    size_t nread = 0;
    IPacket* raw = nullptr;
    while (len > 0 && (raw = factory.createPacket(constBuffer(buf, len), nread))) {
        if (nread <= 0)
            return false;
        std::unique_ptr<IPacket> pkt(raw);
        pkt->info = std::make_unique<PacketInfo>(this->impl, peerAddress);
        if (onPacket(*pkt))
            return true;
        buf += nread;
        len -= nread;
    }
    return false;
}


bool PacketSocketEmitter::onPacket(IPacket& pkt)
{
    // LTrace("On packet: ", pkt.size());
    PacketSignal::emit(pkt);
    return false;
}


} // namespace net
} // namespace scy


/// @\}
