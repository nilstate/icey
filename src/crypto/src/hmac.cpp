///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#include "scy/crypto/hmac.h"
#include "scy/util.h"
#include <stdexcept>

#ifdef SCY_WIN
// hack for name collision of OCSP_RESPONSE and wincrypto.h in openssl release
// 0.9.8h
// http://www.google.com/search?q=OCSP%5fRESPONSE+wincrypt%2eh
// continue to watch this issue for a real fix.
#undef OCSP_RESPONSE
#endif
#include <openssl/hmac.h>


namespace scy {
namespace crypto {


std::string computeHMAC(const std::string& input, const std::string& key)
{
    // SDebug << "Compute HMAC: input='" << util::dumpbin(input.c_str(), input.length())
    //    << "', inputLength=" << input.length() << ", key='" << key << "',
    //    keyLength=" << key.length() << std::endl;
    unsigned int len = 0;
    char buf[20];
    HMAC(EVP_sha1(), key.c_str(), key.length(),
         reinterpret_cast<const unsigned char*>(input.c_str()), input.length(),
         reinterpret_cast<unsigned char*>(&buf), &len);
    if (len != 20)
        throw std::runtime_error("HMAC: unexpected digest length");
    return std::string(buf, len);
}


} // namespace crypto
} // namespace scy


/// @\}
