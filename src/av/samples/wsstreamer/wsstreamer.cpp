///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// WebSocket Media Streamer
//
// Demonstrates streaming encoded media over WebSocket binary frames.
// Each connected WebSocket client gets an independent encoding pipeline:
// shared capture source -> MJPEG encoder -> WebSocket binary frames.
//
// Set USE_AVDEVICE_CAPTURE to 1 to stream from a camera, or leave at 0
// to stream from the bundled test file (no hardware required).
//
// Test with a WebSocket client:
//   websocat ws://localhost:329
//
// Or from JavaScript:
//   const ws = new WebSocket('ws://localhost:329');
//   ws.binaryType = 'arraybuffer';
//   ws.onmessage = (e) => {
//     const blob = new Blob([e.data], { type: 'image/jpeg' });
//     document.getElementById('img').src = URL.createObjectURL(blob);
//   };
//
/// @addtogroup av
/// @{


#include "scy/application.h"
#include "scy/av/mediacapture.h"
#include "scy/av/multiplexpacketencoder.h"
#include "scy/http/server.h"
#include "scy/http/websocket.h"
#include "scy/logger.h"
#include "scy/packetstream.h"

#include <iostream>
#include <memory>


using namespace scy;

#define USE_AVDEVICE_CAPTURE 0

#if USE_AVDEVICE_CAPTURE
#include "scy/av/videocapture.h"
#endif


/// Shared video capture source.
static std::shared_ptr<av::MediaCapture> gCapture;


/// Each WebSocket client gets its own responder with an independent
/// encoding pipeline wired to the shared capture source.
class WSStreamResponder : public http::ServerResponder
{
public:
    WSStreamResponder(http::ServerConnection& conn)
        : http::ServerResponder(conn)
        , _stream("wsstream")
    {
        // Attach the shared capture source
        _stream.attachSource(gCapture.get(), false, true);

        // Configure MJPEG output
        av::EncoderOptions options;
        options.oformat = av::Format(
            "MJPEG", "mjpeg",
            av::VideoCodec("mjpeg", 400, 300, 25));
        gCapture->getEncoderFormat(options.iformat);

        auto encoder = new av::MultiplexPacketEncoder(options);
        encoder->init();
        _stream.attach(encoder, 5, true);

        // Send encoded frames as WebSocket binary frames
        _stream.emitter += packetSlot(this, &WSStreamResponder::onPacket);
        _stream.start();

        std::cout << "WebSocket client connected" << std::endl;
    }

    ~WSStreamResponder()
    {
        _stream.emitter -= packetSlot(this, &WSStreamResponder::onPacket);
        _stream.stop();
        std::cout << "WebSocket client disconnected" << std::endl;
    }

    void onClose() override
    {
        _stream.stop();
    }

    void onPacket(RawPacket& packet)
    {
        try {
            connection().send(packet.data(), packet.size(), http::ws::Binary);
        }
        catch (std::exception& exc) {
            std::cerr << "Send error: " << exc.what() << std::endl;
            connection().close();
        }
    }

private:
    PacketStream _stream;
};


/// Factory that creates a WSStreamResponder for each WebSocket connection.
class WSConnectionFactory : public http::ServerConnectionFactory
{
public:
    std::unique_ptr<http::ServerResponder> createResponder(http::ServerConnection& conn) override
    {
        return std::make_unique<WSStreamResponder>(conn);
    }
};


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

#if USE_AVDEVICE_CAPTURE
    auto capture = std::make_shared<av::VideoCapture>();
    capture->openVideo("0", 640, 480);
    gCapture = capture;
#else
    gCapture = std::make_shared<av::MediaCapture>();
    gCapture->openFile("../data/test.mp4");
    gCapture->setLoopInput(true);
    gCapture->setLimitFramerate(true);
#endif

    gCapture->start();

    http::Server server("0.0.0.0", 329,
                        net::makeSocket<net::TCPSocket>(),
                        std::make_unique<WSConnectionFactory>());
    server.start();

    std::cout << "WebSocket streamer listening on ws://localhost:329" << std::endl;

    waitForShutdown([](void* opaque) {
        reinterpret_cast<http::Server*>(opaque)->shutdown();
    }, &server);

    gCapture->stop();
    gCapture.reset();

    Logger::destroy();
    return 0;
}
