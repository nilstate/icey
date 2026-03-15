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


#include "scy/delegate.h"
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <vector>


namespace scy {


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
template <typename RT>
class Signal;
template <typename RT, typename... Args>
class Signal<RT(Args...)>
{
public:
    using Function = std::function<RT(Args...)>;
    using SlotPtr = std::shared_ptr<internal::Slot<RT, Args...>>;

    /// Connects a `lambda` or `std::function` to the `Signal`.
    /// The returned value can be used to detach the slot.
    [[nodiscard]] int attach(Function const& func, void* instance = nullptr, int id = -1, int priority = -1) const
    {
        return attach(std::make_shared<internal::Slot<RT, Args...>>(
            new FunctionDelegate<RT, Args...>(func), instance, id, priority));
    }

    /// Connects a `SlotPtr` instance to the `Signal`.
    /// The returned value can be used to detach the slot.
    [[nodiscard]] int attach(SlotPtr slot) const
    {
        detach(slot); // clear duplicates
        std::unique_lock<std::shared_mutex> guard(_mutex);
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

    /// Detaches a previously attached slot.
    bool detach(int id) const
    {
        std::unique_lock<std::shared_mutex> guard(_mutex);
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

    /// Detaches all slots for the given instance.
    bool detach(const void* instance) const
    {
        std::unique_lock<std::shared_mutex> guard(_mutex);
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

    /// Detaches all attached functions for the given instance.
    bool detach(SlotPtr other) const
    {
        std::unique_lock<std::shared_mutex> guard(_mutex);
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

    /// Detaches all previously attached functions.
    void detachAll() const
    {
        std::unique_lock<std::shared_mutex> guard(_mutex);
        while (!_slots.empty()) {
            _slots.back()->kill();
            _slots.pop_back();
        }
    }

    /// Emits the signal to all attached functions.
    /// For Signal<bool(...)>, returns true if any slot returned true
    /// (stops propagation to remaining slots).
    /// For Signal<void(...)>, calls all slots unconditionally.
    virtual RT emit(Args... args)
    {
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

    /// Returns the managed slot list.
    std::vector<SlotPtr> slots() const
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        return _slots;
    }

    /// Returns the number of active slots.
    [[nodiscard]] size_t nslots() const
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        return _slots.size();
    }

    /// Convenience operators
    int operator+=(Function const& func) { return attach(func); }
    int operator+=(SlotPtr slot) { return attach(slot); }
    bool operator-=(int id) { return detach(id); }
    bool operator-=(const void* instance) { return detach(instance); }
    bool operator-=(SlotPtr slot) { return detach(slot); }

    // static bool ComparePrioroty(const SlotPtr* l, const SlotPtr* r)
    // {
    //     return l->priority > r->priority;
    // }

    /// Default constructor
    Signal()
    {
    }

    /// Copy constructor
    Signal(const Signal& r)
        : _slots(r._slots)
        , _lastId(r._lastId)
    {
    }

    /// Assignment operator
    Signal& operator=(const Signal& r)
    {
        if (&r != this) {
            _slots = r._slots;
            _lastId = r._lastId;
        }
        return *this;
    }

private:
    mutable std::shared_mutex _mutex;
    mutable std::vector<SlotPtr> _slots;
    mutable int _lastId = 0;
};


using NullSignal = Signal<void()>;


//
// Inline Helpers
//


// Class member function slot
template <class Class, class RT, typename... Args>
std::shared_ptr<internal::Slot<RT, Args...>>
slot(Class* instance, RT (Class::*method)(Args...), int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, Args...>>(
        new ClassDelegate<Class, RT, Args...>(instance, method), instance, id, priority);
}

// Const class member function slot
template <class Class, class RT, typename... Args>
std::shared_ptr<internal::Slot<RT, Args...>>
slot(Class* instance, RT (Class::*method)(Args...) const, int id = -1, int priority = -1)
{
    return std::make_shared<internal::Slot<RT, Args...>>(
        new ConstClassDelegate<Class, RT, Args...>(instance, method), instance, id, priority);
}

// Static function slot
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

/// Signal slot storage class.
template <typename RT, typename... Args>
struct Slot
{
    std::unique_ptr<AbstractDelegate<RT, Args...>> delegate;
    void* instance;
    int id;
    int priority;
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

    Slot(AbstractDelegate<RT, Args...>* delegate, void* instance = nullptr, int id = -1, int priority = -1)
        : delegate(delegate)
        , instance(instance)
        , id(id)
        , priority(priority)
    {
        flag.test_and_set();
    }

    ~Slot() = default;

    void kill()
    {
        flag.clear();
    }

    bool alive()
    {
        return flag.test_and_set();
    }

    /// NonCopyable and NonMovable
    // Slot(const Slot&) = delete;
    // Slot& operator=(const Slot&) = delete;
};

} // namespace internal


} // namespace scy


/// @\}
