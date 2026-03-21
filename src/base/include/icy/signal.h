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


#include "icy/delegate.h"
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <vector>


namespace icy {


/// No-op mutex for single-threaded signal usage.
/// When all signal operations occur on a single libuv event loop thread,
/// the shared_mutex is unnecessary overhead. Use LocalSignal alias instead.
struct NullSharedMutex
{
    void lock() {}
    void unlock() {}
    void lock_shared() {}
    void unlock_shared() {}
};


/// Internal classes
namespace internal {

/// Signal slot storage class.
template <typename RT, typename... Args>
struct Slot;

} // namespace internal


/// Signal and slots implementation.
///
/// To create a signal, declare member variables of type
/// `Signal<...>` in your class. The template parameter is the
/// argument types that will be passed to the callback functions.
///
/// Here's a simple example with a class `MyClass` that has a single
/// signal `my_signal` which takes a single `int` argument:
///
///     class MyClass
///     {
///     public:
///         Signal<int> my_signal;
///         ...
///     };
///
/// To connect to a signal, call its `attach()` member function,
/// passing a function (in the `std::function` sense) as the argument.
/// The function can be a function pointer, a functor object, or
/// an anonymous lambda function.
///
/// Here's an example connecting to the above signal to a lambda function:
///
///     MyClass my_class;
///     my_class.my_signal.attach([](int x) {
///         cout << "value: " << x << endl;
///     });
///
/// The `attach()` function returns a unique ID of type `int`
/// which can be used later to disconnect the callback function.
///
/// Here's an example using the above code to then immediately disconnect
/// the signal connection with the `detach()` member function:
///
///     MyClass my_class;
///     auto id = my_class.my_signal.attach([](int x) {
///         cout << "value: " << x << endl;
///     });
///     my_class.my_signal.detach(id);
///
/// The `detach()` function is passed the callback ID and will
/// return `true` if a callback was disconnected or `false` if the
/// ID wasn't found. Note that `detach()` can be safely called from
/// within the callback scope.
///
/// In the case of class members there is a `slot()` helper that can be
/// used to bind the signal like so:
///
///     class TargetClass
///     {
///     public:
///         Signal<int> my_signal;
///
///         int print(int x)
///         {
///             cout << "say: " << x << endl;
///         }
///     };
///
///     MyClass my_class;
///
///     TargetClass target_class;
///     my_class.my_signal += slot(&target_class, &TargetClass::print)
///
/// The `slot()` helper can also be used to disconnect class member callbacks
/// like so:
///
///     my_class.my_signal -= slot(&target_class, &TargetClass::print)
///
/// To emit the signal, call its `emit()` member function passing
/// arguments matching the types of those in the signal variable
/// declaration.
///
/// Using the above example code, here's an example showing how to
/// emit `my_signal`:
///
///     my_class.my_signal.emit(42);
///
/// Since the signal was declared to take a single `int` argument, this
/// will cause any callbacks connected to the signal to be called,
/// passing the integer `42` as the only argument.
///
template <typename RT, typename MutexT = std::shared_mutex>
class Signal;
/// Thread-safe signal and slot implementation for callback-based event dispatch
template <typename RT, typename... Args, typename MutexT>
class Signal<RT(Args...), MutexT>
{
public:
    using Function = std::function<RT(Args...)>;
    using SlotPtr = std::shared_ptr<internal::Slot<RT, Args...>>;

    /// Connects a `lambda` or `std::function` to the signal.
    ///
    /// @param func      The callable to invoke when the signal is emitted.
    /// @param instance  Optional owner pointer used for instance-based detach; pass `nullptr` if not applicable.
    /// @param id        Explicit slot ID to assign; pass `-1` to auto-assign.
    /// @param priority  Higher values are called first; pass `-1` for default ordering.
    /// @return The assigned slot ID, which can be passed to `detach()` to disconnect.
    [[nodiscard]] int attach(Function const& func, void* instance = nullptr, int id = -1, int priority = -1) const
    {
        return attach(std::make_shared<internal::Slot<RT, Args...>>(
            new FunctionDelegate<RT, Args...>(func), instance, id, priority));
    }

    /// Connects a pre-constructed `SlotPtr` to the signal.
    ///
    /// Duplicate slots (matched by delegate equality) are removed before insertion.
    /// Slots are kept sorted in descending priority order after insertion.
    ///
    /// @param slot  The slot to attach. Must have a valid delegate.
    /// @return The assigned slot ID, which can be passed to `detach()` to disconnect.
    /// @throws std::logic_error if `slot->id` is set explicitly and already in use.
    [[nodiscard]] int attach(SlotPtr slot) const
    {
        detach(slot); // clear duplicates
        std::unique_lock<MutexT> guard(_mutex);
        if (slot->id == -1) {
            slot->id = ++_lastId;
        } else {
            // Ensure explicit IDs don't collide with existing slots
            for (auto const& s : _slots) {
                if (s->alive() && s->id == slot->id)
                    throw std::logic_error("Signal slot ID already in use");
            }
            if (slot->id > _lastId)
                _lastId = slot->id;
        }
        _slots.push_back(slot);
        //_slots.sort(Slot::ComparePrioroty);
        std::sort(_slots.begin(), _slots.end(),
                  [](SlotPtr const& l, SlotPtr const& r) { return l->priority > r->priority; });
        return slot->id;
    }

    /// Detaches the slot with the given ID.
    ///
    /// Safe to call from within a slot's callback (the slot is marked dead before erasure).
    ///
    /// @param id  The slot ID returned by `attach()`.
    /// @return `true` if a slot was found and removed; `false` if the ID was not found.
    bool detach(int id) const
    {
        std::unique_lock<MutexT> guard(_mutex);
        for (auto it = _slots.begin(); it != _slots.end();) {
            auto& slot = *it;
            if (slot->alive() && slot->id == id) {
                slot->kill();
                _slots.erase(it);
                return true;
            } else
                ++it;
        }
        return false;
    }

    /// Detaches all slots associated with the given instance pointer.
    ///
    /// Useful for bulk disconnect when an object is destroyed. Matches slots
    /// by their stored `instance` pointer, not by delegate equality.
    ///
    /// @param instance  The owner pointer used when the slots were attached.
    /// @return `true` if at least one slot was removed; `false` otherwise.
    bool detach(const void* instance) const
    {
        std::unique_lock<MutexT> guard(_mutex);
        bool removed = false;
        for (auto it = _slots.begin(); it != _slots.end();) {
            auto& slot = *it;
            if (slot->alive() && slot->instance == instance) {
                slot->kill();
                it = _slots.erase(it);
                removed = true;
            } else
                ++it;
        }
        return removed;
    }

    /// Detaches the slot whose delegate compares equal to `other->delegate`.
    ///
    /// Used by the `slot()` helper overloads and `operator-=` to disconnect
    /// a specific class-member or function binding by value.
    ///
    /// @param other  A slot whose delegate is compared against attached slots.
    /// @return `true` if a matching slot was found and removed; `false` otherwise.
    bool detach(SlotPtr other) const
    {
        std::unique_lock<MutexT> guard(_mutex);
        for (auto it = _slots.begin(); it != _slots.end();) {
            auto& slot = *it;
            if (slot->alive() && (*slot->delegate == *other->delegate)) {
                slot->kill();
                _slots.erase(it);
                return true;
            } else
                ++it;
        }
        return false;
    }

    /// Detaches and destroys all currently attached slots.
    ///
    /// Each slot is marked dead before removal. After this call `nslots()` returns 0.
    void detachAll() const
    {
        std::unique_lock<MutexT> guard(_mutex);
        while (!_slots.empty()) {
            _slots.back()->kill();
            _slots.pop_back();
        }
    }

    /// Emits the signal, invoking all live attached slots in priority order.
    ///
    /// For `Signal<bool(...)>`: iterates slots and returns `true` as soon as any
    /// slot returns `true`, stopping further propagation. Returns `false` if no
    /// slot handled the event.
    ///
    /// For `Signal<void(...)>`: calls every live slot unconditionally.
    ///
    /// When `MutexT` is `NullSharedMutex` the slot list is traversed directly
    /// (lock-free fast path). Otherwise a snapshot copy is taken under a shared
    /// lock before iteration, allowing slots to be added or removed mid-emission.
    ///
    /// @param args  Arguments forwarded to each connected slot.
    /// @return For `bool` return type: `true` if any slot handled the event, `false` otherwise.
    ///         For `void` return type: nothing.
    virtual RT emit(Args... args)
    {
        if constexpr (std::is_same_v<MutexT, NullSharedMutex>) {
            // Lock-free fast path: iterate slots directly (single-threaded)
            if constexpr (std::is_same_v<RT, bool>) {
                for (auto const& slot : _slots) {
                    if (slot->alive()) {
                        if ((*slot->delegate)(std::forward<Args>(args)...))
                            return true;
                    }
                }
                return false;
            } else {
                for (auto const& slot : _slots) {
                    if (slot->alive()) {
                        (*slot->delegate)(std::forward<Args>(args)...);
                    }
                }
            }
        } else {
            // Thread-safe path: copy slots under lock
            if constexpr (std::is_same_v<RT, bool>) {
                for (auto const& slot : slots()) {
                    if (slot->alive()) {
                        if ((*slot->delegate)(std::forward<Args>(args)...))
                            return true;
                    }
                }
                return false;
            } else {
                for (auto const& slot : slots()) {
                    if (slot->alive()) {
                        (*slot->delegate)(std::forward<Args>(args)...);
                    }
                }
            }
        }
    }

    /// Returns a snapshot copy of the current slot list.
    ///
    /// The copy is taken under a shared lock, so it is safe to call concurrently
    /// with attach/detach operations. Dead slots may be included if they were
    /// killed concurrently; callers should check `slot->alive()` if needed.
    ///
    /// @return A vector of `SlotPtr` representing all currently registered slots.
    std::vector<SlotPtr> slots() const
    {
        std::shared_lock<MutexT> guard(_mutex);
        return _slots;
    }

    /// Returns the number of slots currently registered with this signal.
    ///
    /// The count is taken under a shared lock. Slots that were concurrently
    /// killed but not yet erased may still be included in the count.
    ///
    /// @return The number of entries in the internal slot list.
    [[nodiscard]] size_t nslots() const
    {
        std::shared_lock<MutexT> guard(_mutex);
        return _slots.size();
    }

    /// Attaches a function; equivalent to `attach(func)`. @return Assigned slot ID.
    int operator+=(Function const& func) { return attach(func); }
    /// Attaches a pre-constructed slot; equivalent to `attach(slot)`. @return Assigned slot ID.
    int operator+=(SlotPtr slot) { return attach(slot); }
    /// Detaches the slot with the given ID; equivalent to `detach(id)`. @return `true` if removed.
    bool operator-=(int id) { return detach(id); }
    /// Detaches all slots for the given instance; equivalent to `detach(instance)`. @return `true` if any removed.
    bool operator-=(const void* instance) { return detach(instance); }
    /// Detaches the slot matching `slot`'s delegate; equivalent to `detach(slot)`. @return `true` if removed.
    bool operator-=(SlotPtr slot) { return detach(slot); }

    Signal() = default;

    /// Copy constructor; copies the slot list and last-assigned ID from `r`.
    /// @param r  The signal to copy from.
    Signal(const Signal& r)
        : _slots(r._slots)
        , _lastId(r._lastId)
    {
    }

    /// Copy assignment operator; copies the slot list and last-assigned ID from `r`.
    /// @param r  The signal to copy from.
    /// @return Reference to this signal.
    Signal& operator=(const Signal& r)
    {
        if (&r != this) {
            _slots = r._slots;
            _lastId = r._lastId;
        }
        return *this;
    }

private:
    mutable MutexT _mutex;
    mutable std::vector<SlotPtr> _slots;
    mutable int _lastId = 0;
};


using NullSignal = Signal<void()>;

/// Lock-free signal for use within a single libuv event loop thread.
/// All net/http signals operate on a single loop thread, so the
/// shared_mutex provides no safety benefit and adds overhead.
template <typename RT>
using LocalSignal = Signal<RT, NullSharedMutex>;


//
// Inline Helpers
//


/// Creates a slot that binds a non-const class member function to an instance.
///
/// The returned `SlotPtr` can be passed to `Signal::attach()` or `operator+=`,
/// and to `Signal::detach()` or `operator-=` to disconnect it later.
///
/// @tparam Class     The class that owns the member function.
/// @tparam RT        Return type of the member function.
/// @tparam Args      Parameter types of the member function.
/// @param instance   Pointer to the object on which `method` will be called.
/// @param method     Pointer to the non-const member function to bind.
/// @param id         Explicit slot ID to assign; pass `-1` to auto-assign.
/// @param priority   Higher values are called first; pass `-1` for default ordering.
/// @return A `SlotPtr` ready to attach to a compatible `Signal`.
template <class Class, class RT, typename... Args>
std::shared_ptr<internal::Slot<RT, Args...>>
slot(Class* instance, RT (Class::*method)(Args...), int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, Args...>>(
        new ClassDelegate<Class, RT, Args...>(instance, method), instance, id, priority);
}

/// Creates a slot that binds a `const` class member function to an instance.
///
/// @tparam Class     The class that owns the member function.
/// @tparam RT        Return type of the member function.
/// @tparam Args      Parameter types of the member function.
/// @param instance   Pointer to the object on which `method` will be called.
/// @param method     Pointer to the const member function to bind.
/// @param id         Explicit slot ID to assign; pass `-1` to auto-assign.
/// @param priority   Higher values are called first; pass `-1` for default ordering.
/// @return A `SlotPtr` ready to attach to a compatible `Signal`.
template <class Class, class RT, typename... Args>
std::shared_ptr<internal::Slot<RT, Args...>>
slot(Class* instance, RT (Class::*method)(Args...) const, int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, Args...>>(
        new ConstClassDelegate<Class, RT, Args...>(instance, method), instance, id, priority);
}

/// Creates a slot that wraps a free (static) function pointer.
///
/// @tparam RT        Return type of the function.
/// @tparam Args      Parameter types of the function.
/// @param method     Pointer to the free function to bind.
/// @param id         Explicit slot ID to assign; pass `-1` to auto-assign.
/// @param priority   Higher values are called first; pass `-1` for default ordering.
/// @return A `SlotPtr` ready to attach to a compatible `Signal`.
template <class RT, typename... Args>
std::shared_ptr<internal::Slot<RT, Args...>>
slot(RT (*method)(Args...), int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, Args...>>(
        new FunctionDelegate<RT, Args...>([method](Args... args) {
            return (*method)(std::forward<Args>(args)...);
        }),
        nullptr, id, priority);
}


/// Internal classes
namespace internal {

/// Internal storage for a single signal connection.
///
/// Owns the callable delegate, tracks the associated instance pointer for
/// bulk-detach operations, holds the slot ID and priority, and uses an
/// `atomic_flag` as a lock-free liveness bit so that slots can be marked
/// dead from one thread while another thread is mid-emission.
template <typename RT, typename... Args>
struct Slot
{
    /// The callable bound to this slot; invoked by `Signal::emit()`.
    std::unique_ptr<AbstractDelegate<RT, Args...>> delegate;

    /// Optional pointer to the owning object; used by `Signal::detach(const void*)`.
    /// `nullptr` for free-function and lambda slots.
    void* instance;

    /// Unique identifier assigned by `Signal::attach()`; used by `Signal::detach(int)`.
    int id;

    /// Emission order; slots with higher priority values are called first.
    int priority;

    /// Liveness flag. Set on construction; cleared by `kill()`.
    /// `alive()` reads and re-sets the flag atomically, making it safe to
    /// test from the emission thread while `kill()` is called concurrently.
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

    /// Constructs a slot, taking ownership of `delegate` and marking it alive.
    ///
    /// @param delegate  Heap-allocated delegate; ownership is transferred.
    /// @param instance  Optional owner pointer for instance-based detach.
    /// @param id        Initial slot ID; `-1` means not yet assigned.
    /// @param priority  Slot priority; higher values emit first.
    Slot(AbstractDelegate<RT, Args...>* delegate, void* instance = nullptr, int id = -1, int priority = -1)
        : delegate(delegate)
        , instance(instance)
        , id(id)
        , priority(priority)
    {
        flag.test_and_set();
    }

    ~Slot() = default;

    /// Marks the slot as dead; subsequent `alive()` calls return `false`.
    /// Safe to call concurrently with `alive()`.
    void kill()
    {
        flag.clear();
    }

    /// Returns `true` if the slot is still live, `false` if `kill()` has been called.
    ///
    /// Implemented as an atomic test-and-set so it can be polled during emission
    /// without a separate mutex.
    bool alive()
    {
        return flag.test_and_set();
    }

    /// NonCopyable and NonMovable
    // Slot(const Slot&) = delete;
    // Slot& operator=(const Slot&) = delete;
};

} // namespace internal


} // namespace icy


/// @\}
