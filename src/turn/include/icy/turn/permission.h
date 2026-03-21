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


#include "icy/net/address.h"
#include "icy/timeout.h"
#include "icy/turn/fivetuple.h"

#include <string>
#include <string_view>
#include <vector>


namespace icy {
namespace turn {


/// Permission Lifetime MUST be 300 seconds (= 5 minutes).
static constexpr int PERMISSION_LIFETIME = 5 * 60 * 1000;


/// A single TURN permission entry associating a peer IP with a 5-minute expiry timer.
/// Per RFC 5766 section 8, permissions last exactly 300 seconds and must be refreshed
/// via a new CreatePermission request before they expire.
struct Permission
{
    std::string ip; ///< IPv4 address string of the permitted peer.
    Timeout timeout; ///< Countdown timer; expires after PERMISSION_LIFETIME milliseconds.

    /// Constructs a permission for @p ip and immediately starts the expiry timer.
    /// @param ip IPv4 address string of the permitted peer.
    Permission(const std::string& ip)
        : ip(ip)
        , timeout(PERMISSION_LIFETIME)
    {
        refresh();
    }

    /// Resets the expiry timer, extending the permission lifetime by another 300 seconds.
    void refresh() { timeout.reset(); }

    /// Equality comparison against an IP string.
    /// @param r IPv4 address string to compare against.
    /// @return true if this permission's IP matches @p r.
    bool operator==(std::string_view r) const { return ip == r; }
};


using PermissionList = std::vector<Permission>;


} // namespace turn
} // namespace icy


/// @\}
