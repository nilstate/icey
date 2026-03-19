///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#pragma once


#include "icy/net/socket.h"
#include "icy/net/transaction.h"
#include "icy/stun/message.h"


namespace icy {
namespace stun {


class STUN_API Transaction : public net::Transaction<Message>
{
public:
    Transaction(const net::Socket::Ptr& socket, const net::Address& peerAddress,
                long timeout = 10000, int retries = 1);

    bool checkResponse(const Message& message) override;
    void onResponse() override;

protected:
    virtual ~Transaction();
};
} // namespace stun
} // namespace icy


/// @\}
