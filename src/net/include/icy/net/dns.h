///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "icy/logger.h"
#include "icy/net/address.h"
#include "icy/net/net.h"
#include "icy/request.h"
#include "icy/util.h"


namespace icy {
namespace net {

/// DNS utilities.
namespace dns {


inline auto resolve(const std::string& host, int port,
                    std::function<void(int, const net::Address&)> callback,
                    uv::Loop* loop = uv::defaultLoop())
{
    return uv::createRequest<uv::GetAddrInfoReq>([=](const uv::GetAddrInfoEvent& event) {
               if (event.status) {
                   LWarn("Cannot resolve DNS for ", host, ": ", uv_strerror(event.status));
                   callback(event.status, net::Address{});
               } else
                   callback(event.status, net::Address{event.addr->ai_addr, static_cast<socklen_t>(event.addr->ai_addrlen)});
           })
        .resolve(host, port, loop);
}


} // namespace dns
} // namespace net
} // namespace icy


/// @\}
