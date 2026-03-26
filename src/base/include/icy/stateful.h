///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/signal.h"

#include <iostream>
#include <string>


namespace icy {


/// State class for state machines.
///
/// This class defines the state for a state machine, and should be
/// extended and passed to implementations of the `Stateful` template.
///
/// For an example @see PacketStreamState
class Base_API State
{
public:
    using ID = uint32_t;

    /// @param id Initial state ID. Defaults to 0.
    State(ID id = 0);

    /// Copy constructor.
    /// @param that State to copy from.
    State(const State& that);

    /// Copy assignment.
    /// @param that State to assign from.
    State& operator=(const State& that);
    virtual ~State() = default;

    /// Returns the current state ID.
    /// @return Atomic state ID value.
    ID id() const;

    /// Sets the state ID.
    /// @param id New state ID to assign.
    void set(ID id);

    /// Returns a human-readable string for the given state ID.
    /// Override in derived classes to provide meaningful names.
    /// @param id State ID to convert.
    /// @return String representation of the state, or "undefined" by default.
    virtual std::string str(ID id) const;

    /// Returns a human-readable string for the current state ID.
    /// @return Result of `str(id())`.
    virtual std::string toString() const;

    /// Returns true if the current state ID equals the given ID.
    /// @param id State ID to compare against.
    /// @return True if IDs match.
    bool equals(ID id) const;

    /// Returns true if the current state ID is in the inclusive range [lid, rid].
    /// @param lid Lower bound state ID.
    /// @param rid Upper bound state ID.
    /// @return True if lid <= id() <= rid.
    bool between(ID lid, ID rid) const;

    bool operator==(const State& that) const { return equals(that.id()); }
    bool operator==(const State::ID& that) const { return equals(that); }

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

    /// Returns true if the current state ID equals the given ID.
    /// @param id State ID to compare against.
    /// @return True if the current state matches.
    virtual bool stateEquals(typename T::ID id) const
    {
        return _state.id() == id;
    }

    /// Returns true if the current state ID is in the inclusive range [lid, rid].
    /// @param lid Lower bound state ID.
    /// @param rid Upper bound state ID.
    /// @return True if lid <= state.id() <= rid.
    virtual bool stateBetween(typename T::ID lid, typename T::ID rid) const
    {
        return _state.id() >= lid && _state.id() <= rid;
    }

    /// Returns a mutable reference to the current state.
    /// @return Reference to the internal state object.
    virtual T& state() { return _state; }

    /// Returns a copy of the current state.
    /// @return Current state value.
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


} // namespace icy


/// @}
