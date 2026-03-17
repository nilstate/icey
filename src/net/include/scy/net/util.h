///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "scy/handle.h"
#include "scy/net/address.h"
#include "scy/net/net.h"

#include <vector>


namespace scy {
namespace net {


//
// Network Interfaces
//


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


#if defined(SCY_WIN)
#define nativeSocketFd(handle) ((handle)->socket)
#else
// uv__stream_fd taken from libuv unix/internal.h
#if defined(SCY_APPLE)
int uv___stream_fd(const uv_stream_t* handle);
#define uv__stream_fd(handle) (uv___stream_fd((const uv_stream_t*)(handle)))
#else
#define uv__stream_fd(handle) ((handle)->io_watcher.fd)
#endif
#define nativeSocketFd(handle) (uv__stream_fd(handle))
#endif


template <typename T>
int getServerSocketSendBufSize(uv::Handle<T>& handle)
{
    int val = 0;
    return uv_send_buffer_size(handle.template get<uv_handle_t>(), &val);
}


template <typename T>
int getServerSocketRecvBufSize(uv::Handle<T>& handle)
{
    int val = 0;
    return uv_recv_buffer_size(handle.template get<uv_handle_t>(), &val);
}


template <typename T>
int setServerSocketSendBufSize(uv::Handle<T>& handle, int size)
{
    return uv_send_buffer_size(handle.template get<uv_handle_t>(), &size);
}


template <typename T>
int setServerSocketRecvBufSize(uv::Handle<T>& handle, int size)
{
    return uv_recv_buffer_size(handle.template get<uv_handle_t>(), &size);
}


} // namespace net
} // namespace scy


/// @\}
