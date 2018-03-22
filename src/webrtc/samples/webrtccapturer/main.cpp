///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
///


#include "scy/idler.h"
#include "scy/logger.h"
#include "signaler.h"
#include "util.h"

#include "rtc_base/ssladapter.h"
#include "rtc_base/thread.h"


using std::endl;
using namespace scy;


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Trace)); // Level::Trace, Level::Debug

    // Setup WebRTC environment
    rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE); // LS_VERBOSE, LS_INFO, LERROR
    rtc::LogMessage::LogTimestamps();
    rtc::LogMessage::LogThreads();

    rtc::InitializeSSL();

    {
        smpl::Client::Options options;
        options.host = getDefaultServerName();
        options.port = 4500;
        options.name = getPeerName();
        options.user = getUsername();
        options.type = "demo";

        // NOTE: The server uses anonymous authentication for this demo.
        // options.token = ""; token based authentication
        
        Signaler app(options);

        // Process WebRTC threads on the main loop.
        auto rtcthread = rtc::Thread::Current();
        Idler rtc([=]() {
            rtcthread->ProcessMessages(1);
        });

        app.waitForShutdown();
    }

    rtc::CleanupSSL();
    Logger::destroy();

    return 0;
}
