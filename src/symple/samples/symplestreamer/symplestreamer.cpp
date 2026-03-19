///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// Symple Media Streamer
//
// Streams camera or file video to the Symple client player via MJPEG
// over WebSocket. Connects to a Symple server for peer discovery and
// call signalling, then serves an MJPEG WebSocket stream that the
// symple-client-player can consume directly.
//
// The signalling flow follows the Symple call protocol:
//   1. Streamer connects to Symple server and announces presence
//   2. Remote peer (player) sends call:init message
//   3. Streamer auto-accepts and starts an MJPEG WebSocket server
//   4. Streamer responds with the stream URL in call:accept
//   5. Player connects to the WebSocket URL and receives JPEG frames
//   6. Either side can send call:hangup to end the stream
//
// Set USE_AVDEVICE_CAPTURE to 1 to stream from a camera, or leave at 0
// to stream from the bundled test file.
//
// Usage:
//   symplestreamer -host <server> -port <port> -token <token> -user <id> -name <name>
//
// Requires a running Symple server (https://github.com/nicedoc/symple-server).
//
/// @addtogroup symple
/// @{


#include "scy/application.h"
#include "scy/av/mediacapture.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/http/server.h"
#include "scy/http/websocket.h"
#include "scy/logger.h"
#include "scy/packetstream.h"
#include "scy/symple/client.h"

#include <iostream>
#include <memory>
#include <string>


using namespace scy;

#define USE_AVDEVICE_CAPTURE 0

#if USE_AVDEVICE_CAPTURE
#include "scy/av/videocapture.h"
#endif

static constexpr uint16_t StreamPort = 329;


/// Shared video capture source.
static std::shared_ptr<av::MediaCapture> gCapture;


/// WebSocket responder that streams MJPEG frames as binary WebSocket
/// messages. Each connected player gets its own encoding pipeline from
/// the shared capture source.
class StreamResponder : public http::ServerResponder
{
public:
    StreamResponder(http::ServerConnection& conn)
        : http::ServerResponder(conn)
        , _stream("symplestream")
    {
        _stream.attachSource(gCapture.get(), false, true);

        av::EncoderOptions options;
        options.oformat = av::Format(
            "MJPEG", "mjpeg",
            av::VideoCodec("mjpeg", 400, 300, 25));
        gCapture->getEncoderFormat(options.iformat);

        auto encoder = new av::MultiplexPacketEncoder(options);
        encoder->init();
        _stream.attach(encoder, 5, true);

        _stream.emitter += packetSlot(this, &StreamResponder::onPacket);
        _stream.start();

        std::cout << "Stream client connected" << '\n';
    }

    ~StreamResponder()
    {
        _stream.emitter -= packetSlot(this, &StreamResponder::onPacket);
        _stream.stop();
        std::cout << "Stream client disconnected" << '\n';
    }

    void onClose() override { _stream.stop(); }

    void onPacket(RawPacket& packet)
    {
        try {
            connection().send(packet.data(), packet.size(), http::ws::Binary);
        }
        catch (std::exception& exc) {
            std::cerr << "Send error: " << exc.what() << '\n';
            connection().close();
        }
    }

private:
    PacketStream _stream;
};


/// Factory for per-connection stream responders.
class StreamConnectionFactory : public http::ServerConnectionFactory
{
public:
    std::unique_ptr<http::ServerResponder> createResponder(http::ServerConnection& conn) override
    {
        return std::make_unique<StreamResponder>(conn);
    }
};


/// Main application: connects to Symple for signalling, serves MJPEG
/// WebSocket streams to players.
class SympleStreamer
{
public:
    smpl::Client client;
    http::Server streamServer;
    bool streaming{false};

    SympleStreamer(const smpl::Client::Options& opts)
        : client(opts)
        , streamServer("0.0.0.0", StreamPort,
                       uv::defaultLoop(),
                       std::make_unique<StreamConnectionFactory>())
    {
    }

    void start()
    {
        // Wire up Symple signals
        client += packetSlot(this, &SympleStreamer::onRecvMessage);
        client.Announce += slot(this, &SympleStreamer::onAnnounce);
        client.StateChange += slot(this, &SympleStreamer::onStateChange);
        client.CreatePresence += slot(this, &SympleStreamer::onCreatePresence);

        client.connect();

        // Start the WebSocket media server
        streamServer.start();
        std::cout << "MJPEG WebSocket server listening on ws://localhost:"
                  << StreamPort << '\n';
    }

    void shutdown()
    {
        streamServer.shutdown();
        client.close();
    }

    /// Handle incoming Symple messages for call signalling.
    void onRecvMessage(smpl::Message& msg)
    {
        // The Symple player uses message subtype for call signalling
        std::string subtype;
        if (msg.find("subtype") != msg.end())
            subtype = msg["subtype"].get<std::string>();

        if (subtype == "call:init") {
            // Remote peer wants to start a stream - auto-accept
            std::cout << "Incoming call from " << msg.from().toString() << '\n';

            smpl::Message accept;
            accept.setType("message");
            accept["subtype"] = "call:accept";
            accept.setTo(msg.from());

            // Tell the player where to connect for the MJPEG stream
            accept.setData("url", "ws://localhost:" + std::to_string(StreamPort));
            accept.setData("engine", "MJPEGWebSocket");

            client.send(accept);
            streaming = true;

            std::cout << "Call accepted, streaming on ws://localhost:"
                      << StreamPort << '\n';
        }
        else if (subtype == "call:hangup") {
            std::cout << "Call ended by " << msg.from().toString() << '\n';
            streaming = false;
        }
    }

    void onAnnounce(const int& status)
    {
        std::cout << "Announce response: " << status << '\n';
        if (status != 200) {
            std::cerr << "Authentication failed (status " << status << ")" << '\n';
        }
    }

    void onStateChange(void*, sockio::ClientState& state, const sockio::ClientState&)
    {
        std::cout << "Client state: " << state.toString() << '\n';
        if (state.id() == sockio::ClientState::Online) {
            std::cout << "Online as " << client.ourID() << '\n';
            client.joinRoom("public");
        }
    }

    void onCreatePresence(smpl::Peer& peer)
    {
        peer["agent"] = "LibSourcey";
        peer["type"] = "streamer";
    }
};


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    // Parse command line options
    smpl::Client::Options opts;
    for (int i = 1; i + 1 < argc; i += 2) {
        std::string key = argv[i];
        std::string val = argv[i + 1];
        if (key == "-host") opts.host = val;
        else if (key == "-port") opts.port = static_cast<uint16_t>(std::stoi(val));
        else if (key == "-token") opts.token = val;
        else if (key == "-user") opts.user = val;
        else if (key == "-name") opts.name = val;
        else std::cerr << "Unknown option: " << key << '\n';
    }

    if (opts.user.empty()) {
        std::cerr << "Usage: " << argv[0]
                  << " -host <server> -port <port> -token <token>"
                  << " -user <id> -name <name>" << '\n';
        return 1;
    }

    // Set up video capture
#if USE_AVDEVICE_CAPTURE
    auto capture = std::make_shared<av::VideoCapture>();
    capture->openVideo("0", 640, 480);
    gCapture = capture;
#else
    gCapture = std::make_shared<av::MediaCapture>();
    gCapture->openFile(SCY_DATA_DIR "/test.mp4");
    gCapture->setLoopInput(true);
    gCapture->setLimitFramerate(true);
#endif
    gCapture->start();

    // Run the streamer
    SympleStreamer app(opts);
    app.start();

    waitForShutdown([](void* opaque) {
        reinterpret_cast<SympleStreamer*>(opaque)->shutdown();
    }, &app);

    gCapture->stop();
    gCapture.reset();

    Logger::destroy();
    return 0;
}
