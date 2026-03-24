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
    unsigned data; ///< Backing storage for the flag bits.

    /// Constructs a Bitwise with optional initial flags.
    /// @param flags Initial flag value (default: 0).
    Bitwise(unsigned flags = 0)
        : data(flags)
    {
    }

    /// Clears all flags (sets data to 0).
    virtual void reset() { data = 0; };

    /// Sets the given flag only if it is not already set.
    /// @param flag The flag bit(s) to set.
    virtual void set(unsigned flag)
    {
        if (!has(flag))
            data |= flag;
    }

    /// Unconditionally sets (OR) the given flag bits.
    /// @param flag The flag bit(s) to add.
    virtual void add(unsigned flag) { data |= flag; };

    /// Clears the given flag bits.
    /// @param flag The flag bit(s) to remove.
    virtual void remove(unsigned flag) { data &= ~flag; };

    /// Toggles (XOR) the given flag bits.
    /// @param flag The flag bit(s) to toggle.
    virtual void toggle(unsigned flag) { data ^= flag; };

    /// Returns true if all bits in flag are set.
    /// @param flag The flag bit(s) to test.
    /// @return true if every bit in flag is present in data.
    virtual bool has(unsigned flag) const { return (data & flag) == flag; };
};


} // namespace icy


/// @}
