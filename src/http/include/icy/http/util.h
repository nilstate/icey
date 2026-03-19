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


/// Parses the URI part from a HTTP request.
std::string parseURI(std::string_view request);

/// Parses the URI against the expression.
bool matchURL(std::string_view uri, std::string_view expression);

/// Parses an item from a HTTP cookie.
std::string parseCookieItem(std::string_view cookie, std::string_view item);

/// Splits and adds the URI query parameters into the given collection.
bool splitURIParameters(std::string_view uri, NVCollection& out);

/// Splits the given std::string into a value and a collection of parameters.
/// Parameters are expected to be separated by semicolons.
///
/// Enclosing quotes of parameter values are removed.
///
/// For example, the std::string
///   multipart/mixed; boundary="boundary-01234567"
/// is split into the value
///   multipart/mixed
/// and the parameter
///   boundary -> boundary-01234567
void splitParameters(const std::string& s, std::string& value,
                     NVCollection& parameters);

/// Splits the given std::string into a collection of parameters.
/// Parameters are expected to be separated by semicolons.
///
/// Enclosing quotes of parameter values are removed.
void splitParameters(const std::string::const_iterator& begin,
                     const std::string::const_iterator& end,
                     NVCollection& parameters);


} // namespace http
} // namespace icy


/// @}
