#include "mediaserver.h"

using namespace std;
using namespace scy;


#define SERVER_PORT 1328


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    //Logger::instance().setWriter(new AsyncLogWriter);
    {
        // Pre-initialize video captures in the main thread
        // MediaFactory::instance().loadVideoCaptures();

        // Start the application and server
        Application app;
        {
            MediaServer server(SERVER_PORT);
            server.start();

            // Wait for Ctrl-C
            app.waitForShutdown();
            //app.waitForShutdown([](void* opaque) {
            //    reinterpret_cast<MediaServer*>(opaque)->shutdown();
            //}, &server);
        }

        // Free all pointers pending garbage collection
        //
        // Do this before shutting down the MediaFactory as
        // capture instances may be pending deletion and we
        // need to dereference the implementation instances
        // so system devices can be properly released.
        // GarbageCollector::destroy();

        // Shutdown the media factory and release devices
        // MediaFactory::instance().unloadVideoCaptures();
        // MediaFactory::shutdown();

        // Shutdown the garbage collector once and for all
        // GarbageCollector::instance().shutdown();

        // Finalize the application to free all memory
        // Note: 2 tiny mem leaks (964 bytes) are from OpenCV
        app.finalize();
    }
    Logger::destroy();
    return 0;
}
