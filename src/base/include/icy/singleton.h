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


#include <memory>
#include <mutex>


namespace icy {


/// Helper template class for managing singleton objects allocated on the heap.
template <class S>
class Singleton
{
public:
    Singleton() = default;

    ~Singleton() = default;

    /// Returns a pointer to the managed singleton, instantiating it on first call.
    /// Thread-safe; protected by an internal mutex.
    /// @return Pointer to the singleton instance (never null).
    S* get()
    {
        std::lock_guard<std::mutex> guard(_m);
        if (!_ptr)
            _ptr.reset(new S());
        return _ptr.get();
    }

    /// Replaces the managed singleton with newPtr and returns the previous instance.
    /// The caller takes ownership of the returned pointer.
    /// Thread-safe; protected by an internal mutex.
    /// @param newPtr Replacement instance (may be nullptr).
    /// @return Previously managed pointer (caller must delete if non-null).
    S* swap(S* newPtr)
    {
        std::lock_guard<std::mutex> guard(_m);
        S* oldPtr = _ptr.release();
        _ptr.reset(newPtr);
        return oldPtr;
    }

    /// Destroys the managed singleton instance and resets the internal pointer to null.
    /// Thread-safe; protected by an internal mutex.
    void destroy()
    {
        std::lock_guard<std::mutex> guard(_m);
        _ptr.reset();
    }

private:
    /// Custom deleter that can access private destructors via friendship.
    struct Deleter
    {
        void operator()(S* p) const { delete p; }
    };

    std::unique_ptr<S, Deleter> _ptr;
    std::mutex _m;
};


} // namespace icy
