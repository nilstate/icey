///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#pragma once


#include "icy/crypto/crypto.h"
#include <string>
#include <string_view>


namespace icy {
namespace crypto {


/// HMAC is a MAC (message authentication code), i.e. a keyed hash function
/// used for message authentication, which is based on a hash function (SHA1).
///
/// Input is the data to be signed, and key is the private password.
[[nodiscard]] std::string computeHMAC(std::string_view input, std::string_view key);


} // namespace crypto
} // namespace icy


/// @\}
