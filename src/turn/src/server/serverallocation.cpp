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

namespace {

bool isLocalPermissionAddress(const net::Address& address)
{
    auto* raw = address.addr();
    if (!raw)
        return false;

    if (address.af() == AF_INET) {
        const auto* ipv4 = reinterpret_cast<const sockaddr_in*>(raw);
        uint32_t host = ntohl(ipv4->sin_addr.s_addr);
        if ((host & 0xff000000u) == 0x0a000000u)   // 10.0.0.0/8
            return true;
        if ((host & 0xfff00000u) == 0xac100000u)   // 172.16.0.0/12
            return true;
        if ((host & 0xffff0000u) == 0xc0a80000u)   // 192.168.0.0/16
            return true;
        if ((host & 0xff000000u) == 0x7f000000u)   // 127.0.0.0/8
            return true;
        return false;
    }

    if (address.af() == AF_INET6) {
        const auto* ipv6 = reinterpret_cast<const sockaddr_in6*>(raw);
        const auto& bytes = ipv6->sin6_addr.s6_addr;

        static constexpr uint8_t loopback[16] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
        };
        if (std::memcmp(bytes, loopback, sizeof(loopback)) == 0)
            return true;
        if (bytes[0] == 0xfe && (bytes[1] & 0xc0) == 0x80) // fe80::/10
            return true;
    }

    return false;
}

} // namespace


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
    // RFC 5766 section 7.2: If no LIFETIME attribute is present, use
    // the default lifetime configured on the server.
    auto lifetimeAttr = request.get<stun::Lifetime>();
    uint32_t desiredLifetime;
    if (lifetimeAttr) {
        desiredLifetime = std::min<uint32_t>(
            _server.options().allocationMaxLifetime / 1000, lifetimeAttr->value());
    } else {
        desiredLifetime = _server.options().allocationDefaultLifetime / 1000;
    }

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

    response.add<stun::Lifetime>().setValue(desiredLifetime);

    _server.respond(request, response);
}


void ServerAllocation::handleCreatePermission(Request& request)
{
    LTrace("Handle Create Permission");

    for (int i = 0; i < _server.options().allocationMaxPermissions; i++) {
        auto peerAttr = request.get<stun::XorPeerAddress>(i);
        if (!peerAttr || peerAttr->family() == stun::AddressFamily::Undefined) {
            if (i == 0) {
                _server.respondError(request, kErrorBadRequest, "Bad Request");
                return;
            } else
                break;
        }
        addPermission(peerAttr->address());
    }

    stun::Message response(stun::Message::SuccessResponse,
                           stun::Message::CreatePermission);
    response.setTransactionID(request.transactionID());

    _server.respond(request, response);
}


bool ServerAllocation::onTimer()
{
    LTrace("ServerAllocation: On timer: ", IAllocation::deleted());
    if (IAllocation::deleted() || _refreshDeleteRequested)
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


bool ServerAllocation::hasPermission(const std::string& peerIP)
{
    if (IAllocation::hasPermission(peerIP))
        return true;

    if (_server.options().enableLocalIPPermissions) {
        try {
            net::Address peerAddress(peerIP, 0);
            if (isLocalPermissionAddress(peerAddress)) {
                LWarn("Auto-granting permission for local IP: ", peerIP);
                return true;
            }
        } catch (...) {
        }
    }

    return false;
}


bool ServerAllocation::hasPermission(const net::Address& peerAddress)
{
    if (IAllocation::hasPermission(peerAddress))
        return true;

    if (_server.options().enableLocalIPPermissions) {
        if (isLocalPermissionAddress(peerAddress)) {
            LWarn("Auto-granting permission for local IP: ", peerAddress);
            return true;
        }
    }

    return false;
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
