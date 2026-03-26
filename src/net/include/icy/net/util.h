///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "icy/handle.h"
#include "icy/net/address.h"
#include "icy/net/net.h"

#include <vector>


namespace icy {
namespace net {


//
// Network Interfaces
//


/// Populates @p hosts with all local network interface addresses.
///
/// Each entry is an IPv4 address constructed from the interface's address4 field.
/// The results include loopback and any other active interfaces reported by libuv.
/// @param hosts Vector to append the discovered addresses to.
inline void getNetworkInterfaces(std::vector<net::Address>& hosts)
{
    uv_interface_address_t* info;
    int count, i;

    uv_interface_addresses(&info, &count);
    i = count;

    while (i--) {
        uv_interface_address_t iface = info[i];
        hosts.push_back(net::Address(
            reinterpret_cast<const sockaddr*>(&iface.address.address4), 16));
    }

    uv_free_interface_addresses(info, count);
}


//
// Socket Helpers
//


#if defined(ICY_WIN)
#define nativeSocketFd(handle) ((handle)->socket)
#else
// uv__stream_fd taken from libuv unix/internal.h
#if defined(ICY_APPLE)
int uv___stream_fd(const uv_stream_t* handle);
#define uv__stream_fd(handle) (uv___stream_fd((const uv_stream_t*)(handle)))
#else
#define uv__stream_fd(handle) ((handle)->io_watcher.fd)
#endif
#define nativeSocketFd(handle) (uv__stream_fd(handle))
#endif


/// Returns the current send buffer size for a socket handle, in bytes.
/// Passes val=0 to uv_send_buffer_size, which queries rather than sets the value.
/// @tparam T  The libuv handle type (e.g. uv_tcp_t, uv_udp_t).
/// @param handle The socket handle to query.
/// @return The send buffer size, or a libuv error code on failure.
template <typename T>
int getServerSocketSendBufSize(uv::Handle<T>& handle)
{
    int val = 0;
    return uv_send_buffer_size(handle.template get<uv_handle_t>(), &val);
}


/// Returns the current receive buffer size for a socket handle, in bytes.
/// Passes val=0 to uv_recv_buffer_size, which queries rather than sets the value.
/// @tparam T  The libuv handle type (e.g. uv_tcp_t, uv_udp_t).
/// @param handle The socket handle to query.
/// @return The receive buffer size, or a libuv error code on failure.
template <typename T>
int getServerSocketRecvBufSize(uv::Handle<T>& handle)
{
    int val = 0;
    return uv_recv_buffer_size(handle.template get<uv_handle_t>(), &val);
}


/// Sets the send buffer size for a socket handle.
/// @tparam T    The libuv handle type (e.g. uv_tcp_t, uv_udp_t).
/// @param handle The socket handle to configure.
/// @param size   The desired send buffer size in bytes.
/// @return 0 on success, or a libuv error code on failure.
template <typename T>
int setServerSocketSendBufSize(uv::Handle<T>& handle, int size)
{
    return uv_send_buffer_size(handle.template get<uv_handle_t>(), &size);
}


/// Sets the receive buffer size for a socket handle.
/// @tparam T    The libuv handle type (e.g. uv_tcp_t, uv_udp_t).
/// @param handle The socket handle to configure.
/// @param size   The desired receive buffer size in bytes.
/// @return 0 on success, or a libuv error code on failure.
template <typename T>
int setServerSocketRecvBufSize(uv::Handle<T>& handle, int size)
{
    return uv_recv_buffer_size(handle.template get<uv_handle_t>(), &size);
}


} // namespace net
} // namespace icy


/// @}
