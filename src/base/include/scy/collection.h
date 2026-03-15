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


#include "scy/base.h"
#include "scy/logger.h"
#include "scy/signal.h"
#include "scy/util.h"

#include <cassert>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <vector>


namespace scy {


/// AbstractCollection is an abstract interface for managing a
/// key-value store of indexed pointers.
template <class TKey, class TValue>
class AbstractCollection
{
public:
    AbstractCollection() {};
    virtual ~AbstractCollection() {}

    virtual bool add(const TKey& key, TValue* item, bool whiny = true) = 0;
    virtual bool remove(const TValue* item) = 0;
    virtual TValue* remove(const TKey& key) = 0;
    virtual bool exists(const TKey& key) const = 0;
    virtual bool exists(const TValue* item) const = 0;
    virtual bool free(const TKey& key) = 0;
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    virtual TValue* get(const TKey& key, bool whiny = true) const = 0;
    virtual void clear() = 0;
};


//
// Pointer Collection
//


/// This collection is used to maintain a map of unique_ptr
/// values indexed by key in a thread-safe way.
template <class TKey, class TValue>
class /* Base_API */ PointerCollection : public AbstractCollection<TKey, TValue>
{
public:
    using Map = std::map<TKey, std::unique_ptr<TValue>>;

public:
    PointerCollection() {}

    virtual ~PointerCollection() { clear(); }

    virtual bool add(const TKey& key, TValue* item, bool whiny = true) override
    {
        return add(key, std::unique_ptr<TValue>(item), whiny);
    }

    virtual bool add(const TKey& key, std::unique_ptr<TValue> item, bool whiny = true)
    {
        if (exists(key)) {
            if (whiny)
                throw std::runtime_error("Item already exists");
            return false;
        }
        auto* raw = item.get();
        {
            std::unique_lock<std::shared_mutex> guard(_mutex);
            _map[key] = std::move(item);
        }
        onAdd(key, raw);
        return true;
    }

    virtual void update(const TKey& key, std::unique_ptr<TValue> item)
    {
        auto* raw = item.get();
        {
            std::unique_lock<std::shared_mutex> guard(_mutex);
            _map[key] = std::move(item);
        }
        onAdd(key, raw);
    }

    virtual TValue* get(const TKey& key, bool whiny = true) const override
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        auto it = _map.find(key);
        if (it != _map.end()) {
            return it->second.get();
        } else if (whiny) {
            throw std::runtime_error("Item not found");
        }

        return nullptr;
    }

    virtual bool free(const TKey& key) override
    {
        std::unique_ptr<TValue> item;
        TValue* raw = nullptr;
        {
            std::unique_lock<std::shared_mutex> guard(_mutex);
            auto it = _map.find(key);
            if (it != _map.end()) {
                raw = it->second.get();
                item = std::move(it->second);
                _map.erase(it);
            }
        }
        if (raw)
            onRemove(key, raw);
        // item is destroyed here via unique_ptr
        return raw != nullptr;
    }

    virtual TValue* remove(const TKey& key) override
    {
        TValue* raw = nullptr;
        std::unique_ptr<TValue> owned;
        {
            std::unique_lock<std::shared_mutex> guard(_mutex);
            auto it = _map.find(key);
            if (it != _map.end()) {
                owned = std::move(it->second);
                raw = owned.release(); // caller takes ownership
                _map.erase(it);
            }
        }
        if (raw)
            onRemove(key, raw);
        return raw;
    }

    virtual bool remove(const TValue* item) override
    {
        TKey key;
        TValue* raw = nullptr;
        std::unique_ptr<TValue> owned;
        {
            std::unique_lock<std::shared_mutex> guard(_mutex);
            for (auto it = _map.begin(); it != _map.end(); ++it) {
                if (item == it->second.get()) {
                    key = it->first;
                    raw = it->second.get();
                    owned = std::move(it->second);
                    _map.erase(it);
                    break;
                }
            }
        }
        if (raw)
            onRemove(key, raw);
        // owned is destroyed here via unique_ptr
        return raw != nullptr;
    }

    virtual bool exists(const TKey& key) const override
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        return _map.find(key) != _map.end();
    }

    virtual bool exists(const TValue* item) const override
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        for (const auto& [key, val] : _map) {
            if (item == val.get())
                return true;
        }
        return false;
    }

    virtual bool empty() const override
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        return _map.empty();
    }

    virtual size_t size() const override
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        return _map.size();
    }

    virtual void clear() override
    {
        std::unique_lock<std::shared_mutex> guard(_mutex);
        _map.clear();
    }

    virtual Map& map()
    {
        std::unique_lock<std::shared_mutex> guard(_mutex);
        return _map;
    }

    virtual const Map& map() const
    {
        std::shared_lock<std::shared_mutex> guard(_mutex);
        return _map;
    }

    virtual void onAdd(const TKey&, TValue*)
    {
        // override me
    }

    virtual void onRemove(const TKey&, TValue*)
    {
        // override me
    }

protected:
    mutable std::shared_mutex _mutex;
    Map _map;
};


//
// Live Collection
//


template <class TKey, class TValue>
class /* Base_API */ LiveCollection : public PointerCollection<TKey, TValue>
{
public:
    using Base = PointerCollection<TKey, TValue>;

public:
    virtual void onAdd(const TKey&, TValue* item) override
    {
        ItemAdded.emit(*item);
    }

    virtual void onRemove(const TKey&, TValue* item) override
    {
        ItemRemoved.emit(*item);
    }

    Signal<void(TValue&)> ItemAdded;
    Signal<void(const TValue&)> ItemRemoved;
};


//
// KV Collection
//


/// Reusable stack based unique key-value store for DRY coding.
template <class TKey, class TValue>
class /* Base_API */ KVCollection
{
public:
    using Map = std::map<TKey, TValue>;

public:
    KVCollection()
    {
    }

    virtual ~KVCollection()
    {
        clear();
    }

    virtual bool add(const TKey& key, const TValue& item, bool update = true, bool whiny = true)
    {
        if (!update && has(key)) {
            if (whiny)
                throw std::runtime_error("Item already exists");
            return false;
        }

        // _map[key] = item;
        _map.insert(typename Map::value_type(key, item));
        return true;
    }

    virtual TValue& get(const TKey& key)
    {
        typename Map::iterator it = _map.find(key);
        if (it != _map.end())
            return it->second;
        else
            throw std::runtime_error("Item not found");
    }

    virtual const TValue& get(const TKey& key, const TValue& defaultValue) const
    {
        typename Map::const_iterator it = _map.find(key);
        if (it != _map.end())
            return it->second;
        return defaultValue;
    }

    virtual bool remove(const TKey& key)
    {
        typename Map::iterator it = _map.find(key);
        if (it != _map.end()) {
            _map.erase(it);
            return true;
        }
        return false;
    }

    virtual bool has(const TKey& key) const
    {
        return _map.find(key) != _map.end();
    }

    virtual bool empty() const
    {
        return _map.empty();
    }

    virtual size_t size() const
    {
        return _map.size();
    }

    virtual void clear()
    {
        _map.clear();
    }

    virtual Map& map()
    {
        return _map;
    }

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
    struct ILT
    {
        bool operator()(const std::string& s1, const std::string& s2) const
        {
            return util::icompare(s1, s2) < 0;
        }
    };

    using Map = std::multimap<std::string, std::string, ILT>;
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
    const std::string& operator[](const std::string& name) const;

    /// Sets the value of the (first) name-value pair with the given name.
    void set(const std::string& name, const std::string& value);

    /// Adds a new name-value pair with the given name and value.
    void add(const std::string& name, const std::string& value);

    /// Returns the value of the first name-value pair with the given name.
    ///
    /// Throws a NotFoundException if the name-value pair does not exist.
    const std::string& get(const std::string& name) const;

    /// Returns the value of the first name-value pair with the given name.
    /// If no value with the given name has been found, the defaultValue is
    /// returned.
    const std::string& get(const std::string& name, const std::string& defaultValue) const;

    /// Returns true if there is at least one name-value pair
    /// with the given name.
    bool has(const std::string& name) const;

    /// Returns an iterator pointing to the first name-value pair
    /// with the given name.
    ConstIterator find(const std::string& name) const;

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
    void erase(const std::string& name);

    /// Removes all name-value pairs and their values.
    void clear();

private:
    Map _map;
};


using StringMap = std::map<std::string, std::string>;
using StringVec = std::vector<std::string>;


} // namespace scy


/// @\}
