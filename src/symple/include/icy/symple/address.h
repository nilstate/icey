///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/symple/symple.h"
#include <cstdint>
#include <string>
#include <string_view>


namespace icy {
namespace smpl {


/// The Address structure is an endpoint identifier for a
/// peer on the network.
/// The format is like so: user|id
struct Symple_API Address
{
public:
    /// Constructs an empty (invalid) address.
    Address();

    /// Parses an address string of the form `user|id`.
    /// @param addr Address string to parse.
    Address(std::string_view addr);

    /// Constructs an address from explicit user and session ID components.
    /// @param user User identifier.
    /// @param id   Session ID.
    Address(const std::string& user,
            const std::string& id);
    // const std::string& group,
    virtual ~Address();

    /// Parses an address string of the form `user|id`.
    /// Populates the `user` and `id` fields.
    /// @param addr Address string to parse.
    /// @return True if the result is a valid address.
    bool parse(std::string_view addr);

    /// Returns true if at least one of `user` or `id` is non-empty.
    [[nodiscard]] bool valid() const;

    /// Writes the address in `user|id` format to the given stream.
    /// @param os Output stream.
    void print(std::ostream& os) const;

    /// Returns the address as a string in `user|id` format.
    /// @return Serialised address string.
    [[nodiscard]] std::string toString() const;

    /// Compares two addresses for equality (both user and id must match).
    /// @param r Address to compare against.
    bool operator==(const Address& r) const;

    /// Compares this address against a string in `user|id` format without allocating.
    /// @param r String to compare against.
    bool operator==(const std::string& r) const;

    friend std::ostream& operator<<(std::ostream& os, const Address& addr)
    {
        addr.print(os);
        return os;
    }

    std::string user;
    // std::string group;
    std::string id;
};


} // namespace smpl
} // namespace icy


/// @\}
