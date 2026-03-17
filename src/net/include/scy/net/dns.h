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


#include "scy/logger.h"
#include "scy/net/address.h"
#include "scy/net/net.h"
#include "scy/request.h"
#include "scy/util.h"


namespace scy {
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
} // namespace scy


/// @\}
