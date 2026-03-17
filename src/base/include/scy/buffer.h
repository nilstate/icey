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
#include "scy/byteorder.h"

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>


namespace scy {


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


template <typename T>
inline MutableBuffer mutableBuffer(T data, size_t size)
{
    return MutableBuffer(reinterpret_cast<void*>(data), size);
}

inline MutableBuffer mutableBuffer(std::string& str)
{
    // std::string is contiguous as of C++11
    return MutableBuffer(reinterpret_cast<void*>(&str[0]), str.size());
}


inline MutableBuffer mutableBuffer(const std::string& str)
{
    return MutableBuffer(reinterpret_cast<void*>(const_cast<char*>(&str[0])), str.size()); // careful!
}


template <typename T>
inline MutableBuffer mutableBuffer(const std::vector<T>& vec)
{
    return MutableBuffer(reinterpret_cast<void*>(const_cast<T>(&vec[0])), vec.size()); // careful!
}


inline MutableBuffer mutableBuffer(Buffer& buf)
{
    return MutableBuffer(reinterpret_cast<void*>(buf.data()), buf.size());
}


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


template <typename T>
inline ConstBuffer constBuffer(T data, size_t size)
{
    return ConstBuffer(reinterpret_cast<const void*>(data), size);
}

inline ConstBuffer constBuffer(const std::string& str)
{
    return ConstBuffer(reinterpret_cast<const void*>(&str[0]), str.size()); // careful!
}

template <typename T>
inline ConstBuffer constBuffer(const std::vector<T>& vec)
{
    return ConstBuffer(reinterpret_cast<const void*>(&vec[0]), vec.size()); // careful!
}

inline constexpr ConstBuffer constBuffer(const MutableBuffer& buf)
{
    return ConstBuffer(buf.data(), buf.size());
}

template <typename T>
inline ConstBuffer constBuffer(Buffer& buf)
{
    return ConstBuffer(reinterpret_cast<void*>(buf.data()), buf.size());
}

template <typename T>
inline ConstBuffer constBuffer(const Buffer& buf)
{
    return ConstBuffer(reinterpret_cast<void*>(const_cast<char*>(buf.data())), buf.size());
}


//
// Buffer Cast
//


/// Cast a non-modifiable buffer to a specified pointer to POD type.
template <typename PointerToPodType>
inline constexpr PointerToPodType bufferCast(const MutableBuffer& b)
{
    return static_cast<PointerToPodType>(b.data());
}


/// Cast a non-modifiable buffer to a specified pointer to POD type.
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
    BitReader(const char* bytes, size_t size, ByteOrder order = ByteOrder::Network);
    BitReader(const Buffer& buf, ByteOrder order = ByteOrder::Network);
    BitReader(const ConstBuffer& pod, ByteOrder order = ByteOrder::Network);
    ~BitReader();

    /// Reads a value from the BitReader.
    /// Returns false if there isn't enough data left for the specified type.
    /// Throws a std::out_of_range exception if reading past the limit.
    void get(char* val, size_t len);
    void get(std::string& val, size_t len);
    void getU8(uint8_t& val);
    void getU16(uint16_t& val);
    void getU24(uint32_t& val);
    void getU32(uint32_t& val);

    void getU64(uint64_t& val);

    char peek();
    uint8_t peekU8();
    uint16_t peekU16();
    uint32_t peekU24();
    uint32_t peekU32();

    /// Peeks data from the BitReader.
    /// -1 is returned if reading past boundary.
    uint64_t peekU64();

    //
    // String parsing methods.
    //

    size_t skipToChar(char c);
    size_t skipWhitespace();
    size_t skipToNextLine();
    size_t skipNextWord();
    size_t readNextWord(std::string& val);
    size_t readNextNumber(unsigned int& val);
    size_t readLine(std::string& val);
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

    [[nodiscard]] const char* begin() const { return _bytes; }
    [[nodiscard]] const char* current() const { return _bytes + _position; }

    ByteOrder order() const { return _order; }

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
    BitWriter(char* bytes, size_t size, ByteOrder order = ByteOrder::Network);
    BitWriter(Buffer& buf, ByteOrder order = ByteOrder::Network);
    BitWriter(MutableBuffer& pod, ByteOrder order = ByteOrder::Network);
    virtual ~BitWriter();

    /// Append bytes to the buffer.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    virtual void put(const char* val, size_t len);
    void put(const std::string& val);
    void putU8(uint8_t val);
    void putU16(uint16_t val);
    void putU24(uint32_t val);
    void putU32(uint32_t val);
    void putU64(uint64_t val);

    /// Update a byte range.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    virtual bool update(const char* val, size_t len, size_t pos);
    bool update(const std::string& val, size_t pos);
    bool updateU8(uint8_t val, size_t pos);
    bool updateU16(uint16_t val, size_t pos);
    bool updateU24(uint32_t val, size_t pos);
    bool updateU32(uint32_t val, size_t pos);
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

    [[nodiscard]] char* begin() { return _bytes; }
    [[nodiscard]] char* current() { return _bytes + _position; }

    [[nodiscard]] const char* begin() const { return _bytes; }
    [[nodiscard]] const char* current() const { return _bytes + _position; }

    ByteOrder order() const { return _order; }

    /// Returns written bytes as a string.
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
    DynamicBitWriter(Buffer& buf, ByteOrder order = ByteOrder::Network);
    DynamicBitWriter(Buffer& buf, Buffer::iterator offset, ByteOrder order = ByteOrder::Network);
    virtual ~DynamicBitWriter();

    /// Append bytes to the buffer.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    virtual void put(const char* val, size_t len);

    /// Update a byte range.
    /// Throws a `std::out_of_range` exception if reading past the limit.
    virtual bool update(const char* val, size_t len, size_t pos);

protected:
    Buffer& _buffer;
    size_t _offset;
};


} // namespace scy
