///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/server/permissionpolicy.h"


namespace icy {
namespace turn {


bool PermissionPolicy::allowsExplicit(const PermissionList& permissions,
                                      const Permission::Key& peerKey) const noexcept
{
    if (!peerKey.valid())
        return false;

    for (const auto& permission : permissions) {
        if (permission.key.matches(peerKey))
            return true;
    }
    return false;
}


bool PermissionPolicy::allowsImplicit(const Permission::Key& peerKey) const noexcept
{
    return _enableLocalIPPermissions && isLocalAutoGrantAddress(peerKey);
}


bool PermissionPolicy::allows(const PermissionList& permissions,
                              const Permission::Key& peerKey) const noexcept
{
    return allowsExplicit(permissions, peerKey) || allowsImplicit(peerKey);
}


bool PermissionPolicy::isLocalAutoGrantAddress(const Permission::Key& peerKey) noexcept
{
    if (!peerKey.valid())
        return false;

    if (peerKey.af == AF_INET) {
        uint32_t host;
        std::memcpy(&host, peerKey.bytes.data(), sizeof(host));
        host = ntohl(host);
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

    if (peerKey.af == AF_INET6) {
        static constexpr uint8_t loopback[16] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
        };
        if (std::memcmp(peerKey.bytes.data(), loopback, sizeof(loopback)) == 0)
            return true;
        return peerKey.bytes[0] == 0xfe && (peerKey.bytes[1] & 0xc0) == 0x80; // fe80::/10
    }

    return false;
}


} // namespace turn
} // namespace icy


/// @\}
