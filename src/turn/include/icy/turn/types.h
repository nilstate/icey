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


#include "icy/net/address.h"
#include "icy/net/socket.h"
#include "icy/stun/message.h"
#include "icy/turn/turn.h"


namespace icy {
namespace turn {


static constexpr int CLIENT_SOCK_BUF_SIZE = 65536;
static constexpr int SERVER_SOCK_BUF_SIZE = CLIENT_SOCK_BUF_SIZE * 32;

/// TURN protocol numbers
static constexpr uint8_t kProtocolUDP = 17;
static constexpr uint8_t kProtocolTCP = 6;

/// TURN/STUN error codes used in this module
static constexpr int kErrorTryAlternate = 300;
static constexpr int kErrorBadRequest = 400;
static constexpr int kErrorNotAuthorized = 401;
static constexpr int kErrorForbidden = 403;
static constexpr int kErrorUnknownAttribute = 420;
static constexpr int kErrorUnsupportedTransport = 442;
static constexpr int kErrorAllocationMismatch = 437;
static constexpr int kErrorStaleNonce = 438;
static constexpr int kErrorWrongCredentials = 441;
static constexpr int kErrorConnectionAlreadyExists = 446;
static constexpr int kErrorConnectionTimeoutOrFailure = 447;
static constexpr int kErrorAllocationQuotaReached = 486;
static constexpr int kErrorInsufficientCapacity = 508;
static constexpr int kErrorOperationNotSupported = 600;


/// Result returned by ServerObserver::authenticateRequest() to control how
/// the server proceeds with an incoming STUN request.
enum class AuthenticationState
{
    Authenticating = 1, ///< Authentication is in progress; server holds the request.
    Authorized = 2,     ///< Credentials verified; request may proceed.
    QuotaReached = 3,   ///< User quota exceeded; respond with 486.
    NotAuthorized = 4   ///< Credentials rejected; respond with 401.
};


/// A STUN message annotated with the transport type and source/destination addresses
/// needed for server-side routing and response generation.
class TURN_API Request : public stun::Message
{
public:
    net::TransportType transport; ///< Protocol on which the request arrived (UDP or TCP).
    net::Address localAddress;   ///< Server's local address that received the request.
    net::Address remoteAddress;  ///< Client's remote address (used to build the 5-tuple).
    std::string hash;            ///< Pre-computed MessageIntegrity key for signing responses.

    /// Constructs a Request by copying a parsed STUN message and annotating it
    /// with the transport context.
    /// @param message       Parsed STUN message.
    /// @param transport     Transport protocol on which the message arrived.
    /// @param localAddress  Server-side local address.
    /// @param remoteAddress Client-side remote address.
    Request(const stun::Message& message, net::TransportType transport,
            const net::Address& localAddress = net::Address(),
            const net::Address& remoteAddress = net::Address())
        : stun::Message(message)
        , transport(transport)
        , localAddress(localAddress)
        , remoteAddress(remoteAddress)
    {
    }
};


using IPList = std::vector<std::string>;


} // namespace turn
} // namespace icy


/// @}
