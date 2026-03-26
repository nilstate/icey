///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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


#include "icy/application.h"
#include "icy/av/mediacapture.h"
#include "icy/av/multiplexpacketencoder.h"
#include "icy/http/server.h"
#include "icy/http/websocket.h"
#include "icy/logger.h"
#include "icy/packetstream.h"

#include <iostream>
#include <memory>


using namespace icy;

#define USE_AVDEVICE_CAPTURE 0

#if USE_AVDEVICE_CAPTURE
#include "icy/av/videocapture.h"
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

        std::cout << "WebSocket client connected" << '\n';
    }

    ~WSStreamResponder()
    {
        _stream.emitter -= packetSlot(this, &WSStreamResponder::onPacket);
        _stream.stop();
        std::cout << "WebSocket client disconnected" << '\n';
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
            std::cerr << "Send error: " << exc.what() << '\n';
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
    gCapture->openFile(ICY_DATA_DIR "/test.mp4");
    gCapture->setLoopInput(true);
    gCapture->setLimitFramerate(true);
#endif

    gCapture->start();

    http::Server server("0.0.0.0", 329,
                        uv::defaultLoop(),
                        std::make_unique<WSConnectionFactory>());
    server.start();

    std::cout << "WebSocket streamer listening on ws://localhost:329" << '\n';

    waitForShutdown([](void* opaque) {
        reinterpret_cast<http::Server*>(opaque)->stop();
    }, &server);

    gCapture->stop();
    gCapture.reset();

    Logger::destroy();
    return 0;
}

/// @}
