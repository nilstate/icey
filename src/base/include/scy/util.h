///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/error.h"

#include <charconv>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>


namespace scy {
namespace util {


/// Printf style string formatting for POD types.
Base_API std::string format(const char* fmt, ...);

/// Replaces special characters in the given string with
/// underscores and transform to lowercase.
Base_API void toUnderscore(std::string& str);

/// Checks if the string is a number
Base_API bool isNumber(std::string_view str);

/// Returns true if the string ends with the given substring.
Base_API bool endsWith(std::string_view str, std::string_view suffix);

/// Replaces non-alphanumeric characters.
Base_API void removeSpecialCharacters(std::string& str, bool allowSpaces = false);
Base_API void replaceSpecialCharacters(std::string& str, char with = '_',
                                       bool allowSpaces = false);

/// String to hex value.
Base_API bool tryParseHex(std::string_view s, unsigned& value);
Base_API unsigned parseHex(std::string_view s);

/// Dumps the binary representation of the
/// given buffer to the output string.
Base_API std::string dumpbin(const char* data, size_t len);

/// Compares two version strings ie. 3.7.8.0 > 3.2.1.0
/// If L (local) is greater than R (remote) the function returns true.
/// If L is equal or less than R the function returns false.
Base_API bool compareVersion(std::string_view l, std::string_view r);

/// Matches two node lists against each other.
Base_API bool matchNodes(std::string_view node, std::string_view xnode,
                         std::string_view delim = "\r\n");
Base_API bool matchNodes(const std::vector<std::string>& params,
                         const std::vector<std::string>& xparams);

/// Returns the pointer memory address as a string.
std::string memAddress(const void* ptr);


//
// Type converters
//

/// Converts integer T to string.
template <typename T>
std::string itostr(const T& t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

/// Converts string to integer T.
/// Ensure the integer type has
/// sufficient storage capacity.
template <typename T>
T strtoi(std::string_view s)
{
    std::istringstream iss{std::string(s)};
    T x;
    if (!(iss >> x))
        return 0;
    return x;
}

//
// Random generators
//

/// Generates a 31-bit pseudo random number.
Base_API uint32_t randomNumber();

/// Generates a random string.
Base_API std::string randomString(int size);

/// Generates a random (optionally base64 encoded) binary key.
Base_API std::string randomBinaryString(int size, bool doBase64 = false);


//
// String splitters
//

/// Splits the given string at the delimiter string.
Base_API void split(std::string_view str, std::string_view delim, std::vector<std::string>& elems, int limit = -1);
Base_API std::vector<std::string> split(std::string_view str, std::string_view delim, int limit = -1);

/// Splits the given string at the delimiter character.
Base_API void split(std::string_view str, char delim, std::vector<std::string>& elems, int limit = -1);
Base_API std::vector<std::string> split(std::string_view str, char delim, int limit = -1);


//
// String replace methods
//

/// Replace all occurrences of `from` in `str` with `to`, starting at position `start`.
/// Modifies and returns `str` in place. `from` must not be empty.
template <class S>
S& replaceInPlace(S& str, const S& from, const S& to,
                  typename S::size_type start = 0)
{
    if (from.size() == 0)
        throw std::logic_error("replaceInPlace: 'from' string must not be empty");

    S result;
    typename S::size_type pos = 0;
    result.append(str, 0, start);
    do {
        pos = str.find(from, start);
        if (pos != S::npos) {
            result.append(str, start, pos - start);
            result.append(to);
            start = pos + from.length();
        } else
            result.append(str, start, str.size() - start);
    } while (pos != S::npos);
    str.swap(result);
    return str;
}

/// Replace all occurrences of `from` in `str` with `to`, starting at position `start`.
/// C-string overload. Modifies and returns `str` in place.
template <class S>
S& replaceInPlace(S& str, const typename S::value_type* from,
                  const typename S::value_type* to,
                  typename S::size_type start = 0)
{
    if (!*from)
        throw std::logic_error("replaceInPlace: 'from' string must not be empty");

    S result;
    typename S::size_type pos = 0;
    typename S::size_type fromLen = std::strlen(from);
    result.append(str, 0, start);
    do {
        pos = str.find(from, start);
        if (pos != S::npos) {
            result.append(str, start, pos - start);
            result.append(to);
            start = pos + fromLen;
        } else
            result.append(str, start, str.size() - start);
    } while (pos != S::npos);
    str.swap(result);
    return str;
}

/// Replace all occurences of from (which must not be the empty string)
/// in str with to, starting at position start.
template <class S>
S replace(const S& str, const S& from, const S& to,
          typename S::size_type start = 0)
{
    S result(str);
    replaceInPlace(result, from, to, start);
    return result;
}

template <class S>
S replace(const S& str, const typename S::value_type* from,
          const typename S::value_type* to, typename S::size_type start = 0)
{
    S result(str);
    replaceInPlace(result, from, to, start);
    return result;
}


//
// String trimming
//

/// Returns a copy of str with all leading
/// whitespace removed.
template <class S>
S trimLeft(const S& str)
{
    typename S::const_iterator it = str.begin();
    typename S::const_iterator end = str.end();

    while (it != end && ::isspace(*it))
        ++it;
    return S(it, end);
}

/// Removes all leading whitespace in str.
template <class S>
S& trimLeftInPlace(S& str)
{
    typename S::iterator it = str.begin();
    typename S::iterator end = str.end();

    while (it != end && ::isspace(*it))
        ++it;
    str.erase(str.begin(), it);
    return str;
}

/// Returns a copy of str with all trailing
/// whitespace removed.
template <class S>
S trimRight(const S& str)
{
    int pos = int(str.size()) - 1;

    while (pos >= 0 && ::isspace(str[pos]))
        --pos;
    return S(str, 0, pos + 1);
}

/// Removes all trailing whitespace in str.
template <class S>
S& trimRightInPlace(S& str)
{
    int pos = int(str.size()) - 1;

    while (pos >= 0 && ::isspace(str[pos]))
        --pos;
    str.resize(pos + 1);

    return str;
}

/// Returns a copy of str with all leading and
/// trailing whitespace removed.
template <class S>
S trim(const S& str)
{
    int first = 0;
    int last = int(str.size()) - 1;

    while (first <= last && ::isspace(str[first]))
        ++first;
    while (last >= first && ::isspace(str[last]))
        --last;

    return S(str, first, last - first + 1);
}

/// Removes all leading and trailing whitespace in str.
template <class S>
S& trimInPlace(S& str)
{
    int first = 0;
    int last = int(str.size()) - 1;

    while (first <= last && ::isspace(str[first]))
        ++first;
    while (last >= first && ::isspace(str[last]))
        --last;

    str.resize(last + 1);
    str.erase(0, first);

    return str;
}


//
// String case conversion
//

/// Returns a copy of str containing all upper-case characters.
template <class S>
S toUpper(const S& str)
{
    typename S::const_iterator it = str.begin();
    typename S::const_iterator end = str.end();

    S result;
    result.reserve(str.size());
    while (it != end)
        result += static_cast<char>(::toupper(*it++));
    return result;
}

/// Replaces all characters in str with their upper-case counterparts.
template <class S>
S& toUpperInPlace(S& str)
{
    typename S::iterator it = str.begin();
    typename S::iterator end = str.end();

    while (it != end) {
        *it = static_cast<char>(::toupper(*it));
        ++it;
    }
    return str;
}

/// Returns a copy of str containing all lower-case characters.
template <class S>
S toLower(const S& str)
{
    typename S::const_iterator it = str.begin();
    typename S::const_iterator end = str.end();

    S result;
    result.reserve(str.size());
    while (it != end)
        result += static_cast<char>(::tolower(*it++));
    return result;
}

/// Replaces all characters in str with their lower-case counterparts.
template <class S>
S& toLowerInPlace(S& str)
{
    typename S::iterator it = str.begin();
    typename S::iterator end = str.end();

    while (it != end) {
        *it = static_cast<char>(::tolower(*it));
        ++it;
    }
    return str;
}


//
// String case-insensitive comparators
//

/// Case-insensitive string comparison.
/// Returns negative if a < b, zero if equal, positive if a > b.
inline int icompare(std::string_view a, std::string_view b)
{
    auto it1 = a.begin(), end1 = a.end();
    auto it2 = b.begin(), end2 = b.end();
    while (it1 != end1 && it2 != end2) {
        auto c1 = static_cast<unsigned char>(::tolower(static_cast<unsigned char>(*it1)));
        auto c2 = static_cast<unsigned char>(::tolower(static_cast<unsigned char>(*it2)));
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        ++it1;
        ++it2;
    }
    if (it1 == end1)
        return it2 == end2 ? 0 : -1;
    return 1;
}


//
// Stream copiers
//

/// Copy all data from istr to ostr one byte at a time.
Base_API std::streamsize copyStreamUnbuffered(std::istream& istr, std::ostream& ostr);

/// Copy all data from istr to ostr using a buffer of the given size.
Base_API std::streamsize copyStream(std::istream& istr, std::ostream& ostr,
                                    size_t bufferSize = 8192);

/// Read all data from istr into str using a buffer of the given size.
Base_API std::streamsize copyToString(std::istream& istr, std::string& str,
                                      size_t bufferSize = 8192);


//
// Version string helper
//

struct Version
{
    Version(std::string_view version)
    {
        int parts[4] = {0, 0, 0, 0};
        int i = 0;
        auto p = version.data();
        auto end = p + version.size();
        while (p < end && i < 4) {
            auto [ptr, ec] = std::from_chars(p, end, parts[i]);
            if (ec != std::errc())
                break;
            p = ptr;
            if (p < end && *p == '.')
                ++p;
            ++i;
        }
        major = parts[0];
        minor = parts[1];
        revision = parts[2];
        build = parts[3];
    }

    bool operator<(const Version& other)
    {
        if (major < other.major)
            return true;
        if (minor < other.minor)
            return true;
        if (revision < other.revision)
            return true;
        if (build < other.build)
            return true;
        return false;
    }

    bool operator==(const Version& other)
    {
        return major == other.major && minor == other.minor &&
               revision == other.revision && build == other.build;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Version& ver)
    {
        stream << ver.major;
        stream << '.';
        stream << ver.minor;
        stream << '.';
        stream << ver.revision;
        stream << '.';
        stream << ver.build;
        return stream;
    }

    int major, minor, revision, build;
};


//
// Container helpers
//

/// Delete all elements from a list of pointers.
template <typename Val>
inline void clearList(std::list<Val*>& L)
{
    for (auto* p : L)
        delete p;
    L.clear();
}

/// Delete all elements from a deque of pointers.
template <typename Val>
inline void clearDeque(std::deque<Val*>& D)
{
    for (auto* p : D)
        delete p;
    D.clear();
}

/// Delete all elements from a vector of pointers.
template <typename Val>
inline void clearVector(std::vector<Val*>& V)
{
    for (auto* p : V)
        delete p;
    V.clear();
}

/// Delete all associated values from a map (not the key elements).
template <typename Key, typename Val>
inline void clearMap(std::map<Key, Val*>& M)
{
    for (auto& [k, v] : M)
        delete v;
    M.clear();
}


} // namespace util
} // namespace scy


/// @\}