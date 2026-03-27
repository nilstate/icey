///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/client.h"
#include "../protocol.h"
#include "detail.h"
#include "roomstate.h"
#include "icy/logger.h"
#include "icy/util.h"

#include <algorithm>
#include <stdexcept>


namespace icy {
namespace smpl {


ssize_t Client::send(Message& m)
{
    if (!isOnline())
        throw std::runtime_error("Cannot send message while offline");

    auto peer = ourPeer();
    if (!peer)
        throw std::runtime_error("No active peer session");

    m.setFrom(peer->address());

    auto toId = m.toId();
    if (!toId.empty() && toId == m.fromId())
        throw std::runtime_error("Sender and recipient cannot match");

    if (!m.valid())
        throw std::runtime_error("Cannot send invalid message");

    return sendJson(m);
}


ssize_t Client::send(const std::string& data)
{
    if (!isOnline())
        throw std::runtime_error("Cannot send message while offline");

    auto peer = ourPeer();
    if (!peer)
        throw std::runtime_error("No active peer session");

    auto msg = json::Value::parse(data);
    if (!msg.is_object())
        throw std::runtime_error("Cannot send non-object protocol message");

    if (!msg.contains("id"))
        msg["id"] = util::randomString(16);
    if (!msg.contains("type"))
        msg["type"] = "message";

    msg["from"] = peer->address().toString();

    auto toIt = msg.find("to");
    if (toIt != msg.end() && toIt->is_string()) {
        Address to(toIt->get<std::string>());
        if (!to.id.empty() && to.id == peer->id())
            throw std::runtime_error("Sender and recipient cannot match");
    }

    return sendJson(msg);
}


ssize_t Client::respond(Message& m)
{
    m.setTo(m.value("from", ""));
    return send(m);
}


void Client::createPresence(Presence& p)
{
    auto peer = ourPeer();
    if (!peer)
        throw std::runtime_error("No peer session for presence");

    CreatePresence.emit(*peer);
    if (!peer->is_object())
        throw std::runtime_error("Peer must be a JSON object");

    p["data"] = static_cast<json::Value&>(*peer);
}


int Client::sendPresence(bool probe)
{
    Presence p;
    createPresence(p);
    p.setProbe(probe);
    return send(p);
}


int Client::sendPresence(const Address& to, bool probe)
{
    Presence p;
    createPresence(p);
    p.setProbe(probe);
    p.setTo(to);
    return send(p);
}


int Client::joinRoom(const std::string& room)
{
    if (room.empty())
        return -1;

    auto& data = *_data;
    LDebug("Join room: ", room);
    detail::noteDesiredJoin(data.desiredRooms, data.pendingLeaves, room);
    syncDesiredRooms();
    return 0;
}


int Client::leaveRoom(const std::string& room)
{
    if (room.empty())
        return -1;

    auto& data = *_data;
    LDebug("Leave room: ", room);
    if (isOnline() &&
        detail::noteDesiredLeave(data.desiredRooms,
                                 data.currentRooms,
                                 data.pendingLeaves,
                                 room)) {
        if (sendJson(proto::makeRoomMessage("leave", room)) >= 0)
            return 0;
        data.pendingLeaves.erase(room);
    } else {
        data.desiredRooms.erase(room);
    }

    return 0;
}


bool Client::isOnline() const
{
    return stateEquals(ClientState::Online);
}


std::string Client::ourID() const
{
    return _data->ourID;
}


Peer* Client::ourPeer()
{
    auto& data = *_data;
    if (data.ourID.empty())
        return nullptr;
    return data.roster.get(data.ourID);
}


StringVec Client::rooms() const
{
    const auto& data = *_data;
    StringVec rooms(data.currentRooms.begin(), data.currentRooms.end());
    std::sort(rooms.begin(), rooms.end());
    return rooms;
}


Roster& Client::roster()
{
    return _data->roster;
}


const Client::Options& Client::options() const
{
    return _data->options;
}


void Client::setOptions(Options options)
{
    auto& data = *_data;
    if (!stateEquals(ClientState::Closed) || data.ws || data.reconnectTimer.running())
        throw std::logic_error("Cannot change Symple client options while active");
    data.options = std::move(options);
}


void Client::setError(const std::string& error)
{
    auto& data = *_data;
    LError("Client error: ", error);

    // Close the WebSocket connection on error
    if (data.ws) {
        data.closing = true;
        data.ws->close();
        data.ws.reset();
        data.closing = false;
    }

    setState(this, ClientState::Error);
    if (data.options.reconnection)
        startReconnect();
}


} // namespace smpl
} // namespace icy


/// @}
