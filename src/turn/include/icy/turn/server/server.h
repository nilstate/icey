///
///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{

#pragma once


#include "icy/net/tcpsocket.h"
#include "icy/net/udpsocket.h"
#include "icy/stun/message.h"
#include "icy/timer.h"
#include "icy/turn/server/serverallocation.h"
#include "icy/turn/server/tcpallocation.h"
#include "icy/turn/server/udpallocation.h"
#include "icy/turn/turn.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>


namespace icy {
namespace turn {


/// Default server allocation lifetime (2 minutes, in milliseconds)
static constexpr uint32_t kServerDefaultLifetime = 2 * 60 * 1000;

/// Maximum server allocation lifetime (15 minutes, in milliseconds)
static constexpr uint32_t kServerMaxLifetime = 15 * 60 * 1000;

/// Maximum number of permissions per allocation
static constexpr int kServerMaxPermissions = 10;

/// Server timer interval (10 seconds)
static constexpr int kServerTimerInterval = 10 * 1000;

/// Early media buffer size
static constexpr int kServerEarlyMediaBufferSize = 8192;


/// Configuration options for the TURN server.
struct ServerOptions
{
    std::string software;
    std::string realm;

    uint32_t allocationDefaultLifetime;
    uint32_t allocationMaxLifetime;
    int allocationMaxPermissions;
    int timerInterval;
    int earlyMediaBufferSize;

    net::Address listenAddr; ///< The TCP and UDP bind() address
    std::string externalIP;  ///< The external public facing IP address of the server

    bool enableTCP;
    bool enableUDP;

    ServerOptions()
    {
        software = "Icey STUN/TURN Server [rfc5766]";
        realm = "0state.com";
        listenAddr = net::Address("0.0.0.0", 3478);
        externalIP = "";
        allocationDefaultLifetime = kServerDefaultLifetime;
        allocationMaxLifetime = kServerMaxLifetime;
        allocationMaxPermissions = kServerMaxPermissions;
        timerInterval = kServerTimerInterval;
        earlyMediaBufferSize = kServerEarlyMediaBufferSize;
        enableTCP = true;
        enableUDP = true;
    }
};


/// The ServerObserver receives callbacks for and is responsible
/// for managing allocation and bandwidth quotas, authentication
/// methods and authentication.
struct ServerObserver
{
    virtual void onServerAllocationCreated(Server* server, IAllocation* alloc) = 0;
    virtual void onServerAllocationRemoved(Server* server, IAllocation* alloc) = 0;

    /// The observer class can implement authentication
    /// using the long-term credential mechanism of [RFC5389].
    /// The class design is such that authentication can be preformed
    /// asynchronously against a remote database, or locally.
    /// The default implementation returns true to all requests.
    ///
    /// To mitigate either intentional or unintentional denial-of-service
    /// attacks against the server by clients with valid usernames and
    /// passwords, it is RECOMMENDED that the server impose limits on both
    /// the number of allocations active at one time for a given username and
    /// on the amount of bandwidth those allocations can use.  The server
    /// should reject new allocations that would exceed the limit on the
    /// allowed number of allocations active at one time with a 486
    /// (Allocation Quota Exceeded) (see Section 6.2), and should discard
    /// application data traffic that exceeds the bandwidth quota.
    virtual AuthenticationState authenticateRequest(Server* server, Request& request) = 0;
};


using ServerAllocationMap = std::map<FiveTuple, std::unique_ptr<ServerAllocation>>;


/// TURN server rfc5766 implementation
class TURN_API Server
{
public:
    Server(ServerObserver& observer, const ServerOptions& options = ServerOptions());
    virtual ~Server();

    virtual void start();
    virtual void stop();

    void handleRequest(Request& request, AuthenticationState state);
    void handleAuthorizedRequest(Request& request);
    void handleBindingRequest(Request& request);
    void handleAllocateRequest(Request& request);
    void handleConnectionBindRequest(Request& request);

    void respond(Request& request, stun::Message& response);
    void respondError(Request& request, int errorCode, const char* errorDesc);

    /// Returns a snapshot copy of the allocation map for safe iteration.
    /// Callers receive raw pointers that are valid only while the
    /// corresponding unique_ptr in _allocations is alive.
    std::map<FiveTuple, ServerAllocation*> allocations() const;
    void addAllocation(std::unique_ptr<ServerAllocation> alloc);
    void removeAllocation(ServerAllocation* alloc);
    [[nodiscard]] ServerAllocation* getAllocation(const FiveTuple& tuple);
    [[nodiscard]] TCPAllocation* getTCPAllocation(const uint32_t& connectionID);
    [[nodiscard]] net::TCPSocket::Ptr getTCPSocket(const net::Address& remoteAddr);
    void releaseTCPSocket(const net::Socket& socket);

    ServerObserver& observer();
    ServerOptions& options();
    net::UDPSocket& udpSocket();
    net::TCPSocket& tcpSocket();
    Timer& timer();

    void onTCPAcceptConnection(const net::TCPSocket::Ptr& sock);
    bool onTCPSocketClosed(net::Socket& socket);
    bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer,
                      const net::Address& peerAddress);
    void onTimer();

private:
    mutable std::mutex _mutex;
    ServerObserver& _observer;
    ServerOptions _options;
    net::SocketEmitter _udpSocket; // net::UDPSocket
    net::SocketEmitter _tcpSocket; // net::TCPSocket
    std::vector<net::SocketEmitter> _tcpSockets;
    ServerAllocationMap _allocations;
    Timer _timer;
};


} // namespace turn
} // namespace icy


/// @\}
