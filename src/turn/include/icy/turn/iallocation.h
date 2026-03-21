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


#include "icy/logger.h"
#include "icy/net/address.h"
#include "icy/timer.h"
#include "icy/turn/fivetuple.h"
#include "icy/turn/permission.h"
#include "icy/turn/turn.h"
#include "icy/turn/types.h"


namespace icy {
namespace turn {


/// Default allocation lifetime: 10 minutes (in milliseconds)
static constexpr std::int64_t kDefaultAllocationLifetime = 10 * 60 * 1000;


/// All TURN operations revolve around allocations, and all TURN messages
/// are associated with an allocation.  An allocation conceptually
/// consists of the following state data:
///
///   * the relayed transport address;
///   * the 5-tuple: (client's IP address, client's port, server IP
///     address, server port, transport protocol);
///   * the authentication information;
///   * the time-to-expiry;
///  * a list of permissions;
///  * a list of channel to peer bindings.
///
/// The relayed transport address is the transport address allocated by
/// the server for communicating with peers, while the 5-tuple describes
/// the communication path between the client and the server.  On the
/// client, the 5-tuple uses the client's host transport address; on the
/// server, the 5-tuple uses the client's server-reflexive transport
/// address.
///
/// Both the relayed transport address and the 5-tuple MUST be unique
/// across all allocations, so either one can be used to uniquely
/// identify the allocation.
/// The authentication information (e.g., username, password, realm, and
/// nonce) is used to both verify subsequent requests and to compute the
/// message integrity of responses.  The username, realm, and nonce
/// values are initially those used in the authenticated Allocate request
/// that creates the allocation, though the server can change the nonce
/// value during the lifetime of the allocation using a 438 (Stale Nonce)
/// reply.  Note that, rather than storing the password explicitly, for
/// security reasons, it may be desirable for the server to store the key
/// value, which is an MD5 hash over the username, realm, and password
/// (see [RFC5389]).
///
/// The time-to-expiry is the time in seconds left until the allocation
/// expires.  Each Allocate or Refresh transaction sets this timer, which
/// then ticks down towards 0.  By default, each Allocate or Refresh
/// transaction resets this timer to the default lifetime value of 600
/// seconds (10 minutes), but the client can request a different value in
/// the Allocate and Refresh request. Allocations can only be refreshed
/// using the Refresh request; sending data to a peer does not refresh an
/// allocation. When an allocation expires, the state data associated
/// with the allocation can be freed.
///
class TURN_API IAllocation
{
public:
    /// @param tuple    The 5-tuple identifying this allocation.
    /// @param username Authenticated username associated with this allocation.
    /// @param lifetime Initial lifetime in seconds.
    IAllocation(const FiveTuple& tuple = FiveTuple(),
                const std::string& username = "",
                std::int64_t lifetime = kDefaultAllocationLifetime);
    virtual ~IAllocation();

    IAllocation(const IAllocation&) = delete;
    IAllocation& operator=(const IAllocation&) = delete;
    IAllocation(IAllocation&&) = delete;
    IAllocation& operator=(IAllocation&&) = delete;

    /// Updates the last-activity timestamp and accumulates bandwidth usage.
    /// Call this whenever data is relayed through the allocation.
    /// @param numBytes Number of bytes relayed (0 just refreshes the timestamp).
    virtual void updateUsage(std::int64_t numBytes = 0);

    /// Sets the allocation lifetime in seconds and resets the activity timestamp,
    /// effectively extending the expiry from the current moment.
    /// @param lifetime New lifetime in seconds.
    virtual void setLifetime(std::int64_t lifetime);

    /// Sets the maximum number of bytes this allocation may relay in its lifetime.
    /// Pass 0 to disable bandwidth limiting.
    /// @param numBytes Bandwidth cap in bytes (0 = unlimited).
    virtual void setBandwidthLimit(std::int64_t numBytes);

    /// @return true if the allocation's lifetime has elapsed or the bandwidth
    ///         quota has been exhausted.
    [[nodiscard]] virtual bool expired() const;

    /// Returns true if the allocation's deleted flag is set
    /// and or if the allocation has expired.
    ///
    /// This signifies that the allocation is ready to be
    /// destroyed via async garbage collection.
    /// See Server::onTimer() and Client::onTimer()
    [[nodiscard]] virtual bool deleted() const;

    /// @return The configured bandwidth limit in bytes (0 means unlimited).
    [[nodiscard]] virtual std::int64_t bandwidthLimit() const;

    /// @return Total bytes transferred through this allocation since creation.
    [[nodiscard]] virtual std::int64_t bandwidthUsed() const;

    /// @return Bytes remaining before the bandwidth quota is exhausted.
    ///         Returns a large sentinel value when no limit is configured.
    [[nodiscard]] virtual std::int64_t bandwidthRemaining() const;

    /// @return Seconds until the allocation expires (0 if already expired).
    [[nodiscard]] virtual std::int64_t timeRemaining() const;

    /// @return Reference to the 5-tuple identifying this allocation.
    virtual FiveTuple& tuple();

    /// @return The username associated with this allocation.
    [[nodiscard]] virtual std::string username() const;

    /// @return The configured lifetime in seconds.
    [[nodiscard]] virtual std::int64_t lifetime() const;

    /// @return A copy of the current permission list.
    [[nodiscard]] virtual PermissionList permissions() const;

    /// @return The relay transport address assigned to this allocation.
    [[nodiscard]] virtual net::Address relayedAddress() const = 0;

    /// Adds a permission for @p ip, or refreshes the existing one.
    /// @param ip IPv4 address string to permit.
    virtual void addPermission(const std::string& ip);

    /// Adds (or refreshes) permissions for multiple IPs.
    /// @param ips List of IPv4 address strings.
    virtual void addPermissions(const IPList& ips);

    /// Removes the permission for @p ip if present.
    /// @param ip IPv4 address string to remove.
    virtual void removePermission(const std::string& ip);

    /// Removes all permissions from the list.
    virtual void removeAllPermissions();

    /// Removes any permissions whose 5-minute lifetime has elapsed.
    virtual void removeExpiredPermissions();

    /// Checks whether a permission exists for @p peerIP.
    /// Local addresses (192.168.x.x and 127.x.x.x) are always permitted.
    /// @param peerIP IPv4 address string to check.
    /// @return true if a valid (non-expired) permission exists.
    [[nodiscard]] virtual bool hasPermission(const std::string& peerIP);

    virtual void print(std::ostream& os) const
    {
        os << "Allocation[" << relayedAddress() << "]" << '\n';
    }

    friend std::ostream& operator<<(std::ostream& stream,
                                    const IAllocation& alloc)
    {
        alloc.print(stream);
        return stream;
    }

protected:
    FiveTuple _tuple;
    std::string _username;
    PermissionList _permissions;
    std::int64_t _lifetime;
    std::int64_t _bandwidthLimit;
    std::int64_t _bandwidthUsed;
    time_t _createdAt;
    time_t _updatedAt;
    bool _deleted;
};


} // namespace turn
} // namespace icy


/// @\}
