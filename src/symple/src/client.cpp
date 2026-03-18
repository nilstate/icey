///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/client.h"
#include "scy/logger.h"

#include <algorithm>
#include <stdexcept>


namespace scy {
namespace smpl {


Client::Client()
    : _options()
    , _loop(uv::defaultLoop())
    , _reconnectTimer(uv::defaultLoop())
{
}


Client::Client(const Options& options, uv::Loop* loop)
    : _options(options)
    , _loop(loop)
    , _reconnectTimer(loop)
{
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

    _ws->Payload += [this](const MutableBuffer& buf) {
        std::string data(bufferCast<const char*>(buf), buf.size());
        onSocketRecv(data);
    };

    _ws->Close += [this](http::Connection&) {
        onSocketClose();
    };

    // Build auth JSON to send as the first WS message after handshake.
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

    std::string authStr = auth.dump();

    // Store the auth message to send after the WS handshake completes.
    _pendingAuth = authStr;

    // The first Payload from the server means the WS connection is
    // fully established. But we need to send auth before the server
    // sends anything. Use a polling timer to detect handshake completion.
    _authSendTimer.setTimeout(10);
    _authSendTimer.setInterval(10);
    _authSendTimer.Timeout += [this]() {
        // Check if the WS handshake has completed by attempting to send.
        // If the connection is active and the adapter allows sending,
        // the handshake is done.
        if (!_pendingAuth.empty() && _ws) {
            try {
                auto result = _ws->send(
                    _pendingAuth.c_str(), _pendingAuth.size(), http::ws::Text);
                if (result > 0) {
                    LInfo("Auth sent successfully");
                    _pendingAuth.clear();
                    _authSendTimer.stop();
                    setState(this, ClientState::Authenticating);
                }
            }
            catch (...) {
                // Handshake not yet complete, try again next tick
            }
        }
    };
    _authSendTimer.start();

    // Initiate the WebSocket connection
    _ws->send();

    setState(this, ClientState::Authenticating);
}


void Client::close()
{
    _reconnectTimer.stop();

    if (_ws) {
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

    if (!stateEquals(ClientState::Closed)) {
        reset();
        setState(this, ClientState::Closed);
    }
}


int Client::send(Message& m)
{
    if (!isOnline())
        throw std::runtime_error("Cannot send message while offline");

    auto peer = ourPeer();
    if (!peer)
        throw std::runtime_error("No active peer session");

    m.setFrom(peer->address());

    if (m.to().id == m.from().id && !m.to().id.empty())
        throw std::runtime_error("Sender and recipient cannot match");

    if (!m.valid())
        throw std::runtime_error("Cannot send invalid message");

    return sendJson(m);
}


int Client::send(const std::string& data)
{
    Message m;
    if (!m.read(data))
        throw std::runtime_error("Cannot send malformed message");
    return send(m);
}


int Client::respond(Message& m)
{
    m.setTo(m.from());
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
    LDebug("Join room: ", room);
    _rooms.push_back(room);

    json::Value msg;
    msg["type"] = "join";
    msg["room"] = room;
    return sendJson(msg);
}


int Client::leaveRoom(const std::string& room)
{
    LDebug("Leave room: ", room);
    _rooms.erase(std::remove(_rooms.begin(), _rooms.end(), room), _rooms.end());

    json::Value msg;
    msg["type"] = "leave";
    msg["room"] = room;
    return sendJson(msg);
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
    return _roster.get(_ourID, false);
}


StringVec Client::rooms() const
{
    return _rooms;
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

        if (type == "ready") {
            // Server v4 ready signal - ignore, we pre-queued auth
            return;
        }

        if (type == "join:ok" || type == "leave:ok")
            return;

        onServerMessage(msg);
    }
    catch (const std::exception& e) {
        LWarn("Parse error: ", e.what());
    }
}


void Client::onSocketClose()
{
    LInfo("WebSocket disconnected");
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

    Announce.emit(_announceStatus);

    _wasOnline = true;
    _reconnectCount = 0;
    setState(this, ClientState::Online);

    LInfo("Online as ", _options.user, "|", _ourID);

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

        auto peer = _roster.get(id, false);
        if (online) {
            if (!peer) {
                _roster.add(id, std::make_unique<Peer>(data));
                peer = _roster.get(id, false);
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
                _roster.free(id);
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
    _reconnectTimer.Timeout += [this]() {
        doConnect();
    };
    _reconnectTimer.start();
}


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void Client::reset()
{
    _authSendTimer.stop();
    _pendingAuth.clear();
    _roster.clear();
    _rooms.clear();
    _announceStatus = 0;
    _ourID.clear();
    _wasOnline = false;
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
} // namespace scy


/// @\}
