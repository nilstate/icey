///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#pragma once


#include "icy/crypto/crypto.h"
#include "icy/random.h"

#include "uv.h" // ssize_t

#include <openssl/evp.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>


namespace icy {
namespace crypto {


/// Owning OpenSSL cipher context handle with automatic `EVP_CIPHER_CTX_free`.
using EvpCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>;


/// Provides symmetric algorithms for encryption and decryption.
/// The algorithms that are available depend on the particular
/// version of OpenSSL that is installed.
class Crypto_API Cipher
{
public:
    /// Constructs a Cipher with a randomly generated key and IV.
    ///
    /// @param name OpenSSL cipher name (e.g. "aes-256-cbc").
    /// @throws std::invalid_argument if the cipher name is not recognized.
    Cipher(const std::string& name);

    /// Constructs a Cipher with an explicit key and initialization vector.
    ///
    /// @param name OpenSSL cipher name (e.g. "aes-256-cbc").
    /// @param key  Encryption key; must match the cipher's required key length.
    /// @param iv   Initialization vector; must match the cipher's IV length.
    /// @throws std::invalid_argument if the cipher name is not recognized.
    Cipher(const std::string& name, const ByteVec& key, const ByteVec& iv);

    /// Constructs a Cipher and derives a key and IV from a passphrase.
    ///
    /// Uses EVP_BytesToKey with SHA-256 to derive the key material.
    ///
    /// @param name           OpenSSL cipher name (e.g. "aes-256-cbc").
    /// @param passphrase     Secret passphrase for key derivation.
    /// @param salt           Optional salt string; empty string means no salt.
    ///                       Values longer than 8 bytes are folded via XOR.
    /// @param iterationCount Number of key-derivation iterations.
    /// @throws std::invalid_argument if the cipher name is not recognized.
    Cipher(const std::string& name, std::string_view passphrase,
           std::string_view salt, int iterationCount);

    /// Destroys the Cipher and resets the OpenSSL context.
    ~Cipher();

    /// Initializes the cipher context for encryption.
    ///
    /// Must be called before using update() and final() in encrypt mode.
    /// Calling this resets any prior context state.
    void initEncryptor();

    /// Initializes the cipher context for decryption.
    ///
    /// Must be called before using update() and final() in decrypt mode.
    /// Calling this resets any prior context state.
    void initDecryptor();

    /// Processes a block of data through the cipher (encrypt or decrypt).
    ///
    /// Hand consecutive blocks of data to this method for streaming operation.
    /// The output buffer must be at least `inputLength + blockSize() - 1` bytes.
    /// After all input is processed, call final() to flush any remaining
    /// buffered data from the cipher context.
    ///
    /// @param input        Pointer to the input data buffer.
    /// @param inputLength  Number of bytes to process from @p input.
    /// @param output       Pointer to the output buffer.
    /// @param outputLength Size of the output buffer in bytes.
    /// @return Number of bytes written to @p output.
    /// @throws std::runtime_error if the output buffer is too small.
    ssize_t update(const unsigned char* input, size_t inputLength,
                   unsigned char* output, size_t outputLength);

    /// Processes a block of data through the cipher using generic buffer types.
    ///
    /// Convenience wrapper around update(const unsigned char*, size_t,
    /// unsigned char*, size_t). Accepts any type supported by internal::Raw.
    ///
    /// @param input  Input buffer (std::string, ByteVec, etc.).
    /// @param output Output buffer; must be large enough for the result.
    /// @return Number of bytes written to @p output.
    template <typename I, typename O>
    ssize_t update(const I& input, O& output)
    {
        internal::Raw<const unsigned char*> in(input);
        internal::Raw<unsigned char*> out(output);
        return update(in.ptr, in.len, out.ptr, out.len);
    }

    /// Finalizes the cipher operation and flushes any remaining buffered data.
    ///
    /// Must be called after the last update() call to retrieve any trailing
    /// cipher block. Further calls to update() or final() after this point
    /// produce undefined results; call initEncryptor() / initDecryptor() to
    /// reset. The output buffer must be at least blockSize() bytes.
    ///
    /// See EVP_CipherFinal_ex for further information.
    ///
    /// @param output Pointer to the output buffer; must be at least blockSize() bytes.
    /// @param length Size of the output buffer in bytes.
    /// @return Number of bytes written to @p output.
    /// @throws std::runtime_error if the output buffer is smaller than blockSize().
    ssize_t final(unsigned char* output, size_t length);

    /// Finalizes the cipher operation using a generic output buffer type.
    ///
    /// Convenience wrapper around final(unsigned char*, size_t). Accepts any
    /// type supported by internal::Raw.
    ///
    /// @param output Output buffer; must hold at least blockSize() bytes.
    /// @return Number of bytes written to @p output.
    template <typename O>
    ssize_t final(O& output)
    {
        internal::Raw<unsigned char*> out(output);
        return final(out.ptr, out.len);
    }

    /// Transport encoding to use for encrypt() and decrypt().
    enum Encoding
    {
        Binary = 0x00,      ///< Plain binary output
        Base64 = 0x01,      ///< Base64-encoded output
        BinHex = 0x02,      ///< BinHex-encoded output
        Base64_NoLF = 0x81, ///< Base64-encoded output, no linefeeds
        BinHex_NoLF = 0x82, ///< BinHex-encoded output, no linefeeds
    };

    /// Encrypts a buffer and writes the result with optional transport encoding.
    ///
    /// Calls initEncryptor(), update(), and final() internally; the cipher
    /// does not need to be pre-initialized. The output buffer must be large
    /// enough to hold the encrypted and encoded result.
    ///
    /// @param inbuf    Pointer to the plaintext input buffer.
    /// @param inlen    Number of bytes to encrypt from @p inbuf.
    /// @param outbuf   Pointer to the output buffer.
    /// @param outlen   Size of the output buffer in bytes.
    /// @param encoding Transport encoding applied to the ciphertext (default: Binary).
    /// @return Total number of bytes written to @p outbuf.
    ssize_t encrypt(const unsigned char* inbuf, size_t inlen,
                    unsigned char* outbuf, size_t outlen,
                    Encoding encoding = Binary);

    /// Encrypts data using generic input/output buffer types.
    ///
    /// Convenience wrapper around encrypt(const unsigned char*, size_t,
    /// unsigned char*, size_t, Encoding). Accepts any type supported by
    /// internal::Raw.
    ///
    /// @param input    Input buffer containing plaintext.
    /// @param output   Output buffer; must be large enough for the result.
    /// @param encoding Transport encoding applied to the ciphertext (default: Binary).
    /// @return Total number of bytes written to @p output.
    template <typename I, typename O>
    ssize_t encrypt(const I& input, O& output, Encoding encoding = Binary)
    {
        internal::Raw<const unsigned char*> in(input);
        internal::Raw<unsigned char*> out(output);
        return encrypt(in.ptr, in.len, out.ptr, out.len, encoding);
    }

    /// Encrypts a string and returns the result with optional transport encoding.
    ///
    /// Internally streams through encryptStream(); the cipher is re-initialized
    /// on each call.
    ///
    /// @param str      Plaintext string to encrypt.
    /// @param encoding Transport encoding for the output (default: Binary).
    /// @return Encrypted (and optionally encoded) result as a std::string.
    [[nodiscard]] virtual std::string encryptString(const std::string& str, Encoding encoding = Binary);

    /// Decrypts a string that was previously encrypted with optional encoding.
    ///
    /// Internally streams through decryptStream(); the cipher is re-initialized
    /// on each call.
    ///
    /// @param str      Ciphertext string to decrypt, in the format given by @p encoding.
    /// @param encoding Transport encoding of the input (default: Binary).
    /// @return Decrypted plaintext as a std::string.
    [[nodiscard]] virtual std::string decryptString(const std::string& str, Encoding encoding = Binary);

    /// Encrypts all data from @p source and writes the result to @p sink.
    ///
    /// Reads in chunks of `blockSize() * 128` bytes. Calls initEncryptor()
    /// internally; no prior initialization is required.
    ///
    /// @param source   Input stream containing plaintext.
    /// @param sink     Output stream to receive the encrypted (and encoded) data.
    /// @param encoding Transport encoding applied to the output (default: Binary).
    virtual void encryptStream(std::istream& source, std::ostream& sink, Encoding encoding = Binary);

    /// Decrypts all data from @p source and writes the result to @p sink.
    ///
    /// Reads in chunks of `blockSize() * 128` bytes. Calls initDecryptor()
    /// internally; no prior initialization is required.
    ///
    /// @param source   Input stream containing ciphertext (in the given encoding).
    /// @param sink     Output stream to receive the decrypted plaintext.
    /// @param encoding Transport encoding of the input data (default: Binary).
    virtual void decryptStream(std::istream& source, std::ostream& sink, Encoding encoding = Binary);

    /// Sets the encryption key.
    ///
    /// @param key Container whose size must exactly match keySize().
    /// @throws std::logic_error if key.size() != keySize().
    template <typename T>
    void setKey(const T& key)
    {
        if (int(key.size()) != keySize())
            throw std::logic_error("Cipher::setKey: key size mismatch (got " +
                std::to_string(key.size()) + ", expected " + std::to_string(keySize()) + ")");
        _key.clear();
        for (const auto& k : key)
            _key.push_back(static_cast<unsigned char>(k));
    }

    /// Sets the initialization vector (IV).
    ///
    /// @param iv Container whose size must exactly match ivSize().
    /// @throws std::logic_error if iv.size() != ivSize().
    template <typename T>
    void setIV(const T& iv)
    {
        if (int(iv.size()) != ivSize())
            throw std::logic_error("Cipher::setIV: IV size mismatch (got " +
                std::to_string(iv.size()) + ", expected " + std::to_string(ivSize()) + ")");
        _iv.clear();
        for (const auto& v : iv)
            _iv.push_back(static_cast<unsigned char>(v));
    }

    /// Enables or disables PKCS block padding.
    ///
    /// By default, encryption pads input to a block boundary and decryption
    /// strips and validates the padding. If @p padding is zero, no padding is
    /// applied; the total data length must then be an exact multiple of
    /// blockSize() or the operation will fail.
    ///
    /// See EVP_CIPHER_CTX_set_padding for further information.
    ///
    /// @param padding Non-zero to enable padding (default), zero to disable.
    /// @return The return value from EVP_CIPHER_CTX_set_padding.
    int setPadding(int padding);

    /// Returns the raw encryption key bytes.
    ///
    /// @return Reference to the internal key byte vector.
    const ByteVec& getKey() const;

    /// Returns the raw initialization vector bytes.
    ///
    /// @return Reference to the internal IV byte vector.
    const ByteVec& getIV() const;

    /// Returns the OpenSSL cipher name this object was constructed with.
    ///
    /// @return Cipher name string (e.g. "aes-256-cbc").
    const std::string& name() const;

    /// Returns the cipher block size in bytes.
    ///
    /// @return Block size as reported by EVP_CIPHER_block_size.
    int blockSize() const;

    /// Returns the required key length in bytes for this cipher.
    ///
    /// @return Key length as reported by EVP_CIPHER_key_length.
    int keySize() const;

    /// Returns the required initialization vector length in bytes.
    ///
    /// @return IV length as reported by EVP_CIPHER_iv_length.
    int ivSize() const;

    /// Returns the underlying OpenSSL EVP_CIPHER object.
    ///
    /// @return Pointer to the OpenSSL cipher descriptor; valid for the
    ///         lifetime of this Cipher object.
    const EVP_CIPHER* cipher();

protected:
    Cipher() = delete;
    Cipher(const Cipher&) = delete;
    Cipher& operator=(const Cipher&) = delete;
    Cipher(Cipher&&) = delete;
    Cipher& operator=(Cipher&&) = delete;

    /// Derives and sets the key and IV from a passphrase using EVP_BytesToKey.
    ///
    /// Uses SHA-256 as the digest. Salt values longer than 8 bytes are folded
    /// by XOR into an 8-byte array as required by OpenSSL.
    ///
    /// @param passphrase     Secret passphrase for key derivation.
    /// @param salt           Salt string (may be empty for no salt).
    /// @param iterationCount Number of digest iterations.
    void generateKey(std::string_view passphrase, std::string_view salt, int iterationCount);

    /// Fills the key buffer with cryptographically random bytes.
    void setRandomKey();

    /// Fills the IV buffer with cryptographically random bytes.
    void setRandomIV();

    /// Initializes or resets the OpenSSL cipher context for the given direction.
    ///
    /// @param encrypt true to initialize for encryption, false for decryption.
    void init(bool encrypt);

    bool _initialized;
    bool _encrypt;
    const EVP_CIPHER* _cipher;
    EvpCipherCtxPtr _ctx;
    std::string _name;
    ByteVec _key;
    ByteVec _iv;
};


/// Encrypts a string using the specified cipher, key, and IV in a single call.
///
/// Constructs a Cipher, optionally applies @p key and @p iv (skipped when
/// empty), then delegates to Cipher::encryptString().
///
/// @tparam K      Key container type compatible with internal::Raw.
/// @tparam I      IV container type compatible with internal::Raw.
/// @param algorithm OpenSSL cipher name (e.g. "aes-256-cbc").
/// @param data      Plaintext string to encrypt.
/// @param key       Encryption key; pass an empty container to use a random key.
/// @param iv        Initialization vector; pass an empty container to use a random IV.
/// @param encoding  Transport encoding for the output (default: Binary).
/// @return Encrypted (and optionally encoded) result as a std::string.
template <typename K, typename I>
std::string encryptString(const std::string& algorithm, const std::string& data,
                          const K& key, const I& iv,
                          Cipher::Encoding encoding = Cipher::Binary)
{
    Cipher ciph(algorithm);

    if (key.size())
        ciph.setKey(key);
    if (iv.size())
        ciph.setIV(iv);

    return ciph.encryptString(data, encoding);
}


/// Decrypts a string using the specified cipher, key, and IV in a single call.
///
/// Constructs a Cipher, optionally applies @p key and @p iv (skipped when
/// empty), then delegates to Cipher::decryptString().
///
/// @tparam K      Key container type compatible with internal::Raw.
/// @tparam I      IV container type compatible with internal::Raw.
/// @param algorithm OpenSSL cipher name (e.g. "aes-256-cbc").
/// @param data      Ciphertext string to decrypt, in the format given by @p encoding.
/// @param key       Decryption key; pass an empty container to use a random key.
/// @param iv        Initialization vector; pass an empty container to use a random IV.
/// @param encoding  Transport encoding of the input data (default: Binary).
/// @return Decrypted plaintext as a std::string.
template <typename K, typename I>
std::string decryptString(const std::string& algorithm, const std::string& data,
                          const K& key, const I& iv,
                          Cipher::Encoding encoding = Cipher::Binary)
{
    Cipher ciph(algorithm);

    if (key.size())
        ciph.setKey(key);
    if (iv.size())
        ciph.setIV(iv);

    return ciph.decryptString(data, encoding);
}


} // namespace crypto
} // namespace icy


/// @}
