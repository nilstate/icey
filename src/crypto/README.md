# Crypto Module

OpenSSL 3.x cryptographic operations for hashing, HMAC, symmetric ciphers, certificates, and keys.

- Namespace: `icy::crypto`
- CMake target: `icey::crypto`
- Primary headers: `include/icy/crypto/hash.h`, `hmac.h`, `cipher.h`, `x509certificate.h`
- Directory layout: `include/` for the public API, `src/` for the EVP-backed implementation, `tests/` for algorithm and certificate coverage
- Main downstream users inside icey: `net` for TLS, `stun` for message integrity, `turn` through `stun`

Read next:
- [Crypto module guide](../../docs/modules/crypto.md)
- [Net module guide](../../docs/modules/net.md)
