///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/http/server.h"
#include "scy/http/websocket.h"
#include "scy/json/json.h"
#include "scy/logger.h"
#include "scy/symple/peer.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>


namespace scy {
namespace smpl {


/// Per-connection state for a connected Symple peer.
///
/// Created by the server after successful authentication.
/// Holds the peer data, room memberships, and a reference
/// to the underlying WebSocket connection for sending.
class ServerPeer
{
public:
    ServerPeer(http::ServerConnection& conn);

    /// Send a JSON message to this peer.
    void send(const json::Value& msg);

    /// Join a room.
    void join(const std::string& room);

    /// Leave a room.
    void leave(const std::string& room);

    /// Leave all rooms.
    void leaveAll();

    [[nodiscard]] Peer& peer() { return _peer; }
    [[nodiscard]] const Peer& peer() const { return _peer; }
    [[nodiscard]] std::string id() const;
    [[nodiscard]] const std::unordered_set<std::string>& rooms() const { return _rooms; }
    [[nodiscard]] bool authenticated() const { return _authenticated; }

    void setAuthenticated(bool v) { _authenticated = v; }
    void setPeer(const Peer& p) { _peer = p; }

    http::ServerConnection& connection() { return _conn; }

private:
    http::ServerConnection& _conn;
    Peer _peer;
    std::unordered_set<std::string> _rooms;
    bool _authenticated = false;
};


/// Symple v4 server.
///
/// Accepts WebSocket connections, authenticates peers, manages rooms,
/// and routes messages. Implements the Symple v4 protocol over
/// native WebSocket (no Socket.IO).
///
/// Usage:
///   smpl::Server server;
///   server.start({.port = 4500});
///
///   // Optional: custom authentication
///   server.Authenticate += [](ServerPeer& peer, const json::Value& auth, bool& allowed) {
///       allowed = (auth.value("token", "") == "secret");
///   };
///
/// The server also serves as an HTTP server, so you can serve
/// static files (e.g. a web UI) on the same port.
class Server
{
public:
    struct Options
    {
        std::string host = "0.0.0.0";
        uint16_t port = 4500;
        bool authentication = false;  ///< Require token in auth message
        bool dynamicRooms = true;     ///< Allow clients to join/leave rooms
    };

    Server();
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void start(const Options& opts);
    void shutdown();

    /// Broadcast a message to all peers in a room (excluding sender).
    void broadcast(const std::string& room, const json::Value& msg,
                   const std::string& excludeId = {});

    /// Send a message to a specific peer by session ID.
    bool sendTo(const std::string& peerId, const json::Value& msg);

    /// Send a message to any peer with the given user name.
    bool sendToUser(const std::string& user, const json::Value& msg);

    /// Get a connected peer by session ID.
    [[nodiscard]] ServerPeer* getPeer(const std::string& id);

    /// Get all peers in a room.
    [[nodiscard]] std::vector<ServerPeer*> getPeersInRoom(const std::string& room);

    /// Number of connected, authenticated peers.
    [[nodiscard]] size_t peerCount() const;

    //
    // Signals
    //

    /// Custom authentication hook. Set `allowed` to false to reject.
    /// If not connected, all peers with a valid `user` field are accepted.
    Signal<void(ServerPeer&, const json::Value& auth, bool& allowed)> Authenticate;

    /// Peer authenticated and online.
    Signal<void(ServerPeer&)> PeerConnected;

    /// Peer disconnected.
    Signal<void(ServerPeer&)> PeerDisconnected;

    /// Access the underlying HTTP server (e.g. to serve static files).
    [[nodiscard]] http::Server& httpServer() { return *_http; }

private:
    class Responder;
    class Factory;

    void onAuth(ServerPeer& peer, const json::Value& msg);
    void onMessage(ServerPeer& peer, const json::Value& msg);
    void onJoin(ServerPeer& peer, const std::string& room);
    void onLeave(ServerPeer& peer, const std::string& room);
    void onDisconnect(ServerPeer& peer);
    void route(ServerPeer& sender, const json::Value& msg);

    void addToRoom(const std::string& room, const std::string& peerId);
    void removeFromRoom(const std::string& room, const std::string& peerId);
    void removeFromAllRooms(const std::string& peerId);

    std::string generateId();

    Options _opts;
    std::unique_ptr<http::Server> _http;

    // Peers indexed by session ID
    std::unordered_map<std::string, std::unique_ptr<ServerPeer>> _peers;

    // Rooms: room name -> set of session IDs
    std::unordered_map<std::string, std::unordered_set<std::string>> _rooms;

    // Connection pointer -> session ID (for lookup in responder callbacks)
    std::unordered_map<http::ServerConnection*, std::string> _connToPeer;

    mutable std::mutex _mutex;
};


} // namespace smpl
} // namespace scy


/// @\}
