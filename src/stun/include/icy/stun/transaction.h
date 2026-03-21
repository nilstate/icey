///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#pragma once


#include "icy/memory.h"
#include "icy/net/socket.h"
#include "icy/net/transaction.h"
#include "icy/stun/message.h"


namespace icy {
namespace stun {


/// STUN request/response transaction with timeout and retry logic.
/// Extends the generic net::Transaction with STUN-specific transaction ID
/// matching and response class inference (Success, Error, or Indication).
///
/// Lifetime is managed by IntrusivePtr. Create via makeIntrusive or
/// wrap in IntrusivePtr immediately after construction.
class STUN_API Transaction : public net::Transaction<Message>
{
public:
    using Ptr = IntrusivePtr<Transaction>;

    /// Constructs a STUN transaction bound to a specific socket and peer.
    /// @param socket       Socket used to send the request and receive the response.
    /// @param peerAddress  Remote address of the STUN/TURN server.
    /// @param timeout      Response timeout in milliseconds (default 10 s).
    /// @param retries      Number of send retries before declaring failure (default 1).
    Transaction(const net::Socket::Ptr& socket, const net::Address& peerAddress,
                long timeout = 10000, int retries = 1);

    /// Checks that @p message is a valid response for the pending request.
    /// In addition to the base class check, verifies that the transaction IDs match.
    /// @param message Incoming STUN message to evaluate.
    /// @return true if @p message is the expected response.
    bool checkResponse(const Message& message) override;

    /// Called when a valid response is received. Infers the response class
    /// (SuccessResponse, ErrorResponse, or Indication) from the response
    /// attributes and delegates to the base class handler.
    void onResponse() override;

protected:
    // Allow IntrusivePtr to call our destructor
    template <typename U> friend class icy::IntrusivePtr;

    virtual ~Transaction();
};
} // namespace stun
} // namespace icy


/// @\}
