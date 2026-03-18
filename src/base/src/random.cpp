///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/random.h"

#include <cstring>
#include <limits>


namespace scy {


Random::Random(int /*stateSize*/)
{
    seed();
}


Random::~Random() = default;


void Random::seed(uint32_t s)
{
    _engine.seed(s);
}


void Random::seed()
{
    std::random_device rd;
    _engine.seed(rd());
}


uint32_t Random::next()
{
    return _engine() & 0x7FFFFFFF;
}


uint32_t Random::next(uint32_t n)
{
    return next() % n;
}


char Random::nextChar()
{
    return static_cast<char>((_engine() >> 3) & 0xFF);
}


bool Random::nextBool()
{
    return (_engine() & 0x1000) != 0;
}


float Random::nextFloat()
{
    return std::uniform_real_distribution<float>(0.0f, 1.0f)(_engine);
}


double Random::nextDouble()
{
    return std::uniform_real_distribution<double>(0.0, 1.0)(_engine);
}


void Random::getSeed(char* seed, unsigned length)
{
    std::random_device rd;
    // random_device produces uint32_t values; fill the buffer in chunks
    unsigned i = 0;
    while (i + sizeof(uint32_t) <= length) {
        uint32_t val = rd();
        std::memcpy(seed + i, &val, sizeof(uint32_t));
        i += sizeof(uint32_t);
    }
    if (i < length) {
        uint32_t val = rd();
        std::memcpy(seed + i, &val, length - i);
    }
}


} // namespace scy


/// @\}
