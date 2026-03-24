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


#include "icy/net/net.h"
#include "icy/net/sslcontext.h"
#include "icy/net/sslsession.h"
#include "icy/singleton.h"

#include <openssl/ssl.h>


namespace icy {
namespace net {


class Net_API VerificationErrorDetails;


/// SSLManager is a singleton for holding the default server/client
/// Context and handling callbacks for certificate verification errors
/// and private key passphrases.
class Net_API SSLManager
{
public:
    /// Initializes the server side of the SSLManager server-side SSLContext.
    void initializeServer(SSLContext::Ptr ptrContext);

    /// Initializes the client side of the SSLManager with a default client-side
    /// SSLContext.
    void initializeClient(SSLContext::Ptr ptrContext);

    /// Returns the default Context used by the server if initialized.
    SSLContext::Ptr defaultServerContext();

    /// Returns the default Context used by the client if initialized.
    SSLContext::Ptr defaultClientContext();

    /// Fired whenever a certificate verification error is detected by the
    /// server during a handshake.
    ThreadSignal<void(VerificationErrorDetails&)> ServerVerificationError;

    /// Fired whenever a certificate verification error is detected by the
    /// client during a handshake.
    ThreadSignal<void(VerificationErrorDetails&)> ClientVerificationError;

    /// Fired when a encrypted certificate is loaded. Not setting the password
    /// in the event parameter will result in a failure to load the certificate.
    ThreadSignal<void(std::string&)> PrivateKeyPassphraseRequired;

    /// Shuts down the SSLManager and releases the default Context
    /// objects. After a call to shutdown(), the SSLManager can no
    /// longer be used.
    ///
    /// Normally, it's not necessary to call this method directly, as this
    /// will be called either by uninitializeSSL(), or when
    /// the SSLManager instance is destroyed.
    void shutdown();

    /// Returns the instance of the SSLManager singleton.
    static SSLManager& instance();

    /// Shuts down and destroys the SSLManager singleton instance.
    static void destroy();

    /// Initializes a default no verify client context that's useful for
    /// testing.
    static void initNoVerifyClient();

    /// Initializes a default no verify server context that's useful for
    /// testing. Optionally accepts private key and certificate file paths
    /// for server identity; if omitted, no certificate is loaded.
    static void initNoVerifyServer(const std::string& privateKeyFile = "",
                                   const std::string& certificateFile = "");

protected:
    /// The return value of this method defines how errors in
    /// verification are handled. Return 0 to terminate the handshake,
    /// or 1 to continue despite the error.
    static int verifyClientCallback(int ok, X509_STORE_CTX* pStore);

    /// The return value of this method defines how errors in
    /// verification are handled. Return 0 to terminate the handshake,    '
    /// or 1 to continue despite the error.
    static int verifyServerCallback(int ok, X509_STORE_CTX* pStore);

    /// Method is invoked by OpenSSL to retrieve a passwd for an encrypted
    /// certificate.
    /// The request is delegated to the PrivatekeyPassword event. This method
    /// returns the
    /// length of the password.
    static int privateKeyPassphraseCallback(char* pBuf, int size, int flag,
                                            void* userData);

private:
    /// Creates the SSLManager.
    SSLManager();

    /// Destroys the SSLManager.
    ~SSLManager() noexcept;

    SSLManager(const SSLManager&) = delete;
    SSLManager& operator=(const SSLManager&) = delete;
    SSLManager(SSLManager&&) = delete;
    SSLManager& operator=(SSLManager&&) = delete;

    /// The return value of this method defines how errors in
    /// verification are handled. Return 0 to terminate the handshake,
    /// or 1 to continue despite the error.
    static int verifyCallback(bool server, int ok, X509_STORE_CTX* pStore);

    SSLContext::Ptr _defaultServerContext;
    SSLContext::Ptr _defaultClientContext;
    std::mutex _mutex;

    friend class Singleton<SSLManager>;
    friend class SSLContext;
};


//
// Verification Error Details
//

/// A utility class for certificate error handling.
class Net_API VerificationErrorDetails
{
public:
    /// Creates the VerificationErrorDetails. _ignoreError is per default set to
    /// false.
    VerificationErrorDetails(const crypto::X509Certificate& cert, int errDepth,
                             int errNum, const std::string& errMsg);

    /// Destroys the VerificationErrorDetails.
    ~VerificationErrorDetails() noexcept;

    /// Returns the certificate that caused the error.
    const crypto::X509Certificate& certificate() const;

    /// Returns the position of the certificate in the certificate chain.
    int errorDepth() const;

    /// Returns the id of the error
    int errorNumber() const;

    /// Returns the textual presentation of the errorNumber.
    const std::string& errorMessage() const;

    /// setIgnoreError to true, if a verification error is judged non-fatal by
    /// the user.
    void setIgnoreError(bool ignoreError);

    /// returns the value of _ignoreError
    bool getIgnoreError() const;

private:
    crypto::X509Certificate _cert;
    int _errorDepth;
    int _errorNumber;
    std::string _errorMessage; // Textual representation of the `_errorNumber`
    bool _ignoreError;
};


inline const crypto::X509Certificate&
VerificationErrorDetails::certificate() const
{
    return _cert;
}


inline int VerificationErrorDetails::errorDepth() const
{
    return _errorDepth;
}


inline int VerificationErrorDetails::errorNumber() const
{
    return _errorNumber;
}


inline const std::string& VerificationErrorDetails::errorMessage() const
{
    return _errorMessage;
}


inline void VerificationErrorDetails::setIgnoreError(bool ignoreError)
{
    _ignoreError = ignoreError;
}


inline bool VerificationErrorDetails::getIgnoreError() const
{
    return _ignoreError;
}


} // namespace net
} // namespace icy


/// @}
