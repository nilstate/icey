///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{
//
// This file uses functions from POCO C++ Libraries (license below).
//


#include "scy/random.h"
#include "scy/error.h"

#include <stdexcept>


#include <ctime>
#if defined(_WIN32)
#include <wincrypt.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif


/*
 * random.c:
 *
 * An improved random number generation package.  In addition to the standard
 * rand()/srand() like interface, this package also has a special state info
 * interface.  The initstate() routine is called with a seed, an array of
 * bytes, and a count of how many bytes are being passed in; this array is
 * then initialized to contain information for random number generation with
 * that much state information.  Good sizes for the amount of state
 * information are 32, 64, 128, and 256 bytes.  The state can be switched by
 * calling the setstate() routine with the same array as was initiallized
 * with initstate().  By default, the package runs with 128 bytes of state
 * information and generates far better random numbers than a linear
 * congruential generator.  If the amount of state information is less than
 * 32 bytes, a simple linear congruential R.N.G. is used.
 *
 * Internally, the state information is treated as an array of uint32_t's; the
 * zeroeth element of the array is the type of R.N.G. being used (small
 * integer); the remainder of the array is the state information for the
 * R.N.G.  Thus, 32 bytes of state information will give 7 ints worth of
 * state information, which will allow a degree seven polynomial.  (Note:
 * the zeroeth word of state information also has some other information
 * stored in it -- see setstate() for details).
 *
 * The random number generation technique is a linear feedback shift register
 * approach, employing trinomials (since there are fewer terms to sum up that
 * way).  In this approach, the least significant bit of all the numbers in
 * the state table will act as a linear feedback shift register, and will
 * have period 2^deg - 1 (where deg is the degree of the polynomial being
 * used, assuming that the polynomial is irreducible and primitive).  The
 * higher order bits will have longer periods, since their values are also
 * influenced by pseudo-random carries out of the lower bits.  The total
 * period of the generator is approximately deg*(2**deg - 1); thus doubling
 * the amount of state information has a vast influence on the period of the
 * generator.  Note: the deg*(2**deg - 1) is an approximation only good for
 * large deg, when the period of the shift is the dominant factor.
 * With deg equal to seven, the period is actually much longer than the
 * 7*(2**7 - 1) predicted by this formula.
 *
 * Modified 28 December 1994 by Jacob S. Rosenberg.
 * The following changes have been made:
 * All references to the type u_int have been changed to unsigned long.
 * All references to type int have been changed to type long.  Other
 * cleanups have been made as well.  A warning for both initstate and
 * setstate has been inserted to the effect that on Sparc platforms
 * the 'arg_state' variable must be forced to begin on word boundaries.
 * This can be easily done by casting a long integer array to char *.
 * The overall logic has been left STRICTLY alone.  This software was
 * tested on both a VAX and Sun SpacsStation with exactly the same
 * results.  The new version and the original give IDENTICAL results.
 * The new version is somewhat faster than the original.  As the
 * documentation says:  "By default, the package runs with 128 bytes of
 * state information and generates far better random numbers than a linear
 * congruential generator.  If the amount of state information is less than
 * 32 bytes, a simple linear congruential R.N.G. is used."  For a buffer of
 * 128 bytes, this new version runs about 19 percent faster and for a 16
 * byte buffer it is about 5 percent faster.
 */


/*
 * For each of the currently supported random number generators, we have a
 * break value on the amount of state information (you need at least this
 * many bytes of state info to support this random number generator), a degree
 * for the polynomial (actually a trinomial) that the R.N.G. is based on, and
 * the separation between the two lower order coefficients of the trinomial.
 */
#define TYPE_0 0 /* linear congruential */
#define BREAK_0 8
#define DEG_0 0
#define SEP_0 0

#define TYPE_1 1 /* x**7 + x**3 + 1 */
#define BREAK_1 32
#define DEG_1 7
#define SEP_1 3

#define TYPE_2 2 /* x**15 + x + 1 */
#define BREAK_2 64
#define DEG_2 15
#define SEP_2 1

#define TYPE_3 3 /* x**31 + x**3 + 1 */
#define BREAK_3 128
#define DEG_3 31
#define SEP_3 3

#define TYPE_4 4 /* x**63 + x + 1 */
#define BREAK_4 256
#define DEG_4 63
#define SEP_4 1


namespace scy {


Random::Random(int stateSize)
{
    if (stateSize < BREAK_0 || stateSize > BREAK_4)
        throw std::invalid_argument("Random: state size out of range");

    _buffer = std::make_unique<char[]>(stateSize);
    initState(static_cast<uint32_t>(std::time(nullptr)), _buffer.get(), stateSize);
}


Random::~Random() = default;


/*
 * Compute x = (7^5 * x) mod (2^31 - 1)
 * wihout overflowing 31 bits:
 *      (2^31 - 1) = 127773 * (7^5) + 2836
 * From "Random number generators: good ones are hard to find",
 * Park and Miller, Communications of the ACM, vol. 31, no. 10,
 * October 1988, p. 1195.
 */
uint32_t Random::goodRand(std::int32_t x)
{
    std::int32_t hi, lo;

    if (x == 0)
        x = 123459876;
    hi = x / 127773;
    lo = x % 127773;
    x = 16807 * lo - 2836 * hi;
    if (x < 0)
        x += 0x7FFFFFFF;

    return x;
}


/*
 * Initialize the random number generator based on the given seed.  If the
 * type is the trivial no-state-information type, just remember the seed.
 * Otherwise, initializes state[] based on the given "seed" via a linear
 * congruential generator.  Then, the pointers are set to known locations
 * that are exactly rand_sep places apart.  Lastly, it cycles the state
 * information a given number of times to get rid of any initial dependencies
 * introduced by the L.C.R.N.G.  Note that the initialization of randtbl[]
 * for default usage relies on values produced by this routine.
 */
void Random::seed(uint32_t x)
{
    int i, lim;

    _state[0] = x;
    if (_randType == TYPE_0)
        lim = NSHUFF;
    else {
        for (i = 1; i < _randDeg; i++)
            _state[i] = goodRand(_state[i - 1]);
        _fptr = &_state[_randSep];
        _rptr = &_state[0];
        lim = 10 * _randDeg;
    }
    for (i = 0; i < lim; i++)
        next();
}


/*
 * Many programs choose the seed value in a totally predictable manner.
 * This often causes problems.  We seed the generator using the much more
 * secure random(4) interface.  Note that this particular seeding
 * procedure can generate states which are impossible to reproduce by
 * calling srandom() with any value, since the succeeding terms in the
 * state buffer are no longer derived from the LC algorithm applied to
 * a fixed seed.
 */
void Random::seed()
{
    int len;

    if (_randType == TYPE_0)
        len = sizeof _state[0];
    else
        len = _randDeg * sizeof _state[0];

    getSeed(reinterpret_cast<char*>(_state), len);
}


void Random::getSeed(char* seed, unsigned length)
{
    int n = 0;

#ifdef SCY_WIN
    HCRYPTPROV hProvider = 0;
    CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptGenRandom(hProvider, static_cast<DWORD>(length), reinterpret_cast<BYTE*>(seed));
    CryptReleaseContext(hProvider, 0);
    n = static_cast<int>(length);
#else
    int fd = open("/dev/urandom", O_RDONLY, 0);
    if (fd >= 0) {
        n = read(fd, seed, length);
        close(fd);
    }
#endif

    // Note: We could use OpenSSL RAND_bytes(),
    // but we don't want Base to depend on OpenSSL

    if (n == 0)
        throw std::runtime_error("Cannot generate random number seed");
}


/*
 * Initialize the state information in the given array of n bytes for future
 * random number generation.  Based on the number of bytes we are given, and
 * the break values for the different R.N.G.'s, we choose the best (largest)
 * one we can and set things up for it.  srandom() is then called to
 * initialize the state information.
 *
 * Note that on return from srandom(), we set state[-1] to be the type
 * multiplexed with the current value of the rear pointer; this is so
 * successive calls to initstate() won't lose this information and will be
 * able to restart with setstate().
 *
 * Note: the first thing we do is save the current state, if any, just like
 * setstate() so that it doesn't matter when initstate is called.
 *
 * Returns a pointer to the old state.
 *
 * Note: The Sparc platform requires that arg_state begin on an int
 * word boundary; otherwise a bus error will occur. Even so, lint will
 * complain about mis-alignment, but you should disregard these messages.
 */
void Random::initState(uint32_t s, char* argState, std::int32_t n)
{
    auto intArgState = reinterpret_cast<uint32_t*>(argState);

    if (n < BREAK_0) {
        throw std::logic_error("Random: not enough state");
        return;
    }
    if (n < BREAK_1) {
        _randType = TYPE_0;
        _randDeg = DEG_0;
        _randSep = SEP_0;
    } else if (n < BREAK_2) {
        _randType = TYPE_1;
        _randDeg = DEG_1;
        _randSep = SEP_1;
    } else if (n < BREAK_3) {
        _randType = TYPE_2;
        _randDeg = DEG_2;
        _randSep = SEP_2;
    } else if (n < BREAK_4) {
        _randType = TYPE_3;
        _randDeg = DEG_3;
        _randSep = SEP_3;
    } else {
        _randType = TYPE_4;
        _randDeg = DEG_4;
        _randSep = SEP_4;
    }
    _state = intArgState + 1;    /* first location */
    _endPtr = &_state[_randDeg]; /* must set end_ptr before seed */
    seed(s);
    if (_randType == TYPE_0)
        intArgState[0] = _randType;
    else
        intArgState[0] = MAX_TYPES * static_cast<int>(_rptr - _state) + _randType;
}


/*
 * Next:
 *
 * If we are using the trivial TYPE_0 R.N.G., just do the old linear
 * congruential bit.  Otherwise, we do our fancy trinomial stuff, which is
 * the same in all the other cases due to all the global variables that have
 * been set up.  The basic operation is to add the number at the rear pointer
 * into the one at the front pointer.  Then both pointers are advanced to
 * the next location cyclically in the table.  The value returned is the sum
 * generated, reduced to 31 bits by throwing away the "least random" low bit.
 *
 * Note: the code takes advantage of the fact that both the front and
 * rear pointers can't wrap on the same call by not testing the rear
 * pointer if the front one has wrapped.
 *
 * Returns a 31-bit random number.
 */
uint32_t Random::next()
{
    uint32_t i;
    uint32_t *f, *r;

    if (_randType == TYPE_0) {
        i = _state[0];
        _state[0] = i = goodRand(i) & 0x7FFFFFFF;
    } else {
        /*
         * Use local variables rather than static variables for speed.
         */
        f = _fptr;
        r = _rptr;
        *f += *r;
        i = (*f >> 1) & 0x7FFFFFFF; /* chucking least random bit */
        if (++f >= _endPtr) {
            f = _state;
            ++r;
        } else if (++r >= _endPtr) {
            r = _state;
        }

        _fptr = f;
        _rptr = r;
    }
    return i;
}


uint32_t Random::next(uint32_t n)
{
    return next() % n;
}


char Random::nextChar()
{
    return char((next() >> 3) & 0xFF);
}


bool Random::nextBool()
{
    return (next() & 0x1000) != 0;
}


float Random::nextFloat()
{
    return float(next()) / 0x7FFFFFFF;
}


double Random::nextDouble()
{
    return double(next()) / 0x7FFFFFFF;
}


} // namespace scy


/// @\}


//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
