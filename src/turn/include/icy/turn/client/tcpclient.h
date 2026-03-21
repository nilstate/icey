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


#include "icy/collection.h"
#include "icy/net/tcpsocket.h"
#include "icy/turn/client/client.h"

#include <deque>
#include <string>


namespace icy {
namespace turn {


class TURN_API TCPClient;


/// Observer interface for TCP TURN client events (RFC 6062).
/// Extends ClientObserver with callbacks specific to TCP relay connections.
struct TCPClientObserver : public ClientObserver
{
    /// Called when a ConnectionBind succeeds and the relay pipe is live.
    /// @param client      The owning TCP client.
    /// @param socket      The TCP socket that is now connected to the TURN server
    ///                    and ready for direct data transfer to/from the peer.
    /// @param peerAddress The remote peer address associated with this connection.
    virtual void onRelayConnectionCreated(TCPClient& client, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress) = 0;

    /// Called when an error occurs on a relay connection socket.
    /// @param client      The owning TCP client.
    /// @param socket      The affected socket.
    /// @param peerAddress The remote peer address for this connection.
    virtual void onRelayConnectionError(TCPClient& client, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress) {};

    /// Called when a relay connection socket is closed.
    /// @param client      The owning TCP client.
    /// @param socket      The closed socket.
    /// @param peerAddress The remote peer address for this connection.
    virtual void onRelayConnectionClosed(TCPClient& client, const net::TCPSocket::Ptr& socket, const net::Address& peerAddress) = 0;

    /// Called when a Connect request (client-initiated) or a ConnectionBind
    /// handshake fails for the given peer.
    /// @param client      The owning TCP client.
    /// @param peerAddress The peer address whose binding failed.
    virtual void onRelayConnectionBindingFailed(TCPClient& client, const net::Address& peerAddress) {};

    /// Called when the server sends a ConnectionAttempt indication indicating
    /// that a remote peer wants to connect. Return true to accept the connection
    /// (proceeds with ConnectionBind), or false to reject it.
    /// @param client      The owning TCP client.
    /// @param peerAddress The address of the connecting peer.
    /// @return true to accept and bind, false to ignore.
    virtual bool onPeerConnectionAttempt(TCPClient& client, const net::Address& peerAddress) { return true; };
};


/// Binding metadata stored as socket opaque data during a ConnectionBind handshake.
/// Associates a relay socket with its TURN connection ID and peer address.
struct RelayConnectionBinding
{
    uint32_t connectionID;   ///< TURN CONNECTION-ID attribute value from the server.
    net::Address peerAddress; ///< The peer address this binding targets.
};


using ConnectionManager = KVCollection<net::Address, net::SocketEmitter>;
using ConnectionManagerMap = ConnectionManager::Map;


/// TCP transport TURN client with relay connection management per RFC 6062.
/// Uses a single TCP control connection for STUN signalling and opens additional
/// TCP relay connections for each peer via the Connect / ConnectionBind flow.
class TURN_API TCPClient : public Client
{
public:
    /// @param observer Observer for TCP-specific relay events.
    /// @param options  Client configuration; defaults to loopback server, 5-min lifetime.
    TCPClient(TCPClientObserver& observer, const Client::Options& options = Client::Options());
    virtual ~TCPClient();

    /// @copydoc Client::initiate
    void initiate() override;

    /// Shuts down the control connection, cancels all relay connections, and
    /// calls the base class shutdown.
    void shutdown() override;

    /// Sends a Connect request to the server asking it to open a TCP connection
    /// to @p peerAddress on the client's behalf (RFC 6062 section 4.3).
    /// @param peerAddress Target peer address.
    virtual void sendConnectRequest(const net::Address& peerAddress);

    /// Sends raw data to @p peerAddress over the established relay connection.
    /// The peer must have an active permission and a bound relay connection.
    /// @param data        Payload bytes.
    /// @param size        Number of bytes to send.
    /// @param peerAddress Destination peer (must have an entry in connections()).
    void sendData(const char* data, size_t size, const net::Address& peerAddress) override;

    /// Extends the base handler with Connect, ConnectionBind, and
    /// ConnectionAttempt responses.
    /// @param response Incoming STUN response.
    /// @return true if the message was handled.
    bool handleResponse(const stun::Message& response) override;

    /// Processes a successful Connect response; extracts the connection ID
    /// and calls createAndBindConnection().
    /// @param response Connect success response.
    virtual void handleConnectResponse(const stun::Message& response);

    /// Processes a Connect error response; notifies the observer that binding failed.
    /// @param response Connect error response.
    virtual void handleConnectErrorResponse(const stun::Message& response);

    /// Processes a successful ConnectionBind response; arms the relay socket for
    /// data transfer and notifies the observer.
    /// @param response ConnectionBind success response.
    virtual void handleConnectionBindResponse(const stun::Message& response);

    /// Processes a ConnectionBind error response; frees the failed connection.
    /// @param response ConnectionBind error response.
    virtual void handleConnectionBindErrorResponse(const stun::Message& response);

    /// Processes a ConnectionAttempt indication from the server; optionally
    /// accepts the incoming peer connection by calling createAndBindConnection().
    /// @param response ConnectionAttempt indication message.
    virtual void handleConnectionAttemptIndication(const stun::Message& response);

    /// Opens a new TCP relay socket to the TURN server and sends a
    /// ConnectionBind request carrying @p connectionID.
    /// @param connectionID TURN CONNECTION-ID from the server.
    /// @param peerAddress  Peer associated with this connection.
    /// @return true on success.
    virtual bool createAndBindConnection(uint32_t connectionID, const net::Address& peerAddress);

    /// Relay socket connect callback; sends the ConnectionBind request.
    /// @param socket The relay socket that just connected.
    virtual bool onRelayConnectionConnect(net::Socket& socket);

    /// Relay socket receive callback; forwards data to the observer.
    /// @param socket      The receiving relay socket.
    /// @param buffer      Received data.
    /// @param peerAddress Source address (populated by the socket layer).
    virtual bool onRelayDataReceived(net::Socket& socket, const MutableBuffer& buffer, const net::Address& peerAddress);

    /// Relay socket error callback; notifies the observer of the error.
    /// @param socket The affected relay socket.
    /// @param error  Error descriptor.
    virtual bool onRelayConnectionError(net::Socket& socket, const Error& error);

    /// Relay socket close callback; notifies the observer and frees the connection.
    /// @param socket The closed relay socket.
    virtual bool onRelayConnectionClosed(net::Socket& socket);

    /// Removes the relay connection for @p peerAddress, unregisters its callbacks,
    /// and deletes the associated RelayConnectionBinding.
    /// @param peerAddress Peer whose connection to release.
    void freeConnection(const net::Address& peerAddress);

    /// @return kProtocolTCP (6).
    int transportProtocol() override;

    /// @return Reference to the map of active relay connections keyed by peer address.
    ConnectionManager& connections();

    virtual const char* className() const { return "TURNTCPClient"; };

protected:
    TCPClientObserver& _observer;
    ConnectionManager _connections;
};


} // namespace turn
} // namespace icy


/// @\}
