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
#include <openssl/rsa.h>


namespace icy {
namespace crypto {


/// Forward the OpenSSL type to our namespace.
/// This may become a class/wrapper in the future.
using RSAKey = ::RSA;


} // namespace crypto
} // namespace icy


/// @\}
