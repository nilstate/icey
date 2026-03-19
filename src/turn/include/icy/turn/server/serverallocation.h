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


namespace icy {
namespace turn {


class TURN_API Server;


class TURN_API ServerAllocation : public IAllocation
{
public:
    ServerAllocation(Server& server, const FiveTuple& tuple,
                     const std::string& username, std::int64_t lifetime);
    virtual ~ServerAllocation();

    virtual bool handleRequest(Request& request);
    virtual void handleRefreshRequest(Request& request);
    virtual void handleCreatePermission(Request& request);

    /// Asynchronous timer callback for updating the allocation
    /// permissions and state etc.
    /// If this call returns false the allocation will be deleted.
    [[nodiscard]] bool onTimer();

    [[nodiscard]] std::int64_t timeRemaining() const override;
    [[nodiscard]] std::int64_t maxTimeRemaining() const;
    virtual Server& server();

    void print(std::ostream& os) const override;

    /// Returns true if the refresh request set lifetime to 0,
    /// signaling the parent Server to delete this allocation.
    [[nodiscard]] bool refreshDeleteRequested() const { return _refreshDeleteRequested; }

protected:
    friend class Server;

    uint32_t _maxLifetime;
    Server& _server;
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
