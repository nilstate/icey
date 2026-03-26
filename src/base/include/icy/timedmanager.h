///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/collection.h"
#include "icy/timeout.h"

#include <stdexcept>


namespace icy {

/// Timed pointer manager
///
/// Provides timed persistent data storage for class instances.
/// TValue must implement the clone() method.
template <class TKey, class TValue>
class /* ICY_EXTERN */ TimedManager : public KeyedStore<TKey, TValue>
{
public:
    using Base = KeyedStore<TKey, TValue>;
    using TimeoutMap = std::map<TValue*, Timeout>;

    /// Constructs a TimedManager and starts the internal expiry-check timer.
    /// @param loop Event loop used by the internal timer (default: process-wide default loop).
    TimedManager(uv::Loop* loop = uv::defaultLoop())
        : _timer(100, 100, loop) // check every 100ms
    {
        _timer.start([this]() { onTimerUpdate(); });
    }

    virtual ~TimedManager()
    {
    }

    /// Add an item which will expire (and be deleted) after the
    /// specified timeout value.
    /// If the timeout is 0 the item will be stored indefinitely.
    /// The TimedManager assumes ownership of the given pointer.
    void add(const TKey& key, std::unique_ptr<TValue> item, long timeout = 0)
    {
        Base::erase(key); // Free existing item and timeout (if any)
        auto* raw = &Base::add(key, std::move(item));
        if (timeout > 0)
            setTimeout(raw, timeout);
    }

    /// Update the item expiry timeout
    virtual bool expires(const TKey& key, long timeout)
    {
        LTrace("Set expires: ", key, ": ", timeout);
        return expires(Base::get(key), timeout);
    }

    /// Update the item expiry timeout
    virtual bool expires(TValue* item, long timeout)
    {
        LTrace("Set expires: ", item, ": ", timeout);
        return setTimeout(item, timeout);
    }

    /// Removes all items and their associated timeouts.
    virtual void clear() override
    {
        Base::clear();
        std::lock_guard<std::mutex> guard(_tmutex);
        _timeouts.clear();
    }

protected:
    /// Sets or removes the expiry timeout for a specific item pointer.
    /// If timeout > 0, starts a countdown; if timeout == 0, removes any existing timeout.
    /// @param item    Pointer to the managed item.
    /// @param timeout Delay in milliseconds, or 0 to clear the timeout.
    /// @return true on success.
    /// @throws std::logic_error if item is nullptr.
    virtual bool setTimeout(TValue* item, long timeout)
    {
        if (item) {
            std::lock_guard<std::mutex> guard(_tmutex);
            if (timeout > 0) {
                LTrace("Set timeout: ", item, ": ", timeout);
                auto& t = _timeouts[item];
                t.setDelay(timeout);
                t.start();
            } else {
                auto it = _timeouts.find(item);
                if (it != _timeouts.end()) {
                    _timeouts.erase(it);
                }
            }
            return true;
        }
        throw std::logic_error("TimedManager::setTimeout: unknown item");
        return false;
    }

    /// Called when an item is removed from the collection.
    /// Erases the item's timeout entry and calls the base implementation.
    /// @param key  Key of the removed item.
    /// @param item Pointer to the removed item.
    virtual void onRemove(const TKey& key, TValue* item) override
    {
        // Remove timeout entry
        std::lock_guard<std::mutex> guard(_tmutex);
        auto it = _timeouts.find(item);
        if (it != _timeouts.end())
            _timeouts.erase(it);

        Base::onRemove(key, item);
    }

    /// Called when an item's timeout expires. Default implementation removes and deletes the item.
    /// @param item Pointer to the expired item.
    virtual void onTimeout(TValue* item)
    {
        // Find by pointer and erase by key
        for (auto& [key, val] : this->_map) {
            if (val.get() == item) {
                Base::erase(key);
                return;
            }
        }
    }

    /// Internal timer callback; iterates all tracked timeouts and calls onTimeout()
    /// for any that have expired.
    void onTimerUpdate()
    {
        _tmutex.lock();
        TimeoutMap timeouts(_timeouts);
        _tmutex.unlock();

        for (const auto& [item, timeout] : timeouts) {
            if (timeout.expired()) {
                LTrace("Item expired: ", item);
                onTimeout(item);
            }
        }
    }

    mutable std::mutex _tmutex;
    TimeoutMap _timeouts;
    Timer _timer;
};


} // namespace icy


/// @}
