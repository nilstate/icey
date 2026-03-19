///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#include "icy/stun/transaction.h"
#include "icy/logger.h"
#include "icy/net/socket.h"
#include <iostream>


namespace icy {
namespace stun {


Transaction::Transaction(const net::Socket::Ptr& socket,
                         const net::Address& peerAddress, long timeout, int retries)
    : net::Transaction<Message>(socket, peerAddress, timeout, retries)
{
    LDebug("Create");

    // Register STUN message creation strategy
    net::Transaction<Message>::factory.registerPacketType<stun::Message>(0);
}


Transaction::~Transaction()
{
    LDebug("Destroy");
}


bool Transaction::checkResponse(const Message& message)
{
    return net::Transaction<Message>::checkResponse(message) &&
           _request.transactionID() == message.transactionID();
}


void Transaction::onResponse()
{
    LDebug("On response");

    _response.setMethod(_request.methodType());
    _response.setClass(Message::SuccessResponse);
    if (_response.get<stun::ErrorCode>() != nullptr)
        _response.setClass(Message::ErrorResponse);
    else if (_response.methodType() == Message::SendIndication ||
             _response.methodType() == Message::DataIndication)
        _response.setClass(Message::Indication);

    net::Transaction<Message>::onResponse();
}
} // namespace stun
} // namespace icy


/// @\}
