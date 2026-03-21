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


#include "icy/base.h"
#include "icy/byteorder.h"

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>


namespace icy {


/// Core buffer type.
using Buffer = std::vector<char>;


//
// Mutable Buffer
//


/// The MutableBuffer class provides a safe representation of a
/// buffer that can be modified. It does not own the underlying
/// data, and so is cheap to copy or assign.
class Base_API MutableBuffer
{
public:
    /// Construct an empty buffer.
    constexpr MutableBuffer()
        : _data(nullptr)
        , _size(0)
    {
    }

    /// Construct a buffer to represent the given memory range.
    constexpr MutableBuffer(void* data, size_t size)
        : _data(data)
        , _size(size)
    {
    }

    [[nodiscard]] constexpr void* data() const { return _data; }
    [[nodiscard]] constexpr size_t size() const { return _size; }

    /// Cast the buffer as a char pointer.
    [[nodiscard]] char* cstr() const { return reinterpret_cast<char*>(_data); }

    /// Returns the buffer as a string.
    std::string str() const { return std::string(cstr(), size()); }

protected:
    void* _data;
    size_t _size;
};


// Warning: The following functions permit violations of type safety,
// so uses of it in application code should be carefully considered.


/// Creates a `MutableBuffer` from an arbitrary pointer and size.
/// @tparam T Pointer type; must be implicitly castable to `void*`.
/// @param data Pointer to the start of the memory region.
/// @param size Number of bytes in the region.
/// @return `MutableBuffer` referencing the given memory.
template <typename T>
inline MutableBuffer mutableBuffer(T data, size_t size)
{
    return MutableBuffer(reinterpret_cast<void*>(data), size);
}

/// Creates a `MutableBuffer` from a `std::string`.
/// @param str Source string. Must remain valid while the buffer is in use.
/// @return `MutableBuffer` wrapping the string's internal storage.
inline MutableBuffer mutableBuffer(std::string& str)
{
    // std::string is contiguous as of C++11
    return MutableBuffer(reinterpret_cast<void*>(&str[0]), str.size());
}

/// Creates a `MutableBuffer` from a const `std::string`.
/// Casts away constness; use with care.
/// @param str Source string. Must remain valid while the buffer is in use.
/// @return `MutableBuffer` wrapping the string's internal storage.
inline MutableBuffer mutableBuffer(const std::string& str)
{
    return MutableBuffer(reinterpret_cast<void*>(const_cast<char*>(&str[0])), str.size()); // careful!
}

/// Creates a `MutableBuffer` from a const `std::vector`.
/// Casts away constness; use with care.
/// @tparam T Element type of the vector.
/// @param vec Source vector. Must remain valid while the buffer is in use.
/// @return `MutableBuffer` wrapping the vector's internal storage.
template <typename T>
inline MutableBuffer mutableBuffer(const std::vector<T>& vec)
{
    return MutableBuffer(reinterpret_cast<void*>(const_cast<T>(&vec[0])), vec.size()); // careful!
}

/// Creates a `MutableBuffer` from a `Buffer`.
/// @param buf Source buffer. Must remain valid while the buffer is in use.
/// @return `MutableBuffer` wrapping the buffer's data.
inline MutableBuffer mutableBuffer(Buffer& buf)
{
    return MutableBuffer(reinterpret_cast<void*>(buf.data()), buf.size());
}

/// Creates a `MutableBuffer` from a const `Buffer`.
/// Casts away constness; use with care.
/// @param buf Source buffer. Must remain valid while the buffer is in use.
/// @return `MutableBuffer` wrapping the buffer's data.
inline MutableBuffer mutableBuffer(const Buffer& buf)
{
    return MutableBuffer(reinterpret_cast<void*>(const_cast<char*>(buf.data())), buf.size());
}


//
// Const Buffer
//


/// The ConstBuffer class provides a safe representation of a
/// buffer that cannot be modified. It does not own the underlying
/// data, and so is cheap to copy or assign.
class Base_API ConstBuffer
{
public:
    /// Construct an empty buffer.
    constexpr ConstBuffer()
        : _data(nullptr)
        , _size(0)
    {
    }

    /// Construct a buffer to represent the given memory range.
    constexpr ConstBuffer(const void* data, size_t size)
        : _data(data)
        , _size(size)
    {
    }

    /// Construct a non-modifiable buffer from a modifiable one.
    constexpr ConstBuffer(const MutableBuffer& b)
        : _data(b.data())
        , _size(b.size())
    {
    }

    [[nodiscard]] constexpr const void* data() const { return _data; }
    [[nodiscard]] constexpr size_t size() const { return _size; }

    /// Cast the buffer as a const char pointer.
    [[nodiscard]] const char* cstr() const { return reinterpret_cast<const char*>(_data); }

    /// Returns the buffer as a string.
    std::string str() const { return std::string(cstr(), size()); }

protected:
    const void* _data;
    size_t _size;
};


/// Creates a `ConstBuffer` from an arbitrary pointer and size.
/// @tparam T Pointer type; must be implicitly castable to `const void*`.
/// @param data Pointer to the start of the memory region.
/// @param size Number of bytes in the region.
/// @return `ConstBuffer` referencing the given memory.
template <typename T>
inline ConstBuffer constBuffer(T data, size_t size)
{
    return ConstBuffer(reinterpret_cast<const void*>(data), size);
}

/// Creates a `ConstBuffer` from a `std::string`.
/// @param str Source string. Must remain valid while the buffer is in use.
/// @return `ConstBuffer` wrapping the string's internal storage.
inline ConstBuffer constBuffer(const std::string& str)
{
    return ConstBuffer(reinterpret_cast<const void*>(&str[0]), str.size()); // careful!
}

/// Creates a `ConstBuffer` from a `std::vector`.
/// @tparam T Element type of the vector.
/// @param vec Source vector. Must remain valid while the buffer is in use.
/// @return `ConstBuffer` wrapping the vector's internal storage.
template <typename T>
inline ConstBuffer constBuffer(const std::vector<T>& vec)
{
    return ConstBuffer(reinterpret_cast<const void*>(&vec[0]), vec.size()); // careful!
}

/// Creates a `ConstBuffer` from a `MutableBuffer`.
/// @param buf Source mutable buffer.
/// @return `ConstBuffer` referencing the same memory region.
inline constexpr ConstBuffer constBuffer(const MutableBuffer& buf)
{
    return ConstBuffer(buf.data(), buf.size());
}

/// Creates a `ConstBuffer` from a `Buffer`.
/// @tparam T Unused; kept for overload symmetry.
/// @param buf Source buffer. Must remain valid while the buffer is in use.
/// @return `ConstBuffer` wrapping the buffer's data.
template <typename T>
inline ConstBuffer constBuffer(Buffer& buf)
{
    return ConstBuffer(reinterpret_cast<void*>(buf.data()), buf.size());
}

/// Creates a `ConstBuffer` from a const `Buffer`.
/// Casts away constness internally; use with care.
/// @tparam T Unused; kept for overload symmetry.
/// @param buf Source buffer. Must remain valid while the buffer is in use.
/// @return `ConstBuffer` wrapping the buffer's data.
template <typename T>
inline ConstBuffer constBuffer(const Buffer& buf)
{
    return ConstBuffer(reinterpret_cast<void*>(const_cast<char*>(buf.data())), buf.size());
}


//
// Buffer Cast
//


/// Casts a `MutableBuffer` to a specified pointer-to-POD type.
/// @tparam PointerToPodType Target pointer type (e.g. `char*`, `uint8_t*`).
/// @param b Source mutable buffer.
/// @return Pointer to the buffer's data, cast to `PointerToPodType`.
template <typename PointerToPodType>
inline constexpr PointerToPodType bufferCast(const MutableBuffer& b)
{
    return static_cast<PointerToPodType>(b.data());
}


/// Casts a `ConstBuffer` to a specified pointer-to-POD type.
/// @tparam PointerToPodType Target pointer type (e.g. `const char*`, `const uint8_t*`).
/// @param b Source const buffer.
/// @return Pointer to the buffer's data, cast to `PointerToPodType`.
template <typename PointerToPodType>
inline constexpr PointerToPodType bufferCast(const ConstBuffer& b)
{
    return static_cast<PointerToPodType>(b.data());
}


//
// Bit Reader
//


/// Class for reading binary streams.
class Base_API BitReader
{
public:
    /// Constructs a `BitReader` over a raw byte array.
    /// @param bytes Pointer to the start of the data. Must remain valid for the lifetime of the reader.
    /// @param size Number of bytes available for reading.
    /// @param order Byte order used when reading multi-byte integer types.
    BitReader(const char* bytes, size_t size, ByteOrder order = ByteOrder::Network);

    /// Constructs a `BitReader` over a `Buffer`.
    /// @param buf Source buffer. Must remain valid for the lifetime of the reader.
    /// @param order Byte order used when reading multi-byte integer types.
    BitReader(const Buffer& buf, ByteOrder order = ByteOrder::Network);

    /// Constructs a `BitReader` over a `ConstBuffer`.
    /// @param pod Source const buffer. Must remain valid for the lifetime of the reader.
    /// @param order Byte order used when reading multi-byte integer types.
    BitReader(const ConstBuffer& pod, ByteOrder order = ByteOrder::Network);
    ~BitReader();

    /// Reads a value from the BitReader.
    /// Returns false if there isn't enough data left for the specified type.
    /// Throws a std::out_of_range exception if reading past the limit.
    /// Reads `len` raw bytes into `val`. Throws `std::out_of_range` if insufficient data remains.
    /// @param val Destination buffer; must have capacity of at least `len` bytes.
    /// @param len Number of bytes to read.
    void get(char* val, size_t len);

    /// Reads `len` bytes and appends them to `val`. Throws `std::out_of_range` if insufficient data remains.
    /// @param val String to append the read bytes to.
    /// @param len Number of bytes to read.
    void get(std::string& val, size_t len);

    /// Reads an unsigned 8-bit integer. Throws `std::out_of_range` if insufficient data remains.
    /// @param val Output parameter receiving the read value.
    void getU8(uint8_t& val);

    /// Reads an unsigned 16-bit integer, applying byte-order conversion. Throws `std::out_of_range` if insufficient data remains.
    /// @param val Output parameter receiving the read value.
    void getU16(uint16_t& val);

    /// Reads an unsigned 24-bit integer into a 32-bit variable, applying byte-order conversion. Throws `std::out_of_range` if insufficient data remains.
    /// @param val Output parameter receiving the read value.
    void getU24(uint32_t& val);

    /// Reads an unsigned 32-bit integer, applying byte-order conversion. Throws `std::out_of_range` if insufficient data remains.
    /// @param val Output parameter receiving the read value.
    void getU32(uint32_t& val);

    /// Reads an unsigned 64-bit integer, applying byte-order conversion. Throws `std::out_of_range` if insufficient data remains.
    /// @param val Output parameter receiving the read value.
    void getU64(uint64_t& val);

    /// Peeks at the current byte without advancing the position.
    /// @return Current byte as `char`, or 0 if at the end of the buffer.
    char peek();

    /// Peeks at the current byte as a `uint8_t` without advancing the position.
    /// @return Current value, or 0 if at the end of the buffer.
    uint8_t peekU8();

    /// Peeks at the next two bytes as a `uint16_t` without advancing the position.
    /// @return Current value with byte-order conversion applied, or 0 on failure.
    uint16_t peekU16();

    /// Peeks at the next three bytes as a `uint32_t` without advancing the position.
    /// @return Current value with byte-order conversion applied, or 0 on failure.
    uint32_t peekU24();

    /// Peeks at the next four bytes as a `uint32_t` without advancing the position.
    /// @return Current value with byte-order conversion applied, or 0 on failure.
    uint32_t peekU32();

    /// Peeks data from the BitReader.
    /// -1 is returned if reading past boundary.
    uint64_t peekU64();

    //
    // String parsing methods.
    //

    /// Advances the position until the given character is found, stopping before it.
    /// @param c Character to search for.
    /// @return Number of bytes skipped.
    size_t skipToChar(char c);

    /// Advances the position past any leading space characters.
    /// @return Number of bytes skipped.
    size_t skipWhitespace();

    /// Advances the position past the end of the current line (past the newline).
    /// @return Number of bytes skipped including the newline character.
    size_t skipToNextLine();

    /// Advances the position past the next whitespace-delimited word.
    /// @return Number of bytes skipped.
    size_t skipNextWord();

    /// Reads the next whitespace-delimited word into `val`.
    /// @param val String to receive the word.
    /// @return Number of bytes consumed.
    size_t readNextWord(std::string& val);

    /// Reads the next whitespace-delimited decimal number into `val`.
    /// @param val Output parameter receiving the parsed unsigned integer.
    /// @return Number of bytes consumed.
    size_t readNextNumber(unsigned int& val);

    /// Reads bytes up to (but not including) the next newline into `val`.
    /// @param val String to receive the line content.
    /// @return Number of bytes consumed including the newline.
    size_t readLine(std::string& val);

    /// Reads bytes up to (but not including) the next occurrence of `c` into `val`.
    /// @param val String to receive the read bytes.
    /// @param c Delimiter character to stop at.
    /// @return Number of bytes consumed.
    size_t readToNext(std::string& val, char c);

    /// Set position pointer to absolute position.
    /// Throws a std::out_of_range exception if the value exceeds the limit.
    void seek(size_t val);

    /// Set position pointer to relative position.
    /// Throws a std::out_of_range exception if the value exceeds the limit.
    void skip(size_t size);

    /// Returns the read limit.
    [[nodiscard]] size_t limit() const;

    /// Returns the current read position.
    [[nodiscard]] size_t position() const { return _position; }

    /// Returns the number of elements between the current position and the
    /// limit.
    [[nodiscard]] size_t available() const;

    /// Returns a pointer to the start of the buffer.
    [[nodiscard]] const char* begin() const { return _bytes; }

    /// Returns a pointer to the current read position.
    [[nodiscard]] const char* current() const { return _bytes + _position; }

    /// Returns the byte order used for multi-byte integer reads.
    ByteOrder order() const { return _order; }

    /// Returns the remaining unread bytes as a `std::string`.
    /// @return String containing bytes from the current position to the end.
    std::string toString();

    friend std::ostream& operator<<(std::ostream& stream, const BitReader& buf)
    {
        return stream.write(buf.current(), buf.position());
    }

protected:
    void init(const char* bytes, size_t size, ByteOrder order); // nocopy

    size_t _position;
    size_t _limit;
    ByteOrder _order;
    const char* _bytes;
};


//
// Bit Writer
//


/// Class for reading/writing binary streams.
///
/// Note that when using the constructor with the Buffer reference
/// as an argument, the writer will dynamically expand the given buffer
/// when writing passed the buffer capacity.
/// All other cases will throw a std::out_of_range error when writing
/// past the buffer capacity.
class Base_API BitWriter
{
public:
    /// Constructs a `BitWriter` over a raw byte array with a fixed capacity.
    /// @param bytes Pointer to the writable buffer. Must remain valid for the writer's lifetime.
    /// @param size Capacity of the buffer in bytes.
    /// @param order Byte order used for multi-byte integer writes.
    BitWriter(char* bytes, size_t size, ByteOrder order = ByteOrder::Network);

    /// Constructs a `BitWriter` backed by a `Buffer`.
    /// Writes are bounded by the buffer's capacity; use `DynamicBitWriter` for auto-resize.
    /// @param buf Source buffer. Must remain valid for the writer's lifetime.
    /// @param order Byte order used for multi-byte integer writes.
    BitWriter(Buffer& buf, ByteOrder order = ByteOrder::Network);

    /// Constructs a `BitWriter` over a `MutableBuffer` with a fixed capacity.
    /// @param pod Source mutable buffer. Must remain valid for the writer's lifetime.
    /// @param order Byte order used for multi-byte integer writes.
    BitWriter(MutableBuffer& pod, ByteOrder order = ByteOrder::Network);
    virtual ~BitWriter();

    /// Append bytes to the buffer.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    virtual void put(const char* val, size_t len);
    /// Appends the contents of a string. Throws `std::out_of_range` if capacity is exceeded.
    /// @param val String whose bytes are appended.
    void put(const std::string& val);

    /// Appends an unsigned 8-bit integer. Throws `std::out_of_range` if capacity is exceeded.
    /// @param val Value to write.
    void putU8(uint8_t val);

    /// Appends an unsigned 16-bit integer with byte-order conversion. Throws `std::out_of_range` if capacity is exceeded.
    /// @param val Value to write.
    void putU16(uint16_t val);

    /// Appends the low 24 bits of a 32-bit integer with byte-order conversion. Throws `std::out_of_range` if capacity is exceeded.
    /// @param val Value to write (only the lower 3 bytes are written).
    void putU24(uint32_t val);

    /// Appends an unsigned 32-bit integer with byte-order conversion. Throws `std::out_of_range` if capacity is exceeded.
    /// @param val Value to write.
    void putU32(uint32_t val);

    /// Appends an unsigned 64-bit integer with byte-order conversion. Throws `std::out_of_range` if capacity is exceeded.
    /// @param val Value to write.
    void putU64(uint64_t val);

    /// Update a byte range.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    virtual bool update(const char* val, size_t len, size_t pos);
    /// Overwrites a previously written string at the given absolute position.
    /// @param val String to write.
    /// @param pos Absolute byte offset to overwrite at.
    /// @return True on success, false if the range exceeds available space.
    bool update(const std::string& val, size_t pos);

    /// Overwrites a `uint8_t` at the given absolute position.
    /// @param val Value to write.
    /// @param pos Absolute byte offset to overwrite at.
    /// @return True on success, false if the range exceeds available space.
    bool updateU8(uint8_t val, size_t pos);

    /// Overwrites a `uint16_t` at the given absolute position, with byte-order conversion.
    /// @param val Value to write.
    /// @param pos Absolute byte offset to overwrite at.
    /// @return True on success, false if the range exceeds available space.
    bool updateU16(uint16_t val, size_t pos);

    /// Overwrites 3 bytes (low 24 bits of `val`) at the given absolute position, with byte-order conversion.
    /// @param val Value to write.
    /// @param pos Absolute byte offset to overwrite at.
    /// @return True on success, false if the range exceeds available space.
    bool updateU24(uint32_t val, size_t pos);

    /// Overwrites a `uint32_t` at the given absolute position, with byte-order conversion.
    /// @param val Value to write.
    /// @param pos Absolute byte offset to overwrite at.
    /// @return True on success, false if the range exceeds available space.
    bool updateU32(uint32_t val, size_t pos);

    /// Overwrites a `uint64_t` at the given absolute position, with byte-order conversion.
    /// @param val Value to write.
    /// @param pos Absolute byte offset to overwrite at.
    /// @return True on success, false if the range exceeds available space.
    bool updateU64(uint64_t val, size_t pos);

    /// Set position pointer to absolute position.
    /// Throws a `std::out_of_range` exception if the value exceeds the limit.
    void seek(size_t val);

    /// Set position pointer to relative position.
    /// Throws a `std::out_of_range` exception if the value exceeds the limit.
    void skip(size_t size);

    /// Returns the write limit.
    [[nodiscard]] size_t limit() const;

    /// Returns the current write position.
    [[nodiscard]] size_t position() const { return _position; }

    /// Returns the number of elements between the current write position and
    /// the limit.
    [[nodiscard]] size_t available() const;

    /// Returns a pointer to the start of the write buffer.
    [[nodiscard]] char* begin() { return _bytes; }

    /// Returns a pointer to the current write position.
    [[nodiscard]] char* current() { return _bytes + _position; }

    /// Returns a const pointer to the start of the write buffer.
    [[nodiscard]] const char* begin() const { return _bytes; }

    /// Returns a const pointer to the current write position.
    [[nodiscard]] const char* current() const { return _bytes + _position; }

    /// Returns the byte order used for multi-byte integer writes.
    ByteOrder order() const { return _order; }

    /// Returns all bytes written so far as a `std::string`.
    /// @return String containing bytes from the start of the buffer up to the current position.
    std::string toString();

    friend std::ostream& operator<<(std::ostream& stream, const BitWriter& wr)
    {
        return stream.write(wr.begin(), wr.position());
    }

protected:
    virtual void init(char* bytes, size_t size, ByteOrder order); // nocopy

    size_t _position;
    size_t _limit;
    ByteOrder _order;
    char* _bytes;
};


/// Class for reading/writing dynamically resizable binary streams.
///
/// Note that when using the constructor with the Buffer reference
/// as an argument, the writer will dynamically expand the given buffer
/// when writing passed the buffer capacity.
/// All other cases will throw a std::out_of_range error when writing
/// past the buffer capacity.
class Base_API DynamicBitWriter : public BitWriter
{
public:
    /// Constructs a `DynamicBitWriter` backed by a dynamically resizable `Buffer`.
    /// The buffer is expanded automatically as data is written.
    /// @param buf Buffer to write into; expanded as needed.
    /// @param order Byte order used for multi-byte integer writes.
    DynamicBitWriter(Buffer& buf, ByteOrder order = ByteOrder::Network);

    /// Constructs a `DynamicBitWriter` that inserts data starting at a specific iterator position.
    /// @param buf Buffer to write into; expanded as needed.
    /// @param offset Iterator into `buf` indicating the insertion start point.
    /// @param order Byte order used for multi-byte integer writes.
    DynamicBitWriter(Buffer& buf, Buffer::iterator offset, ByteOrder order = ByteOrder::Network);
    virtual ~DynamicBitWriter();

    /// Append bytes to the buffer.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    void put(const char* val, size_t len) override;

    /// Update a byte range.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    bool update(const char* val, size_t len, size_t pos) override;

protected:
    Buffer& _buffer;
    size_t _offset;
};


} // namespace icy
