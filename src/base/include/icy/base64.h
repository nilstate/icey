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
/// Base64 encoding and decoding helpers.
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

/// Resets an `encodestate` to its initial state.
/// @param state_in State to initialize.
Base_API void init_encodestate(internal::encodestate* state_in);

/// Maps a 6-bit value to its Base64 alphabet character.
/// @param value_in Input value in the range 0–63.
/// @return Corresponding Base64 character.
Base_API char encode_value(char value_in);

/// Encodes a block of binary data into Base64 characters.
/// @param readbuf_in Input binary data.
/// @param length_in  Number of bytes to encode.
/// @param code_out   Output buffer; must be large enough to hold 4/3 * length_in bytes.
/// @param state_in   Encoder state; maintains context across calls.
/// @return Number of Base64 characters written to `code_out`.
Base_API ssize_t encode_block(const char* readbuf_in, size_t length_in, char* code_out,
                              internal::encodestate* state_in);

/// Writes any remaining Base64 padding characters and a trailing newline.
/// @param code_out Output buffer; must be at least 5 bytes.
/// @param state_in Encoder state from the preceding `encode_block` calls.
/// @return Number of characters written to `code_out`.
Base_API ssize_t encode_blockend(char* code_out, internal::encodestate* state_in);


} // namespace internal


/// Returns a safe temporary buffer size for encoding up to @p inputSize bytes.
/// Includes padding/newline slack so callers can reuse the same buffer for finalize().
inline size_t encodedBufferCapacity(size_t inputSize, int lineLength = LINE_LENGTH)
{
    const size_t quartets = (inputSize + 2) / 3;
    size_t chars = quartets * 4;
    if (lineLength > 0) {
        const size_t quartetsPerLine = std::max<size_t>(1, static_cast<size_t>(lineLength) / 4);
        chars += quartets / quartetsPerLine;
    }
    return chars + 4;
}


/// Base64 encoder.
struct Encoder : public basic::Encoder
{
    /// @param buffersize Internal read buffer size in bytes.
    Encoder(int buffersize = BUFFER_SIZE)
        : _buffersize(buffersize)
    {
        internal::init_encodestate(&_state);
    }

    /// Encodes the entire input stream and writes Base64 output to `ostrm`.
    /// Resets the encoder state after completion.
    /// @param istrm Source stream to encode.
    /// @param ostrm Destination stream for Base64 output.
    void encode(std::istream& istrm, std::ostream& ostrm)
    {
        const int N = _buffersize;
        auto readbuf = std::make_unique<char[]>(N);
        auto encbuf = std::make_unique<char[]>(
            encodedBufferCapacity(static_cast<size_t>(N), _state.linelength));
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

    /// Encodes a string to Base64 and appends the result to `out`.
    /// Resets the encoder state after completion.
    /// @param in  Input string.
    /// @param out Output string to which Base64 characters are appended.
    void encode(const std::string& in, std::string& out)
    {
        auto encbuf = std::make_unique<char[]>(
            encodedBufferCapacity(in.length(), _state.linelength));
        ssize_t enclen = encode(in.c_str(), in.length(), encbuf.get());
        out.append(encbuf.get(), enclen);

        enclen = finalize(encbuf.get());
        out.append(encbuf.get(), enclen);

        internal::init_encodestate(&_state);
    }

    /// Encodes a raw buffer, writing Base64 characters to `outbuf`.
    /// May be called multiple times before calling `finalize()`.
    /// @param inbuf  Input binary data.
    /// @param nread  Number of bytes to encode.
    /// @param outbuf Output buffer; must be at least `nread * 4 / 3 + 4` bytes.
    /// @return Number of Base64 characters written.
    ssize_t encode(const char* inbuf, size_t nread, char* outbuf) override
    {
        return internal::encode_block(inbuf, nread, outbuf, &_state);
    }

    /// Writes any pending padding and resets the encoder state.
    /// Must be called once after all `encode()` calls to flush the final block.
    /// @param outbuf Output buffer; must be at least 5 bytes.
    /// @return Number of characters written.
    ssize_t finalize(char* outbuf) override
    {
        return internal::encode_blockend(outbuf, &_state);
    }

    /// Sets the line wrap length for encoded output (0 disables line wrapping).
    /// @param lineLength Characters per line; use 0 to disable.
    void setLineLength(int lineLength) { _state.linelength = lineLength; }

    internal::encodestate _state;
    int _buffersize;
};


/// Encodes an STL byte container to a Base64 string.
/// @tparam T Container type with a `size()` method and contiguous `operator[]`.
/// @param bytes      Input data container.
/// @param lineLength Characters per line in the output (0 disables wrapping).
/// @return Base64-encoded string.
template <typename T>
inline std::string encode(const T& bytes, int lineLength = LINE_LENGTH)
{
    if (bytes.empty())
        return {};

    std::string res;
    res.reserve(encodedBufferCapacity(bytes.size(), lineLength));
    auto encbuf = std::make_unique<char[]>(
        encodedBufferCapacity(bytes.size(), lineLength));

    internal::encodestate state;
    internal::init_encodestate(&state);
    state.linelength = lineLength;

    ssize_t enclen = internal::encode_block(reinterpret_cast<const char*>(bytes.data()),
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

/// Resets a `decodestate` to its initial state.
/// @param state_in State to initialize.
Base_API void init_decodestate(internal::decodestate* state_in);

/// Maps a Base64 alphabet character to its 6-bit value.
/// @param value_in Base64 character.
/// @return Decoded 6-bit value, or a negative sentinel on invalid input.
Base_API ssize_t decode_value(char value_in);

/// Decodes a block of Base64 characters into binary data.
/// @param inbuf    Input Base64 characters.
/// @param nread    Number of input characters to decode.
/// @param outbuf   Output buffer; must be at least `nread * 3 / 4` bytes.
/// @param state_in Decoder state; maintains context across calls.
/// @return Number of binary bytes written to `outbuf`.
Base_API ssize_t decode_block(const char* inbuf, const size_t nread, char* outbuf,
                              internal::decodestate* state_in);


} // namespace internal


/// Base64 decoder.
struct Decoder : public basic::Decoder
{
    /// @param buffersize Internal read buffer size in bytes.
    Decoder(int buffersize = BUFFER_SIZE)
        : _buffersize(buffersize)
    {
        internal::init_decodestate(&_state);
    }

    /// Decodes a single Base64 character to its 6-bit value.
    /// @param value_in Base64 character.
    /// @return Decoded 6-bit value, or a negative sentinel on invalid input.
    ssize_t decode(char value_in) { return internal::decode_value(value_in); }

    /// Decodes a raw Base64 buffer into binary data.
    /// @param inbuf  Input Base64 characters.
    /// @param nread  Number of characters to decode.
    /// @param outbuf Output buffer; must be at least `nread * 3 / 4` bytes.
    /// @return Number of binary bytes written.
    ssize_t decode(const char* inbuf, size_t nread, char* outbuf) override
    {
        return internal::decode_block(inbuf, nread, outbuf, &_state);
    }

    /// Decodes the entire input stream and writes binary output to `ostrm`.
    /// Resets the decoder state after completion.
    /// @param istrm Source stream of Base64 data.
    /// @param ostrm Destination stream for decoded binary output.
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


/// Decodes a Base64-encoded STL container to a binary string.
/// @tparam T Container type with a `size()` method and contiguous `operator[]`.
/// @param bytes Input Base64 data container.
/// @return Decoded binary string.
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


/// @}
