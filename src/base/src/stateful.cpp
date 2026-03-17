///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/stateful.h"


using std::endl;


namespace scy {


State::State(State::ID id) //, const std::string& message
    : _id(id)
// , _message(message)
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


// std::string State::message() const
// {
//     return _message;
// }
//
//
// void State::setMessage(const std::string& message)
// {
//     _message = message;
// }


std::string State::str(State::ID id) const
{
    // Must be overridden...
    (void)id;
    return "undefined";
}


std::string State::toString() const
{
    return str(id());
}


//
// Mutex State
//


// MutexState::MutexState(State::ID id)
//     : State(id)
// {
// }


//
// State Signal
//


// StateSignal::StateSignal(State::ID id)
//     : MutexState(id)
// {
// }
//
//
// bool StateSignal::change(State::ID id)
// {
//     if (canChange(id)) {
//         unsigned int old = this->id();
//         MutexState::set(id);
//         onChange(id, old);
//         return true;
//     }
//     return false;
// }
//
//
// bool StateSignal::canChange(State::ID id)
// {
//     // Can be overridden
//     if (this->id() != id)
//         return true;
//     return false;
// }
//
//
// void StateSignal::onChange(ID id, ID prev)
// {
//     // Can be overridden
//     // Change.emit(/*this, */id, prev);
// }
//
//
// void StateSignal::set(State::ID id)
// {
//     MutexState::set(id);
// }


} // namespace scy


/// @\}
