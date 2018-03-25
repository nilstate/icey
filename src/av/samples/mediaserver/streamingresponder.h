#include "mediaserver.h"


namespace scy {


class StreamingRequestHandler : public http::ServerResponder
{
public:
    StreamingRequestHandler(http::ServerConnection& connection, const StreamingOptions& options)
        : http::ServerResponder(connection)
        , options(options)
    {
        LDebug("Create")
    }

    virtual ~StreamingRequestHandler()
    {
        LDebug("Destroy")
    }

    virtual void onRequest(http::Request& request, http::Response& response)
    {
        SDebug << "Handle request: "
                     //<< "\n\tOutput Format: " << options.oformat.name
                     << "\n\tOutput Encoding: " << options.encoding
                     << "\n\tOutput Packetizer: " << options.framing
                     << std::endl;

        // We will be sending our own headers
        connection().shouldSendHeader(false);

        // Create the packet stream
        MediaServer::setupPacketStream(stream, options, true, true);

        // Start the stream
        stream.emitter += packetSlot(this, &StreamingRequestHandler::onVideoEncoded);
        stream.start();
    }

    virtual void onClose()
    {
        LDebug("On close")
        stream.emitter -= packetSlot(this, &StreamingRequestHandler::onVideoEncoded);
        stream.stop();
    }

    void onVideoEncoded(RawPacket& packet)
    {
        SDebug << "Send packet: "
               // assert(!connection().socket()->closed());
               << packet.size() << ": " << fpsCounter.fps << std::endl;

        try {
            connection().socket()->send((const char*)packet.data(), packet.size());
            fpsCounter.tick();
        } catch (std::exception& exc) {
            LError(exc.what())
            connection().close();
        }
    }

    PacketStream stream;
    StreamingOptions options;
    av::FPSCounter fpsCounter;
};


} // namespace scy
