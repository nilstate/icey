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


namespace icy {
namespace smpl {


// ---------------------------------------------------------------------------
// ServerPeer
// ---------------------------------------------------------------------------

ServerPeer::ServerPeer(http::ServerConnection& conn)
    : _conn(conn)
{
}


void ServerPeer::send(const json::Value& msg)
{
    auto str = msg.dump();
    sendSerialized(str.c_str(), str.size());
}


void ServerPeer::sendSerialized(const char* data, size_t len)
{
    try {
        _conn.send(data, len, http::ws::Text);
    }
    catch (const std::exception& e) {
        LWarn("ServerPeer send failed: ", e.what());
    }
}


void ServerPeer::join(const std::string& room)
{
    _rooms.insert(room);
}


void ServerPeer::leave(const std::string& room)
{
    _rooms.erase(room);
}


void ServerPeer::leaveAll()
{
    _rooms.clear();
}


std::string ServerPeer::id() const
{
    return _peer.id();
}


Server::Server(uv::Loop* loop)
    : _loop(loop)
    , _peerRegistry(std::make_unique<PeerRegistry>())
    , _roomIndex(std::make_unique<RoomIndex>())
{
}


Server::~Server()
{
    shutdown();
}


void Server::shutdown()
{
    if (_shuttingDown)
        return;
    _shuttingDown = true;

    // Broadcast shutdown notice to all connected peers
    {
        std::lock_guard lock(_mutex);
        json::Value notice;
        notice["type"] = "event";
        notice["subtype"] = "shutdown";
        notice["message"] = "Server shutting down";
        auto str = notice.dump();
        for (const auto& [id, peer] : _peerRegistry->peers()) {
            try {
                peer->connection().send(
                    str.c_str(), str.size(), http::ws::Text);
            } catch (...) {}
        }
    }

    // Shut down the HTTP server. This closes the listen socket
    // and emits Shutdown. Connection cleanup happens asynchronously
    // via uv_close callbacks.
    if (_http)
        _http->shutdown();

    // Clear all state. Responder::onClose is guarded by _shuttingDown
    // so it won't try to access these collections.
    {
        std::lock_guard lock(_mutex);
        _roomIndex->clear();
        _peerRegistry->clear();
    }

    // Release the HTTP server. Its destructor calls shutdown() again
    // (no-op due to _shuttingDown guard on the http side - the socket
    // is already closed). Connections and responders are destroyed here.
    _http.reset();

    LInfo("Symple server shut down");
}


ServerPeer* Server::getPeer(const std::string& id)
{
    std::lock_guard lock(_mutex);
    return _peerRegistry->find(id);
}


std::vector<ServerPeer*> Server::getPeersInRoom(const std::string& room)
{
    std::lock_guard lock(_mutex);
    std::vector<ServerPeer*> result;
    if (const auto* members = _roomIndex->members(room)) {
        result.reserve(members->size());
        for (const auto& peerId : *members) {
            if (auto* peer = _peerRegistry->find(peerId))
                result.push_back(peer);
        }
    }
    return result;
}


size_t Server::peerCount() const
{
    std::lock_guard lock(_mutex);
    return _peerRegistry->size();
}


void Server::addVirtualPeer(const Peer& peer,
                            const std::vector<std::string>& rooms,
                            std::function<void(const json::Value&)> handler)
{
    std::lock_guard lock(_mutex);

    std::string id = peer.id();
    std::string user = peer.user();

    VirtualPeer vp;
    vp.peer = peer;
    vp.handler = std::move(handler);

    // Join user room (same as real peers)
    vp.rooms.insert(user);
    _roomIndex->join(user, id);

    // Join requested rooms
    for (const auto& room : rooms) {
        if (!room.empty()) {
            vp.rooms.insert(room);
            _roomIndex->join(room, id);
        }
    }

    _peerRegistry->addVirtual(id, std::move(vp));

    // Broadcast online presence
    if (auto* virtualPeer = _peerRegistry->findVirtual(id))
        broadcastRooms(virtualPeer->rooms, detail::makePeerPresence(peer, id, true), id);

    LInfo("Virtual peer registered: ", user, "|", id);
}


void Server::removeVirtualPeer(const std::string& peerId)
{
    std::lock_guard lock(_mutex);

    auto* virtualPeer = _peerRegistry->findVirtual(peerId);
    if (!virtualPeer)
        return;

    std::string user = virtualPeer->peer.user();

    // Broadcast offline presence
    broadcastRooms(virtualPeer->rooms,
                   detail::makePeerPresence(virtualPeer->peer, peerId, false),
                   peerId);

    _roomIndex->leaveAll(peerId);
    _peerRegistry->eraseVirtual(peerId);

    LInfo("Virtual peer removed: ", user, "|", peerId);
}


} // namespace smpl
} // namespace icy
