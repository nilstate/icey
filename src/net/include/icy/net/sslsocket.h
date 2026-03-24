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


#include "icy/handle.h"
#include "icy/net/socket.h"
#include "icy/net/ssladapter.h"
#include "icy/net/sslcontext.h"
#include "icy/net/sslsession.h"
#include "icy/net/tcpsocket.h"

#include <string_view>


namespace icy {
namespace net {


/// SSL socket implementation.
class Net_API SSLSocket : public TCPSocket
{
public:
    using Ptr = std::shared_ptr<SSLSocket>;
    using Vec = std::vector<Ptr>;

    /// Constructs an SSLSocket that acquires its context from SSLManager on first use.
    /// @param loop Event loop to use; defaults to the default loop.
    SSLSocket(uv::Loop* loop = uv::defaultLoop());

    /// Constructs an SSLSocket with an explicit SSL context.
    /// @param sslContext The SSL context to use for this connection.
    /// @param loop       Event loop to use; defaults to the default loop.
    SSLSocket(SSLContext::Ptr sslContext, uv::Loop* loop = uv::defaultLoop());

    /// Constructs an SSLSocket with an explicit context and a prior session for resumption.
    /// @param sslContext The SSL context to use for this connection.
    /// @param session    A previously saved session to attempt resumption with.
    /// @param loop       Event loop to use; defaults to the default loop.
    SSLSocket(SSLContext::Ptr sslContext, SSLSession::Ptr session,
              uv::Loop* loop = uv::defaultLoop());

    virtual ~SSLSocket() noexcept;

    /// Initialize the SSLSocket with the given SSLContext.
    // virtual void init(SSLContext::Ptr sslContext, SocketMode mode = ClientSide);

    /// Initiates a secure connection to the peer at the given address.
    ///
    /// The SSL handshake begins automatically once the TCP connection is established.
    /// @param peerAddress The remote address to connect to.
    virtual void connect(const Address& peerAddress) override;

    /// Resolves @p host and initiates a secure connection.
    ///
    /// Sets the hostname on the SSL adapter for SNI and certificate verification
    /// before resolving and connecting.
    /// @param host Hostname or IP address string.
    /// @param port Destination port.
    virtual void connect(std::string_view host, uint16_t port) override;

    /// Binds the socket to @p address for server-side use.
    /// Throws std::logic_error if the context is not a server context.
    /// @param address Local address to bind to.
    /// @param flags   Optional bind flags (passed to uv_tcp_bind).
    virtual void bind(const net::Address& address, unsigned flags = 0) override;

    /// Starts listening for incoming connections.
    /// Throws std::logic_error if the context is not a server context.
    /// @param backlog Maximum number of pending connections.
    virtual void listen(int backlog = 64) override;

    /// Shuts down the connection by attempting
    /// an orderly SSL shutdown, then actually
    /// shutting down the TCP connection.
    [[nodiscard]] virtual bool shutdown() override;

    /// Closes the socket forcefully.
    virtual void close() override;

    /// Encrypts and sends @p len bytes to the connected peer.
    /// @param data  Pointer to the plaintext payload.
    /// @param len   Number of bytes to send.
    /// @param flags Reserved; currently unused.
    /// @return Number of plaintext bytes accepted, or -1 on error.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, int flags = 0) override;
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, int flags = 0) override;

    /// Encrypts and sends @p len bytes, ignoring @p peerAddress (TCP is connected).
    /// @param data        Pointer to the plaintext payload.
    /// @param len         Number of bytes to send.
    /// @param peerAddress Ignored for SSL/TCP; present for interface conformance.
    /// @param flags       Reserved; currently unused.
    /// @return Number of plaintext bytes accepted, or -1 on error.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len,
                                       const net::Address& peerAddress, int flags = 0) override;
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer,
                                            const net::Address& peerAddress, int flags = 0) override;

    /// Set the expected peer hostname for certificate verification and SNI.
    /// Must be called before connect() to enable hostname verification.
    void setHostname(std::string_view hostname);

    /// Use the given SSL context for this socket.
    void useContext(SSLContext::Ptr context);

    /// Returns the SSL context used for this socket.
    SSLContext::Ptr context() const;

    /// Sets the SSL session to use for the next
    /// connection. Setting a previously saved Session
    /// object is necessary to enable session caching.
    ///
    /// To remove the currently set session, a nullptr pointer
    /// can be given.
    ///
    /// Must be called before connect() to be effective.
    void useSession(SSLSession::Ptr session);

    /// Returns the SSL session of the current connection,
    /// for reuse in a future connection (if session caching
    /// is enabled).
    ///
    /// If no connection is established, returns nullptr.
    SSLSession::Ptr currentSession();

    /// Returns true if a reused session was negotiated during
    /// the handshake.
    bool sessionWasReused();

    /// Returns the number of bytes available from the
    /// SSL buffer for immediate reading.
    int available() const;

    /// Returns the peer's X.509 certificate, or nullptr if no certificate was presented.
    X509* peerCertificate() const;

    /// Returns the SSLTCP transport protocol identifier.
    net::TransportType transport() const override;

    /// Accepts a pending client connection, initializes the server-side SSL
    /// context on the new socket, and fires the AcceptConnection signal.
    virtual void acceptConnection();

    /// Called when the TCP connection is established; starts reading and
    /// initiates the client-side SSL handshake.
    virtual void onConnect();

    /// Feeds raw encrypted bytes from the network into the SSL adapter.
    /// Called by the stream layer when ciphertext arrives from the peer.
    /// @param data Pointer to the encrypted bytes.
    /// @param len  Number of bytes received.
    virtual void onRead(const char* data, size_t len) override;

protected:
    net::SSLContext::Ptr _sslContext;
    net::SSLSession::Ptr _sslSession;
    net::SSLAdapter _sslAdapter;

    friend class net::SSLAdapter;
};


} // namespace net
} // namespace icy


/// @\}
