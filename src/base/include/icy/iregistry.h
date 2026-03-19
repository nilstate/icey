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

    virtual ItemT* createInstance(const std::string& s)
    {
        typename TypeMap::iterator it = _types.find(s);
        if (it == _types.end())
            return nullptr;
        return it->second();
    }

    template <typename T>
    void registerType(const std::string& s)
    {
        _types.insert(std::make_pair(s, &createT<T>));
        TypeRegistered.emit(s);
    }

    virtual void unregisterType(const std::string& s)
    {
        auto it = _types.find(s);
        if (it == _types.end())
            return;
        _types.erase(it);
        TypeUnregistered.emit(s);
    }

    TypeMap types() const { return _types; }

    Signal<void(const std::string&)> TypeRegistered;
    Signal<void(const std::string&)> TypeUnregistered;

private:
    TypeMap _types;
};


} // namespace icy


/// @\}
