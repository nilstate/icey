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


/// TURN permission for a user session
struct Permission
{
    std::string ip;
    Timeout timeout;

    Permission(const std::string& ip)
        : ip(ip)
        , timeout(PERMISSION_LIFETIME)
    {
        refresh();
    }

    void refresh() { timeout.reset(); }

    bool operator==(std::string_view r) const { return ip == r; }
};


using PermissionList = std::vector<Permission>;


} // namespace turn
} // namespace icy


/// @\}
