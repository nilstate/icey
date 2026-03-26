///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/http/server.h"
#include "icy/http/websocket.h"
#include "icy/json/json.h"
#include "icy/logger.h"
#include "icy/loop.h"
#include "icy/ratelimiter.h"
#include "icy/symple/peer.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>


namespace icy {
namespace smpl {


/// Per-connection state for a connected Symple peer.
///
/// Created by the server after successful authentication.
/// Holds the peer data, room memberships, and a reference
/// to the underlying WebSocket connection for sending.
class ServerPeer
{
public:
    /// Constructs a peer bound to the given server-side connection.
    /// @param conn The underlying WebSocket server connection.
    ServerPeer(http::ServerConnection& conn);

    /// Serialises and sends a JSON message over the WebSocket connection.
    /// Logs a warning if the send fails; does not throw.
    /// @param msg JSON value to send.
    void send(const json::Value& msg);

    /// Sends a pre-serialized JSON payload over the WebSocket connection.
    /// Use this on fanout paths that already serialized once.
    void sendSerialized(const char* data, size_t len);

    /// Adds this peer to the named room (local tracking only).
    /// @param room Room name to join.
    void join(const std::string& room);

    /// Removes this peer from the named room (local tracking only).
    /// @param room Room name to leave.
    void leave(const std::string& room);

    /// Removes this peer from all rooms (local tracking only).
    void leaveAll();

    /// Returns a mutable reference to the peer data object.
    [[nodiscard]] Peer& peer() { return _peer; }

    /// Returns a const reference to the peer data object.
    [[nodiscard]] const Peer& peer() const { return _peer; }

    /// Returns the session ID assigned to this peer.
    [[nodiscard]] std::string id() const;

    /// Returns the set of room names this peer is currently joined to.
    [[nodiscard]] const std::unordered_set<std::string>& rooms() const { return _rooms; }

    /// Returns true if the peer has completed authentication.
    [[nodiscard]] bool authenticated() const { return _authenticated; }

    /// Marks the peer as authenticated or unauthenticated.
    /// @param v True to mark as authenticated.
    void setAuthenticated(bool v) { _authenticated = v; }

    /// Replaces the peer's data object.
    /// @param p New peer data.
    void setPeer(const Peer& p) { _peer = p; }

    /// Returns a reference to the underlying server connection.
    http::ServerConnection& connection() { return _conn; }

    /// Per-peer rate limiter. Returns false if message should be dropped.
    [[nodiscard]] bool checkRate() { return _rateLimiter.canSend(); }

    /// Configures the per-peer rate limit.
    /// @param rate    Maximum messages allowed per window.
    /// @param seconds Duration of the rate window in seconds.
    void setRateLimit(double rate, double seconds) {
        _rateLimiter.rate = rate;
        _rateLimiter.seconds = seconds;
        _rateLimiter.allowance = rate;
    }

private:
    http::ServerConnection& _conn;
    Peer _peer;
    std::unordered_set<std::string> _rooms;
    RateLimiter _rateLimiter{100.0, 10.0}; ///< 100 messages per 10 seconds default
    bool _authenticated = false;
};


/// Symple v4 server.
///
/// Accepts WebSocket connections, authenticates peers, manages rooms,
/// and routes messages. Implements the Symple v4 protocol over
/// native WebSocket.
///
/// Usage:
///   smpl::Server server;
///   server.start({.port = 4500});
///
///   // Optional: custom authentication
///   server.Authenticate += [](ServerPeer& peer, const json::Value& auth,
///                              bool& allowed, std::vector<std::string>& rooms) {
///       allowed = (auth.value("token", "") == "secret");
///       rooms.push_back("team-a");
///   };
///
/// The server also serves as an HTTP server, so you can serve
/// static files (e.g. a web UI) on the same port.
class Server
{
public:
    /// Configuration options for the Symple server
    struct Options
    {
        std::string host = "0.0.0.0";
        uint16_t port = 4500;
        bool authentication = false;  ///< Require token in auth message
        bool dynamicRooms = true;     ///< Allow clients to join/leave rooms

        // Production hardening
        size_t maxConnections = 0;    ///< Max WebSocket connections (0 = unlimited)
        size_t maxMessageSize = 64 * 1024; ///< Max message payload in bytes (64KB default)
        double rateLimit = 100.0;     ///< Messages per rate window
        double rateSeconds = 10.0;    ///< Rate window in seconds
    };

    /// Constructs a server using the given event loop.
    /// @param loop libuv event loop; defaults to uv::defaultLoop().
    Server(uv::Loop* loop = uv::defaultLoop());
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    /// Starts the server with the given options.
    /// Begins accepting WebSocket connections on opts.host:opts.port.
    /// @param opts Server configuration options.
    void start(const Options& opts);

    /// Starts the server with a custom HTTP factory for non-WebSocket requests.
    /// The Symple server handles WebSocket upgrades internally; any other
    /// HTTP request (e.g. static files, REST API) is delegated to this factory.
    /// @param opts        Server configuration options.
    /// @param httpFactory Factory for HTTP responders; may be nullptr.
    void start(const Options& opts,
               std::unique_ptr<http::ServerConnectionFactory> httpFactory);

    /// Broadcasts a shutdown notice to all peers, closes the listen socket,
    /// and releases all internal state. Safe to call more than once.
    void shutdown();

    /// Broadcast a message to all peers in a room (excluding sender).
    void broadcast(const std::string& room, const json::Value& msg,
                   const std::string& excludeId = {});

    /// Broadcast to multiple rooms with per-recipient dedup.
    void broadcastRooms(const std::unordered_set<std::string>& rooms,
                        const json::Value& msg,
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

    /// Custom authentication hook.
    /// Set `allowed` to false to reject the peer.
    /// Append to `rooms` to assign team/group memberships.
    /// If not connected, all peers with a valid `user` field are accepted.
    Signal<void(ServerPeer&, const json::Value& auth, bool& allowed, std::vector<std::string>& rooms)> Authenticate;

    /// Peer authenticated and online.
    Signal<void(ServerPeer&)> PeerConnected;

    /// Peer disconnected.
    Signal<void(ServerPeer&)> PeerDisconnected;

    /// Register a virtual peer that receives messages via callback.
    ///
    /// The virtual peer appears in presence broadcasts and is routable
    /// like any WebSocket-connected peer. Messages addressed to it are
    /// delivered via the callback instead of a WebSocket connection.
    ///
    /// @param peer     Peer data (must have "id", "user", "name" fields).
    /// @param rooms    Rooms the virtual peer joins.
    /// @param handler  Called when a message is routed to this peer.
    void addVirtualPeer(const Peer& peer,
                        const std::vector<std::string>& rooms,
                        std::function<void(const json::Value&)> handler);

    /// Remove a virtual peer by session ID.
    void removeVirtualPeer(const std::string& peerId);

    /// Access the underlying HTTP server (e.g. to serve static files).
    [[nodiscard]] http::Server& httpServer() { return *_http; }

    /// Event loop that owns the Symple server and all peer connections.
    [[nodiscard]] uv::Loop* loop() const { return _loop; }

private:
    /// Handler for incoming Symple protocol messages
    class Responder;
    /// Factory for creating server-side peer connections
    class Factory;
    struct VirtualPeer;
    class PeerRegistry;
    class RoomIndex;
    struct RoutingPolicy;
    void onAuth(ServerPeer& peer, const json::Value& msg, std::unique_lock<std::mutex>& lock);
    void onMessage(ServerPeer& peer, json::Value msg);
    void onJoin(ServerPeer& peer, const std::string& room);
    void onLeave(ServerPeer& peer, const std::string& room);
    void onDisconnect(ServerPeer& peer, std::unique_lock<std::mutex>& lock);
    void route(ServerPeer& sender, const json::Value& msg);
    bool deliver(const std::string& peerId, const json::Value& msg);
    bool deliverSerialized(const std::string& peerId,
                           const char* data,
                           size_t len,
                           const json::Value& msg);
    void sendPresenceSnapshot(ServerPeer& recipient,
                              const std::unordered_set<std::string>& rooms,
                              std::string_view excludeId = {});

    Options _opts;
    uv::Loop* _loop = nullptr;
    std::unique_ptr<http::Server> _http;
    std::unique_ptr<PeerRegistry> _peerRegistry;
    std::unique_ptr<RoomIndex> _roomIndex;

    mutable std::mutex _mutex;
    std::atomic<bool> _shuttingDown{false};

    /// Fallback factory for non-WebSocket HTTP requests.
    std::unique_ptr<http::ServerConnectionFactory> _httpFallback;

    friend class Factory;
};


} // namespace smpl
} // namespace icy


/// @}
