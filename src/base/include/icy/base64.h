///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// This file uses parts of the public domain libb64 library:
// http://libb64.sourceforge.net/
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/interface.h"
#include "icy/logger.h"

#include <cstdint>
#include <iostream>
#include <memory>


namespace icy {
namespace base64 {


constexpr int BUFFER_SIZE = 16384;
constexpr int LINE_LENGTH = 72;


//
// Base64 Encoder
//


namespace internal {


enum encodestep
{
    step_A,
    step_B,
    step_C
};

struct encodestate
{
    encodestep step;
    char result;
    int stepcount;
    int linelength;        // added
    int nullptrlterminate; // added
};

Base_API void init_encodestate(internal::encodestate* state_in);

Base_API char encode_value(char value_in);

Base_API ssize_t encode_block(const char* readbuf_in, size_t length_in, char* code_out,
                              internal::encodestate* state_in);

Base_API ssize_t encode_blockend(char* code_out, internal::encodestate* state_in);


} // namespace internal


/// Base64 encoder.
struct Encoder : public basic::Encoder
{
    Encoder(int buffersize = BUFFER_SIZE)
        : _buffersize(buffersize)
    {
        internal::init_encodestate(&_state);
    }

    void encode(std::istream& istrm, std::ostream& ostrm)
    {
        const int N = _buffersize;
        auto readbuf = std::make_unique<char[]>(N);
        auto encbuf = std::make_unique<char[]>(2 * N);
        ssize_t nread;
        ssize_t enclen;

        do {
            istrm.read(readbuf.get(), N);
            nread = static_cast<int>(istrm.gcount());
            enclen = encode(readbuf.get(), nread, encbuf.get());
            ostrm.write(encbuf.get(), enclen);
        } while (istrm.good() && nread > 0);

        enclen = finalize(encbuf.get());
        ostrm.write(encbuf.get(), enclen);

        internal::init_encodestate(&_state);
    }

    void encode(const std::string& in, std::string& out)
    {
        auto encbuf = std::make_unique<char[]>(in.length() * 2);
        ssize_t enclen = encode(in.c_str(), in.length(), encbuf.get());
        out.append(encbuf.get(), enclen);

        enclen = finalize(encbuf.get());
        out.append(encbuf.get(), enclen);

        internal::init_encodestate(&_state);
    }

    ssize_t encode(const char* inbuf, size_t nread, char* outbuf) override
    {
        return internal::encode_block(inbuf, nread, outbuf, &_state);
    }

    ssize_t finalize(char* outbuf) override
    {
        return internal::encode_blockend(outbuf, &_state);
    }

    void setLineLength(int lineLength) { _state.linelength = lineLength; }

    internal::encodestate _state;
    int _buffersize;
};


/// Convert an STL container to Base64.
template <typename T>
inline std::string encode(const T& bytes, int lineLength = LINE_LENGTH)
{
    std::string res;
    res.reserve(bytes.size() * 2);
    auto encbuf = std::make_unique<char[]>(bytes.size() * 2);

    internal::encodestate state;
    internal::init_encodestate(&state);
    state.linelength = lineLength;

    ssize_t enclen = internal::encode_block(reinterpret_cast<const char*>(&bytes[0]),
                                            bytes.size(), encbuf.get(), &state);
    res.append(encbuf.get(), enclen);

    enclen = internal::encode_blockend(encbuf.get(), &state);
    res.append(encbuf.get(), enclen);
    return res;
}


//
// Base64 Decoder
//


namespace internal {


enum decodestep
{
    step_a,
    step_b,
    step_c,
    step_d
};

struct decodestate
{
    decodestep step;
    char plainchar;
};

Base_API void init_decodestate(internal::decodestate* state_in);

Base_API ssize_t decode_value(char value_in);

Base_API ssize_t decode_block(const char* inbuf, const size_t nread, char* outbuf,
                              internal::decodestate* state_in);


} // namespace internal


/// Base64 decoder.
struct Decoder : public basic::Decoder
{
    Decoder(int buffersize = BUFFER_SIZE)
        : _buffersize(buffersize)
    {
        internal::init_decodestate(&_state);
    }

    ssize_t decode(char value_in) { return internal::decode_value(value_in); }

    ssize_t decode(const char* inbuf, size_t nread, char* outbuf) override
    {
        return internal::decode_block(inbuf, nread, outbuf, &_state);
    }

    void decode(std::istream& istrm, std::ostream& ostrm)
    {
        const int N = _buffersize;
        auto decbuf = std::make_unique<char[]>(N);
        auto readbuf = std::make_unique<char[]>(N);
        size_t declen;
        size_t nread;

        do {
            istrm.read(decbuf.get(), N);
            declen = static_cast<int>(istrm.gcount());
            nread = decode(decbuf.get(), declen, readbuf.get());
            ostrm.write(readbuf.get(), nread);
        } while (istrm.good() && declen > 0);

        internal::init_decodestate(&_state);
    }

    internal::decodestate _state;
    int _buffersize;
};


/// Decode an STL container from Base64.
template <typename T>
inline std::string decode(const T& bytes)
{
    std::string res;
    res.reserve(bytes.size() * 2);
    auto encbuf = std::make_unique<char[]>(bytes.size() * 2);

    internal::decodestate state;
    internal::init_decodestate(&state);

    size_t enclen = internal::decode_block(reinterpret_cast<const char*>(&bytes[0]),
                                           bytes.size(), encbuf.get(), &state);
    res.append(encbuf.get(), enclen);

    return res;
}


} // namespace base64
} // namespace icy


/// @\}
