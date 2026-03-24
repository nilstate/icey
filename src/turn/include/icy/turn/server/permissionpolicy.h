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


#include "icy/turn/permission.h"


namespace icy {
namespace turn {


/// Lightweight server-side TURN permission policy.
///
/// Keeps the explicit permission list fast and binary, while making
/// server exceptions such as local-IP auto-grants explicit and auditable.
class TURN_API PermissionPolicy
{
public:
    explicit PermissionPolicy(bool enableLocalIPPermissions = false) noexcept
        : _enableLocalIPPermissions(enableLocalIPPermissions)
    {
    }

    [[nodiscard]] bool allowsExplicit(const PermissionList& permissions,
                                      const Permission::Key& peerKey) const noexcept;
    [[nodiscard]] bool allowsImplicit(const Permission::Key& peerKey) const noexcept;
    [[nodiscard]] bool allows(const PermissionList& permissions,
                              const Permission::Key& peerKey) const noexcept;

private:
    [[nodiscard]] static bool isLocalAutoGrantAddress(const Permission::Key& peerKey) noexcept;

    bool _enableLocalIPPermissions = false;
};


} // namespace turn
} // namespace icy


/// @}
