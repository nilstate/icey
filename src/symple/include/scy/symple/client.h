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


#include "scy/http/client.h"
#include "scy/http/websocket.h"
#include "scy/packetsignal.h"
#include "scy/stateful.h"
#include "scy/symple/command.h"
#include "scy/symple/event.h"
#include "scy/symple/message.h"
#include "scy/symple/peer.h"
#include "scy/symple/presence.h"
#include "scy/symple/roster.h"
#include "scy/symple/symple.h"
#include "scy/timer.h"

#include <string>
#include <vector>


namespace scy {
namespace smpl {


/// Client connection states.
struct ClientState : public State
{
    enum Type
    {
        Closed = 0x00,
        Connecting = 0x01,
        Authenticating = 0x02,
        Online = 0x04,
        Error = 0x08
    };

    std::string str(unsigned int id) const override
    {
        switch (id) {
        case Closed:         return "Closed";
        case Connecting:     return "Connecting";
        case Authenticating: return "Authenticating";
        case Online:         return "Online";
        case Error:          return "Error";
        }
        return "undefined";
    };
};


/// Symple v4 client.
///
/// Connects to a Symple server over plain WebSocket (no Socket.IO).
/// Handles authentication, presence, roster management, rooms, and
/// message routing.
///
/// Usage:
///   smpl::Client client({
///       .host = "localhost",
///       .port = 4500,
///       .user = "alice",
///       .name = "Alice"
///   });
///   client.connect();
///
///   client += packetSlot(&handler, &Handler::onMessage);
///   client.Announce += slot(&handler, &Handler::onAnnounce);
///
/// Messages are emitted as polymorphic IPacket via PacketSignal:
///   - smpl::Message (type "message")
///   - smpl::Presence (type "presence")
///   - smpl::Event (type "event")
///   - smpl::Command (type "command")
///
/// Use packetSlot() with the specific type to filter.
class Symple_API Client : public Stateful<ClientState>
                        , public PacketSignal
{
public:
    struct Options
    {
        std::string host = "127.0.0.1";
        uint16_t port = 4500;
        bool secure = false;           ///< Use wss:// instead of ws://
        bool reconnection = true;      ///< Auto-reconnect on disconnect
        int reconnectAttempts = 0;     ///< 0 = unlimited
        int reconnectDelay = 3000;     ///< Milliseconds between retries

        std::string user;              ///< User identifier (required)
        std::string name;              ///< Display name
        std::string type;              ///< Peer type
        std::string token;             ///< Auth token (optional)

        Options() = default;
    };

    Client(const Options& options,
           uv::Loop* loop = uv::defaultLoop());
    Client(); ///< Default constructor with default Options.
    virtual ~Client();

    /// Connect to the Symple server.
    void connect();

    /// Disconnect from the server.
    void close();

    /// Send a Symple message. Sets the `from` field automatically.
    virtual int send(Message& message);

    /// Send a string message (parsed as JSON).
    virtual int send(const std::string& message);

    /// Swap to/from and send.
    virtual int respond(Message& message);

    /// Broadcast presence to joined rooms.
    virtual int sendPresence(bool probe = false);

    /// Send directed presence to a specific peer.
    virtual int sendPresence(const Address& to, bool probe = false);

    /// Join a room on the server.
    virtual int joinRoom(const std::string& room);

    /// Leave a room on the server.
    virtual int leaveRoom(const std::string& room);

    /// Return true if in Online state.
    [[nodiscard]] bool isOnline() const;

    /// Return the session ID assigned by the server.
    [[nodiscard]] std::string ourID() const;

    /// Return the local peer object (null if offline).
    [[nodiscard]] Peer* ourPeer();

    /// Return joined rooms.
    [[nodiscard]] StringVec rooms() const;

    /// Return the roster of online peers.
    [[nodiscard]] Roster& roster();

    /// Return client options.
    [[nodiscard]] Options& options();

    /// Set an error and transition to Error state.
    void setError(const std::string& error);

    //
    // Signals
    //

    /// Authentication response status (200 = success, 401 = failed).
    Signal<void(const int&)> Announce;

    /// A peer has come online.
    Signal<void(Peer&)> PeerConnected;

    /// A peer has gone offline.
    Signal<void(Peer&)> PeerDisconnected;

    /// Modify the outgoing peer object before presence broadcast.
    Signal<void(Peer&)> CreatePresence;

    /// Underlying connection state changed.
    /// Inherits StateChange from Stateful<ClientState>.

protected:
    virtual void createPresence(Presence& p);
    virtual void onPresenceData(const json::Value& data, bool whiny = false);

private:
    void doConnect();
    void onSocketRecv(const std::string& data);
    void onSocketClose();
    void onSocketError(const std::string& error);
    void onWelcome(const json::Value& msg);
    void onServerMessage(const json::Value& msg);
    void startReconnect();
    void reset();

    int sendJson(const json::Value& msg);
    std::string buildUrl() const;

    Options _options;
    uv::Loop* _loop;
    http::ClientConnection::Ptr _ws;
    Roster _roster;
    std::string _ourID;
    StringVec _rooms;
    int _announceStatus = 0;
    Timer _reconnectTimer;
    int _reconnectCount = 0;
    bool _wasOnline = false;
};


} // namespace smpl
} // namespace scy


/// @\}
