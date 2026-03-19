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


class Symple_API Command : public Message
{
public:
    Command();
    Command(const json::Value& root);
    Command(const Command& root);
    virtual ~Command();

    [[nodiscard]] std::string node() const;
    [[nodiscard]] std::string action() const;

    void setNode(std::string_view node);
    void setAction(std::string_view action);

    virtual bool valid() const;

    [[nodiscard]] std::string param(int n) const;
    std::vector<std::string> params();
    [[nodiscard]] bool matches(std::string_view xnode) const;
};


} // namespace smpl
} // namespace icy


/// @\}
