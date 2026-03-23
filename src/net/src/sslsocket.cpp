///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "icy/net/sslsocket.h"
#include "icy/logger.h"
#include "icy/net/sslmanager.h"


using namespace std;


namespace icy {
namespace net {


SSLSocket::SSLSocket(uv::Loop* loop)
    : TCPSocket(loop)
    , _sslContext(nullptr)
    , _sslSession(nullptr)
    , _sslAdapter(this)
{
    // LTrace("Create");
}


SSLSocket::SSLSocket(SSLContext::Ptr context, uv::Loop* loop)
    : TCPSocket(loop)
    , _sslContext(context)
    , _sslSession(nullptr)
    , _sslAdapter(this)
{
    // LTrace("Create");
}


SSLSocket::SSLSocket(SSLContext::Ptr context, SSLSession::Ptr session, uv::Loop* loop)
    : TCPSocket(loop)
    , _sslContext(context)
    , _sslSession(session)
    , _sslAdapter(this)
{
    // LTrace("Create");
}


SSLSocket::~SSLSocket() noexcept
{
    // LTrace("Destroy");
}


void SSLSocket::setHostname(const std::string& hostname)
{
    _sslAdapter.setHostname(hostname);
}


void SSLSocket::connect(const Address& peerAddress)
{
    TCPSocket::connect(peerAddress);
}


void SSLSocket::connect(const std::string& host, uint16_t port)
{
    // Capture the hostname for SNI and certificate verification
    _sslAdapter.setHostname(host);
    TCPSocket::connect(host, port);
}


int SSLSocket::available() const
{
    return _sslAdapter.available();
}


void SSLSocket::close()
{
    TCPSocket::close();
}


bool SSLSocket::shutdown()
{
    // LTrace("Shutdown");
    try {
        // Try to gracefully shutdown the SSL connection
        _sslAdapter.shutdown();
    } catch (...) {
    }
    return TCPSocket::shutdown();
}


ssize_t SSLSocket::send(const char* data, size_t len, int flags)
{
    return send(data, len, peerAddress(), flags);
}


ssize_t SSLSocket::sendOwned(Buffer&& buffer, int flags)
{
    return sendOwned(std::move(buffer), peerAddress(), flags);
}


void SSLSocket::bind(const net::Address& address, unsigned flags)
{
    if (!_sslContext->isForServerUse())
        throw std::logic_error("SSLSocket: server context required");
    TCPSocket::bind(address, flags);
}


void SSLSocket::listen(int backlog)
{
    if (!_sslContext->isForServerUse())
        throw std::logic_error("SSLSocket: server context required");
    TCPSocket::listen(backlog);
}


ssize_t SSLSocket::send(const char* data, size_t len, const net::Address& /* peerAddress */, int /* flags */)
{
    // LTrace("Send: ", len);
    // assert(len <= net::MAX_TCP_PACKET_SIZE);

    if (!active()) {
        LWarn("Send error");
        return -1;
    }

    // Send unencrypted data to the SSL context

    if (!_sslAdapter._ssl)
        throw std::logic_error("SSLSocket: SSL not initialized");

    _sslAdapter.addOutgoingData(data, len);
    _sslAdapter.flush();
    return len;
}


ssize_t SSLSocket::sendOwned(Buffer&& buffer, const net::Address& /* peerAddress */, int /* flags */)
{
    if (!active()) {
        LWarn("Send error");
        return -1;
    }

    if (!_sslAdapter._ssl)
        throw std::logic_error("SSLSocket: SSL not initialized");

    auto len = buffer.size();
    _sslAdapter.addOutgoingData(std::move(buffer));
    _sslAdapter.flush();
    return static_cast<ssize_t>(len);
}


void SSLSocket::acceptConnection()
{
    if (!_sslContext->isForServerUse())
        throw std::logic_error("SSLSocket: server context required");

    // Create the shared socket pointer so the if the socket handle is not
    // incremented the accepted socket will be destroyed.
    auto socket = std::make_shared<net::SSLSocket>(_sslContext, loop());

    // LTrace("Accept SSL connection: ", socket->ptr());
    // invoke(&uv_tcp_init, loop(), socket->get()); // "Cannot initialize SSL socket"

    if (uv_accept(get<uv_stream_t>(), socket->get<uv_stream_t>()) == 0) {
        socket->readStart();
        socket->_sslAdapter.initServer();

        AcceptConnection.emit(socket);
    } else {
        LError("uv_accept failed");
    }
}


void SSLSocket::useSession(SSLSession::Ptr session)
{
    _sslSession = session;
}


SSLSession::Ptr SSLSocket::currentSession()
{
    if (_sslAdapter._ssl) {
        SSL_SESSION* session = SSL_get1_session(_sslAdapter._ssl);
        if (session) {
            if (_sslSession && session == _sslSession->sslSession()) {
                SSL_SESSION_free(session);
                return _sslSession;
            } else
                return std::make_shared<SSLSession>(session); // new SSLSession(session);
        }
    }
    return nullptr;
}


void SSLSocket::useContext(SSLContext::Ptr context)
{
    if (_sslAdapter._ssl)
        throw std::runtime_error(
            "Cannot change the SSL context for an active socket.");

    _sslContext = context;
}


SSLContext::Ptr SSLSocket::context() const
{
    return _sslContext;
}


bool SSLSocket::sessionWasReused()
{
    if (_sslAdapter._ssl)
        return SSL_session_reused(_sslAdapter._ssl) != 0;
    else
        return false;
}


net::TransportType SSLSocket::transport() const
{
    return net::SSLTCP;
}


//
// Callbacks

void SSLSocket::onRead(const char* data, size_t len)
{
    // LTrace("On SSL read: ", len);

    // SSL encrypted data is sent to the SSL context
    _sslAdapter.addIncomingData(data, len);
    _sslAdapter.flush();
}


void SSLSocket::onConnect()
{
    // LTrace("On connect");
    if (readStart()) {
        _sslAdapter.initClient();
        // _sslAdapter.start();
        onSocketConnect(*this);
    }
}


} // namespace net
} // namespace icy


/// @\}
