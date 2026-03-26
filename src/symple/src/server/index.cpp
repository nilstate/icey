///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/server.h"
#include "detail.h"
#include "routingdetail.h"


namespace icy {
namespace smpl {


void Server::PeerRegistry::add(std::string id, std::unique_ptr<ServerPeer> peer)
{
    _peers[std::move(id)] = std::move(peer);
}


void Server::PeerRegistry::addVirtual(std::string id, VirtualPeer peer)
{
    _virtualPeers[std::move(id)] = std::move(peer);
}


void Server::PeerRegistry::bind(http::ServerConnection& conn, const std::string& id)
{
    _connToPeer[&conn] = id;
}


void Server::PeerRegistry::unbind(http::ServerConnection& conn)
{
    _connToPeer.erase(&conn);
}


void Server::PeerRegistry::erase(const std::string& id)
{
    for (auto it = _connToPeer.begin(); it != _connToPeer.end();) {
        if (it->second == id)
            it = _connToPeer.erase(it);
        else
            ++it;
    }
    _peers.erase(id);
}


void Server::PeerRegistry::eraseVirtual(const std::string& id)
{
    _virtualPeers.erase(id);
}


void Server::PeerRegistry::clear()
{
    _connToPeer.clear();
    _peers.clear();
    _virtualPeers.clear();
}


ServerPeer* Server::PeerRegistry::find(const std::string& id)
{
    auto it = _peers.find(id);
    return it != _peers.end() ? it->second.get() : nullptr;
}


const ServerPeer* Server::PeerRegistry::find(const std::string& id) const
{
    auto it = _peers.find(id);
    return it != _peers.end() ? it->second.get() : nullptr;
}


Server::VirtualPeer* Server::PeerRegistry::findVirtual(const std::string& id)
{
    auto it = _virtualPeers.find(id);
    return it != _virtualPeers.end() ? &it->second : nullptr;
}


const Server::VirtualPeer* Server::PeerRegistry::findVirtual(const std::string& id) const
{
    auto it = _virtualPeers.find(id);
    return it != _virtualPeers.end() ? &it->second : nullptr;
}


ServerPeer* Server::PeerRegistry::findByConnection(http::ServerConnection& conn)
{
    auto it = _connToPeer.find(&conn);
    return it != _connToPeer.end() ? find(it->second) : nullptr;
}


const ServerPeer* Server::PeerRegistry::findByConnection(http::ServerConnection& conn) const
{
    auto it = _connToPeer.find(&conn);
    return it != _connToPeer.end() ? find(it->second) : nullptr;
}


size_t Server::PeerRegistry::size() const
{
    return _peers.size() + _virtualPeers.size();
}


void Server::RoomIndex::join(const std::string& room, const std::string& peerId)
{
    _rooms[room].insert(peerId);
}


void Server::RoomIndex::leave(const std::string& room, const std::string& peerId)
{
    auto it = _rooms.find(room);
    if (it == _rooms.end())
        return;

    it->second.erase(peerId);
    if (it->second.empty())
        _rooms.erase(it);
}


void Server::RoomIndex::leaveAll(const std::string& peerId)
{
    for (auto it = _rooms.begin(); it != _rooms.end();) {
        it->second.erase(peerId);
        if (it->second.empty())
            it = _rooms.erase(it);
        else
            ++it;
    }
}


void Server::RoomIndex::clear()
{
    _rooms.clear();
}


const Server::RoomIndex::MemberSet* Server::RoomIndex::members(const std::string& room) const
{
    auto it = _rooms.find(room);
    return it != _rooms.end() ? &it->second : nullptr;
}


std::unordered_set<std::string> Server::RoomIndex::collectRecipients(
    const std::unordered_set<std::string>& rooms,
    std::string_view excludeId) const
{
    return detail::collectRecipients(_rooms, rooms, excludeId);
}


bool Server::RoutingPolicy::sharesAnyRoom(const std::unordered_set<std::string>& a,
                                          const std::unordered_set<std::string>& b)
{
    return detail::sharesAnyRoom(a, b);
}


bool Server::RoutingPolicy::canDirectMessage(const ServerPeer& sender,
                                             const ServerPeer& recipient)
{
    return sharesAnyRoom(sender.rooms(), recipient.rooms());
}


bool Server::RoutingPolicy::canDirectMessage(const ServerPeer& sender,
                                             const VirtualPeer& recipient)
{
    return sharesAnyRoom(sender.rooms(), recipient.rooms);
}


bool Server::RoutingPolicy::canBroadcastToRoom(const ServerPeer& sender,
                                               const std::string& room)
{
    return sender.rooms().count(room) > 0;
}


} // namespace smpl
} // namespace icy
