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
    bool enableLocalIPPermissions; ///< Auto-grant permissions for RFC 1918/loopback addresses

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
        enableLocalIPPermissions = true;
    }
};


/// Observer interface that the application must implement to participate in
/// server-side allocation management and authentication.
///
/// The observer is responsible for enforcing per-user allocation quotas and
/// bandwidth limits. Implementations may perform authentication synchronously
/// (returning Authorized/NotAuthorized immediately) or asynchronously
/// (returning Authenticating and calling handleRequest() again later).
struct ServerObserver
{
    /// Called after a new allocation is successfully created.
    /// @param server The server that owns the allocation.
    /// @param alloc  The newly created allocation (lifetime managed by the server).
    virtual void onServerAllocationCreated(Server* server, IAllocation* alloc) = 0;

    /// Called just before an allocation is destroyed (expired, deleted, or server stopped).
    /// @param server The server that owned the allocation.
    /// @param alloc  The allocation being removed; do not delete this pointer.
    virtual void onServerAllocationRemoved(Server* server, IAllocation* alloc) = 0;

    /// Authenticates an incoming STUN request using the long-term credential mechanism
    /// (RFC 5389 section 10.2). Return Authorized to proceed, NotAuthorized to reject
    /// with a 401, QuotaReached to reject with a 486, or Authenticating to defer until
    /// the result is available asynchronously.
    /// @param server  The server receiving the request.
    /// @param request The STUN request to authenticate.
    /// @return An AuthenticationState indicating how to proceed.
    virtual AuthenticationState authenticateRequest(Server* server, Request& request) = 0;
};


using ServerAllocationMap = std::map<FiveTuple, std::unique_ptr<ServerAllocation>>;


/// TURN server RFC 5766 / RFC 6062 implementation.
/// Listens on UDP and/or TCP, authenticates requests via ServerObserver,
/// and manages ServerAllocation objects for each 5-tuple.
class TURN_API Server
{
public:
    /// @param observer Observer used for authentication and allocation lifecycle events.
    /// @param options  Server configuration; defaults to 0.0.0.0:3478 with TCP and UDP enabled.
    Server(ServerObserver& observer, const ServerOptions& options = ServerOptions());
    virtual ~Server();

    /// Binds and listens on the configured address, then starts the maintenance timer.
    virtual void start();

    /// Stops the timer, destroys all allocations, and closes server sockets.
    virtual void stop();

    /// Routes an authenticated request to the appropriate handler based on state.
    /// Pending (Authenticating) requests are held until the observer calls back.
    /// @param request Incoming STUN request.
    /// @param state   Result of the observer's authenticateRequest() call.
    void handleRequest(Request& request, AuthenticationState state);

    /// Dispatches an already-authorized request to the specific method handler.
    /// @param request Authorized STUN request.
    void handleAuthorizedRequest(Request& request);

    /// Handles a Binding request; responds with XOR-MAPPED-ADDRESS.
    /// @param request Incoming Binding request.
    void handleBindingRequest(Request& request);

    /// Handles an Allocate request; creates a UDP or TCP ServerAllocation and
    /// sends a success response with XOR-RELAYED-ADDRESS and LIFETIME.
    /// @param request Incoming Allocate request.
    void handleAllocateRequest(Request& request);

    /// Handles a ConnectionBind request by locating the TCPAllocation that owns
    /// the given CONNECTION-ID and delegating to it.
    /// @param request Incoming ConnectionBind request.
    void handleConnectionBindRequest(Request& request);

    /// Sends a STUN response, signing it with MessageIntegrity if the request had a hash.
    /// Routes via UDP or TCP depending on request.transport.
    /// @param request  The original request (provides transport and remote address).
    /// @param response The response message to send.
    void respond(Request& request, stun::Message& response);

    /// Constructs and sends an error response with SOFTWARE, REALM, NONCE, and ERROR-CODE.
    /// @param request    The original request.
    /// @param errorCode  STUN error code (e.g. 400, 401, 437).
    /// @param errorDesc  Human-readable error description string.
    void respondError(Request& request, int errorCode, const char* errorDesc);

    /// Returns a snapshot copy of the allocation map for safe iteration.
    /// Returned raw pointers are valid only while the server holds the allocations.
    /// @return Map from FiveTuple to raw ServerAllocation pointers.
    std::map<FiveTuple, ServerAllocation*> allocations() const;

    /// Transfers ownership of @p alloc to the server and notifies the observer.
    /// @param alloc Newly constructed allocation to register.
    void addAllocation(std::unique_ptr<ServerAllocation> alloc);

    /// Removes @p alloc from the map and notifies the observer.
    /// Called automatically from the ServerAllocation destructor.
    /// @param alloc Allocation being destroyed.
    void removeAllocation(ServerAllocation* alloc);

    /// Looks up an allocation by its 5-tuple.
    /// @param tuple The 5-tuple to search for.
    /// @return Pointer to the matching allocation, or nullptr if not found.
    [[nodiscard]] ServerAllocation* getAllocation(const FiveTuple& tuple);

    /// Finds the TCPAllocation that owns a TCPConnectionPair with the given connection ID.
    /// @param connectionID TURN CONNECTION-ID to search for.
    /// @return Pointer to the owning TCPAllocation, or nullptr if not found.
    [[nodiscard]] TCPAllocation* getTCPAllocation(const uint32_t& connectionID);

    /// Returns the accepted TCP socket whose peer address matches @p remoteAddr.
    /// @param remoteAddr Peer address to search for.
    /// @return Shared pointer to the socket, or empty if not found.
    [[nodiscard]] net::TCPSocket::Ptr getTCPSocket(const net::Address& remoteAddr);

    /// Removes a TCP control socket from the server's socket list and unregisters callbacks.
    /// Called when the socket is handed off to a TCPAllocation (ConnectionBind).
    /// @param socket The socket to release.
    void releaseTCPSocket(const net::Socket& socket);

    /// @return Reference to the observer provided at construction.
    ServerObserver& observer();

    /// @return Reference to the mutable options struct.
    ServerOptions& options();

    /// @return Reference to the UDP server socket.
    net::UDPSocket& udpSocket();

    /// @return Reference to the TCP server listening socket.
    net::TCPSocket& tcpSocket();

    /// @return Reference to the maintenance timer.
    Timer& timer();

    /// Accept callback for the TCP listening socket; registers new connections
    /// for STUN message processing.
    /// @param sock Newly accepted TCP socket.
    void onTCPAcceptConnection(const net::TCPSocket::Ptr& sock);

    /// Close callback for accepted TCP sockets; removes the socket from the list.
    /// @param socket The closed socket.
    bool onTCPSocketClosed(net::Socket& socket);

    /// Receive callback for both UDP and TCP sockets; parses STUN messages and
    /// calls handleRequest() for each one.
    /// @param socket      The receiving socket.
    /// @param buffer      Received data buffer.
    /// @param peerAddress Source address of the data.
    bool onSocketRecv(net::Socket& socket, const MutableBuffer& buffer,
                      const net::Address& peerAddress);

    /// Periodic maintenance callback; expires and removes stale allocations.
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
