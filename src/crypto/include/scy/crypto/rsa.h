///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#pragma once


#include "scy/crypto/crypto.h"
#include <openssl/rsa.h>


namespace scy {
namespace crypto {


/// Forward the OpenSSL type to our namespace.
/// This may become a class/wrapper in the future.
using RSAKey = ::RSA;


} // namespace crypto
} // namespace scy


/// @\}
