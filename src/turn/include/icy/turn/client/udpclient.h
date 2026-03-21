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


/// UDP transport TURN client for datagram-based relay allocations.
/// Uses a single UDP socket for both STUN signalling and Send/Data Indications.
/// All Client methods apply; data is relayed via sendData() using Send Indications.
class TURN_API UDPClient : public Client
{
public:
    /// @param observer Observer for allocation lifecycle and data relay events.
    /// @param options  Client configuration; defaults to loopback server, 5-min lifetime.
    UDPClient(ClientObserver& observer, const Options& options = Options());
    virtual ~UDPClient();
};
} // namespace turn
} // namespace icy


/// @\}
