///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{


#include "scy/socketio/transaction.h"
#include "scy/logger.h"
#include "scy/socketio/client.h"


namespace scy {
namespace sockio {


Transaction::Transaction(Client& client, long timeout)
    : PacketTransaction<Packet>(timeout, 0, client.ws().socket->loop())
    , client(client)
{
    LTrace("Create");
}


Transaction::Transaction(Client& client, const Packet& request, long timeout)
    : PacketTransaction<Packet>(request, timeout, 0, client.ws().socket->loop())
    , client(client)
{
    LTrace("Create");
}


Transaction::~Transaction()
{
    LTrace("Destroy");
}


bool Transaction::send()
{
    LTrace("Send: ", _request.id());
    _request.setAck(true);
    client += packetSlot(this, &Transaction::onPotentialResponse, -1, 100);
    if (client.send(_request))
        return PacketTransaction<Packet>::send();
    return false;
}


void Transaction::onPotentialResponse(sockio::Packet& packet)
{
    LTrace("On potential response: ", packet.id());
    PacketTransaction<Packet>::handlePotentialResponse(packet);
}


bool Transaction::checkResponse(const Packet& packet)
{
    LTrace("Check response: ", packet.id());
    return _request.id() == packet.id();
}


void Transaction::onResponse()
{
    LTrace("On success");
    client -= packetSlot(this, &Transaction::onPotentialResponse);
    PacketTransaction<Packet>::onResponse();
}


} // namespace sockio
} // namespace scy


/// @}
