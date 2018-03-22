///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
///


#include "scy/idler.h"
#include "scy/logger.h"
#include "signaler.h"

#include "rtc_base/ssladapter.h"
#include "rtc_base/thread.h"


using std::endl;
using namespace scy;


// Test this demo with the code in the `client` directory


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Debug)); // LTrace

#if USE_SSL
    net::SSLManager::initNoVerifyClient();
#endif

    // Setup WebRTC environment
    rtc::LogMessage::LogToDebug(rtc::LERROR);
    rtc::LogMessage::LogTimestamps();
    rtc::LogMessage::LogThreads();

    rtc::InitializeSSL();

    {
        smpl::Client::Options options;
        options.host = SERVER_HOST;
        options.port = SERVER_PORT;
        options.name = "Video Recorder";
        options.user = "videorecorder";

        // NOTE: The server must enable anonymous authentication for this demo.
        // options.token = ""; token based authentication

        Signaler app(options);

        // Process WebRTC threads on the main loop.
        auto rtcthread = rtc::Thread::Current();
        Idler rtc([=]() {
            rtcthread->ProcessMessages(1);
        });

        app.waitForShutdown();
    }

#if USE_SSL
    net::SSLManager::destroy();
#endif
    rtc::CleanupSSL();
    Logger::destroy();
    return 0;
}
