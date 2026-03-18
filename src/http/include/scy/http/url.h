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

#include <cstdint>
#include <string>
#include <string_view>


namespace scy {
namespace http {


/// An RFC 3986 based URL parser.
/// Constructors and assignment operators will throw
/// a SyntaxException if the URL is invalid.
class HTTP_API URL
{
public:
    URL();
    URL(const char* url);
    URL(const std::string& url);
    URL(const std::string& scheme, const std::string& authority);
    URL(const std::string& scheme, const std::string& authority,
        const std::string& pathEtc);
    URL(const std::string& scheme, const std::string& authority,
        const std::string& path, const std::string& query,
        const std::string& fragment = "");
    URL(const URL&) = default;
    ~URL();

    URL& operator=(const URL& uri);
    URL& operator=(const char* uri);
    URL& operator=(const std::string& uri);

    /// Parses and assigns an URI from the given std::string.
    /// Throws a SyntaxException if whiny is set and the
    /// given url is invalid.
    bool parse(std::string_view url, bool whiny = true);

    /// RFC 3986 based URL encoding based on JavaScript's
    /// encodeURIComponent()
    static std::string encode(std::string_view str);

    /// RFC 3986 based URL decoding based on JavaScript's
    /// decodeURIComponent()
    static std::string decode(std::string_view str);

public:
    [[nodiscard]] std::string scheme() const;
    [[nodiscard]] std::string userInfo() const;
    [[nodiscard]] std::string host() const;
    [[nodiscard]] uint16_t port() const;
    [[nodiscard]] std::string authority() const;
    [[nodiscard]] std::string path() const;
    [[nodiscard]] std::string pathEtc() const;
    [[nodiscard]] std::string query() const;
    [[nodiscard]] std::string fragment() const;

    [[nodiscard]] bool hasSchema() const;
    [[nodiscard]] bool hasUserInfo() const;
    [[nodiscard]] bool hasHost() const;
    [[nodiscard]] bool hasPort() const;
    [[nodiscard]] bool hasPath() const;
    [[nodiscard]] bool hasQuery() const;
    [[nodiscard]] bool hasFragment() const;

    [[nodiscard]] bool valid() const;

    [[nodiscard]] std::string str() const;

    friend std::ostream& operator<<(std::ostream& stream, const URL& url)
    {
        stream << url.str();
        return stream;
    }

protected:
    std::string _buf;
    std::string _scheme;
    std::string _userInfo;
    std::string _host;
    uint16_t _port;
    std::string _path;
    std::string _query;
    std::string _fragment;
    bool _hasPort;
};


} // namespace http
} // namespace scy


/// @}
