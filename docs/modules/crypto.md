# Crypto

> OpenSSL 3.x EVP cryptography wrapped in a clean, RAII C++ interface; hashing, HMAC, symmetric encryption, X.509 certificates, and RSA keys.

**[API Reference →](../api/crypto.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/crypto/)**

## Overview

The `crypto` module is the cryptographic foundation of icey. Every other module that needs cryptography depends on it; `net` uses it for TLS certificates, `stun` uses it for HMAC-SHA1 message integrity, and `turn` inherits both through `stun`.

All types live in the `icy::crypto` namespace. Headers are under `icy/crypto/`. Link against the `icey::crypto` CMake target:

```cmake
target_link_libraries(my_target PRIVATE icey::crypto)
```

The module wraps OpenSSL's EVP layer exclusively. We do not call legacy API functions (no `SHA1()`, no `MD5()`, no `EVP_DigestInit()` without context). Every algorithm is resolved at runtime via `EVP_get_digestbyname` / `EVP_get_cipherbyname`, which means any algorithm the installed OpenSSL supports is immediately available.

**What the module provides:**

| Facility | Class / Function | Header |
| --- | --- | --- |
| Incremental hashing | `Hash` | `icy/crypto/hash.h` |
| One-shot hashing | `hash()`, `checksum()` | `icy/crypto/hash.h` |
| HMAC-SHA1 | `computeHMAC()` | `icy/crypto/hmac.h` |
| Symmetric encryption | `Cipher` | `icy/crypto/cipher.h` |
| X.509 certificates | `X509Certificate` | `icy/crypto/x509certificate.h` |
| RSA key alias | `RSAKey` | `icy/crypto/rsa.h` |
| Engine init | `initializeEngine()` / `uninitializeEngine()` | `icy/crypto/crypto.h` |
| Buffer adapter | `internal::Raw<T>` | `icy/crypto/crypto.h` |

## Architecture

### OpenSSL version strategy

The module targets OpenSSL 3.x. All initialization is automatic and thread-safe. The old patterns (explicit `SSL_library_init`, `CRYPTO_set_locking_callback`, `ERR_free_strings`, `EVP_cleanup`) are gone. We keep a ref-counted init/uninit pair solely to seed the PRNG on first use.

### Class relationships

```text
crypto::initializeEngine()         (ref-counted, seeds PRNG)
        │
        ├── Hash          wraps EVP_MD_CTX; holds EVP_MD* algorithm descriptor
        │
        ├── Cipher        wraps EVP_CIPHER_CTX; holds EVP_CIPHER* and key/IV bytes
        │                 ├── encryptString / decryptString  (string in, string out)
        │                 ├── encryptStream / decryptStream  (istream -> ostream)
        │                 └── update / final                 (manual, low-level)
        │
        ├── X509Certificate  owns X509* via unique_ptr<X509, X509_free>
        │                    ├── load(file) / load(buffer)
        │                    ├── issuerName, subjectName, commonName
        │                    ├── extractNames (CN + SAN)
        │                    ├── validFrom / expiresOn
        │                    ├── issuedBy (chain verification)
        │                    └── save (to file or stream)
        │
        └── RSAKey        transparent alias for ::RSA; raw OpenSSL RSA_* API
```

### EVP wrapping strategy

`Hash` allocates one `EVP_MD_CTX` per instance using `EVP_MD_CTX_new` and stores the algorithm descriptor as `const EVP_MD*`. `update()` calls `EVP_DigestUpdate`; `digest()` calls `EVP_DigestFinal_ex` once and caches the result in `_digest`. `reset()` calls `EVP_MD_CTX_reset` then `EVP_DigestInit_ex` to reuse the context without reallocating.

`Cipher` allocates one `EVP_CIPHER_CTX` and retains the `const EVP_CIPHER*` descriptor. Direction is set by `EVP_CipherInit_ex`'s encrypt flag (1 = encrypt, 0 = decrypt). `encryptStream` / `decryptStream` compose the low-level `update` + `final` calls with optional `base64` or `hex` encoding layers from `icy::basic::Encoder`.

`X509Certificate` uses `std::unique_ptr<X509, decltype(&X509_free)>` for ownership. Copy uses `X509_dup`; the shared-ownership constructor calls `X509_up_ref` before taking the pointer, keeping the original valid.

### Error handling

All failure paths throw standard exceptions; there are no return-code APIs. The `internal::api(int ret)` helper checks OpenSSL return values and throws on zero:

```cpp
// From crypto.cpp
void throwError()
{
    unsigned long err;
    std::string msg;
    while ((err = ERR_get_error())) {
        if (!msg.empty())
            msg.append("; ");
        msg.append(ERR_error_string(err, nullptr));
    }
    throw std::runtime_error(msg);
}

void api(int ret, const char* error = nullptr)
{
    if (ret == 0) {
        if (error) throw std::runtime_error(error);
        else throwError();
    }
}
```

| Exception | Trigger |
| --- | --- |
| `std::runtime_error` | OpenSSL API failure; message contains the drained OpenSSL error queue |
| `std::invalid_argument` | Unrecognized cipher name, unrecognized encoding value |
| `std::logic_error` | Key or IV size mismatch in `setKey()` / `setIV()`; certificate already loaded |

## Usage

### Engine initialization

Call `initializeEngine()` at application startup before using any crypto type. Each class constructor calls it internally, but an explicit call is good practice:

```cpp
#include "icy/crypto/crypto.h"

int main()
{
    icy::crypto::initializeEngine();

    // ... all crypto operations ...

    icy::crypto::uninitializeEngine();
}
```

`initializeEngine` / `uninitializeEngine` are ref-counted and mutex-protected. Each call to `initializeEngine` must be matched by a call to `uninitializeEngine`. The first `initializeEngine` call seeds the PRNG with 256 bytes from `icy::Random::getSeed`. On OpenSSL 3.x, the heavy initialization (algorithm registration, provider loading) is automatic; we only manage the PRNG seed.

### Hashing

Header: `icy/crypto/hash.h`

`Hash` is an incremental digest engine. Construct it with any algorithm name OpenSSL recognizes, feed data with one or more `update()` calls, then call `digest()` or `digestStr()` to finalize.

```cpp
#include "icy/crypto/hash.h"
#include "icy/hex.h"

// Incremental hashing
icy::crypto::Hash h("sha256");
h.update("hello, ");
h.update("world");

// digest() finalizes on the first call and caches the result.
// Subsequent calls return the same value.
const icy::crypto::ByteVec& raw = h.digest();
std::string hex = icy::hex::encode(raw); // lowercase hex

// Reuse the same engine for a new computation
h.reset();
h.update(someBuffer, someLength);
std::string bin = h.digestStr(); // raw binary bytes, not hex
```

`ByteVec` is `std::vector<unsigned char>`. The `digest()` result is valid until `reset()` is called. `digestStr()` reinterprets the same bytes as `std::string`; pass through `icy::hex::encode()` when you need a printable representation.

`update()` has three overloads:

```cpp
void update(char data);                        // single byte
void update(std::string_view data);            // string view
void update(const void* data, size_t length);  // raw buffer
```

Incremental updates are composable; hashing in chunks produces the same result as hashing all data at once:

```cpp
// These two are equivalent
icy::crypto::Hash h1("sha256");
h1.update("hello world");

icy::crypto::Hash h2("sha256");
h2.update("hello ");
h2.update("world");

assert(icy::hex::encode(h1.digest()) == icy::hex::encode(h2.digest()));
```

#### Supported algorithms

Any algorithm registered with the installed OpenSSL works. Common choices:

| Name | Output | Notes |
| --- | --- | --- |
| `"sha256"` | 32 bytes | Recommended default |
| `"sha512"` | 64 bytes | |
| `"sha1"` | 20 bytes | Used by STUN MESSAGE-INTEGRITY |
| `"md5"` | 16 bytes | Legacy; avoid for security-sensitive contexts |

Algorithm names are case-insensitive; OpenSSL accepts `"SHA256"` and `"sha256"` identically. An unrecognized name throws `std::runtime_error` at construction.

#### One-shot hash free functions

Two `hash()` overloads cover the single-call pattern and return lowercase hex directly:

```cpp
#include "icy/crypto/hash.h"

// Hash a string
std::string digest = icy::crypto::hash("sha256", "some data");

// Hash a raw buffer
std::string digest = icy::crypto::hash("sha256", bufPtr, bufLen);
```

#### File checksum

`checksum()` reads a file in 4096-byte chunks and returns a hex-encoded digest. This is suitable for large files that should not be read into memory all at once:

```cpp
#include "icy/crypto/hash.h"

std::string sha256 = icy::crypto::checksum("sha256", "/path/to/artifact.bin");
std::string md5    = icy::crypto::checksum("md5",    "/path/to/artifact.bin");
```

Throws `std::runtime_error` if the file cannot be opened. The hex digest length matches the algorithm output: 40 chars for SHA-1, 64 for SHA-256, 128 for SHA-512.

#### FIPS test vectors

The test suite verifies against published FIPS 180-1 / RFC 1321 / FIPS 180-2 vectors:

```cpp
// SHA-1 (FIPS 180-1)
icy::crypto::Hash h("SHA1");
h.update("abc", 3);
assert(icy::hex::encode(h.digest()) == "a9993e364706816aba3e25717850c26c9cd0d89d");

// SHA-256 (FIPS 180-2)
icy::crypto::Hash h256("SHA256");
h256.update("abc", 3);
assert(icy::hex::encode(h256.digest()) ==
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
```

### HMAC

Header: `icy/crypto/hmac.h`

`computeHMAC` computes an HMAC-SHA1 message authentication code and returns a 20-byte raw binary string (not hex-encoded):

```cpp
#include "icy/crypto/hmac.h"

std::string mac = icy::crypto::computeHMAC(message, secretKey);
// mac.size() == 20, raw binary
```

The function uses `HMAC(EVP_sha1(), ...)` directly. If OpenSSL returns a digest length other than 20, it throws `std::runtime_error`. Encode the result with `icy::hex::encode()` or `icy::base64::encode()` when transmitting as text.

HMAC-SHA1 is fixed at SHA-1 for the `crypto::computeHMAC` function; this matches the RFC 5389 STUN requirement. If we need HMAC with a different digest (e.g., HMAC-SHA256), use OpenSSL's EVP HMAC API directly, as shown in the tests:

```cpp
#include <openssl/hmac.h>
#include "icy/hex.h"

// HMAC with any algorithm via OpenSSL EVP HMAC API
std::string computeHMACGeneric(const std::string& algorithm,
                               const std::string& key,
                               const std::string& data)
{
    const EVP_MD* md = EVP_get_digestbyname(algorithm.c_str());
    if (!md)
        throw std::runtime_error("HMAC algorithm not supported: " + algorithm);

    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(md, key.data(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(data.data()),
         data.size(), result, &len);

    return icy::hex::encode(std::vector<unsigned char>(result, result + len));
}
```

### Symmetric Encryption (Cipher)

Header: `icy/crypto/cipher.h`

`Cipher` provides symmetric encryption and decryption via `EVP_CipherInit_ex` / `EVP_CipherUpdate` / `EVP_CipherFinal_ex`. It is non-copyable and non-movable; construction is the only way to set the algorithm.

#### Construction

Three constructors cover the common cases:

```cpp
#include "icy/crypto/cipher.h"

// 1. Random key and IV generated internally.
//    Use getKey() / getIV() to retrieve them for the other party.
icy::crypto::Cipher c1("aes-256-cbc");
const icy::crypto::ByteVec& key = c1.getKey(); // 32 bytes for aes-256
const icy::crypto::ByteVec& iv  = c1.getIV();  // 16 bytes for CBC

// 2. Explicit key and IV (both must match the cipher's required sizes).
icy::crypto::Cipher c2("aes-256-cbc", key, iv);

// 3. Passphrase-derived key and IV via EVP_BytesToKey with SHA-256.
//    Salt values longer than 8 bytes are folded by XOR into 8 bytes.
icy::crypto::Cipher c3("aes-256-cbc", "my passphrase", "saltsalt", 2000);
```

The cipher name must be recognized by `EVP_get_cipherbyname`. An unrecognized name throws `std::invalid_argument`. For AES-256-CBC the key is 32 bytes and the IV is 16 bytes; `keySize()` and `ivSize()` return the correct values for any algorithm.

#### String encryption and decryption

The simplest path for in-memory data:

```cpp
icy::crypto::Cipher c("aes-256-cbc");

std::string ciphertext = c.encryptString(plaintext);
std::string recovered  = c.decryptString(ciphertext);

assert(plaintext == recovered);
```

`encryptString` / `decryptString` call `initEncryptor()` / `initDecryptor()` internally, so the same `Cipher` object can be used repeatedly for independent messages.

#### Transport encoding

Both `encryptString` and `decryptString` accept an optional `Encoding` parameter:

```cpp
// Base64 output with no line feeds; useful for JSON, HTTP headers, etc.
std::string b64 = c.encryptString(plaintext, icy::crypto::Cipher::Base64_NoLF);
std::string back = c.decryptString(b64, icy::crypto::Cipher::Base64_NoLF);
```

| Constant | Value | Meaning |
| --- | --- | --- |
| `Cipher::Binary` | `0x00` | Raw bytes (default) |
| `Cipher::Base64` | `0x01` | Base64 with line feeds |
| `Cipher::Base64_NoLF` | `0x81` | Base64, no line feeds |
| `Cipher::BinHex` | `0x02` | Hex-encoded with line feeds |
| `Cipher::BinHex_NoLF` | `0x82` | Hex-encoded, no line feeds |

The encoding and decoding layers are `icy::base64::Encoder` / `icy::hex::Encoder` from the `base` module. Decryption automatically reverses whatever encoding was used to produce the ciphertext.

#### Stream encryption

For data that is too large to hold in memory, or when reading from / writing to files or network sockets, use the stream methods:

```cpp
#include <fstream>
#include "icy/crypto/cipher.h"

icy::crypto::Cipher c("aes-256-cbc");

// Encrypt a file to another file
{
    std::ifstream src("input.bin", std::ios::binary);
    std::ofstream dst("output.enc", std::ios::binary);
    c.encryptStream(src, dst);
}

// Decrypt it back
{
    std::ifstream enc("output.enc", std::ios::binary);
    std::ofstream dec("output.bin", std::ios::binary);
    c.decryptStream(enc, dec);
}
```

Stream methods read in chunks of `blockSize() * 128` bytes. They call `initEncryptor()` / `initDecryptor()` internally, so no prior setup is needed. The encoding parameter is available here as well:

```cpp
// Stream to/from Base64
c.encryptStream(src, dst, icy::crypto::Cipher::Base64_NoLF);
c.decryptStream(enc, dec, icy::crypto::Cipher::Base64_NoLF);
```

#### Low-level manual streaming

When we need direct control over buffer allocation, initialize the context, feed data in chunks, then finalize:

```cpp
icy::crypto::Cipher c("aes-256-cbc");
c.initEncryptor();

// Output buffer for update() must be at least inputLength + blockSize() - 1 bytes.
std::string input = "plaintext data";
std::string output(input.size() + c.blockSize(), '\0');

ssize_t n   = c.update(input, output);   // internal::Raw<> adapts string to unsigned char*
ssize_t fin = c.final(output);           // flush trailing block; at least blockSize() bytes
output.resize(n + fin);
```

The template overloads of `update()` and `final()` accept `std::string`, `ByteVec`, `std::vector<char>`, or raw pointer-length pairs through `internal::Raw<T>` (see Raw Memory Adapter below).

#### Free functions

Single-call encrypt/decrypt without managing a `Cipher` object:

```cpp
#include "icy/crypto/cipher.h"

std::string key(32, '\xAB'); // 32 bytes for aes-256
std::string iv(16,  '\xCD'); // 16 bytes for CBC

std::string ct = icy::crypto::encryptString("aes-256-cbc", plaintext, key, iv);
std::string pt = icy::crypto::decryptString("aes-256-cbc", ct, key, iv);

// Pass empty containers to generate random key / IV internally
std::string ct2 = icy::crypto::encryptString("aes-256-cbc", plaintext,
                                              icy::crypto::ByteVec{},
                                              icy::crypto::ByteVec{});
```

The template parameters `K` and `I` accept any type compatible with `internal::Raw`.

#### Key and IV management

After construction we can inspect or replace the key and IV:

```cpp
icy::crypto::Cipher c("aes-256-cbc");

// Inspect
const icy::crypto::ByteVec& key = c.getKey();  // 32 bytes
const icy::crypto::ByteVec& iv  = c.getIV();   // 16 bytes
int blockSz = c.blockSize();                    // 16 for AES
int keyLen  = c.keySize();                      // 32 for AES-256
int ivLen   = c.ivSize();                       // 16 for CBC

// Replace; sizes must match exactly or std::logic_error is thrown
c.setKey(newKeyBytes);
c.setIV(newIvBytes);
```

#### PKCS padding

PKCS#7 padding is enabled by default. Disable it when the input is already block-aligned:

```cpp
c.setPadding(0); // disable; input must be a multiple of blockSize()
c.setPadding(1); // re-enable (default)
```

Disabling padding and supplying non-aligned input will cause `EVP_CipherFinal_ex` to fail, which throws `std::runtime_error`.

### X.509 Certificates

Header: `icy/crypto/x509certificate.h`

`X509Certificate` is a RAII wrapper around OpenSSL's `X509*`. It calls `X509_free` on destruction. Copy (via `X509_dup`) and move are both supported.

#### Loading

```cpp
#include "icy/crypto/x509certificate.h"

// From a PEM file on disk
icy::crypto::X509Certificate cert("/path/to/server.pem");

// From a PEM buffer in memory (e.g., received over a network connection)
icy::crypto::X509Certificate cert(pemData.data(), pemData.size());

// Taking exclusive ownership of a raw X509* (no ref-count increment)
icy::crypto::X509Certificate cert(rawX509Ptr);

// Shared ownership: increments the reference count so the original
// pointer remains valid after this object is destroyed
icy::crypto::X509Certificate cert(rawX509Ptr, /*shared=*/true);
```

All constructors throw `std::runtime_error` on failure (BIO creation failure, PEM parse error, null pointer). Calling `load()` on an already-loaded certificate throws `std::logic_error`.

#### Inspecting distinguished names

```cpp
// Full one-line DN strings (X509_NAME_oneline format)
const std::string& issuer  = cert.issuerName();   // e.g. "/C=US/O=Let's Encrypt/CN=R3"
const std::string& subject = cert.subjectName();  // e.g. "/CN=example.com"

// Individual DN fields by NID
std::string cn      = cert.commonName();
std::string country = cert.issuerName(icy::crypto::X509Certificate::NID_COUNTRY);
std::string org     = cert.subjectName(icy::crypto::X509Certificate::NID_ORGANIZATION_NAME);
std::string state   = cert.subjectName(icy::crypto::X509Certificate::NID_STATE_OR_PROVINCE);
std::string city    = cert.subjectName(icy::crypto::X509Certificate::NID_LOCALITY_NAME);
std::string ou      = cert.subjectName(icy::crypto::X509Certificate::NID_ORGANIZATION_UNIT_NAME);
```

Available NID constants and their OpenSSL equivalents:

| Constant | NID value | DN field |
| --- | --- | --- |
| `NID_COMMON_NAME` | 13 | `CN` |
| `NID_COUNTRY` | 14 | `C` |
| `NID_LOCALITY_NAME` | 15 | `L` |
| `NID_STATE_OR_PROVINCE` | 16 | `ST` |
| `NID_ORGANIZATION_NAME` | 17 | `O` |
| `NID_ORGANIZATION_UNIT_NAME` | 18 | `OU` |

#### Subject Alternative Names

`extractNames` populates the CN and all DNS SAN entries. If no SAN entries are present, the CN is inserted into `domainNames` as a fallback (the browser-compatible behavior):

```cpp
std::string cn;
std::set<std::string> domains;
cert.extractNames(cn, domains);
// domains: {"example.com", "www.example.com", "api.example.com", ...}
```

This is what we use in `net::SSLManager` to match a peer certificate against an expected hostname.

#### Validity period

```cpp
icy::DateTime from  = cert.validFrom();
icy::DateTime until = cert.expiresOn();

if (until < icy::DateTime::now())
    throw std::runtime_error("Certificate has expired");
```

Both parse the ASN.1 `notBefore` / `notAfter` fields with `DateTimeParser::parse("%y%m%d%H%M%S", ...)` and return `icy::DateTime` values in UTC.

#### Chain verification

```cpp
icy::crypto::X509Certificate leaf("/path/to/leaf.pem");
icy::crypto::X509Certificate ca("/path/to/ca.pem");

if (leaf.issuedBy(ca)) {
    // ca's public key verifies the leaf's signature
}
```

`issuedBy` calls `X509_verify` with the issuer's public key extracted via `X509_get_pubkey`. It throws `std::invalid_argument` if the issuer certificate has no public key. Note that `issuedBy` validates the cryptographic signature only; it does not check expiry, key usage, or name constraints. Full path validation uses OpenSSL's `X509_verify_cert`, which is what `net::SSLContext` does internally.

#### Saving

```cpp
// To a file
cert.save("/path/to/output.pem");

// To any std::ostream (string stream, network stream, etc.)
std::ostringstream oss;
cert.save(oss);
std::string pem = oss.str();
// pem starts with "-----BEGIN CERTIFICATE-----"
```

#### Copy and move semantics

```cpp
// Copy: calls X509_dup internally; both objects own independent X509*
icy::crypto::X509Certificate copy(cert);
icy::crypto::X509Certificate assigned = cert;

// Move: transfers ownership; source is left in a valid but empty state
icy::crypto::X509Certificate moved(std::move(cert));
```

### RSA Keys

Header: `icy/crypto/rsa.h`

`RSAKey` is a transparent alias for the OpenSSL `::RSA` struct, brought into `icy::crypto`:

```cpp
namespace icy {
namespace crypto {
    using RSAKey = ::RSA;
}
}
```

We use this alias as a stable forward-declaration point across the codebase. Today it appears in `net::SSLContext` (the commented-out `usePrivateKey` method). Key generation, population, and freeing use the OpenSSL `RSA_*` family directly:

```cpp
#include "icy/crypto/rsa.h"
#include <openssl/rsa.h>
#include <openssl/bn.h>

// Generate a 2048-bit RSA key pair
icy::crypto::RSAKey* key = RSA_new();
BIGNUM* e = BN_new();
BN_set_word(e, RSA_F4);  // public exponent 65537
RSA_generate_key_ex(key, 2048, e, nullptr);
BN_free(e);

// ... use the key with SSL_CTX_use_RSAPrivateKey etc. ...

RSA_free(key);
```

A higher-level RAII `RSAKey` wrapper that manages lifetime automatically is planned for a future version.

### Raw Memory Adapter

`internal::Raw<T>` is a type-erasing buffer view defined in `icy/crypto/crypto.h`. It lets `Cipher`'s template methods accept `std::string`, `ByteVec`, `std::vector<char>`, or raw pointer-length pairs without an overload for every combination.

```cpp
template <typename T>
struct Raw {
    T      ptr;
    size_t len;

    Raw(T ptr, size_t len);
    Raw(const char* ptr, size_t len);
    Raw(std::string& str);
    Raw(const std::string& str);
    Raw(const std::vector<char>& vec);
    Raw(const ByteVec& vec);
};
```

`Raw` uses `const_cast` internally. C++11 guarantees `std::string` contiguity, so the string constructors are safe. Callers must ensure the underlying data is not modified through a `const T*` pointer. This is an internal type; we do not construct it directly.

In practice it means we can write:

```cpp
std::string input = "plaintext";
std::string output(input.size() + cipher.blockSize(), '\0');

cipher.initEncryptor();
ssize_t n   = cipher.update(input, output);  // Raw<> adapts both strings
ssize_t fin = cipher.final(output);
output.resize(n + fin);
```

### Cross-Module Usage

The `crypto` module has no standalone sample programs; it is a support library used by other modules. The most illustrative real-world usages are in `stun` and `net`.

#### STUN message integrity (HMAC-SHA1)

The `stun` module implements RFC 5389 Section 15.4 MESSAGE-INTEGRITY using `crypto::computeHMAC` directly. When writing a STUN message, the `MessageIntegrity` attribute computes the HMAC over the serialized message bytes up to and including the attribute placeholder:

```cpp
// From stun/src/attributes.cpp

#include "icy/crypto/hmac.h"

// On write: compute HMAC over message bytes with the shared credential key
std::string input(hmacWriter.begin(), hmacWriter.position());
std::string hmac(crypto::computeHMAC(input, _key));
writer.put(hmac.c_str(), hmac.length());  // 20 raw bytes into the wire format

// On read: verify the received HMAC
bool verifyHmac(std::string_view key) const
{
    std::string hmac = crypto::computeHMAC(_input, key);
    return _hmac == hmac;  // constant-time comparison not used here; STUN is not the security boundary
}
```

The key for long-term credentials is an MD5 digest of `username:realm:password` as specified by RFC 5389 Section 15.4. For short-term credentials, the key is the password directly.

#### TLS certificate inspection (net module)

`net::SSLContext` (`icy/net/sslcontext.h`) includes `x509certificate.h` and `rsa.h` directly. When a TLS handshake fails certificate verification, `net::SSLManager` wraps the failing `X509*` in an `X509Certificate` (shared ownership) and passes it to the application's verification error handler:

```cpp
// From net/src/sslmanager.cpp

#include "icy/crypto/x509certificate.h"

// Called by OpenSSL's verify callback
if (!ok) {
    X509* pCert = X509_STORE_CTX_get_current_cert(pStore);
    crypto::X509Certificate x509(pCert, /*shared=*/true);
    // x509 is passed to VerificationErrorDetails; original X509* remains valid
}
```

Loading a server certificate and private key for an HTTPS server:

```cpp
#include "icy/net/sslcontext.h"
#include "icy/crypto/x509certificate.h"

net::SSLContext ctx(net::SSLContext::SERVER_USE);

// Load the certificate chain
icy::crypto::X509Certificate cert("/etc/ssl/server.pem");
icy::crypto::X509Certificate chain("/etc/ssl/intermediate.pem");

ctx.useCertificate(cert);
ctx.addChainCertificate(chain);

// Private key is loaded from a file path directly via SSLContext::usePrivateKeyFile
ctx.usePrivateKeyFile("/etc/ssl/server.key");
```

#### Passphrase-based file encryption

A self-contained pattern for encrypting and decrypting files with a passphrase:

```cpp
#include "icy/crypto/cipher.h"
#include <fstream>
#include <stdexcept>

void encryptFile(const std::string& inputPath,
                 const std::string& outputPath,
                 const std::string& passphrase)
{
    icy::crypto::Cipher cipher("aes-256-cbc", passphrase, "icey-salt", 10000);
    std::ifstream src(inputPath,  std::ios::binary);
    std::ofstream dst(outputPath, std::ios::binary);
    if (!src) throw std::runtime_error("Cannot open input: " + inputPath);
    if (!dst) throw std::runtime_error("Cannot open output: " + outputPath);
    cipher.encryptStream(src, dst);
}

void decryptFile(const std::string& inputPath,
                 const std::string& outputPath,
                 const std::string& passphrase)
{
    icy::crypto::Cipher cipher("aes-256-cbc", passphrase, "icey-salt", 10000);
    std::ifstream src(inputPath,  std::ios::binary);
    std::ofstream dst(outputPath, std::ios::binary);
    if (!src) throw std::runtime_error("Cannot open input: " + inputPath);
    if (!dst) throw std::runtime_error("Cannot open output: " + outputPath);
    cipher.decryptStream(src, dst);
}
```

The key derivation iteration count trades startup cost for brute-force resistance. 10,000 iterations is a reasonable minimum; increase for high-security contexts.

#### File integrity verification

```cpp
#include "icy/crypto/hash.h"
#include <iostream>

void verifyDownload(const std::string& filePath,
                    const std::string& expectedSha256Hex)
{
    std::string actual = icy::crypto::checksum("sha256", filePath);
    if (actual != expectedSha256Hex)
        throw std::runtime_error(
            "Checksum mismatch: expected " + expectedSha256Hex +
            ", got " + actual);
    std::cout << "OK: " << filePath << '\n';
}
```

## Configuration

### OpenSSL version requirements

| OpenSSL version | Status |
| --- | --- |
| 3.x | Required; all tests run against this version |
| 1.1.x | Not supported |
| 1.0.x | Not supported |

OpenSSL is a required dependency. The crypto module's CMakeLists conditionalizes on `HAVE_OPENSSL`:

```cmake
if(HAVE_OPENSSL)
  icy_add_module(crypto
    DEPENDS base
    PACKAGES OpenSSL::SSL OpenSSL::Crypto
  )
endif()
```

### CMake flags

| Flag | Default | Effect |
| --- | --- | --- |
| `-DBUILD_TESTS=ON` | `OFF` | Builds `cryptotests` in `src/crypto/tests/` |

OpenSSL is auto-detected. If it is not found, the crypto module and all modules that depend on it (`net`, `stun`, `turn`, `webrtc`) are disabled. The `base`, `json`, `archo`, `graft`, `sched`, `speech`, and `vision` modules build without OpenSSL. Install OpenSSL 3.x (`libssl-dev` on Ubuntu/Debian) to enable the crypto module.

### Windows considerations

On Windows, `wincrypt.h` defines `X509_NAME`, `X509_CERT_PAIR`, and `X509_EXTENSIONS` as macros that conflict with OpenSSL. `crypto.h` detects `ICY_WIN` and `#undef`s them after including `<windows.h>` and `<winsock2.h>`. Similarly, `hmac.cpp` detects `ICY_WIN` and `#undef`s `OCSP_RESPONSE` to avoid a name collision with `wincrypt.h`. These are handled automatically; no application-level workaround is needed.

For shared library builds on Windows, the `Crypto_API` macro maps to `__declspec(dllexport)` when building the library and `__declspec(dllimport)` when consuming it, controlled by the `Crypto_EXPORTS` preprocessor symbol set automatically by CMake.

## See Also

- [Net](net.md) — `net::SSLContext` and `net::SSLManager`; TLS socket creation, certificate loading, and chain validation
- [STUN](stun.md) — `stun::MessageIntegrity` uses `crypto::computeHMAC` for MESSAGE-INTEGRITY; STUN credential model documented there
- [TURN](turn.md) — inherits STUN authentication; depends on the HMAC path
