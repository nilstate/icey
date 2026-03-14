///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#include "scy/crypto/hash.h"
#include "scy/error.h"
#include <assert.h>
#include <iostream>


using std::endl;


namespace scy {
namespace crypto {
 
Hash::Hash(const std::string& algorithm)
    : _ctx(EVP_MD_CTX_new())
    , _md(nullptr)
    ,  _digest()
    , _algorithm(algorithm)
{
    crypto::initializeEngine();

    _md = EVP_get_digestbyname(algorithm.data());
    if (!_md)
        throw std::runtime_error("Algorithm not supported: " + algorithm);

    EVP_DigestInit(_ctx, _md);
}


Hash::~Hash()
{
    crypto::uninitializeEngine();
    EVP_MD_CTX_free(_ctx);
}


void Hash::reset()
{
    internal::api(EVP_MD_CTX_reset(_ctx));
    internal::api(EVP_DigestInit(_ctx, _md));
    _digest.clear();
}


void Hash::update(const void* data, size_t length)
{
    internal::api(EVP_DigestUpdate(_ctx, data, length));
}


void Hash::update(const std::string& data)
{
    update(data.c_str(), data.length());
}


void Hash::update(char data)
{
    update(&data, 1);
}


const ByteVec& Hash::digest()
{
    // Compute the first time
    if (_digest.size() == 0) {
        _digest.resize(EVP_MAX_MD_SIZE); // TODO: Get actual algorithm size
        unsigned int len = 0;
        internal::api(EVP_DigestFinal(_ctx, &_digest[0], &len));
        _digest.resize(len);
    }
    return _digest;
}


std::string Hash::digestStr()
{
    const ByteVec& vec = digest();
    return std::string((const char*)vec.data(), vec.size());
}


const std::string& Hash::algorithm(void) const
{
    return _algorithm;
}


} // namespace crypto
} // namespace scy


/// @\}
