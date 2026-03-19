///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "icy/http/util.h"
#include "icy/util.h"

#include <string_view>


// using std::endl;


namespace icy {
namespace http {


std::string parseURI(std::string_view request)
{
    std::string req(request);
    std::string value = "";
    std::string::size_type start, end = 0;
    util::toUpper(req);
    start = req.find(" ");
    if (start != std::string::npos) {
        start++;
        end = req.find(" HTTPS", start);
        if (end == std::string::npos)
            end = req.find(" HTTP", start);
        if (end == std::string::npos)
            end = req.find(" RTSP", start);
        if (end == std::string::npos)
            return "";
        value = std::string(request.substr(start, end - start));
    }
    return value;
}


bool matchURL(std::string_view uri, std::string_view expression)
{
    auto index = uri.find('?');
    return util::matchNodes(uri.substr(0, index), expression, "/");
}


std::string parseCookieItem(std::string_view cookie, std::string_view item)
{
    std::string needle(item);
    needle += '=';
    auto start = cookie.find(needle);
    if (start != std::string_view::npos) {
        start += item.size() + 1;
        auto end = cookie.find(';', start);
        return std::string(cookie.substr(start, end - start));
    }
    return "";
}


bool splitURIParameters(std::string_view uri, NVCollection& out)
{
    size_t len = uri.length();
    size_t i = 0;

    // Parse REST parameters
    while (i < len && uri[i] != '?') {
        i++;

        std::string value = "";
        while (i < len && uri[i] != '/' && uri[i] != '?')
            value += uri[i++];

        // REST parameters are referenced by index
        if (!value.empty())
            out.set(util::itostr(out.size()), value);
    }

    // Parse query parameters
    if (i < len && uri[i] == '?')
        i++;
    while (i < len) {
        std::string name = "";
        while (i < len && uri[i] != '=')
            name += uri[i++];
        i++;
        std::string value = "";
        while (i < len && uri[i] != '&')
            value += uri[i++];
        i++;

        if (!name.empty() && !value.empty())
            out.set(name, value);
    }

    return out.size() > 0;
}


void splitParameters(const std::string& s, std::string& value,
                     NVCollection& parameters)
{
    value.clear();
    parameters.clear();

    std::string::const_iterator it = s.begin();
    std::string::const_iterator end = s.end();
    while (it != end && ::isspace(*it))
        ++it;
    while (it != end && *it != ';')
        value += *it++;
    util::trimRightInPlace(value);
    if (it != end)
        ++it;

    splitParameters(it, end, parameters);
}


void splitParameters(const std::string::const_iterator& begin,
                     const std::string::const_iterator& end,
                     NVCollection& parameters)
{
    std::string pname;
    std::string pvalue;
    pname.reserve(32);
    pvalue.reserve(64);
    std::string::const_iterator it = begin;
    while (it != end) {
        pname.clear();
        pvalue.clear();
        while (it != end && ::isspace(*it))
            ++it;
        while (it != end && *it != '=' && *it != ';')
            pname += *it++;
        util::trimRightInPlace(pname);
        if (it != end && *it != ';')
            ++it;
        while (it != end && ::isspace(*it))
            ++it;
        while (it != end && *it != ';') {
            if (*it == '"') {
                ++it;
                while (it != end && *it != '"') {
                    if (*it == '\\') {
                        ++it;
                        if (it != end)
                            pvalue += *it++;
                    } else
                        pvalue += *it++;
                }
                if (it != end)
                    ++it;
            } else if (*it == '\\') {
                ++it;
                if (it != end)
                    pvalue += *it++;
            } else
                pvalue += *it++;
        }
        util::trimRightInPlace(pvalue);
        if (!pname.empty())
            parameters.add(pname, pvalue);
        if (it != end)
            ++it;
    }
}


} // namespace http
} // namespace icy


/// @\}
