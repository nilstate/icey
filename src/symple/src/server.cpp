///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "icy/symple/server.h"
#include "icy/logger.h"
#include "icy/net/tcpsocket.h"
#include "icy/util.h"


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
                sendError(408, "Authentication timeout");
                connection().close();
            }
        };
    }

    void onPayload(const MutableBuffer& buffer) override
    {
        // Close must happen outside the lock: connection().close()
        // triggers onClose() which also acquires _mutex.
        bool shouldClose = false;

        // Max message size check (before allocation/parse)
        if (_server._opts.maxMessageSize > 0 &&
            buffer.size() > _server._opts.maxMessageSize) {
            LWarn("Message too large (", buffer.size(), " bytes), closing");
            sendError(413, "Message too large");
            shouldClose = true;
        }

        json::Value msg;
        std::string type;

        if (!shouldClose) {
            std::string str(bufferCast<const char*>(buffer), buffer.size());
            try {
                msg = json::Value::parse(str);
            }
            catch (const std::exception& e) {
                LWarn("JSON parse error: ", e.what());
                return;
            }
            type = msg.value("type", "");
        }

        if (!shouldClose) {
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
                    sendError(401, "Not authenticated");
                    shouldClose = true;
                }
            }
            else {
                auto peerIt = _server._peers.find(it->second);
                if (peerIt == _server._peers.end())
                    return;
                auto* peer = peerIt->second.get();

                // Per-peer rate limiting
                if (!peer->checkRate()) {
                    LWarn("Rate limit exceeded for peer ", peer->id());
                    sendError(429, "Rate limit exceeded");
                    shouldClose = true;
                }
                else if (type == "message" || type == "presence" ||
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
                    shouldClose = true;
                }
            }
        }

        if (shouldClose)
            connection().close();
    }

    void onClose() override
    {
        _authTimer.stop();
        if (_server._shuttingDown)
            return;
        std::lock_guard lock(_server._mutex);
        auto it = _server._connToPeer.find(&connection());
        if (it != _server._connToPeer.end()) {
            auto peerIt = _server._peers.find(it->second);
            if (peerIt != _server._peers.end()) {
                _server.onDisconnect(*peerIt->second);
            }
        }
    }

    void sendError(int status, const std::string& message)
    {
        json::Value err;
        err["type"] = "error";
        err["status"] = status;
        err["message"] = message;
        auto str = err.dump();
        try {
            connection().send(str.c_str(), str.size(), http::ws::Text);
        } catch (...) {}
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
        if (!conn.upgraded()) {
            // Delegate non-WebSocket requests to the fallback factory
            if (_server._httpFallback)
                return _server._httpFallback->createResponder(conn);
            return nullptr;
        }

        // Max connections check. peerCount() acquires its own lock;
        // no Symple mutex is held during createResponder.
        // Send a 503 error then close. close() is safe to call here:
        // it sets _closed=true and schedules async uv_close; the actual
        // destruction is deferred by Server::onConnectionClose via idle
        // callback with shared_ptr ownership.
        if (_server._opts.maxConnections > 0 &&
            _server.peerCount() >= _server._opts.maxConnections) {
            LWarn("Max connections reached (", _server._opts.maxConnections, "), rejecting");
            json::Value err;
            err["type"] = "error";
            err["status"] = 503;
            err["message"] = "Server at capacity";
            auto str = err.dump();
            try {
                conn.send(str.c_str(), str.size(), http::ws::Text);
            } catch (...) {}
            conn.close();
            return nullptr;
        }

        return std::make_unique<Server::Responder>(conn, _server);
    }

    Server& _server;
};


// ---------------------------------------------------------------------------
// Server
// ---------------------------------------------------------------------------

Server::Server(uv::Loop* loop)
    : _loop(loop)
{
}


Server::~Server()
{
    shutdown();
}


void Server::start(const Options& opts)
{
    start(opts, nullptr);
}


void Server::start(const Options& opts,
                   std::unique_ptr<http::ServerConnectionFactory> httpFactory)
{
    _opts = opts;
    _httpFallback = std::move(httpFactory);

    _http = std::make_unique<http::Server>(
        opts.host, opts.port,
        _loop,
        std::make_unique<Factory>(*this));

    _http->start();

    LInfo("Symple server listening on ", opts.host, ":", opts.port);
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
        for (auto& [id, peer] : _peers) {
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
        _connToPeer.clear();
        _rooms.clear();
        _peers.clear();
        _virtualPeers.clear();
    }

    // Release the HTTP server. Its destructor calls shutdown() again
    // (no-op due to _shuttingDown guard on the http side - the socket
    // is already closed). Connections and responders are destroyed here.
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
        return;
    }

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

    // Custom authentication hook.
    // The hook can reject the peer and/or assign room memberships.
    std::vector<std::string> authRooms;
    if (_opts.authentication || Authenticate.nslots() > 0) {
        bool allowed = !_opts.authentication;
        _mutex.unlock();
        Authenticate.emit(tempPeer, msg, allowed, authRooms);
        _mutex.lock();
        if (!allowed) {
            json::Value err;
            err["type"] = "error";
            err["status"] = 401;
            err["message"] = "Authentication failed";
            tempPeer.send(err);
            return;
        }
    }

    // Register the peer
    auto peer = std::make_unique<ServerPeer>(tempPeer.connection());
    peer->setPeer(p);
    peer->setAuthenticated(true);
    peer->setRateLimit(_opts.rateLimit, _opts.rateSeconds);

    // Auto-join user room
    peer->join(user);
    addToRoom(user, id);

    // Auto-join rooms from auth message
    if (msg.contains("rooms") && msg["rooms"].is_array()) {
        for (const auto& room : msg["rooms"]) {
            if (room.is_string() && !room.get<std::string>().empty()) {
                std::string r = room.get<std::string>();
                peer->join(r);
                addToRoom(r, id);
            }
        }
    }

    // Auto-join rooms assigned by the Authenticate hook
    for (const auto& room : authRooms) {
        if (!room.empty()) {
            peer->join(room);
            addToRoom(room, id);
        }
    }

    _connToPeer[&tempPeer.connection()] = id;
    auto* registeredPeer = peer.get();
    _peers[id] = std::move(peer);

    // Mark the temp peer as authenticated (so Responder knows)
    tempPeer.setAuthenticated(true);

    // Send welcome with protocol version and room list
    json::Value welcome;
    welcome["type"] = "welcome";
    welcome["protocol"] = "symple/4";
    welcome["peer"] = static_cast<const json::Value&>(p);
    welcome["status"] = 200;
    welcome["rooms"] = json::Value::array();
    for (const auto& room : registeredPeer->rooms())
        welcome["rooms"].push_back(room);
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


bool Server::sharesRoom(const ServerPeer& a, const ServerPeer& b) const
{
    for (const auto& room : a.rooms()) {
        if (b.rooms().count(room))
            return true;
    }
    return false;
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
            // Full address: user|id - send to specific session.
            // Permission check: sender and recipient must share a room.
            std::string peerId = addr.substr(pos + 1);

            // Check real peers
            auto targetIt = _peers.find(peerId);
            if (targetIt != _peers.end()) {
                if (sharesRoom(sender, *targetIt->second)) {
                    sendTo(peerId, msg);
                } else {
                    LDebug("Blocked direct message from ", sender.id(), " to ", peerId, " (no shared room)");
                }
                return;
            }

            // Check virtual peers
            auto vit = _virtualPeers.find(peerId);
            if (vit != _virtualPeers.end()) {
                // Check if sender shares a room with the virtual peer
                bool shared = false;
                for (const auto& room : sender.rooms()) {
                    if (vit->second.rooms.count(room)) {
                        shared = true;
                        break;
                    }
                }
                if (shared) {
                    sendTo(peerId, msg);
                } else {
                    LDebug("Blocked direct message from ", sender.id(), " to virtual peer ", peerId, " (no shared room)");
                }
            }
            return;
        }

        // User name only: send to user's room
        std::string user = addr.substr(0, addr.size());
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

        // Real peer
        auto peerIt = _peers.find(peerId);
        if (peerIt != _peers.end()) {
            try {
                peerIt->second->connection().send(
                    str.c_str(), str.size(), http::ws::Text);
            }
            catch (const std::exception& e) {
                LWarn("Broadcast failed to ", peerId, ": ", e.what());
            }
            continue;
        }

        // Virtual peer
        auto vit = _virtualPeers.find(peerId);
        if (vit != _virtualPeers.end()) {
            try {
                vit->second.handler(msg);
            }
            catch (const std::exception& e) {
                LWarn("Broadcast to virtual peer failed: ", peerId, ": ", e.what());
            }
        }
    }
}


bool Server::sendTo(const std::string& peerId, const json::Value& msg)
{
    // Check real peers first
    auto it = _peers.find(peerId);
    if (it != _peers.end()) {
        try {
            it->second->send(msg);
            return true;
        }
        catch (const std::exception& e) {
            LWarn("sendTo failed: ", peerId, ": ", e.what());
            return false;
        }
    }

    // Check virtual peers
    auto vit = _virtualPeers.find(peerId);
    if (vit != _virtualPeers.end()) {
        try {
            vit->second.handler(msg);
            return true;
        }
        catch (const std::exception& e) {
            LWarn("sendTo virtual peer failed: ", peerId, ": ", e.what());
            return false;
        }
    }

    return false;
}


bool Server::sendToUser(const std::string& user, const json::Value& msg)
{
    auto it = _rooms.find(user);
    if (it == _rooms.end())
        return false;

    bool sent = false;
    auto str = msg.dump();
    for (const auto& peerId : it->second) {
        // Real peer
        auto peerIt = _peers.find(peerId);
        if (peerIt != _peers.end()) {
            try {
                peerIt->second->connection().send(
                    str.c_str(), str.size(), http::ws::Text);
                sent = true;
            }
            catch (...) {}
            continue;
        }

        // Virtual peer
        auto vit = _virtualPeers.find(peerId);
        if (vit != _virtualPeers.end()) {
            try {
                vit->second.handler(msg);
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
    return _peers.size() + _virtualPeers.size();
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
    addToRoom(user, id);

    // Join requested rooms
    for (const auto& room : rooms) {
        if (!room.empty()) {
            vp.rooms.insert(room);
            addToRoom(room, id);
        }
    }

    _virtualPeers[id] = std::move(vp);

    // Broadcast online presence
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = static_cast<const json::Value&>(peer);
    for (const auto& room : _virtualPeers[id].rooms) {
        broadcast(room, presence, id);
    }

    LInfo("Virtual peer registered: ", user, "|", id);
}


void Server::removeVirtualPeer(const std::string& peerId)
{
    std::lock_guard lock(_mutex);

    auto it = _virtualPeers.find(peerId);
    if (it == _virtualPeers.end())
        return;

    std::string user = it->second.peer.user();

    // Broadcast offline presence
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + peerId;
    presence["data"] = static_cast<const json::Value&>(it->second.peer);
    presence["data"]["online"] = false;
    for (const auto& room : it->second.rooms) {
        broadcast(room, presence, peerId);
    }

    removeFromAllRooms(peerId);
    _virtualPeers.erase(it);

    LInfo("Virtual peer removed: ", user, "|", peerId);
}


} // namespace smpl
} // namespace icy
