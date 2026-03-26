///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "rosterstate.h"


namespace icy::smpl::detail {


PresenceUpdate applyPresenceData(Roster& roster, const json::Value& data)
{
    PresenceUpdate update;

    try {
        if (!data.is_object() ||
            !data.contains("id") ||
            !data.contains("user") ||
            !data.contains("online")) {
            update.kind = PresenceUpdate::Kind::Invalid;
            update.error = "Bad presence data: " + data.dump();
            return update;
        }

        std::string id = data["id"].get<std::string>();
        bool online = data["online"].get<bool>();

        auto* peer = roster.get(id);
        if (online) {
            if (!peer) {
                roster.add(id, std::make_unique<Peer>(data));
                update.livePeer = roster.get(id);
                update.kind = PresenceUpdate::Kind::Connected;
                return update;
            }

            static_cast<json::Value&>(*peer) = data;
            update.livePeer = peer;
            return update;
        }

        if (peer) {
            update.snapshot = *peer;
            roster.erase(id);
            update.kind = PresenceUpdate::Kind::Disconnected;
        }
    }
    catch (const std::exception&) {
        update.kind = PresenceUpdate::Kind::Invalid;
        update.error = "Bad presence data: " + data.dump();
    }

    return update;
}


} // namespace icy::smpl::detail
