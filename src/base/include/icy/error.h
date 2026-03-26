///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"

#include "uv.h"

#include <exception>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>


namespace icy {


/// Basic error type.
///
/// Errors contain an error code, message, and exception pointer.
struct Error
{
    int err;
    std::string message;
    std::exception_ptr exception;

    /// Default constructor; initializes all fields to a no-error state.
    Error() { reset(); }

    /// Constructs an error with the given message string.
    /// @param msg Human-readable error description.
    Error(const std::string& msg)
    {
        reset();
        message = msg;
    }

    /// Constructs an error with the given C string message.
    /// @param msg Human-readable error description.
    Error(const char* msg)
    {
        reset();
        message = msg;
    }

    /// Returns true if any error condition is set (non-zero code, non-empty message, or exception).
    /// @return True if an error is present.
    bool any() const
    {
        return err != 0 || !message.empty() || exception != nullptr;
    }

    /// Clears all error fields, resetting to a no-error state.
    void reset()
    {
        err = 0;
        message.clear();
        exception = nullptr;
    }

    /// Re-throws the stored exception pointer if one is set.
    /// Has no effect if `exception` is null.
    void rethrow()
    {
        if (exception)
            std::rethrow_exception(exception);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Error& err)
    {
        stream << err.message;
        return stream;
    }
};


//
// UV Error Helpers
//


namespace uv {


/// Formats a human-readable error string from a message and a libuv error code.
/// If `err` is not `UV_UNKNOWN`, the libuv error description is appended after a colon.
/// @param message Descriptive context for the error.
/// @param err libuv error code (e.g. from a failed `uv_*` call). Defaults to `UV_UNKNOWN`.
/// @return Formatted error string.
inline std::string formatError(std::string_view message, int err = UV_UNKNOWN)
{
    std::string result(message);
    if (err != UV_UNKNOWN) {
        if (!result.empty())
            result.append(": ");
        result.append(uv_strerror(err));
    }
    return result;
}


/// Throws a `std::runtime_error` with a formatted error message.
/// @param message Descriptive context for the error.
/// @param err libuv error code to append. Defaults to `UV_UNKNOWN`.
inline void throwError(std::string_view message, int err = UV_UNKNOWN)
{
    throw std::runtime_error(formatError(message, err));
}


} // namespace uv
} // namespace icy


/// @}
