///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include <cstdint>
#include <cstring>

#ifdef POSIX
#include <arpa/inet.h>
#endif

#ifdef ICY_WIN
#include <cstdlib>
#endif


namespace icy {


enum class ByteOrder
{
    Network = 0, ///< Default, use network byte order (big endian).
    Host,        ///< Use the native order of the host.
};


inline void set8(void* memory, size_t offset, uint8_t v)
{
    static_cast<uint8_t*>(memory)[offset] = v;
}

inline uint8_t get8(const void* memory, size_t offset)
{
    return static_cast<const uint8_t*>(memory)[offset];
}

inline void setBE16(void* memory, uint16_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 8));
    set8(memory, 1, static_cast<uint8_t>(v >> 0));
}

inline void setBE32(void* memory, uint32_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 24));
    set8(memory, 1, static_cast<uint8_t>(v >> 16));
    set8(memory, 2, static_cast<uint8_t>(v >> 8));
    set8(memory, 3, static_cast<uint8_t>(v >> 0));
}

inline void setBE64(void* memory, uint64_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 56));
    set8(memory, 1, static_cast<uint8_t>(v >> 48));
    set8(memory, 2, static_cast<uint8_t>(v >> 40));
    set8(memory, 3, static_cast<uint8_t>(v >> 32));
    set8(memory, 4, static_cast<uint8_t>(v >> 24));
    set8(memory, 5, static_cast<uint8_t>(v >> 16));
    set8(memory, 6, static_cast<uint8_t>(v >> 8));
    set8(memory, 7, static_cast<uint8_t>(v >> 0));
}

inline uint16_t getBE16(const void* memory)
{
    return static_cast<uint16_t>((get8(memory, 0) << 8) |
                                 (get8(memory, 1) << 0));
}

inline uint32_t getBE32(const void* memory)
{
    return (static_cast<uint32_t>(get8(memory, 0)) << 24) |
           (static_cast<uint32_t>(get8(memory, 1)) << 16) |
           (static_cast<uint32_t>(get8(memory, 2)) << 8) |
           (static_cast<uint32_t>(get8(memory, 3)) << 0);
}

inline uint64_t getBE64(const void* memory)
{
    return (static_cast<uint64_t>(get8(memory, 0)) << 56) |
           (static_cast<uint64_t>(get8(memory, 1)) << 48) |
           (static_cast<uint64_t>(get8(memory, 2)) << 40) |
           (static_cast<uint64_t>(get8(memory, 3)) << 32) |
           (static_cast<uint64_t>(get8(memory, 4)) << 24) |
           (static_cast<uint64_t>(get8(memory, 5)) << 16) |
           (static_cast<uint64_t>(get8(memory, 6)) << 8) |
           (static_cast<uint64_t>(get8(memory, 7)) << 0);
}

inline void setLE16(void* memory, uint16_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 0));
    set8(memory, 1, static_cast<uint8_t>(v >> 8));
}

inline void setLE32(void* memory, uint32_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 0));
    set8(memory, 1, static_cast<uint8_t>(v >> 8));
    set8(memory, 2, static_cast<uint8_t>(v >> 16));
    set8(memory, 3, static_cast<uint8_t>(v >> 24));
}

inline void setLE64(void* memory, uint64_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 0));
    set8(memory, 1, static_cast<uint8_t>(v >> 8));
    set8(memory, 2, static_cast<uint8_t>(v >> 16));
    set8(memory, 3, static_cast<uint8_t>(v >> 24));
    set8(memory, 4, static_cast<uint8_t>(v >> 32));
    set8(memory, 5, static_cast<uint8_t>(v >> 40));
    set8(memory, 6, static_cast<uint8_t>(v >> 48));
    set8(memory, 7, static_cast<uint8_t>(v >> 56));
}

inline uint16_t getLE16(const void* memory)
{
    return static_cast<uint16_t>((get8(memory, 0) << 0) |
                                 (get8(memory, 1) << 8));
}

inline uint32_t getLE32(const void* memory)
{
    return (static_cast<uint32_t>(get8(memory, 0)) << 0) |
           (static_cast<uint32_t>(get8(memory, 1)) << 8) |
           (static_cast<uint32_t>(get8(memory, 2)) << 16) |
           (static_cast<uint32_t>(get8(memory, 3)) << 24);
}

inline uint64_t getLE64(const void* memory)
{
    return (static_cast<uint64_t>(get8(memory, 0)) << 0) |
           (static_cast<uint64_t>(get8(memory, 1)) << 8) |
           (static_cast<uint64_t>(get8(memory, 2)) << 16) |
           (static_cast<uint64_t>(get8(memory, 3)) << 24) |
           (static_cast<uint64_t>(get8(memory, 4)) << 32) |
           (static_cast<uint64_t>(get8(memory, 5)) << 40) |
           (static_cast<uint64_t>(get8(memory, 6)) << 48) |
           (static_cast<uint64_t>(get8(memory, 7)) << 56);
}

// Check if the current host is big endian.
inline bool isBigEndian()
{
    static const int number = 1;
    return 0 == *reinterpret_cast<const char*>(&number);
}

inline uint16_t hostToNetwork16(uint16_t n)
{
    uint16_t result;
    setBE16(&result, n);
    return result;
}

inline uint32_t hostToNetwork32(uint32_t n)
{
    uint32_t result;
    setBE32(&result, n);
    return result;
}

inline uint64_t hostToNetwork64(uint64_t n)
{
    uint64_t result;
    setBE64(&result, n);
    return result;
}

inline uint16_t networkToHost16(uint16_t n)
{
    return getBE16(&n);
}

inline uint32_t networkToHost32(uint32_t n)
{
    return getBE32(&n);
}

inline uint64_t networkToHost64(uint64_t n)
{
    return getBE64(&n);
}


} // namespace icy


/// @\}
