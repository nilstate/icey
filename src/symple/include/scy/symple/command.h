///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/symple/message.h"
#include "scy/symple/symple.h"


namespace scy {
namespace smpl {


class Symple_API Command : public Message
{
public:
    Command();
    Command(const json::value& root);
    Command(const Command& root);
    virtual ~Command();

    [[nodiscard]] std::string node() const;
    [[nodiscard]] std::string action() const;

    void setNode(const std::string& node);
    void setAction(const std::string& action);

    bool valid() const override;

    [[nodiscard]] std::string param(int n) const;
    std::vector<std::string> params();
    [[nodiscard]] bool matches(const std::string& xnode) const;
};


} // namespace smpl
} // namespace scy


/// @\}
