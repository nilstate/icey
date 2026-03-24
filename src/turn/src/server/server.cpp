///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/server/server.h"
#include "icy/buffer.h"
#include "icy/logger.h"

#include <algorithm>
#include <memory>
#include <stdexcept>


using std::min;
using namespace icy::net;


namespace icy {
namespace turn {


Server::Server(ServerObserver& observer, const ServerOptions& options)
    : _observer(observer)
    , _options(options)
    , _udpSocket(nullptr)
    , _tcpSocket(nullptr)
{
    LTrace("Create");
}


Server::~Server()
{
    LTrace("Destroy");
    stop();
    LTrace("Destroy: OK");
}


void Server::start()
{
    LTrace("Starting");

    if (_options.enableUDP) {
        _udpSocket.swap(net::makeSocket<net::UDPSocket>());
        _udpSocket.Recv += slot(this, &Server::onSocketRecv, 1);
        _udpSocket->bind(_options.listenAddr);
        LTrace("UDP listening on ", _options.listenAddr);
    }

    if (_options.enableTCP) {
        _tcpSocket.swap(net::makeSocket<net::TCPSocket>());
        _tcpSocket->bind(_options.listenAddr);
        _tcpSocket->listen();
        _tcpSocket.as<net::TCPSocket>()->AcceptConnection +=
            slot(this, &Server::onTCPAcceptConnection);
        LTrace("TCP listening on ", _options.listenAddr);
    }

    _timer.setInterval(_options.timerInterval);
    _timer.start([this]() { onTimer(); });
}


void Server::stop()
{
    LTrace("Stopping");

    _timer.stop();

    // Delete allocations - extract all first, then destroy outside lock
    {
        ServerAllocationMap doomed;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            doomed.swap(_allocations);
        }
        // doomed is destroyed here
    }

    // Free all TCP control sockets.
    _tcpSockets.clear();

    // Close server sockets
    if (_udpSocket.impl)
        _udpSocket->close();
    if (_tcpSocket.impl)
        _tcpSocket->close();
}


void Server::onTimer()
{
    // Take a snapshot of raw pointers for iteration.
    // Collect expired allocations, then erase them in a second pass.
    auto allocs = allocations();
    std::vector<FiveTuple> toRemove;
    for (auto& [tuple, alloc] : allocs) {
        if (!alloc->onTimer()) {
            toRemove.push_back(tuple);
        }
    }

    for (const auto& tuple : toRemove) {
        // Extract the unique_ptr so destruction happens outside the lock
        std::unique_ptr<ServerAllocation> doomed;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            auto it = _allocations.find(tuple);
            if (it != _allocations.end()) {
                doomed = std::move(it->second);
                _allocations.erase(it);
            }
        }
        // doomed is destroyed here, which calls ~ServerAllocation -> removeAllocation
        // removeAllocation will not find it in the map (already erased), which is fine
    }
}


void Server::onTCPAcceptConnection(const net::TCPSocket::Ptr& sock)
{
    LTrace("TCP connection accepted: ", sock->peerAddress());

    net::SocketEmitter emitter(sock);
    emitter.Recv += slot(this, &Server::onSocketRecv);
    emitter.Close += slot(this, &Server::onTCPSocketClosed);
    _tcpSockets.push_back(emitter);
}


net::TCPSocket::Ptr Server::getTCPSocket(const net::Address& peerAddr)
{
    for (auto& sock : _tcpSockets) {
        if (sock->peerAddress() == peerAddr) {
            return std::dynamic_pointer_cast<TCPSocket>(sock.impl);
        }
    }
    LWarn("getTCPSocket: no socket found for ", peerAddr);
    return net::TCPSocket::Ptr();
}


bool Server::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer,
                          const net::Address& peerAddress)
{
    LTrace("Data received: ", buffer.size());

    stun::Message message;
    char* buf = bufferCast<char*>(buffer);
    size_t len = buffer.size();
    size_t nread = 0;
    while (len > 0 && (nread = message.read(constBuffer(buf, len))) > 0) {
        if (message.classType() == stun::Message::Request ||
            message.classType() == stun::Message::Indication) {
            Request request(message, socket.transport(), socket.address(), peerAddress);

            // Note: authenticates both Requests and Indications
            handleRequest(request, _observer.authenticateRequest(this, request));
        } else {
            LWarn("Received unknown STUN message class: ", message.classType());
        }

        buf += nread;
        len -= nread;
    }
    if (len == buffer.size())
        LWarn("Non STUN packet received");
    return false;
}


bool Server::onTCPSocketClosed(net::Socket& socket)
{
    LTrace("TCP socket closed");
    releaseTCPSocket(socket);
    return false;
}


void Server::releaseTCPSocket(const net::Socket& socket)
{
    LTrace("Removing TCP socket: ", &socket);
    for (auto it = _tcpSockets.begin(); it != _tcpSockets.end(); ++it) {
        if (it->impl.get() == &socket) {
            it->Recv -= slot(this, &Server::onSocketRecv);
            it->Close -= slot(this, &Server::onTCPSocketClosed);
            _pendingReleasedTCPSockets.insert(&socket);
            scheduleDeferredTCPSocketRelease();
            return;
        }
    }
    LWarn("releaseTCPSocket: socket not found in list");
}


void Server::scheduleDeferredTCPSocketRelease()
{
    if (_tcpSocketReleaseScheduled || _pendingReleasedTCPSockets.empty())
        return;

    _tcpSocketReleaseScheduled = true;
    auto* idle = new uv_idle_t;
    uv_idle_init(_timer.handle().loop(), idle);
    idle->data = this;
    uv_idle_start(idle, [](uv_idle_t* handle) {
        auto* self = static_cast<Server*>(handle->data);
        uv_idle_stop(handle);
        self->drainReleasedTCPSockets();
        uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* h) {
            delete reinterpret_cast<uv_idle_t*>(h);
        });
    });
}


void Server::drainReleasedTCPSockets()
{
    if (_pendingReleasedTCPSockets.empty()) {
        _tcpSocketReleaseScheduled = false;
        return;
    }

    for (auto it = _tcpSockets.begin(); it != _tcpSockets.end();) {
        if (_pendingReleasedTCPSockets.count(it->impl.get()) > 0)
            it = _tcpSockets.erase(it);
        else
            ++it;
    }

    _pendingReleasedTCPSockets.clear();
    _tcpSocketReleaseScheduled = false;
}


void Server::handleRequest(Request& request, AuthenticationState state)
{
    STrace << "Received STUN request:\n"
           << "\tFrom: " << request.remoteAddress << "\n"
           << "\tData: " << request.toString();

    switch (state) {
        case AuthenticationState::Authenticating:
            // await async response
            break;

        case AuthenticationState::Authorized:
            handleAuthorizedRequest(request);
            break;

        case AuthenticationState::QuotaReached:
            respondError(request, kErrorAllocationQuotaReached, "Allocation Quota Reached");
            break;

        case AuthenticationState::NotAuthorized:
            respondError(request, kErrorNotAuthorized, "NotAuthorized");
            break;
    }
}


void Server::handleAuthorizedRequest(Request& request)
{
    LTrace("Handle authorized request: ", request.toString());

    switch (request.methodType()) {
        case stun::Message::Binding:
            handleBindingRequest(request);
            break;

        case stun::Message::Allocate:
            handleAllocateRequest(request);
            break;

        case stun::Message::ConnectionBind:
            handleConnectionBindRequest(request);
            break;

        default: {
            FiveTuple tuple(request.remoteAddress, request.localAddress, request.transport);
            auto allocation = getAllocation(tuple);
            if (!allocation) {
                respondError(request, kErrorAllocationMismatch, "Allocation Mismatch");
                return;
            }

            LTrace("Obtained allocation: ", tuple);
            if (!allocation->handleRequest(request))
                respondError(request, kErrorOperationNotSupported, "Operation Not Supported");
        }
    }
}


void Server::handleConnectionBindRequest(Request& request)
{
    auto connAttr = request.get<stun::ConnectionID>();
    if (!connAttr) {
        LTrace("ConnectionBind request has no ConnectionID");
        respondError(request, kErrorBadRequest, "Bad Request");
        return;
    }

    auto alloc = getTCPAllocation(connAttr->value());
    if (!alloc) {
        STrace << "ConnectionBind request has no allocation for: "
               << connAttr->value();
        respondError(request, kErrorBadRequest, "Bad Request");
        return;
    }

    alloc->handleConnectionBindRequest(request);
}


void Server::handleBindingRequest(Request& request)
{
    LTrace("Handle Binding request");

    if (request.methodType() != stun::Message::Binding)
        throw std::logic_error("handleBindingRequest called with non-Binding message");
    if (request.classType() != stun::Message::Request)
        throw std::logic_error("handleBindingRequest called with non-Request class");

    stun::Message response(stun::Message::SuccessResponse,
                           stun::Message::Binding);
    response.setTransactionID(request.transactionID());

    response.add<stun::XorMappedAddress>().setAddress(request.remoteAddress);

    respond(request, response);
}


void Server::handleAllocateRequest(Request& request)
{
    LTrace("Handle Allocate request");

    if (request.methodType() != stun::Message::Allocate)
        throw std::logic_error("handleAllocateRequest called with non-Allocate message");
    if (request.classType() != stun::Message::Request)
        throw std::logic_error("handleAllocateRequest called with non-Request class");

    auto usernameAttr = request.get<stun::Username>();
    if (!usernameAttr) {
        LTrace("STUN Request not authorized ");
        respondError(request, kErrorNotAuthorized, "NotAuthorized");
        return;
    }

    std::string username(usernameAttr->asString());

    auto transportAttr = request.get<stun::RequestedTransport>();
    if (!transportAttr) {
        LError("No Requested Transport");
        respondError(request, kErrorBadRequest, "Bad Request");
        return;
    }

    int protocol = transportAttr->value() >> 24;
    if (protocol != kProtocolTCP && protocol != kProtocolUDP) {
        LError("Requested Transport is neither TCP or UDP: ", protocol);
        respondError(request, kErrorUnsupportedTransport, "Unsupported Transport Protocol");
        return;
    }

    FiveTuple tuple(request.remoteAddress, request.localAddress,
                    protocol == kProtocolUDP ? net::UDP : net::TCP);
    if (getAllocation(tuple)) {
        LError("Allocation already exists for 5tuple: ", tuple);
        respondError(request, kErrorAllocationMismatch, "Allocation Mismatch");
        return;
    }

    // Compute the appropriate LIFETIME for this allocation.
    uint32_t lifetime = min(options().allocationMaxLifetime / 1000,
                            options().allocationDefaultLifetime / 1000);
    auto lifetimeAttr = request.get<stun::Lifetime>();
    if (lifetimeAttr)
        lifetime = min(lifetime, lifetimeAttr->value());

    std::unique_ptr<ServerAllocation> allocation;

    // Protocol specific allocation handling.
    if (protocol == kProtocolUDP) {
        allocation = std::make_unique<UDPAllocation>(*this, tuple, username, lifetime);
    } else if (protocol == kProtocolTCP) {
        allocation = std::make_unique<TCPAllocation>(*this, getTCPSocket(request.remoteAddress), tuple,
                                                     username, lifetime);
    }

    // Once the allocation is created, the server replies with a success
    // response.  The success response contains:

    stun::Message response(stun::Message::SuccessResponse,
                           stun::Message::Allocate);
    response.setTransactionID(request.transactionID());

    // o  An XOR-RELAYED-ADDRESS attribute containing the relayed transport
    //    address.

    // Use the externalIP if configured (to overcome NAT), otherwise
    // fall back to the relay socket's local address.
    std::string relayHost(options().externalIP);
    if (relayHost.empty()) {
        relayHost.assign(allocation->relayedAddress().host());
    }

    auto& relayAddr = response.add<stun::XorRelayedAddress>();
    relayAddr.setAddress(net::Address(relayHost, allocation->relayedAddress().port()));

    response.add<stun::Lifetime>().setValue(lifetime);

    auto& mappedAddr = response.add<stun::XorMappedAddress>();
    mappedAddr.setAddress(request.remoteAddress);

    STrace << "Allocate response: "
           << "XorRelayedAddress=" << relayAddr.address()
           << ", XorMappedAddress=" << mappedAddr.address()
           << ", MessageIntegrity=" << request.hash;

    respond(request, response);

    // Register the allocation with the server (transfers ownership)
    addAllocation(std::move(allocation));

    LTrace("Handle Allocate request: OK");
}


void Server::respond(Request& request, stun::Message& response)
{
    if (!request.hash.empty()) {
        response.add<stun::MessageIntegrity>().setKey(request.hash);
    }

    LTrace("Sending message: ", response, ": ", request.remoteAddress);

    // The response (either success or error) is sent back to the
    // client on the 5-tuple.
    switch (request.transport) {
        case net::UDP:
            _udpSocket->sendPacket(response, request.remoteAddress);
            break;
        case net::TCP:
        case net::SSLTCP:
            auto socket = getTCPSocket(request.remoteAddress);
            if (socket)
                socket->sendPacket(response);
            break;
    }
}

void Server::respondError(Request& request, int errorCode,
                          const char* errorDesc)
{
    LTrace("Send STUN error: ", errorCode, ": ", errorDesc);

    stun::Message errorMsg(stun::Message::ErrorResponse, request.methodType());
    errorMsg.setTransactionID(request.transactionID());

    errorMsg.add<stun::Software>().copyBytes(
        _options.software.c_str(), _options.software.size());

    errorMsg.add<stun::Realm>().copyBytes(
        _options.realm.c_str(), _options.realm.size());

    std::string nonce = util::randomString(32);
    errorMsg.add<stun::Nonce>().copyBytes(nonce.c_str(), nonce.size());

    auto& err = errorMsg.add<stun::ErrorCode>();
    err.setErrorCode(errorCode);
    err.setReason(errorDesc);

    respond(request, errorMsg);
}


net::UDPSocket& Server::udpSocket()
{
    return *_udpSocket.as<net::UDPSocket>();
}


net::TCPSocket& Server::tcpSocket()
{
    return *_tcpSocket.as<net::TCPSocket>();
}


ServerObserver& Server::observer()
{
    return _observer;
}


const ServerOptions& Server::options() const
{
    return _options;
}


std::map<FiveTuple, ServerAllocation*> Server::allocations() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::map<FiveTuple, ServerAllocation*> result;
    for (const auto& [tuple, alloc] : _allocations) {
        result[tuple] = alloc.get();
    }
    return result;
}


Timer& Server::timer()
{
    return _timer;
}


void Server::addAllocation(std::unique_ptr<ServerAllocation> alloc)
{
    auto* raw = alloc.get();
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_allocations.find(raw->tuple()) != _allocations.end())
            throw std::logic_error("allocation already exists for tuple: " + raw->tuple().toString());
        _allocations[raw->tuple()] = std::move(alloc);

        SDebug << "Allocation added: " << raw->tuple().toString() << ": "
               << _allocations.size() << " total";
    }

    _observer.onServerAllocationCreated(this, raw);
}


void Server::removeAllocation(ServerAllocation* alloc)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _allocations.find(alloc->tuple());
        if (it != _allocations.end()) {
            // Release ownership but don't erase yet - the caller (destructor) owns the memory
            it->second.release();
            _allocations.erase(it);

            SDebug << "Allocation removed: " << alloc->tuple().toString() << ": "
                   << _allocations.size() << " remaining";
        }
        // If not found, the allocation was already removed from the map
        // (e.g., by onTimer or stop) before the destructor ran - this is normal.
    }

    _observer.onServerAllocationRemoved(this, alloc);
}


ServerAllocation* Server::getAllocation(const FiveTuple& tuple)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _allocations.find(tuple);
    if (it != _allocations.end())
        return it->second.get();
    return nullptr;
}


TCPAllocation* Server::getTCPAllocation(const uint32_t& connectionID)
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& [tuple, alloc_ptr] : _allocations) {
        auto alloc = dynamic_cast<TCPAllocation*>(alloc_ptr.get());
        if (alloc && alloc->pairs().count(connectionID))
            return alloc;
    }

    return nullptr;
}


} // namespace turn
} // namespace icy


/// @}
