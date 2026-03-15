///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "scy/http/http.h"
#include "scy/http/url.h"
#include <string>


namespace scy {
namespace http {


class HTTP_API Request;
class HTTP_API Response;


/// @ingroup http
/// This is a utility class for working with HTTP
/// authentication (basic or digest) in http::Request objects.
///
/// Note: Do not forget to read the entire response stream from the 401 response
/// before sending the authenticated request, otherwise there may be
/// problems if a persistent connection is used.
class HTTP_API Authenticator
{
public:
    /// Creates an empty Authenticator object.
    Authenticator();

    /// Creates an Authenticator object with the given username and password.
    Authenticator(const std::string& username, const std::string& password);

    /// Destroys the Authenticator.
    ~Authenticator();

    /// Parses username:password std::string and sets username and password of
    /// the credentials object.
    /// Throws SyntaxException on invalid user information.
    void fromUserInfo(const std::string& userInfo);

    /// Extracts username and password from the given URI and sets username
    /// and password of the credentials object.
    /// Does nothing if URI has no user info part.
    void fromURI(const http::URL& uri);

    /// Sets the username.
    void setUsername(const std::string& username);

    /// Returns the username.
    [[nodiscard]] const std::string& username() const;

    /// Sets the password.
    void setPassword(const std::string& password);

    /// Returns the password.
    [[nodiscard]] const std::string& password() const;

    /// Inspects WWW-Authenticate header of the response, initializes
    /// the internal state (in case of digest authentication) and
    /// adds required information to the given http::Request.
    void authenticate(http::Request& request, const http::Response& response);

    /// Updates internal state (in case of digest authentication) and
    /// replaces authentication information in the request accordingly.
    void updateAuthInfo(http::Request& request);

    /// Inspects Proxy-Authenticate header of the response, initializes
    /// the internal state (in case of digest authentication) and
    /// adds required information to the given http::Request.
    void proxyAuthenticate(http::Request& request,
                           const http::Response& response);

    /// Updates internal state (in case of digest authentication) and
    /// replaces proxy authentication information in the request accordingly.
    void updateProxyAuthInfo(http::Request& request);

private:
    Authenticator(const Authenticator&) = delete;
    Authenticator& operator=(const Authenticator&) = delete;

    std::string _username;
    std::string _password;
};


//
// Basic Authenticator (rfc2617)
//

/// @ingroup http
/// This is a utility class for working with HTTP Basic
/// Authentication in http::Request objects.
class HTTP_API BasicAuthenticator
{
public:
    /// Creates an empty BasicAuthenticator object.
    BasicAuthenticator();

    /// Creates a BasicAuthenticator object with the given username and
    /// password.
    BasicAuthenticator(const std::string& username,
                       const std::string& password);

    /// Creates a BasicAuthenticator object with the authentication information
    /// from the given request.
    ///
    /// Throws a NotAuthenticatedException if the request does
    /// not contain basic authentication information.
    explicit BasicAuthenticator(const http::Request& request);

    /// Creates a BasicAuthenticator object with the authentication information
    /// in the given std::string. The authentication information can be
    /// extracted
    /// from a http::Request object by calling http::Request::getCredentials().
    explicit BasicAuthenticator(const std::string& authInfo);

    /// Destroys the BasicAuthenticator.
    ~BasicAuthenticator();

    /// Sets the username.
    void setUsername(const std::string& username);

    /// Returns the username.
    [[nodiscard]] const std::string& username() const;

    /// Sets the password.
    void setPassword(const std::string& password);

    /// Returns the password.
    [[nodiscard]] const std::string& password() const;

    /// Adds authentication information to the given http::Request.
    void authenticate(http::Request& request) const;

    /// Adds proxy authentication information to the given http::Request.
    void proxyAuthenticate(http::Request& request) const;

protected:
    /// Extracts username and password from Basic authentication info
    /// by base64-decoding authInfo and splitting the resulting
    /// std::string at the ':' delimiter.
    void parseAuthInfo(const std::string& authInfo);

private:
    BasicAuthenticator(const BasicAuthenticator&) = delete;
    BasicAuthenticator& operator=(const BasicAuthenticator&) = delete;

    std::string _username;
    std::string _password;
};


//
// Helpers
//


[[nodiscard]] bool isBasicCredentials(const std::string& header);
[[nodiscard]] bool isDigestCredentials(const std::string& header);
[[nodiscard]] bool hasBasicCredentials(const http::Request& request);
[[nodiscard]] bool hasDigestCredentials(const http::Request& request);
[[nodiscard]] bool hasProxyBasicCredentials(const http::Request& request);
[[nodiscard]] bool hasProxyDigestCredentials(const http::Request& request);

void extractCredentials(const std::string& userInfo, std::string& username,
                        std::string& password);

void extractCredentials(const http::URL& uri, std::string& username,
                        std::string& password);


} // namespace http
} // namespace scy


/// @}


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
