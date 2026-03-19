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


namespace icy {
namespace net {


/// SSL socket implementation.
class Net_API SSLSocket : public TCPSocket
{
public:
    using Ptr = std::shared_ptr<SSLSocket>;
    using Vec = std::vector<Ptr>;

    SSLSocket(uv::Loop* loop = uv::defaultLoop()); //, SocketMode mode = ClientSide
    SSLSocket(SSLContext::Ptr sslContext, uv::Loop* loop = uv::defaultLoop());
    SSLSocket(SSLContext::Ptr sslContext, SSLSession::Ptr session,
              uv::Loop* loop = uv::defaultLoop());

    virtual ~SSLSocket() noexcept;

    /// Initialize the SSLSocket with the given SSLContext.
    // virtual void init(SSLContext::Ptr sslContext, SocketMode mode = ClientSide);

    /// Initializes the socket and establishes a secure connection to
    /// the TCP server at the given address.
    ///
    /// The SSL handshake is performed when the socket is connected.
    virtual void connect(const Address& peerAddress) override;
    virtual void connect(const std::string& host, uint16_t port) override;
    virtual void bind(const net::Address& address, unsigned flags = 0) override;
    virtual void listen(int backlog = 64) override;

    /// Shuts down the connection by attempting
    /// an orderly SSL shutdown, then actually
    /// shutting down the TCP connection.
    [[nodiscard]] virtual bool shutdown() override;

    /// Closes the socket forcefully.
    virtual void close() override;

    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, int flags = 0) override;
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len,
                                       const net::Address& peerAddress, int flags = 0) override;

    /// Set the expected peer hostname for certificate verification and SNI.
    /// Must be called before connect() to enable hostname verification.
    void setHostname(const std::string& hostname);

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

    /// Returns the peer's certificate.
    X509* peerCertificate() const;

    net::TransportType transport() const override;

    virtual void acceptConnection();

    virtual void onConnect();

    /// Reads raw encrypted SSL data
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
