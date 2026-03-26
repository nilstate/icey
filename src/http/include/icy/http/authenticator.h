///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "icy/http/http.h"
#include "icy/http/url.h"
#include <string>
#include <string_view>


namespace icy {
namespace http {


class HTTP_API Request;
class HTTP_API Response;


/// Maintains HTTP Basic or Digest authentication state for outbound requests.
///
/// Note: Do not forget to read the entire response stream from the 401 response
/// before sending the authenticated request, otherwise there may be
/// problems if a persistent connection is used.
class HTTP_API Authenticator
{
public:
    /// Creates an empty authenticator.
    Authenticator();

    /// Creates an authenticator with the given username and password.
    Authenticator(const std::string& username, const std::string& password);

    /// Destroys the authenticator.
    ~Authenticator();

    /// Parses username:password std::string and sets username and password of
    /// the credentials object.
    /// Throws SyntaxException on invalid user information.
    void fromUserInfo(std::string_view userInfo);

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

/// Encodes and decodes HTTP Basic authentication credentials.
class HTTP_API BasicAuthenticator
{
public:
    /// Creates an empty basic authenticator.
    BasicAuthenticator();

    /// Creates a basic authenticator with the given username and password.
    BasicAuthenticator(const std::string& username,
                       const std::string& password);

    /// Extracts basic authentication credentials from the given request.
    ///
    /// Throws a NotAuthenticatedException if the request does
    /// not contain basic authentication information.
    explicit BasicAuthenticator(const http::Request& request);

    /// Parses a raw Basic authentication payload string.
    /// The value can be extracted from a request via `Request::getCredentials()`.
    explicit BasicAuthenticator(const std::string& authInfo);

    /// Destroys the basic authenticator.
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
    void parseAuthInfo(std::string_view authInfo);

private:
    BasicAuthenticator(const BasicAuthenticator&) = delete;
    BasicAuthenticator& operator=(const BasicAuthenticator&) = delete;

    std::string _username;
    std::string _password;
};


//
// Helpers
//


/// Returns true if the given Authorization header value uses HTTP Basic authentication.
/// @param header Value of the Authorization or WWW-Authenticate header.
[[nodiscard]] bool isBasicCredentials(std::string_view header);

/// Returns true if the given Authorization header value uses HTTP Digest authentication.
/// @param header Value of the Authorization or WWW-Authenticate header.
[[nodiscard]] bool isDigestCredentials(std::string_view header);

/// Returns true if the request contains a Basic Authorization header.
/// @param request HTTP request to inspect.
[[nodiscard]] bool hasBasicCredentials(const http::Request& request);

/// Returns true if the request contains a Digest Authorization header.
/// @param request HTTP request to inspect.
[[nodiscard]] bool hasDigestCredentials(const http::Request& request);

/// Returns true if the request contains a Basic Proxy-Authorization header.
/// @param request HTTP request to inspect.
[[nodiscard]] bool hasProxyBasicCredentials(const http::Request& request);

/// Returns true if the request contains a Digest Proxy-Authorization header.
/// @param request HTTP request to inspect.
[[nodiscard]] bool hasProxyDigestCredentials(const http::Request& request);

/// Splits a "user:password" user-info string into separate username and password strings.
/// If no ':' is present, the entire string is treated as the username and password is empty.
/// @param userInfo Input string in the form "user:password".
/// @param username Receives the extracted username.
/// @param password Receives the extracted password.
void extractCredentials(std::string_view userInfo, std::string& username,
                        std::string& password);

/// Extracts username and password from the user-info component of a URL.
/// Does nothing if the URL has no user-info part.
/// @param uri URL to extract credentials from.
/// @param username Receives the extracted username.
/// @param password Receives the extracted password.
void extractCredentials(const http::URL& uri, std::string& username,
                        std::string& password);


} // namespace http
} // namespace icy


/// @}
