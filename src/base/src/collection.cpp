///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/collection.h"

#include <algorithm>


namespace icy {


NVCollection& NVCollection::operator=(const NVCollection& nvc)
{
    if (&nvc != this) {
        _map = nvc._map;
    }
    return *this;
}


NVCollection& NVCollection::operator=(NVCollection&& nvc) noexcept
{
    if (&nvc != this) {
        _map = std::move(nvc._map);
    }
    return *this;
}


const std::string& NVCollection::operator[](std::string_view name) const
{
    auto it = find(name);
    if (it != _map.end())
        return it->second;
    else
        throw std::runtime_error("Item not found: " + std::string(name));
}


void NVCollection::set(const std::string& name, const std::string& value)
{
    auto it = std::find_if(_map.begin(), _map.end(),
        [&](const auto& p) { return util::icompare(p.first, name) == 0; });
    if (it != _map.end())
        it->second = value;
    else
        _map.emplace_back(name, value);
}


void NVCollection::add(const std::string& name, const std::string& value)
{
    _map.emplace_back(name, value);
}


void NVCollection::add(std::string&& name, std::string&& value)
{
    _map.emplace_back(std::move(name), std::move(value));
}


const std::string& NVCollection::get(std::string_view name) const
{
    auto it = find(name);
    if (it != _map.end())
        return it->second;
    else
        throw std::runtime_error("Item not found: " + std::string(name));
}


const std::string& NVCollection::get(std::string_view name,
                                     const std::string& defaultValue) const
{
    auto it = find(name);
    if (it != _map.end())
        return it->second;
    else
        return defaultValue;
}


bool NVCollection::has(std::string_view name) const
{
    return find(name) != _map.end();
}


NVCollection::ConstIterator NVCollection::find(std::string_view name) const
{
    return std::find_if(_map.begin(), _map.end(),
        [&](const auto& p) { return util::icompare(p.first, name) == 0; });
}


NVCollection::ConstIterator NVCollection::begin() const
{
    return _map.begin();
}


NVCollection::ConstIterator NVCollection::end() const
{
    return _map.end();
}


bool NVCollection::empty() const
{
    return _map.empty();
}


int NVCollection::size() const
{
    return static_cast<int>(_map.size());
}


void NVCollection::erase(std::string_view name)
{
    _map.erase(std::remove_if(_map.begin(), _map.end(),
        [&](const auto& p) { return util::icompare(p.first, name) == 0; }),
        _map.end());
}


void NVCollection::clear()
{
    _map.clear();
}


} // namespace icy


/// @}
