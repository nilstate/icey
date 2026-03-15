///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/authenticator.h"
#include "scy/base64.h"
#include "scy/http/request.h"
#include "scy/http/response.h"


namespace scy {
namespace http {


Authenticator::Authenticator()
{
}


Authenticator::Authenticator(const std::string& username,
                             const std::string& password)
    : _username(username)
    , _password(password)
{
}


Authenticator::~Authenticator()
{
}


void Authenticator::fromUserInfo(const std::string& userInfo)
{
    std::string username;
    std::string password;
    extractCredentials(userInfo, username, password);
    setUsername(username);
    setPassword(password);
}


void Authenticator::fromURI(const http::URL& uri)
{
    std::string username;
    std::string password;
    extractCredentials(uri, username, password);
    setUsername(username);
    setPassword(password);
}


void Authenticator::authenticate(http::Request& request, const http::Response& response)
{
    for (http::Response::ConstIterator iter = response.find("WWW-Authenticate");
         iter != response.end(); ++iter) {
        if (isBasicCredentials(iter->second)) {
            BasicAuthenticator(_username, _password).authenticate(request);
            return;
        }
        // Digest authentication not implemented
    }
}


void Authenticator::updateAuthInfo(http::Request& request)
{
    if (request.has("Authorization")) {
        const std::string& authorization = request.get("Authorization");

        if (isBasicCredentials(authorization)) {
            BasicAuthenticator(_username, _password).authenticate(request);
        }
        // Digest authentication not implemented
    }
}


void Authenticator::proxyAuthenticate(http::Request& request, const http::Response& response)
{
    for (http::Response::ConstIterator iter = response.find("Proxy-Authenticate");
         iter != response.end(); ++iter) {
        if (isBasicCredentials(iter->second)) {
            BasicAuthenticator(_username, _password).proxyAuthenticate(request);
            return;
        }
        // Digest authentication not implemented
    }
}


void Authenticator::updateProxyAuthInfo(http::Request& request)
{
    if (request.has("Proxy-Authorization")) {
        const std::string& authorization = request.get("Proxy-Authorization");

        if (isBasicCredentials(authorization)) {
            BasicAuthenticator(_username, _password).proxyAuthenticate(request);
        }
        // Digest authentication not implemented
    }
}


void Authenticator::setUsername(const std::string& username)
{
    _username = username;
}


const std::string& Authenticator::username() const
{
    return _username;
}


void Authenticator::setPassword(const std::string& password)
{
    _password = password;
}


const std::string& Authenticator::password() const
{
    return _password;
}


//
// Helpers
//


bool isBasicCredentials(const std::string& header)
{
    return (header.size() > 5 ? ::isspace(header[5]) : true) &&
           util::icompare(header.substr(0, 5), "Basic") == 0;
}


bool isDigestCredentials(const std::string& header)
{
    return (header.size() > 6 ? ::isspace(header[6]) : true) &&
           util::icompare(header.substr(0, 6), "Digest") == 0;
}


bool hasBasicCredentials(const http::Request& request)
{
    return request.has("Authorization") &&
           isBasicCredentials(request.get("Authorization"));
}


bool hasDigestCredentials(const http::Request& request)
{
    return request.has("Authorization") &&
           isDigestCredentials(request.get("Authorization"));
}


bool hasProxyBasicCredentials(const http::Request& request)
{
    return request.has("Proxy-Authorization") &&
           isBasicCredentials(request.get("Proxy-Authorization"));
}


bool hasProxyDigestCredentials(const http::Request& request)
{
    return request.has("Proxy-Authorization") &&
           isDigestCredentials(request.get("Proxy-Authorization"));
}


void extractCredentials(const std::string& userInfo, std::string& username, std::string& password)
{
    const std::string::size_type p = userInfo.find(':');

    if (p != std::string::npos) {
        username.assign(userInfo, 0, p);
        password.assign(userInfo, p + 1, std::string::npos);
    } else {
        username.assign(userInfo);
        password.clear();
    }
}


void extractCredentials(const http::URL& uri, std::string& username,
                        std::string& password)
{
    if (!uri.userInfo().empty()) {
        extractCredentials(uri.userInfo(), username, password);
    }
}


//
// Basic Authenticator
//


BasicAuthenticator::BasicAuthenticator()
{
}


BasicAuthenticator::BasicAuthenticator(const std::string& username,
                                       const std::string& password)
    : _username(username)
    , _password(password)
{
}


BasicAuthenticator::BasicAuthenticator(const http::Request& request)
{
    std::string scheme;
    std::string authInfo;
    request.getCredentials(scheme, authInfo);
    if (util::icompare(scheme, "Basic") == 0) {
        parseAuthInfo(authInfo);
    } else
        throw std::runtime_error("Basic authentication expected");
}


BasicAuthenticator::BasicAuthenticator(const std::string& authInfo)
{
    parseAuthInfo(authInfo);
}


BasicAuthenticator::~BasicAuthenticator()
{
}


void BasicAuthenticator::setUsername(const std::string& username)
{
    _username = username;
}


void BasicAuthenticator::setPassword(const std::string& password)
{
    _password = password;
}


void BasicAuthenticator::authenticate(http::Request& request) const
{
    request.setCredentials("Basic", base64::encode(_username + ":" + _password, 0));
}


void BasicAuthenticator::proxyAuthenticate(http::Request& request) const
{
    request.setProxyCredentials("Basic", base64::encode(_username + ":" + _password, 0));
}


void BasicAuthenticator::parseAuthInfo(const std::string& authInfo)
{
    std::string res = base64::decode(authInfo);
    http::extractCredentials(authInfo, _username, _password);

    /*
    const std::string::size_type p = userInfo.find(':');

    if (p != std::string::npos)
    {
        username.assign(userInfo, 0, p);
        password.assign(userInfo, p + 1, std::string::npos);
    }
    else
    {
        username.assign(userInfo);
        password.clear();
    }

    std::vector<std::string> fragments;
    util::split(res, ':', fragments);
    if (fragments.size() == 2) {
        _username = fragments[0];
        _password = fragments[1];
    }
    else throw std::invalid_argument("Invalid Basic authentication data");
    */
}


const std::string& BasicAuthenticator::username() const
{
    return _username;
}


const std::string& BasicAuthenticator::password() const
{
    return _password;
}


} // namespace http
} // namespace scy


/// @\}


//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
