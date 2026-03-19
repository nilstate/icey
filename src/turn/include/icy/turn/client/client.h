///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "icy/net/udpsocket.h"
#include "icy/stateful.h"
#include "icy/stun/transaction.h"
#include "icy/turn/fivetuple.h"
#include "icy/turn/iallocation.h"
#include "icy/turn/turn.h"
#include "icy/turn/types.h"

#include <deque>


namespace icy {
namespace turn {


struct ClientState : public State
{
    enum Type
    {
        None = 0x00,
        Allocating = 0x02,
        Authorizing = 0x04,
        Success = 0x08,
        // Terminated      = 0x10,
        Failed = 0x10
    };

    std::string toString() const
    {
        switch (id()) {
            case None:
                return "None";
            case Allocating:
                return "Allocating";
            case Authorizing:
                return "Authorizing";
            case Success:
                return "Success";
            // case Terminated:
            //    return "Terminated";
            case Failed:
                return "Failed";
        }
        return "undefined";
    };
};


class TURN_API Client;


struct ClientObserver
{
    virtual void onClientStateChange(Client& client, ClientState& state, const ClientState& oldState) = 0;

    virtual void onRelayDataReceived(Client& client, const char* data, size_t size, const net::Address& peerAddress) = 0;

    // virtual void onAllocationCreated(Client& client, const stun::Transaction& transaction) {};
    virtual void onAllocationFailed(Client& client, int errorCode, const std::string& reason) {};
    virtual void onAllocationDeleted(Client& client, const stun::Transaction& transaction) {};
    virtual void onAllocationPermissionsCreated(Client& client, const PermissionList& permissions) {};

    /// All received transaction responses will be routed here after local
    /// processing so the observer can easily implement extra functionality.
    virtual void onTransactionResponse(Client& client, const stun::Transaction& transaction) {};

    /// Fires after the client's internal timer callback.
    /// Handy for performing extra async cleanup tasks.
    virtual void onTimer(Client& client) {};
};


/// Timer interval for client maintenance (30 seconds)
static constexpr std::int64_t kClientTimerInterval = 30 * 1000;

/// Default client allocation lifetime (5 minutes, in milliseconds)
static constexpr std::int64_t kClientDefaultLifetime = 5 * 60 * 1000;

/// Default client transaction timeout (10 seconds)
static constexpr long kClientDefaultTimeout = 10 * 1000;


class TURN_API Client : public Stateful<ClientState>
    , protected IAllocation
{
public:
    struct Options
    {
        std::string software;
        std::string username;
        // std::string realm;
        std::string password;

        long timeout;
        std::int64_t lifetime;
        std::int64_t timerInterval;
        net::Address serverAddr;
        Options()
        {
            software = "Icey STUN/TURN Client [rfc5766]";
            username = util::randomString(4);
            password = util::randomString(22);
            // realm                    = "0state.com";
            lifetime = kClientDefaultLifetime;
            timeout = kClientDefaultTimeout;
            timerInterval = kClientTimerInterval;
            serverAddr = net::Address("127.0.0.1", 3478);
        }
    };

public:
    Client(ClientObserver& observer, const Options& options, const net::Socket::Ptr& socket);
    virtual ~Client();

    /// Initiates the allocation sequence.
    virtual void initiate();

    /// Shutdown the client and destroy the active allocation.
    virtual void shutdown();

    /// Sends the allocation request.
    virtual void sendAllocate();

    virtual void addPermission(const IPList& peerIPs);

    /// Peer permissions should be added/created before we kick
    /// off the allocation sequence, but may be added later.
    void addPermission(const std::string& ip) override;

    /// Sends a CreatePermission request including all hosts
    // added via addPermission();

    /// A CreatePermission request will be sent as soon as the
    /// Allocation is created, and at timer x intervals.
    virtual void sendCreatePermission();

    /// Channel bindings (RFC 5766 Section 11) are intentionally not implemented.
    /// They are a bandwidth optimization that replaces STUN-framed Send/Data
    /// indications with a compact 4-byte ChannelData header. This only benefits
    /// high-throughput media relay scenarios; in practice, media flows directly
    /// via ICE/DTLS rather than through this TURN client's data path, so the
    /// optimization is not worth the complexity (channel number allocation,
    /// 10-minute binding refresh timers, ChannelData wire framing).
    /// Data relay uses sendData() with Send Indications instead.
    virtual void sendChannelBind(const std::string& peerIP);
    virtual void sendRefresh();
    virtual void sendData(const char* data, size_t size, const net::Address& peerAddress);

    virtual bool handleResponse(const stun::Message& response);
    virtual void handleAllocateResponse(const stun::Message& response);
    virtual void handleAllocateErrorResponse(const stun::Message& response);
    virtual void handleCreatePermissionResponse(const stun::Message& response);
    virtual void handleCreatePermissionErrorResponse(const stun::Message& response);
    virtual void handleRefreshResponse(const stun::Message& response);
    virtual void handleDataIndication(const stun::Message& response);

    virtual int transportProtocol();
    virtual stun::Transaction* createTransaction(const net::Socket::Ptr& socket = nullptr);
    virtual void authenticateRequest(stun::Message& request);
    virtual bool sendAuthenticatedTransaction(stun::Transaction* transaction);
    virtual bool removeTransaction(stun::Transaction* transaction);

    [[nodiscard]] net::Address mappedAddress() const;
    [[nodiscard]] net::Address relayedAddress() const;

    [[nodiscard]] bool closed() const;

    ClientObserver& observer();
    Options& options();

protected:
    virtual void setError(const icy::Error& error);

    virtual bool onSocketConnect(net::Socket& socket);
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress);
    virtual bool onSocketClose(net::Socket& socket);
    virtual void onTransactionProgress(void* sender, TransactionState& state, const TransactionState&);
    void onStateChange(ClientState& state, const ClientState& oldState) override;
    virtual void onTimer();

protected:
    ClientObserver& _observer;
    Options _options;
    net::SocketEmitter _socket;
    Timer _timer;
    icy::Error _error;
    net::Address _mappedAddress;
    net::Address _relayedAddress;

    std::string _realm;
    std::string _nonce;

    /// A list of queued Send indication packets awaiting server permissions
    std::deque<stun::Message> _pendingIndications;

    /// A list containing currently active transactions
    std::vector<stun::Transaction*> _transactions;
};


} // namespace turn
} // namespace icy


/// @\}
