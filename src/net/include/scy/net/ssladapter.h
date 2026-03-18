///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "scy/crypto/crypto.h"
#include "scy/handle.h"
#include "scy/net/address.h"
#include "scy/net/net.h"

#include <string>
#include <string_view>
#include <vector>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>


namespace scy {
namespace net {


/// A wrapper for the OpenSSL SSL connection context.
/// Currently coupled to SSLSocket for BIO read/write callbacks.
class Net_API SSLSocket;
class Net_API SSLAdapter
{
public:
    SSLAdapter(net::SSLSocket* socket);
    ~SSLAdapter() noexcept;

    SSLAdapter(const SSLAdapter&) = delete;
    SSLAdapter& operator=(const SSLAdapter&) = delete;
    SSLAdapter(SSLAdapter&&) = delete;
    SSLAdapter& operator=(SSLAdapter&&) = delete;

    /// Initializes the SSL context as a client.
    void initClient();

    /// Initializes the SSL context as a server.
    void initServer();

    /// Returns true when SSL context has been initialized.
    bool initialized() const;

    /// Returns true when the handshake is complete.
    bool ready() const;

    /// Start/continue the SSL handshake process.
    void handshake();

    /// Returns the number of bytes available in
    /// the SSL buffer for immediate reading.
    int available() const;

    /// Issues an orderly SSL shutdown.
    void shutdown();

    /// Flushes the SSL read/write buffers.
    void flush();

    /// Set the expected peer hostname for certificate verification.
    /// Must be called before initClient() to enable hostname verification.
    void setHostname(const std::string& hostname);

    void addIncomingData(const char* data, size_t len);
    void addOutgoingData(std::string_view data);
    void addOutgoingData(const char* data, size_t len);

protected:
    void handleError(int rc);

    void flushReadBIO();
    void flushWriteBIO();

protected:
    friend class net::SSLSocket;

    net::SSLSocket* _socket;
    SSL* _ssl;
    BIO* _readBIO;                ///< The incoming buffer we write encrypted SSL data into
    BIO* _writeBIO;               ///<  The outgoing buffer we write to the socket
    std::vector<char> _bufferOut; ///<  The outgoing payload to be encrypted and sent
    std::string _hostname;        ///<  Expected peer hostname for verification
};


} // namespace net
} // namespace scy


/// @\}
