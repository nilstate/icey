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


#include "scy/net/address.h"
#include "scy/timeout.h"
#include "scy/turn/fivetuple.h"

#include <string>
#include <string_view>
#include <vector>


namespace scy {
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
} // namespace scy


/// @\}
