///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/server.h"
#include "detail.h"
#include "../protocol.h"
#include "presence.h"


namespace icy {
namespace smpl::detail {


json::Value makePeerPresence(const Peer& peer,
                             std::string_view id,
                             bool online,
                             const json::Value* extra)
{
    return proto::makePresence(peer, id, online, extra ? extra : &peer);
}


} // namespace smpl::detail

namespace smpl {


void Server::sendPresenceSnapshot(ServerPeer& recipient,
                                  const std::unordered_set<std::string>& rooms,
                                  std::string_view excludeId)
{
    auto recipients = _roomIndex->collectRecipients(rooms, excludeId);

    for (const auto& peerId : recipients) {
        if (auto* peer = _peerRegistry->find(peerId)) {
            recipient.send(detail::makePeerPresence(peer->peer(), peer->id(), true));
            continue;
        }

        if (auto* virtualPeer = _peerRegistry->findVirtual(peerId))
            recipient.send(detail::makePeerPresence(virtualPeer->peer, virtualPeer->peer.id(), true));
    }
}


} // namespace smpl
} // namespace icy
