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
#include "scy/loop.h"

#include <memory>


namespace scy {


/// Schedule deferred deletion on the next event loop iteration.
/// This is essential for deleting objects that may still be referenced
/// by pending libuv callbacks (e.g. socket adapters with in-flight I/O).
template <typename T>
inline void deleteLater(T* ptr, uv::Loop* loop)
{
    if (!ptr)
        return;
    auto* idle = new uv_idle_t;
    uv_idle_init(loop, idle);
    idle->data = ptr;
    uv_idle_start(idle, [](uv_idle_t* handle) {
        uv_idle_stop(handle);
        delete static_cast<T*>(handle->data);
        uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* h) {
            delete reinterpret_cast<uv_idle_t*>(h);
        });
    });
}


namespace deleter {


/// Deleter functor that calls dispose() on the pointer.
template <class T>
struct Dispose
{
    void operator()(T* ptr)
    {
        if (!ptr) return;
        static_assert(0 < sizeof(T), "can't delete an incomplete type");
        ptr->dispose();
    }
};


/// Deleter functor for array pointers.
template <class T>
struct Array
{
    void operator()(T* ptr)
    {
        if (!ptr) return;
        static_assert(0 < sizeof(T), "can't delete an incomplete type");
        delete[] ptr;
    }
};


} // namespace deleter
} // namespace scy
