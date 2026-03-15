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


#include <functional>


namespace scy {


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
    virtual RT operator()(Args... args) const = 0;
    virtual bool operator==(const AbstractDelegate<RT, Args...>& that) const = 0;
};


/// The `FunctionDelegate` contains a `std::function`.
///
template <typename RT, typename... Args>
struct FunctionDelegate : AbstractDelegate<RT, Args...>
{
    std::function<RT(Args...)> func;

    FunctionDelegate(std::function<RT(Args...)> func)
        : func(func)
    {
    }

    virtual RT operator()(Args... args) const override
    {
        return func(std::forward<Args>(args)...);
    }

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

    ClassDelegate(Class* instance, RT (Class::*method)(Args...))
        : instance(instance)
        , method(method)
    {
    }

    virtual RT operator()(Args... args) const override
    {
        return (instance->*method)(std::forward<Args>(args)...);
    }

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

    ConstClassDelegate(Class* instance, RT (Class::*method)(Args...) const)
        : instance(instance)
        , method(method)
    {
    }

    virtual RT operator()(Args... args) const override
    {
        return (instance->*method)(std::forward<Args>(args)...);
    }

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

    PolymorphicDelegate(Class* instance, RT (Class::*method)(PT&))
        : instance(instance)
        , method(method)
    {
    }

    virtual RT operator()(IT& object) const override
    {
        auto test = dynamic_cast<PT*>(&object);
        if (test)
            return (instance->*method)(*test);
        return RT();
    }

    virtual bool operator==(const AbstractDelegate<RT, IT&>& that) const override
    {
        auto other = dynamic_cast<const PolymorphicDelegate*>(&that);
        return other && other->instance == this->instance &&
               other->method == this->method;
    }
};


} // namespace scy


/// @\}
