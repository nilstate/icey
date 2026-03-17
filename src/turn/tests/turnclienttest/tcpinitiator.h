#ifndef TURN_TCPInitiator_TEST_H
#define TURN_TCPInitiator_TEST_H


#include "scy/logger.h"
#include "scy/net/tcpsocket.h"
#include "scy/signal.h"
#include "scy/time.h"
#include "scy/timer.h"
#include "scy/turn/client/tcpclient.h"
#include "scy/util.h"

#include <iostream>


using namespace std;


namespace scy {


struct TCPInitiator : public turn::TCPClientObserver
{
    int id;
    int nFramesSent;
    int nFramesWanted;
    turn::TCPClient client;
    net::Address lastPeerAddr;
    bool success;
    NullSignal AllocationCreated;
    Signal<void(const net::Address&)> ConnectionCreated;
    Signal<void(bool)> TestComplete;


    TCPInitiator(int id, const turn::Client::Options opts)
        : id(id)
        , nFramesSent(0)
        , nFramesWanted(NUM_ECHO_PACKETS)
        , client(*this, opts)
        , success(false)
    {
        LDebug(id, ": Creating");
    }

    virtual ~TCPInitiator()
    {
        LDebug(id, ": Destroying");
    }

    void initiate(const std::string& peerIP)
    {
        LDebug(id, ": Initializing");
        try {
            client.addPermission(peerIP);
            client.addPermission("127.0.0.1");
            client.addPermission("192.168.1.1");
            client.initiate();
        } catch (std::exception& exc) {
            LError(id, ": Error: ", exc.what());
        }
    }

    void shutdown()
    {
        client.shutdown();
    }

    void sendPacketToResponder()
    {
        if (nFramesSent++ == nFramesWanted) {
            success = true;
            TestComplete.emit(success);
            return;
        }

        // Send the unix ticks milisecond for checking latency
        // std::string payload;
        // payload.append(":");
        // payload.append(util::itostr(time::ticks()));

        // Send large packets to test throttling
        // payload.append(65536, 'x');
        // payload.append(10000, 'x');

        LDebug(id, ": Sending packet to responder");
        std::string payload("initiator > responder");
        client.sendData(payload.c_str(), payload.length(), lastPeerAddr);
    }

    void onClientStateChange(turn::Client&, turn::ClientState& state, const turn::ClientState&)
    {
        LDebug(id, ": State change: ", state.toString());

        switch (state.id()) {
            case turn::ClientState::None:
                break;
            case turn::ClientState::Allocating:
                break;
            case turn::ClientState::Authorizing:
                break;
            case turn::ClientState::Success:
                AllocationCreated.emit();
                break;
            case turn::ClientState::Failed:
                // assert(false);
                success = false;
                TestComplete.emit(success);
                break;
        }
    }

    void onRelayConnectionCreated(turn::TCPClient&, const net::TCPSocket::Ptr& socket, const net::Address& peerAddr)
    {
        LDebug(id, ": Connection Created: ", peerAddr);

        // Remember the last peer
        lastPeerAddr = peerAddr;
        ConnectionCreated.emit(peerAddr);

        // Send the intial data packet to responder
        sendPacketToResponder();
    }

    void onRelayConnectionClosed(turn::TCPClient&, const net::TCPSocket::Ptr& socket, const net::Address& peerAddr)
    {
        LDebug(id, ": Connection Closed");
    }

    void onRelayDataReceived(turn::Client&, const char* data, size_t size, const net::Address& peerAddr)
    {
        LDebug(id, ": Data received from responder");

        sendPacketToResponder();
    }

    void onAllocationPermissionsCreated(turn::Client&, const turn::PermissionList& permissions)
    {
        LDebug(id, ": Permissions Created");
    }
};


} //  namespace scy


#endif // TURN_TCPInitiator_TEST_H