///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup turn
/// @{


#include "icy/turn/client/client.h"
#include "icy/application.h"
#include "icy/crypto/hash.h"
#include "icy/hex.h"
#include "icy/logger.h"
#include "icy/net/tcpsocket.h"
#include "icy/net/udpsocket.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>


namespace icy {
namespace turn {


Client::Client(ClientObserver& observer, const Options& options, const net::Socket::Ptr& socket)
    : _observer(observer)
    , _options(options)
    , _socket(socket)
{
}


Client::~Client()
{
    LTrace("Destroy");
    stop();
}


void Client::start()
{
    LDebug("TURN client connecting to ", _options.serverAddr);

    if (_permissions.empty())
        throw std::runtime_error("permissions not set before starting TURN client");
    if (!_socket.impl)
        throw std::runtime_error("socket not set before starting TURN client");

    auto udpSocket = dynamic_cast<net::UDPSocket*>(_socket.impl.get());
    if (udpSocket) {
        udpSocket->bind(net::Address("0.0.0.0", 0));
    }

    _socket.Recv += slot(this, &Client::onSocketRecv, -1, -1);
    _socket.Connect += slot(this, &Client::onSocketConnect);
    _socket.Close += slot(this, &Client::onSocketClose);
    _socket->connect(_options.serverAddr);
}


void Client::stop()
{
    _timer.stop();

    for (auto& txn : _transactions) {
        LTrace("Shutdown base: Disposing transaction: ", txn.get());
        txn->StateChange -= slot(this, &Client::onTransactionProgress);
        txn->dispose();
    }
    _transactions.clear();

    _socket.Connect -= slot(this, &Client::onSocketConnect);
    _socket.Recv -= slot(this, &Client::onSocketRecv);
    //_socket->Error -= slot(this, &Client::onSocketError);
    _socket.Close -= slot(this, &Client::onSocketClose);
    if (!_socket->closed()) {
        _socket->close();
    }
}


bool Client::onSocketConnect(net::Socket& socket)
{
    LTrace("Client connected");
    _socket.Connect -= slot(this, &Client::onSocketConnect);

    _timer.setInterval(_options.timerInterval);
    _timer.start([this]() { onTimer(); });

    sendAllocate();
    return false;
}


bool Client::onSocketRecv(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress)
{
    LTrace("Control socket recv: ", buffer.size());

    stun::Message message;
    char* buf = bufferCast<char*>(buffer);
    size_t len = buffer.size();
    size_t nread = 0;
    while (len > 0 && (nread = message.read(constBuffer(buf, len))) > 0) {
        handleResponse(message);
        buf += nread;
        len -= nread;
    }
    if (len == buffer.size())
        LWarn("Non STUN packet received");
    return false;
}


bool Client::onSocketClose(net::Socket& socket)
{
    if (&socket != _socket.impl.get())
        throw std::logic_error("onSocketClose called with unexpected socket");
    LTrace("Control socket closed");
    if (!_socket->closed())
        throw std::logic_error("socket reported close but is not in closed state");
    stop();
    setError(_socket->error());
    return false;
}


void Client::sendRefresh()
{
    // 7. Refreshing an Allocation
    //
    // A Refresh transaction can be used to either (a) refresh an existing
    // allocation and update its time-to-expiry or (b) delete an existing
    // allocation.
    //
    // If a client wishes to continue using an allocation, then the client
    // MUST refresh it before it expires. It is suggested that the client
    // refresh the allocation roughly 1 minute before it expires. If a
    // client no longer wishes to use an allocation, then it SHOULD
    // explicitly delete the allocation. A client MAY refresh an allocation
    // at any time for other reasons.

    LTrace("Send refresh allocation request");

    auto transaction = createTransaction();
    transaction->request().setClass(stun::Message::Request);
    transaction->request().setMethod(stun::Message::Refresh);

    transaction->request().add<stun::Lifetime>().setValue(
        static_cast<uint32_t>(_options.lifetime) / 1000);

    sendAuthenticatedTransaction(transaction.get());
}


void Client::handleRefreshResponse(const stun::Message& response)
{
    LTrace("Received a Refresh Response: ", response.toString());

    if (response.methodType() != stun::Message::Refresh)
        throw std::logic_error("handleRefreshResponse called with non-Refresh message");

    // 7.3. Receiving a Refresh Response
    //
    //
    // If the client receives a success response to its Refresh request with
    // a non-zero lifetime, it updates its copy of the allocation data
    // structure with the time-to-expiry value contained in the response.
    //
    // If the client receives a 437 (Allocation Mismatch) error response to
    // a request to delete the allocation, then the allocation no longer
    // exists and it should consider its request as having effectively
    // succeeded.
    auto errorAttr = response.get<stun::ErrorCode>();
    if (errorAttr) {
        if (errorAttr->errorCode() == kErrorAllocationMismatch) {
            // Allocation already gone, treat as success
            return;
        }
        LWarn("Unexpected error in Refresh response: ", errorAttr->errorCode());
        return;
    }

    auto lifetimeAttr = response.get<stun::Lifetime>();
    if (!lifetimeAttr) {
        LWarn("Refresh response missing Lifetime attribute");
        return;
    }

    setLifetime(lifetimeAttr->value());

    LTrace("Refreshed allocation expires in: ", timeRemaining());

    // If lifetime is 0 the allocation will be cleaned up by garbage collection.
}


stun::Transaction::Ptr Client::removeTransaction(stun::Transaction* transaction)
{
    LTrace("Removing transaction: ", transaction);

    for (auto it = _transactions.begin(); it != _transactions.end(); ++it) {
        if (it->get() == transaction) {
            stun::Transaction::Ptr ptr = std::move(*it);
            ptr->StateChange -= slot(this, &Client::onTransactionProgress);
            _transactions.erase(it);
            return ptr;
        }
    }
    LWarn("Attempted to remove unknown transaction: ", transaction);
    return nullptr;
}


void Client::authenticateRequest(stun::Message& request)
{
    // Authenticate messages once the server provides us with realm and noonce
    if (_realm.empty())
        return;

    if (_options.username.size()) {
        request.add<stun::Username>().copyBytes(
            _options.username.c_str(), _options.username.size());
    }

    if (_realm.size()) {
        request.add<stun::Realm>().copyBytes(_realm.c_str(), _realm.size());
    }

    if (_nonce.size()) {
        request.add<stun::Nonce>().copyBytes(_nonce.c_str(), _nonce.size());
    }

    if (_realm.size() && _options.password.size()) {
        crypto::Hash engine("md5");
        engine.update(_options.username + ":" + _realm + ":" +
                      _options.password);
        STrace << "Generating HMAC: data="
               << (_options.username + ":" + _realm + ":" + _options.password)
               << ", key=" << engine.digestStr();
        request.add<stun::MessageIntegrity>().setKey(engine.digestStr());
    }
}


bool Client::sendAuthenticatedTransaction(stun::Transaction* transaction)
{
    authenticateRequest(transaction->request());
    STrace << "Send authenticated transaction: "
           << transaction->request().toString();
    return transaction->send();
}


stun::Transaction::Ptr Client::createTransaction(const net::Socket::Ptr& socket)
{
    auto transaction = makeIntrusive<stun::Transaction>(
        socket ? socket : _socket.impl, _options.serverAddr, _options.timeout, 1);
    transaction->StateChange += slot(this, &Client::onTransactionProgress);
    _transactions.push_back(transaction);
    return transaction;
}


bool Client::handleResponse(const stun::Message& response)
{
    LTrace("Handle response: ", response.toString());

    // Send this response to the appropriate handler.
    if (response.methodType() == stun::Message::Allocate) {
        if (response.classType() == stun::Message::SuccessResponse)
            handleAllocateResponse(response);

        else if (response.classType() == stun::Message::ErrorResponse)
            handleAllocateErrorResponse(response);

        // Must be a Transaction response
        else {
            LWarn("Unexpected Allocate response class: ", response.classType());
            return false;
        }
    }

    else if (response.methodType() == stun::Message::Refresh)
        handleRefreshResponse(response);

    else if (response.methodType() == stun::Message::CreatePermission &&
             response.classType() == stun::Message::SuccessResponse)
        handleCreatePermissionResponse(response);

    else if (response.methodType() == stun::Message::CreatePermission &&
             response.classType() == stun::Message::ErrorResponse)
        handleCreatePermissionErrorResponse(response);

    else if (response.methodType() == stun::Message::DataIndication)
        handleDataIndication(response);

    else
        return false;

    return true;
}


void Client::sendAllocate()
{
    LTrace("Send allocation request");

    if (_options.username.empty())
        throw std::runtime_error("username must be set before sending Allocate");
    if (_options.password.empty())
        throw std::runtime_error("password must be set before sending Allocate");

    auto transaction = createTransaction();
    transaction->request().setClass(stun::Message::Request);
    transaction->request().setMethod(stun::Message::Allocate);

    transaction->request().add<stun::RequestedTransport>().setValue(
        transportProtocol() << 24);

    if (_options.lifetime) {
        transaction->request().add<stun::Lifetime>().setValue(
            static_cast<uint32_t>(_options.lifetime) / 1000);
    }

    sendAuthenticatedTransaction(transaction.get());
}


//  At this point the response has already been authenticated, but we
//  have not checked for existing allocations on this 5 tuple.
void Client::handleAllocateResponse(const stun::Message& response)
{
    LTrace("Allocate success response");
    if (response.methodType() != stun::Message::Allocate)
        throw std::logic_error("handleAllocateResponse called with non-Allocate message");

    auto lifetimeAttr = response.get<stun::Lifetime>();
    if (!lifetimeAttr) {
        LWarn("Allocate response missing Lifetime attribute");
        return;
    }

    auto mappedAttr = response.get<stun::XorMappedAddress>();
    if (!mappedAttr || mappedAttr->family() == stun::AddressFamily::Undefined) {
        LWarn("Allocate response missing or invalid XorMappedAddress");
        return;
    }
    _mappedAddress = mappedAttr->address();

    auto relayedAttr = response.get<stun::XorRelayedAddress>();
    if (!relayedAttr || relayedAttr->family() == stun::AddressFamily::Undefined) {
        LWarn("Allocate response missing or invalid XorRelayedAddress");
        return;
    }

    if (relayedAttr->address().host() == "0.0.0.0") {
        LError("Allocate response contains invalid loopback relay address");
        return;
    }

    // Use the relay server host and relayed port
    _relayedAddress = net::Address(
        relayedAttr->address().host(),
        relayedAttr->address().port());

    STrace << "Allocation created:"
           << "\n\tRelayed address: " << _relayedAddress
           << "\n\tMapped address: " << _mappedAddress
           << "\n\tLifetime: " << lifetimeAttr->value();

    // Once the allocation is created we transition to Authorizing while
    // peer permissions are created.
    setState(this, ClientState::Authorizing);

    // If the permission list has entries create them now.
    // A successful response here will set the client state to Success.
    if (!closed())
        sendCreatePermission();
}


void Client::handleAllocateErrorResponse(const stun::Message& response)
{
    LTrace("Allocate error response");

    if (response.methodType() != stun::Message::Allocate ||
        response.classType() != stun::Message::ErrorResponse) {
        LWarn("Unexpected message in handleAllocateErrorResponse");
        return;
    }

    auto errorAttr = response.get<stun::ErrorCode>();
    if (!errorAttr) {
        LWarn("Allocate error response missing ErrorCode attribute");
        return;
    }

    STrace << "Allocation error response: " << errorAttr->errorCode() << ": "
           << errorAttr->reason();

    switch (errorAttr->errorCode()) {
        case kErrorTryAlternate:
            LWarn("Server requested Try Alternate (300)");
            break;

        case kErrorBadRequest:
            LWarn("Server returned Bad Request (400)");
            break;

        case kErrorNotAuthorized: {
            if (_realm.empty() || _nonce.empty()) {

                // REALM
                const stun::Realm* realmAttr = response.get<stun::Realm>();
                if (realmAttr) {
                    _realm = realmAttr->asString();
                }

                // NONCE
                const stun::Nonce* nonceAttr = response.get<stun::Nonce>();
                if (nonceAttr) {
                    _nonce = nonceAttr->asString();
                }

                // Now that our realm and nonce are set we can re-send the
                // allocate request.
                if (_realm.size() && _nonce.size()) {
                    LTrace("Resending authenticated allocation request");
                    sendAllocate();
                    return;
                }
            }
        }

        break;

        case kErrorForbidden:
            LWarn("Server returned Forbidden (403)");
            break;

        case kErrorUnknownAttribute:
            LWarn("Server returned Unknown Attribute (420)");
            break;

        case kErrorAllocationMismatch:
            LWarn("Server returned Allocation Mismatch (437)");
            break;

        case kErrorStaleNonce: {
            // RFC 5389 section 10.2.3: If a 438 response is received, the
            // client updates the nonce and re-sends the request.
            LTrace("Stale Nonce (438): updating nonce and retrying");
            const stun::Nonce* nonceAttr = response.get<stun::Nonce>();
            if (nonceAttr) {
                _nonce = nonceAttr->asString();
            }
            const stun::Realm* realmAttr = response.get<stun::Realm>();
            if (realmAttr) {
                _realm = realmAttr->asString();
            }
            if (!_nonce.empty() && !_realm.empty()) {
                sendAllocate();
                return;
            }
        } break;

        case kErrorWrongCredentials:
            LWarn("Server returned Wrong Credentials (441)");
            break;

        case kErrorUnsupportedTransport:
            LWarn("Server returned Unsupported Transport (442)");
            break;

        case kErrorAllocationQuotaReached:
            LWarn("Server returned Allocation Quota Reached (486)");
            break;

        case kErrorInsufficientCapacity:
            LWarn("Server returned Insufficient Capacity (508)");
            break;

        // An unknown error response MUST be handled as described in [RFC5389].
        default:
            LWarn("Server returned unknown error: ", errorAttr->errorCode());
            break;
    }

    setError(util::format("(%d) %s", static_cast<int>(errorAttr->errorCode()),
                          errorAttr->reason().c_str()));

    if (!closed()) {
        _observer.onAllocationFailed(
            *this, errorAttr->errorCode(),
            errorAttr->reason()); // may result in instance deletion
    }
}


void Client::setError(const icy::Error& error)
{
    _error = error;
    setState(this, ClientState::Failed);
}


void Client::addPermission(const IPList& peerIPs)
{
    for (const auto& ip : peerIPs) {
        addPermission(ip);
    }
}


void Client::addPermission(const std::string& peerIP)
{
    IAllocation::addPermission(peerIP);
}


void Client::sendCreatePermission()
{
    LTrace("Send Create Permission Request");

    if (_permissions.empty())
        throw std::runtime_error("no permissions set for CreatePermission request");

    auto transaction = createTransaction();
    transaction->request().setClass(stun::Message::Request);
    transaction->request().setMethod(stun::Message::CreatePermission);

    for (const auto& perm : _permissions) {
        LTrace("Create permission request: ", perm.ip);
        transaction->request().add<stun::XorPeerAddress>().setAddress(
            net::Address(perm.ip, 0));
    }

    sendAuthenticatedTransaction(transaction.get());
}


void Client::handleCreatePermissionResponse(const stun::Message& /* response */)
{
    // If the client receives a valid CreatePermission success response,
    // then the client updates its data structures to indicate that the
    // permissions have been installed or refreshed.
    LTrace("Permission created");

    // Send all queued requests...
    {
        while (!_pendingIndications.empty()) {
            _socket->sendPacket(_pendingIndications.front());
            _pendingIndications.pop_front();
        }
    }

    if (!closed()) {
        _observer.onAllocationPermissionsCreated(*this, _permissions);
    }

    // Once permissions have been created the allocation
    // process is considered a success.
    LTrace("Allocation Success");
    setState(this, ClientState::Success);
}


void Client::handleCreatePermissionErrorResponse(const stun::Message& /* response */)
{
    LWarn("Permission Creation Failed");

    removeAllPermissions();

    setError("Cannot create server permissions.");
}


void Client::sendChannelBind(const std::string& /* peerIP */)
{
    // See client.h for rationale on why channel bindings are not implemented.
    // Data relay uses sendData() with Send Indications instead.
    throw std::logic_error("sendChannelBind not implemented: channel bindings "
                           "(RFC 5766 s11) are not needed; use sendData() instead");
}


void Client::sendData(const char* data, size_t size, const net::Address& peerAddress)
{
    LTrace("Send Data Indication to peer: ", peerAddress);

    stun::Message request;
    request.setClass(stun::Message::Indication);
    request.setMethod(stun::Message::SendIndication);

    request.add<stun::XorPeerAddress>().setAddress(peerAddress);
    request.add<stun::Data>().copyBytes(data, size);

    // Ensure permissions exist for the peer.
    if (!hasPermission(peerAddress)) {
        throw std::runtime_error("No permission exists for peer IP: " + peerAddress.host());
    }

    // If permission exists and is currently being negotiated with
    // the server then queue the outgoing request.
    // Queued requests will be sent when the CreatePermission
    // callback is received from the server.
    else if (stateEquals(ClientState::Authorizing)) {
        LTrace("Queueing outgoing request: ", request.toString());

        _pendingIndications.push_back(request);
        if (_pendingIndications.size() >= 100)
            LWarn("Excessive pending indications queued: ", _pendingIndications.size());
    }

    // If a permission exists on server and client send our data!
    else {
        _socket->sendPacket(request, _options.serverAddr);
    }
}


void Client::handleDataIndication(const stun::Message& response)
{
    auto peerAttr = response.get<stun::XorPeerAddress>();
    if (!peerAttr || peerAttr->family() == stun::AddressFamily::Undefined) {
        LWarn("Data indication missing or invalid XorPeerAddress");
        return;
    }

    auto dataAttr = response.get<stun::Data>();
    if (!dataAttr) {
        LWarn("Data indication missing Data attribute");
        return;
    }

    LTrace("Handle Data indication: ", response.toString());

    if (!closed()) {
        _observer.onRelayDataReceived(*this, dataAttr->bytes(), dataAttr->size(), peerAttr->address());
    }
}


void Client::onTransactionProgress(void* sender, TransactionState& state, const TransactionState&)
{
    LTrace("Transaction state change: ", sender, ": ", state);

    auto* raw = static_cast<stun::Transaction*>(sender);

    // Store an IntrusivePtr in the response so TCPClient handlers can access
    // the original request (e.g. to extract XorPeerAddress). The IntrusivePtr
    // keeps the transaction alive as long as the response message exists.
    // Find our Ptr in the list and copy it.
    for (const auto& txn : _transactions) {
        if (txn.get() == raw) {
            raw->response().opaque = txn; // copy the IntrusivePtr into std::any
            break;
        }
    }

    if (!closed())
        _observer.onTransactionResponse(*this, *raw);

    switch (state.id()) {
        case TransactionState::Running:
            return;

        case TransactionState::Success: {
            STrace << "STUN transaction success:"
                   << "\n\tState: " << state.toString()
                   << "\n\tFrom: " << raw->peerAddress().toString()
                   << "\n\tRequest: " << raw->request().toString()
                   << "\n\tResponse: " << raw->response().toString()
                  ;

            // Remove from list but hold an IntrusivePtr to keep alive during handling
            auto ref = removeTransaction(raw);
            if (ref) {
                if (!handleResponse(ref->response())) {
                    STrace << "Unhandled STUN response: "
                           << ref->response().toString();
                }
            }
            // ref drops here; transaction deleted if no other refs
        } break;

        case TransactionState::Failed:
            SWarn << "STUN transaction error:"
                  << "\n\tState: " << state.toString()
                  << "\n\tFrom: " << raw->peerAddress().toString()
                  << "\n\tData: " << raw->response().toString();

            if (removeTransaction(raw)) {
                setError("Transaction failed");
            }
            break;
    }
}


void Client::onTimer()
{
    if (expired()) {
        // Attempt to re-allocate
        sendAllocate();
    } else if (timeRemaining() < lifetime() * 0.33) {
        sendRefresh();
    }

    // Refresh permissions before they expire (5 min lifetime per RFC 5766).
    // Re-send CreatePermission when roughly 1 minute remains.
    if (stateEquals(ClientState::Success) && !_permissions.empty()) {
        for (const auto& perm : _permissions) {
            if (perm.timeout.running() && perm.timeout.remaining() < 60 * 1000) {
                sendCreatePermission();
                break;
            }
        }
    }

    _observer.onTimer(*this);
}


void Client::onStateChange(ClientState& state, const ClientState& oldState)
{
    _observer.onClientStateChange(*this, state, oldState);
}


int Client::transportProtocol()
{
    return kProtocolUDP;
}


bool Client::closed() const
{
    return stateEquals(ClientState::None) || stateEquals(ClientState::Failed);
}


const Client::Options& Client::options() const
{
    return _options;
}


net::Address Client::mappedAddress() const
{
    return _mappedAddress;
}


net::Address Client::relayedAddress() const
{
    return _relayedAddress;
}


} // namespace turn
} // namespace icy


/// @}
