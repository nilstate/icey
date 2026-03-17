///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/util.h"
#include "scy/base64.h"
#include "scy/random.h"

#include <memory>

#include <algorithm>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>


using std::endl;


namespace scy {
namespace util {


std::string string_vprintf(const char* fmt, va_list args)
{
    size_t size = 500;
    char* buf = static_cast<char*>(malloc(size));
    // Grow the buffer size until the output is no longer truncated
    while (true) {
        va_list args_copy;
#if defined(SCY_WIN)
        args_copy = args;
        size_t nwritten = _vsnprintf(buf, size - 1, fmt, args_copy);
#else
        va_copy(args_copy, args);
        size_t nwritten = vsnprintf(buf, size - 1, fmt, args_copy);
#endif
        // Some c libraries return -1 for overflow,
        // some return a number larger than size-1
        if (nwritten < size - 2) {
            buf[nwritten + 1] = 0;
            std::string ret(buf);
            free(buf);
            return ret;
        }
        size *= 2;
        buf = static_cast<char*>(realloc(buf, size));
    }
}


std::string format(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string ret = string_vprintf(fmt, args);
    va_end(args);
    return ret;
}


bool isNumber(std::string_view str)
{
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.length(); i++) {
        if (!::isdigit(static_cast<unsigned char>(str[i])))
            return false;
    }
    return true;
}


bool tryParseHex(const std::string& str, unsigned& value)
{
    char temp;
    return std::sscanf(str.c_str(), "%x%c", &value, &temp) == 1;
}


unsigned parseHex(const std::string& str)
{
    unsigned result;
    if (tryParseHex(str, result))
        return result;
    else
        throw std::runtime_error(
            "Syntax error: Not a valid hexadecimal integer: " + str);
}


std::string memAddress(const void* ptr)
{
    return itostr<const void*>(ptr);
}


std::string randomBinaryString(int size, bool doBase64)
{
    std::string res;
    Random rnd;
    rnd.seed();
    for (int i = 0; i < size; ++i)
        res.push_back(rnd.nextChar());

    if (doBase64) {
        std::string out;
        base64::Encoder enc;
        enc.encode(res, out);
        res = out;
    }
    return res;
}


std::string randomString(int size)
{
    return randomBinaryString(size, true).substr(0, size);
}


uint32_t randomNumber()
{
    Random rnd;
    rnd.seed();
    return rnd.next();
}


void split(const std::string& s, const std::string& delim,
           std::vector<std::string>& elems, int limit)
{
    std::string::size_type prev = 0, pos = 0;
    while ((pos = s.find(delim, prev)) != std::string::npos) {
        if (limit > 0 && static_cast<int>(elems.size()) == limit - 1) {
            // Last allowed element: take everything from prev to end
            break;
        }
        elems.push_back(s.substr(prev, pos - prev));
        prev = pos + delim.size();
    }
    // Remaining portion (or entire string if no delimiter found)
    if (prev <= s.size())
        elems.push_back(s.substr(prev));
}


std::vector<std::string> split(const std::string& s, const std::string& delim,
                               int limit)
{
    std::vector<std::string> elems;
    split(s, delim, elems, limit);
    return elems;
}


void split(const std::string& s, char delim, std::vector<std::string>& elems,
           int limit)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
        if (limit && static_cast<int>(elems.size() + 1) == limit)
            break;
    }
    if (ss.tellg() > 0)
        elems.push_back(
            ss.str().substr(static_cast<unsigned int>(ss.tellg()),
                            static_cast<unsigned int>(s.size() - ss.tellg())));
}


std::vector<std::string> split(const std::string& s, char delim, int limit)
{
    std::vector<std::string> elems;
    split(s, delim, elems, limit);
    return elems;
}


bool endsWith(std::string_view str, std::string_view suffix)
{
    if (suffix.size() > str.size())
        return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


std::string dumpbin(const char* data, size_t len)
{
    std::string output;
    for (size_t i = 0; i < len; i++) {
        char byte = data[i];
        for (size_t mask = 0x80; mask > 0; mask >>= 1) {
            output.push_back(byte & mask ? '1' : '0');
        }
        if (i % 4 == 3)
            output.push_back('\n');
        else
            output.push_back(' ');
    }
    return output;
}


bool compareVersion(const std::string& l, const std::string& r)
{
    if (l.empty())
        return false;
    if (r.empty())
        return true;

    bool equal = true;
    std::vector<std::string> lnums, rnums;
    util::split(l, ".", lnums);
    util::split(r, ".", rnums);
    for (unsigned i = 0; i < lnums.size(); i++) {
        if (rnums.size() < i + 1)
            break;
        int ln = util::strtoi<int>(lnums[i]);
        int rn = util::strtoi<int>(rnums[i]);
        if (ln < rn)
            return false;
        else if (ln > rn)
            equal = false;
    }
    return !equal;
}


void removeSpecialCharacters(std::string& str, bool allowSpaces)
{
    for (size_t i = 0; i < str.length(); ++i)
        if (!::isalnum(str[i]) && (!allowSpaces || !::isspace(str[i])) &&
            str[i] != '.')
            str.erase(i, 1);
}


void replaceSpecialCharacters(std::string& str, char with, bool allowSpaces)
{
    for (size_t i = 0; i < str.length(); ++i)
        if (!::isalnum(str[i]) && (!allowSpaces || !::isspace(str[i])) &&
            str[i] != '.')
            str[i] = with;
}


void toUnderscore(std::string& str)
{
    replaceSpecialCharacters(str, '_', false);
    toLower(str);
}


bool matchNodes(const std::string& node, const std::string& xnode,
                const std::string& delim)
{
    if (xnode == "*")
        return true;
    std::vector<std::string> params = util::split(node, delim);
    std::vector<std::string> xparams = util::split(xnode, delim);
    return matchNodes(params, xparams);
}


bool matchNodes(const std::vector<std::string>& params,
                const std::vector<std::string>& xparams)
{
    // xparams is a simple matcher pattern with nodes and
    // * as wildcard.
    // No match if xparams are greater than the params.
    if (xparams.size() > params.size())
        return false;

    // If params is longer the last xparam the last xparam
    // must be a *.
    if (params.size() > xparams.size() && xparams[xparams.size() - 1] != "*")
        return false;

    for (size_t i = 0; i < xparams.size(); ++i) {

        // Wildcard * matches anything.
        if (xparams[i] == "*")
            continue;

        if (xparams[i] != params[i])
            return false;
    }

    return true;
}


std::streamsize copyStream(std::istream& istr, std::ostream& ostr,
                           size_t bufferSize)
{
    if (bufferSize <= 0)
        throw std::invalid_argument("Buffer size must be positive");

    auto buffer = std::make_unique<char[]>(bufferSize);
    std::streamsize len = 0;
    istr.read(buffer.get(), bufferSize);
    std::streamsize n = istr.gcount();
    while (n > 0) {
        len += n;
        ostr.write(buffer.get(), n);
        if (istr && ostr) {
            istr.read(buffer.get(), bufferSize);
            n = istr.gcount();
        } else
            n = 0;
    }
    return len;
}


std::streamsize copyStreamUnbuffered(std::istream& istr, std::ostream& ostr)
{
    char c;
    std::streamsize len = 0;
    istr.get(c);
    while (istr && ostr) {
        ++len;
        ostr.put(c);
        istr.get(c);
    }
    return len;
}


std::streamsize copyToString(std::istream& istr, std::string& str,
                             size_t bufferSize)
{
    if (bufferSize <= 0)
        throw std::invalid_argument("Buffer size must be positive");

    auto buffer = std::make_unique<char[]>(bufferSize);
    std::streamsize len = 0;
    istr.read(buffer.get(), bufferSize);
    std::streamsize n = istr.gcount();
    while (n > 0) {
        len += n;
        str.append(buffer.get(), static_cast<std::string::size_type>(n));
        if (istr) {
            istr.read(buffer.get(), bufferSize);
            n = istr.gcount();
        } else
            n = 0;
    }
    return len;
}


} // namespace util
} // namespace scy


/// @\}
