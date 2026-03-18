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


#include "scy/bitwise.h"
#include "scy/http/websocket.h"
#include "scy/net/socket.h"
#include "scy/socketio/client.h"
#include "scy/symple/command.h"
#include "scy/symple/event.h"
#include "scy/symple/form.h"
#include "scy/symple/message.h"
#include "scy/symple/peer.h"
#include "scy/symple/presence.h"
#include "scy/symple/roster.h"
#include "scy/symple/symple.h"
#include "scy/timedmanager.h"


namespace scy {
namespace smpl {


using PersistenceT = TimedManager<std::string, Message>;


//
// Symple Client
//


class Symple_API Client : public sockio::Client
{
public:
    struct Options
    {
        std::string host = "127.0.0.1";
        uint16_t port = 4500;

        /// Weather or not to reconnect if disconnected from the server.
        bool reconnection = true;

        /// The number of times to attempt to reconnect if disconnected
        /// from the server. (0 = unlimited)
        int reconnectAttempts = 0;

        std::string user;
        std::string name;
        std::string type;
        std::string token;

        Options()
        {
            // Required on gcc 6
            // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70528
        }
    };

public:
    Client(const net::Socket::Ptr& socket, const Options& options = Options());
    virtual ~Client();

    void connect();
    void close();

    /// Send a message.
    /// May be a polymorphic Command, Presence, Event or other ...
    virtual int send(Message& message, bool ack = false);

    /// Send a string message.
    /// The message must be a valid Symple message otherwise it will
    /// not be delivered.
    virtual int send(const std::string& message, bool ack = false);

    /// Create a Transaction object with the given message which will
    /// notify on Ack response from the server.
    sockio::Transaction* createTransaction(Message& message);

    /// Swap the 'to' and 'from' fields and send the given message.
    virtual int respond(Message& message, bool ack = false);

    /// Broadcast presence to the user group scope.
    /// The outgoing Presence object may be modified via
    /// the CreatePresence signal.
    virtual int sendPresence(bool probe = false);

    /// Send directed presence to the given peer.
    virtual int sendPresence(const Address& to, bool probe = false);

    /// Join the given room.
    virtual int joinRoom(const std::string& room);

    /// Leave the given room.
    virtual int leaveRoom(const std::string& room);

    /// Return the session ID of our current peer object.
    /// Return an empty string when offline.
    [[nodiscard]] virtual std::string ourID() const;

    /// Return a list of rooms the client has joined.
    [[nodiscard]] StringVec rooms() const;

    /// Return the peer object for the current session,
    /// or throws an exception when offline.
    virtual Peer* ourPeer();

    /// Return the roster which stores all online peers.
    virtual Roster& roster();

    /// Return the persistence manager which stores
    /// long lived messages.
    virtual PersistenceT& persistence();

    /// Return a reference to the client options object.
    Client::Options& options();

    /// Stream operator alias for send().
    virtual Client& operator>>(Message& message);

    /// Update the roster from the given client object.
    virtual void onPresenceData(const json::Value& data, bool whiny = false);

    ///
    /// Signals
    ///

    /// Notifies the outside application about the
    /// response status code of our announce() call.
    /// Possible status codes are:    //   - 200: Authentication success
    ///   - 401: Authentication failed
    ///   - 400: Bad request data
    ///   - 500: Server not found
    Signal<void(const int&)> Announce;

    /// Signals when a peer connects.
    Signal<void(Peer&)> PeerConnected;

    /// Signals when a peer disconnects.
    Signal<void(Peer&)> PeerDisconnected;

    /// Called by createPresence() so outside classes
    /// can modify the outgoing Peer JSON object.
    Signal<void(Peer&)> CreatePresence;

protected:
    /// Called when a new connection is established
    /// to announce and authenticate the peer on the
    /// server.
    virtual int announce();

    /// Resets variables and data at the beginning
    /// and end of each session.
    virtual void reset();

    /// Creates a Presence object.
    virtual void createPresence(Presence& p);

    /// Override PacketSignal::emit
    virtual void emit(/*void* sender, */ IPacket& packet) override;

    virtual void onOnline() override;
    virtual void onAnnounceState(void* sender, TransactionState& state,
                                 const TransactionState&);
    // virtual void onPacket(sockio::Packet& packet);

protected:
    Roster _roster;
    std::string _ourID;
    PersistenceT _persistence;
    Client::Options _options;
    StringVec _rooms;
    int _announceStatus;
};


//
// TCP Client
//


Client* createTCPClient(const Client::Options& options = Client::Options(),
                        uv::Loop* loop = uv::defaultLoop());


class Symple_API TCPClient : public Client
{
public:
    TCPClient(const Client::Options& options = Client::Options(),
              uv::Loop* loop = uv::defaultLoop());
};


//
// SSL Client
//


Client* createSSLClient(const Client::Options& options = Client::Options(),
                        uv::Loop* loop = uv::defaultLoop());


class Symple_API SSLClient : public Client
{
public:
    SSLClient(const Client::Options& options = Client::Options(),
              uv::Loop* loop = uv::defaultLoop());
};


//
// Filters
//


enum FilterFlags
{
    AcceptRequests = 0x01,
    AcceptResponses = 0x02
};


struct Filter //: public Flaggable
{
    Filter(const std::string& path, unsigned flags = 0)
        : flags(flags)
        , path(path)
    {
    }

    Filter(unsigned flags = 0)
        : flags(flags)
        , path("*")
    {
    }

    Bitwise flags;
    std::string path;
};


//
// Polymorphic Message Delegates
//


} // namespace smpl
} // namespace scy


/// @\}
