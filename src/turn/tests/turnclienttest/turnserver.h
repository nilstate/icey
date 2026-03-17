#ifndef TURN_TurnServer_H
#define TURN_TurnServer_H


#include "scy/turn/server/server.h"
#include <iostream>


namespace scy {


struct TestServer : public turn::ServerObserver
{
    turn::Server server;

    TestServer(const turn::ServerOptions& opts)
        : server(*this, opts)
    {
    }

    virtual ~TestServer()
    {
    }

    void start()
    {
        server.start();
    }

    void stop()
    {
        server.stop();
    }

    turn::AuthenticationState authenticateRequest(turn::Server*, turn::Request& request)
    {
        return turn::AuthenticationState::Authorized;
    }

    void onServerAllocationCreated(turn::Server*, turn::IAllocation* alloc)
    {
        LDebug("Allocation created");
    }

    void onServerAllocationRemoved(turn::Server*, turn::IAllocation* alloc)
    {
        LDebug("Allocation removed");
    }
};


#define RunTestServer() \
    LDebug("Running TURN Server"); \
    turn::ServerOptions so; \
    so.software = "Sourcey STUN/TURN Server [rfc5766]"; \
    so.realm = "sourcey.com"; \
    so.allocationDefaultLifetime = 1 * 60 * 1000; \
    so.allocationMaxLifetime = 15 * 60 * 1000; \
    so.timerInterval = 5 * 1000; \
    so.listenAddr = net::Address("127.0.0.1", 3478); \
    so.externalIP = TURN_SERVER_EXTERNAL_IP; \
    TestServer srv(so); \
    srv.start();


} //  namespace scy


#endif // TURN_TurnServer_H