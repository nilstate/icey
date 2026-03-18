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

#include <random>


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
    _conn.send(str.c_str(), str.size(), http::ws::Text);
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


const std::string& ServerPeer::id() const
{
    // Peer::id() returns by value, but we need a stable reference.
    // The underlying json::Value stores the string, so access it directly.
    static const std::string empty;
    auto it = _peer.find("id");
    if (it != _peer.end() && it->is_string())
        return it->get_ref<const std::string&>();
    return empty;
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
    {
    }

    void onPayload(const MutableBuffer& buffer) override
    {
        // Parse JSON from WebSocket text frame
        std::string str(bufferCast<const char*>(buffer), buffer.size());

        try {
            auto msg = json::Value::parse(str);
            std::string type = msg.value("type", "");

            std::lock_guard lock(_server._mutex);

            // Look up the peer for this connection
            auto it = _server._connToPeer.find(&connection());

            if (it == _server._connToPeer.end()) {
                // Not authenticated yet - only accept auth messages
                if (type == "auth") {
                    _server.onAuth(*_peer, msg);
                }
                else {
                    // Send error and close
                    json::Value err;
                    err["type"] = "error";
                    err["status"] = 401;
                    err["message"] = "Not authenticated";
                    auto errStr = err.dump();
                    connection().send(errStr.c_str(), errStr.size(), http::ws::Text);
                    connection().close();
                }
                return;
            }

            auto* peer = _server._peers[it->second].get();
            if (!peer)
                return;

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
        catch (const std::exception& e) {
            LWarn("Symple server: parse error: ", e.what());
        }
    }

    void onClose() override
    {
        std::lock_guard lock(_server._mutex);
        auto it = _server._connToPeer.find(&connection());
        if (it != _server._connToPeer.end()) {
            auto peerIt = _server._peers.find(it->second);
            if (peerIt != _server._peers.end()) {
                _server.onDisconnect(*peerIt->second);
            }
        }
    }

    // Temporary peer object used during auth before the peer is registered
    std::unique_ptr<ServerPeer> _peer;
    Server& _server;
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

        auto responder = std::make_unique<Server::Responder>(conn, _server);
        // Create a temporary peer for auth phase
        responder->_peer = std::make_unique<ServerPeer>(conn);
        return responder;
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

    // Disconnect all peers
    for (auto& [id, peer] : _peers) {
        try {
            json::Value msg;
            msg["type"] = "presence";
            msg["from"] = peer->peer().address().toString();
            msg["data"] = peer->peer();
            msg["data"]["online"] = false;
            // Don't broadcast - we're shutting down
        }
        catch (...) {}
    }

    _peers.clear();
    _rooms.clear();
    _connToPeer.clear();

    if (_http)
        _http->shutdown();
    _http.reset();

    LInfo("Symple server shut down");
}


void Server::onAuth(ServerPeer& peer, const json::Value& msg)
{
    std::string user = msg.value("user", "");
    if (user.empty()) {
        json::Value err;
        err["type"] = "error";
        err["status"] = 401;
        err["message"] = "Missing user field";
        peer.send(err);
        peer.connection().close();
        return;
    }

    // Generate session ID
    std::string id = generateId();

    // Build peer object
    Peer p;
    p["id"] = id;
    p["user"] = user;
    p["name"] = msg.value("name", user);
    p["online"] = true;

    // Copy any extra data from auth message
    if (msg.contains("data") && msg["data"].is_object()) {
        for (auto& [k, v] : msg["data"].items()) {
            p[k] = v;
        }
    }

    peer.setPeer(p);
    peer.setAuthenticated(true);

    // Custom authentication hook
    if (_opts.authentication || Authenticate.nslots() > 0) {
        bool allowed = !_opts.authentication;  // Default allowed if no auth required
        Authenticate.emit(peer, msg, allowed);
        if (!allowed) {
            json::Value err;
            err["type"] = "error";
            err["status"] = 401;
            err["message"] = "Authentication failed";
            peer.send(err);
            peer.connection().close();
            return;
        }
    }

    // Register the peer
    auto peerPtr = std::make_unique<ServerPeer>(peer.connection());
    peerPtr->setPeer(p);
    peerPtr->setAuthenticated(true);

    // Auto-join user room (enables direct messaging by user identity)
    peerPtr->join(user);
    addToRoom(user, id);

    _connToPeer[&peer.connection()] = id;
    auto* registeredPeer = peerPtr.get();
    _peers[id] = std::move(peerPtr);

    // Send welcome
    json::Value welcome;
    welcome["type"] = "welcome";
    welcome["peer"] = p;
    welcome["status"] = 200;
    registeredPeer->send(welcome);

    LInfo("Peer authenticated: ", user, "|", id);

    // Broadcast online presence to all rooms
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = p;
    presence["data"]["online"] = true;
    broadcast(user, presence, id);

    // Notify server hooks (outside lock ideally, but we're already locked)
    _mutex.unlock();
    PeerConnected.emit(*registeredPeer);
    _mutex.lock();
}


void Server::onMessage(ServerPeer& sender, const json::Value& msg)
{
    // Validate message has from field
    if (!msg.contains("from") || !msg["from"].is_string()) {
        LDebug("Dropping message without from field");
        return;
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
    addToRoom(room, peer.peer().id());

    json::Value ok;
    ok["type"] = "join:ok";
    ok["room"] = room;
    peer.send(ok);

    LDebug("Peer ", peer.peer().id(), " joined room: ", room);
}


void Server::onLeave(ServerPeer& peer, const std::string& room)
{
    if (room.empty())
        return;

    peer.leave(room);
    removeFromRoom(room, peer.peer().id());

    json::Value ok;
    ok["type"] = "leave:ok";
    ok["room"] = room;
    peer.send(ok);

    LDebug("Peer ", peer.peer().id(), " left room: ", room);
}


void Server::onDisconnect(ServerPeer& peer)
{
    std::string id = peer.peer().id();
    std::string user = peer.peer().user();

    LInfo("Peer disconnected: ", user, "|", id);

    // Broadcast offline presence
    json::Value presence;
    presence["type"] = "presence";
    presence["from"] = user + "|" + id;
    presence["data"] = peer.peer();
    presence["data"]["online"] = false;

    // Broadcast to all rooms the peer was in
    for (const auto& room : peer.rooms()) {
        broadcast(room, presence, id);
    }

    // Clean up
    removeFromAllRooms(id);
    _connToPeer.erase(&peer.connection());

    _mutex.unlock();
    PeerDisconnected.emit(peer);
    _mutex.lock();

    _peers.erase(id);
}


void Server::route(ServerPeer& sender, const json::Value& msg)
{
    if (!msg.contains("to")) {
        // No recipient - broadcast to all sender's rooms (excluding sender)
        for (const auto& room : sender.rooms()) {
            broadcast(room, msg, sender.peer().id());
        }
        return;
    }

    const auto& to = msg["to"];

    if (to.is_string()) {
        std::string addr = to.get<std::string>();

        // Try direct peer lookup first (user|id format)
        auto pos = addr.find('|');
        if (pos != std::string::npos) {
            std::string peerId = addr.substr(pos + 1);
            if (sendTo(peerId, msg))
                return;
        }

        // Try user room (sends to all sessions of that user)
        sendToUser(addr.substr(0, pos != std::string::npos ? pos : addr.size()), msg);
    }
    else if (to.is_array()) {
        // Send to multiple rooms
        for (const auto& room : to) {
            if (room.is_string())
                broadcast(room.get<std::string>(), msg, sender.peer().id());
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
                LWarn("Broadcast send failed to ", peerId, ": ", e.what());
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
        LWarn("sendTo failed for ", peerId, ": ", e.what());
        return false;
    }
}


bool Server::sendToUser(const std::string& user, const json::Value& msg)
{
    // Send to all peers in the user's room
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


std::string Server::generateId()
{
    static thread_local std::mt19937 gen(std::random_device{}());
    static constexpr char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::uniform_int_distribution<size_t> dist(0, sizeof(chars) - 2);

    std::string id;
    id.reserve(16);
    for (int i = 0; i < 16; ++i)
        id += chars[dist(gen)];
    return id;
}


} // namespace smpl
} // namespace scy
