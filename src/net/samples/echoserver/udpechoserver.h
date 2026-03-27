#pragma once

#include "icy/net/socketadapter.h"
#include "icy/net/udpsocket.h"


namespace icy {
namespace net {


class UDPEchoServer : public SocketAdapter
{
public:
    UDPSocket::Ptr server;

    UDPEchoServer()
        : server(std::make_shared<UDPSocket>())
    {
        server->addReceiver(this);
    }

    virtual ~UDPEchoServer()
    {
        server->removeReceiver(this);
        shutdown();
    }

    void start(const std::string& host, uint16_t port)
    {
        server->bind(Address(host, port));
    }

    void shutdown()
    {
        server->close();
    }

    bool onSocketRecv(Socket&, const MutableBuffer& buffer, const net::Address& peerAddress) override
    {
        LDebug("On recv: ", peerAddress, ": ", buffer.size());

        // Echo back to client
        if (server->sendOwned(Buffer(bufferCast<const char*>(buffer),
                                     bufferCast<const char*>(buffer) + buffer.size()),
                              peerAddress) < 0) {
            LError("Failed to echo UDP payload");
        }
        return false;
    }

    bool onSocketError(Socket&, const icy::Error& error) override
    {
        LError("On error: ", error.message);
        return false;
    }

    bool onSocketClose(Socket&) override
    {
        LDebug("On close");
        return false;
    }
};


} // namespace net
} // namespace icy
