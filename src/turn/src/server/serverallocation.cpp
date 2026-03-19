///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/logger.h"
#include "icy/turn/server/server.h"
#include "icy/util.h"

#include <algorithm>
#include <stdexcept>


namespace icy {
namespace turn {


ServerAllocation::ServerAllocation(Server& server, const FiveTuple& tuple,
                                   const std::string& username,
                                   std::int64_t lifetime)
    : IAllocation(tuple, username, lifetime)
    , _maxLifetime(server.options().allocationMaxLifetime / 1000)
    , _server(server)
{
    // Note: the allocation registers itself with the server via
    // Server::addAllocation(), which is called from the concrete
    // allocation constructors (UDPAllocation, TCPAllocation) or
    // from Server::handleAllocateRequest() after construction.
}


ServerAllocation::~ServerAllocation()
{
    _server.removeAllocation(this);
}


bool ServerAllocation::handleRequest(Request& request)
{
    LTrace("Handle Request");

    if (IAllocation::deleted()) {
        LWarn("Dropping request for deleted allocation");
        return false;
    }

    if (request.methodType() == stun::Message::CreatePermission)
        handleCreatePermission(request);
    else if (request.methodType() == stun::Message::Refresh)
        handleRefreshRequest(request);
    else
        return false; // respondError(request, 600, "Operation Not Supported");

    return true;
}


void ServerAllocation::handleRefreshRequest(Request& request)
{
    LTrace("Handle Refresh Request");
    if (request.methodType() != stun::Message::Refresh)
        throw std::logic_error("handleRefreshRequest called with non-Refresh message");
    if (request.classType() != stun::Message::Request)
        throw std::logic_error("handleRefreshRequest called with non-Request class");

    // Compute the appropriate LIFETIME for this allocation.
    auto lifetimeAttr = request.get<stun::Lifetime>();
    if (!lifetimeAttr) {
        return;
    }
    uint32_t desiredLifetime = std::min<uint32_t>(
        _server.options().allocationMaxLifetime / 1000, lifetimeAttr->value());

    if (desiredLifetime > 0) {
        setLifetime(desiredLifetime);
    } else {
        // Signal the parent to delete this allocation instead of `delete this`
        _refreshDeleteRequested = true;
    }

    // If the request succeeds, then the server sends a success response.
    stun::Message response(stun::Message::SuccessResponse,
                           stun::Message::Refresh);
    response.setTransactionID(request.transactionID());

    auto resLifetimeAttr = new stun::Lifetime;
    resLifetimeAttr->setValue(desiredLifetime);
    response.add(resLifetimeAttr);

    _server.respond(request, response);
}


void ServerAllocation::handleCreatePermission(Request& request)
{
    LTrace("Handle Create Permission");

    for (int i = 0; i < _server.options().allocationMaxPermissions; i++) {
        auto peerAttr = request.get<stun::XorPeerAddress>(i);
        if (!peerAttr || peerAttr->family() != stun::AddressFamily::IPv4) {
            if (i == 0) {
                _server.respondError(request, kErrorBadRequest, "Bad Request");
                return;
            } else
                break;
        }
        addPermission(std::string(peerAttr->address().host()));
    }

    stun::Message response(stun::Message::SuccessResponse,
                           stun::Message::CreatePermission);
    response.setTransactionID(request.transactionID());

    _server.respond(request, response);
}


bool ServerAllocation::onTimer()
{
    LTrace("ServerAllocation: On timer: ", IAllocation::deleted());
    if (IAllocation::deleted())
        return false; // bye bye

    removeExpiredPermissions();
    return true;
}


std::int64_t ServerAllocation::maxTimeRemaining() const
{
    std::int64_t elapsed = static_cast<std::int64_t>(time(0) - _createdAt);
    return elapsed > _maxLifetime ? 0 : _maxLifetime - elapsed;
}


std::int64_t ServerAllocation::timeRemaining() const
{
    return std::min<std::int64_t>(IAllocation::timeRemaining(), maxTimeRemaining());
}


Server& ServerAllocation::server()
{
    return _server;
}


void ServerAllocation::print(std::ostream& os) const
{
    os << "ServerAllocation["
       << "\r\tTuple=" << _tuple << "\r\tUsername=" << username()
       << "\n\tBandwidth Limit=" << bandwidthLimit()
       << "\n\tBandwidth Used=" << bandwidthUsed()
       << "\n\tBandwidth Remaining=" << bandwidthRemaining()
       << "\n\tBase Time Remaining=" << IAllocation::timeRemaining()
       << "\n\tTime Remaining=" << timeRemaining()
       << "\n\tMax Time Remaining=" << maxTimeRemaining()
       << "\n\tDeletable=" << IAllocation::deleted()
       << "\n\tExpired=" << expired() << "]" << '\n';
}


} // namespace turn
} // namespace icy


/// @\}
