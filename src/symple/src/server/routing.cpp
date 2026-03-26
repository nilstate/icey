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
#include "icy/logger.h"


namespace icy {
namespace smpl {


void Server::route(ServerPeer& sender, const json::Value& msg)
{
    // Caller holds _mutex.

    if (!msg.contains("to")) {
        broadcastRooms(sender.rooms(), msg, sender.id());
        return;
    }

    const auto& to = msg["to"];

    if (to.is_string()) {
        std::string addr = to.get<std::string>();
        auto pos = addr.find('|');

        if (pos != std::string::npos) {
            std::string peerId = addr.substr(pos + 1);

            if (auto* target = _peerRegistry->find(peerId)) {
                if (RoutingPolicy::canDirectMessage(sender, *target))
                    deliver(peerId, msg);
                else
                    LDebug("Blocked direct message from ", sender.id(), " to ", peerId, " (no shared room)");
                return;
            }

            if (auto* target = _peerRegistry->findVirtual(peerId)) {
                if (RoutingPolicy::canDirectMessage(sender, *target))
                    deliver(peerId, msg);
                else
                    LDebug("Blocked direct message from ", sender.id(), " to virtual peer ", peerId, " (no shared room)");
            }
            return;
        }

        const auto* members = _roomIndex->members(addr);
        if (!members)
            return;

        auto str = msg.dump();
        for (const auto& peerId : *members) {
            if (peerId == sender.id())
                continue;

            if (auto* target = _peerRegistry->find(peerId)) {
                if (!RoutingPolicy::canDirectMessage(sender, *target)) {
                    LDebug("Blocked direct message from ", sender.id(), " to ", peerId, " (no shared room)");
                    continue;
                }
                deliverSerialized(peerId, str.c_str(), str.size(), msg);
                continue;
            }

            if (auto* target = _peerRegistry->findVirtual(peerId)) {
                if (!RoutingPolicy::canDirectMessage(sender, *target)) {
                    LDebug("Blocked direct message from ", sender.id(), " to virtual peer ", peerId, " (no shared room)");
                    continue;
                }
                deliverSerialized(peerId, str.c_str(), str.size(), msg);
            }
        }
        return;
    }

    if (!to.is_array())
        return;

    std::unordered_set<std::string> rooms;
    for (const auto& room : to) {
        if (!room.is_string())
            continue;

        auto roomName = room.get<std::string>();
        if (RoutingPolicy::canBroadcastToRoom(sender, roomName))
            rooms.insert(roomName);
        else
            LDebug("Blocked broadcast from ", sender.id(), " to room ", roomName, " (not a member)");
    }
    broadcastRooms(rooms, msg, sender.id());
}


void Server::broadcastRooms(const std::unordered_set<std::string>& rooms,
                            const json::Value& msg,
                            const std::string& excludeId)
{
    auto recipients = _roomIndex->collectRecipients(rooms, excludeId);

    auto str = msg.dump();
    for (const auto& peerId : recipients)
        deliverSerialized(peerId, str.c_str(), str.size(), msg);
}


void Server::broadcast(const std::string& room, const json::Value& msg,
                       const std::string& excludeId)
{
    const auto* members = _roomIndex->members(room);
    if (!members)
        return;

    auto str = msg.dump();
    for (const auto& peerId : *members) {
        if (peerId != excludeId)
            deliverSerialized(peerId, str.c_str(), str.size(), msg);
    }
}


bool Server::sendTo(const std::string& peerId, const json::Value& msg)
{
    return deliver(peerId, msg);
}


bool Server::sendToUser(const std::string& user, const json::Value& msg)
{
    const auto* members = _roomIndex->members(user);
    if (!members)
        return false;

    bool sent = false;
    auto str = msg.dump();
    for (const auto& peerId : *members) {
        if (deliverSerialized(peerId, str.c_str(), str.size(), msg))
            sent = true;
    }
    return sent;
}


bool Server::deliver(const std::string& peerId, const json::Value& msg)
{
    if (_peerRegistry->find(peerId)) {
        auto str = msg.dump();
        return deliverSerialized(peerId, str.c_str(), str.size(), msg);
    }

    if (auto* peer = _peerRegistry->findVirtual(peerId)) {
        try {
            peer->handler(msg);
            return true;
        }
        catch (const std::exception& e) {
            LWarn("Virtual peer delivery failed: ", peerId, ": ", e.what());
        }
    }

    return false;
}


bool Server::deliverSerialized(const std::string& peerId,
                               const char* data,
                               size_t len,
                               const json::Value& msg)
{
    if (auto* peer = _peerRegistry->find(peerId)) {
        try {
            peer->sendSerialized(data, len);
            return true;
        }
        catch (const std::exception& e) {
            LWarn("Delivery failed to ", peerId, ": ", e.what());
            return false;
        }
    }

    if (auto* peer = _peerRegistry->findVirtual(peerId)) {
        try {
            peer->handler(msg);
            return true;
        }
        catch (const std::exception& e) {
            LWarn("Virtual peer delivery failed: ", peerId, ": ", e.what());
            return false;
        }
    }

    return false;
}


} // namespace smpl
} // namespace icy
