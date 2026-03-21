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


#include "icy/symple/message.h"
#include "icy/symple/symple.h"
#include <string_view>


namespace icy {
namespace smpl {


/// Symple command message with a node path and action verb.
///
/// The `node` field is a colon-delimited path (e.g. "camera:zoom:in").
/// Individual path segments can be retrieved with param().
class Symple_API Command : public Message
{
public:
    /// Constructs an empty command with type set to "command".
    Command();

    /// Constructs a command from a JSON value.
    /// @param root JSON object to initialise from.
    Command(const json::Value& root);

    /// Copy constructor.
    /// @param root Source command.
    Command(const Command& root);

    virtual ~Command();

    /// Returns the colon-delimited node path (e.g. "camera:zoom").
    [[nodiscard]] std::string node() const;

    /// Returns the action verb (defaults to "execute").
    [[nodiscard]] std::string action() const;

    /// Sets the node path field.
    /// @param node Colon-delimited node path string.
    void setNode(std::string_view node);

    /// Sets the action verb field.
    /// @param action Action string (e.g. "execute", "get", "set").
    void setAction(std::string_view action);

    /// Returns true if the base message is valid and the `node` field is set.
    virtual bool valid() const;

    /// Returns the nth colon-separated segment of the node path (1-based).
    /// Throws std::out_of_range if n exceeds the number of segments.
    /// @param n 1-based segment index.
    /// @return The nth path segment.
    [[nodiscard]] std::string param(int n) const;

    /// Returns all colon-separated segments of the node path.
    /// @return Vector of path segment strings.
    std::vector<std::string> params();

    /// Returns true if the node path matches the given pattern.
    /// Uses wildcard node matching via util::matchNodes.
    /// @param xnode Pattern to match against (colon-delimited, supports wildcards).
    [[nodiscard]] bool matches(std::string_view xnode) const;
};


} // namespace smpl
} // namespace icy


/// @\}
