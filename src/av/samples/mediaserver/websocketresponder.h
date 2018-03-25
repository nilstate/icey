#include "mediaserver.h"
#include "scy/http/websocket.h"


namespace scy {


// ----------------------------------------------------------------------------
//
class WebSocketRequestHandler : public http::ServerResponder
{
public:
    WebSocketRequestHandler(http::ServerConnection& connection, const StreamingOptions& options)
        : http::ServerResponder(connection)
        , options(options)
    {
        LDebug("Create")

        // Create the packet stream
        MediaServer::setupPacketStream(stream, options);

        // Start the stream
        stream.emitter += packetSlot(this, &WebSocketRequestHandler::onVideoEncoded);
        stream.start();
    }

    virtual ~WebSocketRequestHandler()
    {
        LDebug("Destroy")
    }

    void onClose()
    {
        LDebug("On close")

        stream.emitter -= packetSlot(this, &WebSocketRequestHandler::onVideoEncoded);
        stream.stop();
    }

    void onVideoEncoded(RawPacket& packet)
    {
        SDebug << "Sending Packet: "
               << &connection() << ": " << packet.size() << ": "
               << fpsCounter.fps << std::endl;

        try {
            connection().send(packet.data(), packet.size(), http::ws::Binary);
            fpsCounter.tick();
        } catch (std::exception& exc) {
            LError("Error: ", exc.what())
            connection().close();
        }
    }

    PacketStream stream;
    StreamingOptions options;
    av::FPSCounter fpsCounter;
};


} // namespace scy
