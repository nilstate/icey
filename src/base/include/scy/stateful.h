///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/signal.h"

#include <iostream>
#include <string>


namespace scy {


/// State class for state machines.
///
/// This class defined the state for a state machine, and should be
/// extended and passed to implementations of the `Stateful` template.
///
/// For an example @see PacketStreamState
class Base_API State
{
public:
    using ID = uint32_t;

    State(ID id = 0);
    State(const State& that);
    State& operator=(const State& that);
    virtual ~State() = default;

    ID id() const;
    void set(ID id);

    virtual std::string str(ID id) const;
    virtual std::string toString() const;

    bool equals(ID id) const;
    bool between(ID lid, ID rid) const;

    bool operator==(const State& that) { return equals(that.id()); }
    bool operator==(const State::ID& that) { return equals(that); }

    friend std::ostream& operator<<(std::ostream& os, const State& state)
    {
        os << state.toString();
        return os;
    }

protected:
    std::atomic<ID> _id;
};


/// State machine implementation.
///
/// For an example @see PacketStream
///
/// @tparam The derived `State` type.
template <typename T>
class Stateful
{
public:
    Stateful() {}

    virtual ~Stateful() {}

    virtual bool stateEquals(typename T::ID id) const
    {
        return _state.id() == id;
    }

    virtual bool stateBetween(typename T::ID lid, typename T::ID rid) const
    {
        return _state.id() >= lid && _state.id() <= rid;
    }

    virtual T& state() { return _state; }
    virtual const T state() const { return _state; }

    /// Signals when the state changes.
    Signal<void(void*, T&, const T&)> StateChange;

protected:
    /// Override to handle pre state change logic.
    /// Return false to prevent state change.
    virtual bool beforeStateChange(const T& state)
    {
        if (_state == state)
            return false; // return false by default if unchanged
        return true;
    }

    /// Override to handle post state change logic.
    virtual void onStateChange(T& /*state*/, const T& /*oldState*/) {}

    /// Sets the state and sends the state signal if
    /// the state change was successful.
    virtual bool setState(void* sender, typename T::ID id)
    {
        T state;
        state.set(id);
        return setState(sender, state);
    }

    /// Sets the state and sends the state signal if
    /// the state change was successful.
    virtual bool setState(void* sender, const T& state)
    {
        if (beforeStateChange(state)) {
            T oldState = _state;
            _state = state;
            onStateChange(_state, oldState);
            if (sender)
                StateChange.emit(sender, _state, oldState);
            return true;
        }
        return false;
    }

protected:
    T _state;
};


} // namespace scy


/// @\}
