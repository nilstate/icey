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


namespace icy {


/// Container for smart management of bitwise integer flags.
struct Bitwise
{
    unsigned data; // storage integer

    Bitwise(unsigned flags = 0)
        : data(flags)
    {
    }

    virtual void reset() { data = 0; };
    virtual void set(unsigned flag)
    {
        if (!has(flag))
            data |= flag;
    }

    virtual void add(unsigned flag) { data |= flag; };
    virtual void remove(unsigned flag) { data &= ~flag; };
    virtual void toggle(unsigned flag) { data ^= flag; };
    virtual bool has(unsigned flag) const { return (data & flag) == flag; };
};


} // namespace icy


/// @\}
