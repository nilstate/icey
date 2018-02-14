#include "sympletests.h"


using namespace std;
using namespace scy;
using namespace scy::test;


int main(int argc, char** argv)
{
    // Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    test::init();

#if USE_SSL
    net::SSLManager::initNoVerifyClient();
#endif


    // =========================================================================
    // Address
    //
    describe("address", []() {
        smpl::Address a1("user|");
        assert(a1.user == "user");
        assert(a1.id == "");
        assert(a1.valid());

        smpl::Address a2("user");
        assert(a2.user == "user");
        assert(a2.id == "");
        assert(a2.valid());

        smpl::Address a3("");
        assert(a3.user == "");
        assert(a3.id == "");
        assert(!a3.valid());

        smpl::Address a4("|567257247245275");
        assert(a4.user == "");
        assert(a4.id == "567257247245275");
        assert(a4.valid());
    });


    // =========================================================================
    // Client
    //
    describe("client", []() {

        // Run the test server
        // If not available the test will fail gracefully with a warning
        // NOTE: The server must allow anonymous authentication for this test
        Process proc;
        if (!openTestServer(proc)) {
            std::cerr << "cannot start test server, skipping..." << std::endl;
            return;
        }

        smpl::Client::Options loptions;
        loptions.host = SERVER_HOST;
        loptions.port = SERVER_PORT;
        loptions.user = "l";
        loptions.name = "Left";
        // loptions.token = "2NuMmyXw2YDuQfyPCKDO2Qtta";

        smpl::Client::Options roptions;
        roptions.host = SERVER_HOST;
        roptions.port = SERVER_PORT;
        roptions.user = "r";
        roptions.name = "Right";
        // roptions.token = "2NuMmyXw2YDuQfyPCKDO2Qtta";

        TestClient lclient(loptions);
        TestClient rclient(roptions);

        lclient.connect();
        rclient.connect();

        while (!lclient.completed() || !rclient.completed()) {

            // LDebug("waiting for test completion")
            uv::runLoop(uv::defaultLoop(), UV_RUN_ONCE);

            // // Connect the rclient when lclient is online
            // if (lclient.client.isOnline() &&
            //     rclient.client.stateEquals(sockio::ClientState::Closed))
            //     rclient.connect();
        }

        lclient.check();
        rclient.check();
    });

    // TODO:
    //  - Obtain authentication token
    //  - Transaction test
    //  - Presence test
    //  - Ack test
    //  - Benchmarks

    test::runAll();

#if USE_SSL
    net::SSLManager::instance().shutdown();
#endif
    Logger::destroy();
    return test::finalize();
}
