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


#include "icy/turn/fivetuple.h"
#include "icy/turn/iallocation.h"
#include "icy/turn/server/permissionpolicy.h"


namespace icy {
namespace turn {


class TURN_API Server;


/// Server-side TURN allocation that tracks permissions, lifetime, and relay state.
/// Handles Refresh and CreatePermission requests common to both UDP and TCP allocations.
/// Subclassed by UDPAllocation and TCPAllocation for transport-specific behaviour.
class TURN_API ServerAllocation : public IAllocation
{
public:
    /// @param server   Owning TURN server.
    /// @param tuple    5-tuple uniquely identifying this allocation.
    /// @param username Authenticated username from the Allocate request.
    /// @param lifetime Initial lifetime in seconds as negotiated with the client.
    ServerAllocation(Server& server, const FiveTuple& tuple,
                     const std::string& username, std::int64_t lifetime);
    virtual ~ServerAllocation();

    /// Dispatches incoming STUN requests to handleCreatePermission() or
    /// handleRefreshRequest(). Subclasses override to add transport-specific methods.
    /// @param request Incoming STUN request.
    /// @return true if the request was handled; false signals the server to respond
    ///         with a 600 "Operation Not Supported" error.
    virtual bool handleRequest(Request& request);

    /// Processes a Refresh request; updates the lifetime or sets the delete flag
    /// if the requested lifetime is 0, then sends a success response.
    /// @param request Incoming Refresh request.
    virtual void handleRefreshRequest(Request& request);

    /// Processes a CreatePermission request; installs or refreshes permissions for
    /// each XOR-PEER-ADDRESS attribute, then sends a success response.
    /// @param request Incoming CreatePermission request.
    virtual void handleCreatePermission(Request& request);

    /// Periodic maintenance callback called by Server::onTimer().
    /// Removes expired permissions. Returns false to signal that this allocation
    /// should be destroyed.
    /// @return true to keep the allocation alive; false to destroy it.
    [[nodiscard]] bool onTimer();

    /// @return The lesser of IAllocation::timeRemaining() and maxTimeRemaining().
    [[nodiscard]] std::int64_t timeRemaining() const override;

    /// @return Seconds remaining before the server-enforced maximum lifetime expires.
    [[nodiscard]] std::int64_t maxTimeRemaining() const;

    /// Checks permission, auto-granting local IPs if enabled in server options.
    [[nodiscard]] bool hasPermission(const std::string& peerIP) override;
    [[nodiscard]] bool hasPermission(const net::Address& peerAddress) override;

    /// @return Reference to the owning TURN server.
    virtual Server& server();

    /// Writes a detailed diagnostic dump of this allocation to @p os.
    void print(std::ostream& os) const override;

    /// Returns true if the refresh request set lifetime to 0,
    /// signaling the parent Server to delete this allocation.
    [[nodiscard]] bool refreshDeleteRequested() const { return _refreshDeleteRequested; }

protected:
    friend class Server;

    uint32_t _maxLifetime;
    Server& _server;
    PermissionPolicy _permissionPolicy;
    bool _refreshDeleteRequested = false;

private:
    /// NonCopyable and NonMovable
    ServerAllocation(const ServerAllocation&) = delete;
    ServerAllocation& operator=(const ServerAllocation&) = delete;
    ServerAllocation(ServerAllocation&&) = delete;
    ServerAllocation& operator=(ServerAllocation&&) = delete;
};


} // namespace turn
} // namespace icy


/// @\}
