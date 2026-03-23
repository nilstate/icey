///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/request.h"
#include "icy/http/util.h"

#include <stdexcept>
#include <string_view>
#include <utility>


namespace icy {
namespace http {


const std::string Method::Get = "GET";
const std::string Method::Head = "HEAD";
const std::string Method::Put = "PUT";
const std::string Method::Post = "POST";
const std::string Method::Options = "OPTIONS";
const std::string Method::Delete = "DELETE";
const std::string Method::Trace = "TRACE";
const std::string Method::Connect = "CONNECT";


Request::Request()
    : _method(Method::Get)
    , _uri("/")
{
}


Request::Request(const std::string& version)
    : http::Message(version)
    , _method(Method::Get)
    , _uri("/")
{
}


Request::Request(const std::string& method, const std::string& uri)
    : _method(method)
    , _uri(uri)
{
}


Request::Request(const std::string& method, const std::string& uri,
                 const std::string& version)
    : http::Message(version)
    , _method(method)
    , _uri(uri)
{
}


Request::~Request()
{
}


void Request::setMethod(const std::string& method)
{
    _method = method;
}


void Request::setURI(std::string uri)
{
    _uri = std::move(uri);
}


void Request::appendURI(std::string_view uri)
{
    _uri.append(uri.data(), uri.size());
}


void Request::setHost(const std::string& host)
{
    set("Host", host);
}


void Request::setHost(const std::string& host, uint16_t port)
{
    std::string value(host);
    if (port != 80 && port != 443) {
        value.append(":");
        value.append(util::itostr<uint16_t>(port));
    }
    setHost(value);
}


const std::string& Request::getHost() const
{
    return get("Host");
}


const std::string& Request::getMethod() const
{
    return _method;
}


const std::string& Request::getURI() const
{
    return _uri;
}

void Request::setCookies(const NVCollection& cookies)
{
    std::string cookie;
    bool first = true;
    for (const auto& [key, val] : cookies) {
        if (!first)
            cookie.append("; ");
        first = false;
        cookie.append(key);
        cookie.append("=");
        cookie.append(val);
    }
    add("Cookie", cookie);
}


void Request::getCookies(NVCollection& cookies) const
{
    for (const auto& [name, value] : *this) {
        if (util::icompare(name, "Cookie") == 0)
            http::splitParameters(value.begin(), value.end(), cookies);
    }
}


void Request::getURIParameters(NVCollection& params) const
{
    http::splitURIParameters(getURI(), params);
}


bool Request::hasCredentials() const
{
    return has("Authorization");
}


void Request::getCredentials(std::string& scheme, std::string& authInfo) const
{
    getCredentials("Authorization", scheme, authInfo);
}


void Request::setCredentials(std::string_view scheme,
                             std::string_view authInfo)
{
    setCredentials("Authorization", scheme, authInfo);
}


bool Request::hasProxyCredentials() const
{
    return has("Proxy-Authorization");
}


void Request::getProxyCredentials(std::string& scheme,
                                  std::string& authInfo) const
{
    getCredentials("Proxy-Authorization", scheme, authInfo);
}


void Request::setProxyCredentials(std::string_view scheme,
                                  std::string_view authInfo)
{
    setCredentials("Proxy-Authorization", scheme, authInfo);
}


void Request::write(std::ostream& ostr) const
{
    if (_uri.empty()) {
        throw std::runtime_error("Request::write: empty URI");
    }
    ostr << _method << " " << _uri << " " << _version << "\r\n";
    http::Message::write(ostr);
    ostr << "\r\n";
}


void Request::write(std::string& str) const
{
    if (_uri.empty()) {
        throw std::runtime_error("Request::write: empty URI");
    }
    str.append(_method);
    str.append(" ");
    str.append(_uri);
    str.append(" ");
    str.append(_version);
    str.append("\r\n");
    http::Message::write(str);
    str.append("\r\n");
}


void Request::getCredentials(const std::string& header, std::string& scheme,
                             std::string& authInfo) const
{
    scheme.clear();
    authInfo.clear();
    if (has(header)) {
        const std::string& auth = get(header);
        std::string::const_iterator it = auth.begin();
        std::string::const_iterator end = auth.end();
        while (it != end && ::isspace(*it))
            ++it;
        while (it != end && !::isspace(*it))
            scheme += *it++;
        while (it != end && ::isspace(*it))
            ++it;
        while (it != end)
            authInfo += *it++;
    } else
        throw std::runtime_error("Request is not authenticated");
}


void Request::setCredentials(const std::string& header,
                             std::string_view scheme,
                             std::string_view authInfo)
{
    std::string auth(scheme);
    auth.append(" ");
    auth.append(authInfo);
    set(header, auth);
}


} // namespace http
} // namespace icy


/// @}
