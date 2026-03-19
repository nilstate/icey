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


enum class AuthenticationState
{
    Authenticating = 1,
    Authorized = 2,
    QuotaReached = 3,
    NotAuthorized = 4
};


class TURN_API Request : public stun::Message
{
public:
    net::TransportType transport;
    net::Address localAddress;
    net::Address remoteAddress;
    std::string hash; // for MessageIntegrity signing

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


/// @\}
