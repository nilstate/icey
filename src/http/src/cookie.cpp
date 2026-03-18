///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/cookie.h"
#include "scy/datetime.h"
#include "scy/http/url.h"


namespace scy {
namespace http {


Cookie::Cookie()
    : _version(0)
    , _secure(false)
    , _maxAge(-1)
    , _httpOnly(false)
{
}


Cookie::Cookie(const std::string& name)
    : _version(0)
    , _name(name)
    , _secure(false)
    , _maxAge(-1)
    , _httpOnly(false)
{
}


Cookie::Cookie(const NVCollection& nvc)
    : _version(0)
    , _secure(false)
    , _maxAge(-1)
    , _httpOnly(false)
{
    for (const auto& [name, value] : nvc) {
        if (util::icompare(name, "comment") == 0) {
            setComment(value);
        } else if (util::icompare(name, "domain") == 0) {
            setDomain(value);
        } else if (util::icompare(name, "path") == 0) {
            setPath(value);
        } else if (util::icompare(name, "max-age") == 0) {
            setMaxAge(util::strtoi<int>(value));
        } else if (util::icompare(name, "secure") == 0) {
            setSecure(true);
        } else if (util::icompare(name, "expires") == 0) {
            int tzd;
            DateTime exp = DateTimeParser::parse(value, tzd);
            Timestamp now;
            setMaxAge(static_cast<int>((exp.timestamp() - now) / Timestamp::resolution()));
        } else if (util::icompare(name, "version") == 0) {
            setVersion(util::strtoi<int>(value));
        } else if (util::icompare(name, "HttpOnly") == 0) {
            setHttpOnly(true);
        } else {
            setName(name);
            setValue(value);
        }
    }
}


Cookie::Cookie(const std::string& name, const std::string& value)
    : _version(0)
    , _name(name)
    , _value(value)
    , _secure(false)
    , _maxAge(-1)
    , _httpOnly(false)
{
}


Cookie::Cookie(const Cookie& cookie)
    : _version(cookie._version)
    , _name(cookie._name)
    , _value(cookie._value)
    , _comment(cookie._comment)
    , _domain(cookie._domain)
    , _path(cookie._path)
    , _secure(cookie._secure)
    , _maxAge(cookie._maxAge)
    , _httpOnly(cookie._httpOnly)
{
}


Cookie::~Cookie()
{
}


Cookie& Cookie::operator=(const Cookie& cookie)
{
    if (&cookie != this) {
        _version = cookie._version;
        _name = cookie._name;
        _value = cookie._value;
        _comment = cookie._comment;
        _domain = cookie._domain;
        _path = cookie._path;
        _secure = cookie._secure;
        _maxAge = cookie._maxAge;
        _httpOnly = cookie._httpOnly;
    }
    return *this;
}


void Cookie::setVersion(int version)
{
    _version = version;
}


void Cookie::setName(const std::string& name)
{
    _name = name;
}


void Cookie::setValue(const std::string& value)
{
    _value = value;
}


void Cookie::setComment(const std::string& comment)
{
    _comment = comment;
}


void Cookie::setDomain(const std::string& domain)
{
    _domain = domain;
}


void Cookie::setPath(const std::string& path)
{
    _path = path;
}


void Cookie::setSecure(bool secure)
{
    _secure = secure;
}


void Cookie::setMaxAge(int maxAge)
{
    _maxAge = maxAge;
}


void Cookie::setHttpOnly(bool flag)
{
    _httpOnly = flag;
}


std::string Cookie::toString() const
{
    std::string res;
    res.reserve(256);
    res.append(_name);
    res.append("=");
    if (_version == 0) {
        /// Netscape cookie
        res.append(_value);
        if (!_domain.empty()) {
            res.append("; domain=");
            res.append(_domain);
        }
        if (!_path.empty()) {
            res.append("; path=");
            res.append(_path);
        }
        if (_maxAge >= 0) {
            Timestamp ts;
            ts += _maxAge * Timestamp::resolution();
            res.append("; expires=");
            DateTimeFormatter::append(res, ts, DateTimeFormat::HTTP_FORMAT);
        }
        if (_secure) {
            res.append("; secure");
        }
        if (_httpOnly) {
            res.append("; HttpOnly");
        }
    } else {
        /// RFC 2109 cookie
        res.append("\"");
        res.append(_value);
        res.append("\"");
        if (!_comment.empty()) {
            res.append("; Comment=\"");
            res.append(_comment);
            res.append("\"");
        }
        if (!_domain.empty()) {
            res.append("; Domain=\"");
            res.append(_domain);
            res.append("\"");
        }
        if (!_path.empty()) {
            res.append("; Path=\"");
            res.append(_path);
            res.append("\"");
        }
        if (_maxAge >= 0) {
            res.append("; Max-Age=\"");
            res.append(util::itostr(_maxAge));
            res.append("\"");
        }
        if (_secure) {
            res.append("; secure");
        }
        if (_httpOnly) {
            res.append("; HttpOnly");
        }
        res.append("; Version=\"1\"");
    }
    return res;
}


std::string Cookie::escape(std::string_view str)
{
    return http::URL::encode(str);
}


std::string Cookie::unescape(std::string_view str)
{
    return http::URL::decode(str);
}


} // namespace http
} // namespace scy


/// @\}
