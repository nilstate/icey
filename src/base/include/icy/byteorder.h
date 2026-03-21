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


/// Writes a single byte at the given offset in memory.
/// @param memory Pointer to the destination buffer.
/// @param offset Byte offset within the buffer.
/// @param v      Value to write.
inline void set8(void* memory, size_t offset, uint8_t v)
{
    static_cast<uint8_t*>(memory)[offset] = v;
}

/// Reads a single byte at the given offset from memory.
/// @param memory Pointer to the source buffer.
/// @param offset Byte offset within the buffer.
/// @return The byte value at the specified offset.
inline uint8_t get8(const void* memory, size_t offset)
{
    return static_cast<const uint8_t*>(memory)[offset];
}

/// Writes a 16-bit value to memory in big-endian byte order.
/// @param memory Pointer to the destination buffer (must be at least 2 bytes).
/// @param v      Value to write.
inline void setBE16(void* memory, uint16_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 8));
    set8(memory, 1, static_cast<uint8_t>(v >> 0));
}

/// Writes a 32-bit value to memory in big-endian byte order.
/// @param memory Pointer to the destination buffer (must be at least 4 bytes).
/// @param v      Value to write.
inline void setBE32(void* memory, uint32_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 24));
    set8(memory, 1, static_cast<uint8_t>(v >> 16));
    set8(memory, 2, static_cast<uint8_t>(v >> 8));
    set8(memory, 3, static_cast<uint8_t>(v >> 0));
}

/// Writes a 64-bit value to memory in big-endian byte order.
/// @param memory Pointer to the destination buffer (must be at least 8 bytes).
/// @param v      Value to write.
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

/// Reads a 16-bit big-endian value from memory.
/// @param memory Pointer to the source buffer (must be at least 2 bytes).
/// @return The 16-bit value in host byte order.
inline uint16_t getBE16(const void* memory)
{
    return static_cast<uint16_t>((get8(memory, 0) << 8) |
                                 (get8(memory, 1) << 0));
}

/// Reads a 32-bit big-endian value from memory.
/// @param memory Pointer to the source buffer (must be at least 4 bytes).
/// @return The 32-bit value in host byte order.
inline uint32_t getBE32(const void* memory)
{
    return (static_cast<uint32_t>(get8(memory, 0)) << 24) |
           (static_cast<uint32_t>(get8(memory, 1)) << 16) |
           (static_cast<uint32_t>(get8(memory, 2)) << 8) |
           (static_cast<uint32_t>(get8(memory, 3)) << 0);
}

/// Reads a 64-bit big-endian value from memory.
/// @param memory Pointer to the source buffer (must be at least 8 bytes).
/// @return The 64-bit value in host byte order.
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

/// Writes a 16-bit value to memory in little-endian byte order.
/// @param memory Pointer to the destination buffer (must be at least 2 bytes).
/// @param v      Value to write.
inline void setLE16(void* memory, uint16_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 0));
    set8(memory, 1, static_cast<uint8_t>(v >> 8));
}

/// Writes a 32-bit value to memory in little-endian byte order.
/// @param memory Pointer to the destination buffer (must be at least 4 bytes).
/// @param v      Value to write.
inline void setLE32(void* memory, uint32_t v)
{
    set8(memory, 0, static_cast<uint8_t>(v >> 0));
    set8(memory, 1, static_cast<uint8_t>(v >> 8));
    set8(memory, 2, static_cast<uint8_t>(v >> 16));
    set8(memory, 3, static_cast<uint8_t>(v >> 24));
}

/// Writes a 64-bit value to memory in little-endian byte order.
/// @param memory Pointer to the destination buffer (must be at least 8 bytes).
/// @param v      Value to write.
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

/// Reads a 16-bit little-endian value from memory.
/// @param memory Pointer to the source buffer (must be at least 2 bytes).
/// @return The 16-bit value in host byte order.
inline uint16_t getLE16(const void* memory)
{
    return static_cast<uint16_t>((get8(memory, 0) << 0) |
                                 (get8(memory, 1) << 8));
}

/// Reads a 32-bit little-endian value from memory.
/// @param memory Pointer to the source buffer (must be at least 4 bytes).
/// @return The 32-bit value in host byte order.
inline uint32_t getLE32(const void* memory)
{
    return (static_cast<uint32_t>(get8(memory, 0)) << 0) |
           (static_cast<uint32_t>(get8(memory, 1)) << 8) |
           (static_cast<uint32_t>(get8(memory, 2)) << 16) |
           (static_cast<uint32_t>(get8(memory, 3)) << 24);
}

/// Reads a 64-bit little-endian value from memory.
/// @param memory Pointer to the source buffer (must be at least 8 bytes).
/// @return The 64-bit value in host byte order.
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

/// Returns true if the host CPU is big-endian.
/// @return true if the host byte order is big-endian, false if little-endian.
inline bool isBigEndian()
{
    static const int number = 1;
    return 0 == *reinterpret_cast<const char*>(&number);
}

/// Converts a 16-bit value from host byte order to network (big-endian) byte order.
/// @param n Value in host byte order.
/// @return Value in network byte order.
inline uint16_t hostToNetwork16(uint16_t n)
{
    uint16_t result;
    setBE16(&result, n);
    return result;
}

/// Converts a 32-bit value from host byte order to network (big-endian) byte order.
/// @param n Value in host byte order.
/// @return Value in network byte order.
inline uint32_t hostToNetwork32(uint32_t n)
{
    uint32_t result;
    setBE32(&result, n);
    return result;
}

/// Converts a 64-bit value from host byte order to network (big-endian) byte order.
/// @param n Value in host byte order.
/// @return Value in network byte order.
inline uint64_t hostToNetwork64(uint64_t n)
{
    uint64_t result;
    setBE64(&result, n);
    return result;
}

/// Converts a 16-bit value from network (big-endian) byte order to host byte order.
/// @param n Value in network byte order.
/// @return Value in host byte order.
inline uint16_t networkToHost16(uint16_t n)
{
    return getBE16(&n);
}

/// Converts a 32-bit value from network (big-endian) byte order to host byte order.
/// @param n Value in network byte order.
/// @return Value in host byte order.
inline uint32_t networkToHost32(uint32_t n)
{
    return getBE32(&n);
}

/// Converts a 64-bit value from network (big-endian) byte order to host byte order.
/// @param n Value in network byte order.
/// @return Value in host byte order.
inline uint64_t networkToHost64(uint64_t n)
{
    return getBE64(&n);
}


} // namespace icy


/// @\}
