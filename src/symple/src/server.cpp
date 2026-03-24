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

#include <string_view>


namespace icy {
namespace smpl {

namespace {

bool isReservedPresenceField(std::string_view key)
{
    return key == "id" ||
           key == "user" ||
           key == "online" ||
           key == "name" ||
           key == "type";
}

} // namespace


json::Value Server::PresenceBuilder::make(const Peer& peer,
                                         const std::string& id,
                                         bool online,
                                         const json::Value* extra)
{
    json::Value data = json::Value::object();
    if (extra && extra->is_object()) {
        for (const auto& [key, value] : extra->items()) {
            if (!isReservedPresenceField(key))
                data[key] = value;
        }
    }

    data["id"] = id;
    data["user"] = peer.user();
    data["online"] = online;
    if (!peer.name().empty())
        data["name"] = peer.name();
    if (!peer.type().empty())
        data["type"] = peer.type();
    return data;
}

void Server::PresenceBuilder::rewrite(json::Value& data,
                                      const Peer& peer,
                                      const std::string& id,
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

    data["id"] = id;
    data["user"] = peer.user();
    data["online"] = online;
    if (!peer.name().empty())
        data["name"] = peer.name();
    if (!peer.type().empty())
        data["type"] = peer.type();
}


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
    size_t expectedRecipients = 0;
    for (const auto& room : rooms) {
        auto it = _rooms.find(room);
        if (it != _rooms.end())
            expectedRecipients += it->second.size();
    }

    std::unordered_set<std::string> recipients;
    recipients.reserve(expectedRecipients);
    for (const auto& room : rooms) {
        auto it = _rooms.find(room);
        if (it == _rooms.end())
            continue;
        for (const auto& peerId : it->second) {
            if (peerId != excludeId)
                recipients.insert(peerId);
        }
    }
    return recipients;
}


bool Server::RoutingPolicy::sharesAnyRoom(const std::unordered_set<std::string>& a,
                                          const std::unordered_set<std::string>& b)
{
    for (const auto& room : a) {
        if (b.count(room))
            return true;
    }
    return false;
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
            std::unique_lock<std::mutex> lock(_server._mutex);
            auto* peer = _server._peerRegistry.findByConnection(connection());

            if (!peer) {
                // Not authenticated: only accept auth
                if (type == "auth") {
                    _server.onAuth(_tempPeer, msg, lock);
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
                // Per-peer rate limiting
                if (!peer->checkRate()) {
                    LWarn("Rate limit exceeded for peer ", peer->id());
                    sendError(429, "Rate limit exceeded");
                    shouldClose = true;
                }
                else if (type == "message" || type == "presence" ||
                    type == "command" || type == "event") {
                    _server.onMessage(*peer, std::move(msg));
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
        std::unique_lock<std::mutex> lock(_server._mutex);
        if (auto* peer = _server._peerRegistry.findByConnection(connection()))
            _server.onDisconnect(*peer, lock);
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

        // Max connections check. Count all active sockets (including
        // unauthenticated ones in the auth timeout window), not just
        // authenticated peers, to prevent connection-flood bypass.
        // The current accepted socket is already present in the HTTP
        // server's live connection map by the time we create a responder,
        // so allow `maxConnections` total by rejecting only once the count
        // exceeds the limit.
        if (_server._opts.maxConnections > 0 &&
            _server._http->connectionCount() > static_cast<size_t>(_server._opts.maxConnections)) {
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
        for (const auto& [id, peer] : _peerRegistry.peers()) {
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
        _roomIndex.clear();
        _peerRegistry.clear();
    }

    // Release the HTTP server. Its destructor calls shutdown() again
    // (no-op due to _shuttingDown guard on the http side - the socket
    // is already closed). Connections and responders are destroyed here.
    _http.reset();

    LInfo("Symple server shut down");
}


void Server::onAuth(ServerPeer& tempPeer,
                    const json::Value& msg,
                    std::unique_lock<std::mutex>& lock)
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
            if (isReservedPresenceField(k))
                continue;
            p[k] = v;
        }
    }

    // Custom authentication hook.
    // The hook can reject the peer and/or assign room memberships.
    std::vector<std::string> authRooms;
    if (_opts.authentication || Authenticate.nslots() > 0) {
        bool allowed = !_opts.authentication;
        lock.unlock();
        try {
            Authenticate.emit(tempPeer, msg, allowed, authRooms);
            lock.lock();
        }
        catch (...) {
            lock.lock();
            throw;
        }
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
    _roomIndex.join(user, id);

    // Auto-join rooms from auth message
    if (msg.contains("rooms") && msg["rooms"].is_array()) {
        for (const auto& room : msg["rooms"]) {
            if (room.is_string() && !room.get<std::string>().empty()) {
                std::string r = room.get<std::string>();
                peer->join(r);
                _roomIndex.join(r, id);
            }
        }
    }

    // Auto-join rooms assigned by the Authenticate hook
    for (const auto& room : authRooms) {
        if (!room.empty()) {
            peer->join(room);
            _roomIndex.join(room, id);
        }
    }

    auto* registeredPeer = peer.get();
    _peerRegistry.bind(tempPeer.connection(), id);
    _peerRegistry.add(id, std::move(peer));

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

    // Seed the client with the peers already visible in its rooms.
    // Virtual peers do not answer presence probes, so discovery needs
    // to be deterministic when a session comes online.
    sendPresenceSnapshot(*registeredPeer, registeredPeer->rooms(), id);

    LInfo("Peer authenticated: ", user, "|", id);

    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = PresenceBuilder::make(p, id, true, &p);
    broadcastRooms(registeredPeer->rooms(), presence, id);

    // Notify server hooks (release lock for signal emission)
    lock.unlock();
    try {
        PeerConnected.emit(*registeredPeer);
        lock.lock();
    }
    catch (...) {
        lock.lock();
        throw;
    }
}


void Server::onMessage(ServerPeer& sender, json::Value msg)
{
    // Caller holds _mutex.

    // Enforce that the from field matches the authenticated peer.
    std::string expectedFrom = sender.peer().user() + "|" + sender.id();
    msg["from"] = expectedFrom;

    if (msg.value("type", "") == "presence") {
        PresenceBuilder::rewrite(msg["data"], sender.peer(), sender.id(), true);
    }

    route(sender, msg);
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
    _roomIndex.join(room, peer.id());

    json::Value ok;
    ok["type"] = "join:ok";
    ok["room"] = room;
    peer.send(ok);

    sendPresenceSnapshot(peer, std::unordered_set<std::string>{room}, peer.id());

    LDebug("Peer ", peer.id(), " joined room: ", room);
}


void Server::onLeave(ServerPeer& peer, const std::string& room)
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

    peer.leave(room);
    _roomIndex.leave(room, peer.id());

    json::Value ok;
    ok["type"] = "leave:ok";
    ok["room"] = room;
    peer.send(ok);

    LDebug("Peer ", peer.id(), " left room: ", room);
}


void Server::onDisconnect(ServerPeer& peer, std::unique_lock<std::mutex>& lock)
{
    // Caller holds _mutex.

    std::string id = peer.id();
    std::string user = peer.peer().user();

    LInfo("Peer disconnected: ", user, "|", id);

    // Broadcast offline presence to all rooms
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = PresenceBuilder::make(peer.peer(), id, false, &peer.peer());
    broadcastRooms(peer.rooms(), presence, id);

    // Clean up
    _roomIndex.leaveAll(id);
    _peerRegistry.unbind(peer.connection());

    // Notify hooks (release lock)
    lock.unlock();
    try {
        PeerDisconnected.emit(peer);
        lock.lock();
    }
    catch (...) {
        lock.lock();
        throw;
    }

    _peerRegistry.erase(id);
}


void Server::route(ServerPeer& sender, const json::Value& msg)
{
    // Caller holds _mutex.

    if (!msg.contains("to")) {
        // No recipient: broadcast to all sender's rooms, deduped.
        broadcastRooms(sender.rooms(), msg, sender.id());
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
            if (auto* target = _peerRegistry.find(peerId)) {
                if (RoutingPolicy::canDirectMessage(sender, *target)) {
                    deliver(peerId, msg);
                } else {
                    LDebug("Blocked direct message from ", sender.id(), " to ", peerId, " (no shared room)");
                }
                return;
            }

            // Check virtual peers
            if (auto* target = _peerRegistry.findVirtual(peerId)) {
                if (RoutingPolicy::canDirectMessage(sender, *target)) {
                    deliver(peerId, msg);
                } else {
                    LDebug("Blocked direct message from ", sender.id(), " to virtual peer ", peerId, " (no shared room)");
                }
            }
            return;
        }

        // User name only: send to that user's sessions if at least one
        // room is shared with the sender.
        std::string user = addr;
        const auto* members = _roomIndex.members(user);
        if (!members)
            return;

        auto str = msg.dump();
        for (const auto& peerId : *members) {
            if (peerId == sender.id())
                continue;

            if (auto* target = _peerRegistry.find(peerId)) {
                if (!RoutingPolicy::canDirectMessage(sender, *target)) {
                    LDebug("Blocked direct message from ", sender.id(), " to ", peerId, " (no shared room)");
                    continue;
                }
                deliverSerialized(peerId, str.c_str(), str.size(), msg);
                continue;
            }

            if (auto* target = _peerRegistry.findVirtual(peerId)) {
                if (!RoutingPolicy::canDirectMessage(sender, *target)) {
                    LDebug("Blocked direct message from ", sender.id(), " to virtual peer ", peerId, " (no shared room)");
                    continue;
                }
                deliverSerialized(peerId, str.c_str(), str.size(), msg);
            }
        }
    }
    else if (to.is_array()) {
        std::unordered_set<std::string> rooms;
        for (const auto& room : to) {
            if (room.is_string()) {
                auto roomName = room.get<std::string>();
                if (RoutingPolicy::canBroadcastToRoom(sender, roomName))
                    rooms.insert(roomName);
                else
                    LDebug("Blocked broadcast from ", sender.id(), " to room ", roomName, " (not a member)");
            }
        }
        broadcastRooms(rooms, msg, sender.id());
    }
}


void Server::broadcastRooms(const std::unordered_set<std::string>& rooms,
                             const json::Value& msg,
                             const std::string& excludeId)
{
    auto recipients = _roomIndex.collectRecipients(rooms, excludeId);

    // Single serialization, single send per recipient.
    auto str = msg.dump();
    for (const auto& peerId : recipients) {
        deliverSerialized(peerId, str.c_str(), str.size(), msg);
    }
}


void Server::broadcast(const std::string& room, const json::Value& msg,
                        const std::string& excludeId)
{
    const auto* members = _roomIndex.members(room);
    if (!members)
        return;

    auto str = msg.dump();
    for (const auto& peerId : *members) {
        if (peerId == excludeId)
            continue;
        deliverSerialized(peerId, str.c_str(), str.size(), msg);
    }
}


bool Server::sendTo(const std::string& peerId, const json::Value& msg)
{
    return deliver(peerId, msg);
}


bool Server::sendToUser(const std::string& user, const json::Value& msg)
{
    const auto* members = _roomIndex.members(user);
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


ServerPeer* Server::getPeer(const std::string& id)
{
    std::lock_guard lock(_mutex);
    return _peerRegistry.find(id);
}


std::vector<ServerPeer*> Server::getPeersInRoom(const std::string& room)
{
    std::lock_guard lock(_mutex);
    std::vector<ServerPeer*> result;
    if (const auto* members = _roomIndex.members(room)) {
        result.reserve(members->size());
        for (const auto& peerId : *members) {
            if (auto* peer = _peerRegistry.find(peerId))
                result.push_back(peer);
        }
    }
    return result;
}


size_t Server::peerCount() const
{
    std::lock_guard lock(_mutex);
    return _peerRegistry.size();
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
    _roomIndex.join(user, id);

    // Join requested rooms
    for (const auto& room : rooms) {
        if (!room.empty()) {
            vp.rooms.insert(room);
            _roomIndex.join(room, id);
        }
    }

    _peerRegistry.addVirtual(id, std::move(vp));

    // Broadcast online presence
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = PresenceBuilder::make(peer, id, true, &peer);
    if (auto* virtualPeer = _peerRegistry.findVirtual(id))
        broadcastRooms(virtualPeer->rooms, presence, id);

    LInfo("Virtual peer registered: ", user, "|", id);
}


void Server::removeVirtualPeer(const std::string& peerId)
{
    std::lock_guard lock(_mutex);

    auto* virtualPeer = _peerRegistry.findVirtual(peerId);
    if (!virtualPeer)
        return;

    std::string user = virtualPeer->peer.user();

    // Broadcast offline presence
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + peerId;
    presence["data"] = PresenceBuilder::make(
        virtualPeer->peer, peerId, false, &virtualPeer->peer);
    broadcastRooms(virtualPeer->rooms, presence, peerId);

    _roomIndex.leaveAll(peerId);
    _peerRegistry.eraseVirtual(peerId);

    LInfo("Virtual peer removed: ", user, "|", peerId);
}


bool Server::deliver(const std::string& peerId, const json::Value& msg)
{
    if (_peerRegistry.find(peerId)) {
        auto str = msg.dump();
        return deliverSerialized(peerId, str.c_str(), str.size(), msg);
    }

    if (auto* peer = _peerRegistry.findVirtual(peerId)) {
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
    if (auto* peer = _peerRegistry.find(peerId)) {
        try {
            peer->sendSerialized(data, len);
            return true;
        }
        catch (const std::exception& e) {
            LWarn("Delivery failed to ", peerId, ": ", e.what());
            return false;
        }
    }

    if (auto* peer = _peerRegistry.findVirtual(peerId)) {
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


void Server::sendPresenceSnapshot(ServerPeer& recipient,
                                  const std::unordered_set<std::string>& rooms,
                                  std::string_view excludeId)
{
    auto recipients = _roomIndex.collectRecipients(rooms, excludeId);

    for (const auto& peerId : recipients) {
        json::Value presence;
        presence["type"] = "presence";

        if (auto* peer = _peerRegistry.find(peerId)) {
            presence["from"] = peer->peer().user() + "|" + peer->id();
            presence["data"] = PresenceBuilder::make(peer->peer(), peer->id(), true, &peer->peer());
            recipient.send(presence);
            continue;
        }

        if (auto* virtualPeer = _peerRegistry.findVirtual(peerId)) {
            presence["from"] = virtualPeer->peer.user() + "|" + virtualPeer->peer.id();
            presence["data"] = PresenceBuilder::make(
                virtualPeer->peer, virtualPeer->peer.id(), true, &virtualPeer->peer);
            recipient.send(presence);
        }
    }
}


} // namespace smpl
} // namespace icy
