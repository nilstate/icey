///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "scy/symple/server.h"
#include "scy/logger.h"
#include "scy/net/tcpsocket.h"
#include "scy/util.h"


namespace scy {
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
    try {
        _conn.send(str.c_str(), str.size(), http::ws::Text);
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


// ---------------------------------------------------------------------------
// Server::Responder - handles per-connection WebSocket frames
// ---------------------------------------------------------------------------

class Server::Responder : public http::ServerResponder
{
public:
    Responder(http::ServerConnection& conn, Server& server)
        : http::ServerResponder(conn)
        , _server(server)
        , _tempPeer(conn)
    {
        // Start auth timeout: close if not authenticated within 10 seconds
        _authTimer.setTimeout(10000);
        _authTimer.start();
        _authTimer.Timeout += [this]() {
            if (!_authenticated) {
                LWarn("Auth timeout, closing connection");
                json::Value err;
                err["type"] = "error";
                err["status"] = 408;
                err["message"] = "Authentication timeout";
                auto str = err.dump();
                try {
                    connection().send(str.c_str(), str.size(), http::ws::Text);
                } catch (...) {}
                connection().close();
            }
        };
    }

    void onPayload(const MutableBuffer& buffer) override
    {
        std::string str(bufferCast<const char*>(buffer), buffer.size());

        json::Value msg;
        try {
            msg = json::Value::parse(str);
        }
        catch (const std::exception& e) {
            LWarn("JSON parse error: ", e.what());
            return;
        }

        std::string type = msg.value("type", "");

        std::lock_guard lock(_server._mutex);

        auto it = _server._connToPeer.find(&connection());

        if (it == _server._connToPeer.end()) {
            // Not authenticated: only accept auth
            if (type == "auth") {
                _server.onAuth(_tempPeer, msg);
                if (_tempPeer.authenticated()) {
                    _authenticated = true;
                    _authTimer.stop();
                }
            }
            else {
                json::Value err;
                err["type"] = "error";
                err["status"] = 401;
                err["message"] = "Not authenticated";
                auto errStr = err.dump();
                try {
                    connection().send(errStr.c_str(), errStr.size(), http::ws::Text);
                } catch (...) {}
                connection().close();
            }
            return;
        }

        auto peerIt = _server._peers.find(it->second);
        if (peerIt == _server._peers.end())
            return;
        auto* peer = peerIt->second.get();

        if (type == "message" || type == "presence" ||
            type == "command" || type == "event") {
            _server.onMessage(*peer, msg);
        }
        else if (type == "join") {
            _server.onJoin(*peer, msg.value("room", ""));
        }
        else if (type == "leave") {
            _server.onLeave(*peer, msg.value("room", ""));
        }
        else if (type == "close") {
            connection().close();
        }
    }

    void onClose() override
    {
        _authTimer.stop();
        std::lock_guard lock(_server._mutex);
        auto it = _server._connToPeer.find(&connection());
        if (it != _server._connToPeer.end()) {
            auto peerIt = _server._peers.find(it->second);
            if (peerIt != _server._peers.end()) {
                _server.onDisconnect(*peerIt->second);
            }
        }
    }

    Server& _server;
    ServerPeer _tempPeer;
    Timer _authTimer;
    bool _authenticated = false;
};


// ---------------------------------------------------------------------------
// Server::Factory
// ---------------------------------------------------------------------------

class Server::Factory : public http::ServerConnectionFactory
{
public:
    Factory(Server& server) : _server(server) {}

    std::unique_ptr<http::ServerResponder> createResponder(
        http::ServerConnection& conn) override
    {
        if (!conn.upgraded())
            return nullptr;

        return std::make_unique<Server::Responder>(conn, _server);
    }

    Server& _server;
};


// ---------------------------------------------------------------------------
// Server
// ---------------------------------------------------------------------------

Server::Server() = default;


Server::~Server()
{
    shutdown();
}


void Server::start(const Options& opts)
{
    _opts = opts;

    _http = std::make_unique<http::Server>(
        opts.host, opts.port,
        net::makeSocket<net::TCPSocket>(),
        std::make_unique<Factory>(*this));

    _http->start();

    LInfo("Symple server listening on ", opts.host, ":", opts.port);
}


void Server::shutdown()
{
    std::lock_guard lock(_mutex);

    _peers.clear();
    _rooms.clear();
    _connToPeer.clear();

    if (_http)
        _http->shutdown();
    _http.reset();

    LInfo("Symple server shut down");
}


void Server::onAuth(ServerPeer& tempPeer, const json::Value& msg)
{
    // Caller holds _mutex.

    std::string user = msg.value("user", "");
    if (user.empty()) {
        json::Value err;
        err["type"] = "error";
        err["status"] = 401;
        err["message"] = "Missing user field";
        tempPeer.send(err);
        tempPeer.connection().close();
        return;
    }

    LDebug("onAuth: user=", user);

    // Generate session ID using base module's random string
    std::string id = util::randomString(16);

    // Build peer object
    Peer p;
    p["id"] = id;
    p["user"] = user;
    p["name"] = msg.value("name", user);
    p["online"] = true;

    // Copy extra data
    if (msg.contains("data") && msg["data"].is_object()) {
        for (auto& [k, v] : msg["data"].items()) {
            p[k] = v;
        }
    }

    // Custom authentication hook
    if (_opts.authentication || Authenticate.nslots() > 0) {
        bool allowed = !_opts.authentication;
        _mutex.unlock();
        Authenticate.emit(tempPeer, msg, allowed);
        _mutex.lock();
        if (!allowed) {
            json::Value err;
            err["type"] = "error";
            err["status"] = 401;
            err["message"] = "Authentication failed";
            tempPeer.send(err);
            tempPeer.connection().close();
            return;
        }
    }

    // Register the peer
    auto peer = std::make_unique<ServerPeer>(tempPeer.connection());
    peer->setPeer(p);
    peer->setAuthenticated(true);

    // Auto-join user room
    peer->join(user);
    addToRoom(user, id);

    _connToPeer[&tempPeer.connection()] = id;
    auto* registeredPeer = peer.get();
    _peers[id] = std::move(peer);

    // Mark the temp peer as authenticated (so Responder knows)
    tempPeer.setAuthenticated(true);

    // Send welcome
    json::Value welcome;
    welcome["type"] = "welcome";
    welcome["peer"] = static_cast<const json::Value&>(p);
    welcome["status"] = 200;
    registeredPeer->send(welcome);

    LInfo("Peer authenticated: ", user, "|", id);

    // Broadcast online presence
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = static_cast<const json::Value&>(p);
    broadcast(user, presence, id);

    // Notify server hooks (release lock for signal emission)
    _mutex.unlock();
    PeerConnected.emit(*registeredPeer);
    _mutex.lock();
}


void Server::onMessage(ServerPeer& sender, const json::Value& msg)
{
    // Caller holds _mutex.

    // Enforce that the from field matches the authenticated peer.
    // This prevents spoofing.
    std::string expectedFrom = sender.peer().user() + "|" + sender.id();
    json::Value routable = msg;
    routable["from"] = expectedFrom;

    route(sender, routable);
}


void Server::onJoin(ServerPeer& peer, const std::string& room)
{
    if (room.empty())
        return;

    if (!_opts.dynamicRooms) {
        json::Value err;
        err["type"] = "error";
        err["message"] = "Dynamic rooms disabled";
        peer.send(err);
        return;
    }

    peer.join(room);
    addToRoom(room, peer.id());

    json::Value ok;
    ok["type"] = "join:ok";
    ok["room"] = room;
    peer.send(ok);

    LDebug("Peer ", peer.id(), " joined room: ", room);
}


void Server::onLeave(ServerPeer& peer, const std::string& room)
{
    if (room.empty())
        return;

    peer.leave(room);
    removeFromRoom(room, peer.id());

    json::Value ok;
    ok["type"] = "leave:ok";
    ok["room"] = room;
    peer.send(ok);

    LDebug("Peer ", peer.id(), " left room: ", room);
}


void Server::onDisconnect(ServerPeer& peer)
{
    // Caller holds _mutex.

    std::string id = peer.id();
    std::string user = peer.peer().user();

    LInfo("Peer disconnected: ", user, "|", id);

    // Broadcast offline presence to all rooms
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = static_cast<const json::Value&>(peer.peer());
    presence["data"]["online"] = false;

    for (const auto& room : peer.rooms()) {
        broadcast(room, presence, id);
    }

    // Clean up
    removeFromAllRooms(id);
    _connToPeer.erase(&peer.connection());

    // Notify hooks (release lock)
    _mutex.unlock();
    PeerDisconnected.emit(peer);
    _mutex.lock();

    _peers.erase(id);
}


void Server::route(ServerPeer& sender, const json::Value& msg)
{
    // Caller holds _mutex.

    if (!msg.contains("to")) {
        // No recipient: broadcast to all sender's rooms (excluding sender)
        for (const auto& room : sender.rooms()) {
            broadcast(room, msg, sender.id());
        }
        return;
    }

    const auto& to = msg["to"];

    if (to.is_string()) {
        std::string addr = to.get<std::string>();
        auto pos = addr.find('|');

        if (pos != std::string::npos) {
            // Full address: user|id - send to specific session
            std::string peerId = addr.substr(pos + 1);
            if (sendTo(peerId, msg))
                return;
        }

        // User name only: send to user's room
        std::string user = addr.substr(0, pos != std::string::npos ? pos : addr.size());
        sendToUser(user, msg);
    }
    else if (to.is_array()) {
        for (const auto& room : to) {
            if (room.is_string())
                broadcast(room.get<std::string>(), msg, sender.id());
        }
    }
}


void Server::broadcast(const std::string& room, const json::Value& msg,
                        const std::string& excludeId)
{
    auto it = _rooms.find(room);
    if (it == _rooms.end())
        return;

    auto str = msg.dump();
    for (const auto& peerId : it->second) {
        if (peerId == excludeId)
            continue;

        auto peerIt = _peers.find(peerId);
        if (peerIt != _peers.end()) {
            try {
                peerIt->second->connection().send(
                    str.c_str(), str.size(), http::ws::Text);
            }
            catch (const std::exception& e) {
                LWarn("Broadcast failed to ", peerId, ": ", e.what());
            }
        }
    }
}


bool Server::sendTo(const std::string& peerId, const json::Value& msg)
{
    auto it = _peers.find(peerId);
    if (it == _peers.end())
        return false;

    try {
        it->second->send(msg);
        return true;
    }
    catch (const std::exception& e) {
        LWarn("sendTo failed: ", peerId, ": ", e.what());
        return false;
    }
}


bool Server::sendToUser(const std::string& user, const json::Value& msg)
{
    auto it = _rooms.find(user);
    if (it == _rooms.end())
        return false;

    bool sent = false;
    auto str = msg.dump();
    for (const auto& peerId : it->second) {
        auto peerIt = _peers.find(peerId);
        if (peerIt != _peers.end()) {
            try {
                peerIt->second->connection().send(
                    str.c_str(), str.size(), http::ws::Text);
                sent = true;
            }
            catch (...) {}
        }
    }
    return sent;
}


ServerPeer* Server::getPeer(const std::string& id)
{
    std::lock_guard lock(_mutex);
    auto it = _peers.find(id);
    return it != _peers.end() ? it->second.get() : nullptr;
}


std::vector<ServerPeer*> Server::getPeersInRoom(const std::string& room)
{
    std::lock_guard lock(_mutex);
    std::vector<ServerPeer*> result;
    auto it = _rooms.find(room);
    if (it != _rooms.end()) {
        for (const auto& peerId : it->second) {
            auto peerIt = _peers.find(peerId);
            if (peerIt != _peers.end())
                result.push_back(peerIt->second.get());
        }
    }
    return result;
}


size_t Server::peerCount() const
{
    std::lock_guard lock(_mutex);
    return _peers.size();
}


void Server::addToRoom(const std::string& room, const std::string& peerId)
{
    _rooms[room].insert(peerId);
}


void Server::removeFromRoom(const std::string& room, const std::string& peerId)
{
    auto it = _rooms.find(room);
    if (it != _rooms.end()) {
        it->second.erase(peerId);
        if (it->second.empty())
            _rooms.erase(it);
    }
}


void Server::removeFromAllRooms(const std::string& peerId)
{
    for (auto it = _rooms.begin(); it != _rooms.end();) {
        it->second.erase(peerId);
        if (it->second.empty())
            it = _rooms.erase(it);
        else
            ++it;
    }
}


} // namespace smpl
} // namespace scy
