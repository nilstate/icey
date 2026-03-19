///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/buffer.h"
#include "icy/byteorder.h"
#include "icy/logger.h"
#include "icy/util.h"

#include <cstddef>
#include <cstring>
#include <iterator>
#include <stdexcept>


namespace icy {


//
// Bit Reader
//


BitReader::BitReader(const ConstBuffer& pod, ByteOrder order)
{
    init(bufferCast<const char*>(pod), pod.size(), order); // copied
}


BitReader::BitReader(const Buffer& buf, ByteOrder order)
{
    init(buf.data(), buf.size(), order);
    //_buffer = &buf;
}


BitReader::BitReader(const char* bytes, size_t size, ByteOrder order)
{
    init(bytes, size, order);
}


void BitReader::init(const char* bytes, size_t size, ByteOrder order)
{
    // _buffer = nullptr;
    //_mark = 0;
    _position = 0;
    //_capacity = size;
    _limit = size;
    _order = order;
    _bytes = bytes;
    //_free = false;
}


BitReader::~BitReader()
{
}


void BitReader::seek(size_t val)
{
    if (val > _limit)
        throw std::out_of_range("index out of range");

    _position = val;
}


void BitReader::skip(size_t val)
{
    if (val > _limit)
        throw std::out_of_range("index out of range");

    _position += val;
}


std::string BitReader::toString()
{
    return std::string(current(), available());
}


size_t BitReader::available() const
{
    return _limit - _position;
}


size_t BitReader::limit() const
{
    return _limit;
}


//
// Get methods
//

void BitReader::getU8(uint8_t& val)
{
    get(reinterpret_cast<char*>(&val), 1);
}


void BitReader::getU16(uint16_t& val)
{
    uint16_t v;
    get(reinterpret_cast<char*>(&v), 2);
    val = (_order == ByteOrder::Network) ? networkToHost16(v) : v;
}


void BitReader::getU24(uint32_t& val)
{
    uint32_t v = 0;
    char* target = reinterpret_cast<char*>(&v);
    if (_order == ByteOrder::Network || isBigEndian())
        ++target;

    get(target, 3);
    val = (_order == ByteOrder::Network) ? networkToHost32(v) : v;
}


void BitReader::getU32(uint32_t& val)
{
    uint32_t v;
    get(reinterpret_cast<char*>(&v), 4);
    val = (_order == ByteOrder::Network) ? networkToHost32(v) : v;
}


void BitReader::getU64(uint64_t& val)
{
    uint64_t v;
    get(reinterpret_cast<char*>(&v), 8);
    val = (_order == ByteOrder::Network) ? networkToHost64(v) : v;
}


void BitReader::get(std::string& val, size_t len)
{
    if (len > available()) {
        SError << "index out of range: "
               << "len=" << len << ", "
               << "available=" << available() << ", "
               << "position=" << position() << ", "
               << "limit=" << limit();
        throw std::out_of_range("index out of range");
    }

    val.append(_bytes + _position, len);
    _position += len;
}


void BitReader::get(char* val, size_t len)
{
    if (len > available()) {
        SError << "index out of range: "
               << "len=" << len << ", "
               << "available=" << available() << ", "
               << "position=" << position() << ", "
               << "limit=" << limit();
        throw std::out_of_range("index out of range");
    }

    memcpy(val, _bytes + _position, len);
    _position += len;
}


char BitReader::peek()
{
    if (_limit > _position)
        return static_cast<char>(_bytes[_position]);
    // LDebug("Peeking next character is NULL");
    return 0;
}


uint8_t BitReader::peekU8()
{
    try {
        uint8_t v;
        getU8(v);
        _position -= 1;
        return v;
    } catch (std::out_of_range&) {
        LDebug("Peeking uint8_t: NULL");
    }
    return 0;
}


uint16_t BitReader::peekU16()
{
    try {
        uint16_t v;
        getU16(v);
        _position -= 2;
        return v;
    } catch (std::out_of_range&) {
        LDebug("Peeking uint16_t: NULL");
    }

    return 0;
}


uint32_t BitReader::peekU24()
{
    try {
        uint32_t v;
        getU24(v);
        _position -= 3;
        return v;
    } catch (std::out_of_range&) {
        LDebug("Peeking UInt24: NULL");
    }

    return 0;
}


uint32_t BitReader::peekU32()
{
    try {
        uint32_t v;
        getU32(v);
        _position -= 4;
        return v;
    } catch (std::out_of_range&) {
        LDebug("Peeking uint32_t: NULL");
    }

    return 0;
}


uint64_t BitReader::peekU64()
{
    try {
        uint64_t v;
        getU64(v);
        _position -= 8;
        return v;
    } catch (std::out_of_range&) {
        LDebug("Peeking uint64_t: NULL");
    }

    return 0;
}


//
// String parsing
//

size_t BitReader::skipToChar(char c)
{
    size_t len = 0;
    while (_limit > _position + len && _bytes[_position + len] != c)
        len++;
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::skipWhitespace()
{
    size_t len = 0;
    while (_limit > _position + len && _bytes[_position + len] == ' ')
        len++;
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::skipToNextLine()
{
    size_t len = 0;
    while (_limit > _position + len && _bytes[_position + len] != '\n') {
        len++;
    }
    len++; // advance past newline
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::skipNextWord()
{
    size_t len = skipWhitespace();
    while (_limit > _position + len && _bytes[_position + len] != ' ' &&
           _bytes[_position + len] != '\t' && _bytes[_position + len] != '\n' &&
           _bytes[_position + len] != '\r')
        len++;
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::readToNext(std::string& val, char c)
{
    size_t len = 0;
    while (_limit > _position + len && _bytes[_position + len] != c)
        len++;
    val.append(_bytes + _position, len);
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::readNextWord(std::string& val)
{
    size_t len = skipWhitespace();
    while (_limit > _position + len && _bytes[_position + len] != ' ' &&
           _bytes[_position + len] != '\t' && _bytes[_position + len] != '\n' &&
           _bytes[_position + len] != '\r')
        len++;
    val.append(_bytes + _position, len);
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::readNextNumber(unsigned int& val)
{
    size_t len = skipWhitespace();
    while (_limit > _position + len && isdigit(_bytes[_position + len]))
        len++;
    if (len)
        val = util::strtoi<uint32_t>(std::string(_bytes + _position, len));
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


size_t BitReader::readLine(std::string& val)
{
    size_t len = 0;
    while (_limit > _position + len && _bytes[_position + len] != '\n')
        len++;
    val.append(_bytes + _position, len);
    len++; // advance past newline
    if (_limit > _position + len)
        _position += len;
    else
        _position = _limit;
    return len;
}


//
// Bit Writer
//


BitWriter::BitWriter(MutableBuffer& pod, ByteOrder order)
{
    init(bufferCast<char*>(pod), pod.size(), order); // copied
}


BitWriter::BitWriter(char* bytes, size_t size, ByteOrder order)
{
    init(bytes, size, order);
}


BitWriter::BitWriter(Buffer& buf, ByteOrder order)
{
    if (buf.capacity() == 0) {
        buf.reserve(256);
    }
    init(buf.data(), buf.capacity(), order); //&buf[0]
}


void BitWriter::init(char* bytes, size_t size, ByteOrder order)
{
    _bytes = bytes;
    _limit = size;
    _order = order;
    _position = 0;
}


BitWriter::~BitWriter()
{
}


void BitWriter::skip(size_t val)
{
    if (_position + val > _limit) {
        SError << "index out of range: "
               << "val=" << val << ", "
               << "available=" << available() << ", "
               << "position=" << position() << ", "
               << "limit=" << limit();
        throw std::out_of_range("index out of range");
    }

    _position += val;
}


void BitWriter::seek(size_t val)
{
    if (val > _limit) {
        SError << "index out of range: "
               << "val=" << val << ", "
               << "available=" << available() << ", "
               << "position=" << position() << ", "
               << "limit=" << limit();
        throw std::out_of_range("index out of range");
    }

    _position = val;
}


std::string BitWriter::toString()
{
    return std::string(begin(), position());
}


size_t BitWriter::available() const
{
    return _limit - _position;
}


size_t BitWriter::limit() const
{
    return _limit;
}


//
// Write functions
//

void BitWriter::putU8(uint8_t val)
{
    put(reinterpret_cast<const char*>(&val), 1);
}


void BitWriter::putU16(uint16_t val)
{
    uint16_t v = (_order == ByteOrder::Network) ? hostToNetwork16(val) : val;
    put(reinterpret_cast<const char*>(&v), 2);
}


void BitWriter::putU24(uint32_t val)
{
    uint32_t v = (_order == ByteOrder::Network) ? hostToNetwork32(val) : val;
    char* start = reinterpret_cast<char*>(&v);
    if (_order == ByteOrder::Network || isBigEndian())
        ++start;

    put(start, 3);
}


void BitWriter::putU32(uint32_t val)
{
    uint32_t v = (_order == ByteOrder::Network) ? hostToNetwork32(val) : val;
    put(reinterpret_cast<const char*>(&v), 4);
}


void BitWriter::putU64(uint64_t val)
{
    uint64_t v = (_order == ByteOrder::Network) ? hostToNetwork64(val) : val;
    put(reinterpret_cast<const char*>(&v), 8);
}


void BitWriter::put(const std::string& val)
{
    put(val.c_str(), val.size());
}


void BitWriter::put(const char* val, size_t len)
{
    if ((_position + len) > _limit) {
        SError << "insufficient buffer capacity: "
               << "len=" << len << ", "
               << "available=" << available() << ", "
               << "position=" << position() << ", "
               << "limit=" << limit();
        throw std::out_of_range("insufficient buffer capacity");
    }

    memcpy(_bytes + _position, val, len);
    _position += len;
}


//
// Update functions
//


bool BitWriter::updateU8(uint8_t val, size_t pos)
{
    return update(reinterpret_cast<const char*>(&val), 1, pos);
}


bool BitWriter::updateU16(uint16_t val, size_t pos)
{
    uint16_t v = (_order == ByteOrder::Network) ? hostToNetwork16(val) : val;
    return update(reinterpret_cast<const char*>(&v), 2, pos);
}


bool BitWriter::updateU24(uint32_t val, size_t pos)
{
    uint32_t v = (_order == ByteOrder::Network) ? hostToNetwork32(val) : val;
    char* start = reinterpret_cast<char*>(&v);
    if (_order == ByteOrder::Network || isBigEndian())
        ++start;

    return update(start, 3, pos);
}


bool BitWriter::updateU32(uint32_t val, size_t pos)
{
    uint32_t v = (_order == ByteOrder::Network) ? hostToNetwork32(val) : val;
    return update(reinterpret_cast<const char*>(&v), 4, pos);
}


bool BitWriter::updateU64(uint64_t val, size_t pos)
{
    uint64_t v = (_order == ByteOrder::Network) ? hostToNetwork64(val) : val;
    return update(reinterpret_cast<const char*>(&v), 8, pos);
}


bool BitWriter::update(const std::string& val, size_t pos)
{
    return update(val.c_str(), val.size(), pos);
}


bool BitWriter::update(const char* val, size_t len, size_t pos)
{
    if ((pos + len) > available())
        return false;

    memcpy(_bytes + pos, val, len);
    return true;
}


//
// Dynamic Bit Writer
//


DynamicBitWriter::DynamicBitWriter(Buffer& buf, ByteOrder order)
    : BitWriter(buf, order)
    , _buffer(buf)
    , _offset(0)
{
}


DynamicBitWriter::DynamicBitWriter(Buffer& buf, Buffer::iterator offset, ByteOrder order)
    : BitWriter(buf, order)
    , _buffer(buf)
    , _offset(std::distance(buf.begin(), offset))
{
}


DynamicBitWriter::~DynamicBitWriter()
{
}


void DynamicBitWriter::put(const char* val, size_t len)
{
    //STrace << "DynamicBitWriter::put: "
    //    << "val=" << std::string(val, len) << ", "
    //    << "len=" << len << ", "
    //    << "available=" << available() << ", "
    //    << "position=" << position() << ", "
    //    << "limit=" << limit() << std::endl;

    // _buffer.resize(std::max<size_t>(3 * len / 2, 2048));
    _buffer.insert(_buffer.begin() + _offset + _position, val, val + len);
    _bytes = &_buffer[0]; // ->data();
    _limit = _buffer.size();
    _position += len;
}

bool DynamicBitWriter::update(const char* val, size_t len, size_t pos)
{
    if ((pos + len) > available())
        return false;

    _buffer.insert(_buffer.begin() + _offset + pos, val, val + len);
    return true;
}


} // namespace icy


/// @\}
