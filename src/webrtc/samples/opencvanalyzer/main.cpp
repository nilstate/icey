#include "scy/http/server.h"
#include "scy/idler.h"
#include "rtc_base/ssladapter.h"
#include "signaler.h"
#include "websocketserver.h"

using std::endl;
using namespace scy;
using namespace scy::wrtc;


// Test this demo with the code in the `client` directory

const char* params
        = "{ help           | false | print usage         }"
                "{ proto          | network/mobilenetssd.prototxt.txt | model configuration }"
                "{ model          | network/mobilenetssd.caffemodel | model weights }"
                "{ min_confidence | 0.5   | min confidence      }";

const uint16_t HttpPort = 1234;
const net::Address address("0.0.0.0", HttpPort);
ThreadSafeQueue<nlohmann::json>* detectionQueue = new ThreadSafeQueue<nlohmann::json>();

int main(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv, params);
    if (parser.get<bool>("help"))
    {
        std::cout << "This sample uses Single-Shot Detector " << endl;
        parser.printMessage();
        return 0;
    }

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
#ifdef SCY_UNIX
        int ncores = std::thread::hardware_concurrency();
        setenv("UV_THREADPOOL_SIZE", std::to_string(ncores).c_str(), 1);
#endif

        std::string modelConfiguration = parser.get<std::string>("proto");
        std::string modelBinary = parser.get<std::string>("model");
        float confidenceThreshold = parser.get<float>("min_confidence");

        smpl::Client::Options options;
        options.host = SERVER_HOST;
        options.port = SERVER_PORT;
        options.name = "Video Recorder";
        options.user = "videorecorder";

        // NOTE: The server must enable anonymous authentication for this demo.
        // options.token = ""; token based authentication
        Signaler app(options, &modelBinary, &modelConfiguration, confidenceThreshold, detectionQueue);

        // start detection wesocket server
        WebSocketServer websocketServer(address, detectionQueue);
        websocketServer.start();


        // Process WebRTC threads on the main loop.
        auto rtcthread = rtc::Thread::Current();
        Idler rtc([=]() {
            rtcthread->ProcessMessages(1);
        });

        app.waitForShutdown();
        waitForShutdown();
    }

#if USE_SSL
    net::SSLManager::destroy();
#endif
    rtc::CleanupSSL();
    Logger::destroy();
    return 0;
}
