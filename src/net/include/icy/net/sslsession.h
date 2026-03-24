///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "icy/crypto/crypto.h"
#include "icy/net/net.h"

#include <memory>
#include <openssl/ssl.h>


namespace icy {
namespace net {


/// This class encapsulates a SSL session object
/// used with session caching on the client side.
///
/// For session caching to work, a client must
/// save the session object from an existing connection,
/// if it wants to reuse it with a future connection.
class Net_API SSLSession
{
public:
    using Ptr = std::shared_ptr<SSLSession>;

    /// Returns the stored OpenSSL SSL_SESSION object.
    SSL_SESSION* sslSession() const;

    /// Creates a new SSLSession wrapping the given OpenSSL session object.
    ///
    /// The SSL_SESSION's reference count is not incremented; the SSLSession
    /// takes ownership and will call SSL_SESSION_free() on destruction.
    /// @param ptr The OpenSSL session object to wrap.
    SSLSession(SSL_SESSION* ptr);

    /// Destroys the Session.
    ///
    /// Calls SSL_SESSION_free() on the stored
    /// SSL_SESSION object.
    ~SSLSession() noexcept;

    /// Constructs an empty SSLSession with a null session pointer.
    SSLSession();

    SSLSession(const SSLSession&) = delete;
    SSLSession& operator=(const SSLSession&) = delete;
    SSLSession(SSLSession&&) = delete;
    SSLSession& operator=(SSLSession&&) = delete;

protected:
    SSL_SESSION* _ptr;
};


} // namespace net
} // namespace icy


/// @}
