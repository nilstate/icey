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


/// Computes an HMAC-SHA1 message authentication code.
///
/// Uses OpenSSL HMAC with SHA-1 as the underlying digest. The output is a
/// 20-byte raw binary string (not hex-encoded).
///
/// @param input Data to authenticate.
/// @param key   Secret key used for the HMAC computation.
/// @return 20-byte raw binary HMAC-SHA1 digest.
/// @throws std::runtime_error if OpenSSL returns an unexpected digest length.
[[nodiscard]] std::string computeHMAC(std::string_view input, std::string_view key);


} // namespace crypto
} // namespace icy


/// @\}
