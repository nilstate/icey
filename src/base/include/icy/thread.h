///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/runner.h"

#include <mutex>
#include <thread>
#include <tuple>
#include <utility>


namespace icy {


/// Platform-independent wrapper around an operating system thread.
///
/// This class inherits the `Runner` interface and may be used with any
/// implementation that's powered by an asynchronous `Runner`.
///
class Base_API Thread : public Runner
{
public:
    using Ptr = std::shared_ptr<Thread>;

    /// Default constructor.
    Thread();

    /// Constructs a `Thread` and immediately starts it with the given function and arguments.
    /// @tparam Function Callable type.
    /// @tparam Args Argument types forwarded to the function.
    /// @param func Callable to execute on the new thread.
    /// @param args Arguments forwarded to `func`.
    template <typename Function, typename... Args>
    explicit Thread(Function&& func, Args&&... args)
    {
        _thread = std::thread(internal::runAsync<Function, Args...>, _context,
                              std::forward<Function>(func),
                              std::forward<Args>(args)...);
    }

    /// Destructor.
    virtual ~Thread();

    /// Starts the thread with a variadic function and arguments.
    /// The thread is started immediately; the previous thread must have exited before calling again.
    /// @tparam Function Callable type.
    /// @tparam Args Argument types forwarded to the function.
    /// @param func Callable to execute on the new thread.
    /// @param args Arguments forwarded to `func`.
    template <typename Function, typename... Args>
    void start(Function&& func, Args&&... args)
    {
        _thread = std::thread(internal::runAsync<Function, Args...>, _context,
                              std::forward<Function>(func),
                              std::forward<Args>(args)...);
    }

    /// Starts the thread with a `std::function` callback.
    /// Overrides `Runner::start`; delegates to the variadic `start` template.
    /// @param func Callable to execute on the new thread.
    void start(std::function<void()> func) override;

    /// Wait until the thread exits.
    void join();

    /// Return the native thread handle.
    std::thread::id id() const;

    /// Return the native thread ID of the current thread.
    static std::thread::id currentID();

    /// Accessor for the main thread ID.
    static const std::thread::id mainID;

protected:
    /// NonCopyable and NonMovable
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread(Thread&&) = delete;
    Thread& operator=(Thread&&) = delete;

    bool async() const override;

    std::thread _thread;
};


} // namespace icy


/// @\}
