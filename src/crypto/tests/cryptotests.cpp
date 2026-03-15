#include "scy/crypto/cipher.h"
#include "scy/crypto/crypto.h"
#include "scy/crypto/hash.h"
#include "scy/crypto/rsa.h"
#include "scy/crypto/x509certificate.h"
#include "scy/filesystem.h"
#include "scy/hex.h"
#include "scy/logger.h"
#include "scy/test.h"
#include "scy/util.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <openssl/evp.h>
#include <openssl/hmac.h>


using std::endl;
using namespace scy;
using namespace scy::test;


namespace scy {


void testCipher(const std::string algorithm, int iterations)
{
    {
        crypto::Cipher ciph(algorithm);
        clock_t start = clock();
        for (int n = 1; n < iterations; n++) {
            std::string in(n, 'x');
            std::string out = ciph.encryptString(in, crypto::Cipher::Binary);
            std::string result = ciph.decryptString(out, crypto::Cipher::Binary);
            expect(in == result);
        }
        LDebug("Binary: ", (clock() - start));
    }

    {
        crypto::Cipher ciph(algorithm);
        clock_t start = clock();
        for (int n = 1; n < iterations; n++) {
            std::string in(n, 'x');
            std::string out = ciph.encryptString(in, crypto::Cipher::Base64);
            std::string result = ciph.decryptString(out, crypto::Cipher::Base64);
            expect(in == result);
        }
        LDebug("Base64: ", (clock() - start));
    }

    {
        crypto::Cipher ciph(algorithm);
        clock_t start = clock();
        for (int n = 1; n < iterations; n++) {
            std::string in(n, 'x');
            std::string out = ciph.encryptString(in, crypto::Cipher::BinHex);
            std::string result = ciph.decryptString(out, crypto::Cipher::BinHex);
            expect(in == result);
        }
        LDebug("BinHex: ", (clock() - start));
    }

    {
        crypto::Cipher ciph(algorithm);
        std::string iv(util::randomString(16));
        std::string key(util::randomString(32));
        std::string in(1000, 'x');

        std::string out = crypto::encryptString(algorithm, in, key, iv, crypto::Cipher::Binary);
        std::string result = crypto::decryptString(algorithm, out, key, iv, crypto::Cipher::Binary);
        expect(in == result);
    }

    {
        // Quick test using string input buffer

        crypto::Cipher ciph(algorithm);
        std::string in(20, 'x');

        // We can use a string as buffer as long
        // as the implementation is contiguous (as per c++11)
        std::string out(100, '\0');
        ssize_t len = ciph.encrypt(in, out, crypto::Cipher::BinHex);
        out.resize(len);

        std::string result = ciph.decryptString(out, crypto::Cipher::BinHex);
        expect(in == result);
    }
}


/// Compute HMAC using OpenSSL EVP API directly
std::string computeHMAC(const std::string& algorithm, const std::string& key,
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

    return hex::encode(std::vector<unsigned char>(result, result + len));
}


} // namespace scy


int main(int argc, char** argv)
{
    // Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();
    crypto::initializeEngine();

    // =========================================================================
    // Cipher: AES-256-CBC
    //
    describe("aes256 cipher", []() {
        testCipher("aes-256-cbc", 100);
    });

    // =========================================================================
    // Cipher: empty input
    //
    describe("cipher empty input", []() {
        crypto::Cipher ciph("aes-256-cbc");
        std::string in;
        std::string out = ciph.encryptString(in, crypto::Cipher::Binary);
        std::string result = ciph.decryptString(out, crypto::Cipher::Binary);
        expect(result.empty());
    });

    // =========================================================================
    // Cipher: large input (64KB)
    //
    describe("cipher large input", []() {
        crypto::Cipher ciph("aes-256-cbc");
        std::string in(65536, 'A');
        std::string out = ciph.encryptString(in, crypto::Cipher::Binary);
        std::string result = ciph.decryptString(out, crypto::Cipher::Binary);
        expect(in == result);
    });

    // =========================================================================
    // Cipher: passphrase-based key derivation
    //
    describe("cipher with passphrase", []() {
        crypto::Cipher ciph("aes-256-cbc", "my secret passphrase", "saltsalt", 2000);
        std::string in = "encrypt me with a passphrase";
        std::string out = ciph.encryptString(in, crypto::Cipher::Base64);
        std::string result = ciph.decryptString(out, crypto::Cipher::Base64);
        expect(in == result);
    });

    // =========================================================================
    // Cipher: stream encryption
    //
    describe("cipher stream encrypt/decrypt", []() {
        crypto::Cipher ciph("aes-256-cbc");
        std::string in = "stream encryption test data that is longer than one block";

        std::istringstream source(in);
        std::ostringstream encrypted;
        ciph.encryptStream(source, encrypted, crypto::Cipher::Base64);

        std::istringstream encSource(encrypted.str());
        std::ostringstream decrypted;
        ciph.decryptStream(encSource, decrypted, crypto::Cipher::Base64);

        expect(decrypted.str() == in);
    });

    // =========================================================================
    // Cipher: invalid algorithm throws
    //
    describe("cipher invalid algorithm", []() {
        try {
            crypto::Cipher ciph("nonexistent-cipher-xyz");
            expect(0 && "invalid algorithm - must throw");
        } catch (std::exception&) {
        }
    });

    // =========================================================================
    // SHA1 Hash
    //
    describe("sha1 hash", []() {
        // test vectors from FIPS 180-1

        crypto::Hash engine("SHA1");
        engine.update("abc", 3);
        expect(hex::encode(engine.digest()) == "a9993e364706816aba3e25717850c26c9cd0d89d");

        engine.reset();
        engine.update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
        expect(hex::encode(engine.digest()) == "84983e441c3bd26ebaae4aa1f95129e5e54670f1");

        engine.reset();
        for (int i = 0; i < 1000000; ++i)
            engine.update('a');
        expect(hex::encode(engine.digest()) == "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
    });

    // =========================================================================
    // MD5 Hash
    //
    describe("md5 hash", []() {
        crypto::Hash engine("MD5");

        // test vectors from RFC 1321

        engine.update("");
        expect(hex::encode(engine.digest()) == "d41d8cd98f00b204e9800998ecf8427e");

        engine.reset();
        engine.update("a");
        expect(hex::encode(engine.digest()) == "0cc175b9c0f1b6a831c399e269772661");

        engine.reset();
        engine.update("abc");
        expect(hex::encode(engine.digest()) == "900150983cd24fb0d6963f7d28e17f72");

        engine.reset();
        engine.update("message digest");
        expect(hex::encode(engine.digest()) == "f96b697d7cb7938d525a2f31aaf161d0");

        engine.reset();
        engine.update("abcdefghijklmnopqrstuvwxyz");
        expect(hex::encode(engine.digest()) == "c3fcd3d76192e4007dfb496cca67e13b");

        engine.reset();
        engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
        expect(hex::encode(engine.digest()) == "d174ab98d277d9f5a5611c2c9f419d9f");

        engine.reset();
        engine.update("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
        expect(hex::encode(engine.digest()) == "57edf4a22be3c955ac49da2e2107b67a");
    });

    // =========================================================================
    // SHA-256 Hash
    //
    describe("sha256 hash", []() {
        // test vectors from FIPS 180-2

        crypto::Hash engine("SHA256");
        engine.update("abc", 3);
        expect(hex::encode(engine.digest()) == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

        engine.reset();
        engine.update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
        expect(hex::encode(engine.digest()) == "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");

        // empty string
        engine.reset();
        engine.update("", 0);
        expect(hex::encode(engine.digest()) == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    });

    // =========================================================================
    // SHA-512 Hash
    //
    describe("sha512 hash", []() {
        crypto::Hash engine("SHA512");
        engine.update("abc", 3);
        expect(hex::encode(engine.digest()) ==
               "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
               "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");

        // empty string
        engine.reset();
        engine.update("", 0);
        expect(hex::encode(engine.digest()) ==
               "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
               "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
    });

    // =========================================================================
    // Hash: empty input
    //
    describe("hash empty input", []() {
        // MD5 of empty string
        crypto::Hash engine("MD5");
        engine.update("", 0);
        expect(hex::encode(engine.digest()) == "d41d8cd98f00b204e9800998ecf8427e");
    });

    // =========================================================================
    // Hash: large input (1MB)
    //
    describe("hash large input", []() {
        crypto::Hash engine("SHA256");
        std::string chunk(4096, 'z');
        for (int i = 0; i < 256; ++i)
            engine.update(chunk);

        // Just verify it completes and produces a 64-char hex digest
        std::string result = hex::encode(engine.digest());
        expect(result.size() == 64);
    });

    // =========================================================================
    // Hash: invalid algorithm throws
    //
    describe("hash invalid algorithm", []() {
        try {
            crypto::Hash engine("NONEXISTENT_HASH");
            expect(0 && "invalid algorithm - must throw");
        } catch (std::exception&) {
        }
    });

    // =========================================================================
    // Hash: convenience function
    //
    describe("hash convenience function", []() {
        std::string result = crypto::hash("SHA1", "abc");
        expect(result == "a9993e364706816aba3e25717850c26c9cd0d89d");

        result = crypto::hash("MD5", std::string(""));
        expect(result == "d41d8cd98f00b204e9800998ecf8427e");
    });

    // =========================================================================
    // Hash: digestStr
    //
    describe("hash digestStr", []() {
        crypto::Hash engine("MD5");
        engine.update("abc");
        std::string raw = engine.digestStr();
        // MD5 produces 16 bytes
        expect(raw.size() == 16);
    });

    // =========================================================================
    // Hash: incremental update
    //
    describe("hash incremental update", []() {
        // Hashing in chunks should produce same result as hashing all at once
        crypto::Hash engine1("SHA256");
        engine1.update("hello world");

        crypto::Hash engine2("SHA256");
        engine2.update("hello ");
        engine2.update("world");

        expect(hex::encode(engine1.digest()) == hex::encode(engine2.digest()));
    });

    // =========================================================================
    // HMAC: SHA-256
    //
    describe("hmac sha256", []() {
        // RFC 4231 test vectors

        // Test case 1
        std::string key1(20, '\x0b');
        std::string data1 = "Hi There";
        expect(computeHMAC("SHA256", key1, data1) ==
               "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");

        // Test case 2: "Jefe" / "what do ya want for nothing?"
        expect(computeHMAC("SHA256", "Jefe", "what do ya want for nothing?") ==
               "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
    });

    // =========================================================================
    // HMAC: SHA-1
    //
    describe("hmac sha1", []() {
        // RFC 2202 test case 1
        std::string key(20, '\x0b');
        std::string data = "Hi There";
        expect(computeHMAC("SHA1", key, data) ==
               "b617318655057264e28bc0b6fb378c8ef146be00");
    });

    // =========================================================================
    // HMAC: MD5
    //
    describe("hmac md5", []() {
        // RFC 2104 test case 1
        std::string key(16, '\x0b');
        std::string data = "Hi There";
        expect(computeHMAC("MD5", key, data) ==
               "9294727a3638bb1c13f48ef8158bfc9d");
    });

    // =========================================================================
    // HMAC: empty data
    //
    describe("hmac empty data", []() {
        std::string result = computeHMAC("SHA256", "key", "");
        // Just verify it produces a valid 64-char hex string
        expect(result.size() == 64);
    });

    // =========================================================================
    // X509Certificate: load from file
    //
    describe("x509 load from file", []() {
        std::string certPath(SCY_SOURCE_DIR);
        fs::addnode(certPath, "net");
        fs::addnode(certPath, "tests");
        fs::addnode(certPath, "cert.pem");

        if (!fs::exists(certPath)) {
            std::cout << "Skipping X509 file test - cert.pem not found at: " << certPath << std::endl;
            return;
        }

        crypto::X509Certificate cert(certPath);

        // Verify basic accessors return non-empty strings
        expect(!cert.issuerName().empty());
        expect(!cert.subjectName().empty());
    });

    // =========================================================================
    // X509Certificate: load from buffer
    //
    describe("x509 load from buffer", []() {
        std::string certPath(SCY_SOURCE_DIR);
        fs::addnode(certPath, "net");
        fs::addnode(certPath, "tests");
        fs::addnode(certPath, "cert.pem");

        if (!fs::exists(certPath)) {
            std::cout << "Skipping X509 buffer test - cert.pem not found" << std::endl;
            return;
        }

        // Read cert data into a string
        std::ifstream file(certPath);
        std::string certData((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        expect(!certData.empty());

        crypto::X509Certificate cert(certData.data(), certData.size());

        expect(!cert.issuerName().empty());
        expect(!cert.subjectName().empty());
    });

    // =========================================================================
    // X509Certificate: copy and assignment
    //
    describe("x509 copy and assignment", []() {
        std::string certPath(SCY_SOURCE_DIR);
        fs::addnode(certPath, "net");
        fs::addnode(certPath, "tests");
        fs::addnode(certPath, "cert.pem");

        if (!fs::exists(certPath)) {
            std::cout << "Skipping X509 copy test" << std::endl;
            return;
        }

        crypto::X509Certificate cert1(certPath);

        // Copy constructor
        crypto::X509Certificate cert2(cert1);
        expect(cert1.issuerName() == cert2.issuerName());
        expect(cert1.subjectName() == cert2.subjectName());

        // Assignment operator
        crypto::X509Certificate cert3(certPath);
        cert3 = cert1;
        expect(cert1.issuerName() == cert3.issuerName());
        expect(cert1.subjectName() == cert3.subjectName());
    });

    // =========================================================================
    // X509Certificate: save and reload (round-trip)
    //
    describe("x509 save and reload", []() {
        std::string certPath(SCY_SOURCE_DIR);
        fs::addnode(certPath, "net");
        fs::addnode(certPath, "tests");
        fs::addnode(certPath, "cert.pem");

        if (!fs::exists(certPath)) {
            std::cout << "Skipping X509 save test" << std::endl;
            return;
        }

        crypto::X509Certificate cert1(certPath);

        // Save to stream
        std::ostringstream oss;
        cert1.save(oss);
        std::string pemData = oss.str();
        expect(!pemData.empty());
        expect(pemData.find("-----BEGIN CERTIFICATE-----") != std::string::npos);

        // Reload from buffer
        crypto::X509Certificate cert2(pemData.data(), pemData.size());
        expect(cert1.issuerName() == cert2.issuerName());
        expect(cert1.subjectName() == cert2.subjectName());
    });

    // =========================================================================
    // X509Certificate: extractNames
    //
    describe("x509 extract names", []() {
        std::string certPath(SCY_SOURCE_DIR);
        fs::addnode(certPath, "net");
        fs::addnode(certPath, "tests");
        fs::addnode(certPath, "cert.pem");

        if (!fs::exists(certPath)) {
            std::cout << "Skipping X509 extractNames test" << std::endl;
            return;
        }

        crypto::X509Certificate cert(certPath);
        std::string commonName;
        std::set<std::string> domainNames;
        cert.extractNames(commonName, domainNames);

        // commonName should be populated for any valid cert
        // (domainNames may or may not have SANs depending on cert)
        expect(!commonName.empty() || !domainNames.empty());
    });

    // =========================================================================
    // X509Certificate: validity dates
    //
    describe("x509 validity dates", []() {
        std::string certPath(SCY_SOURCE_DIR);
        fs::addnode(certPath, "net");
        fs::addnode(certPath, "tests");
        fs::addnode(certPath, "cert.pem");

        if (!fs::exists(certPath)) {
            std::cout << "Skipping X509 validity test" << std::endl;
            return;
        }

        crypto::X509Certificate cert(certPath);

        // Just verify the date accessors don't throw
        DateTime validFrom = cert.validFrom();
        DateTime expiresOn = cert.expiresOn();

        // expiresOn should be after validFrom
        expect(expiresOn > validFrom);
    });

    // =========================================================================
    // X509Certificate: invalid PEM data throws
    //
    describe("x509 invalid data throws", []() {
        try {
            crypto::X509Certificate cert("not a valid certificate", 23);
            expect(0 && "invalid PEM data - must throw");
        } catch (std::exception&) {
        }
    });

    // =========================================================================
    // X509Certificate: invalid file path throws
    //
    describe("x509 invalid path throws", []() {
        try {
            crypto::X509Certificate cert("/nonexistent/path/cert.pem");
            expect(0 && "nonexistent file - must throw");
        } catch (std::exception&) {
        }
    });

    // =========================================================================
    // Hex encoder/decoder
    //
    describe("hex encoder/decoder", []() {
        hex::Encoder enc;
        hex::Decoder dec;

        std::string in(1000, 'x');

        char encBuf[2048];
        size_t len = enc.encode(in.c_str(), in.length(), encBuf);
        std::string encRes(encBuf, len);
        LDebug("Encoded: ", encRes);

        char decBuf[2048];
        len = dec.decode(encBuf, len, decBuf);
        std::string decRes(decBuf, len);
        LDebug("Decoded: ", decRes);

        expect(in == decRes);
    });

    // =========================================================================
    // Hex: empty input
    //
    describe("hex empty input", []() {
        std::vector<unsigned char> empty;
        std::string result = hex::encode(empty);
        expect(result.empty());
    });

    // =========================================================================
    // Hex: round-trip various data
    //
    describe("hex round-trip", []() {
        hex::Encoder enc;
        hex::Decoder dec;

        // Test with all byte values 0x00-0xFF
        std::string in;
        for (int i = 0; i < 256; ++i)
            in += static_cast<char>(i);

        char encBuf[1024];
        size_t encLen = enc.encode(in.c_str(), in.length(), encBuf);

        char decBuf[512];
        size_t decLen = dec.decode(encBuf, encLen, decBuf);

        expect(decLen == 256);
        for (int i = 0; i < 256; ++i)
            expect(static_cast<unsigned char>(decBuf[i]) == static_cast<unsigned char>(i));
    });

    // =========================================================================
    // File checksum
    //
    describe("file checksum", []() {
        // Create a temporary file and verify its checksum
        std::string path = "checksum_test.tmp";
        std::string content = "hello checksum world";
        fs::savefile(path, content.c_str(), content.size(), true);

        std::string md5 = crypto::checksum("MD5", path);
        expect(!md5.empty());
        expect(md5.size() == 32); // MD5 hex digest is 32 chars

        std::string sha1 = crypto::checksum("SHA1", path);
        expect(!sha1.empty());
        expect(sha1.size() == 40); // SHA1 hex digest is 40 chars

        std::string sha256 = crypto::checksum("SHA256", path);
        expect(!sha256.empty());
        expect(sha256.size() == 64); // SHA256 hex digest is 64 chars

        // Verify MD5 matches manual computation
        crypto::Hash manual("MD5");
        manual.update(content);
        expect(md5 == hex::encode(manual.digest()));

        fs::unlink(path);
    });

    // =========================================================================
    // File checksum: nonexistent file throws
    //
    describe("file checksum nonexistent", []() {
        try {
            crypto::checksum("MD5", "/nonexistent/file.bin");
            expect(0 && "nonexistent file - must throw");
        } catch (std::exception&) {
        }
    });

    test::runAll();

    crypto::uninitializeEngine();
    // Logger::destroy();

    return finalize();
}
