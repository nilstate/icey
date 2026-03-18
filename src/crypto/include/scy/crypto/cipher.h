///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#pragma once


#include "scy/crypto/crypto.h"
#include "scy/random.h"

#include "uv.h" // ssize_t

#include <openssl/evp.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>


namespace scy {
namespace crypto {


using EvpCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>;


/// Provides symmetric algorithms for encryption and decryption.
/// The algorithms that are available depend on the particular
/// version of OpenSSL that is installed.
class Crypto_API Cipher
{
public:
    /// Creates a new Cipher object. Auto initializes the key
    /// and initialization vector with random bytes.
    Cipher(const std::string& name);

    /// Creates a new Cipher object, using the given cipher name,
    /// key and initialization vector.
    Cipher(const std::string& name, const ByteVec& key, const ByteVec& iv);

    /// Creates a new Cipher object, using the given cipher name,
    /// passphrase, salt value and iteration count.
    Cipher(const std::string& name, std::string_view passphrase,
           std::string_view salt, int iterationCount);

    /// Destroys the Cipher.
    ~Cipher();

    /// Initializes the Cipher for encryption.
    void initEncryptor();

    /// Initializes the Cipher for decryption.
    void initDecryptor();

    /// Encrypts data in a streaming fashion.
    /// Hand consecutive blocks of data to the update method in order
    /// to encrypt it.
    /// Returns the encrypted data chunk. When done, the output
    /// of final() should be additionally added to the result.
    ssize_t update(const unsigned char* input, size_t inputLength,
                   unsigned char* output, size_t outputLength);

    /// Alias for update() which accepts a range of buffer types.
    template <typename I, typename O>
    ssize_t update(const I& input, O& output)
    {
        internal::Raw<const unsigned char*> in(input);
        internal::Raw<unsigned char*> out(output);
        return update(in.ptr, in.len, out.ptr, out.len);
    }

    /// Returns the remaining data held in the cipher object.
    /// Further calls to update() or final() will return garbage.
    ///
    /// See EVP_CipherFinal_ex for further information.
    ssize_t final(unsigned char* output, size_t length);

    /// Alias for final() which accepts a range of buffer types.
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

    /// Encrypts a buffer and encode it using the given encoding.
    /// This method performs the encryption, and calls final() internally.
    ssize_t encrypt(const unsigned char* inbuf, size_t inlen,
                    unsigned char* outbuf, size_t outlen,
                    Encoding encoding = Binary);

    /// Alias for encrypt() which accepts a range of buffer types.
    template <typename I, typename O>
    ssize_t encrypt(const I& input, O& output, Encoding encoding = Binary)
    {
        internal::Raw<const unsigned char*> in(input);
        internal::Raw<unsigned char*> out(output);
        return encrypt(in.ptr, in.len, out.ptr, out.len, encoding);
    }

    /// Encrypts a string and encodes it using the given encoding.
    [[nodiscard]] virtual std::string encryptString(const std::string& str, Encoding encoding = Binary);

    /// Decrypts a string that is encoded with the given encoding.
    [[nodiscard]] virtual std::string decryptString(const std::string& str, Encoding encoding = Binary);

    /// Encrypts an input stream and encodes it using the given encoding.
    virtual void encryptStream(std::istream& source, std::ostream& sink, Encoding encoding = Binary);

    /// Decrypts an input stream that is encoded with the given encoding.
    virtual void decryptStream(std::istream& source, std::ostream& sink, Encoding encoding = Binary);

    /// Sets the key for the Cipher.
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

    /// Sets the initialization vector (IV) for the Cipher.
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

    /// Enables or disables padding. By default encryption operations
    /// are padded using standard block padding and the padding is checked
    /// and removed when decrypting.
    /// If the pad parameter is zero then no padding is performed, the total
    /// amount of data encrypted or decrypted must then be a multiple of the
    /// block size or an error will occur.
    ///
    /// See EVP_CIPHER_CTX_set_padding for further information.
    int setPadding(int padding);

    /// Returns the key for the Cipher.
    const ByteVec& getKey() const;

    /// Returns the initialization vector (IV) for the Cipher.
    const ByteVec& getIV() const;

    /// Returns the name of the Cipher.
    const std::string& name() const;

    /// Returns the block size of the Cipher.
    int blockSize() const;

    /// Returns the key size of the Cipher.
    int keySize() const;

    /// Returns the IV size of the Cipher.
    int ivSize() const;

    /// Returns the cipher object.
    const EVP_CIPHER* cipher();

protected:
    Cipher() = default;
    Cipher(const Cipher&) = delete;
    Cipher& operator=(const Cipher&) = delete;
    Cipher(Cipher&&) = delete;
    Cipher& operator=(Cipher&&) = delete;

    /// Generates and sets the key and IV from a password and optional salt string.
    void generateKey(std::string_view passphrase, std::string_view salt, int iterationCount);

    /// Generates and sets key from random data.
    void setRandomKey();

    /// Generates and sets IV from random data.
    void setRandomIV();

    /// Initializes the Cipher using the given direction.
    void init(bool encrypt);

    bool _initialized;
    bool _encrypt;
    const EVP_CIPHER* _cipher;
    EvpCipherCtxPtr _ctx;
    std::string _name;
    ByteVec _key;
    ByteVec _iv;
};


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
} // namespace scy


/// @\}
