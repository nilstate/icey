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
#include "icy/datetime.h"

#include <istream>
#include <memory>
#include <set>

#include <openssl/ssl.h>


namespace icy {
namespace crypto {


/// RAII pointer alias for OpenSSL `X509*` values.
using X509Ptr = std::unique_ptr<X509, decltype(&X509_free)>;

/// RAII wrapper for an OpenSSL X509 certificate with PEM loading and inspection.
class Crypto_API X509Certificate
{
public:
    /// Name identifier for extracting fields from a certificate's distinguished name.
    ///
    /// Values correspond to OpenSSL NID constants used with
    /// X509_NAME_get_text_by_NID.
    enum NID
    {
        NID_COMMON_NAME = 13,          ///< Common name (CN field).
        NID_COUNTRY = 14,              ///< Country code (C field).
        NID_LOCALITY_NAME = 15,        ///< Locality / city (L field).
        NID_STATE_OR_PROVINCE = 16,    ///< State or province (ST field).
        NID_ORGANIZATION_NAME = 17,    ///< Organization name (O field).
        NID_ORGANIZATION_UNIT_NAME = 18 ///< Organizational unit (OU field).
    };

    /// Constructs an X509Certificate by parsing a PEM-encoded certificate from memory.
    ///
    /// @param data   Pointer to a buffer containing the PEM-encoded certificate.
    /// @param length Number of bytes in @p data.
    /// @throws std::runtime_error if the BIO cannot be created or PEM parsing fails.
    explicit X509Certificate(const char* data, size_t length);

    /// Constructs an X509Certificate by reading a PEM-encoded certificate from a file.
    ///
    /// @param path Filesystem path to the PEM certificate file.
    /// @throws std::runtime_error if the file cannot be opened or PEM parsing fails.
    explicit X509Certificate(const std::string& path);

    /// Constructs an X509Certificate taking ownership of an existing OpenSSL X509 object.
    ///
    /// @param pCert Non-null pointer to an OpenSSL X509 certificate. This object
    ///              takes ownership and will call X509_free on destruction.
    /// @throws std::runtime_error if @p pCert is null.
    explicit X509Certificate(X509* pCert);

    /// Constructs an X509Certificate from an existing OpenSSL X509 object,
    /// optionally sharing ownership via reference count increment.
    ///
    /// @param pCert   Non-null pointer to an OpenSSL X509 certificate. Ownership
    ///                is always taken (X509_free called on destruction).
    /// @param shared  If true, increments the certificate's reference count via
    ///                X509_up_ref before taking ownership, so the original pointer
    ///                remains valid after this object is destroyed.
    /// @throws std::runtime_error if @p pCert is null.
    X509Certificate(X509* pCert, bool shared);

    /// Copy-constructs an X509Certificate by duplicating the underlying X509 object.
    ///
    /// @param cert The certificate to copy.
    X509Certificate(const X509Certificate& cert);

    /// Move-constructs an X509Certificate, transferring ownership from @p cert.
    ///
    /// @param cert The certificate to move from; left in a valid but empty state.
    X509Certificate(X509Certificate&& cert) noexcept;

    /// Copy-assigns a certificate, duplicating the underlying X509 object.
    ///
    /// @param cert The certificate to copy.
    /// @return Reference to this object.
    X509Certificate& operator=(const X509Certificate& cert);

    /// Move-assigns a certificate, transferring ownership from @p cert.
    ///
    /// @param cert The certificate to move from; left in a valid but empty state.
    /// @return Reference to this object.
    X509Certificate& operator=(X509Certificate&& cert) noexcept;

    /// Swaps this certificate with @p cert.
    ///
    /// @param cert The certificate to swap with.
    void swap(X509Certificate& cert);

    /// Destroys the X509Certificate and releases the underlying OpenSSL X509 object.
    ~X509Certificate();

    /// Returns the full distinguished name of the certificate issuer.
    ///
    /// @return One-line string representation produced by X509_NAME_oneline.
    const std::string& issuerName() const;

    /// Extracts a single field from the certificate issuer's distinguished name.
    ///
    /// @param nid The field to extract (e.g. NID_COMMON_NAME).
    /// @return Field value, or an empty string if the field is absent.
    std::string issuerName(NID nid) const;

    /// Returns the full distinguished name of the certificate subject.
    ///
    /// @return One-line string representation produced by X509_NAME_oneline.
    const std::string& subjectName() const;

    /// Extracts a single field from the certificate subject's distinguished name.
    ///
    /// @param nid The field to extract (e.g. NID_ORGANIZATION_NAME).
    /// @return Field value, or an empty string if the field is absent.
    std::string subjectName(NID nid) const;

    /// Returns the common name (CN) from the certificate subject.
    ///
    /// Convenience wrapper for subjectName(NID_COMMON_NAME).
    ///
    /// @return Common name string, or empty if absent.
    std::string commonName() const;

    /// Extracts the common name and the set of Subject Alternative Name (SAN)
    /// DNS entries from the certificate.
    ///
    /// If no SAN DNS entries are present and the common name is non-empty,
    /// the common name is added to @p domainNames as a fallback.
    ///
    /// @param commonName  Receives the certificate's common name.
    /// @param domainNames Receives all DNS SAN entries (cleared before population).
    void extractNames(std::string& commonName,
                      std::set<std::string>& domainNames) const;

    /// Returns the date and time from which the certificate is valid.
    ///
    /// Parsed from the X509 notBefore field.
    ///
    /// @return UTC DateTime representing the start of the validity period.
    DateTime validFrom() const;

    /// Returns the date and time at which the certificate expires.
    ///
    /// Parsed from the X509 notAfter field.
    ///
    /// @return UTC DateTime representing the end of the validity period.
    DateTime expiresOn() const;

    /// Writes the certificate in PEM format to an output stream.
    ///
    /// @param stream Destination stream to write to.
    /// @throws std::runtime_error if the BIO cannot be created or write fails.
    void save(std::ostream& stream) const;

    /// Writes the certificate in PEM format to a file.
    ///
    /// @param path Filesystem path of the output file (created or truncated).
    /// @throws std::runtime_error if the file cannot be opened or write fails.
    void save(const std::string& path) const;

    /// Verifies whether this certificate was signed by the given issuer.
    ///
    /// Extracts the public key from @p issuerCertificate and calls X509_verify.
    /// Use this to validate links in a certificate chain.
    ///
    /// @param issuerCertificate The certificate of the purported issuer.
    /// @return true if this certificate's signature verifies against the
    ///         issuer's public key, false otherwise.
    /// @throws std::invalid_argument if the issuer certificate has no public key.
    bool issuedBy(const X509Certificate& issuerCertificate) const;

    /// Returns a const pointer to the underlying OpenSSL X509 object.
    ///
    /// @return Pointer valid for the lifetime of this X509Certificate.
    const X509* certificate() const;

    /// Returns a mutable pointer to the underlying OpenSSL X509 object.
    ///
    /// @return Pointer valid for the lifetime of this X509Certificate.
    X509* certificate();

protected:
    /// Parses a PEM-encoded certificate from a memory buffer and stores it.
    ///
    /// @param data   Pointer to PEM data.
    /// @param length Number of bytes in @p data.
    /// @throws std::logic_error if a certificate is already loaded.
    /// @throws std::runtime_error if BIO creation or PEM parsing fails.
    void load(const char* data, size_t length);

    /// Reads a PEM-encoded certificate from a file and stores it.
    ///
    /// @param path Filesystem path to the PEM certificate file.
    /// @throws std::logic_error if a certificate is already loaded.
    /// @throws std::runtime_error if the file cannot be opened or PEM parsing fails.
    void load(const std::string& path);

    /// Populates _issuerName and _subjectName from the loaded certificate.
    ///
    /// Called after each successful load or construction from an X509 pointer.
    void init();

private:
    enum
    {
        NAME_BUFFER_SIZE = 256
    };

    std::string _issuerName;
    std::string _subjectName;
    X509Ptr _certificate;
};


} // namespace crypto
} // namespace icy


/// @}
