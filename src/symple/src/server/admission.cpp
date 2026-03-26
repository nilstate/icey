///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/server.h"
#include "detail.h"
#include "../protocol.h"
#include "presence.h"
#include "icy/logger.h"
#include "icy/util.h"


namespace icy {
namespace smpl {

namespace {

Peer makeAuthenticatedPeer(const json::Value& msg, const std::string& id)
{
    Peer peer;
    peer["id"] = id;
    peer["user"] = msg.value("user", "");
    peer["name"] = msg.value("name", msg.value("user", ""));
    peer["online"] = true;

    proto::mergePresenceExtras(
        peer,
        msg.contains("data") && msg["data"].is_object() ? &msg["data"] : nullptr);

    return peer;
}

} // namespace


void Server::onAuth(ServerPeer& tempPeer,
                    const json::Value& msg,
                    std::unique_lock<std::mutex>& lock)
{
    std::string user = msg.value("user", "");
    if (user.empty()) {
        tempPeer.send(proto::makeError(401, "Missing user field"));
        return;
    }

    std::string id = util::randomString(16);
    std::vector<std::string> authRooms;

    if (_opts.authentication || Authenticate.nslots() > 0) {
        bool allowed = !_opts.authentication;
        lock.unlock();
        try {
            Authenticate.emit(tempPeer, msg, allowed, authRooms);
            lock.lock();
        }
        catch (...) {
            lock.lock();
            throw;
        }
        if (!allowed) {
            tempPeer.send(proto::makeError(401, "Authentication failed"));
            return;
        }
    }

    auto peer = std::make_unique<ServerPeer>(tempPeer.connection());
    peer->setPeer(makeAuthenticatedPeer(msg, id));
    peer->setAuthenticated(true);
    peer->setRateLimit(_opts.rateLimit, _opts.rateSeconds);

    peer->join(peer->peer().user());
    _roomIndex->join(peer->peer().user(), id);

    if (msg.contains("rooms") && msg["rooms"].is_array()) {
        for (const auto& room : msg["rooms"]) {
            if (!room.is_string())
                continue;

            auto roomName = room.get<std::string>();
            if (roomName.empty())
                continue;

            peer->join(roomName);
            _roomIndex->join(roomName, id);
        }
    }

    for (const auto& room : authRooms) {
        if (room.empty())
            continue;
        peer->join(room);
        _roomIndex->join(room, id);
    }

    auto* registeredPeer = peer.get();
    _peerRegistry->bind(tempPeer.connection(), id);
    _peerRegistry->add(id, std::move(peer));

    tempPeer.setAuthenticated(true);

    registeredPeer->send(proto::makeWelcome(registeredPeer->peer(), registeredPeer->rooms()));
    sendPresenceSnapshot(*registeredPeer, registeredPeer->rooms(), id);

    LInfo("Peer authenticated: ", user, "|", id);

    broadcastRooms(
        registeredPeer->rooms(),
        detail::makePeerPresence(registeredPeer->peer(), id, true),
        id);

    lock.unlock();
    try {
        PeerConnected.emit(*registeredPeer);
        lock.lock();
    }
    catch (...) {
        lock.lock();
        throw;
    }
}


void Server::onJoin(ServerPeer& peer, const std::string& room)
{
    if (room.empty())
        return;

    if (!_opts.dynamicRooms) {
        peer.send(proto::makeError(400, "Dynamic rooms disabled"));
        return;
    }

    peer.join(room);
    _roomIndex->join(room, peer.id());

    peer.send(proto::makeRoomMessage("join:ok", room));
    sendPresenceSnapshot(peer, std::unordered_set<std::string>{room}, peer.id());

    LDebug("Peer ", peer.id(), " joined room: ", room);
}


void Server::onLeave(ServerPeer& peer, const std::string& room)
{
    if (room.empty())
        return;

    if (!_opts.dynamicRooms) {
        peer.send(proto::makeError(400, "Dynamic rooms disabled"));
        return;
    }

    peer.leave(room);
    _roomIndex->leave(room, peer.id());

    peer.send(proto::makeRoomMessage("leave:ok", room));

    LDebug("Peer ", peer.id(), " left room: ", room);
}


void Server::onDisconnect(ServerPeer& peer, std::unique_lock<std::mutex>& lock)
{
    std::string id = peer.id();
    std::string user = peer.peer().user();

    LInfo("Peer disconnected: ", user, "|", id);

    broadcastRooms(peer.rooms(), detail::makePeerPresence(peer.peer(), id, false), id);

    _roomIndex->leaveAll(id);
    _peerRegistry->unbind(peer.connection());

    lock.unlock();
    try {
        PeerDisconnected.emit(peer);
        lock.lock();
    }
    catch (...) {
        lock.lock();
        throw;
    }

    _peerRegistry->erase(id);
}


} // namespace smpl
} // namespace icy
