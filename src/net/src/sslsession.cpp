///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/sslsession.h"


using namespace std;


namespace scy {
namespace net {


SSLSession::SSLSession(SSL_SESSION* ptr)
    : _ptr(ptr)
{
}


SSLSession::~SSLSession() noexcept
{
    SSL_SESSION_free(_ptr);
}


SSL_SESSION* SSLSession::sslSession() const
{
    return _ptr;
}


} // namespace net
} // namespace scy


/// @\}
