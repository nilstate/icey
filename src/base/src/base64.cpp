///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{
// This file uses the public domain libb64 library:
// http://libb64.sourceforge.net/
//


#include "scy/base64.h"


namespace scy {
namespace base64 {
namespace internal {


//
// Encoder
//


void init_encodestate(encodestate* state_in)
{
    state_in->step = step_A;
    state_in->result = 0;
    state_in->stepcount = 0;
    state_in->linelength = LINE_LENGTH; // added: set 0 for no line feeds
    state_in->nullptrlterminate = 0; // added: set 1 for nullptrl terminated output string
}


char encode_value(char value_in)
{
    static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (value_in > 63)
        return '=';
    return encoding[(int)value_in];
}


ssize_t encode_block(const char* plaintext_in, size_t length_in, char* code_out, encodestate* state_in)
{
    const char* plainchar = plaintext_in;
    const char* const plaintextend = plaintext_in + length_in;
    char* codechar = code_out;
    char result;
    char fragment;

    result = state_in->result;

    switch (state_in->step) {
        while (1) {
            case step_A:
                if (plainchar == plaintextend) {
                    state_in->result = result;
                    state_in->step = step_A;
                    return codechar - code_out;
                }
                fragment = *plainchar++;
                result = (fragment & 0x0fc) >> 2;
                *codechar++ = encode_value(result);
                result = (fragment & 0x003) << 4;
            case step_B:
                if (plainchar == plaintextend) {
                    state_in->result = result;
                    state_in->step = step_B;
                    return codechar - code_out;
                }
                fragment = *plainchar++;
                result |= (fragment & 0x0f0) >> 4;
                *codechar++ = encode_value(result);
                result = (fragment & 0x00f) << 2;
            case step_C:
                if (plainchar == plaintextend) {
                    state_in->result = result;
                    state_in->step = step_C;
                    return codechar - code_out;
                }
                fragment = *plainchar++;
                result |= (fragment & 0x0c0) >> 6;
                *codechar++ = encode_value(result);
                result = (fragment & 0x03f) >> 0;
                *codechar++ = encode_value(result);

                if (state_in->linelength) { // added
                    ++(state_in->stepcount);
                    if (state_in->stepcount == state_in->linelength / 4) {
                        *codechar++ = '\n';
                        state_in->stepcount = 0;
                    }
                }
        }
    }

    // control should not reach here
    return codechar - code_out;
}


ssize_t encode_blockend(char* code_out, encodestate* state_in)
{
    char* codechar = code_out;

    switch (state_in->step) {
        case step_B:
            *codechar++ = encode_value(state_in->result);
            *codechar++ = '=';
            *codechar++ = '=';
            break;
        case step_C:
            *codechar++ = encode_value(state_in->result);
            *codechar++ = '=';
            break;
        case step_A:
            break;
    }
    if (state_in->nullptrlterminate)
        *codechar++ = '\n';

    return codechar - code_out;
}


//
// Decoder
//


ssize_t decode_value(char value_in)
{
    static const signed char decoding[] = {
        62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1,
        -1, -1, -2, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
    static const char decoding_size = sizeof(decoding);
    value_in -= 43;
    if (value_in < 0 || value_in >= decoding_size)
        return -1;
    return decoding[(int)value_in];
}


void init_decodestate(decodestate* state_in)
{
    state_in->step = step_a;
    state_in->plainchar = 0;
}


ssize_t decode_block(const char* code_in, const size_t length_in, char* plaintext_out, decodestate* state_in)
{
    const char* codechar = code_in;
    char* plainchar = plaintext_out;
    char fragment;

    *plainchar = state_in->plainchar;

    switch (state_in->step) {
        while (1) {
            case step_a:
                do {
                    if (codechar == code_in + length_in) {
                        state_in->step = step_a;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = (char)decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar = (fragment & 0x03f) << 2;
            case step_b:
                do {
                    if (codechar == code_in + length_in) {
                        state_in->step = step_b;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = (char)decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar++ |= (fragment & 0x030) >> 4;
                *plainchar = (fragment & 0x00f) << 4;
            case step_c:
                do {
                    if (codechar == code_in + length_in) {
                        state_in->step = step_c;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = (char)decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar++ |= (fragment & 0x03c) >> 2;
                *plainchar = (fragment & 0x003) << 6;
            case step_d:
                do {
                    if (codechar == code_in + length_in) {
                        state_in->step = step_d;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = (char)decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar++ |= (fragment & 0x03f);
        }
    }

    // control should not reach here
    return plainchar - plaintext_out;
}


} // namespace internal
} // namespace base64
} // namespace scy


/// @\}
