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
#include "icy/interface.h"
#include "icy/memory.h"
#include "icy/packet.h"
#include "icy/stateful.h"
#include "icy/timer.h"


namespace icy {


/// State machine states for PacketTransaction
struct TransactionState : public State
{
    enum Type
    {
        Waiting = 0,
        Running,
        Success,
        Cancelled,
        Failed
    };

    /// @param id A `TransactionState::Type` value.
    /// @return Human-readable name of the state.
    std::string str(unsigned int id) const
    {
        switch (id) {
            case Waiting:
                return "Waiting";
            case Running:
                return "Running";
            case Success:
                return "Success";
            case Cancelled:
                return "Cancelled";
            case Failed:
                return "Failed";
        }
        return "undefined";
    };
};


/// Request/response transaction with timeout and retry logic.
///
/// Lifetime is managed by IntrusivePtr via the RefCounted base class.
/// The transaction is kept alive as long as at least one IntrusivePtr
/// references it. The owning client holds a Ptr in its transaction list;
/// callbacks should capture a Ptr copy to prevent premature deletion.
///
/// When a terminal state (Success or Failed) is reached, the transaction
/// cleans up its timer but does NOT delete itself. The IntrusivePtr
/// destructor handles deletion when the last reference is released.
template <class PacketT>
class PacketTransaction : public basic::Sendable
    , public Stateful<TransactionState>
    , public RefCounted<PacketTransaction<PacketT>>
{
public:
    using Ptr = IntrusivePtr<PacketTransaction<PacketT>>;

    /// @param timeout Transaction timeout in milliseconds before failure or retry.
    /// @param retries Maximum number of retransmissions (0 means one attempt only).
    /// @param loop Event loop for the timeout timer.
    PacketTransaction(long timeout = 10000, int retries = 0,
                      uv::Loop* loop = uv::defaultLoop())
        : _timer(timeout, loop)
        , _retries(retries)
        , _attempts(0)
    {
    }

    /// @param request Initial request packet to store and send.
    /// @param timeout Transaction timeout in milliseconds before failure or retry.
    /// @param retries Maximum number of retransmissions (0 means one attempt only).
    /// @param loop Event loop for the timeout timer.
    PacketTransaction(const PacketT& request, long timeout = 10000,
                      int retries = 0, uv::Loop* loop = uv::defaultLoop())
        : _request(request)
        , _timer(timeout, loop)
        , _retries(retries)
        , _attempts(0)
    {
    }

    /// Starts the transaction timer and sends the request.
    /// Overriding classes should implement send logic here.
    virtual bool send()
    {
        if (!canResend())
            return false;

        _attempts++;
        if (_timer.active())
            _timer.stop();
        _timer.Timeout += slot(this, &PacketTransaction::onTimeout);
        _timer.start();

        return setState(this, TransactionState::Running);
    }

    /// Cancellation means that the agent will not retransmit
    /// the request, will not treat the lack of response to be
    /// a failure, but will wait the duration of the transaction
    /// timeout for a response.
    /// Transitions the transaction to the `Cancelled` state.
    void cancel();

    /// @return True if the transaction is in the `Cancelled` state.
    bool cancelled() const;

    /// Stops the timer and unregisters callbacks. Does NOT delete the object;
    /// the IntrusivePtr destructor handles deletion when the last reference
    /// is released. Safe to call multiple times.
    virtual void dispose()
    {
        LTrace("Dispose");
        if (!_disposed) {
            _disposed = true;
            _timer.Timeout -= slot(this, &PacketTransaction::onTimeout);
            _timer.stop();
        }
    }

    /// @return True if dispose() has been called.
    [[nodiscard]] bool disposed() const { return _disposed; }

    /// @return True if the transaction is not cancelled and has attempts remaining.
    virtual bool canResend();

    /// @return The number of times `send()` has been called for this transaction.
    int attempts() const;

    /// @return The configured maximum number of retransmissions.
    int retries() const;

    /// @return Mutable reference to the outgoing request packet.
    PacketT& request();

    /// @return Copy of the outgoing request packet.
    PacketT request() const;

    /// @return Mutable reference to the received response packet.
    PacketT& response();

    /// @return Copy of the received response packet.
    PacketT response() const;

protected:
    template <typename U> friend class IntrusivePtr;

    virtual ~PacketTransaction()
    {
        dispose();
    }

    /// Post state change hook. Calls dispose() on terminal states to stop the
    /// timer, but does not delete the object; IntrusivePtr handles that.
    virtual void onStateChange(TransactionState& state, const TransactionState&) override
    {
        LTrace("On state change:", state.toString());

        if (state.equals(TransactionState::Success) ||
            state.equals(TransactionState::Failed))
            dispose();
    }

    /// Processes a potential response candidate
    /// and updates the state accordingly.
    virtual bool handlePotentialResponse(const PacketT& packet)
    {
        if (stateEquals(TransactionState::Running) && checkResponse(packet)) {
            _response = packet;
            onResponse();
            setState(this, TransactionState::Success);
            return true;
        }
        return false;
    }

    /// Checks a potential response candidate and
    /// returns true on successful match.
    virtual bool checkResponse(const PacketT& packet) = 0;

    /// Called when a successful response is received.
    virtual void onResponse()
    {
        LTrace("Success:", _response.toString());
    }

    /// Called by the timer when the transaction timeout elapses.
    /// Retransmits if retries remain, otherwise transitions to `Failed`.
    virtual void onTimeout()
    {
        LDebug("Timeout");

        if (!canResend()) {
            setState(this, TransactionState::Failed); // Timeout
        } else
            send();
    }

protected:
    PacketT _request;
    PacketT _response;
    Timer _timer;  ///< The request timeout callback.
    int _retries;  ///< The maximum number of attempts before the transaction is considered failed.
    int _attempts; ///< The number of times the transaction has been sent.
    bool _disposed = false;
};


template <class T>
inline void PacketTransaction<T>::cancel()
{
    setState(this, TransactionState::Cancelled);
}

template <class T>
inline bool PacketTransaction<T>::cancelled() const
{
    return stateEquals(TransactionState::Cancelled);
}

template <class T>
inline bool PacketTransaction<T>::canResend()
{
    return !cancelled() && attempts() <= retries();
}

template <class T>
inline int PacketTransaction<T>::attempts() const
{
    return _attempts;
}

template <class T>
inline int PacketTransaction<T>::retries() const
{
    return _retries;
}

template <class T>
inline T& PacketTransaction<T>::request()
{
    return _request;
}

template <class T>
inline T PacketTransaction<T>::request() const
{
    return _request;
}

template <class T>
inline T& PacketTransaction<T>::response()
{
    return _response;
}

template <class T>
inline T PacketTransaction<T>::response() const
{
    return _response;
}


} // namespace icy


/// @}
