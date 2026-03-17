///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "scy/net/udpsocket.h"
#include "scy/stateful.h"
#include "scy/stun/transaction.h"
#include "scy/turn/client/client.h"
#include "scy/turn/iallocation.h"
#include "scy/turn/turn.h"

#include <deque>


namespace scy {
namespace turn {


class TURN_API UDPClient : public Client
{
public:
    UDPClient(ClientObserver& observer, const Options& options = Options());
    virtual ~UDPClient();
};
} // namespace turn
} // namespace scy


/// @\}
