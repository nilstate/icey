///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/ssladapter.h"
#include "scy/logger.h"
#include "scy/net/sslmanager.h"
#include "scy/net/sslsocket.h"
#include <algorithm>
#include <iterator>
#include <openssl/x509v3.h>
#include <stdexcept>
#include <vector>

using namespace std;


namespace scy {
namespace net {


SSLAdapter::SSLAdapter(net::SSLSocket* socket)
    : _socket(socket)
    , _ssl(nullptr)
    , _readBIO(nullptr)
    , _writeBIO(nullptr)
{
    // LTrace("Create");
}


SSLAdapter::~SSLAdapter() noexcept
{
    // LTrace("Destroy");
    if (_ssl) {
        SSL_free(_ssl);
        _ssl = nullptr;
    }
    // LTrace("Destroy: OK");
}


void SSLAdapter::initClient()
{
    // LTrace("Init client");
    if (!_socket)
        throw std::runtime_error("SSLAdapter: socket is null");
    if (!_socket->context())
        _socket->useContext(SSLManager::instance().defaultClientContext());
    if (_socket->context()->isForServerUse())
        throw std::logic_error("SSLAdapter: client init called with server context");

    _ssl = SSL_new(_socket->context()->sslContext());

    // Enable hostname verification if a hostname was set
    if (!_hostname.empty()) {
        // Set SNI hostname for the TLS handshake
        SSL_set_tlsext_host_name(_ssl, _hostname.c_str());

        // Enable certificate hostname verification (OpenSSL 1.1.0+)
        SSL_set_hostflags(_ssl, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
        if (!SSL_set1_host(_ssl, _hostname.c_str()))
            LWarn("Failed to set hostname verification for: ", _hostname);
    }

    // Reuse previous SSL session if available
    if (_socket->currentSession())
        SSL_set_session(_ssl, _socket->currentSession()->sslSession());

    _readBIO = BIO_new(BIO_s_mem());
    _writeBIO = BIO_new(BIO_s_mem());
    SSL_set_bio(_ssl, _readBIO, _writeBIO);
    SSL_set_connect_state(_ssl);
    SSL_do_handshake(_ssl);
}


void SSLAdapter::initServer() //(SSL* ssl)
{
    // LTrace("Init server");
    if (!_socket)
        throw std::runtime_error("SSLAdapter: socket is null");
    if (!_socket->context())
        _socket->useContext(SSLManager::instance().defaultServerContext());
    if (!_socket->context()->isForServerUse())
        throw std::logic_error("SSLAdapter: server init called with client context");

    _ssl = SSL_new(_socket->context()->sslContext());
    _readBIO = BIO_new(BIO_s_mem());
    _writeBIO = BIO_new(BIO_s_mem());
    SSL_set_bio(_ssl, _readBIO, _writeBIO);
    SSL_set_accept_state(_ssl);
    SSL_do_handshake(_ssl);
}


void SSLAdapter::shutdown()
{
    // LTrace("Shutdown");
    if (_ssl) {
        // LTrace("Shutdown SSL");

        // Don't shut down the socket more than once.
        int shutdownState = SSL_get_shutdown(_ssl);
        bool shutdownSent =
            (shutdownState & SSL_SENT_SHUTDOWN) == SSL_SENT_SHUTDOWN;
        if (!shutdownSent) {
            // A proper clean shutdown would require us to
            // retry the shutdown if we get a zero return
            // value, until SSL_shutdown() returns 1.
            // However, this will lead to problems with
            // most web browsers, so we just set the shutdown
            // flag by calling SSL_shutdown() once and be
            // done with it.
            int rc = SSL_shutdown(_ssl);
            if (rc < 0)
                handleError(rc);
        }
    }
}


bool SSLAdapter::initialized() const
{
    return !!_ssl;
}


bool SSLAdapter::ready() const
{
    return _ssl && SSL_is_init_finished(_ssl);
}


int SSLAdapter::available() const
{
    if (!_ssl)
        return 0;
    return SSL_pending(_ssl);
}


void SSLAdapter::setHostname(const std::string& hostname)
{
    _hostname = hostname;
}


void SSLAdapter::addIncomingData(const char* data, size_t len)
{
    // LTrace("Add incoming data: ", len);
    if (!_readBIO)
        throw std::runtime_error("SSLAdapter: read BIO not initialized");
    BIO_write(_readBIO, data, static_cast<int>(len));
    flush();
}


void SSLAdapter::addOutgoingData(std::string_view s)
{
    addOutgoingData(s.data(), s.size());
}


void SSLAdapter::addOutgoingData(const char* data, size_t len)
{
    std::copy(data, data + len, std::back_inserter(_bufferOut));
}


void SSLAdapter::handshake()
{
    int r = SSL_do_handshake(_ssl);
    if (r < 0)
        handleError(r);
}


void SSLAdapter::flush()
{
    LTrace("Flushing");

    // Keep trying to handshake until initialized
    if (!ready())
        return handshake();

    // Read any decrypted remote data from SSL and emit to the app
    flushReadBIO();

    // Write any local data to SSL for excryption
    if (_bufferOut.size() > 0) {
        int r = SSL_write(_ssl, &_bufferOut[0], static_cast<int>(_bufferOut.size()));
        if (r < 0) {
            handleError(r);
        }
        _bufferOut.clear();
        // flushWriteBIO();
    }

    // Send any encrypted data from SSL to the remote peer
    flushWriteBIO();
}


void SSLAdapter::flushReadBIO()
{
    size_t npending = BIO_ctrl_pending(_readBIO);
    if (npending > 0) {
        int nread;
        std::vector<char> buffer(npending);
        while ((nread = SSL_read(_ssl, buffer.data(), npending)) > 0) {
            _socket->onRecv(mutableBuffer(buffer.data(), nread));
        }
    }
}


void SSLAdapter::flushWriteBIO()
{
    size_t npending = BIO_ctrl_pending(_writeBIO);
    if (npending > 0) {
        std::vector<char> buffer(npending);
        int nread = BIO_read(_writeBIO, buffer.data(), npending);
        if (nread > 0) {
            _socket->write(buffer.data(), nread);
        }
    }
}


void SSLAdapter::handleError(int rc)
{
    if (rc >= 0)
        return;
    int error = SSL_get_error(_ssl, rc);
    switch (error) {
        case SSL_ERROR_ZERO_RETURN:
            // LTrace("SSL_ERROR_ZERO_RETURN");
            return;
        case SSL_ERROR_WANT_READ:
            // LTrace("SSL_ERROR_WANT_READ");
            flushWriteBIO();
            break;
        case SSL_ERROR_WANT_WRITE:
            // LTrace("SSL_ERROR_WANT_WRITE");
            flushWriteBIO();
            break;
        case SSL_ERROR_WANT_CONNECT:
        case SSL_ERROR_WANT_ACCEPT:
        case SSL_ERROR_WANT_X509_LOOKUP:
            LWarn("Unexpected SSL error state: ", error);
            break;
        default:
            char buffer[256];
            ERR_error_string_n(ERR_get_error(), buffer, sizeof(buffer));
            std::string msg(buffer);
            // throw std::runtime_error("SSL connection failed: " + msg);
            _socket->setError("SSL connection failed: " + msg);
            break;
    }
}


} // namespace net
} // namespace scy


/// @\}
