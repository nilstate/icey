///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup crypto
/// @{


#include "icy/crypto/hash.h"
#include "icy/error.h"
#include <assert.h>
#include <iostream>




namespace icy {
namespace crypto {

Hash::Hash(const std::string& algorithm)
    : _ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free)
    , _md(nullptr)
    , _digest()
    , _algorithm(algorithm)
{
    crypto::initializeEngine();

    _md = EVP_get_digestbyname(algorithm.data());
    if (!_md)
        throw std::runtime_error("Algorithm not supported: " + algorithm);

    EVP_DigestInit_ex(_ctx.get(), _md, nullptr);
}


Hash::~Hash()
{
    crypto::uninitializeEngine();
}


void Hash::reset()
{
    internal::api(EVP_MD_CTX_reset(_ctx.get()));
    internal::api(EVP_DigestInit_ex(_ctx.get(), _md, nullptr));
    _digest.clear();
}


void Hash::update(const void* data, size_t length)
{
    internal::api(EVP_DigestUpdate(_ctx.get(), data, length));
}


void Hash::update(std::string_view data)
{
    update(data.data(), data.size());
}


void Hash::update(char data)
{
    update(&data, 1);
}


const ByteVec& Hash::digest()
{
    // Compute the first time
    if (_digest.size() == 0) {
        _digest.resize(EVP_MAX_MD_SIZE);
        unsigned int len = 0;
        internal::api(EVP_DigestFinal_ex(_ctx.get(), &_digest[0], &len));
        _digest.resize(len);
    }
    return _digest;
}


std::string Hash::digestStr()
{
    const ByteVec& vec = digest();
    return std::string(reinterpret_cast<const char*>(vec.data()), vec.size());
}


const std::string& Hash::algorithm() const
{
    return _algorithm;
}


} // namespace crypto
} // namespace icy


/// @\}
