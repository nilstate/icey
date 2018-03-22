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
#include "scy/filesystem.h"
#include "signaler.h"

#include "rtc_base/ssladapter.h"
#include "rtc_base/thread.h"


using std::endl;
using namespace scy;


// Sample data directory helper
std::string sampleDataDir(const std::string& file)
{
    std::string dir;
    fs::addnode(dir, SCY_SOURCE_DIR);
    fs::addnode(dir, "av");
    fs::addnode(dir, "samples");
    fs::addnode(dir, "data");
    if (!file.empty())
        fs::addnode(dir, file);
    return dir;
}


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    // Logger::instance().setWriter(new AsyncLogWriter);

//#if USE_SSL
    // net::SSLManager::initNoVerifyClient();
//#endif

    // Setup WebRTC environment
    rtc::LogMessage::LogToDebug(rtc::LS_INFO); // LS_VERBOSE, LS_INFO, LERROR
    // rtc::LogMessage::LogTimestamps();
    // rtc::LogMessage::LogThreads();

    rtc::InitializeSSL();

    {
        // Video source file
        std::string sourceFile(sampleDataDir("test.mp4"));

        // Symple signalling client options
        smpl::Client::Options options;
        options.host = SERVER_HOST;
        options.port = SERVER_PORT;
        options.name = "Video Server";
        options.user = "videoserver";

        // NOTE: The server must enable anonymous
        // authentication for this test.
        // options.token = ""; token based authentication

        // Parse command line args
        OptionParser optparse(argc, argv, "-");
        for (auto& kv : optparse.args) {
            const std::string& key = kv.first;
            const std::string& value = kv.second;
            LDebug("Setting option: ", key, ": ", value)
            if (key == "file") {
                sourceFile = value;
            }
            else {
                LWarn("Unknown option: ", key, "=", value)
            }
        }

        Signaler app(options);
        app.startStreaming(sourceFile, true);

        // Process WebRTC threads on the main loop.
        auto rtcthread = rtc::Thread::Current();
        Idler rtc([rtcthread]() {
            rtcthread->ProcessMessages(3);
        });

        app.waitForShutdown();
    }

//#if USE_SSL
    // net::SSLManager::destroy();
//#endif
    rtc::CleanupSSL();
    Logger::destroy();
    return 0;
}
