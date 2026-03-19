///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/stateful.h"


namespace icy {


State::State(State::ID id)
    : _id(id)
{
}


State::State(const State& that)
    : _id(that._id.load())
{
}


State& State::operator=(const State& that)
{
    _id = that._id.load();
    return *this;
}


unsigned int State::id() const
{
    return _id;
}


void State::set(State::ID id)
{
    _id = id;
}


bool State::equals(ID id) const
{
    return this->id() == id;
}


bool State::between(ID lid, ID rid) const
{
    ID id = this->id();
    return id >= lid && id <= rid;
}


std::string State::str(State::ID id) const
{
    (void)id;
    return "undefined";
}


std::string State::toString() const
{
    return str(id());
}


} // namespace icy


/// @\}
