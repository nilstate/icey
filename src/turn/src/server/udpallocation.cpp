///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/server/udpallocation.h"
#include "icy/buffer.h"
#include "icy/logger.h"
#include "icy/net/udpsocket.h"
#include "icy/turn/server/server.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>


namespace icy {
namespace turn {


UDPAllocation::UDPAllocation(Server& server, const FiveTuple& tuple,
                             const std::string& username,
                             const uint32_t& lifetime)
    : ServerAllocation(server, tuple, username, lifetime)
    , _relaySocket(net::makeSocket<net::UDPSocket>())
{
    // Handle data from the relay socket directly from the allocation.
    // This will remove the need for allocation lookups when receiving
    // data from peers.
    _relaySocket->bind(net::Address(server.options().listenAddr.host(), 0));
    _relaySocket.Recv += slot(this, &UDPAllocation::onPeerDataReceived);

    LTrace(" Initializing on address: ", _relaySocket->address());
}


UDPAllocation::~UDPAllocation()
{
    LTrace("Destroy");
    _relaySocket.Recv -= slot(this, &UDPAllocation::onPeerDataReceived);
    _relaySocket->close();
}


bool UDPAllocation::handleRequest(Request& request)
{
    LTrace("Handle Request");

    if (!ServerAllocation::handleRequest(request)) {
        if (request.methodType() == stun::Message::SendIndication)
            handleSendIndication(request);
        else
            return false;
    }

    return true;
}


void UDPAllocation::handleSendIndication(Request& request)
{
    LTrace("Handle Send Indication");

    auto peerAttr = request.get<stun::XorPeerAddress>();
    if (!peerAttr || peerAttr->family() == stun::AddressFamily::Undefined) {
        LError("Send Indication error: No Peer Address");
        // silently discard...
        return;
    }

    auto dataAttr = request.get<stun::Data>();
    if (!dataAttr) {
        LError("Send Indication error: No Data attribute");
        // silently discard...
        return;
    }

    net::Address peerAddress = peerAttr->address();
    if (!hasPermission(peerAddress)) {
        SError << "Send Indication error: No permission for: "
               << peerAddress.host();
        // silently discard...
        return;
    }

    STrace << "Relaying Send Indication: "
           << "\r\tFrom: " << request.remoteAddress.toString()
           << "\r\tTo: " << peerAddress;

    if (send(dataAttr->bytes(), dataAttr->size(), peerAddress) == -1) {
        _server.respondError(request, kErrorAllocationQuotaReached, "Allocation Quota Reached");
        // Signal the server to delete this allocation instead of `delete this`
        _deleted = true;
    }
}


bool UDPAllocation::onPeerDataReceived(net::Socket&,
                                       const MutableBuffer& buffer,
                                       const net::Address& peerAddress)
{
    LTrace("Received UDP Datagram from ", peerAddress);

    if (!hasPermission(peerAddress)) {
        LTrace("No Permission: ", peerAddress.host());
        return false;
    }

    updateUsage(buffer.size());

    // Check that we have not exceeded out lifetime and bandwidth quota.
    if (IAllocation::deleted())
        return false;

    stun::Message message(stun::Message::Indication,
                          stun::Message::DataIndication);

    // XOR-PEER-ADDRESS must contain the actual peer transport address
    // so the client knows which peer sent this data.
    message.add<stun::XorPeerAddress>().setAddress(peerAddress);
    message.add<stun::Data>().copyBytes(
        bufferCast<const char*>(buffer), buffer.size());

    STrace << "Send data indication:"
           << "\n\tFrom: " << peerAddress << "\n\tTo: " << _tuple.remote()
          ;

    server().udpSocket().sendPacket(message, _tuple.remote());
    return false;
}


ssize_t UDPAllocation::send(const char* data, size_t size,
                            const net::Address& peerAddress)
{
    updateUsage(size);

    // Check that we have not exceeded our lifetime and bandwidth quota.
    if (IAllocation::deleted()) {
        LWarn("Send indication dropped: Allocation quota reached");
        return -1;
    }

    return _relaySocket.send(data, size, peerAddress);
}


net::Address UDPAllocation::relayedAddress() const
{
    return _relaySocket->address();
}


} // namespace turn
} // namespace icy


/// @\}
