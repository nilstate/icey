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


/// Alias for the OpenSSL RSA key type, brought into the icy::crypto namespace.
///
/// Currently a transparent alias for the OpenSSL RSA struct. Use OpenSSL
/// RSA_* functions directly to create, populate, and free RSAKey objects.
/// This alias exists as a stable forward-declaration point; a higher-level
/// RAII wrapper may replace it in a future version.
using RSAKey = ::RSA;


} // namespace crypto
} // namespace icy


/// @}
