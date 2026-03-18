#pragma once
#include "scy/logger.h"
#include "scy/time.h"
#include "scy/turn/client/udpclient.h"
#include "turnclienttest.h"

#include <iostream>


using namespace std;


namespace scy {


class UDPInitiator : public turn::ClientObserver
{
public:
    int id;
    int nFramesSent;
    int nFramesWanted;
    turn::UDPClient client;
    Timer timer;
    bool success;
    NullSignal AllocationCreated;
    Signal<void(bool)> TestComplete; // unused
    net::Address responderAddress;   ///< The responder local socket address for Send indications

    UDPInitiator(int id, const turn::Client::Options& opts)
        : id(id)
        , nFramesSent(0)
        , nFramesWanted(NUM_ECHO_PACKETS)
        , client(*this, opts)
        , timer(1000, 1000)
        , success(false)
    {
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
            LError(id, ": ", exc.what());
        }
    }

    void shutdown()
    {
        client.shutdown();
        timer.stop();
    }

    void sendPacketToResponder()
    {
        if (nFramesSent++ == nFramesWanted) {
            success = true;
            TestComplete.emit(success);
            return;
        }

        LDebug(id, ": Senidng to responder: ", responderAddress);

        // Send large packets to test throttling
        // payload.append(65536, 'x');
        assert(responderAddress.valid());
        std::string payload(createLatencyCheck(1024));
        client.sendData(payload.c_str(), payload.length(), responderAddress);
    }

protected:
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

                // Send the intial data packet to responder
                sendPacketToResponder();
                //#ifdef TEST_INITIATOR_TO_RESPONDER
                //                timer.Timeout += slot(this, &UDPInitiator::onSendTimer);
                //                timer.start();
                //#endif
                break;
            case turn::ClientState::Failed:
                break;
        }
    }

    void onRelayDataReceived(turn::Client&, const char* data, size_t size, const net::Address& peerAddr)
    {
        LDebug(id, ": Received response data from ", peerAddr, ": size=", size);

        // Send the intial data packet to responder
        sendPacketToResponder();
    }

    void onAllocationCreated(turn::Client&, const stun::Transaction& transaction)
    {
        LDebug(id, ": Permissions Created");
        // AllocationCreated
    }

    void onAllocationPermissionsCreated(turn::Client&, const turn::PermissionList& permissions)
    {
        LDebug(id, ": Permissions Created");
    }
};


} //  namespace scy


