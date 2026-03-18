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

#include "uv.h"

#include <exception>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>


namespace scy {


/// Basic error type.
///
/// Errors contain an error code, message, and exception pointer.
struct Error
{
    int err;
    std::string message;
    std::exception_ptr exception;

    Error() { reset(); }

    Error(const std::string& msg)
    {
        reset();
        message = msg;
    }

    Error(const char* msg)
    {
        reset();
        message = msg;
    }

    bool any() const
    {
        return err != 0 || !message.empty() || exception != nullptr;
    }

    void reset()
    {
        err = 0;
        message.clear();
        exception = nullptr;
    }

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


inline void throwError(std::string_view message, int err = UV_UNKNOWN)
{
    throw std::runtime_error(formatError(message, err));
}


} // namespace uv
} // namespace scy


/// @\}
