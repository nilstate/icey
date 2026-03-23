///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/client.h"
#include "icy/logger.h"
#include "icy/util.h"

#include <algorithm>
#include <stdexcept>


namespace icy {
namespace smpl {


Client::Client()
    : _options()
    , _loop(uv::defaultLoop())
    , _reconnectTimer(uv::defaultLoop())
{
    _reconnectTimer.Timeout += [this]() {
        doConnect();
    };
}


Client::Client(const Options& options, uv::Loop* loop)
    : _options(options)
    , _loop(loop)
    , _reconnectTimer(loop)
{
    _reconnectTimer.Timeout += [this]() {
        doConnect();
    };
}


Client::~Client()
{
    _reconnectTimer.stop();
    close();
}


void Client::connect()
{
    if (_options.host.empty())
        throw std::runtime_error("Symple client host must not be empty");
    if (_options.user.empty())
        throw std::runtime_error("Symple client user must not be empty");

    _reconnectCount = 0;
    doConnect();
}


void Client::doConnect()
{
    reset();
    setState(this, ClientState::Connecting);

    std::string url = buildUrl();
    LDebug("Connecting to ", url);

    _ws = http::createConnectionT<http::ClientConnection>(http::URL(url), _loop);

    // Connect fires after WS handshake completes (not on TCP connect).
    // Send auth as the first message.
    _ws->Connect += [this]() {
        LDebug("WebSocket ready, sending auth");
        setState(this, ClientState::Authenticating);

        json::Value auth;
        auth["type"] = "auth";
        auth["user"] = _options.user;
        if (!_options.name.empty())
            auth["name"] = _options.name;
        if (!_options.token.empty())
            auth["token"] = _options.token;
        if (!_options.type.empty()) {
            json::Value data;
            data["peerType"] = _options.type;
            auth["data"] = data;
        }
        if (!_desiredRooms.empty()) {
            auth["rooms"] = json::Value::array();
            for (const auto& room : _desiredRooms)
                auth["rooms"].push_back(room);
        }
        sendJson(auth);
    };

    _ws->Payload += [this](const MutableBuffer& buf) {
        std::string data(bufferCast<const char*>(buf), buf.size());
        onSocketRecv(data);
    };

    _ws->Close += [this](http::Connection&) {
        onSocketClose();
    };

    // Initiate the WebSocket connection.
    // TCP connect -> WS handshake -> Connect signal -> auth sent.
    _ws->send();
}


void Client::close()
{
    // Guard against re-entrant close (e.g. from onSocketClose callback)
    if (_closing)
        return;
    _closing = true;

    _reconnectTimer.stop();

    if (_ws) {
        // Send graceful close message before disconnecting
        if (isOnline()) {
            try {
                json::Value msg;
                msg["type"] = "close";
                sendJson(msg);
            }
            catch (...) {}
        }

        _ws->close();
        _ws.reset();
    }

    reset();

    if (!stateEquals(ClientState::Closed))
        setState(this, ClientState::Closed);

    _closing = false;
}


int Client::send(Message& m)
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


int Client::send(const std::string& data)
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


int Client::respond(Message& m)
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

    LDebug("Join room: ", room);
    _desiredRooms.insert(room);
    _pendingLeaves.erase(room);
    syncDesiredRooms();
    return 0;
}


int Client::leaveRoom(const std::string& room)
{
    if (room.empty())
        return -1;

    LDebug("Leave room: ", room);
    _desiredRooms.erase(room);

    if (isOnline() && _currentRooms.count(room) && !_pendingLeaves.count(room)) {
        json::Value msg;
        msg["type"] = "leave";
        msg["room"] = room;
        if (sendJson(msg) >= 0)
            _pendingLeaves.insert(room);
    }

    return 0;
}


bool Client::isOnline() const
{
    return stateEquals(ClientState::Online);
}


std::string Client::ourID() const
{
    return _ourID;
}


Peer* Client::ourPeer()
{
    if (_ourID.empty())
        return nullptr;
    return _roster.get(_ourID);
}


StringVec Client::rooms() const
{
    StringVec rooms(_currentRooms.begin(), _currentRooms.end());
    std::sort(rooms.begin(), rooms.end());
    return rooms;
}


Roster& Client::roster()
{
    return _roster;
}


Client::Options& Client::options()
{
    return _options;
}


void Client::setError(const std::string& error)
{
    LError("Client error: ", error);

    // Close the WebSocket connection on error
    if (_ws) {
        _closing = true;
        _ws->close();
        _ws.reset();
        _closing = false;
    }

    setState(this, ClientState::Error);
    if (_options.reconnection)
        startReconnect();
}


// ---------------------------------------------------------------------------
// WebSocket callbacks
// ---------------------------------------------------------------------------

void Client::onSocketRecv(const std::string& data)
{
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

        if (type == "error") {
            int status = msg.value("status", 500);
            std::string message = msg.value("message", "Unknown error");
            LError("Server error: ", status, " ", message);
            _announceStatus = status;
            Announce.emit(status);
            setError(message);
            return;
        }

        if (type == "join:ok") {
            auto room = msg.value("room", "");
            if (!room.empty()) {
                _pendingJoins.erase(room);
                _currentRooms.insert(room);
                if (!_desiredRooms.count(room) && !_pendingLeaves.count(room)) {
                    json::Value leave;
                    leave["type"] = "leave";
                    leave["room"] = room;
                    if (sendJson(leave) >= 0)
                        _pendingLeaves.insert(room);
                }
            }
            return;
        }
        if (type == "leave:ok") {
            auto room = msg.value("room", "");
            if (!room.empty()) {
                _pendingLeaves.erase(room);
                _currentRooms.erase(room);
                if (_desiredRooms.count(room) && !_pendingJoins.count(room))
                    syncDesiredRooms();
            }
            return;
        }

        onServerMessage(msg);
    }
    catch (const std::exception& e) {
        LWarn("Parse error: ", e.what());
    }
}


void Client::onSocketClose()
{
    LInfo("WebSocket disconnected");

    // If close() was called explicitly, don't process the async callback.
    // close() already handled cleanup and state transition.
    if (_closing || stateEquals(ClientState::Closed))
        return;

    bool wasOnline = _wasOnline;
    reset();
    setState(this, ClientState::Closed);

    if (wasOnline && _options.reconnection)
        startReconnect();
}


void Client::onSocketError(const std::string& error)
{
    LError("WebSocket error: ", error);
    setError(error);
}


void Client::onWelcome(const json::Value& msg)
{
    _announceStatus = msg.value("status", 200);

    if (_announceStatus != 200) {
        std::string message = msg.value("message", "Auth failed");
        Announce.emit(_announceStatus);
        setError(message);
        return;
    }

    if (!msg.contains("peer") || !msg["peer"].is_object()) {
        setError("Invalid welcome: missing peer data");
        return;
    }

    const auto& peerData = msg["peer"];
    _ourID = peerData.value("id", "");
    if (_ourID.empty()) {
        setError("Invalid welcome: missing peer ID");
        return;
    }

    onPresenceData(peerData, true);

    _currentRooms.clear();
    _pendingJoins.clear();
    _pendingLeaves.clear();
    if (msg.contains("rooms") && msg["rooms"].is_array()) {
        for (const auto& room : msg["rooms"]) {
            if (room.is_string() && !room.get<std::string>().empty())
                _currentRooms.insert(room.get<std::string>());
        }
    }

    Announce.emit(_announceStatus);

    _wasOnline = true;
    _reconnectCount = 0;
    setState(this, ClientState::Online);

    LInfo("Online as ", _options.user, "|", _ourID);
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
    }
    else if (type == "event") {
        Event e(data);
        if (e.valid())
            PacketSignal::emit(e);
    }
    else if (type == "presence") {
        Presence p(data);
        if (p.valid()) {
            PacketSignal::emit(p);
            if (p.find("data") != p.end()) {
                onPresenceData(p["data"], false);
                if (p.isProbe())
                    sendPresence(p.from());
            }
        }
    }
    else if (type == "command") {
        Command c(data);
        if (c.valid()) {
            PacketSignal::emit(c);
            if (c.isRequest()) {
                c.setStatus(404);
                respond(c);
            }
        }
    }
    else {
        Message m(data);
        if (m.valid())
            PacketSignal::emit(m);
    }
}


void Client::onPresenceData(const json::Value& data, bool whiny)
{
    if (data.is_object() &&
        data.contains("id") &&
        data.contains("user") &&
        data.contains("online")) {

        std::string id = data["id"].get<std::string>();
        bool online = data["online"].get<bool>();

        auto peer = _roster.get(id);
        if (online) {
            if (!peer) {
                _roster.add(id, std::make_unique<Peer>(data));
                peer = _roster.get(id);
                LDebug("Peer connected: ", peer->address().toString());
                PeerConnected.emit(*peer);
            }
            else {
                static_cast<json::Value&>(*peer) = data;
            }
        }
        else {
            if (peer) {
                LDebug("Peer disconnected: ", peer->address().toString());
                PeerDisconnected.emit(*peer);
                _roster.erase(id);
            }
        }
    }
    else {
        std::string error("Bad presence data: " + data.dump());
        LError(error);
        if (whiny)
            throw std::runtime_error(error);
    }
}


// ---------------------------------------------------------------------------
// Reconnection
// ---------------------------------------------------------------------------

void Client::startReconnect()
{
    if (_options.reconnectAttempts > 0 &&
        _reconnectCount >= _options.reconnectAttempts) {
        LWarn("Max reconnect attempts reached");
        return;
    }

    _reconnectCount++;
    LInfo("Reconnecting (attempt ", _reconnectCount, ")...");

    _reconnectTimer.setTimeout(_options.reconnectDelay);
    _reconnectTimer.start();
}


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void Client::reset()
{
    _roster.clear();
    _currentRooms.clear();
    _pendingJoins.clear();
    _pendingLeaves.clear();
    _announceStatus = 0;
    _ourID.clear();
    _wasOnline = false;
}


void Client::syncDesiredRooms()
{
    if (!_ws || !isOnline())
        return;

    for (const auto& room : _desiredRooms) {
        if (_currentRooms.count(room) || _pendingJoins.count(room))
            continue;

        json::Value msg;
        msg["type"] = "join";
        msg["room"] = room;
        if (sendJson(msg) >= 0)
            _pendingJoins.insert(room);
    }
}


int Client::sendJson(const json::Value& msg)
{
    if (!_ws)
        return -1;

    auto str = msg.dump();
    return _ws->send(str.c_str(), str.size(), http::ws::Text);
}


std::string Client::buildUrl() const
{
    std::string scheme = _options.secure ? "wss" : "ws";
    return scheme + "://" + _options.host + ":" +
           std::to_string(_options.port) + "/";
}


} // namespace smpl
} // namespace icy


/// @\}
