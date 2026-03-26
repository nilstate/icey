///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/error.h"

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


namespace icy {
/// Miscellaneous string, parsing, and version utilities.
namespace util {


/// Printf-style string formatting for POD types.
/// @param fmt printf format string.
/// @param ... Format arguments.
/// @return Formatted string.
Base_API std::string format(const char* fmt, ...);

/// Replaces all non-alphanumeric characters in str with underscores and converts to lowercase.
/// @param str String to transform in place.
Base_API void toUnderscore(std::string& str);

/// Returns true if str consists entirely of digit characters.
/// @param str String to test.
/// @return true if every character in str is a decimal digit.
Base_API bool isNumber(std::string_view str);

/// Returns true if str ends with the given suffix.
/// @param str    String to test.
/// @param suffix Suffix to look for.
/// @return true if str ends with suffix.
Base_API bool endsWith(std::string_view str, std::string_view suffix);

/// Replaces non-alphanumeric characters.
/// Removes all non-alphanumeric characters from str in place.
/// @param str         String to modify.
/// @param allowSpaces If true, ASCII spaces are preserved.
Base_API void removeSpecialCharacters(std::string& str, bool allowSpaces = false);

/// Replaces all non-alphanumeric characters in str with `with` in place.
/// @param str         String to modify.
/// @param with        Replacement character (default: '_').
/// @param allowSpaces If true, ASCII spaces are preserved rather than replaced.
Base_API void replaceSpecialCharacters(std::string& str, char with = '_',
                                       bool allowSpaces = false);

/// Attempts to parse a hex string into an unsigned integer.
/// @param s     Hex string (with or without 0x prefix).
/// @param value Output: parsed value on success.
/// @return true if parsing succeeded, false otherwise.
Base_API bool tryParseHex(std::string_view s, unsigned& value);

/// Parses a hex string into an unsigned integer.
/// @param s Hex string (with or without 0x prefix).
/// @return Parsed value.
/// @throws std::invalid_argument if the string is not valid hex.
Base_API unsigned parseHex(std::string_view s);

/// Formats the binary contents of data as a hex+ASCII dump string.
/// @param data Pointer to the buffer to dump.
/// @param len  Number of bytes to dump.
/// @return Multi-line hex dump string.
Base_API std::string dumpbin(const char* data, size_t len);

/// Compares two dot-separated version strings.
/// @param l Left (local) version string.
/// @param r Right (remote) version string.
/// @return true if l is strictly greater than r, false if l is equal or less.
Base_API bool compareVersion(std::string_view l, std::string_view r);

/// Checks whether node matches xnode by splitting both on delim and comparing element-wise.
/// @param node  Node list string to test.
/// @param xnode Expected node list pattern.
/// @param delim Delimiter used to split both strings (default: "\r\n").
/// @return true if all elements of node match the corresponding elements of xnode.
Base_API bool matchNodes(std::string_view node, std::string_view xnode,
                         std::string_view delim = "\r\n");

/// Checks whether params matches xparams element-wise.
/// @param params  Parameter list to test.
/// @param xparams Expected parameter list.
/// @return true if every element of params matches the corresponding element of xparams.
Base_API bool matchNodes(const std::vector<std::string>& params,
                         const std::vector<std::string>& xparams);

/// Returns the memory address of ptr as a hex string (e.g. "0x7f3a2b10c0").
/// @param ptr Pointer whose address to format.
/// @return Hex string representation of the pointer value.
std::string memAddress(const void* ptr);


//
// Type converters
//

/// Converts an integer (or any stream-insertable type) to its string representation.
/// @tparam T Type to convert; must support `operator<<` on std::ostream.
/// @param t Value to convert.
/// @return String representation of t.
template <typename T>
std::string itostr(const T& t)
{
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

/// Parses a string into integer type T using std::istringstream.
/// Returns 0 if parsing fails. Ensure T has sufficient range for the value.
/// @tparam T Target integer type.
/// @param s  String to parse.
/// @return Parsed value, or 0 on failure.
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

/// Generates a 31-bit pseudo random number using the internal Random instance.
/// @return Pseudo random uint32_t value.
Base_API uint32_t randomNumber();

/// Generates a random alphanumeric string of the given length.
/// @param size Number of characters to generate.
/// @return Random string of length size.
Base_API std::string randomString(int size);

/// Generates a random binary string of the given byte length.
/// @param size     Number of random bytes to generate.
/// @param doBase64 If true, returns the bytes as a base64-encoded string.
/// @return Random binary string, optionally base64-encoded.
Base_API std::string randomBinaryString(int size, bool doBase64 = false);


//
// String splitters
//

/// Splits str on the delimiter string and appends tokens to elems.
/// @param str   String to split.
/// @param delim Delimiter string.
/// @param elems Output vector; tokens are appended to it.
/// @param limit Maximum number of splits (-1 for unlimited).
Base_API void split(std::string_view str, std::string_view delim, std::vector<std::string>& elems, int limit = -1);

/// Splits str on the delimiter string and returns the tokens as a vector.
/// @param str   String to split.
/// @param delim Delimiter string.
/// @param limit Maximum number of splits (-1 for unlimited).
/// @return Vector of token strings.
Base_API std::vector<std::string> split(std::string_view str, std::string_view delim, int limit = -1);

/// Splits str on the delimiter character and appends tokens to elems.
/// @param str   String to split.
/// @param delim Delimiter character.
/// @param elems Output vector; tokens are appended to it.
/// @param limit Maximum number of splits (-1 for unlimited).
Base_API void split(std::string_view str, char delim, std::vector<std::string>& elems, int limit = -1);

/// Splits str on the delimiter character and returns the tokens as a vector.
/// @param str   String to split.
/// @param delim Delimiter character.
/// @param limit Maximum number of splits (-1 for unlimited).
/// @return Vector of token strings.
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

/// Returns a copy of str with all occurrences of from replaced by to (C-string overload).
/// @param str   Source string.
/// @param from  Substring to search for; must not be empty.
/// @param to    Replacement string.
/// @param start Position in str at which to begin searching (default: 0).
/// @return New string with all replacements applied.
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

/// Case-insensitive string comparison (locale-independent, ASCII only).
/// @param a First string.
/// @param b Second string.
/// @return Negative if a < b, zero if a == b, positive if a > b.
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

/// Copies all bytes from istr to ostr one byte at a time (no internal buffer).
/// @param istr Source stream.
/// @param ostr Destination stream.
/// @return Total number of bytes copied.
Base_API std::streamsize copyStreamUnbuffered(std::istream& istr, std::ostream& ostr);

/// Copies all bytes from istr to ostr using an internal buffer.
/// @param istr       Source stream.
/// @param ostr       Destination stream.
/// @param bufferSize Internal buffer size in bytes (default: 8192).
/// @return Total number of bytes copied.
Base_API std::streamsize copyStream(std::istream& istr, std::ostream& ostr,
                                    size_t bufferSize = 8192);

/// Reads all bytes from istr and appends them to str.
/// @param istr       Source stream.
/// @param str        Output string to append data to.
/// @param bufferSize Internal buffer size in bytes (default: 8192).
/// @return Total number of bytes read.
Base_API std::streamsize copyToString(std::istream& istr, std::string& str,
                                      size_t bufferSize = 8192);


//
// Version string helper
//

/// Semantic version number with major, minor, and patch fields
struct Version
{
    /// Parses a dot-separated version string into up to four numeric fields.
    /// Unspecified fields default to 0. Examples: "1.2.3", "2.0", "3.7.8.0".
    /// @param version Dot-separated version string.
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

    /// Returns true if this version is strictly less than other.
    /// Compares fields in major, minor, revision, build order.
    /// @param other Version to compare against.
    /// @return true if this < other.
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

    /// Returns true if all four version fields are equal.
    /// @param other Version to compare against.
    /// @return true if this == other.
    bool operator==(const Version& other) const
    {
        return major == other.major && minor == other.minor &&
               revision == other.revision && build == other.build;
    }

    /// Writes the version to a stream in "major.minor.revision.build" format.
    /// @param stream Output stream.
    /// @param ver    Version to format.
    /// @return Reference to stream.
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
} // namespace icy


/// @}
