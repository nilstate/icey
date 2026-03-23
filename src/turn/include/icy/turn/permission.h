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

#include <array>
#include <cstring>
#include <cstdint>
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
    struct Key
    {
        int af = AF_UNSPEC;
        std::array<uint8_t, 16> bytes{};
        uint8_t size = 0;

        [[nodiscard]] bool valid() const
        {
            return af == AF_INET || af == AF_INET6;
        }

        static Key fromAddress(const net::Address& address)
        {
            Key key;
            auto* raw = address.addr();
            if (!raw)
                return key;

            if (address.af() == AF_INET) {
                const auto* ipv4 = reinterpret_cast<const sockaddr_in*>(raw);
                key.af = AF_INET;
                key.size = 4;
                std::memcpy(key.bytes.data(), &ipv4->sin_addr, key.size);
            }
            else if (address.af() == AF_INET6) {
                const auto* ipv6 = reinterpret_cast<const sockaddr_in6*>(raw);
                key.af = AF_INET6;
                key.size = 16;
                std::memcpy(key.bytes.data(), &ipv6->sin6_addr, key.size);
            }
            return key;
        }

        static Key fromIP(const std::string& ip)
        {
            try {
                return fromAddress(net::Address(ip, 0));
            } catch (...) {
                return {};
            }
        }

        [[nodiscard]] bool matches(const net::Address& address) const
        {
            if (!valid())
                return false;

            Key other = fromAddress(address);
            return matches(other);
        }

        [[nodiscard]] bool matches(const Key& other) const
        {
            return valid() &&
                   other.valid() &&
                   other.af == af &&
                   other.size == size &&
                   std::memcmp(bytes.data(), other.bytes.data(), size) == 0;
        }
    };

    std::string ip; ///< IPv4 address string of the permitted peer.
    Key key;        ///< Binary IP key for allocation hot-path comparisons.
    Timeout timeout; ///< Countdown timer; expires after PERMISSION_LIFETIME milliseconds.

    /// Constructs a permission for @p ip and immediately starts the expiry timer.
    /// @param ip IPv4 address string of the permitted peer.
    Permission(const std::string& ip)
        : ip(ip)
        , key(Key::fromIP(ip))
        , timeout(PERMISSION_LIFETIME)
    {
        refresh();
    }

    /// Constructs a permission from a binary socket address.
    /// @param address Peer IP address; port is ignored.
    Permission(const net::Address& address)
        : ip(address.host())
        , key(Key::fromAddress(address))
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

    /// Binary IP comparison used on the TURN relay hot path.
    [[nodiscard]] bool matches(const net::Address& address) const { return key.matches(address); }
};


using PermissionList = std::vector<Permission>;


} // namespace turn
} // namespace icy


/// @\}
