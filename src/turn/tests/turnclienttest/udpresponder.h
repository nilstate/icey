#pragma once
#include "icy/logger.h"
#include "icy/net/udpsocket.h"
#include "icy/time.h"
#include "icy/timer.h"
#include "turnclienttest.h"


using namespace std;


namespace icy {


class UDPResponder : public net::SocketAdapter
{

public:
    int id;
    net::Address relayedAddr;
    net::UDPSocket socket;
    Timer timer;

    UDPResponder(int id)
        : id(id)
        , timer(1000, 1000)
    {
        LDebug(id, ": Creating");

        socket.addReceiver(this);

        // socket.bind(net::Address(TURN_AUTHORIZE_PEER_IP, 4020));
        socket.bind(net::Address("0.0.0.0", 0));

        LDebug(id, ": Listening on: ", socket.address());
    }

    virtual ~UDPResponder()
    {
        LDebug(id, ": Destroying");
        socket.removeReceiver(this);
    }

    void connect(const net::Address& relayAddr)
    {
        LDebug(id, ": Starting on: ", relayAddr);

        try {
            this->relayedAddr = relayAddr;

            // socket.bind(net::Address("0.0.0.0", 0));
            // socket.bind(net::Address(TURN_AUTHORIZE_PEER_IP, 4020));
            // socket.bind(net::Address("0.0.0.0", 4020));
            socket.connect(relayAddr);
        } catch (std::exception& exc) {
            LError(id, ": ERROR: ", exc.what());
            assert(false);
        }
    }

    void shutdown()
    {
        timer.stop();
        socket.close();
    }

    void onSendTimer()
    {
        std::string payload(createLatencyCheck(30000));
        socket.send(payload.c_str(), payload.length());
    }

    bool onSocketConnect(net::Socket&)
    {
        //#if TEST_RESPONDER_TO_INITIATOR_LATENCY
        //        timer.Timeout += slot(this, &UDPResponder::onSendTimer);
        //        timer.start();
        //#endif
        return false;
    }

    bool onSocketRecv(net::Socket&, const MutableBuffer& buffer, const net::Address& peerAddr)
    {
        std::string payload(bufferCast<const char*>(buffer), buffer.size());
        LDebug(id, ": On recv: ", peerAddr, ": ", buffer.size());

        // Echo back to client
        socket.send(payload.c_str(), payload.size(), relayedAddr); // peerAddr
        return false;
    }

    bool onSocketError(net::Socket&, const icy::Error& error)
    {
        LDebug(id, ": On error: ", error.message);
        return false;
    }

    bool onSocketClose(net::Socket&)
    {
        LDebug(id, ": On close");
        shutdown();
        return false;
    }
};


} //  namespace icy


