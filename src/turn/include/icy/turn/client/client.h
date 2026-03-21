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


/// State machine states for the TURN client
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

    /// @return Human-readable name for the current state.
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


/// Abstract observer interface for TURN client events.
/// Callers subclass this to receive allocation lifecycle and data relay callbacks.
struct ClientObserver
{
    /// Called whenever the client's state machine transitions to a new state.
    /// @param client   The client that changed state.
    /// @param state    The new state.
    /// @param oldState The previous state.
    virtual void onClientStateChange(Client& client, ClientState& state, const ClientState& oldState) = 0;

    /// Called when relayed data is received from a peer via a Data Indication.
    /// @param client      The owning client.
    /// @param data        Pointer to the received payload bytes.
    /// @param size        Number of bytes in @p data.
    /// @param peerAddress Source address of the remote peer.
    virtual void onRelayDataReceived(Client& client, const char* data, size_t size, const net::Address& peerAddress) = 0;

    /// Called when the server rejects an Allocate request with an error.
    /// @param client    The owning client.
    /// @param errorCode STUN error code (e.g. 401, 438).
    /// @param reason    Human-readable error reason string.
    virtual void onAllocationFailed(Client& client, int errorCode, const std::string& reason) {};

    /// Called when the server confirms deletion of the allocation.
    /// @param client      The owning client.
    /// @param transaction The completed Refresh transaction (lifetime == 0).
    virtual void onAllocationDeleted(Client& client, const stun::Transaction& transaction) {};

    /// Called after a CreatePermission request succeeds.
    /// @param client      The owning client.
    /// @param permissions The full list of active permissions after this update.
    virtual void onAllocationPermissionsCreated(Client& client, const PermissionList& permissions) {};

    /// All received transaction responses will be routed here after local
    /// processing so the observer can easily implement extra functionality.
    /// @param client      The owning client.
    /// @param transaction The completed transaction (request + response accessible).
    virtual void onTransactionResponse(Client& client, const stun::Transaction& transaction) {};

    /// Fires after the client's internal timer callback.
    /// Handy for performing extra async cleanup tasks.
    /// @param client The owning client.
    virtual void onTimer(Client& client) {};
};


/// Timer interval for client maintenance (30 seconds)
static constexpr std::int64_t kClientTimerInterval = 30 * 1000;

/// Default client allocation lifetime (5 minutes, in milliseconds)
static constexpr std::int64_t kClientDefaultLifetime = 5 * 60 * 1000;

/// Default client transaction timeout (10 seconds)
static constexpr long kClientDefaultTimeout = 10 * 1000;


/// TURN client that manages relay allocations, permissions, and data relay via RFC 5766
class TURN_API Client : public Stateful<ClientState>
    , protected IAllocation
{
public:
    /// Configuration options for the TURN client
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
    /// @param observer Observer to receive lifecycle and data callbacks.
    /// @param options  Configuration for this client instance.
    /// @param socket   Underlying transport socket (TCP or UDP).
    Client(ClientObserver& observer, const Options& options, const net::Socket::Ptr& socket);
    virtual ~Client();

    /// Connects the socket to the TURN server and starts the allocation sequence.
    /// Permissions must be added via addPermission() before calling this.
    virtual void initiate();

    /// Stops the timer, cancels pending transactions, and closes the socket.
    virtual void shutdown();

    /// Sends an Allocate request to the server with the configured transport
    /// and lifetime. On first call the server will typically respond with a
    /// 401 challenge; the client re-sends with credentials automatically.
    virtual void sendAllocate();

    /// Adds multiple peer IP addresses to the permission list.
    /// @param peerIPs List of IPv4 address strings to permit.
    virtual void addPermission(const IPList& peerIPs);

    /// Adds a single peer IP to the permission list, or refreshes it if
    /// already present. Permissions should be added before initiate(); they
    /// may also be added later, in which case a new CreatePermission request
    /// is required.
    /// @param ip IPv4 address string of the permitted peer.
    void addPermission(const std::string& ip) override;

    /// Sends a CreatePermission request for all IPs currently in the
    /// permission list. Called automatically after allocation succeeds and
    /// periodically by the timer to refresh expiring permissions.
    virtual void sendCreatePermission();

    /// Channel bindings (RFC 5766 Section 11) are intentionally not implemented.
    /// They are a bandwidth optimization that replaces STUN-framed Send/Data
    /// indications with a compact 4-byte ChannelData header. This only benefits
    /// high-throughput media relay scenarios; in practice, media flows directly
    /// via ICE/DTLS rather than through this TURN client's data path, so the
    /// optimization is not worth the complexity (channel number allocation,
    /// 10-minute binding refresh timers, ChannelData wire framing).
    /// Data relay uses sendData() with Send Indications instead.
    /// @param peerIP Unused; always throws std::logic_error.
    virtual void sendChannelBind(const std::string& peerIP);

    /// Sends a Refresh request to extend the allocation lifetime.
    /// Called automatically by the timer when roughly one-third of the
    /// lifetime remains.
    virtual void sendRefresh();

    /// Sends a Send Indication to relay @p data to @p peerAddress through
    /// the TURN server. If permissions are still being negotiated the
    /// indication is queued and flushed once CreatePermission succeeds.
    /// @param data        Pointer to the payload to relay.
    /// @param size        Number of bytes to relay.
    /// @param peerAddress Destination peer address (must have an active permission).
    virtual void sendData(const char* data, size_t size, const net::Address& peerAddress);

    /// Dispatches an incoming STUN/TURN response to the appropriate handler.
    /// @param response Received STUN message.
    /// @return true if the message was handled, false if it was unrecognised.
    virtual bool handleResponse(const stun::Message& response);

    /// Processes a successful Allocate response; extracts mapped/relayed addresses
    /// and advances the state to Authorizing, then sends CreatePermission.
    /// @param response Allocate success response from the server.
    virtual void handleAllocateResponse(const stun::Message& response);

    /// Handles an Allocate error response; manages the 401 challenge/re-send
    /// flow and sets the client to Failed for unrecoverable errors.
    /// @param response Allocate error response from the server.
    virtual void handleAllocateErrorResponse(const stun::Message& response);

    /// Handles a successful CreatePermission response; flushes queued Send
    /// Indications and advances state to Success.
    /// @param response CreatePermission success response from the server.
    virtual void handleCreatePermissionResponse(const stun::Message& response);

    /// Handles a failed CreatePermission response; clears all permissions
    /// and sets the client to Failed.
    /// @param response CreatePermission error response from the server.
    virtual void handleCreatePermissionErrorResponse(const stun::Message& response);

    /// Handles a Refresh response; updates the stored lifetime.
    /// @param response Refresh response from the server.
    virtual void handleRefreshResponse(const stun::Message& response);

    /// Handles an incoming Data Indication; extracts peer address and data
    /// and forwards to ClientObserver::onRelayDataReceived().
    /// @param response Data Indication message from the server.
    virtual void handleDataIndication(const stun::Message& response);

    /// @return The IANA protocol number for the underlying transport
    ///         (kProtocolUDP or kProtocolTCP).
    virtual int transportProtocol();

    /// Creates a new STUN transaction, registers the progress callback, and
    /// adds it to the active transaction list.
    /// @param socket Optional override socket; uses the control socket if nullptr.
    /// @return IntrusivePtr to the new transaction.
    virtual stun::Transaction::Ptr createTransaction(const net::Socket::Ptr& socket = nullptr);

    /// Adds STUN long-term credential attributes (Username, Realm, Nonce,
    /// MessageIntegrity) to @p request if the realm has been received from the server.
    /// @param request Message to decorate with authentication attributes.
    virtual void authenticateRequest(stun::Message& request);

    /// Calls authenticateRequest() then sends the transaction.
    /// @param transaction Transaction whose request to authenticate and send.
    /// @return true if the send succeeded.
    virtual bool sendAuthenticatedTransaction(stun::Transaction* transaction);

    /// Removes a transaction from the active list.
    /// The IntrusivePtr keeps the object alive until the caller's copy is released.
    /// @param transaction Transaction to remove.
    /// @return The removed Ptr, or nullptr if not found.
    virtual stun::Transaction::Ptr removeTransaction(stun::Transaction* transaction);

    /// @return The server-reflexive (mapped) address assigned by the TURN server.
    [[nodiscard]] net::Address mappedAddress() const;

    /// @return The relayed transport address assigned by the TURN server.
    [[nodiscard]] net::Address relayedAddress() const;

    /// @return true when the client is in the None or Failed state.
    [[nodiscard]] bool closed() const;

    /// @return Reference to the observer provided at construction.
    ClientObserver& observer();

    /// @return Reference to the mutable options struct.
    Options& options();

protected:
    /// Sets the error field and transitions the client to the Failed state.
    /// @param error Error descriptor.
    virtual void setError(const icy::Error& error);

    /// Socket connect callback; starts the timer and sends the first Allocate request.
    /// @param socket The connected socket.
    virtual bool onSocketConnect(net::Socket& socket);

    /// Socket receive callback; parses STUN messages from the buffer and dispatches them.
    /// @param socket      The receiving socket.
    /// @param buffer      Received data.
    /// @param peerAddress Source address of the received data.
    virtual bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress);

    /// Socket close callback; shuts down the client and records the socket error.
    /// @param socket The closed socket.
    virtual bool onSocketClose(net::Socket& socket);

    /// STUN transaction state-change callback; handles Success and Failed outcomes.
    /// @param sender    Pointer to the stun::Transaction that changed state.
    /// @param state     The new transaction state.
    virtual void onTransactionProgress(void* sender, TransactionState& state, const TransactionState&);

    /// Forwards state-change events to the observer.
    void onStateChange(ClientState& state, const ClientState& oldState) override;

    /// Periodic timer callback; re-allocates on expiry or refreshes when lifetime
    /// is below one-third remaining. Also calls ClientObserver::onTimer().
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
    std::vector<stun::Transaction::Ptr> _transactions;
};


} // namespace turn
} // namespace icy


/// @\}
