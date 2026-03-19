///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup stun STUN module (rfc5389)
///
/// The `stun` module contains a STUN (rfc5389) implementation.
///
/// @addtogroup stun
/// @{


#pragma once


#include "icy/base.h"
#include <cstdint>


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(STUN_EXPORTS)
#define STUN_API __declspec(dllexport)
#else
#define STUN_API __declspec(dllimport)
#endif
#else
#define STUN_API // nothing
#endif


namespace icy {
namespace stun {


// Following values correspond to RFC5389.
constexpr int kAttributeHeaderSize = 4;
constexpr int kMessageHeaderSize = 20;
constexpr int kTransactionIdOffset = 8;
constexpr int kTransactionIdLength = 12;
constexpr uint32_t kMagicCookie = 0x2112A442;
constexpr int kMagicCookieLength = sizeof(kMagicCookie);


/// STUN address types as defined in RFC 5389.
/// NB: Undefined is not part of the STUN spec.
enum class AddressFamily : uint8_t
{
    Undefined = 0,
    IPv4 = 1,
    IPv6 = 2
};


} // namespace stun
} // namespace icy


/// @\}
