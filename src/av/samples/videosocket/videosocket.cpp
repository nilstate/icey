#include "scy/application.h"
#include "scy/av/iencoder.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/http/packetizers.h"
#include "scy/http/server.h"
#include "scy/net/dns.h"
#include "scy/packetstream.h"


using namespace std;
using namespace scy;


#define USE_AVDEVICE_CAPTURE 0

#if USE_AVDEVICE_CAPTURE
#include "scy/av/mediacapture.h"
#define VIDEO_FILE_SOURCE "test.mp4"
av::MediaCapture* gVideoCapture;
#else
#include "scy/av/mediacapture.h"
av::VideoCapture* gVideoCapture;
#endif


namespace scy {


class MPEGResponder : public http::ServerResponder
{
    av::FPSCounter fpsCounter;
    PacketStream* stream;

public:
    MPEGResponder(http::ServerConnection& conn)
        : http::ServerResponder(conn)
    {
        LDebug("Creating");

        auto stream = new PacketStream;

        // We will be sending our own headers
        conn.shouldSendHeader(false);

        // Attach the video capture
        stream->attachSource(gVideoCapture, false);

        // Setup the encoder options
        av::EncoderOptions options;
        options.oformat = av::Format(
            "MJPEG", "mjpeg", av::VideoCodec("MJPEG", "mjpeg", 400, 300, 25, 48000, 128000, "yuvj420p"));
        gVideoCapture->getEncoderFormat(options.iformat);

        // Create and attach the encoder
        av::MultiplexPacketEncoder* encoder =
            new av::MultiplexPacketEncoder(options);
        encoder->initialize();
        stream->attach(encoder, 5, true);

        // Create and attach the HTTP multipart packetizer
        auto packetizer = new http::MultipartAdapter("image/jpeg", false);
        stream->attach(packetizer, 10, true);
        // assert(0 && "fixme");

        // Start the stream
        stream->emitter += packetSlot(this, &MPEGResponder::onVideoEncoded);
        stream->start();
    }

    ~MPEGResponder()
    {
        LDebug("Destroying");
        // stream->destroy();
        delete stream;
    }

    void onPayload(const Buffer& body)
    {
        LDebug("On recv payload: ", body.size());

        // do something with data from peer
    }

    void onClose()
    {
        LDebug("On close");

        stream->emitter -= packetSlot(this, &MPEGResponder::onVideoEncoded);
        LDebug("On close 1");
        stream->stop();
        LDebug("On close 2");
    }

    void onVideoEncoded(void* sender, RawPacket& packet)
    {
        STrace << "Sending packet: " << packet.size() << ": " << fpsCounter.fps
               << endl;

        try {
            connection().send(packet.data(), packet.size());
            fpsCounter.tick();
        } catch (std::exception /*Exception*/& exc) {
            SError << "Error: " << std::string(exc.what()) /*message()*/
                   << endl;
            connection().close();
        }
    }
};


// -------------------------------------------------------------------
//
class StreamingResponderFactory : public http::ServerResponderFactory
{
public:
    http::ServerResponder* createResponder(http::ServerConnection& conn)
    {
        return new MPEGResponder(conn);
    }
};


} // namespace scy


static void onShutdownSignal(void* opaque)
{
    reinterpret_cast<http::Server*>(opaque)->shutdown();
}


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));

#if USE_AVDEVICE_CAPTURE
    gVideoCapture = new av::MediaCapture();
    // gAVVideoCapture->openDevice(0);
    gVideoCapture->openFile(VIDEO_FILE_SOURCE);
    gVideoCapture->start();
#else
    // VideoCapture instances must be
    // instantiated in the main thread.
    gVideoCapture = new av::VideoCapture(0);
#endif

    {
        Application app;
        http::Server server(328, new StreamingResponderFactory);
        server.start();
        app.waitForShutdown(onShutdownSignal, &server);
    }

    delete gVideoCapture;
    Logger::destroy();
    return 0;
}
