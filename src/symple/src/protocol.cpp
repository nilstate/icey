///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "protocol.h"


namespace icy {
namespace smpl::proto {


std::string addressFor(std::string_view user, std::string_view id)
{
    if (id.empty())
        return std::string(user);
    if (user.empty())
        return std::string(id);
    return std::string(user) + "|" + std::string(id);
}


bool isReservedPresenceField(std::string_view key)
{
    return key == "id" ||
           key == "user" ||
           key == "online" ||
           key == "name" ||
           key == "type";
}


void mergePresenceExtras(json::Value& out, const json::Value* extra)
{
    if (!extra || !extra->is_object())
        return;

    for (const auto& [key, value] : extra->items()) {
        if (!isReservedPresenceField(key))
            out[key] = value;
    }
}


json::Value makeError(int status, std::string_view message)
{
    json::Value msg;
    msg["type"] = "error";
    msg["status"] = status;
    msg["message"] = std::string(message);
    return msg;
}


json::Value makeClose()
{
    json::Value msg;
    msg["type"] = "close";
    return msg;
}


json::Value makeRoomMessage(std::string_view type, std::string_view room)
{
    json::Value msg;
    msg["type"] = std::string(type);
    msg["room"] = std::string(room);
    return msg;
}


json::Value makeWelcome(const Peer& peer,
                        const std::unordered_set<std::string>& rooms,
                        int status,
                        std::string_view protocol)
{
    json::Value msg;
    msg["type"] = "welcome";
    msg["protocol"] = std::string(protocol);
    msg["peer"] = static_cast<const json::Value&>(peer);
    msg["status"] = status;
    msg["rooms"] = json::Value::array();
    for (const auto& room : rooms)
        msg["rooms"].push_back(room);
    return msg;
}


json::Value makeAuth(std::string_view user,
                     std::string_view name,
                     std::string_view token,
                     std::string_view peerType,
                     const std::unordered_set<std::string>& rooms)
{
    json::Value msg;
    msg["type"] = "auth";
    msg["user"] = std::string(user);
    if (!name.empty())
        msg["name"] = std::string(name);
    if (!token.empty())
        msg["token"] = std::string(token);
    if (!peerType.empty()) {
        json::Value data;
        data["peerType"] = std::string(peerType);
        msg["data"] = std::move(data);
    }
    if (!rooms.empty()) {
        msg["rooms"] = json::Value::array();
        for (const auto& room : rooms)
            msg["rooms"].push_back(room);
    }
    return msg;
}


json::Value makePresence(const Peer& peer,
                         std::string_view id,
                         bool online,
                         const json::Value* extra)
{
    json::Value msg;
    msg["type"] = "presence";
    msg["from"] = addressFor(peer.user(), id);

    json::Value data = json::Value::object();
    mergePresenceExtras(data, extra);
    rewritePresence(data, peer, id, online);
    msg["data"] = std::move(data);
    return msg;
}


void rewritePresence(json::Value& data,
                     const Peer& peer,
                     std::string_view id,
                     bool online)
{
    if (!data.is_object())
        data = json::Value::object();
    else {
        static constexpr std::string_view reserved[] = {
            "id", "user", "online", "name", "type"
        };
        for (auto key : reserved)
            data.erase(std::string(key));
    }

    data["id"] = std::string(id);
    data["user"] = peer.user();
    data["online"] = online;
    if (!peer.name().empty())
        data["name"] = peer.name();
    if (!peer.type().empty())
        data["type"] = peer.type();
}


std::optional<Welcome> parseWelcome(const json::Value& msg, std::string* error)
{
    Welcome welcome;
    welcome.status = msg.value("status", 200);
    if (welcome.status != 200) {
        welcome.message = msg.value("message", "Auth failed");
        return welcome;
    }

    auto peerIt = msg.find("peer");
    if (peerIt == msg.end() || !peerIt->is_object()) {
        if (error)
            *error = "Invalid welcome: missing peer data";
        return std::nullopt;
    }

    const auto& peerData = *peerIt;
    if (peerData.value("id", "").empty()) {
        if (error)
            *error = "Invalid welcome: missing peer ID";
        return std::nullopt;
    }
    if (!peerData.contains("user") || !peerData.contains("online")) {
        if (error)
            *error = "Invalid welcome: missing peer data";
        return std::nullopt;
    }

    welcome.peer = Peer(peerData);
    auto roomsIt = msg.find("rooms");
    if (roomsIt != msg.end() && roomsIt->is_array()) {
        for (const auto& room : *roomsIt) {
            if (room.is_string() && !room.get<std::string>().empty())
                welcome.rooms.insert(room.get<std::string>());
        }
    }

    return welcome;
}


std::optional<std::pair<int, std::string>> parseError(const json::Value& msg)
{
    if (msg.value("type", "") != "error")
        return std::nullopt;
    return std::pair<int, std::string>(
        msg.value("status", 500),
        msg.value("message", "Unknown error"));
}


std::optional<std::string> parseRoomMessage(const json::Value& msg,
                                            std::string_view type)
{
    if (msg.value("type", "") != type)
        return std::nullopt;

    auto room = msg.value("room", "");
    if (room.empty())
        return std::nullopt;
    return room;
}


} // namespace smpl::proto
} // namespace icy
