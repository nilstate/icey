#include "udpinitiator.h"
#include "udpresponder.h"
#include "tcpinitiator.h"
#include "tcpresponder.h"
#include "turnserver.h"
#include "scy/logger.h"
#include "scy/util.h"

#include <iostream>


using namespace std;
using namespace scy;
using namespace scy::test;


int main(int argc, char** argv)
{
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    // Logger::instance().setWriter(new AsyncLogWriter);
    test::init();

    turn::Client::Options co;
    co.serverAddr = net::Address(TURN_SERVER_IP, TURN_SERVER_PORT);
    co.username = TURN_SERVER_USERNAME;
    co.password = TURN_SERVER_PASSWORD;
    co.lifetime = 120 * 1000; // 1 minute
    co.timeout = 10 * 1000;
    co.timerInterval = 3 * 1000;


    // =========================================================================
    // TURN TCP Client
    //
    describe("TURN TCP client", [&]() {
#if RAISE_LOCAL_SERVER
        RunTestServer()
#endif
        TCPInitiator initiator(0, co);
        TCPResponder responder(0);

        initiator.AllocationCreated += [&]()
        {
            LDebug("Initiator allocation created")

            // Start the responder when the allocation is created
            responder.connect(initiator.client.relayedAddress());
        };

        initiator.TestComplete += [&](bool success)
        {
            LDebug("Test complete: ", success)
            expect(success);
            initiator.shutdown();
            responder.shutdown();
#if RAISE_LOCAL_SERVER
            srv.stop();
#endif
        };

        // TODO: Use STUN binding request to get IP
        initiator.initiate(TURN_AUTHORIZE_PEER_IP);

        uv::runLoop();
    });


    // =========================================================================
    // TURN UDP Client
    //
    describe("TURN UDP client", [&]() {
#if RAISE_LOCAL_SERVER
        RunTestServer()
#endif
        UDPInitiator initiator(0, co);
        UDPResponder responder(0);

        initiator.AllocationCreated += [&]()
        {
            LDebug("Initiator allocation created")

            // Start the responder when the allocation is created
            responder.connect(initiator.client.relayedAddress());

            // Set the local responder address for UDP send indications
            initiator.responderAddress = net::Address(
                TURN_AUTHORIZE_PEER_IP, responder.socket.address().port());
        };

        initiator.TestComplete += [&](bool success)
        {
            LDebug("Test complete: ", success)
            expect(success);
            initiator.shutdown();
            responder.shutdown();
#if RAISE_LOCAL_SERVER
            srv.stop();
#endif
        };

        // TODO: Use STUN binding request to get IP
        initiator.initiate(TURN_AUTHORIZE_PEER_IP);

        uv::runLoop();
    });


    test::runAll();
    return test::finalize();
}
