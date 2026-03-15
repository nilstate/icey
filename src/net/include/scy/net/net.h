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


#include "scy/base.h"


//
/// Windows

#ifdef SCY_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
using socklen_t = int;
// typedef SOCKET scy_socket_t;
#else
#include <netinet/in.h>
#include <sys/socket.h>
// typedef int scy_socket_t;
// #define INVALID_SOCKET -1
#endif

#define SCY_HAVE_IPv6 1


//
/// Linux

#ifdef SCY_LINUX
#include <linux/version.h>

// Linux Kernel 3.9 added the SO_REUSEPORT socket flag, which supports
// multithread socket load balancing at the kernel level. This option allows
// two (or more) sockets, TCP or UDP, listening (server) or non-listening (client),
// to be bound to exactly the same address and port combination as long as all
// sockets (including the very first one) had this flag set prior to binding them.
// To prevent "port hijacking", there is one special limitation, though:
// All sockets that want to share the same address and port combination must
// belong to processes that share the same effective user ID! So one user cannot
// "steal" ports of another user. Additionally the kernel performs some
// "special magic" for SO_REUSEPORT sockets that isn't found in any other
// operating system so far: For UDP sockets, it tries to distribute datagrams
// evenly, for TCP listening sockets, it tries to distribute incoming connect
// requests (those accepted by calling accept()) evenly across all the sockets
// that share the same address and port combination. That means while it is more
// or less random which socket receives a datagram or connect request in other
// operating systems that allow full address reuse, Linux tries to optimize
// distribution so that, for example, multiple instances of a simple server
// process can easily use SO_REUSEPORT sockets to achieve a kind of simple
// load balancing and that absolutely for free as the kernel is doing
// "all the hard work" for them.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
#define SCY_HAS_KERNEL_SOCKET_LOAD_BALANCING 1
#endif
#endif


// Shared library exports
#if defined(SCY_WIN) && defined(SCY_SHARED_LIBRARY)
#if defined(Net_EXPORTS)
#define Net_API __declspec(dllexport)
#else
#define Net_API __declspec(dllimport)
#endif
#else
#define Net_API // nothing
#endif


namespace scy {
namespace net {


constexpr int MAX_TCP_PACKET_SIZE = 64 * 1024;
constexpr int MAX_UDP_PACKET_SIZE = 1500;


enum TransportType
{
    UDP,
    TCP,
    SSLTCP
};


enum SocketMode
{
    ServerSide, ///< Server-side adapter.
    ClientSide  ///< Client-side adapter.
};


} // namespace net
} // namespace scy
