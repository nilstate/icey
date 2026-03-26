///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include <cstdint>
#include <random>


namespace icy {


/// Random implements a pseudo random number generator (PRNG)
/// using the Mersenne Twister algorithm (std::mt19937).
class Base_API Random
{
public:
    /// Creates and initializes the PRNG.
    /// The stateSize parameter is accepted for API compatibility
    /// but is ignored; the engine always uses mt19937's fixed state size.
    /// @param stateSize Ignored; present for API compatibility only.
    Random(int stateSize = 256);

    /// Destroys the PRNG.
    ~Random();

    /// Seeds the pseudo random generator with the given seed.
    /// @param seed 32-bit seed value.
    void seed(uint32_t seed);

    /// Seeds the pseudo random generator with entropy from std::random_device.
    void seed();

    /// Returns the next pseudo random number from the mt19937 engine.
    /// @return Pseudo random uint32_t value.
    uint32_t next();

    /// Returns the next pseudo random number in the range [0, n).
    /// @param n Upper bound (exclusive).
    /// @return Pseudo random value in [0, n).
    uint32_t next(uint32_t n);

    /// Returns the next pseudo random byte as a char.
    /// @return Pseudo random char value.
    char nextChar();

    /// Returns the next pseudo random boolean value.
    /// @return true or false with equal probability.
    bool nextBool();

    /// Returns the next pseudo random float in [0.0, 1.0].
    /// @return Pseudo random float value.
    float nextFloat();

    /// Returns the next pseudo random double in [0.0, 1.0].
    /// @return Pseudo random double value.
    double nextDouble();

    /// Fills the buffer with cryptographically random bytes from std::random_device.
    /// @param seed   Buffer to fill.
    /// @param length Number of bytes to write into seed.
    static void getSeed(char* seed, unsigned length);

private:
    std::mt19937 _engine;
};


} // namespace icy


/// @}
