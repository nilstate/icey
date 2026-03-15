///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "scy/logger.h"
#include "scy/turn/server/server.h"
#include "scy/util.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>


namespace scy {
namespace turn {


#define ENABLE_LOCAL_IPS 1


IAllocation::IAllocation(const FiveTuple& tuple, const std::string& username,
                         std::int64_t lifetime)
    : _tuple(tuple)
    , _username(username)
    , _lifetime(lifetime)
    , _bandwidthLimit(0)
    , _bandwidthUsed(0)
    , _createdAt(static_cast<std::int64_t>(time(0)))
    , _updatedAt(static_cast<std::int64_t>(time(0)))
    , _deleted(false)
{
}


IAllocation::~IAllocation()
{
    LTrace("Destroy");
    _permissions.clear();
}


void IAllocation::updateUsage(std::int64_t numBytes)
{
    std::lock_guard<std::mutex> lock(_mutex);
    LTrace("Update usage: ", _bandwidthUsed, ": ", numBytes);
    _updatedAt = time(0);
    _bandwidthUsed += numBytes;
}


std::int64_t IAllocation::timeRemaining() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    // uint32_t remaining = static_cast<std::int64_t>(_lifetime - (time(0) - _updatedAt));
    std::int64_t remaining = _lifetime - static_cast<std::int64_t>(time(0) - _updatedAt);
    return remaining > 0 ? remaining : 0;
}


bool IAllocation::expired() const
{
    return timeRemaining() == 0 || bandwidthRemaining() == 0;
}


bool IAllocation::deleted() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _deleted || expired();
}


void IAllocation::setLifetime(std::int64_t lifetime)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _lifetime = lifetime;
    _updatedAt = static_cast<std::int64_t>(time(0));
    LTrace("Updating Lifetime: ", _lifetime);
}


void IAllocation::setBandwidthLimit(std::int64_t numBytes)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _bandwidthLimit = numBytes;
}


std::int64_t IAllocation::bandwidthLimit() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _bandwidthLimit;
}


std::int64_t IAllocation::bandwidthUsed() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _bandwidthUsed;
}


std::int64_t IAllocation::bandwidthRemaining() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _bandwidthLimit > 0 ? (_bandwidthLimit > _bandwidthUsed
                                      ? _bandwidthLimit - _bandwidthUsed
                                      : 0)
                               : 99999999;
}


FiveTuple& IAllocation::tuple()
{
    return _tuple;
}


std::string IAllocation::username() const
{
    return _username;
}


std::int64_t IAllocation::lifetime() const
{
    return _lifetime;
}


PermissionList IAllocation::permissions() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _permissions;
}


void IAllocation::addPermission(const std::string& ip)
{
    std::lock_guard<std::mutex> lock(_mutex);
    // If the permission is already in the list then refresh it.
    for (auto& perm : _permissions) {
        if (perm.ip == ip) {
            LTrace("Refreshing permission: ", ip);
            perm.refresh();
            return;
        }
    }

    // Otherwise create it...
    LTrace("Create permission: ", ip);
    _permissions.push_back(Permission(ip));
}


void IAllocation::addPermissions(const IPList& ips)
{
    for (const auto& ip : ips) {
        addPermission(ip);
    }
}


void IAllocation::removePermission(const std::string& ip)
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto it = _permissions.begin(); it != _permissions.end();) {
        if (it->ip == ip) {
            it = _permissions.erase(it);
            return;
        } else
            ++it;
    }
}


void IAllocation::removeAllPermissions()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _permissions.clear();
}


void IAllocation::removeExpiredPermissions()
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto it = _permissions.begin(); it != _permissions.end();) {
        if (it->timeout.expired()) {
            LInfo("Removing Expired Permission: ", it->ip);
            it = _permissions.erase(it);
        } else
            ++it;
    }
}


bool IAllocation::hasPermission(const std::string& peerIP)
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (const auto& perm : _permissions) {
        if (perm == peerIP)
            return true;
    }

#if ENABLE_LOCAL_IPS
    if (peerIP.find("192.168.") == 0 || peerIP.find("127.") == 0) {
        LWarn("Granting permission for local IP without explicit permission: ", peerIP);
        return true;
    }
#endif

    LTrace("No permission for: ", peerIP);
    return false;
}


} // namespace turn
} // namespace scy


/// @\}
