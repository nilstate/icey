///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/server.h"
#include "../protocol.h"
#include "icy/logger.h"


namespace icy {
namespace smpl {

void Server::onMessage(ServerPeer& sender, json::Value msg)
{
    msg["from"] = proto::addressFor(sender.peer().user(), sender.id());

    if (msg.value("type", "") == "presence")
        proto::rewritePresence(msg["data"], sender.peer(), sender.id(), true);

    route(sender, msg);
}


} // namespace smpl
} // namespace icy
