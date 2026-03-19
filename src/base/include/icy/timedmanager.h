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
#include "icy/collection.h"
#include "icy/timeout.h"

#include <stdexcept>


namespace icy {

/// Timed pointer manager
///
/// Provides timed persistent data storage for class instances.
/// TValue must implement the clone() method.
template <class TKey, class TValue>
class /* ICY_EXTERN */ TimedManager : public PointerCollection<TKey, TValue>
{
public:
    using Base = PointerCollection<TKey, TValue>;
    using Base::add; // inherit base add overloads
    using TimeoutMap = std::map<TValue*, Timeout>;

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
    virtual void add(const TKey& key, TValue* item, long timeout = 0)
    {
        Base::free(key);      // Free existing item and timeout (if any)
        Base::add(key, item); // Add new entry
        if (timeout > 0)
            setTimeout(item, timeout);
    }

    /// Update the item expiry timeout
    virtual bool expires(const TKey& key, long timeout)
    {
        LTrace("Set expires: ", key, ": ", timeout);
        return expires(Base::get(key, false), timeout);
    }

    /// Update the item expiry timeout
    virtual bool expires(TValue* item, long timeout)
    {
        LTrace("Set expires: ", item, ": ", timeout);
        return setTimeout(item, timeout);
    }

    virtual void clear() override
    {
        Base::clear();
        std::lock_guard<std::mutex> guard(_tmutex);
        _timeouts.clear();
    }

protected:
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

    virtual void onRemove(const TKey& key, TValue* item) override
    {
        // Remove timeout entry
        std::lock_guard<std::mutex> guard(_tmutex);
        auto it = _timeouts.find(item);
        if (it != _timeouts.end())
            _timeouts.erase(it);

        Base::onRemove(key, item);
    }

    virtual void onTimeout(TValue* item)
    {
        Base::remove(item); // removes and deletes via unique_ptr
    }

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


/// @\}
