///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#pragma once


#include "icy/crypto/crypto.h"
#include "icy/hex.h"
#include <cstdint>
#include <memory>
#include <string_view>

#include <openssl/evp.h>


namespace icy {
namespace crypto {


using EvpMdCtxPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;


/// Incremental cryptographic hash engine wrapping OpenSSL EVP digest functions.
///
/// Construct with an algorithm name recognized by OpenSSL (e.g. "sha256",
/// "md5"). Feed data with one or more calls to update(), then call digest()
/// or digestStr() to finalize and retrieve the result. Call reset() to reuse
/// the engine for a new computation without reallocating the context.
class Crypto_API Hash
{
public:
    /// Constructs a Hash engine for the given algorithm.
    ///
    /// @param algorithm OpenSSL digest name (e.g. "sha256", "sha1", "md5").
    /// @throws std::runtime_error if the algorithm is not recognized by OpenSSL.
    Hash(const std::string& algorithm);

    /// Destroys the Hash engine and releases OpenSSL resources.
    ~Hash();

    /// Feeds a single character into the digest computation.
    ///
    /// @param data The byte to hash.
    void update(char data);

    /// Feeds a string view into the digest computation.
    ///
    /// @param data The data to hash.
    void update(std::string_view data);

    /// Feeds a raw memory buffer into the digest computation.
    ///
    /// This method may be called multiple times for streaming large inputs.
    ///
    /// @param data   Pointer to the input buffer.
    /// @param length Number of bytes to hash from @p data.
    void update(const void* data, size_t length);

    /// Finalizes the digest computation and returns the raw binary result.
    ///
    /// The result is computed on the first call and cached; subsequent calls
    /// return the same value without recomputing. Call reset() before reusing
    /// the engine for a new computation.
    ///
    /// @return Reference to the internal byte vector containing the digest.
    [[nodiscard]] const ByteVec& digest();

    /// Finalizes the digest computation and returns the result as a raw binary
    /// string (not hex-encoded). Use icy::hex::encode() on digest() if you
    /// need a printable representation.
    ///
    /// @return Binary digest as a std::string.
    [[nodiscard]] std::string digestStr();

    /// Resets the digest context and clears the cached result, allowing the
    /// engine to be reused for a new computation with the same algorithm.
    void reset();

    /// Returns the algorithm name this engine was constructed with.
    ///
    /// @return OpenSSL digest name string (e.g. "sha256").
    const std::string& algorithm() const;

protected:
    Hash& operator=(Hash const&);

    EvpMdCtxPtr _ctx;
    const EVP_MD* _md;
    crypto::ByteVec _digest;
    std::string _algorithm;
};


/// Computes a hex-encoded digest of a string in a single call.
///
/// @param algorithm OpenSSL digest name (e.g. "sha256", "md5").
/// @param data      Input data to hash.
/// @return Lowercase hex-encoded digest string.
inline std::string hash(const std::string& algorithm, std::string_view data)
{
    Hash engine(algorithm);
    engine.update(data);
    return hex::encode(engine.digest());
}


/// Computes a hex-encoded digest of a raw buffer in a single call.
///
/// @param algorithm OpenSSL digest name (e.g. "sha256", "md5").
/// @param data      Pointer to the input buffer.
/// @param length    Number of bytes to hash.
/// @return Lowercase hex-encoded digest string.
inline std::string hash(const std::string& algorithm, const void* data,
                        unsigned length)
{
    Hash engine(algorithm);
    engine.update(data, length);
    return hex::encode(engine.digest());
}


/// Computes the hex-encoded checksum of a file using the given algorithm.
///
/// Reads the file in 4096-byte chunks; suitable for large files.
///
/// @param algorithm OpenSSL digest name (e.g. "sha256", "md5").
/// @param path      Filesystem path to the file to hash.
/// @return Lowercase hex-encoded digest string.
/// @throws std::runtime_error if the file cannot be opened.
inline std::string checksum(const std::string& algorithm,
                            const std::string& path)
{
    std::ifstream fstr(path, std::ios::in | std::ios::binary);
    if (!fstr.is_open())
        throw std::runtime_error("Cannot open file: " + path);

    Hash engine(algorithm);
    char buffer[4096];
    while (fstr.read(buffer, 4096) || fstr.gcount() > 0) {
        engine.update(buffer, static_cast<size_t>(fstr.gcount()));
    }

    return hex::encode(engine.digest());
}


} // namespace crypto
} // namespace icy


/// @\}
