///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "scy/turn/client/udpclient.h"
#include "scy/net/udpsocket.h"


namespace scy {
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
} // namespace scy


/// @\}
