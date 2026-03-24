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


#include "icy/base.h"
#include "icy/loop.h"

#include <memory>


namespace icy {


/// Schedules deferred deletion of ptr on the next event loop iteration.
/// This is essential for deleting objects that may still be referenced
/// by pending libuv callbacks (e.g. socket adapters with in-flight I/O).
/// Uses a self-cleaning uv_idle_t handle that fires once and then closes itself.
/// @tparam T Type of the object to delete.
/// @param ptr  Object to delete. Does nothing if nullptr.
/// @param loop Event loop on which to schedule the deletion.
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

    /// Increments the reference count. Called by IntrusivePtr on acquisition.
    void addRef() const noexcept { ++_refCount; }

    /// Decrements the reference count.
    /// @return true if the count reached zero (caller should delete the object).
    bool releaseRef() const noexcept { return --_refCount == 0; }

    /// Returns the current reference count.
    /// @return Current reference count.
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

    /// Releases ownership of the current pointer, decrementing its refcount.
    /// The pointer is set to null.
    void reset() noexcept
    {
        IntrusivePtr().swap(*this);
    }

    /// Releases the current pointer and takes ownership of p, incrementing its refcount.
    /// @param p New raw pointer to manage (may be nullptr).
    void reset(T* p) noexcept
    {
        IntrusivePtr(p).swap(*this);
    }

    /// Returns the raw pointer without transferring ownership.
    /// @return Raw pointer to the managed object, or nullptr.
    T* get() const noexcept { return _ptr; }

    /// Dereferences the managed pointer.
    /// @return Reference to the managed object.
    T& operator*() const noexcept { return *_ptr; }

    /// Member access on the managed pointer.
    /// @return Raw pointer to the managed object.
    T* operator->() const noexcept { return _ptr; }

    /// Returns true if the pointer is non-null.
    explicit operator bool() const noexcept { return _ptr != nullptr; }

    /// Swaps the managed pointer with another IntrusivePtr.
    /// @param r The other IntrusivePtr to swap with.
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


/// Creates an IntrusivePtr managing a newly heap-allocated T. Equivalent to std::make_shared.
/// @tparam T    Type to construct; must inherit from RefCounted<T>.
/// @param args  Arguments forwarded to T's constructor.
/// @return IntrusivePtr<T> owning the new object.
template <typename T, typename... Args>
IntrusivePtr<T> makeIntrusive(Args&&... args)
{
    return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
}


namespace deleter {


/// Deleter functor that calls dispose() on the pointer.
/// Useful with std::unique_ptr for objects that require a dispose() call
/// rather than direct deletion.
template <class T>
struct Dispose
{
    /// Calls ptr->dispose() if ptr is non-null.
    /// @param ptr Pointer to dispose; may be nullptr.
    void operator()(T* ptr)
    {
        if (!ptr) return;
        static_assert(0 < sizeof(T), "can't delete an incomplete type");
        ptr->dispose();
    }
};


/// Deleter functor for array pointers. Calls delete[] on the pointer.
template <class T>
struct Array
{
    /// Calls delete[] on ptr if non-null.
    /// @param ptr Array pointer to delete; may be nullptr.
    void operator()(T* ptr)
    {
        if (!ptr) return;
        static_assert(0 < sizeof(T), "can't delete an incomplete type");
        delete[] ptr;
    }
};


} // namespace deleter
} // namespace icy

/// @}
