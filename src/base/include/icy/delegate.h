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


#include <functional>


namespace icy {


/// Abstract delegate interface.
///
/// The `Delegate` class contains a pointer to a function. This wrapper class
/// is used instead of `std::function` since it is interchangable with fast
/// delegates and also provides an equality operator for comparing the
/// underlying function where supported.
///
template <typename RT, typename... Args>
struct AbstractDelegate
{
    virtual ~AbstractDelegate() {}

    /// Invokes the underlying callable with the supplied arguments.
    /// @param args Arguments forwarded to the wrapped function.
    /// @return Result of the wrapped function call.
    virtual RT operator()(Args... args) const = 0;

    /// Compares two delegates for equality (same target function and instance).
    /// @param that Other delegate to compare against.
    /// @return True if both delegates refer to the same callable target.
    virtual bool operator==(const AbstractDelegate<RT, Args...>& that) const = 0;
};


/// The `FunctionDelegate` contains a `std::function`.
///
template <typename RT, typename... Args>
struct FunctionDelegate : AbstractDelegate<RT, Args...>
{
    std::function<RT(Args...)> func;

    /// @param func `std::function` to wrap.
    FunctionDelegate(std::function<RT(Args...)> func)
        : func(func)
    {
    }

    /// Calls the wrapped `std::function`.
    /// @param args Arguments forwarded to the function.
    /// @return Function result.
    virtual RT operator()(Args... args) const override
    {
        return func(std::forward<Args>(args)...);
    }

    /// Always returns false; `std::function` targets cannot be compared for equality.
    virtual bool operator==(const AbstractDelegate<RT, Args...>& /* that */) const override
    {
        return false; // dynamic function delegates cannot be compared
    }
};


/// The `ClassDelegate` contains a pointer to a class member.
///
/// This class implements fast delegates and function comparison.
///
template <class Class, typename RT, typename... Args>
struct ClassDelegate : AbstractDelegate<RT, Args...>
{
    Class* instance;
    RT (Class::*method)(Args...);

    /// @param instance Object on which to invoke the member function.
    /// @param method   Pointer to the member function to wrap.
    ClassDelegate(Class* instance, RT (Class::*method)(Args...))
        : instance(instance)
        , method(method)
    {
    }

    /// Calls the member function on `instance`.
    /// @param args Arguments forwarded to the method.
    /// @return Method result.
    virtual RT operator()(Args... args) const override
    {
        return (instance->*method)(std::forward<Args>(args)...);
    }

    /// @param that Other delegate to compare.
    /// @return True if both delegates wrap the same instance/method pair.
    virtual bool operator==(const AbstractDelegate<RT, Args...>& that) const override
    {
        auto other = dynamic_cast<const ClassDelegate*>(&that);
        return other && other->instance == this->instance &&
               other->method == this->method;
    }
};


/// The `ConstClassDelegate` contains a pointer to a `const` class member.
///
/// This class implements fast delegates and function comparison.
///
template <class Class, typename RT, typename... Args>
struct ConstClassDelegate : AbstractDelegate<RT, Args...>
{
    Class* instance;
    RT (Class::*method)(Args...) const;

    /// @param instance Object on which to invoke the const member function.
    /// @param method   Pointer to the const member function to wrap.
    ConstClassDelegate(Class* instance, RT (Class::*method)(Args...) const)
        : instance(instance)
        , method(method)
    {
    }

    /// Calls the const member function on `instance`.
    /// @param args Arguments forwarded to the method.
    /// @return Method result.
    virtual RT operator()(Args... args) const override
    {
        return (instance->*method)(std::forward<Args>(args)...);
    }

    /// @param that Other delegate to compare.
    /// @return True if both delegates wrap the same instance/method pair.
    virtual bool operator==(const AbstractDelegate<RT, Args...>& that) const override
    {
        auto other = dynamic_cast<const ConstClassDelegate*>(&that);
        return other && other->instance == this->instance &&
               other->method == this->method;
    }
};


/// Polymorphic function delegate.
///
/// Theis class contains a pointer to a class member that receices a derived
/// subclass (`PT`) of the base type specified by the `IT` param.
///
template <class Class, typename RT, class PT, class IT>
struct PolymorphicDelegate : AbstractDelegate<RT, IT&>
{
    Class* instance;
    RT (Class::*method)(PT&);

    /// @param instance Object on which to invoke the member function.
    /// @param method   Pointer to the member function accepting a derived `PT&`.
    PolymorphicDelegate(Class* instance, RT (Class::*method)(PT&))
        : instance(instance)
        , method(method)
    {
    }

    /// Attempts to cast `object` to `PT`; invokes the method if successful.
    /// Returns a default-constructed `RT` if the cast fails (packet type mismatch).
    /// @param object Base-type reference; cast to `PT` before dispatch.
    /// @return Method result, or default `RT` on cast failure.
    virtual RT operator()(IT& object) const override
    {
        auto test = dynamic_cast<PT*>(&object);
        if (test)
            return (instance->*method)(*test);
        return RT();
    }

    /// @param that Other delegate to compare.
    /// @return True if both delegates wrap the same instance/method pair.
    virtual bool operator==(const AbstractDelegate<RT, IT&>& that) const override
    {
        auto other = dynamic_cast<const PolymorphicDelegate*>(&that);
        return other && other->instance == this->instance &&
               other->method == this->method;
    }
};


} // namespace icy


/// @\}
