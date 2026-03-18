///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/collection.h"


namespace scy {


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


const std::string& NVCollection::operator[](const std::string& name) const
{
    ConstIterator it = _map.find(name);
    if (it != _map.end())
        return it->second;
    else
        throw std::runtime_error("Item not found: " + name);
}


void NVCollection::set(const std::string& name, const std::string& value)
{
    Iterator it = _map.find(name);
    if (it != _map.end())
        it->second = value;
    else
        _map.insert(Map::value_type(name, value));
}


void NVCollection::add(const std::string& name, const std::string& value)
{
    _map.insert(Map::value_type(name, value));
}


void NVCollection::add(std::string&& name, std::string&& value)
{
    _map.emplace(std::move(name), std::move(value));
}


const std::string& NVCollection::get(const std::string& name) const
{
    ConstIterator it = _map.find(name);
    if (it != _map.end())
        return it->second;
    else
        throw std::runtime_error("Item not found: " + name);
}


const std::string& NVCollection::get(const std::string& name,
                                     const std::string& defaultValue) const
{
    ConstIterator it = _map.find(name);
    if (it != _map.end())
        return it->second;
    else
        return defaultValue;
}


bool NVCollection::has(const std::string& name) const
{
    return _map.find(name) != _map.end();
}


NVCollection::ConstIterator NVCollection::find(const std::string& name) const
{
    return _map.find(name);
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


void NVCollection::erase(const std::string& name)
{
    _map.erase(name);
}


void NVCollection::clear()
{
    _map.clear();
}


} // namespace scy


/// @\}
