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

#include <string_view>


namespace icy {
namespace http {


/// Extracts the URI (path and query) from a raw HTTP request line.
/// @param request Raw HTTP request line (e.g. "GET /path?q=1 HTTP/1.1").
/// @return The URI portion (e.g. "/path?q=1").
std::string parseURI(std::string_view request);

/// Tests whether a URI matches a glob-style expression.
/// @param uri The URI to test.
/// @param expression Pattern to match against. '*' matches any sequence of characters.
/// @return true if the URI matches the expression.
bool matchURL(std::string_view uri, std::string_view expression);

/// Extracts a named attribute from a Cookie header value.
/// @param cookie Full Cookie header value (e.g. "name=value; Path=/; HttpOnly").
/// @param item Attribute name to find (e.g. "Path").
/// @return The value of the named attribute, or an empty string if not found.
std::string parseCookieItem(std::string_view cookie, std::string_view item);

/// Parses URL query parameters from a URI into key-value pairs.
/// Handles percent-decoding of names and values.
/// @param uri URI string optionally containing a '?' query component.
/// @param out Collection to populate with parsed parameters.
/// @return true if at least one parameter was parsed; false otherwise.
bool splitURIParameters(std::string_view uri, NVCollection& out);

/// Splits a header-style parameter string into a primary value and named attributes.
/// Attributes are separated by ';'. Enclosing quotes are stripped from values.
///
/// Example input: "multipart/mixed; boundary=\"boundary-01234567\""
/// Output value: "multipart/mixed", parameters: { "boundary" -> "boundary-01234567" }
///
/// @param s Input string to split.
/// @param value Receives the primary value before the first ';'.
/// @param parameters Receives the parsed attribute key-value pairs.
void splitParameters(const std::string& s, std::string& value,
                     NVCollection& parameters);

/// Splits a substring (defined by iterators) into named attributes.
/// Attributes are separated by ';'. Enclosing quotes are stripped from values.
/// @param begin Iterator to the start of the string to parse.
/// @param end Iterator past the end of the string to parse.
/// @param parameters Receives the parsed attribute key-value pairs.
void splitParameters(const std::string::const_iterator& begin,
                     const std::string::const_iterator& end,
                     NVCollection& parameters);


} // namespace http
} // namespace icy


/// @}
