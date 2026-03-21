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


#include "icy/error.h"
#include "icy/interface.h"
#include "icy/logger.h"
#include <cstring>
#include <stdexcept>
#include <iostream>


namespace icy {
namespace hex {


//
// Hex Encoder
//


/// Hex encoder.
struct Encoder : public basic::Encoder
{
    Encoder()
        : _linePos(0)
        , _lineLength(72)
        , _uppercase(0)
    {
    }

    /// Encodes binary input as lowercase hex characters, optionally inserting
    /// newlines every `_lineLength` output characters.
    /// @param inbuf  Input buffer to encode.
    /// @param nread  Number of bytes to read from inbuf.
    /// @param outbuf Destination buffer; must have capacity >= nread * 2 + nread/_lineLength + 1.
    /// @return Number of bytes written to outbuf.
    virtual ssize_t encode(const char* inbuf, size_t nread, char* outbuf) override
    {
        // static const int eof = std::char_traits<char>::eof();
        static const char digits[] = "0123456789abcdef0123456789ABCDEF";

        char c;
        size_t nwrite = 0;
        for (unsigned i = 0; i < nread; i++) {
            c = inbuf[i];
            std::memcpy(outbuf + nwrite++, &digits[_uppercase + ((c >> 4) & 0xF)], 1);
            std::memcpy(outbuf + nwrite++, &digits[_uppercase + (c & 0xF)], 1);
            if (_lineLength > 0 &&
                (_linePos += 2) >= _lineLength) { //++_linePos;
                _linePos = 0;
                std::memcpy(outbuf + nwrite++, "\n", 1);
            }
        }

        return nwrite;
    }

    /// No-op finalizer; hex encoding has no pending state.
    /// @return Always 0.
    virtual ssize_t finalize(char* /* outbuf */) override { return 0; }

    /// Controls whether encoded output uses uppercase hex digits (A-F) or lowercase (a-f).
    /// @param flag true for uppercase, false for lowercase.
    void setUppercase(bool flag) { _uppercase = flag ? 16 : 0; }

    /// Sets the maximum number of output characters per line before a newline is inserted.
    /// Set to 0 to disable line wrapping.
    /// @param lineLength Characters per line.
    void setLineLength(int lineLength) { _lineLength = lineLength; }

    int _linePos;
    int _lineLength;
    int _uppercase;
};


/// Converts the STL container to Hex.
template <typename T>
inline std::string encode(const T& bytes)
{
    static const char digits[] = "0123456789abcdef";
    std::string res;
    res.reserve(bytes.size() * 2);
    for (const auto& byte : bytes) {
        const unsigned char c = static_cast<const unsigned char>(byte);
        res += digits[(c >> 4) & 0xF];
        res += digits[c & 0xF];
    }
    return res;
}


//
// Hex Decoder
//


/// Hex decoder.
struct Decoder : public basic::Decoder
{
    Decoder()
        : lastbyte('\0')
    {
    }
    virtual ~Decoder() {}

    /// Decodes hex-encoded input to binary. Whitespace in the input is ignored.
    /// A trailing unpaired nibble is buffered and prepended on the next call.
    /// @param inbuf  Hex-encoded input buffer.
    /// @param nread  Number of bytes to read from inbuf.
    /// @param outbuf Destination buffer; must have capacity >= nread / 2.
    /// @return Number of decoded bytes written to outbuf.
    virtual ssize_t decode(const char* inbuf, size_t nread, char* outbuf) override
    {
        int n;
        char c;
        size_t rpos = 0;
        size_t nwrite = 0;
        while (rpos < nread) {
            if (readnext(inbuf, nread, rpos, c))
                n = (nybble(c) << 4);

            else if (rpos >= nread) {
                // Store the last byte to be
                // prepended on next decode()
                if (!iswspace(inbuf[rpos - 1]))
                    std::memcpy(&lastbyte, &inbuf[rpos - 1], 1);
                break;
            }

            readnext(inbuf, nread, rpos, c);
            n = n | nybble(c);
            std::memcpy(outbuf + nwrite++, &n, 1);
        }
        return nwrite;
    }

    /// No-op finalizer; hex decoding has no pending output state.
    /// @return Always 0.
    virtual ssize_t finalize(char* /* outbuf */) override { return 0; }

    /// Reads the next non-whitespace character from inbuf, prepending any
    /// buffered lastbyte before consuming from the stream.
    /// @param inbuf  Input buffer.
    /// @param nread  Total bytes in inbuf.
    /// @param rpos   Current read position; advanced on each consumed byte.
    /// @param c      Output: the next non-whitespace character.
    /// @return true if more input remains after c was read, false otherwise.
    bool readnext(const char* inbuf, size_t nread, size_t& rpos,
                  char& c)
    {
        if (rpos == 0 && lastbyte != '\0') {
            if (iswspace(lastbyte))
                throw std::logic_error("Hex decoder: stored last byte must not be whitespace");
            c = lastbyte;
            lastbyte = '\0';
        } else {
            c = inbuf[rpos++];
            while (iswspace(c) && rpos < nread)
                c = inbuf[rpos++];
        }
        return rpos < nread;
    }

    /// Converts an ASCII hex character to its 4-bit integer value.
    /// @param n ASCII character ('0'-'9', 'a'-'f', 'A'-'F').
    /// @return Integer value in the range [0, 15].
    /// @throws std::runtime_error if n is not a valid hex character.
    int nybble(const int n)
    {
        if (n >= '0' && n <= '9')
            return n - '0';
        else if (n >= 'A' && n <= 'F')
            return n - ('A' - 10);
        else if (n >= 'a' && n <= 'f')
            return n - ('a' - 10);
        else
            throw std::runtime_error("Invalid hex format");
    }

    /// Returns true if c is an ASCII whitespace character (space, CR, tab, LF).
    /// @param c Character to test.
    /// @return true if c is whitespace.
    bool iswspace(const char c)
    {
        return c == ' ' || c == '\r' || c == '\t' || c == '\n';
    }

    char lastbyte;
};


} // namespace hex
} // namespace icy


/// @\}
