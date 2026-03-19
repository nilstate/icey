#pragma once
#include "icy/test.h"
#include <iostream>


#define TEST_TCP 1
#define RAISE_LOCAL_SERVER 1
#define NUM_ECHO_PACKETS 100

#define TURN_SERVER_IP "127.0.0.1"
#define TURN_SERVER_PORT 3478
#define TURN_SERVER_USERNAME "username"
#define TURN_SERVER_PASSWORD "password"
#define TURN_SERVER_EXTERNAL_IP "127.0.0.1"
#define TURN_AUTHORIZE_PEER_IP "127.0.0.1" // for CreatePremission


namespace icy {


inline std::string createLatencyCheck(int packetSize = 1024)
{
    std::string payload;
    // payload.append(util::itostr(time::ticks()));
    payload.append(packetSize - payload.length(), 'x');
    return payload;
}


} //  namespace icy


