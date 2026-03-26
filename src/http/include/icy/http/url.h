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

#include "icy/collection.h"
#include "icy/http/http.h"

#include <cstdint>
#include <string>
#include <string_view>


namespace icy {
namespace http {


/// An RFC 3986 based URL parser.
/// Constructors and assignment operators will throw
/// a SyntaxException if the URL is invalid.
class HTTP_API URL
{
public:
    /// Creates an empty URL.
    URL();

    /// Parses the URL from a null-terminated string.
    /// @param url Null-terminated URL string to parse.
    URL(const char* url);

    /// Parses the URL from a std::string.
    /// @param url URL string to parse.
    URL(const std::string& url);

    /// Constructs a URL from scheme and authority components.
    /// @param scheme URL scheme (e.g. "http", "https").
    /// @param authority Host and optional port (e.g. "example.com:8080").
    URL(const std::string& scheme, const std::string& authority);

    /// Constructs a URL from scheme, authority, and path+query+fragment.
    /// @param scheme URL scheme (e.g. "http").
    /// @param authority Host and optional port.
    /// @param pathEtc Path, query and fragment combined (e.g. "/path?q=1#frag").
    URL(const std::string& scheme, const std::string& authority,
        const std::string& pathEtc);

    /// Constructs a URL from individual components.
    /// @param scheme URL scheme (e.g. "http").
    /// @param authority Host and optional port.
    /// @param path URL path (e.g. "/index.html").
    /// @param query Query string without leading '?' (e.g. "key=value").
    /// @param fragment Fragment identifier without leading '#'.
    URL(const std::string& scheme, const std::string& authority,
        const std::string& path, const std::string& query,
        const std::string& fragment = "");

    URL(const URL&) = default;
    ~URL();

    /// Assigns a URL from another URL instance.
    /// @param uri Source URL to copy from.
    /// @return Reference to this URL.
    URL& operator=(const URL& uri);

    /// Assigns a URL from a null-terminated string.
    /// @param uri Null-terminated URL string to parse.
    /// @return Reference to this URL.
    URL& operator=(const char* uri);

    /// Assigns a URL from a std::string.
    /// @param uri URL string to parse.
    /// @return Reference to this URL.
    URL& operator=(const std::string& uri);

    /// Parses and assigns a URL from the given string view, resetting all components first.
    /// @param url URL string to parse.
    /// @param whiny If true, throws std::runtime_error on an invalid URL; otherwise returns false.
    /// @return true if the URL was parsed successfully; false if invalid and whiny is false.
    bool parse(std::string_view url, bool whiny = true);

    /// Percent-encodes a string per RFC 3986, preserving unreserved characters
    /// (A-Z, a-z, 0-9, '-', '_', '.', '~'). Equivalent to JavaScript's encodeURIComponent().
    /// @param str Input string to encode.
    /// @return Percent-encoded string.
    static std::string encode(std::string_view str);

    /// Decodes a percent-encoded string per RFC 3986.
    /// Equivalent to JavaScript's decodeURIComponent().
    /// @param str Percent-encoded input string.
    /// @return Decoded string.
    static std::string decode(std::string_view str);

public:
    /// Returns the URL scheme (e.g. "http", "https", "ws"). Always lowercase.
    [[nodiscard]] std::string scheme() const;

    /// Returns the user info component (e.g. "user:pass" from "http://user:pass@host/").
    /// Returns an empty string if not present.
    [[nodiscard]] std::string userInfo() const;

    /// Returns the host component (e.g. "example.com").
    /// Returns an empty string if not present.
    [[nodiscard]] std::string host() const;

    /// Returns the port number.
    /// If no explicit port was in the URL, returns the default port for the scheme
    /// (80 for http, 443 for https), or 0 if the scheme is unknown.
    [[nodiscard]] uint16_t port() const;

    /// Returns the authority component (`userinfo@host:port`).
    /// Only includes components that are present.
    [[nodiscard]] std::string authority() const;

    /// Returns the path component (e.g. "/index.html").
    /// Returns an empty string if not present.
    [[nodiscard]] std::string path() const;

    /// Returns the path, query and fragment combined (e.g. "/path?q=1#frag").
    [[nodiscard]] std::string pathEtc() const;

    /// Returns the query string without the leading '?' (e.g. "key=value&foo=bar").
    /// Returns an empty string if not present.
    [[nodiscard]] std::string query() const;

    /// Returns the fragment identifier without the leading '#'.
    /// Returns an empty string if not present.
    [[nodiscard]] std::string fragment() const;

    /// Returns true if the URL has a scheme component.
    [[nodiscard]] bool hasSchema() const;

    /// Returns true if the URL has a user info component.
    [[nodiscard]] bool hasUserInfo() const;

    /// Returns true if the URL has a host component.
    [[nodiscard]] bool hasHost() const;

    /// Returns true if an explicit port was specified in the URL.
    [[nodiscard]] bool hasPort() const;

    /// Returns true if the URL has a path component.
    [[nodiscard]] bool hasPath() const;

    /// Returns true if the URL has a query component.
    [[nodiscard]] bool hasQuery() const;

    /// Returns true if the URL has a fragment component.
    [[nodiscard]] bool hasFragment() const;

    /// Returns true if the URL is non-empty and was successfully parsed.
    [[nodiscard]] bool valid() const;

    /// Returns the original URL string as parsed.
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
} // namespace icy


/// @}
