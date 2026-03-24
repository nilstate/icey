///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "icy/net/sslmanager.h"
#include "icy/net/sslcontext.h"
#include "icy/singleton.h"


using namespace std;


namespace icy {
namespace net {


int SSLManager::verifyServerCallback(int ok, X509_STORE_CTX* pStore)
{
    return SSLManager::verifyCallback(true, ok, pStore);
}


int SSLManager::verifyClientCallback(int ok, X509_STORE_CTX* pStore)
{
    return SSLManager::verifyCallback(false, ok, pStore);
}


void SSLManager::initNoVerifyClient()
{
    net::SSLManager::instance().initializeClient(
        std::make_shared<net::SSLContext>(
            net::SSLContext::CLIENT_USE, "", "", "",
            net::SSLContext::VERIFY_NONE, 9, false,
            "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"));
}


void SSLManager::initNoVerifyServer(const std::string& privateKeyFile,
                                    const std::string& certificateFile)
{
    net::SSLManager::instance().initializeServer(
        std::make_shared<net::SSLContext>(
            net::SSLContext::SERVER_USE,
            privateKeyFile,
            certificateFile,
            "", net::SSLContext::VERIFY_NONE, 9, false,
            "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"));
}


SSLManager::SSLManager()
{
}


SSLManager::~SSLManager() noexcept
{
    shutdown();
}


void SSLManager::shutdown()
{
    PrivateKeyPassphraseRequired.detachAll();
    ClientVerificationError.detachAll();
    ServerVerificationError.detachAll();
    _defaultServerContext = nullptr;
    _defaultClientContext = nullptr;
}


Singleton<SSLManager>& singleton()
{
    static Singleton<SSLManager> singleton;
    return singleton;
}


SSLManager& SSLManager::instance()
{
    return *singleton().get();
}


void SSLManager::destroy()
{
    singleton().destroy();
}


void SSLManager::initializeServer(SSLContext::Ptr ptrContext)
{
    _defaultServerContext = ptrContext;
}


void SSLManager::initializeClient(SSLContext::Ptr ptrContext)
{
    _defaultClientContext = ptrContext;
}


SSLContext::Ptr SSLManager::defaultServerContext()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _defaultServerContext;
}


SSLContext::Ptr SSLManager::defaultClientContext()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _defaultClientContext;
}


int SSLManager::verifyCallback(bool server, int ok, X509_STORE_CTX* pStore)
{
    if (!ok) {
        X509* pCert = X509_STORE_CTX_get_current_cert(pStore);
        crypto::X509Certificate x509(pCert, true);
        int depth = X509_STORE_CTX_get_error_depth(pStore);
        int err = X509_STORE_CTX_get_error(pStore);
        std::string error(X509_verify_cert_error_string(err));
        VerificationErrorDetails args(x509, depth, err, error);
        if (server)
            SSLManager::instance().ServerVerificationError.emit(args);
        else
            SSLManager::instance().ClientVerificationError.emit(args);
        ok = args.getIgnoreError() ? 1 : 0;
    }

    return ok;
}


int SSLManager::privateKeyPassphraseCallback(char* pBuf, int size, int flag,
                                             void* userData)
{
    std::string pwd;
    SSLManager::instance().PrivateKeyPassphraseRequired.emit(
        /*&SSLManager::instance(), */ pwd);

    strncpy(pBuf, pwd.c_str(), static_cast<size_t>(size));
    pBuf[size - 1] = '\0';
    if (size > static_cast<int>(pwd.length()))
        size = static_cast<int>(pwd.length());

    return size;
}


void initializeSSL()
{
    crypto::initializeEngine();
}


void uninitializeSSL()
{
    SSLManager::instance().shutdown();
    crypto::uninitializeEngine();
}


//
// Verification Error Details
//


VerificationErrorDetails::VerificationErrorDetails(
    const crypto::X509Certificate& cert, int errDepth, int errNum,
    const std::string& errMsg)
    : _cert(cert)
    , _errorDepth(errDepth)
    , _errorNumber(errNum)
    , _errorMessage(errMsg)
    , _ignoreError(false)
{
}


VerificationErrorDetails::~VerificationErrorDetails() noexcept
{
}


} // namespace net
} // namespace icy


/// @}
