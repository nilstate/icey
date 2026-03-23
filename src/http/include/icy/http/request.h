///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "icy/base.h"
#include "icy/collection.h"
#include "icy/http/message.h"

#include <sstream>
#include <string_view>


namespace icy {
namespace http {


/// HTTP request methods
struct Method
{
    static const std::string Get;
    static const std::string Head;
    static const std::string Put;
    static const std::string Post;
    static const std::string Options;
    static const std::string Delete;
    static const std::string Trace;
    static const std::string Connect;
};


/// This class encapsulates an HTTP request message.
///
/// In addition to the properties common to all HTTP messages,
/// a HTTP request has a method (e.g. GET, HEAD, POST, etc.) and
/// a request URI.
class HTTP_API Request : public http::Message
{
public:
    /// Creates a GET / HTTP/1.1 HTTP request.
    Request();

    /// Creates a GET / HTTP/1.x request with
    /// the given version (HTTP/1.0 or HTTP/1.1).
    Request(const std::string& version);

    /// Creates a HTTP/1.0 request with the given method and URI.
    Request(const std::string& method, const std::string& uri);

    /// Creates a HTTP request with the given method, URI and version.
    Request(const std::string& method, const std::string& uri, const std::string& version);

    /// Destroys the Request.
    virtual ~Request();

    /// Sets the method.
    void setMethod(const std::string& method);

    /// Returns the method.
    [[nodiscard]] const std::string& getMethod() const;

    /// Sets the request URI.
    void setURI(std::string uri);

    /// Appends a fragment to the request URI.
    /// Used by the parser when llhttp splits the URL across callbacks.
    void appendURI(std::string_view uri);

    /// Returns the request URI.
    [[nodiscard]] const std::string& getURI() const;

    /// Sets the value of the Host header field.
    void setHost(const std::string& host);

    /// Sets the value of the Host header field.
    ///
    /// If the given port number is a non-standard
    /// port number (other than 80 or 443), it is
    /// included in the Host header field.
    void setHost(const std::string& host, uint16_t port);

    /// Returns the value of the Host header field.
    ///
    /// Throws a NotFoundException if the request
    /// does not have a Host header field.
    [[nodiscard]] const std::string& getHost() const;

    /// Adds a Cookie header with the names and
    /// values from cookies.
    void setCookies(const NVCollection& cookies);

    /// Fills cookies with the cookies extracted
    /// from the Cookie headers in the request.
    void getCookies(NVCollection& cookies) const;

    /// Returns the request URI parameters.
    void getURIParameters(NVCollection& params) const;

    /// Returns true if the request contains authentication
    /// information in the form of an Authorization header.
    [[nodiscard]] bool hasCredentials() const;

    /// Returns the authentication scheme and additional authentication
    /// information contained in this request.
    ///
    /// Throws a std::exception if no authentication information
    /// is contained in the request.
    void getCredentials(std::string& scheme, std::string& authInfo) const;

    /// Sets the authentication scheme and information for
    /// this request.
    void setCredentials(std::string_view scheme, std::string_view authInfo);

    /// Returns true if the request contains proxy authentication
    /// information in the form of an Proxy-Authorization header.
    [[nodiscard]] bool hasProxyCredentials() const;

    /// Returns the proxy authentication scheme and additional proxy
    /// authentication
    /// information contained in this request.
    ///
    /// Throws a std::exception if no proxy authentication information
    /// is contained in the request.
    void getProxyCredentials(std::string& scheme, std::string& authInfo) const;

    /// Sets the proxy authentication scheme and information for this request.
    void setProxyCredentials(std::string_view scheme,
                             std::string_view authInfo);

    /// Writes the HTTP request to the given output stream.
    virtual void write(std::ostream& ostr) const;

    /// Writes the HTTP request to the given output string.
    virtual void write(std::string& str) const;

    friend std::ostream& operator<<(std::ostream& stream, const Request& req)
    {
        req.write(stream);
        return stream;
    }

protected:
    /// Returns the authentication scheme and additional authentication
    /// information contained in the given header of request.
    ///
    /// Throws a NotAuthenticatedException if no authentication information
    /// is contained in the request.
    void getCredentials(const std::string& header, std::string& scheme, std::string& authInfo) const;

    /// Writes the authentication scheme and information for
    /// this request to the given header.
    void setCredentials(const std::string& header, std::string_view scheme, std::string_view authInfo);

private:
    std::string _method;
    std::string _uri;
};


} // namespace http
} // namespace icy


/// @}
