///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "scy/logger.h"
#include "scy/net/address.h"
#include "scy/timer.h"
#include "scy/turn/fivetuple.h"
#include "scy/turn/permission.h"
#include "scy/turn/turn.h"
#include "scy/turn/types.h"
#include <mutex>


namespace scy {
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
    IAllocation(const FiveTuple& tuple = FiveTuple(),
                const std::string& username = "",
                std::int64_t lifetime = kDefaultAllocationLifetime);
    virtual ~IAllocation();

    IAllocation(const IAllocation&) = delete;
    IAllocation& operator=(const IAllocation&) = delete;
    IAllocation(IAllocation&&) = delete;
    IAllocation& operator=(IAllocation&&) = delete;

    /// Updates the allocation's internal timeout and bandwidth
    /// usage each time the allocation is used.
    virtual void updateUsage(std::int64_t numBytes = 0);

    /// Sets the lifetime of the allocation and resets the timeout.
    virtual void setLifetime(std::int64_t lifetime);

    /// Sets the bandwidth limit in bytes for this allocation.
    virtual void setBandwidthLimit(std::int64_t numBytes);

    /// Returns true if the allocation is expired ie. is timed
    /// out or the bandwidth limit has been reached.
    [[nodiscard]] virtual bool expired() const;

    /// Returns true if the allocation's deleted flag is set
    /// and or if the allocation has expired.
    ///
    /// This signifies that the allocation is ready to be
    /// destroyed via async garbage collection.
    /// See Server::onTimer() and Client::onTimer()
    [[nodiscard]] virtual bool deleted() const;

    [[nodiscard]] virtual std::int64_t bandwidthLimit() const;
    [[nodiscard]] virtual std::int64_t bandwidthUsed() const;
    [[nodiscard]] virtual std::int64_t bandwidthRemaining() const;
    [[nodiscard]] virtual std::int64_t timeRemaining() const;

    virtual FiveTuple& tuple();
    [[nodiscard]] virtual std::string username() const;
    [[nodiscard]] virtual std::int64_t lifetime() const;
    [[nodiscard]] virtual PermissionList permissions() const;

    [[nodiscard]] virtual net::Address relayedAddress() const = 0;

    virtual void addPermission(const std::string& ip);
    virtual void addPermissions(const IPList& ips);
    virtual void removePermission(const std::string& ip);
    virtual void removeAllPermissions();
    virtual void removeExpiredPermissions();
    // virtual void refreshAllPermissions();
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
    mutable std::mutex _mutex;
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
} // namespace scy


/// @\}
