///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/url.h"
#include "icy/util.h"

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <string_view>


namespace icy {
namespace http {


URL::URL()
    : _port(0)
    , _hasPort(false)
{
    parse("");
}


URL::URL(const char* url)
    : _port(0)
    , _hasPort(false)
{
    parse(url);
}


URL::URL(const std::string& url)
    : _port(0)
    , _hasPort(false)
{
    parse(url);
}


URL::URL(const std::string& scheme, const std::string& authority)
    : _port(0)
    , _hasPort(false)
{
    parse(scheme + "://" + authority);
}


URL::URL(const std::string& scheme, const std::string& authority,
         const std::string& pathEtc)
    : _port(0)
    , _hasPort(false)
{
    parse(scheme + "://" + authority + pathEtc);
}


URL::URL(const std::string& scheme, const std::string& authority,
         const std::string& path, const std::string& query,
         const std::string& fragment)
    : _port(0)
    , _hasPort(false)
{
    parse(scheme + "://" + authority + path + "?" + query + "#" + fragment);
}


URL::~URL()
{
}


URL& URL::operator=(const URL& uri)
{
    if (&uri != this)
        parse(uri.str());
    return *this;
}


URL& URL::operator=(const char* uri)
{
    parse(uri);
    return *this;
}


URL& URL::operator=(const std::string& uri)
{
    parse(uri);
    return *this;
}


bool URL::parse(std::string_view url, bool whiny)
{
    LTrace("Parsing: ", url);
    std::string src(url);
    util::trimInPlace(src);
    _buf = src;
    _scheme.clear();
    _userInfo.clear();
    _host.clear();
    _port = 0;
    _hasPort = false;
    _path.clear();
    _query.clear();
    _fragment.clear();

    if (src.empty())
        return false;

    std::string::size_type pos = 0;

    // Extract scheme
    auto schemeEnd = src.find("://", pos);
    if (schemeEnd != std::string::npos) {
        _scheme = src.substr(pos, schemeEnd - pos);
        util::toLowerInPlace(_scheme);
        pos = schemeEnd + 3;
    }

    // Extract fragment (from the end)
    auto fragPos = src.find('#', pos);
    std::string remaining;
    if (fragPos != std::string::npos) {
        _fragment = src.substr(fragPos + 1);
        remaining = src.substr(pos, fragPos - pos);
    } else {
        remaining = src.substr(pos);
    }

    // Extract query (from the end of remaining)
    auto queryPos = remaining.find('?');
    std::string authorityAndPath;
    if (queryPos != std::string::npos) {
        _query = remaining.substr(queryPos + 1);
        authorityAndPath = remaining.substr(0, queryPos);
    } else {
        authorityAndPath = remaining;
    }

    // If we had a scheme, extract authority
    if (!_scheme.empty()) {
        auto pathStart = authorityAndPath.find('/');
        std::string authority;
        if (pathStart != std::string::npos) {
            authority = authorityAndPath.substr(0, pathStart);
            _path = authorityAndPath.substr(pathStart);
        } else {
            authority = authorityAndPath;
        }

        // Extract userinfo from authority
        auto atPos = authority.find('@');
        std::string hostPort;
        if (atPos != std::string::npos) {
            _userInfo = authority.substr(0, atPos);
            hostPort = authority.substr(atPos + 1);
        } else {
            hostPort = authority;
        }

        // Extract host and port (handle IPv6 brackets)
        if (!hostPort.empty() && hostPort[0] == '[') {
            auto bracketEnd = hostPort.find(']');
            if (bracketEnd != std::string::npos) {
                _host = hostPort.substr(1, bracketEnd - 1);
                if (bracketEnd + 1 < hostPort.size() && hostPort[bracketEnd + 1] == ':') {
                    auto portStr = hostPort.substr(bracketEnd + 2);
                    _port = static_cast<uint16_t>(std::stoi(portStr));
                    _hasPort = true;
                }
            }
        } else {
            auto colonPos = hostPort.rfind(':');
            if (colonPos != std::string::npos) {
                _host = hostPort.substr(0, colonPos);
                auto portStr = hostPort.substr(colonPos + 1);
                if (!portStr.empty() && std::all_of(portStr.begin(), portStr.end(), ::isdigit)) {
                    _port = static_cast<uint16_t>(std::stoi(portStr));
                    _hasPort = true;
                }
            } else {
                _host = hostPort;
            }
        }
    } else {
        // No scheme - treat entire string as path
        _path = authorityAndPath;
    }

    if (_buf.empty()) {
        if (whiny)
            throw std::runtime_error("Syntax error: Cannot parse invalid URL: " + src);
        return false;
    }

    return true;
}


std::string URL::scheme() const
{
    return _scheme;
}


std::string URL::host() const
{
    return _host;
}


uint16_t URL::port() const
{
    if (_hasPort)
        return _port;
    if (_scheme == "http")
        return 80;
    else if (_scheme == "https")
        return 443;
    return 0;
}


std::string URL::authority() const
{
    std::string res;
    if (hasUserInfo()) {
        res.append(userInfo());
        res.append("@");
    }
    res.append(host());
    if (hasPort()) {
        res.append(":");
        res.append(util::itostr<uint16_t>(_port));
    }
    return res;
}


std::string URL::pathEtc() const
{
    std::string res;
    res.append(path());
    if (hasQuery()) {
        res.append("?");
        res.append(query());
    }
    if (hasFragment()) {
        res.append("#");
        res.append(fragment());
    }
    return res;
}


std::string URL::path() const
{
    return _path;
}


std::string URL::query() const
{
    return _query;
}


std::string URL::fragment() const
{
    return _fragment;
}


std::string URL::userInfo() const
{
    return _userInfo;
}


bool URL::valid() const
{
    return !_buf.empty();
}


std::string URL::str() const
{
    return _buf;
}


bool URL::hasSchema() const
{
    return !_scheme.empty();
}


bool URL::hasHost() const
{
    return !_host.empty();
}


bool URL::hasPort() const
{
    return _hasPort;
}


bool URL::hasPath() const
{
    return !_path.empty();
}


bool URL::hasQuery() const
{
    return !_query.empty();
}


bool URL::hasFragment() const
{
    return !_fragment.empty();
}


bool URL::hasUserInfo() const
{
    return !_userInfo.empty();
}


std::string URL::encode(std::string_view str)
{
    const std::string_view unreserved =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

    std::string escaped;
    for (size_t i = 0; i < str.length(); i++) {
        if (unreserved.find(str[i]) != std::string_view::npos) {
            escaped.push_back(str[i]);
        } else {
            escaped.append("%");
            char buf[4];
            std::snprintf(buf, sizeof(buf), "%.2X", static_cast<unsigned char>(str[i]));
            escaped.append(buf);
        }
    }
    return escaped;
}


std::string URL::decode(std::string_view str)
{
    std::string clean;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            const std::string_view digits = "0123456789ABCDEF";
            clean +=
                static_cast<char>(digits.find(str[i + 1]) * 16 + digits.find(str[i + 2]));
            i += 2;
        } else {
            clean += str[i];
        }
    }
    return clean;
}


} // namespace http
} // namespace icy


/// @\}
