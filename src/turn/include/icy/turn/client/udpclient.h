///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "icy/net/udpsocket.h"
#include "icy/stateful.h"
#include "icy/stun/transaction.h"
#include "icy/turn/client/client.h"
#include "icy/turn/iallocation.h"
#include "icy/turn/turn.h"

#include <deque>


namespace icy {
namespace turn {


class TURN_API UDPClient : public Client
{
public:
    UDPClient(ClientObserver& observer, const Options& options = Options());
    virtual ~UDPClient();
};
} // namespace turn
} // namespace icy


/// @\}
