///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/client.h"
#include "../protocol.h"
#include "detail.h"
#include "roomstate.h"
#include "rosterstate.h"
#include "icy/logger.h"

#include <stdexcept>


namespace icy {
namespace smpl {


void Client::onSocketRecv(const std::string& data)
{
    auto& state = *_data;
    try {
        auto msg = json::Value::parse(data);
        std::string type = msg.value("type", "");

        if (type.empty()) {
            LWarn("Received message with no type");
            return;
        }

        if (type == "welcome") {
            onWelcome(msg);
            return;
        }

        if (auto error = proto::parseError(msg)) {
            int status = error->first;
            const auto& message = error->second;
            LError("Server error: ", status, " ", message);
            state.announceStatus = status;
            Announce.emit(status);
            setError(message);
            return;
        }

        if (auto room = proto::parseRoomMessage(msg, "join:ok")) {
            if (detail::applyJoinAck(
                    state.desiredRooms,
                    state.currentRooms,
                    state.pendingJoins,
                    state.pendingLeaves,
                    *room)) {
                if (sendJson(proto::makeRoomMessage("leave", *room)) >= 0)
                    return;
                state.pendingLeaves.erase(*room);
            }
            return;
        }
        if (auto room = proto::parseRoomMessage(msg, "leave:ok")) {
            if (detail::applyLeaveAck(
                    state.desiredRooms,
                    state.currentRooms,
                    state.pendingJoins,
                    state.pendingLeaves,
                    *room))
                syncDesiredRooms();
            return;
        }

        onServerMessage(msg);
    }
    catch (const std::exception& e) {
        LWarn("Parse error: ", e.what());
    }
}


void Client::onWelcome(const json::Value& msg)
{
    auto& data = *_data;
    std::string error;
    auto welcome = proto::parseWelcome(msg, &error);
    if (!welcome) {
        setError(error.empty() ? "Invalid welcome" : error);
        return;
    }

    data.announceStatus = welcome->status;

    if (data.announceStatus != 200) {
        std::string message = welcome->message.empty() ? "Auth failed" : welcome->message;
        Announce.emit(data.announceStatus);
        setError(message);
        return;
    }

    data.ourID = welcome->peer.id();
    onPresenceData(static_cast<const json::Value&>(welcome->peer), true);

    detail::applyWelcome(data.currentRooms,
                         data.pendingJoins,
                         data.pendingLeaves,
                         welcome->rooms);

    Announce.emit(data.announceStatus);

    data.wasOnline = true;
    data.reconnectCount = 0;
    setState(this, ClientState::Online);

    LInfo("Online as ", data.options.user, "|", data.ourID);
    syncDesiredRooms();
    sendPresence(true);
}


void Client::onServerMessage(const json::Value& data)
{
    std::string type = data.value("type", "");

    if (type == "message") {
        Message m(data);
        if (m.valid())
            PacketSignal::emit(m);
        return;
    }

    if (type == "event") {
        Event e(data);
        if (e.valid())
            PacketSignal::emit(e);
        return;
    }

    if (type == "presence") {
        Presence p(data);
        if (p.valid()) {
            PacketSignal::emit(p);
            if (p.find("data") != p.end()) {
                onPresenceData(p["data"], false);
                if (p.isProbe())
                    sendPresence(p.from());
            }
        }
        return;
    }

    if (type == "command") {
        Command c(data);
        if (c.valid()) {
            PacketSignal::emit(c);
            if (c.isRequest()) {
                c.setStatus(404);
                respond(c);
            }
        }
        return;
    }

    Message m(data);
    if (m.valid())
        PacketSignal::emit(m);
}


void Client::onPresenceData(const json::Value& data, bool whiny)
{
    auto update = detail::applyPresenceData(_data->roster, data);
    switch (update.kind) {
    case detail::PresenceUpdate::Kind::Connected:
        LDebug("Peer connected: ", update.livePeer->address().toString());
        PeerConnected.emit(*update.livePeer);
        return;
    case detail::PresenceUpdate::Kind::Disconnected:
        LDebug("Peer disconnected: ", update.snapshot.address().toString());
        PeerDisconnected.emit(update.snapshot);
        return;
    case detail::PresenceUpdate::Kind::None:
        return;
    case detail::PresenceUpdate::Kind::Invalid:
        break;
    }

    LError(update.error);
    if (whiny)
        throw std::runtime_error(update.error);
}


} // namespace smpl
} // namespace icy
