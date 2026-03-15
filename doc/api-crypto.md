# crypto 

### Classes

| Name | Description |
|------|-------------|
| [`Cipher`](#classscy_1_1crypto_1_1Cipher) | Provides symmetric algorithms for encryption and decryption. The algorithms that are available depend on the particular version of OpenSSL that is installed. |
| [`Hash`](#classscy_1_1crypto_1_1Hash) |  |
| [`X509Certificate`](#classscy_1_1crypto_1_1X509Certificate) | This class represents a X509 Certificate. |

### Members

| Name | Description |
|------|-------------|
| [`Crypto_API`](#group__crypto_1ga5b6282a38265a398bb16794294a91f94) |  |

---

#### Crypto_API 

```cpp
Crypto_API()
```

## Cipher 

> **Defined in:** `cipher.h`

Provides symmetric algorithms for encryption and decryption. The algorithms that are available depend on the particular version of OpenSSL that is installed.

### Members

| Name | Description |
|------|-------------|
| [`Encoding`](#group__crypto_1ga44fda7872beb30a68bbc8f2cac9271ae) | Transport encoding to use for [encrypt()](#group__crypto_1ga7cd265c66a382e6b9b396db93875a54c) and decrypt(). |
| [`Cipher`](#group__crypto_1ga29c00ec7db3b341e2a40fb34324a6d00) | Creates a new [Cipher](#classscy_1_1crypto_1_1Cipher) object. Auto initializes the key and initialization vector with random bytes. |
| [`Cipher`](#group__crypto_1ga3084b42100789d88220012f926e60504) | Creates a new [Cipher](#classscy_1_1crypto_1_1Cipher) object, using the given cipher name, key and initialization vector. |
| [`Cipher`](#group__crypto_1gafa9fbfd0ac80c3e5fe6ee9d939e44907) | Creates a new [Cipher](#classscy_1_1crypto_1_1Cipher) object, using the given cipher name, passphrase, salt value and iteration count. |
| [`~Cipher`](#group__crypto_1gabae774949c196156b08b3e62c997516f) | Destroys the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`initEncryptor`](#group__crypto_1ga144dd6c25b7d3d0ec97da382829cf636) | Initializes the [Cipher](#classscy_1_1crypto_1_1Cipher) for encryption. |
| [`initDecryptor`](#group__crypto_1ga0677c7dac45328f0517b35cb32e3d48c) | Initializes the [Cipher](#classscy_1_1crypto_1_1Cipher) for decryption. |
| [`update`](#group__crypto_1ga3041433d718ba3d7d7b47b4b40afac42) | Encrypts data in a streaming fashion. Hand consecutive blocks of data to the update method in order to encrypt it. Returns the encrypted data chunk. When done, the output of [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) should be additionally added to the result. |
| [`update`](#group__crypto_1ga04c53b48009782e244a2a468a7109c0d) | Alias for [update()](#group__crypto_1ga3041433d718ba3d7d7b47b4b40afac42) which accepts a range of buffer types. |
| [`final`](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) | Returns the remaining data held in the cipher object. Further calls to [update()](#group__crypto_1ga3041433d718ba3d7d7b47b4b40afac42) or [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) will return garbage. |
| [`final`](#group__crypto_1gadcd86bffcf3501416c9114574b377b18) | Alias for [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) which accepts a range of buffer types. |
| [`encrypt`](#group__crypto_1ga7cd265c66a382e6b9b396db93875a54c) | Encrypts a buffer and encode it using the given encoding. This method performs the encryption, and calls [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) internally. |
| [`encrypt`](#group__crypto_1gaeb0d5c7ccfc327728ef33957822682f3) | Alias for [encrypt()](#group__crypto_1ga7cd265c66a382e6b9b396db93875a54c) which accepts a range of buffer types. |
| [`encryptString`](#group__crypto_1ga8c440edf0d347751a1d1b9f43e99bd43) | Encrypts a string and encodes it using the given encoding. |
| [`decryptString`](#group__crypto_1gaa51a187bd02ae8d71aa58686f738ff9b) | Decrypts a string that is encoded with the given encoding. |
| [`encryptStream`](#group__crypto_1ga77107e1458845773e1ea7d3a81a6a3c3) | Encrypts an input stream and encodes it using the given encoding. |
| [`decryptStream`](#group__crypto_1gada68a82360e3eb8853efe26e55fee509) | Decrypts an input stream that is encoded with the given encoding. |
| [`setKey`](#group__crypto_1gaeec7a89c1fc12a93e00f2a119d67fc58) | Sets the key for the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`setIV`](#group__crypto_1ga0368e6675a26e93d69beea3a090b7908) | Sets the initialization vector (IV) for the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`setPadding`](#group__crypto_1ga4f8d10556e26c93af353a79d4f5bf65f) | Enables or disables padding. By default encryption operations are padded using standard block padding and the padding is checked and removed when decrypting. If the pad parameter is zero then no padding is performed, the total amount of data encrypted or decrypted must then be a multiple of the block size or an error will occur. |
| [`getKey`](#group__crypto_1gad9b197b9bc698da0defede327f66aad6) | Returns the key for the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`getIV`](#group__crypto_1gacb5497a7ce6d4a39a57147cdf0b39f7c) | Returns the initialization vector (IV) for the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`name`](#group__crypto_1gaa230464d6fc653fde4b08afcd75242cf) | Returns the name of the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`blockSize`](#group__crypto_1gaddae447539926bb35a9394ad96dec391) | Returns the block size of the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`keySize`](#group__crypto_1ga25f3530bbbd017c9fc219c538a50726e) | Returns the key size of the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`ivSize`](#group__crypto_1ga50eab14578b8d9404f38276136514ea2) | Returns the IV size of the [Cipher](#classscy_1_1crypto_1_1Cipher). |
| [`cipher`](#group__crypto_1ga51844517077a71950e7f384ccb2dd8b1) | Returns the cipher object. |
| [`Cipher`](#group__crypto_1ga82b3135d1ca065df49483bba540ea3de) |  |
| [`Cipher`](#group__crypto_1ga4113c32229f659416f774e08426d9488) |  |
| [`operator=`](#group__crypto_1gacabc90b6f083892e54ffa99cc41552b0) |  |
| [`generateKey`](#group__crypto_1ga6edba619802b9ca520bc4f8dd1b5d8b2) | Generates and sets the key and IV from a password and optional salt string. |
| [`setRandomKey`](#group__crypto_1gad71d5cde5fea40cfeab6b08518ddbc95) | Generates and sets key from random data. |
| [`setRandomIV`](#group__crypto_1gab3206763d10ce4674eb7e05f0390913a) | Generates and sets IV from random data. |
| [`init`](#group__crypto_1ga84b340347deeb58f81292539edd90c0c) | Initializes the [Cipher](#classscy_1_1crypto_1_1Cipher) using the given direction. |
| [`_initialized`](#group__crypto_1gaef37db5562b977395a89945b02e3b684) |  |
| [`_encrypt`](#group__crypto_1ga234fb383e465ae6c7f322a4be0ba6a2f) |  |
| [`_cipher`](#group__crypto_1ga6234e02c10a2a4be511fb2caaa7180b0) |  |
| [`_ctx`](#group__crypto_1ga6d959336eca73fd8e1ba012de98de928) |  |
| [`_name`](#group__crypto_1gae8bceb0c5aab20317421d37ccdf9b046) |  |
| [`_key`](#group__crypto_1ga88a997190e8cd5880718fdbe4a613109) |  |
| [`_iv`](#group__crypto_1ga9fb992dd64b02eaa38cac541d5072ac3) |  |

---

#### Encoding 

```cpp
enum Encoding
```

Transport encoding to use for [encrypt()](#group__crypto_1ga7cd265c66a382e6b9b396db93875a54c) and decrypt().

| Value | Description |
|-------|-------------|
| `Binary` | Plain binary output. |
| `Base64` | Base64-encoded output. |
| `BinHex` | BinHex-encoded output. |
| `Base64_NoLF` | Base64-encoded output, no linefeeds. |
| `BinHex_NoLF` | BinHex-encoded output, no linefeeds. |

---

#### Cipher 

```cpp
Cipher(const std::string & name)
```

Creates a new [Cipher](#classscy_1_1crypto_1_1Cipher) object. Auto initializes the key and initialization vector with random bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### Cipher 

```cpp
Cipher(const std::string & name, const ByteVec & key, const ByteVec & iv)
```

Creates a new [Cipher](#classscy_1_1crypto_1_1Cipher) object, using the given cipher name, key and initialization vector.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `key` | `const ByteVec &` |  |
| `iv` | `const ByteVec &` |  |

---

#### Cipher 

```cpp
Cipher(const std::string & name, const std::string & passphrase, const std::string & salt, int iterationCount)
```

Creates a new [Cipher](#classscy_1_1crypto_1_1Cipher) object, using the given cipher name, passphrase, salt value and iteration count.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `passphrase` | `const std::string &` |  |
| `salt` | `const std::string &` |  |
| `iterationCount` | `int` |  |

---

#### ~Cipher 

```cpp
~Cipher()
```

Destroys the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### initEncryptor 

```cpp
void initEncryptor()
```

Initializes the [Cipher](#classscy_1_1crypto_1_1Cipher) for encryption.

---

#### initDecryptor 

```cpp
void initDecryptor()
```

Initializes the [Cipher](#classscy_1_1crypto_1_1Cipher) for decryption.

---

#### update 

```cpp
ssize_t update(const unsigned char * input, size_t inputLength, unsigned char * output, size_t outputLength)
```

Encrypts data in a streaming fashion. Hand consecutive blocks of data to the update method in order to encrypt it. Returns the encrypted data chunk. When done, the output of [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) should be additionally added to the result.

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `const unsigned char *` |  |
| `inputLength` | `size_t` |  |
| `output` | `unsigned char *` |  |
| `outputLength` | `size_t` |  |

---

#### update 

```cpp
template<typename I, typename O> inline ssize_t update(const I & input, O & output)
```

Alias for [update()](#group__crypto_1ga3041433d718ba3d7d7b47b4b40afac42) which accepts a range of buffer types.

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `const I &` |  |
| `output` | `O &` |  |

---

#### final 

```cpp
ssize_t final(unsigned char * output, size_t length)
```

Returns the remaining data held in the cipher object. Further calls to [update()](#group__crypto_1ga3041433d718ba3d7d7b47b4b40afac42) or [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) will return garbage.

See EVP_CipherFinal_ex for further information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `output` | `unsigned char *` |  |
| `length` | `size_t` |  |

---

#### final 

```cpp
template<typename O> inline ssize_t final(O & output)
```

Alias for [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) which accepts a range of buffer types.

| Parameter | Type | Description |
|-----------|------|-------------|
| `output` | `O &` |  |

---

#### encrypt 

```cpp
ssize_t encrypt(const unsigned char * inbuf, size_t inlen, unsigned char * outbuf, size_t outlen, Encoding encoding)
```

Encrypts a buffer and encode it using the given encoding. This method performs the encryption, and calls [final()](#group__crypto_1ga67e162bd2a71957d98cb1e0e2675e4ea) internally.

| Parameter | Type | Description |
|-----------|------|-------------|
| `inbuf` | `const unsigned char *` |  |
| `inlen` | `size_t` |  |
| `outbuf` | `unsigned char *` |  |
| `outlen` | `size_t` |  |
| `encoding` | `Encoding` |  |

---

#### encrypt 

```cpp
template<typename I, typename O> inline ssize_t encrypt(const I & input, O & output, Encoding encoding)
```

Alias for [encrypt()](#group__crypto_1ga7cd265c66a382e6b9b396db93875a54c) which accepts a range of buffer types.

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `const I &` |  |
| `output` | `O &` |  |
| `encoding` | `Encoding` |  |

---

#### encryptString 

```cpp
virtual std::string encryptString(const std::string & str, Encoding encoding)
```

Encrypts a string and encodes it using the given encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |
| `encoding` | `Encoding` |  |

---

#### decryptString 

```cpp
virtual std::string decryptString(const std::string & str, Encoding encoding)
```

Decrypts a string that is encoded with the given encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `str` | `const std::string &` |  |
| `encoding` | `Encoding` |  |

---

#### encryptStream 

```cpp
virtual void encryptStream(std::istream & source, std::ostream & sink, Encoding encoding)
```

Encrypts an input stream and encodes it using the given encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `std::istream &` |  |
| `sink` | `std::ostream &` |  |
| `encoding` | `Encoding` |  |

---

#### decryptStream 

```cpp
virtual void decryptStream(std::istream & source, std::ostream & sink, Encoding encoding)
```

Decrypts an input stream that is encoded with the given encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `source` | `std::istream &` |  |
| `sink` | `std::ostream &` |  |
| `encoding` | `Encoding` |  |

---

#### setKey 

```cpp
template<typename T> inline void setKey(const T & key)
```

Sets the key for the [Cipher](#classscy_1_1crypto_1_1Cipher).

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const T &` |  |

---

#### setIV 

```cpp
template<typename T> inline void setIV(const T & iv)
```

Sets the initialization vector (IV) for the [Cipher](#classscy_1_1crypto_1_1Cipher).

| Parameter | Type | Description |
|-----------|------|-------------|
| `iv` | `const T &` |  |

---

#### setPadding 

```cpp
int setPadding(int padding)
```

Enables or disables padding. By default encryption operations are padded using standard block padding and the padding is checked and removed when decrypting. If the pad parameter is zero then no padding is performed, the total amount of data encrypted or decrypted must then be a multiple of the block size or an error will occur.

See EVP_CIPHER_CTX_set_padding for further information.

| Parameter | Type | Description |
|-----------|------|-------------|
| `padding` | `int` |  |

---

#### getKey 

```cpp
const ByteVec & getKey() const
```

Returns the key for the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### getIV 

```cpp
const ByteVec & getIV() const
```

Returns the initialization vector (IV) for the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### name 

```cpp
const std::string & name() const
```

Returns the name of the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### blockSize 

```cpp
int blockSize() const
```

Returns the block size of the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### keySize 

```cpp
int keySize() const
```

Returns the key size of the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### ivSize 

```cpp
int ivSize() const
```

Returns the IV size of the [Cipher](#classscy_1_1crypto_1_1Cipher).

---

#### cipher 

```cpp
const EVP_CIPHER * cipher()
```

Returns the cipher object.

---

#### Cipher 

```cpp
Cipher() = default
```

---

#### Cipher 

```cpp
Cipher(const Cipher &) = delete
```

---

#### operator= 

```cpp
Cipher & operator=(const Cipher &) = delete
```

---

#### generateKey 

```cpp
void generateKey(const std::string & passphrase, const std::string & salt, int iterationCount)
```

Generates and sets the key and IV from a password and optional salt string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `passphrase` | `const std::string &` |  |
| `salt` | `const std::string &` |  |
| `iterationCount` | `int` |  |

---

#### setRandomKey 

```cpp
void setRandomKey()
```

Generates and sets key from random data.

---

#### setRandomIV 

```cpp
void setRandomIV()
```

Generates and sets IV from random data.

---

#### init 

```cpp
void init(bool encrypt)
```

Initializes the [Cipher](#classscy_1_1crypto_1_1Cipher) using the given direction.

| Parameter | Type | Description |
|-----------|------|-------------|
| `encrypt` | `bool` |  |

---

#### _initialized 

```cpp
bool _initialized
```

---

#### _encrypt 

```cpp
bool _encrypt
```

---

#### _cipher 

```cpp
const EVP_CIPHER * _cipher
```

---

#### _ctx 

```cpp
EVP_CIPHER_CTX * _ctx
```

---

#### _name 

```cpp
std::string _name
```

---

#### _key 

```cpp
ByteVec _key
```

---

#### _iv 

```cpp
ByteVec _iv
```

## Hash 

> **Defined in:** `hash.h`

### Members

| Name | Description |
|------|-------------|
| [`Hash`](#group__crypto_1gaed22bcaf84b35efc1815a03903541da4) |  |
| [`~Hash`](#group__crypto_1gaf1bea9c52e354b7b97550e1715105ee0) |  |
| [`update`](#group__crypto_1ga7389984a20183881be045ff427036ce8) |  |
| [`update`](#group__crypto_1ga8b68b47d859b92608a2ac7295c3ef702) |  |
| [`update`](#group__crypto_1ga23679ad362d7a4a21f0ee73a61795161) | This function may (and normally will) be called many times for large blocks of data. |
| [`digest`](#group__crypto_1gacbc12b3d6816140c2e5033fe32efafca) | Finish up the digest operation and return the result. |
| [`digestStr`](#group__crypto_1gad11ff9012ce5157206e0a05a7c3f7347) | Finish up the digest operation and return the result as a string. |
| [`reset`](#group__crypto_1ga58abad9c8210a2882d69dceb3883a973) | Resets the engine and digest state ready for the next computation. |
| [`algorithm`](#group__crypto_1ga6f4e22146b390772a4d2c1d4ef6352c7) | Returns the hash algorithm being used. |
| [`operator=`](#group__crypto_1gab33fa99f39b8f58fb69008442f1245b4) |  |
| [`_ctx`](#group__crypto_1gaed86aa72bb57df7c08d903a6c1bf5621) |  |
| [`_md`](#group__crypto_1gada899a7cc6726572e494dfa2e0aa4838) |  |
| [`_digest`](#group__crypto_1gaa8321b86e61d992786de9a8be63630a7) |  |
| [`_algorithm`](#group__crypto_1ga778a1b93ea54aff4e5b1b1b80ccbcb6f) |  |

---

#### Hash 

```cpp
Hash(const std::string & algorithm)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `algorithm` | `const std::string &` |  |

---

#### ~Hash 

```cpp
~Hash()
```

---

#### update 

```cpp
void update(char data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `char` |  |

---

#### update 

```cpp
void update(const std::string & data)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const std::string &` |  |

---

#### update 

```cpp
void update(const void * data, size_t length)
```

This function may (and normally will) be called many times for large blocks of data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const void *` |  |
| `length` | `size_t` |  |

---

#### digest 

```cpp
const ByteVec & digest()
```

Finish up the digest operation and return the result.

---

#### digestStr 

```cpp
std::string digestStr()
```

Finish up the digest operation and return the result as a string.

---

#### reset 

```cpp
void reset()
```

Resets the engine and digest state ready for the next computation.

---

#### algorithm 

```cpp
const std::string & algorithm(void) const
```

Returns the hash algorithm being used.

---

#### operator= 

```cpp
Hash & operator=(Hash const &)
```

---

#### _ctx 

```cpp
EVP_MD_CTX * _ctx
```

---

#### _md 

```cpp
const EVP_MD * _md
```

---

#### _digest 

```cpp
crypto::ByteVec _digest
```

---

#### _algorithm 

```cpp
std::string _algorithm
```

## X509Certificate 

> **Defined in:** `x509certificate.h`

This class represents a X509 Certificate.

### Members

| Name | Description |
|------|-------------|
| [`NID`](#group__crypto_1gaf15888f0ebe57ab7978cb91f7bbd40d7) | Name identifier for extracting information from a certificate subject's or issuer's distinguished name. |
| [``](#group__crypto_1ga16824404470aa043a22a79b9ca9a68c0) |  |
| [`X509Certificate`](#group__crypto_1ga459ca16eab567ab22d1b675f6a334ced) | Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) object by reading a certificate in PEM format from the given buffer. |
| [`X509Certificate`](#group__crypto_1ga417bdf3f76dfecef89626b6cb71a9a32) | Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) object by reading a certificate in PEM format from a file. |
| [`X509Certificate`](#group__crypto_1ga2aa7feaea1f6c99c994fcc6fb7bea0ac) | Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) from an existing OpenSSL certificate. Ownership is taken of the certificate. |
| [`X509Certificate`](#group__crypto_1gaf7e31eac8d3095866847b5af0db366fa) | Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) from an existing OpenSSL certificate. Ownership is taken of the certificate. If shared is true, the certificate's reference count is incremented. |
| [`X509Certificate`](#group__crypto_1ga9aaeb1e01fbc97752b90be697230a504) | Creates the certificate by copying another one. |
| [`operator=`](#group__crypto_1ga1516b536eadd451c76ecafa299f63d7c) | Assigns a certificate. |
| [`swap`](#group__crypto_1gac487d92721c8e2bca13f0d13edcf942d) | Exchanges the certificate with another one. |
| [`~X509Certificate`](#group__crypto_1ga9bca014de92bf3dcb7988eea0a41b92f) | Destroys the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate). |
| [`issuerName`](#group__crypto_1ga59be81cc1b19268c8c3f9b1fd046b20d) | Returns the certificate issuer's distinguished name. |
| [`issuerName`](#group__crypto_1gade700582f523d36f9ba35b4ff42ae9da) | Extracts the information specified by the given NID (name identifier) from the certificate issuer's distinguished name. |
| [`subjectName`](#group__crypto_1ga08cbb2a291c084ac15d427f2a75bed51) | Returns the certificate subject's distinguished name. |
| [`subjectName`](#group__crypto_1ga448f08e983b8fc5e93bbd1de5f33d0ca) | Extracts the information specified by the given NID (name identifier) from the certificate subject's distinguished name. |
| [`commonName`](#group__crypto_1ga5c8fd7098b3c4114b414a737ad5b10f8) | Returns the common name stored in the certificate subject's distinguished name. |
| [`extractNames`](#group__crypto_1gacfd53defe5a0eb4329622b02f0725221) | Extracts the common name and the alias domain names from the certificate. |
| [`validFrom`](#group__crypto_1ga79d4899027eb85cfad01e892f3595e33) | Returns the date and time the certificate is valid from. |
| [`expiresOn`](#group__crypto_1gad2d8ba7bc9502843db12690f6ed24646) | Returns the date and time the certificate expires. |
| [`save`](#group__crypto_1gaf0aaafac5aa2869c52d19b34f8768c0b) | Writes the certificate to the given stream. The certificate is written in PEM format. |
| [`save`](#group__crypto_1ga0cea8f317ad0371de4a05ae1f4c4387d) | Writes the certificate to the file given by path. The certificate is written in PEM format. |
| [`issuedBy`](#group__crypto_1ga8e93fd651db9982f6178b5fae9447fcc) | Checks whether the certificate has been issued by the issuer given by issuerCertificate. This can be used to validate a certificate chain. |
| [`certificate`](#group__crypto_1ga351a6a8c90b667597ab6ddb1233be511) | Returns the underlying OpenSSL certificate. |
| [`load`](#group__crypto_1ga0472559c4633b117e91bc68b50bb7de6) | Loads the certificate from the given buffer. The certificate must be in PEM format. |
| [`load`](#group__crypto_1ga3d18050527eb8fd86b4b63924ecc7ca5) | Loads the certificate from the given file. The certificate must be in PEM format. |
| [`init`](#group__crypto_1gaaf8694ec9aa62ec67324253b9456ba2e) | Extracts issuer and subject name from the certificate. |
| [`_issuerName`](#group__crypto_1ga079edf7060b186eab27aa557db2021bf) |  |
| [`_subjectName`](#group__crypto_1ga01c347e1866e5c461bf888093d7b78f8) |  |
| [`_certificate`](#group__crypto_1ga8b1647c17acdec201f9247c9c43de2bc) |  |

---

#### NID 

```cpp
enum NID
```

Name identifier for extracting information from a certificate subject's or issuer's distinguished name.

| Value | Description |
|-------|-------------|
| `NID_COMMON_NAME` |  |
| `NID_COUNTRY` |  |
| `NID_LOCALITY_NAME` |  |
| `NID_STATE_OR_PROVINCE` |  |
| `NID_ORGANIZATION_NAME` |  |
| `NID_ORGANIZATION_UNIT_NAME` |  |

---

####  

```cpp
enum 
```

| Value | Description |
|-------|-------------|
| `NAME_BUFFER_SIZE` |  |

---

#### X509Certificate 

```cpp
explicit X509Certificate(const char * data, size_t length)
```

Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) object by reading a certificate in PEM format from the given buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `length` | `size_t` |  |

---

#### X509Certificate 

```cpp
explicit X509Certificate(const std::string & path)
```

Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) object by reading a certificate in PEM format from a file.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### X509Certificate 

```cpp
explicit X509Certificate(X509 * pCert)
```

Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) from an existing OpenSSL certificate. Ownership is taken of the certificate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pCert` | `X509 *` |  |

---

#### X509Certificate 

```cpp
X509Certificate(X509 * pCert, bool shared)
```

Creates the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate) from an existing OpenSSL certificate. Ownership is taken of the certificate. If shared is true, the certificate's reference count is incremented.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pCert` | `X509 *` |  |
| `shared` | `bool` |  |

---

#### X509Certificate 

```cpp
X509Certificate(const X509Certificate & cert)
```

Creates the certificate by copying another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cert` | `const X509Certificate &` |  |

---

#### operator= 

```cpp
X509Certificate & operator=(const X509Certificate & cert)
```

Assigns a certificate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cert` | `const X509Certificate &` |  |

---

#### swap 

```cpp
void swap(X509Certificate & cert)
```

Exchanges the certificate with another one.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cert` | `X509Certificate &` |  |

---

#### ~X509Certificate 

```cpp
~X509Certificate()
```

Destroys the [X509Certificate](#classscy_1_1crypto_1_1X509Certificate).

---

#### issuerName 

```cpp
const std::string & issuerName() const
```

Returns the certificate issuer's distinguished name.

---

#### issuerName 

```cpp
std::string issuerName(NID nid) const
```

Extracts the information specified by the given NID (name identifier) from the certificate issuer's distinguished name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `nid` | `NID` |  |

---

#### subjectName 

```cpp
const std::string & subjectName() const
```

Returns the certificate subject's distinguished name.

---

#### subjectName 

```cpp
std::string subjectName(NID nid) const
```

Extracts the information specified by the given NID (name identifier) from the certificate subject's distinguished name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `nid` | `NID` |  |

---

#### commonName 

```cpp
std::string commonName() const
```

Returns the common name stored in the certificate subject's distinguished name.

---

#### extractNames 

```cpp
void extractNames(std::string & commonName, std::set< std::string > & domainNames) const
```

Extracts the common name and the alias domain names from the certificate.

| Parameter | Type | Description |
|-----------|------|-------------|
| `commonName` | `std::string &` |  |
| `domainNames` | `std::set< std::string > &` |  |

---

#### validFrom 

```cpp
DateTime validFrom() const
```

Returns the date and time the certificate is valid from.

---

#### expiresOn 

```cpp
DateTime expiresOn() const
```

Returns the date and time the certificate expires.

---

#### save 

```cpp
void save(std::ostream & stream) const
```

Writes the certificate to the given stream. The certificate is written in PEM format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `std::ostream &` |  |

---

#### save 

```cpp
void save(const std::string & path) const
```

Writes the certificate to the file given by path. The certificate is written in PEM format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### issuedBy 

```cpp
bool issuedBy(const X509Certificate & issuerCertificate) const
```

Checks whether the certificate has been issued by the issuer given by issuerCertificate. This can be used to validate a certificate chain.

Verifies if the certificate has been signed with the issuer's private key, using the public key from the issuer certificate.

Returns true if verification against the issuer certificate was successful, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `issuerCertificate` | `const X509Certificate &` |  |

---

#### certificate 

```cpp
const X509 * certificate() const
```

Returns the underlying OpenSSL certificate.

---

#### load 

```cpp
void load(const char * data, size_t length)
```

Loads the certificate from the given buffer. The certificate must be in PEM format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const char *` |  |
| `length` | `size_t` |  |

---

#### load 

```cpp
void load(const std::string & path)
```

Loads the certificate from the given file. The certificate must be in PEM format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### init 

```cpp
void init()
```

Extracts issuer and subject name from the certificate.

---

#### _issuerName 

```cpp
std::string _issuerName
```

---

#### _subjectName 

```cpp
std::string _subjectName
```

---

#### _certificate 

```cpp
X509 * _certificate
```

