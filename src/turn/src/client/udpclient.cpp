///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/client/udpclient.h"
#include "icy/net/udpsocket.h"


namespace icy {
namespace turn {


UDPClient::UDPClient(ClientObserver& observer, const Options& options)
    : Client(observer, options, net::makeSocket<net::UDPSocket>())
{
    LTrace("Create");
}


UDPClient::~UDPClient()
{
    LTrace("Destroy");
}


} // namespace turn
} // namespace icy


/// @}
