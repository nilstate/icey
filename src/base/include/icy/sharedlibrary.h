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


/// Loads a shared library at runtime and resolves exported symbols
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

    /// Reads the last libuv dynamic-linker error, stores it in _error, and throws
    /// a std::runtime_error with the combined prefix and error message.
    /// @param prefix Human-readable context string prepended to the error detail.
    /// @throws std::runtime_error always.
    void setError(const std::string& prefix)
    {
        std::string err(uv_dlerror(&_lib));
        if (err.empty())
            err = "unknown error";
        _error = prefix + ": " + err;
        throw std::runtime_error(prefix + ": " + err);
    }

    /// Returns the last error message recorded by setError().
    /// Empty if no error has occurred.
    /// @return Last error string.
    std::string error() const { return _error; }

protected:
    uv_lib_t _lib;
    std::string _error;
};


} // namespace icy


/// @\}
