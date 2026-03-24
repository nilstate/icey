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


#include "icy/signal.h"

#include <map>


namespace icy {


/// Abstract interface for object registries
template <class ItemT>
class IRegistry
{
    template <typename T>
    static ItemT* createT()
    {
        return new T();
    }

public:
    using TypeMap = std::map<std::string, ItemT* (*)()>;

    IRegistry() = default;
    virtual ~IRegistry() = default;

    /// Creates and returns a new heap-allocated instance of the type registered under key s.
    /// @param s The registration key (type name).
    /// @return Pointer to the new instance, or nullptr if s is not registered.
    virtual ItemT* createInstance(const std::string& s)
    {
        typename TypeMap::iterator it = _types.find(s);
        if (it == _types.end())
            return nullptr;
        return it->second();
    }

    /// Registers type T under the given key s. Emits TypeRegistered.
    /// Subsequent calls to createInstance(s) will return `new T()`.
    /// @tparam T Concrete type to register; must be default-constructible and derive from ItemT.
    /// @param s Registration key (type name).
    template <typename T>
    void registerType(const std::string& s)
    {
        _types.insert(std::make_pair(s, &createT<T>));
        TypeRegistered.emit(s);
    }

    /// Removes the type registered under key s. Emits TypeUnregistered.
    /// Does nothing if s is not registered.
    /// @param s Registration key to remove.
    virtual void unregisterType(const std::string& s)
    {
        auto it = _types.find(s);
        if (it == _types.end())
            return;
        _types.erase(it);
        TypeUnregistered.emit(s);
    }

    /// Returns a copy of the current type map.
    /// @return Map of registration keys to factory function pointers.
    TypeMap types() const { return _types; }

    Signal<void(const std::string&)> TypeRegistered;
    Signal<void(const std::string&)> TypeUnregistered;

private:
    TypeMap _types;
};


} // namespace icy


/// @}
