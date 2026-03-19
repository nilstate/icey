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

    /// Returns a pointer to the singleton object held by the Singleton.
    /// The first call to get will instantiate the singleton.
    S* get()
    {
        std::lock_guard<std::mutex> guard(_m);
        if (!_ptr)
            _ptr.reset(new S());
        return _ptr.get();
    }

    /// Swaps the old pointer with the new one and returns the old instance.
    S* swap(S* newPtr)
    {
        std::lock_guard<std::mutex> guard(_m);
        S* oldPtr = _ptr.release();
        _ptr.reset(newPtr);
        return oldPtr;
    }

    /// Destroys the managed singleton instance.
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
