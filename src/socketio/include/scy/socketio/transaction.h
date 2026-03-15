///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{

#pragma once

#include "scy/packettransaction.h"
#include "scy/socketio/packet.h"


namespace scy {
namespace sockio {


class SocketIO_API Client;


struct SocketIO_API Transaction : public PacketTransaction<sockio::Packet>
{
    Transaction(sockio::Client& client, long timeout = 10000);
    Transaction(sockio::Client& client, const sockio::Packet& request, long timeout = 10000);

    bool send() override;
    bool checkResponse(const sockio::Packet& packet) override;

    virtual void onPotentialResponse(sockio::Packet& packet);
    void onResponse() override;

    virtual const char* className() const { return "SocketIO::Transaction"; }

    sockio::Client& client;

protected:
    ~Transaction() override;
};


} // namespace sockio
} // namespace scy


/// @}
