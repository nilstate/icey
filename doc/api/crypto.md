{#cryptomodule}

# crypto

Cryptographic operations; hashing, HMAC, RSA, X509 certificates.

### Namespaces

| Name | Description |
|------|-------------|
| [`crypto`](#crypto) | Cryptographic primitives, key helpers, and certificate utilities backed by OpenSSL. |

{#crypto}

# crypto

Cryptographic primitives, key helpers, and certificate utilities backed by OpenSSL.

### Classes

| Name | Description |
|------|-------------|
| [`Cipher`](#cipher) | Provides symmetric algorithms for encryption and decryption. The algorithms that are available depend on the particular version of OpenSSL that is installed. |
| [`Hash`](#hash-3) | Incremental cryptographic hash engine wrapping OpenSSL EVP digest functions. |
| [`X509Certificate`](#x509certificate) | RAII wrapper for an OpenSSL X509 certificate with PEM loading and inspection. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>` | [`EvpCipherCtxPtr`](#evpcipherctxptr)  | Owning OpenSSL cipher context handle with automatic `EVP_CIPHER_CTX_free`. |
| `std::vector< unsigned char >` | [`ByteVec`](#bytevec)  | Generic storage container for storing cryptographic binary data. |
| `std::unique_ptr< EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>` | [`EvpMdCtxPtr`](#evpmdctxptr)  | Owning OpenSSL digest context handle with automatic `EVP_MD_CTX_free`. |
| `::RSA` | [`RSAKey`](#rsakey)  | Alias for the OpenSSL RSA key type, brought into the [icy::crypto](#crypto) namespace. |
| `std::unique_ptr< X509, decltype(&X509_free)>` | [`X509Ptr`](#x509ptr)  | RAII pointer alias for OpenSSL `X509*` values. |

---

{#evpcipherctxptr}

#### EvpCipherCtxPtr

```cpp
std::unique_ptr< EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> EvpCipherCtxPtr()
```

Owning OpenSSL cipher context handle with automatic `EVP_CIPHER_CTX_free`.

---

{#bytevec}

#### ByteVec

```cpp
std::vector< unsigned char > ByteVec()
```

Generic storage container for storing cryptographic binary data.

---

{#evpmdctxptr}

#### EvpMdCtxPtr

```cpp
std::unique_ptr< EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> EvpMdCtxPtr()
```

Owning OpenSSL digest context handle with automatic `EVP_MD_CTX_free`.

---

{#rsakey}

#### RSAKey

```cpp
::RSA RSAKey()
```

Alias for the OpenSSL RSA key type, brought into the [icy::crypto](#crypto) namespace.

Currently a transparent alias for the OpenSSL RSA struct. Use OpenSSL RSA_* functions directly to create, populate, and free RSAKey objects. This alias exists as a stable forward-declaration point; a higher-level RAII wrapper may replace it in a future version.

---

{#x509ptr}

#### X509Ptr

```cpp
std::unique_ptr< X509, decltype(&X509_free)> X509Ptr()
```

RAII pointer alias for OpenSSL `X509*` values.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`encryptString`](#encryptstring)  | Encrypts a string using the specified cipher, key, and IV in a single call. |
| `std::string` | [`decryptString`](#decryptstring)  | Decrypts a string using the specified cipher, key, and IV in a single call. |
| `void` | [`initializeEngine`](#initializeengine)  | Initialize the Crypto library, as well as the underlying OpenSSL libraries. |
| `void` | [`uninitializeEngine`](#uninitializeengine)  | Uninitializes the Crypto library. |
| `std::string` | [`hash`](#hash-1) `inline` | Computes a hex-encoded digest of a string in a single call. |
| `std::string` | [`hash`](#hash-2) `inline` | Computes a hex-encoded digest of a raw buffer in a single call. |
| `std::string` | [`checksum`](#checksum-1) `inline` | Computes the hex-encoded checksum of a file using the given algorithm. |
| `std::string` | [`computeHMAC`](#computehmac)  | Computes an HMAC-SHA1 message authentication code. |

---

{#encryptstring}

#### encryptString

```cpp
template<typename K, typename I> std::string encryptString(const std::string & algorithm, const std::string & data, const K & key, const I & iv, Cipher::Encoding encoding)
```

Encrypts a string using the specified cipher, key, and IV in a single call.

Constructs a [Cipher](#cipher), optionally applies `key` and `iv` (skipped when empty), then delegates to [Cipher::encryptString()](#encryptstring-1).

#### Parameters
* `K` Key container type compatible with internal::Raw. 

* `I` IV container type compatible with internal::Raw. 

#### Parameters
* `algorithm` OpenSSL cipher name (e.g. "aes-256-cbc"). 

* `data` Plaintext string to encrypt. 

* `key` Encryption key; pass an empty container to use a random key. 

* `iv` Initialization vector; pass an empty container to use a random IV. 

* `encoding` Transport encoding for the output (default: Binary). 

#### Returns
Encrypted (and optionally encoded) result as a std::string.

---

{#decryptstring}

#### decryptString

```cpp
template<typename K, typename I> std::string decryptString(const std::string & algorithm, const std::string & data, const K & key, const I & iv, Cipher::Encoding encoding)
```

Decrypts a string using the specified cipher, key, and IV in a single call.

Constructs a [Cipher](#cipher), optionally applies `key` and `iv` (skipped when empty), then delegates to [Cipher::decryptString()](#decryptstring-1).

#### Parameters
* `K` Key container type compatible with internal::Raw. 

* `I` IV container type compatible with internal::Raw. 

#### Parameters
* `algorithm` OpenSSL cipher name (e.g. "aes-256-cbc"). 

* `data` Ciphertext string to decrypt, in the format given by `encoding`. 

* `key` Decryption key; pass an empty container to use a random key. 

* `iv` Initialization vector; pass an empty container to use a random IV. 

* `encoding` Transport encoding of the input data (default: Binary). 

#### Returns
Decrypted plaintext as a std::string.

---

{#initializeengine}

#### initializeEngine

```cpp
void initializeEngine()
```

Initialize the Crypto library, as well as the underlying OpenSSL libraries.

OpenSSL must be initialized before using any classes from the Crypto library. OpenSSL will be initialized automatically through OpenSSL instances held by various Crypto classes ([Cipher](#cipher), [Hash](#hash-3), [X509Certificate](#x509certificate)), however it is recommended to call [initializeEngine()](#initializeengine) in any case at application startup.

The Crypto library can be called multiple times; however, for every call to [initializeEngine()](#initializeengine), a matching call to [uninitializeEngine()](#uninitializeengine) must be performed.

---

{#uninitializeengine}

#### uninitializeEngine

```cpp
void uninitializeEngine()
```

Uninitializes the Crypto library.

---

{#hash-1}

#### hash

`inline`

```cpp
inline std::string hash(const std::string & algorithm, std::string_view data)
```

Computes a hex-encoded digest of a string in a single call.

#### Parameters
* `algorithm` OpenSSL digest name (e.g. "sha256", "md5"). 

* `data` Input data to hash. 

#### Returns
Lowercase hex-encoded digest string.

---

{#hash-2}

#### hash

`inline`

```cpp
inline std::string hash(const std::string & algorithm, const void * data, unsigned length)
```

Computes a hex-encoded digest of a raw buffer in a single call.

#### Parameters
* `algorithm` OpenSSL digest name (e.g. "sha256", "md5"). 

* `data` Pointer to the input buffer. 

* `length` Number of bytes to hash. 

#### Returns
Lowercase hex-encoded digest string.

---

{#checksum-1}

#### checksum

`inline`

```cpp
inline std::string checksum(const std::string & algorithm, const std::string & path)
```

Computes the hex-encoded checksum of a file using the given algorithm.

Reads the file in 4096-byte chunks; suitable for large files.

#### Parameters
* `algorithm` OpenSSL digest name (e.g. "sha256", "md5"). 

* `path` Filesystem path to the file to hash. 

#### Returns
Lowercase hex-encoded digest string. 

#### Exceptions
* `std::runtime_error` if the file cannot be opened.

---

{#computehmac}

#### computeHMAC

```cpp
std::string computeHMAC(std::string_view input, std::string_view key)
```

Computes an HMAC-SHA1 message authentication code.

Uses OpenSSL HMAC with SHA-1 as the underlying digest. The output is a 20-byte raw binary string (not hex-encoded).

#### Parameters
* `input` Data to authenticate. 

* `key` Secret key used for the HMAC computation. 

#### Returns
20-byte raw binary HMAC-SHA1 digest. 

#### Exceptions
* `std::runtime_error` if OpenSSL returns an unexpected digest length.

{#cipher}

## Cipher

```cpp
#include <icy/crypto/cipher.h>
```

Provides symmetric algorithms for encryption and decryption. The algorithms that are available depend on the particular version of OpenSSL that is installed.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Cipher`](#cipher-1)  | Constructs a [Cipher](#cipher) with a randomly generated key and IV. |
|  | [`Cipher`](#cipher-2)  | Constructs a [Cipher](#cipher) with an explicit key and initialization vector. |
|  | [`Cipher`](#cipher-3)  | Constructs a [Cipher](#cipher) and derives a key and IV from a passphrase. |
|  | [`~Cipher`](#cipher-4)  | Destroys the [Cipher](#cipher) and resets the OpenSSL context. |
| `void` | [`initEncryptor`](#initencryptor)  | Initializes the cipher context for encryption. |
| `void` | [`initDecryptor`](#initdecryptor)  | Initializes the cipher context for decryption. |
| `ssize_t` | [`update`](#update-7)  | Processes a block of data through the cipher (encrypt or decrypt). |
| `ssize_t` | [`update`](#update-8) `inline` | Processes a block of data through the cipher using generic buffer types. |
| `ssize_t` | [`final`](#final)  | Finalizes the cipher operation and flushes any remaining buffered data. |
| `ssize_t` | [`final`](#final-1) `inline` | Finalizes the cipher operation using a generic output buffer type. |
| `ssize_t` | [`encrypt`](#encrypt)  | Encrypts a buffer and writes the result with optional transport encoding. |
| `ssize_t` | [`encrypt`](#encrypt-1) `inline` | Encrypts data using generic input/output buffer types. |
| `std::string` | [`encryptString`](#encryptstring-1) `virtual` | Encrypts a string and returns the result with optional transport encoding. |
| `std::string` | [`decryptString`](#decryptstring-1) `virtual` | Decrypts a string that was previously encrypted with optional encoding. |
| `void` | [`encryptStream`](#encryptstream) `virtual` | Encrypts all data from `source` and writes the result to `sink`. |
| `void` | [`decryptStream`](#decryptstream) `virtual` | Decrypts all data from `source` and writes the result to `sink`. |
| `void` | [`setKey`](#setkey-1) `inline` | Sets the encryption key. |
| `void` | [`setIV`](#setiv) `inline` | Sets the initialization vector (IV). |
| `int` | [`setPadding`](#setpadding)  | Enables or disables PKCS block padding. |
| `const ByteVec &` | [`getKey`](#getkey) `const` | Returns the raw encryption key bytes. |
| `const ByteVec &` | [`getIV`](#getiv) `const` | Returns the raw initialization vector bytes. |
| `const std::string &` | [`name`](#name-10) `const` | Returns the OpenSSL cipher name this object was constructed with. |
| `int` | [`blockSize`](#blocksize) `const` | Returns the cipher block size in bytes. |
| `int` | [`keySize`](#keysize) `const` | Returns the required key length in bytes for this cipher. |
| `int` | [`ivSize`](#ivsize) `const` | Returns the required initialization vector length in bytes. |
| `const EVP_CIPHER *` | [`cipher`](#cipher-5)  | Returns the underlying OpenSSL EVP_CIPHER object. |

---

{#cipher-1}

#### Cipher

```cpp
Cipher(const std::string & name)
```

Constructs a [Cipher](#cipher) with a randomly generated key and IV.

#### Parameters
* `name` OpenSSL cipher name (e.g. "aes-256-cbc"). 

#### Exceptions
* `std::invalid_argument` if the cipher name is not recognized.

---

{#cipher-2}

#### Cipher

```cpp
Cipher(const std::string & name, const ByteVec & key, const ByteVec & iv)
```

Constructs a [Cipher](#cipher) with an explicit key and initialization vector.

#### Parameters
* `name` OpenSSL cipher name (e.g. "aes-256-cbc"). 

* `key` Encryption key; must match the cipher's required key length. 

* `iv` Initialization vector; must match the cipher's IV length. 

#### Exceptions
* `std::invalid_argument` if the cipher name is not recognized.

---

{#cipher-3}

#### Cipher

```cpp
Cipher(const std::string & name, std::string_view passphrase, std::string_view salt, int iterationCount)
```

Constructs a [Cipher](#cipher) and derives a key and IV from a passphrase.

Uses EVP_BytesToKey with SHA-256 to derive the key material.

#### Parameters
* `name` OpenSSL cipher name (e.g. "aes-256-cbc"). 

* `passphrase` Secret passphrase for key derivation. 

* `salt` Optional salt string; empty string means no salt. Values longer than 8 bytes are folded via XOR. 

* `iterationCount` Number of key-derivation iterations. 

#### Exceptions
* `std::invalid_argument` if the cipher name is not recognized.

---

{#cipher-4}

#### ~Cipher

```cpp
~Cipher()
```

Destroys the [Cipher](#cipher) and resets the OpenSSL context.

---

{#initencryptor}

#### initEncryptor

```cpp
void initEncryptor()
```

Initializes the cipher context for encryption.

Must be called before using [update()](#update-7) and [final()](#final) in encrypt mode. Calling this resets any prior context state.

---

{#initdecryptor}

#### initDecryptor

```cpp
void initDecryptor()
```

Initializes the cipher context for decryption.

Must be called before using [update()](#update-7) and [final()](#final) in decrypt mode. Calling this resets any prior context state.

---

{#update-7}

#### update

```cpp
ssize_t update(const unsigned char * input, size_t inputLength, unsigned char * output, size_t outputLength)
```

Processes a block of data through the cipher (encrypt or decrypt).

Hand consecutive blocks of data to this method for streaming operation. The output buffer must be at least `inputLength + [blockSize()](#blocksize) - 1` bytes. After all input is processed, call [final()](#final) to flush any remaining buffered data from the cipher context.

#### Parameters
* `input` Pointer to the input data buffer. 

* `inputLength` Number of bytes to process from `input`. 

* `output` Pointer to the output buffer. 

* `outputLength` Size of the output buffer in bytes. 

#### Returns
Number of bytes written to `output`. 

#### Exceptions
* `std::runtime_error` if the output buffer is too small.

---

{#update-8}

#### update

`inline`

```cpp
template<typename I, typename O> inline ssize_t update(const I & input, O & output)
```

Processes a block of data through the cipher using generic buffer types.

Convenience wrapper around [update(const unsigned char*, size_t,
unsigned char*, size_t)](#update-7). Accepts any type supported by internal::Raw.

#### Parameters
* `input` Input buffer (std::string, ByteVec, etc.). 

* `output` Output buffer; must be large enough for the result. 

#### Returns
Number of bytes written to `output`.

---

{#final}

#### final

```cpp
ssize_t final(unsigned char * output, size_t length)
```

Finalizes the cipher operation and flushes any remaining buffered data.

Must be called after the last [update()](#update-7) call to retrieve any trailing cipher block. Further calls to [update()](#update-7) or [final()](#final) after this point produce undefined results; call [initEncryptor()](#initencryptor) / [initDecryptor()](#initdecryptor) to reset. The output buffer must be at least [blockSize()](#blocksize) bytes.

See EVP_CipherFinal_ex for further information.

#### Parameters
* `output` Pointer to the output buffer; must be at least [blockSize()](#blocksize) bytes. 

* `length` Size of the output buffer in bytes. 

#### Returns
Number of bytes written to `output`. 

#### Exceptions
* `std::runtime_error` if the output buffer is smaller than [blockSize()](#blocksize).

---

{#final-1}

#### final

`inline`

```cpp
template<typename O> inline ssize_t final(O & output)
```

Finalizes the cipher operation using a generic output buffer type.

Convenience wrapper around [final(unsigned char*, size_t)](#final). Accepts any type supported by internal::Raw.

#### Parameters
* `output` Output buffer; must hold at least [blockSize()](#blocksize) bytes. 

#### Returns
Number of bytes written to `output`.

---

{#encrypt}

#### encrypt

```cpp
ssize_t encrypt(const unsigned char * inbuf, size_t inlen, unsigned char * outbuf, size_t outlen, Encoding encoding)
```

Encrypts a buffer and writes the result with optional transport encoding.

Calls [initEncryptor()](#initencryptor), [update()](#update-7), and [final()](#final) internally; the cipher does not need to be pre-initialized. The output buffer must be large enough to hold the encrypted and encoded result.

#### Parameters
* `inbuf` Pointer to the plaintext input buffer. 

* `inlen` Number of bytes to encrypt from `inbuf`. 

* `outbuf` Pointer to the output buffer. 

* `outlen` Size of the output buffer in bytes. 

* `encoding` Transport encoding applied to the ciphertext (default: Binary). 

#### Returns
Total number of bytes written to `outbuf`.

---

{#encrypt-1}

#### encrypt

`inline`

```cpp
template<typename I, typename O> inline ssize_t encrypt(const I & input, O & output, Encoding encoding)
```

Encrypts data using generic input/output buffer types.

Convenience wrapper around [encrypt(const unsigned char*, size_t,
unsigned char*, size_t, Encoding)](#encrypt). Accepts any type supported by internal::Raw.

#### Parameters
* `input` Input buffer containing plaintext. 

* `output` Output buffer; must be large enough for the result. 

* `encoding` Transport encoding applied to the ciphertext (default: Binary). 

#### Returns
Total number of bytes written to `output`.

---

{#encryptstring-1}

#### encryptString

`virtual`

```cpp
virtual std::string encryptString(const std::string & str, Encoding encoding)
```

Encrypts a string and returns the result with optional transport encoding.

Internally streams through [encryptStream()](#encryptstream); the cipher is re-initialized on each call.

#### Parameters
* `str` Plaintext string to encrypt. 

* `encoding` Transport encoding for the output (default: Binary). 

#### Returns
Encrypted (and optionally encoded) result as a std::string.

---

{#decryptstring-1}

#### decryptString

`virtual`

```cpp
virtual std::string decryptString(const std::string & str, Encoding encoding)
```

Decrypts a string that was previously encrypted with optional encoding.

Internally streams through [decryptStream()](#decryptstream); the cipher is re-initialized on each call.

#### Parameters
* `str` Ciphertext string to decrypt, in the format given by `encoding`. 

* `encoding` Transport encoding of the input (default: Binary). 

#### Returns
Decrypted plaintext as a std::string.

---

{#encryptstream}

#### encryptStream

`virtual`

```cpp
virtual void encryptStream(std::istream & source, std::ostream & sink, Encoding encoding)
```

Encrypts all data from `source` and writes the result to `sink`.

Reads in chunks of `[blockSize()](#blocksize) * 128` bytes. Calls [initEncryptor()](#initencryptor) internally; no prior initialization is required.

#### Parameters
* `source` Input stream containing plaintext. 

* `sink` Output stream to receive the encrypted (and encoded) data. 

* `encoding` Transport encoding applied to the output (default: Binary).

---

{#decryptstream}

#### decryptStream

`virtual`

```cpp
virtual void decryptStream(std::istream & source, std::ostream & sink, Encoding encoding)
```

Decrypts all data from `source` and writes the result to `sink`.

Reads in chunks of `[blockSize()](#blocksize) * 128` bytes. Calls [initDecryptor()](#initdecryptor) internally; no prior initialization is required.

#### Parameters
* `source` Input stream containing ciphertext (in the given encoding). 

* `sink` Output stream to receive the decrypted plaintext. 

* `encoding` Transport encoding of the input data (default: Binary).

---

{#setkey-1}

#### setKey

`inline`

```cpp
template<typename T> inline void setKey(const T & key)
```

Sets the encryption key.

#### Parameters
* `key` Container whose size must exactly match [keySize()](#keysize). 

#### Exceptions
* `std::logic_error` if key.size() != [keySize()](#keysize).

---

{#setiv}

#### setIV

`inline`

```cpp
template<typename T> inline void setIV(const T & iv)
```

Sets the initialization vector (IV).

#### Parameters
* `iv` Container whose size must exactly match [ivSize()](#ivsize). 

#### Exceptions
* `std::logic_error` if iv.size() != [ivSize()](#ivsize).

---

{#setpadding}

#### setPadding

```cpp
int setPadding(int padding)
```

Enables or disables PKCS block padding.

By default, encryption pads input to a block boundary and decryption strips and validates the padding. If `padding` is zero, no padding is applied; the total data length must then be an exact multiple of [blockSize()](#blocksize) or the operation will fail.

See EVP_CIPHER_CTX_set_padding for further information.

#### Parameters
* `padding` Non-zero to enable padding (default), zero to disable. 

#### Returns
The return value from EVP_CIPHER_CTX_set_padding.

---

{#getkey}

#### getKey

`const`

```cpp
const ByteVec & getKey() const
```

Returns the raw encryption key bytes.

#### Returns
Reference to the internal key byte vector.

---

{#getiv}

#### getIV

`const`

```cpp
const ByteVec & getIV() const
```

Returns the raw initialization vector bytes.

#### Returns
Reference to the internal IV byte vector.

---

{#name-10}

#### name

`const`

```cpp
const std::string & name() const
```

Returns the OpenSSL cipher name this object was constructed with.

#### Returns
[Cipher](#cipher) name string (e.g. "aes-256-cbc").

---

{#blocksize}

#### blockSize

`const`

```cpp
int blockSize() const
```

Returns the cipher block size in bytes.

#### Returns
Block size as reported by EVP_CIPHER_block_size.

---

{#keysize}

#### keySize

`const`

```cpp
int keySize() const
```

Returns the required key length in bytes for this cipher.

#### Returns
Key length as reported by EVP_CIPHER_key_length.

---

{#ivsize}

#### ivSize

`const`

```cpp
int ivSize() const
```

Returns the required initialization vector length in bytes.

#### Returns
IV length as reported by EVP_CIPHER_iv_length.

---

{#cipher-5}

#### cipher

```cpp
const EVP_CIPHER * cipher()
```

Returns the underlying OpenSSL EVP_CIPHER object.

#### Returns
Pointer to the OpenSSL cipher descriptor; valid for the lifetime of this [Cipher](#cipher) object.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `bool` | [`_initialized`](#_initialized-2)  |  |
| `bool` | [`_encrypt`](#_encrypt)  |  |
| `const EVP_CIPHER *` | [`_cipher`](#_cipher)  |  |
| `EvpCipherCtxPtr` | [`_ctx`](#_ctx)  |  |
| `std::string` | [`_name`](#_name-2)  |  |
| `ByteVec` | [`_key`](#_key-2)  |  |
| `ByteVec` | [`_iv`](#_iv)  |  |

---

{#_initialized-2}

#### _initialized

```cpp
bool _initialized
```

---

{#_encrypt}

#### _encrypt

```cpp
bool _encrypt
```

---

{#_cipher}

#### _cipher

```cpp
const EVP_CIPHER * _cipher
```

---

{#_ctx}

#### _ctx

```cpp
EvpCipherCtxPtr _ctx
```

---

{#_name-2}

#### _name

```cpp
std::string _name
```

---

{#_key-2}

#### _key

```cpp
ByteVec _key
```

---

{#_iv}

#### _iv

```cpp
ByteVec _iv
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Cipher`](#cipher-6)  | Deleted constructor. |
|  | [`Cipher`](#cipher-7)  | Deleted constructor. |
|  | [`Cipher`](#cipher-8)  | Deleted constructor. |
| `void` | [`generateKey`](#generatekey)  | Derives and sets the key and IV from a passphrase using EVP_BytesToKey. |
| `void` | [`setRandomKey`](#setrandomkey)  | Fills the key buffer with cryptographically random bytes. |
| `void` | [`setRandomIV`](#setrandomiv)  | Fills the IV buffer with cryptographically random bytes. |
| `void` | [`init`](#init-9)  | Initializes or resets the OpenSSL cipher context for the given direction. |

---

{#cipher-6}

#### Cipher

```cpp
Cipher() = delete
```

Deleted constructor.

---

{#cipher-7}

#### Cipher

```cpp
Cipher(const Cipher &) = delete
```

Deleted constructor.

---

{#cipher-8}

#### Cipher

```cpp
Cipher(Cipher &&) = delete
```

Deleted constructor.

---

{#generatekey}

#### generateKey

```cpp
void generateKey(std::string_view passphrase, std::string_view salt, int iterationCount)
```

Derives and sets the key and IV from a passphrase using EVP_BytesToKey.

Uses SHA-256 as the digest. Salt values longer than 8 bytes are folded by XOR into an 8-byte array as required by OpenSSL.

#### Parameters
* `passphrase` Secret passphrase for key derivation. 

* `salt` Salt string (may be empty for no salt). 

* `iterationCount` Number of digest iterations.

---

{#setrandomkey}

#### setRandomKey

```cpp
void setRandomKey()
```

Fills the key buffer with cryptographically random bytes.

---

{#setrandomiv}

#### setRandomIV

```cpp
void setRandomIV()
```

Fills the IV buffer with cryptographically random bytes.

---

{#init-9}

#### init

```cpp
void init(bool encrypt)
```

Initializes or resets the OpenSSL cipher context for the given direction.

#### Parameters
* `encrypt` true to initialize for encryption, false for decryption.

### Public Types

| Name | Description |
|------|-------------|
| [`Encoding`](#encoding-1)  | Transport encoding to use for [encrypt()](#encrypt) and decrypt(). |

---

{#encoding-1}

#### Encoding

```cpp
enum Encoding
```

Transport encoding to use for [encrypt()](#encrypt) and decrypt().

| Value | Description |
|-------|-------------|
| `Binary` | Plain binary output. |
| `Base64` | Base64-encoded output. |
| `BinHex` | BinHex-encoded output. |
| `Base64_NoLF` | Base64-encoded output, no linefeeds. |
| `BinHex_NoLF` | BinHex-encoded output, no linefeeds. |

{#hash-3}

## Hash

```cpp
#include <icy/crypto/hash.h>
```

Incremental cryptographic hash engine wrapping OpenSSL EVP digest functions.

Construct with an algorithm name recognized by OpenSSL (e.g. "sha256", "md5"). Feed data with one or more calls to [update()](#update-9), then call [digest()](#digest) or [digestStr()](#digeststr) to finalize and retrieve the result. Call [reset()](#reset-13) to reuse the engine for a new computation without reallocating the context.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Hash`](#hash-4)  | Constructs a [Hash](#hash-3) engine for the given algorithm. |
|  | [`~Hash`](#hash-5)  | Destroys the [Hash](#hash-3) engine and releases OpenSSL resources. |
| `void` | [`update`](#update-9)  | Feeds a single character into the digest computation. |
| `void` | [`update`](#update-10)  | Feeds a string view into the digest computation. |
| `void` | [`update`](#update-11)  | Feeds a raw memory buffer into the digest computation. |
| `const ByteVec &` | [`digest`](#digest)  | Finalizes the digest computation and returns the raw binary result. |
| `std::string` | [`digestStr`](#digeststr)  | Finalizes the digest computation and returns the result as a raw binary string (not hex-encoded). Use [icy::hex::encode()](base.md#encode-14) on [digest()](#digest) if you need a printable representation. |
| `void` | [`reset`](#reset-13)  | Resets the digest context and clears the cached result, allowing the engine to be reused for a new computation with the same algorithm. |
| `const std::string &` | [`algorithm`](#algorithm) `const` | Returns the algorithm name this engine was constructed with. |

---

{#hash-4}

#### Hash

```cpp
Hash(const std::string & algorithm)
```

Constructs a [Hash](#hash-3) engine for the given algorithm.

#### Parameters
* `algorithm` OpenSSL digest name (e.g. "sha256", "sha1", "md5"). 

#### Exceptions
* `std::runtime_error` if the algorithm is not recognized by OpenSSL.

---

{#hash-5}

#### ~Hash

```cpp
~Hash()
```

Destroys the [Hash](#hash-3) engine and releases OpenSSL resources.

---

{#update-9}

#### update

```cpp
void update(char data)
```

Feeds a single character into the digest computation.

#### Parameters
* `data` The byte to hash.

---

{#update-10}

#### update

```cpp
void update(std::string_view data)
```

Feeds a string view into the digest computation.

#### Parameters
* `data` The data to hash.

---

{#update-11}

#### update

```cpp
void update(const void * data, size_t length)
```

Feeds a raw memory buffer into the digest computation.

This method may be called multiple times for streaming large inputs.

#### Parameters
* `data` Pointer to the input buffer. 

* `length` Number of bytes to hash from `data`.

---

{#digest}

#### digest

```cpp
const ByteVec & digest()
```

Finalizes the digest computation and returns the raw binary result.

The result is computed on the first call and cached; subsequent calls return the same value without recomputing. Call [reset()](#reset-13) before reusing the engine for a new computation.

#### Returns
Reference to the internal byte vector containing the digest.

---

{#digeststr}

#### digestStr

```cpp
std::string digestStr()
```

Finalizes the digest computation and returns the result as a raw binary string (not hex-encoded). Use [icy::hex::encode()](base.md#encode-14) on [digest()](#digest) if you need a printable representation.

#### Returns
Binary digest as a std::string.

---

{#reset-13}

#### reset

```cpp
void reset()
```

Resets the digest context and clears the cached result, allowing the engine to be reused for a new computation with the same algorithm.

---

{#algorithm}

#### algorithm

`const`

```cpp
const std::string & algorithm() const
```

Returns the algorithm name this engine was constructed with.

#### Returns
OpenSSL digest name string (e.g. "sha256").

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `EvpMdCtxPtr` | [`_ctx`](#_ctx-1)  |  |
| `const EVP_MD *` | [`_md`](#_md)  |  |
| `crypto::ByteVec` | [`_digest`](#_digest)  |  |
| `std::string` | [`_algorithm`](#_algorithm)  |  |

---

{#_ctx-1}

#### _ctx

```cpp
EvpMdCtxPtr _ctx
```

---

{#_md}

#### _md

```cpp
const EVP_MD * _md
```

---

{#_digest}

#### _digest

```cpp
crypto::ByteVec _digest
```

---

{#_algorithm}

#### _algorithm

```cpp
std::string _algorithm
```

{#x509certificate}

## X509Certificate

```cpp
#include <icy/crypto/x509certificate.h>
```

RAII wrapper for an OpenSSL X509 certificate with PEM loading and inspection.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`X509Certificate`](#x509certificate-1) `explicit` | Constructs an [X509Certificate](#x509certificate) by parsing a PEM-encoded certificate from memory. |
|  | [`X509Certificate`](#x509certificate-2) `explicit` | Constructs an [X509Certificate](#x509certificate) by reading a PEM-encoded certificate from a file. |
|  | [`X509Certificate`](#x509certificate-3) `explicit` | Constructs an [X509Certificate](#x509certificate) taking ownership of an existing OpenSSL X509 object. |
|  | [`X509Certificate`](#x509certificate-4)  | Constructs an [X509Certificate](#x509certificate) from an existing OpenSSL X509 object, optionally sharing ownership via reference count increment. |
|  | [`X509Certificate`](#x509certificate-5)  | Copy-constructs an [X509Certificate](#x509certificate) by duplicating the underlying X509 object. |
|  | [`X509Certificate`](#x509certificate-6)  | Move-constructs an [X509Certificate](#x509certificate), transferring ownership from `cert`. |
| `X509Certificate &` | [`operator=`](#operator-25)  | Copy-assigns a certificate, duplicating the underlying X509 object. |
| `X509Certificate &` | [`operator=`](#operator-26)  | Move-assigns a certificate, transferring ownership from `cert`. |
| `void` | [`swap`](#swap-6)  | Swaps this certificate with `cert`. |
|  | [`~X509Certificate`](#x509certificate-7)  | Destroys the [X509Certificate](#x509certificate) and releases the underlying OpenSSL X509 object. |
| `const std::string &` | [`issuerName`](#issuername) `const` | Returns the full distinguished name of the certificate issuer. |
| `std::string` | [`issuerName`](#issuername-1) `const` | Extracts a single field from the certificate issuer's distinguished name. |
| `const std::string &` | [`subjectName`](#subjectname) `const` | Returns the full distinguished name of the certificate subject. |
| `std::string` | [`subjectName`](#subjectname-1) `const` | Extracts a single field from the certificate subject's distinguished name. |
| `std::string` | [`commonName`](#commonname) `const` | Returns the common name (CN) from the certificate subject. |
| `void` | [`extractNames`](#extractnames) `const` | Extracts the common name and the set of Subject Alternative Name (SAN) DNS entries from the certificate. |
| `DateTime` | [`validFrom`](#validfrom) `const` | Returns the date and time from which the certificate is valid. |
| `DateTime` | [`expiresOn`](#expireson) `const` | Returns the date and time at which the certificate expires. |
| `void` | [`save`](#save-1) `const` | Writes the certificate in PEM format to an output stream. |
| `void` | [`save`](#save-2) `const` | Writes the certificate in PEM format to a file. |
| `bool` | [`issuedBy`](#issuedby) `const` | Verifies whether this certificate was signed by the given issuer. |
| `const X509 *` | [`certificate`](#certificate-1) `const` | Returns a const pointer to the underlying OpenSSL X509 object. |
| `X509 *` | [`certificate`](#certificate-2)  | Returns a mutable pointer to the underlying OpenSSL X509 object. |

---

{#x509certificate-1}

#### X509Certificate

`explicit`

```cpp
explicit X509Certificate(const char * data, size_t length)
```

Constructs an [X509Certificate](#x509certificate) by parsing a PEM-encoded certificate from memory.

#### Parameters
* `data` Pointer to a buffer containing the PEM-encoded certificate. 

* `length` Number of bytes in `data`. 

#### Exceptions
* `std::runtime_error` if the BIO cannot be created or PEM parsing fails.

---

{#x509certificate-2}

#### X509Certificate

`explicit`

```cpp
explicit X509Certificate(const std::string & path)
```

Constructs an [X509Certificate](#x509certificate) by reading a PEM-encoded certificate from a file.

#### Parameters
* `path` Filesystem path to the PEM certificate file. 

#### Exceptions
* `std::runtime_error` if the file cannot be opened or PEM parsing fails.

---

{#x509certificate-3}

#### X509Certificate

`explicit`

```cpp
explicit X509Certificate(X509 * pCert)
```

Constructs an [X509Certificate](#x509certificate) taking ownership of an existing OpenSSL X509 object.

#### Parameters
* `pCert` Non-null pointer to an OpenSSL X509 certificate. This object takes ownership and will call X509_free on destruction. 

#### Exceptions
* `std::runtime_error` if `pCert` is null.

---

{#x509certificate-4}

#### X509Certificate

```cpp
X509Certificate(X509 * pCert, bool shared)
```

Constructs an [X509Certificate](#x509certificate) from an existing OpenSSL X509 object, optionally sharing ownership via reference count increment.

#### Parameters
* `pCert` Non-null pointer to an OpenSSL X509 certificate. Ownership is always taken (X509_free called on destruction). 

* `shared` If true, increments the certificate's reference count via X509_up_ref before taking ownership, so the original pointer remains valid after this object is destroyed. 

#### Exceptions
* `std::runtime_error` if `pCert` is null.

---

{#x509certificate-5}

#### X509Certificate

```cpp
X509Certificate(const X509Certificate & cert)
```

Copy-constructs an [X509Certificate](#x509certificate) by duplicating the underlying X509 object.

#### Parameters
* `cert` The certificate to copy.

---

{#x509certificate-6}

#### X509Certificate

```cpp
X509Certificate(X509Certificate && cert) noexcept
```

Move-constructs an [X509Certificate](#x509certificate), transferring ownership from `cert`.

#### Parameters
* `cert` The certificate to move from; left in a valid but empty state.

---

{#operator-25}

#### operator=

```cpp
X509Certificate & operator=(const X509Certificate & cert)
```

Copy-assigns a certificate, duplicating the underlying X509 object.

#### Parameters
* `cert` The certificate to copy. 

#### Returns
Reference to this object.

---

{#operator-26}

#### operator=

```cpp
X509Certificate & operator=(X509Certificate && cert) noexcept
```

Move-assigns a certificate, transferring ownership from `cert`.

#### Parameters
* `cert` The certificate to move from; left in a valid but empty state. 

#### Returns
Reference to this object.

---

{#swap-6}

#### swap

```cpp
void swap(X509Certificate & cert)
```

Swaps this certificate with `cert`.

#### Parameters
* `cert` The certificate to swap with.

---

{#x509certificate-7}

#### ~X509Certificate

```cpp
~X509Certificate()
```

Destroys the [X509Certificate](#x509certificate) and releases the underlying OpenSSL X509 object.

---

{#issuername}

#### issuerName

`const`

```cpp
const std::string & issuerName() const
```

Returns the full distinguished name of the certificate issuer.

#### Returns
One-line string representation produced by X509_NAME_oneline.

---

{#issuername-1}

#### issuerName

`const`

```cpp
std::string issuerName(NID nid) const
```

Extracts a single field from the certificate issuer's distinguished name.

#### Parameters
* `nid` The field to extract (e.g. NID_COMMON_NAME). 

#### Returns
Field value, or an empty string if the field is absent.

---

{#subjectname}

#### subjectName

`const`

```cpp
const std::string & subjectName() const
```

Returns the full distinguished name of the certificate subject.

#### Returns
One-line string representation produced by X509_NAME_oneline.

---

{#subjectname-1}

#### subjectName

`const`

```cpp
std::string subjectName(NID nid) const
```

Extracts a single field from the certificate subject's distinguished name.

#### Parameters
* `nid` The field to extract (e.g. NID_ORGANIZATION_NAME). 

#### Returns
Field value, or an empty string if the field is absent.

---

{#commonname}

#### commonName

`const`

```cpp
std::string commonName() const
```

Returns the common name (CN) from the certificate subject.

Convenience wrapper for subjectName(NID_COMMON_NAME).

#### Returns
Common name string, or empty if absent.

---

{#extractnames}

#### extractNames

`const`

```cpp
void extractNames(std::string & commonName, std::set< std::string > & domainNames) const
```

Extracts the common name and the set of Subject Alternative Name (SAN) DNS entries from the certificate.

If no SAN DNS entries are present and the common name is non-empty, the common name is added to `domainNames` as a fallback.

#### Parameters
* `commonName` Receives the certificate's common name. 

* `domainNames` Receives all DNS SAN entries (cleared before population).

---

{#validfrom}

#### validFrom

`const`

```cpp
DateTime validFrom() const
```

Returns the date and time from which the certificate is valid.

Parsed from the X509 notBefore field.

#### Returns
UTC [DateTime](base.md#datetime) representing the start of the validity period.

---

{#expireson}

#### expiresOn

`const`

```cpp
DateTime expiresOn() const
```

Returns the date and time at which the certificate expires.

Parsed from the X509 notAfter field.

#### Returns
UTC [DateTime](base.md#datetime) representing the end of the validity period.

---

{#save-1}

#### save

`const`

```cpp
void save(std::ostream & stream) const
```

Writes the certificate in PEM format to an output stream.

#### Parameters
* `stream` Destination stream to write to. 

#### Exceptions
* `std::runtime_error` if the BIO cannot be created or write fails.

---

{#save-2}

#### save

`const`

```cpp
void save(const std::string & path) const
```

Writes the certificate in PEM format to a file.

#### Parameters
* `path` Filesystem path of the output file (created or truncated). 

#### Exceptions
* `std::runtime_error` if the file cannot be opened or write fails.

---

{#issuedby}

#### issuedBy

`const`

```cpp
bool issuedBy(const X509Certificate & issuerCertificate) const
```

Verifies whether this certificate was signed by the given issuer.

Extracts the public key from `issuerCertificate` and calls X509_verify. Use this to validate links in a certificate chain.

#### Parameters
* `issuerCertificate` The certificate of the purported issuer. 

#### Returns
true if this certificate's signature verifies against the issuer's public key, false otherwise. 

#### Exceptions
* `std::invalid_argument` if the issuer certificate has no public key.

---

{#certificate-1}

#### certificate

`const`

```cpp
const X509 * certificate() const
```

Returns a const pointer to the underlying OpenSSL X509 object.

#### Returns
Pointer valid for the lifetime of this [X509Certificate](#x509certificate).

---

{#certificate-2}

#### certificate

```cpp
X509 * certificate()
```

Returns a mutable pointer to the underlying OpenSSL X509 object.

#### Returns
Pointer valid for the lifetime of this [X509Certificate](#x509certificate).

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`load`](#load-2)  | Parses a PEM-encoded certificate from a memory buffer and stores it. |
| `void` | [`load`](#load-3)  | Reads a PEM-encoded certificate from a file and stores it. |
| `void` | [`init`](#init-10)  | Populates _issuerName and _subjectName from the loaded certificate. |

---

{#load-2}

#### load

```cpp
void load(const char * data, size_t length)
```

Parses a PEM-encoded certificate from a memory buffer and stores it.

#### Parameters
* `data` Pointer to PEM data. 

* `length` Number of bytes in `data`. 

#### Exceptions
* `std::logic_error` if a certificate is already loaded. 

* `std::runtime_error` if BIO creation or PEM parsing fails.

---

{#load-3}

#### load

```cpp
void load(const std::string & path)
```

Reads a PEM-encoded certificate from a file and stores it.

#### Parameters
* `path` Filesystem path to the PEM certificate file. 

#### Exceptions
* `std::logic_error` if a certificate is already loaded. 

* `std::runtime_error` if the file cannot be opened or PEM parsing fails.

---

{#init-10}

#### init

```cpp
void init()
```

Populates _issuerName and _subjectName from the loaded certificate.

Called after each successful load or construction from an X509 pointer.

### Public Types

| Name | Description |
|------|-------------|
| [`NID`](#nid)  | Name identifier for extracting fields from a certificate's distinguished name. |

---

{#nid}

#### NID

```cpp
enum NID
```

Name identifier for extracting fields from a certificate's distinguished name.

Values correspond to OpenSSL NID constants used with X509_NAME_get_text_by_NID.

| Value | Description |
|-------|-------------|
| `NID_COMMON_NAME` | Common name (CN field). |
| `NID_COUNTRY` | Country code (C field). |
| `NID_LOCALITY_NAME` | Locality / city (L field). |
| `NID_STATE_OR_PROVINCE` | [State](base.md#state) or province (ST field). |
| `NID_ORGANIZATION_NAME` | Organization name (O field). |
| `NID_ORGANIZATION_UNIT_NAME` | Organizational unit (OU field). |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_issuerName`](#_issuername)  |  |
| `std::string` | [`_subjectName`](#_subjectname)  |  |
| `X509Ptr` | [`_certificate`](#_certificate)  |  |

---

{#_issuername}

#### _issuerName

```cpp
std::string _issuerName
```

---

{#_subjectname}

#### _subjectName

```cpp
std::string _subjectName
```

---

{#_certificate}

#### _certificate

```cpp
X509Ptr _certificate
```

