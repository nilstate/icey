///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include <cstdint>
#include <random>


namespace scy {


/// Random implements a pseudo random number generator (PRNG)
/// using the Mersenne Twister algorithm (std::mt19937).
class Base_API Random
{
public:
    /// Creates and initializes the PRNG.
    /// The stateSize parameter is accepted for API compatibility
    /// but is ignored; the engine always uses mt19937's fixed state size.
    Random(int stateSize = 256);

    /// Destroys the PRNG.
    ~Random();

    /// Seeds the pseudo random generator with the given seed.
    void seed(uint32_t seed);

    /// Seeds the pseudo random generator with a random seed
    /// obtained from std::random_device.
    void seed();

    /// Returns the next 31-bit pseudo random number.
    uint32_t next();

    /// Returns the next 31-bit pseudo random number modulo n.
    uint32_t next(uint32_t n);

    /// Returns the next pseudo random character.
    char nextChar();

    /// Returns the next boolean pseudo random value.
    bool nextBool();

    /// Returns the next float pseudo random number between 0.0 and 1.0.
    float nextFloat();

    /// Returns the next double pseudo random number between 0.0 and 1.0.
    double nextDouble();

    /// Fills the buffer with random bytes from std::random_device.
    static void getSeed(char* seed, unsigned length);

private:
    std::mt19937 _engine;
};


} // namespace scy


/// @\}
