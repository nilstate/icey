///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#include "icy/crypto/crypto.h"
#include "icy/random.h"

#include <mutex>
#include <stdexcept>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>


namespace icy {
namespace crypto {
namespace internal {


void throwError()
{
    unsigned long err;
    std::string msg;

    while ((err = ERR_get_error())) {
        if (!msg.empty())
            msg.append("; ");
        msg.append(ERR_error_string(err, nullptr));
    }

    throw std::runtime_error(msg);
}


void api(int ret, const char* error)
{
    if (ret == 0) {
        if (error)
            throw std::runtime_error(error);
        else
            throwError();
    }
}


//
// OpenSSL 1.1.0+ / 3.x initialization
//
// Since OpenSSL 1.1.0, library initialization is automatic and thread-safe.
// The old CRYPTO_set_locking_callback, SSL_library_init, SSL_load_error_strings,
// OpenSSL_add_all_algorithms, OPENSSL_config, EVP_cleanup, ERR_free_strings,
// and the dynlock callbacks are all unnecessary and removed in OpenSSL 3.0.
//
// We keep the ref-counted init/uninit pattern for RAND seeding, but the
// locking infrastructure is gone.
//

const int SEEDSIZE = 256;
static std::mutex _mutex;
static int _refCount(0);


void init()
{
    std::lock_guard<std::mutex> guard(_mutex);

    if (++_refCount == 1) {
        char seed[SEEDSIZE];
        Random::getSeed(seed, sizeof(seed));
        RAND_seed(seed, SEEDSIZE);
    }
}


void uninit()
{
    std::lock_guard<std::mutex> guard(_mutex);
    --_refCount;
}


} // namespace internal


/// Initializes the OpenSSL library.
void initializeEngine()
{
    internal::init();
}

/// Shuts down the OpenSSL library.
void uninitializeEngine()
{
    internal::uninit();
}


} // namespace crypto
} // namespace icy


/// @}
