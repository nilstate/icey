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


#include "icy/logger.h"
#include <chrono>


namespace icy {


/// @addtogroup base
/// A simple message rate limiter based on the token bucket algorithm.
class /* ICY_EXTERN */ RateLimiter
{
public:
    double rate;      ///< How many messages
    double seconds;   ///< Over how many seconds
    double allowance; ///< Remaining send allowance

    RateLimiter(double rate = 5.0, double seconds = 6.0)
        : rate(rate)
        , seconds(seconds)
        , allowance(rate)
        , _lastCheck()
        , _started(false)
    {
    }

    bool canSend()
    {
        auto current = std::chrono::steady_clock::now();
        if (!_started) {
            _lastCheck = current;
            _started = true;
        }
        double elapsed = std::chrono::duration<double>(current - _lastCheck).count();
        _lastCheck = current;
        allowance += elapsed * (rate / seconds);

        LTrace("Can send: allowance=", allowance, " elapsed=", elapsed,
               " rate=", rate, " seconds=", seconds);

        if (allowance > rate)
        {
            allowance = rate; // throttle
            LTrace("Throttling: ", allowance);
        }
        else if (allowance < 1.0)
        {
            LTrace("Message rate exceeded: ", allowance);
            return false;
        }
        allowance -= 1.0;
        LTrace("Can send message: ", allowance);
        return true;
    }

private:
    std::chrono::steady_clock::time_point _lastCheck;
    bool _started;
};


} // namespace icy


/// @\}
