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


#include "icy/buffer.h"
#include "icy/crypto/crypto.h"
#include "icy/handle.h"
#include "icy/net/address.h"
#include "icy/net/net.h"

#include <string>
#include <string_view>
#include <vector>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>


namespace icy {
namespace net {


/// A wrapper for the OpenSSL SSL connection context.
/// Currently coupled to SSLSocket for BIO read/write callbacks.
class Net_API SSLSocket;
/// Manages the OpenSSL context and BIO buffers for an SSL socket connection
class Net_API SSLAdapter
{
public:
    /// Constructs the SSLAdapter and associates it with the given socket.
    /// The socket pointer must remain valid for the lifetime of this adapter.
    /// @param socket The owning SSLSocket that sends and receives raw data.
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

    /// Feeds encrypted data received from the network into the SSL read BIO.
    /// Triggers a flush, which drives the handshake or decrypts and delivers
    /// plaintext to the socket via onRecv().
    /// @param data Pointer to the encrypted bytes.
    /// @param len  Number of bytes to feed.
    void addIncomingData(const char* data, size_t len);

    /// Queues plaintext data for encryption and transmission.
    /// @param data String view of the plaintext payload.
    void addOutgoingData(std::string_view data);

    /// Queues plaintext data for encryption and transmission.
    /// @param data Pointer to the plaintext bytes.
    /// @param len  Number of bytes to queue.
    void addOutgoingData(const char* data, size_t len);

    /// Moves plaintext data into the pending write buffer when possible.
    void addOutgoingData(Buffer&& data);

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
} // namespace icy


/// @\}
