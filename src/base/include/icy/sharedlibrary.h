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


#include "icy/util.h"

#include "uv.h"


namespace icy {


struct SharedLibrary
{
    /// Opens a shared library.
    /// The filename is in utf-8. Returns true on success and false on error.
    /// Call `SharedLibrary::error()` to get the error message.
    bool open(const std::string& path)
    {
        if (uv_dlopen(path.c_str(), &_lib)) {
            setError("Cannot load library");
            return false;
        }
        return true;
    }

    /// Closes the shared library.
    void close()
    {
        uv_dlclose(&_lib);
    }

    /// Retrieves a data pointer from a dynamic library.
    /// It is legal for a symbol to map to nullptr.
    /// Returns 0 on success and -1 if the symbol was not found.
    bool sym(const char* name, void** ptr)
    {
        if (uv_dlsym(&_lib, name, ptr)) {
            setError(util::format("Symbol '%s' not found.", name));
            return false;
        }
        return true;
    }

    void setError(const std::string& prefix)
    {
        std::string err(uv_dlerror(&_lib));
        if (err.empty())
            err = "unknown error";
        _error = prefix + ": " + err;
        throw std::runtime_error(prefix + ": " + err);
    }

    std::string error() const { return _error; }

protected:
    uv_lib_t _lib;
    std::string _error;
};


} // namespace icy


/// @\}
