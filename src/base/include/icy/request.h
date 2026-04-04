///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup uv
/// @{


#pragma once


#include "icy/base.h"
#include "icy/loop.h"
#include "icy/util.h"

#include "uv.h"

#include <functional>


namespace icy {
namespace uv {


template <typename T>
class Handle;


/// Default request callback event carrying a libuv status code.
struct BasicEvent
{
    int status; ///< libuv result: 0 on success, negative on error.
};


/// Wrapper class for managing `uv_req_t` variants.
///
/// This class provides safe access to the parent handle in case the handle gets
/// destroyed before the request callback returns, and should be used whenever
/// the handle pointer is accessed via the callback.
template <typename T, typename E = BasicEvent>
struct Request
{
    using Type = T;
    using Event = E;

    T req;                                ///< The underlying libuv request object.
    std::function<void(const E&)> callback; ///< Called when the request completes.

    /// Construct the request and set `req.data` to `this` so callbacks can
    /// recover the wrapper pointer.
    Request()
    {
        req.data = this;
    }

    /// Standard libuv completion callback. Invokes `callback` with the status
    /// event and then deletes the `Request` wrapper.
    ///
    /// @param req     The completed libuv request.
    /// @param status  libuv status code (0 on success, negative on error).
    static void defaultCallback(T* req, int status)
    {
        auto wrap = static_cast<Request*>(req->data);
        if (wrap->callback)
            wrap->callback(E{status});
        delete wrap;
    }

    /// Call @p f with @p args. If @p f returns a non-zero libuv error code,
    /// the `callback` is invoked immediately with that status.
    ///
    /// Enabled only when @p f returns a non-void type (i.e. an error code).
    ///
    /// @param f     libuv function to call.
    /// @param args  Arguments forwarded to @p f.
    /// @return      `true` (non-zero = success) if @p f returned 0; `false` on error.
    template <typename F, typename... Args>
    auto invoke(F&& f, Args&&... args)
        -> std::enable_if_t<!std::is_void<std::invoke_result_t<F, Args...>>::value, int>
    {
        auto err = std::forward<F>(f)(std::forward<Args>(args)...);
        if (err && callback)
            callback(E{err});
        return !err;
    }

    /// Call @p f with @p args. Overload for void-returning functions;
    /// no error checking is performed.
    ///
    /// @param f     Function to call.
    /// @param args  Arguments forwarded to @p f.
    template <typename F, typename... Args>
    auto invoke(F&& f, Args&&... args)
        -> std::enable_if_t<std::is_void<std::invoke_result_t<F, Args...>>::value>
    {
        std::forward<F>(f)(std::forward<Args>(args)...);
    }
};


/// Allocate a heap-owned `Request` of type @p T and attach @p callback to it.
///
/// The returned reference is valid until the request's `defaultCallback` fires
/// and deletes the object.
///
/// @tparam T        A specialization of `Request`.
/// @param callback  Completion handler; receives a `T::Event` on completion.
/// @return          Reference to the newly allocated request.
template <typename T>
inline T& createRequest(std::function<void(const typename T::Event&)> callback)
{
    auto req = new T();
    req->callback = callback;
    return *req;
}


/// Allocate a heap-owned `Request` of type @p T whose callback retains
/// additional state until completion.
///
/// This is the standard way to bind request completion to handle lifetime or
/// other retained context without hand-rolling per-call capture logic.
///
/// @tparam T         A specialization of `Request`.
/// @tparam Retained  Retained object type copied or moved into the callback.
/// @tparam Callback  Callable invoked as `callback(retained, event)`.
/// @param retained   Extra state to keep alive until the request completes.
/// @param callback   Completion handler receiving the retained state and event.
/// @return           Reference to the newly allocated request.
template <typename T, typename Retained, typename Callback>
inline T& createRetainedRequest(Retained&& retained, Callback&& callback)
{
    using RetainedType = std::decay_t<Retained>;
    using CallbackType = std::decay_t<Callback>;
    return createRequest<T>(
        [retained = RetainedType(std::forward<Retained>(retained)),
         callback = CallbackType(std::forward<Callback>(callback))](
            const typename T::Event& event) mutable {
            callback(retained, event);
        });
}


/// Asynchronous connection request for TCP sockets and named pipes.
struct ConnectReq : public uv::Request<uv_connect_t>
{
    /// Construct and set `req.data` to `this`.
    ConnectReq()
    {
        req.data = this;
    }

    /// Initiate a TCP connection to @p addr on @p handle.
    ///
    /// @param handle  Initialized `uv_tcp_t` to connect.
    /// @param addr    Target address (IPv4 or IPv6 `sockaddr`).
    /// @return        `true` if the connect request was submitted successfully.
    auto connect(uv_tcp_t* handle, const struct sockaddr* addr)
    {
        return invoke(&uv_tcp_connect, &req, handle, addr, &defaultCallback);
    }

    /// Initiate a named-pipe connection to @p name on @p handle.
    ///
    /// @param handle  Initialized `uv_pipe_t` to connect.
    /// @param name    Filesystem path (Unix) or named-pipe name (Windows).
    /// @return        `true` if the connect request was submitted successfully.
    auto connect(uv_pipe_t* handle, const char* name)
    {
        return invoke(&uv_pipe_connect, &req, handle, name, &defaultCallback);
    }
};


/// Callback event delivered when a `GetAddrInfoReq` resolves.
struct GetAddrInfoEvent
{
    int status;              ///< libuv status: 0 on success, negative on error.
    struct addrinfo* addr = nullptr; ///< Resolved address list; freed after the callback returns.
};


/// DNS resolver request to get the IP address of a hostname.
struct GetAddrInfoReq : public uv::Request<uv_getaddrinfo_t, GetAddrInfoEvent>
{
    using Request = uv::Request<uv_getaddrinfo_t, GetAddrInfoEvent>;

    /// Construct and set `req.data` to `this`.
    GetAddrInfoReq()
    {
        req.data = this;
    }

    /// libuv completion callback for `uv_getaddrinfo`.
    ///
    /// Invokes the stored callback with the resolved address list, then frees
    /// the `addrinfo` chain and deletes the wrapper.
    ///
    /// @param req     The completed `uv_getaddrinfo_t` request.
    /// @param status  libuv status code.
    /// @param res     Resolved address list (freed after callback returns).
    static void getAddrInfoCallback(Request::Type* req, int status, struct addrinfo* res)
    {
        auto wrap = static_cast<GetAddrInfoReq*>(req->data);
        if (wrap->callback)
            wrap->callback(GetAddrInfoEvent{status, res});
        uv_freeaddrinfo(res);
        delete wrap;
    }

    /// Begin asynchronous DNS resolution of @p host at @p port.
    ///
    /// The result is delivered to `callback` as a `GetAddrInfoEvent`. The
    /// `addrinfo` pointer in the event is freed immediately after the callback
    /// returns; do not retain it.
    ///
    /// @param host  Hostname or numeric IP address string to resolve.
    /// @param port  Port number; converted to a service string for `getaddrinfo`.
    /// @param loop  Event loop on which to run the resolution.
    /// @return      `true` if the request was submitted successfully.
    auto resolve(const std::string& host, int port, uv::Loop* loop = uv::defaultLoop())
    {
        return invoke(&uv_getaddrinfo, loop, &req, &getAddrInfoCallback,
                      host.c_str(), util::itostr(port).c_str(), nullptr);
    }
};


} // namespace uv
} // namespace icy


/// @}
