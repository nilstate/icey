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
#include "icy/logger.h"
#include "icy/signal.h"
#include "icy/util.h"

#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>


namespace icy {


/// A keyed store of unique_ptr values with optional lifecycle hooks.
/// Not thread-safe; designed for single-threaded event loop contexts.
///
/// Subclasses can override onAdd/onRemove for lifecycle reactions,
/// and add Signal members for external observers.
template <class TKey, class TValue>
class KeyedStore
{
public:
    using Map = std::map<TKey, std::unique_ptr<TValue>>;

    KeyedStore() = default;
    virtual ~KeyedStore() { _map.clear(); }

    // Non-copyable, movable
    KeyedStore(const KeyedStore&) = delete;
    KeyedStore& operator=(const KeyedStore&) = delete;
    KeyedStore(KeyedStore&&) = default;
    KeyedStore& operator=(KeyedStore&&) = default;

    /// Returns the item for `key`, or nullptr if not found.
    TValue* get(const TKey& key) const
    {
        auto it = _map.find(key);
        return it != _map.end() ? it->second.get() : nullptr;
    }

    /// Inserts a uniquely owned item. Returns a reference to the stored item.
    /// @throws std::runtime_error if the key already exists.
    TValue& add(const TKey& key, std::unique_ptr<TValue> item)
    {
        auto* raw = item.get();
        auto [it, ok] = _map.emplace(key, std::move(item));
        if (!ok)
            throw std::runtime_error("KeyedStore: duplicate key");
        onAdd(key, raw);
        return *raw;
    }

    /// Inserts if absent; returns false on duplicate (never throws).
    bool tryAdd(const TKey& key, std::unique_ptr<TValue> item)
    {
        if (_map.count(key))
            return false;
        auto* raw = item.get();
        _map.emplace(key, std::move(item));
        onAdd(key, raw);
        return true;
    }

    /// Inserts or replaces the item under `key`.
    TValue& put(const TKey& key, std::unique_ptr<TValue> item)
    {
        auto* raw = item.get();
        _map[key] = std::move(item);
        onAdd(key, raw);
        return *raw;
    }

    /// Removes and destroys the item under `key`.
    /// @return true if the item existed and was removed.
    bool erase(const TKey& key)
    {
        auto it = _map.find(key);
        if (it == _map.end())
            return false;
        auto* raw = it->second.get();
        auto owned = std::move(it->second);
        _map.erase(it);
        onRemove(key, raw);
        return true;
    }

    bool contains(const TKey& key) const { return _map.count(key) > 0; }
    bool empty() const { return _map.empty(); }
    size_t size() const { return _map.size(); }
    void clear() { _map.clear(); }

    /// Direct map access for iteration.
    Map& map() { return _map; }
    const Map& map() const { return _map; }

    // Iteration
    auto begin() { return _map.begin(); }
    auto end() { return _map.end(); }
    auto begin() const { return _map.begin(); }
    auto end() const { return _map.end(); }

protected:
    /// Override for lifecycle reactions.
    virtual void onAdd(const TKey&, TValue*) {}
    virtual void onRemove(const TKey&, TValue*) {}

    Map _map;
};


//
// KV Collection
//


/// A keyed value store (values stored by copy, not pointer).
template <class TKey, class TValue>
class KVCollection
{
public:
    using Map = std::map<TKey, TValue>;

    KVCollection() = default;
    ~KVCollection() = default;

    /// Inserts a value; returns false if key already exists.
    bool add(const TKey& key, const TValue& item)
    {
        auto [it, ok] = _map.emplace(key, item);
        return ok;
    }

    /// Returns the value or throws.
    TValue& get(const TKey& key)
    {
        auto it = _map.find(key);
        if (it != _map.end())
            return it->second;
        throw std::runtime_error("Item not found");
    }

    /// Returns the value or defaultValue.
    const TValue& get(const TKey& key, const TValue& defaultValue) const
    {
        auto it = _map.find(key);
        return it != _map.end() ? it->second : defaultValue;
    }

    bool remove(const TKey& key) { return _map.erase(key) > 0; }
    bool has(const TKey& key) const { return _map.count(key) > 0; }
    bool empty() const { return _map.empty(); }
    size_t size() const { return _map.size(); }
    void clear() { _map.clear(); }
    Map& map() { return _map; }
    const Map& map() const { return _map; }

protected:
    Map _map;
};


//
// NV Collection
//


/// A storage container for a name value collections.
/// This collection can store multiple entries for each
/// name, and it's getters are case-insensitive.
class Base_API NVCollection
{
public:
    using Map = std::vector<std::pair<std::string, std::string>>;
    using Iterator = Map::iterator;
    using ConstIterator = Map::const_iterator;

    NVCollection()
    {
    }

    NVCollection(const NVCollection& nvc)
        : _map(nvc._map)
    {
    }

    NVCollection(NVCollection&& nvc) noexcept
        : _map(std::move(nvc._map))
    {
    }

    virtual ~NVCollection()
    {
    }

    /// Assigns the name-value pairs of another NVCollection to this one.
    NVCollection& operator=(const NVCollection& nvc);
    NVCollection& operator=(NVCollection&& nvc) noexcept;

    /// Returns the value of the (first) name-value pair with the given name.
    ///
    /// Throws a NotFoundException if the name-value pair does not exist.
    const std::string& operator[](std::string_view name) const;

    /// Sets the value of the (first) name-value pair with the given name.
    void set(const std::string& name, const std::string& value);

    /// Adds a new name-value pair with the given name and value.
    void add(const std::string& name, const std::string& value);

    /// Adds a new name-value pair using move semantics.
    void add(std::string&& name, std::string&& value);

    /// Returns the value of the first name-value pair with the given name.
    ///
    /// Throws a NotFoundException if the name-value pair does not exist.
    const std::string& get(std::string_view name) const;

    /// Returns the value of the first name-value pair with the given name.
    /// If no value with the given name has been found, the defaultValue is
    /// returned.
    const std::string& get(std::string_view name, const std::string& defaultValue) const;

    /// Returns true if there is at least one name-value pair
    /// with the given name.
    bool has(std::string_view name) const;

    /// Returns an iterator pointing to the first name-value pair
    /// with the given name.
    ConstIterator find(std::string_view name) const;

    /// Returns an iterator pointing to the begin of
    /// the name-value pair collection.
    ConstIterator begin() const;

    /// Returns an iterator pointing to the end of
    /// the name-value pair collection.
    ConstIterator end() const;

    /// Returns true iff the header does not have any content.
    bool empty() const;

    /// Returns the number of name-value pairs in the
    /// collection.
    int size() const;

    /// Removes all name-value pairs with the given name.
    void erase(std::string_view name);

    /// Removes all name-value pairs and their values.
    void clear();

private:
    Map _map;
};


using StringMap = std::map<std::string, std::string>;
using StringVec = std::vector<std::string>;


} // namespace icy


/// @}
