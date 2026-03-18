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


/// Base class for intrusive reference counting.
///
/// Embeds the refcount in the object itself - no separate control block
/// allocation, no atomic operations. Safe for single-threaded libuv loops.
///
/// Usage: inherit from RefCounted<YourClass>, then use IntrusivePtr<YourClass>
/// instead of std::shared_ptr<YourClass>.
template <typename T>
class RefCounted
{
public:
    RefCounted() = default;
    RefCounted(const RefCounted&) noexcept : _refCount(0) {}
    RefCounted& operator=(const RefCounted&) noexcept { return *this; }

    void addRef() const noexcept { ++_refCount; }
    bool releaseRef() const noexcept { return --_refCount == 0; }
    [[nodiscard]] int refCount() const noexcept { return _refCount; }

protected:
    ~RefCounted() = default;

private:
    mutable int _refCount = 0;
};


/// Intrusive smart pointer for RefCounted objects.
///
/// Like std::shared_ptr but with zero allocation overhead:
/// - No separate control block (refcount is embedded in the object)
/// - Non-atomic refcount (safe for single-threaded libuv loops)
/// - Same API as shared_ptr for easy migration
template <typename T>
class IntrusivePtr
{
public:
    IntrusivePtr() noexcept : _ptr(nullptr) {}
    IntrusivePtr(std::nullptr_t) noexcept : _ptr(nullptr) {}

    explicit IntrusivePtr(T* p) noexcept : _ptr(p)
    {
        if (_ptr) _ptr->addRef();
    }

    IntrusivePtr(const IntrusivePtr& r) noexcept : _ptr(r._ptr)
    {
        if (_ptr) _ptr->addRef();
    }

    template <typename U>
    IntrusivePtr(const IntrusivePtr<U>& r) noexcept : _ptr(r.get())
    {
        if (_ptr) _ptr->addRef();
    }

    IntrusivePtr(IntrusivePtr&& r) noexcept : _ptr(r._ptr)
    {
        r._ptr = nullptr;
    }

    template <typename U>
    IntrusivePtr(IntrusivePtr<U>&& r) noexcept : _ptr(r.get())
    {
        r.detach();
    }

    ~IntrusivePtr()
    {
        if (_ptr && _ptr->releaseRef())
            delete _ptr;
    }

    IntrusivePtr& operator=(const IntrusivePtr& r) noexcept
    {
        IntrusivePtr(r).swap(*this);
        return *this;
    }

    IntrusivePtr& operator=(IntrusivePtr&& r) noexcept
    {
        IntrusivePtr(std::move(r)).swap(*this);
        return *this;
    }

    IntrusivePtr& operator=(T* p) noexcept
    {
        IntrusivePtr(p).swap(*this);
        return *this;
    }

    IntrusivePtr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    void reset() noexcept
    {
        IntrusivePtr().swap(*this);
    }

    void reset(T* p) noexcept
    {
        IntrusivePtr(p).swap(*this);
    }

    T* get() const noexcept { return _ptr; }
    T& operator*() const noexcept { return *_ptr; }
    T* operator->() const noexcept { return _ptr; }
    explicit operator bool() const noexcept { return _ptr != nullptr; }

    void swap(IntrusivePtr& r) noexcept { std::swap(_ptr, r._ptr); }

    /// Release ownership without decrementing refcount.
    /// Used internally for move construction across types.
    void detach() noexcept { _ptr = nullptr; }

    bool operator==(const IntrusivePtr& r) const noexcept { return _ptr == r._ptr; }
    bool operator!=(const IntrusivePtr& r) const noexcept { return _ptr != r._ptr; }
    bool operator==(std::nullptr_t) const noexcept { return !_ptr; }
    bool operator!=(std::nullptr_t) const noexcept { return _ptr; }
    bool operator<(const IntrusivePtr& r) const noexcept { return _ptr < r._ptr; }

private:
    T* _ptr;
};


/// Create an IntrusivePtr with a new object. Equivalent to make_shared.
template <typename T, typename... Args>
IntrusivePtr<T> makeIntrusive(Args&&... args)
{
    return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
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
