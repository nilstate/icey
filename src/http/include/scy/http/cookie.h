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

#include "scy/collection.h"
#include "scy/http/http.h"
#include "scy/net/net.h"


namespace scy {
namespace http {


/// @ingroup http
/// This class represents a HTTP Cookie.
///
/// A cookie is a small amount of information sent by a Web
/// server to a Web browser, saved by the browser, and later sent back
/// to the server. A cookie's value can uniquely identify a client, so
/// cookies are commonly used for session management.
///
/// A cookie has a name, a single value, and optional attributes such
/// as a comment, path and domain qualifiers, a maximum age, and a
/// version number.
///
/// This class supports both the Version 0 (by Netscape) and Version 1
/// (by RFC 2109) cookie specifications. By default, cookies are created
/// using Version 0 to ensure the best interoperability.
class HTTP_API Cookie
{
public:
    /// Creates an empty Cookie.
    Cookie();

    /// Creates a cookie with the given name.
    /// The cookie never expires.
    explicit Cookie(const std::string& name);

    /// Creates a cookie from the given NVCollection.
    explicit Cookie(const NVCollection& nvc);

    /// Creates a cookie with the given name and value.
    /// The cookie never expires.
    ///
    /// Note: If value contains whitespace or non-alphanumeric
    /// characters, the value should be escaped by calling escape()
    /// before passing it to the constructor.
    Cookie(const std::string& name, const std::string& value);

    /// Creates the Cookie by copying another one.
    Cookie(const Cookie& cookie);

    /// Destroys the Cookie.
    ~Cookie();

    /// Assigns a cookie.
    Cookie& operator=(const Cookie& cookie);

    /// Sets the version of the cookie.
    ///
    /// Version must be either 0 (denoting a Netscape cookie)
    /// or 1 (denoting a RFC 2109 cookie).
    void setVersion(int version);

    /// Returns the version of the cookie, which is
    /// either 0 or 1.
    [[nodiscard]] int getVersion() const;

    /// Sets the name of the cookie.
    void setName(const std::string& name);

    /// Returns the name of the cookie.
    [[nodiscard]] const std::string& getName() const;

    /// Sets the value of the cookie.
    ///
    /// According to the cookie specification, the
    /// size of the value should not exceed 4 Kbytes.
    ///
    /// Note: If value contains whitespace or non-alphanumeric
    /// characters, the value should be escaped by calling escape()
    /// prior to passing it to setName().
    void setValue(const std::string& value);

    /// Returns the value of the cookie.
    [[nodiscard]] const std::string& getValue() const;

    /// Sets the comment for the cookie.
    ///
    /// Comments are only supported for version 1 cookies.
    void setComment(const std::string& comment);

    /// Returns the comment for the cookie.
    [[nodiscard]] const std::string& getComment() const;

    /// Sets the domain for the cookie.
    void setDomain(const std::string& domain);

    /// Returns the domain for the cookie.
    [[nodiscard]] const std::string& getDomain() const;

    /// Sets the path for the cookie.
    void setPath(const std::string& path);

    /// Returns the path for the cookie.
    [[nodiscard]] const std::string& getPath() const;

    /// Sets the value of the secure flag for
    /// the cookie.
    void setSecure(bool secure);

    /// Returns the value of the secure flag
    /// for the cookie.
    [[nodiscard]] bool getSecure() const;

    /// Sets the maximum age in seconds for
    /// the cookie.
    ///
    /// A value of -1 causes the cookie to
    /// never expire on the client.
    ///
    /// A value of 0 deletes the cookie on
    /// the client.
    void setMaxAge(int maxAge);

    /// Returns the maximum age in seconds for
    /// the cookie.
    [[nodiscard]] int getMaxAge() const;

    /// Sets the HttpOnly flag for the cookie.
    void setHttpOnly(bool flag = true);

    /// Returns true if the cookie's HttpOnly flag is set.
    [[nodiscard]] bool getHttpOnly() const;

    /// Returns a std::string representation of the cookie,
    /// suitable for use in a Set-Cookie header.
    [[nodiscard]] std::string toString() const;

    /// Escapes the given std::string by replacing all
    /// non-alphanumeric characters with escape
    /// sequences in the form %xx, where xx is the
    /// hexadecimal character code.
    static std::string escape(const std::string& str);

    /// Unescapes the given std::string by replacing all
    /// escape sequences in the form %xx with the
    /// respective characters.
    static std::string unescape(const std::string& str);

private:
    int _version;
    std::string _name;
    std::string _value;
    std::string _comment;
    std::string _domain;
    std::string _path;
    bool _secure;
    int _maxAge;
    bool _httpOnly;
};


inline int Cookie::getVersion() const
{
    return _version;
}


inline const std::string& Cookie::getName() const
{
    return _name;
}


inline const std::string& Cookie::getValue() const
{
    return _value;
}


inline const std::string& Cookie::getComment() const
{
    return _comment;
}


inline const std::string& Cookie::getDomain() const
{
    return _domain;
}


inline const std::string& Cookie::getPath() const
{
    return _path;
}


inline bool Cookie::getSecure() const
{
    return _secure;
}


inline int Cookie::getMaxAge() const
{
    return _maxAge;
}


inline bool Cookie::getHttpOnly() const
{
    return _httpOnly;
}


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
