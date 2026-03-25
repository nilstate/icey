///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup crypto Crypto module
///
/// Cryptographic operations; hashing, HMAC, RSA, X509 certificates.
/// @{


#pragma once


#include "icy/base.h"
#include <string>
#include <vector>

#ifdef ICY_WIN
#include <windows.h>
#include <winsock2.h>

// Undefine the following definitions defined in wincrypt.h
// as they conflict with OpenSSL
#undef X509_NAME
#undef X509_CERT_PAIR
#undef X509_EXTENSIONS
#endif

// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(Crypto_EXPORTS)
#define Crypto_API __declspec(dllexport)
#else
#define Crypto_API __declspec(dllimport)
#endif
#else
#define Crypto_API // nothing
#endif


namespace icy {
/// Cryptographic primitives, key helpers, and certificate utilities backed by OpenSSL.
namespace crypto {


/// Initialize the Crypto library, as well as the underlying OpenSSL
/// libraries.
///
/// OpenSSL must be initialized before using any classes from the
/// Crypto library. OpenSSL will be initialized automatically
/// through OpenSSL instances held by various Crypto classes
/// (Cipher, Hash, X509Certificate), however it is recommended to
/// call initializeEngine() in any case at application startup.
///
/// The Crypto library can be called multiple times; however,
/// for every call to initializeEngine(), a matching call to
/// uninitializeEngine() must be performed.
Crypto_API void initializeEngine();

/// Uninitializes the Crypto library.
Crypto_API void uninitializeEngine();

/// Generic storage container for storing cryptographic binary data.
using ByteVec = std::vector<unsigned char>;

namespace internal {

/// Checks an OpenSSL return value and throws on failure.
///
/// @param ret   Return value from an OpenSSL API call; zero indicates failure.
/// @param error Optional error message to throw instead of the OpenSSL error
///              string. If null, the last OpenSSL error string is used.
/// @throws std::runtime_error on failure.
void api(int ret, const char* error = nullptr);

/// Throws a std::runtime_error containing all pending OpenSSL error strings.
///
/// Drains the OpenSSL error queue, concatenates the messages with "; ", and
/// throws the result.
/// @throws std::runtime_error always.
void throwError();

/// Type-erasing buffer view used to accept a wide range of buffer types in
/// template cipher and hash methods.
///
/// Uses const_cast internally for maximum flexibility; callers must ensure
/// the underlying data is not modified through a const T pointer. C++11
/// guarantees std::string contiguity, so string-based constructors are safe.
///
/// @tparam T Pointer type of the underlying buffer (e.g. unsigned char*).
template <typename T>
struct Raw
{
    T ptr;       ///< Pointer to the start of the buffer.
    size_t len;  ///< Length of the buffer in bytes.

    /// Constructs from an existing pointer and explicit length.
    ///
    /// @param ptr Pointer to the buffer.
    /// @param len Number of bytes in the buffer.
    Raw(T ptr, size_t len)
        : ptr(ptr)
        , len(len)
    {
    }

    /// Constructs from a const char pointer and explicit length.
    ///
    /// @param ptr Pointer to the buffer.
    /// @param len Number of bytes in the buffer.
    Raw(const char* ptr, size_t len)
        : ptr(reinterpret_cast<T>(const_cast<char*>(ptr)))
        , len(len)
    {
    }

    /// Constructs from a mutable std::string, pointing into its internal buffer.
    ///
    /// @param str Source string; must outlive this Raw instance.
    Raw(std::string& str)
    {
        ptr = reinterpret_cast<T>(&str[0]);
        len = str.length();
    }

    /// Constructs from a const std::string, pointing into its internal buffer.
    ///
    /// @param str Source string; must outlive this Raw instance.
    Raw(const std::string& str)
    {
        ptr = reinterpret_cast<T>(const_cast<char*>(&str[0]));
        len = str.length();
    }

    /// Constructs from a const std::vector<char>.
    ///
    /// @param vec Source vector; must outlive this Raw instance.
    Raw(const std::vector<char>& vec)
    {
        ptr = reinterpret_cast<T>(const_cast<char*>(&vec[0]));
        len = vec.size();
    }

    /// Constructs from a const ByteVec (std::vector<unsigned char>).
    ///
    /// @param vec Source vector; must outlive this Raw instance.
    Raw(const ByteVec& vec)
    {
        ptr = reinterpret_cast<T>(const_cast<unsigned char*>(&vec[0]));
        len = vec.size();
    }
};

} // namespace internal


} // namespace crypto
} // namespace icy


/// @}
