///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/av/format.h"
#include <mutex>
#include <string_view>
#include <vector>


namespace icy {
namespace av {


class AV_API FormatRegistry
{
public:
    static FormatRegistry& instance();

    FormatRegistry();
    virtual ~FormatRegistry() noexcept;

    virtual Format& get(std::string_view name);
    virtual Format& getByID(std::string_view id);

    /// Returns the default media format.
    virtual Format& getOrDefault(std::string_view name);

    /// If a default has been specified it will be
    /// returned, other the format with the highest
    /// priority will take precedence.
    virtual Format& getDefault();

    /// Registers the given media format overriding
    /// existing media formats of the same name.
    virtual void registerFormat(const Format& format);

    /// Unregisters the media format matching the
    /// given name.
    virtual bool unregisterFormat(std::string_view name);

    /// Sets the default fallback media format.
    virtual void setDefault(std::string_view name);

    virtual bool exists(std::string_view name);
    virtual void clear();

    virtual FormatList formats() const;

private:
    FormatRegistry(const FormatRegistry&) = delete;
    FormatRegistry& operator=(const FormatRegistry&) = delete;
    FormatRegistry(FormatRegistry&&) = delete;
    FormatRegistry& operator=(FormatRegistry&&) = delete;

    Format& findByName(std::string_view name);  // requires _mutex held
    Format& defaultLocked();                        // requires _mutex held

    FormatList _formats;
    std::string _default;
    mutable std::mutex _mutex;
};


} // namespace av
} // namespace icy


/// @\}
