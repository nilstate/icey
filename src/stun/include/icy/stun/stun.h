///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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
/// STUN messages, attributes, and transaction helpers defined by RFC 5389.
namespace stun {


// Following values correspond to RFC5389.
constexpr int kAttributeHeaderSize = 4;     ///< Bytes in a STUN attribute header.
constexpr int kMessageHeaderSize = 20;      ///< Bytes in the fixed STUN message header.
constexpr int kTransactionIdOffset = 8;     ///< Byte offset of the transaction ID inside the message header.
constexpr int kTransactionIdLength = 12;    ///< Length in bytes of a STUN transaction ID.
constexpr uint32_t kMagicCookie = 0x2112A442; ///< RFC 5389 magic cookie used by modern STUN/TURN messages.
constexpr int kMagicCookieLength = sizeof(kMagicCookie); ///< Length in bytes of the magic cookie field.


/// STUN address types as defined in RFC 5389.
/// NB: Undefined is not part of the STUN spec.
enum class AddressFamily : uint8_t
{
    Undefined = 0, ///< Not a valid STUN address family; used as a sentinel.
    IPv4 = 1,      ///< IPv4 transport address.
    IPv6 = 2       ///< IPv6 transport address.
};


} // namespace stun
} // namespace icy


/// @}
